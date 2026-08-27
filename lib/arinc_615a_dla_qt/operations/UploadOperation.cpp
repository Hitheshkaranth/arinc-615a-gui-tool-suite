// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::UploadOperation.
 **/

#include "UploadOperation.hpp"

#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/Protocol.hpp>
#include <arinc_615a/host/UploadOperation.hpp>

#include <arinc_615a/tftp/servers/ReadOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/UploadStatus.hpp>

#include <arinc_615a/StatusCodeDescription.hpp>

#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/files/StreamFile.hpp>

#include <spdlog/spdlog.h>

#include <QMessageBox>

#include <boost/exception/all.hpp>

#include <format>

namespace Arinc615aDlaQt {

UploadOperation::UploadOperation(
  Arinc615a::Host::ProtocolPtr protocol,
  DataLoaderConfiguration configuration,
  Arinc665::Utils::MediaSetManagerPtr mediaSetManager,
  QObject * const parent ) :
  QObject{ parent },
  protocolV{ std::move( protocol ) },
  configurationV{ std::move( configuration ) },
  mediaSetManagerV{ std::move( mediaSetManager ) }
{
  qRegisterMetaType< std::chrono::seconds >( "std::chrono::seconds" );
  qRegisterMetaType< std::string >( "std::string" );
  qRegisterMetaType< Arinc615a::StatusCode >( "Arinc615a::StatusCode" );
  qRegisterMetaType< Arinc615a::Information::InitializationResponse >(
    "Arinc615a::Information::InitializationResponse" );
  qRegisterMetaType< Arinc615a::Information::UploadStatus >( "Arinc615a::Information::UploadStatus" );
}

void UploadOperation::startOperation(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId )
{
  try
  {
    operationV = protocolV->uploadOperation(
      Arinc615a::Host::UploadOperationConfiguration{
        .dataLoaderConfiguration = configurationV.dataLoaderConfiguration,
        .handler = *this,
        .targetAddress = targetAddress,
        .targetId = targetId,
        .dlpTimeout = configurationV.dlpTimeout,
        .portOption = configurationV.portOption
      } );
    assert( operationV );

    operationV->start();
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Upload Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void UploadOperation::abortOperation()
{
  try
  {
    if ( operationV )
    {
      operationV->abort( Arinc615a::Host::UploadOperation::AbortReason::Operator );
    }
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Upload Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void UploadOperation::transmitLoads( Arinc665::Media::ConstMediaSetPtr mediaSet, Arinc665::Media::ConstLoads loads )
{
  try
  {
    loadsV = std::move( loads );
    checkValuesV = mediaSetManagerV->checkValues( mediaSet );

    Arinc615a::Information::UploadLoads loadsList{};

    for ( const auto &load : loadsV )
    {
      assert( load );
      loadsList.emplace_back( Arinc615a::Information::UploadLoad{
        .headerFilename = std::string{ load->name() },
        .partNumber = std::string{ load->partNumber() } } );
    }

    if ( operationV )
    {
      operationV->loadList( std::move( loadsList ) );
    }
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Upload Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void UploadOperation::initialisationDeferred( const std::chrono::seconds waitTime )
{
  emit operationDeferred( waitTime );
}

void UploadOperation::initialisationResponse( const Arinc615a::Information::InitializationResponse &response )
{
  emit operationInitialised( response );
}

void UploadOperation::finished( const Arinc615a::StatusCode code, std::string_view description )
{
  emit operationFinished( code, std::string{ description } );
}

void UploadOperation::status( const Arinc615a::Information::UploadStatus &status )
{
  emit receivedStatus( status );
}

void UploadOperation::fileRequest(
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::string_view loadPartNumber,
  const Arinc649::CheckValue &checkValue )
{
  SPDLOG_INFO( "Request file '{}' Load Part Number '{}' Check Value '{}'", filename, loadPartNumber, checkValue.format() );

  const auto file{ Arinc665::Media::Loads_file( loadsV, checkValuesV, filename, loadPartNumber, checkValue ) };

  if ( !file )
  {
    SPDLOG_ERROR( "File '{}' was not found.", filename );
    operationV->errorOperation(
      remote,
      ::Tftp::Packets::ErrorCode::FileNotFound,
      std::format( "File '{}' was not found.", filename ) );
    return;
  }

  auto realFilename{ mediaSetManagerV->filePath( file ) };

  SPDLOG_INFO( "Local file '{}'", realFilename.string() );

  if ( !std::filesystem::is_regular_file( realFilename ) )
  {
    SPDLOG_ERROR( "File '{}' was not locally found.", realFilename.string() );
    operationV->errorOperation(
      remote,
      Tftp::Packets::ErrorCode::FileNotFound,
      std::format( "File '{}' was not found.", filename ) );
    return;
  }

  const auto streamFile{ std::make_shared< Tftp::Files::StreamFile >(
    Tftp::Files::StreamFile::Operation::Transmit,
    realFilename,
    std::filesystem::file_size( realFilename ) ) };
  assert( streamFile );

  auto fileOperation{
    operationV->fileTransfer( streamFile, remote, clientTftpOptions, std::string{ loadPartNumber }, checkValue ) };
  assert( fileOperation );

  fileOperation
    ->completionHandler( std::bind_front( &UploadOperation::fileCompleted, this, fileOperation ) );

  fileOperationsV.emplace_front( std::move( fileOperation ) )->start();
}

void UploadOperation::fileCompleted(
  const Arinc615a::Tftp::Servers::ReadOperationPtr &operation,
  const Tftp::TransferStatus status )
{
  if ( 1U != fileOperationsV.remove( operation ) )
  {
    SPDLOG_ERROR( "File operation completed, which was not created" );
    operationV->abort( Arinc615a::Host::Operation::AbortReason::Protocol );
  }

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_WARN( "File transfer failed - Ignore it from the host side" );
    return;
  }
}

}
