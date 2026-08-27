// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::InformationOperation.
 **/

#include "InformationOperation.hpp"

#include <arinc_615a/host/InformationOperation.hpp>
#include <arinc_615a/host/OperationConfiguration.hpp>
#include <arinc_615a/host/Protocol.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/PartNumber.hpp>
#include <arinc_615a/information/Status.hpp>
#include <arinc_615a/information/TargetHardware.hpp>

#include <QMessageBox>

#include <boost/exception/all.hpp>

#include <utility>

namespace Arinc615aDlaQt {

InformationOperation::InformationOperation(
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
  qRegisterMetaType< Arinc615a::Information::InformationStatus >( "Arinc615a::Information::InformationStatus" );
  qRegisterMetaType< Arinc615a::Information::TargetsHardware >( "Arinc615a::Information::TargetsHardware" );
}

void InformationOperation::startOperation(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId )
{
  try
  {
    operationV = protocolV->informationOperation(
      Arinc615a::Host::InformationOperationConfiguration{
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
      tr( "Error during Information Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg(
        QString::fromStdString( info ) ) );

    emit operationFinished(
      Arinc615a::StatusCode::OperationAbortedByDlp,
      "Error During Operation" );
  }
}

void InformationOperation::abortOperation()
{
  try
  {
    if ( operationV )
    {
      operationV->abort(
        Arinc615a::Host::InformationOperation::AbortReason::Operator );
    }
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during Information Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg(
        QString::fromStdString( info ) ) );

    emit operationFinished(
      Arinc615a::StatusCode::OperationAbortedByDlp,
      "Error During Operation" );
  }
}

void InformationOperation::initialisationDeferred(
  const std::chrono::seconds waitTime )
{
  emit operationDeferred( waitTime );
}

void InformationOperation::initialisationResponse(
  const Arinc615a::Information::InitializationResponse &response )
{
  emit operationInitialised( response );
}

void InformationOperation::finished( const Arinc615a::StatusCode code, const std::string_view description )
{
  emit operationFinished( code, std::string{ description } );
}

void InformationOperation::status( const Arinc615a::Information::InformationStatus &status )
{
  emit receivedStatus( status );
}

void InformationOperation::targetInformation(
  const Arinc615a::Information::TargetsHardware &targetsHardware,
  const bool integrity )
{
  emit receivedInformation( targetsHardware, integrity );
}

}
