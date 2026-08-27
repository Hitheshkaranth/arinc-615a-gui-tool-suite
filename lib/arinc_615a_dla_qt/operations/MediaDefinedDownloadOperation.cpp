// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::MediaDefinedDownloadOperation.
 **/

#include "MediaDefinedDownloadOperation.hpp"

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/DownloadStatus.hpp>

#include <arinc_615a/host/Protocol.hpp>
#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/MediaDefinedDownloadOperation.hpp>

#include <arinc_615a/tftp/servers/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>
#include <arinc_615a/TargetId.hpp>

#include <arinc_649/CheckValue.hpp>
#include <arinc_649/CheckValueGenerator.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/files/StreamFile.hpp>

#include <helper/Filenames.hpp>

#include <spdlog/spdlog.h>

#include <QMessageBox>

#include <boost/exception/all.hpp>

#include <chrono>
#include <format>

namespace Arinc615aDlaQt {

MediaDefinedDownloadOperation::MediaDefinedDownloadOperation(
  Arinc615a::Host::ProtocolPtr protocol,
  DataLoaderConfiguration configuration,
  QObject * const parent ) :
  QObject{ parent },
  protocolV{ std::move( protocol ) },
  configurationV{ std::move( configuration ) }
{
  qRegisterMetaType< std::chrono::seconds >( "std::chrono::seconds" );
  qRegisterMetaType< std::string >( "std::string" );
  qRegisterMetaType< Arinc615a::StatusCode >( "Arinc615a::StatusCode" );
  qRegisterMetaType< Arinc615a::Information::InitializationResponse >(
    "Arinc615a::Information::InitializationResponse" );
  qRegisterMetaType< Arinc615a::Information::DownloadStatus >( "Arinc615a::Information::DownloadStatus" );
}

void MediaDefinedDownloadOperation::startOperation(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId,
  const std::filesystem::path &downloadBaseDirectory,
  const bool createDownloadDirectory )
{
  try
  {
    downloadInformationV.start( targetId, targetAddress );

    downloadDataPathV =
      createDownloadDirectory ? downloadInformationV.downloadDataPath( downloadBaseDirectory ) : downloadBaseDirectory;
    downloadInformationPathV = downloadInformationV.downloadInformationPath( downloadBaseDirectory );

    std::error_code errorCode;
    std::filesystem::create_directories( downloadDataPathV, errorCode );
    if ( errorCode )
    {
      QMessageBox::critical(
        nullptr,
        tr( "Media Defined Download Operation" ),
        QString{ tr( "Error creating Download Data Directory: <i>%1</i><br><tt>%2</tt>" ) }.arg(
          QString::fromStdString( downloadDataPathV.string() ),
          QString::fromStdString( errorCode.message() ) ) );

      emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error creating download directory" );
    }

    operationV = protocolV->mediaDefinedDownloadOperation(
      Arinc615a::Host::MediaDefinedDownloadOperationConfiguration{
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
      tr( "Error during Media Defined Download Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void MediaDefinedDownloadOperation::abortOperation()
{
  try
  {
    if ( operationV )
    {
      operationV->abort( Arinc615a::Host::MediaDefinedDownloadOperation::AbortReason::Operator );
    }
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Media Defined Download Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void MediaDefinedDownloadOperation::transmitFiles(
  Arinc615a::Information::DownloadFiles files,
  Helper::RawData userDefinedData )
{
  try
  {
    if ( operationV )
    {
      downloadInformationV.files( files );

      operationV->request( std::move( files ), std::move( userDefinedData ) );
    }
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Media Defined Download Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void MediaDefinedDownloadOperation::initialisationDeferred( const std::chrono::seconds waitTime )
{
  emit operationDeferred( waitTime );
}

void MediaDefinedDownloadOperation::initialisationResponse(
  const Arinc615a::Information::InitializationResponse &response )
{
  emit operationInitialised( response );
}

void MediaDefinedDownloadOperation::finished( const Arinc615a::StatusCode code, std::string_view description )
{
  try
  {
    downloadInformationV.finished( Arinc615a::status( Arinc615a::OperationClass::Download, code, description ) );

    downloadInformationV.save( downloadInformationPathV, configurationV.verifyDownloadFiles );

    emit operationFinished( code, std::string{ description } );
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Media Defined Download Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );

    emit operationFinished( Arinc615a::StatusCode::OperationAbortedByDlp, "Error During Operation" );
  }
}

void MediaDefinedDownloadOperation::status( const Arinc615a::Information::DownloadStatus &status )
{
  emit receivedStatus( status );
}

void MediaDefinedDownloadOperation::fileRequest(
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::string_view partNumber,
  const Arinc649::CheckValue &checkValue )
{
  const std::filesystem::path filePath{ downloadDataPathV / Helper::normaliseFilename( filename ) };

  SPDLOG_INFO(
    "Request file '{}' Part Number '{}' Check Value '{}' to {}",
    filename,
    partNumber,
    checkValue.format(),
    filePath.string() );

  downloadInformationV.fileStart( filename, filePath, std::string{ partNumber }, checkValue );

  const auto streamFile{
    std::make_shared< Tftp::Files::StreamFile >( Tftp::Files::StreamFile::Operation::Receive, filePath ) };
  assert( streamFile );

  auto fileOperation{
    operationV->fileTransfer( streamFile, remote, clientTftpOptions, std::string{ partNumber }, checkValue ) };
  assert( fileOperation );

  fileOperation->completionHandler(
    std::bind_front(
      &MediaDefinedDownloadOperation::fileCompleted,
      this,
      std::string{ filename },
      filePath,
      clientTftpOptions.transferSize,
      fileOperation ) );

  fileOperationsV.emplace_front( std::move( fileOperation ) )->start();
}

void MediaDefinedDownloadOperation::fileCompleted(
  std::string_view filename,
  const std::filesystem::path &filePath,
  const std::optional< const std::size_t > &expectedFileSize,
  const Arinc615a::Tftp::Servers::WriteOperationPtr &operation,
  const Tftp::TransferStatus status )
{
  if ( 1U != fileOperationsV.remove( operation ) )
  {
    SPDLOG_ERROR( "File operation completed, which was not created" );
    operationV->abort( Arinc615a::Host::Operation::AbortReason::Protocol );
  }

  if ( Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_WARN( "File transfer failed - Ignore it from host side" );

    downloadInformationV.fileFinished( filename, Arinc615a::Host::DownloadInformation::TransferError, 0U );

    return;
  }

  const auto fileSize{ std::filesystem::file_size( filePath ) };

  if ( expectedFileSize && ( fileSize != expectedFileSize.value() ) )
  {
    SPDLOG_WARN( "File size differs to expected one" );
  }

  downloadInformationV.fileFinished( filename, Arinc615a::Host::DownloadInformation::TransferOk, fileSize );
}

}
