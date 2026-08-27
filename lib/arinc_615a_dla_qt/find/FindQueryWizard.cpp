// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::FindQueryWizard.
 **/

#include "FindQueryWizard.hpp"

#include "ui_FindQueryWizard.h"

#include <arinc_615a_dla_qt/operations/FindQuery.hpp>

#include <arinc_615a_qt/TargetsInformationModel.hpp>

namespace Arinc615aDlaQt {

FindQueryWizard::FindQueryWizard(
  Arinc615a::Find::Clients::ClientPtr findClient,
  DataLoaderConfiguration configuration,
  Arinc615a::Find::TargetsAddressInformation &targets,
  QWidget * const parent ) :
  QWizard{ parent },
  uiV{ std::make_unique< Ui::FindQueryWizard >() },
  targetsV{ targets },
  targetsInformationModelV{ std::make_unique< Arinc615aQt::TargetsInformationModel >( this ) },
  queryV{ std::make_unique< FindQuery >( std::move( findClient ), std::move( configuration ), this ) }
{
  assert( queryV );

  uiV->setupUi( this );

  uiV->selectAddress->setCommitPage( true );

  uiV->results->targetsInformationModel( targetsInformationModelV.get() );

  connect(
    uiV->selectAddress,
    &FindQuerySelectAddressPage::targetAddressChanged,
    this,
    &FindQueryWizard::targetAddressSelected );

  connect(
    uiV->results,
    &FindQueryResultsPage::startQuery,
    this,
    &FindQueryWizard::startQuery );
  connect(
    uiV->results,
    &FindQueryResultsPage::abortQuery,
    queryV.get(),
    &FindQuery::abortQuery );
  connect(
    this,
    &FindQueryWizard::accepted,
    this,
    &FindQueryWizard::saveResult );

  setButtonText( CustomButton1, tr( "Abort Operation" ) );

  // set Logo
  QIcon icon;
  icon.addFile( QString::fromUtf8( ":/arinc615a_find_query.svg" ), QSize{}, QIcon::Normal, QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap( QWizard::WizardPixmap::LogoPixmap, icon.pixmap( 64 ) );
  }

  connect(
    queryV.get(),
    &FindQuery::receivedFindResponse,
    this,
    &FindQueryWizard::receivedFindResponse,
    Qt::QueuedConnection );
  connect(
    queryV.get(),
    &FindQuery::queryFinished,
    this,
    &FindQueryWizard::operationFinished,
    Qt::QueuedConnection );
}

FindQueryWizard::~FindQueryWizard() = default;

void FindQueryWizard::targetAddressSelected( boost::asio::ip::address address )
{
  queryAddressV = std::move( address );
}

void FindQueryWizard::startQuery()
{
  assert( queryV );

  queryV->startQuery( queryAddressV );
}

void FindQueryWizard::saveResult()
{
  assert( targetsInformationModelV );

  if ( field( "AddToTargetList" ).toBool() )
  {
    targetsV.insert(
      targetsV.end(),
      targetsInformationModelV->targetsInformation().begin(),
      targetsInformationModelV->targetsInformation().end() );
  }
}

void FindQueryWizard::receivedFindResponse(
  boost::asio::ip::address address,
  Arinc615a::Find::TargetInformation information )
{
  assert( targetsInformationModelV );

  targetsInformationModelV->targetInformation(
    Arinc615a::Find::TargetAddressInformation{ std::move( address ), std::move( information ) } );
}

void FindQueryWizard::operationFinished()
{
  uiV->results->queryFinished();
}

}
