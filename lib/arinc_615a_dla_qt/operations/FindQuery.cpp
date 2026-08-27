/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::FindQuery.
 **/

#include "FindQuery.hpp"

#include <arinc_615a/find/clients/Client.hpp>
#include <arinc_615a/find/clients/Query.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <QMessageBox>

#include <boost/exception/all.hpp>

namespace Arinc615aDlaQt {

FindQuery::FindQuery(
  Arinc615a::Find::Clients::ClientPtr findClient,
  DataLoaderConfiguration configuration,
  QObject * const parent ) :
  QObject{ parent },
  findClientV{ std::move( findClient ) },
  configurationV{ std::move( configuration ) }
{
  qRegisterMetaType< boost::asio::ip::address >( "boost::asio::ip::address" );
  qRegisterMetaType< Arinc615a::Find::TargetInformation >( "Arinc615a::Find::TargetInformation" );
}

void FindQuery::startQuery( const boost::asio::ip::address &targetAddress )
{
  try
  {
    queryV = findClientV->query();
    assert( queryV );

    // configure
    queryV
      ->responseHandler( std::bind_front( &FindQuery::findResponse, this ) )
      .completionHandler( std::bind_front( &FindQuery::finished, this ) )
      .localAddress( configurationV.findConfiguration.localInterfaceAddress )
      .remoteAddress( targetAddress )
      .port( configurationV.findConfiguration.findPort )
      .dynamicLocalPort( configurationV.dynamicFindPort )
      .timeout(  configurationV.findTimeout );

    queryV->start();
  }
  catch ( const boost::exception &e )
  {
    QMessageBox::critical(
      nullptr,
      tr( "Error during FIND Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg(
        QString::fromStdString( boost::diagnostic_information( e ) ) ) );

    emit queryFinished();
  }
}

void FindQuery::abortQuery()
{
  try
  {
    if ( queryV )
    {
      queryV->abort();
    }
  }
  catch ( const boost::exception &e )
  {
    QMessageBox::critical(
      nullptr,
      tr( "Error during FIND Operation" ),
      QString{ tr( "<b>Error Description:</b><br/><tt>%1</tt>" ) }.arg(
        QString::fromStdString( boost::diagnostic_information( e ) ) ) );

    emit queryFinished();
  }
}

void FindQuery::findResponse(
  const boost::asio::ip::address &address,
  const Arinc615a::Find::TargetInformation &targetInformation )
{
  emit receivedFindResponse( address, targetInformation );
}

void FindQuery::finished()
{
  emit queryFinished();
}

}
