// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::InformationOperationWizard.
 **/

#include "InformationOperationWizard.hpp"

#include "ui_InformationOperationWizard.h"

#include <arinc_615a_dla_qt/operations/InformationOperation.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

#include <QIcon>

#include <format>

namespace Arinc615aDlaQt {

InformationOperationWizard::InformationOperationWizard(
  const Arinc615a::Host::ProtocolPtr &protocol,
  const DataLoaderConfiguration &configuration,
  const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
  QWidget * const parent ) :
  QWizard{ parent },
  ui{ std::make_unique< Ui::InformationOperationWizard >() },
  operationV{ std::make_unique< InformationOperation >(
    protocol,
    configuration,
    this ) }
{
  assert( operationV );

  ui->setupUi( this );

  ui->settings->targets( targetsInformation );

  setButtonText( CustomButton1, tr( "Abort Operation" ) );

  // set Logo
  QIcon icon{};
  icon.addFile(
    QString::fromUtf8( ":/arinc615a_information_operation.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap(
      QWizard::WizardPixmap::LogoPixmap,
      icon.pixmap( 64 ) );
  }

  connect(
    ui->settings,
    &InformationOperationSettingsPage::targetSelected,
    this,
    &InformationOperationWizard::targetSelected );

  connect(
    ui->status,
    &InformationOperationStatusPage::startOperation,
    this,
    &InformationOperationWizard::startOperation );
  connect(
    ui->status,
    &InformationOperationStatusPage::abortOperation,
    operationV.get(),
    &InformationOperation::abortOperation );

  connect(
    operationV.get(),
    &InformationOperation::operationDeferred,
    this,
    &InformationOperationWizard::operationDeferred,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &InformationOperation::operationInitialised,
    this,
    &InformationOperationWizard::operationInitialised,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &InformationOperation::operationFinished,
    this,
    &InformationOperationWizard::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &InformationOperation::operationFinished,
    ui->status,
    &InformationOperationStatusPage::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &InformationOperation::operationFinished,
    ui->completed,
    &InformationOperationCompletedPage::operationFinished,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &InformationOperation::receivedStatus,
    ui->status,
    &InformationOperationStatusPage::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &InformationOperation::receivedStatus,
    ui->completed,
    &InformationOperationCompletedPage::operationStatus,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &InformationOperation::receivedInformation,
    ui->completed,
    &InformationOperationCompletedPage::information,
    Qt::QueuedConnection );
}

InformationOperationWizard::~InformationOperationWizard() = default;

void InformationOperationWizard::targetSelected(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId )
{
  targetAddressV = targetAddress;
  targetIdV = targetId;
  ui->completed->targetId( QString::fromStdString( targetId.toString() ) );
}

void InformationOperationWizard::startOperation()
{
  assert( operationV );
  operationV->startOperation( targetAddressV, targetIdV );
}

void InformationOperationWizard::operationDeferred( const std::chrono::seconds waitTime )
{
  ui->status->operationInitialised(
    Arinc615a::Information::InitializationResponse{
      Arinc615a::OperationAcceptanceStatusCode::OperationDeferred,
      std::format( "Deferred for {} seconds.", waitTime.count() ) } );
}

void InformationOperationWizard::operationInitialised( const Arinc615a::Information::InitializationResponse &response )
{
  ui->status->operationInitialised( response );
}

void InformationOperationWizard::operationFinished()
{
  next();
}

}
