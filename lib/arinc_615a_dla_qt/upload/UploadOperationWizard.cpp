// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::UploadOperationWizard.
 **/

#include "UploadOperationWizard.hpp"

#include "ui_UploadOperationWizard.h"

#include <arinc_615a_dla_qt/operations/UploadOperation.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/UploadStatus.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>

#include <arinc_665/media/Load.hpp>

#include <QIcon>

#include <format>
#include <ranges>

namespace Arinc615aDlaQt {

UploadOperationWizard::UploadOperationWizard(
  const Arinc615a::Host::ProtocolPtr &protocol,
  const DataLoaderConfiguration &configuration,
  const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
  const Arinc665::Utils::MediaSetManagerPtr &mediaSetManager,
  QWidget * const parent ) :
  QWizard{ parent },
  ui{ std::make_unique< Ui::UploadOperationWizard >() },
  operationV{ std::make_unique< UploadOperation >( protocol, configuration, mediaSetManager, this ) }
{
  assert( operationV );

  ui->setupUi( this );

  ui->settings->targets( targetsInformation );
  Arinc665::Media::ConstMediaSets mediaSets{};
  for ( auto [ mediaSet, checkValues ] : mediaSetManager->mediaSets() | std::views::values )
  {
    mediaSets.emplace_back( std::move(  mediaSet ) );
  }
  ui->settings->mediaSets( std::move( mediaSets ) );

  setButtonText( CustomButton1, tr( "Abort Operation" ) );

  // set Logo
  QIcon icon{};
  icon.addFile( QString::fromUtf8( ":/arinc615a_upload_operation.svg" ), QSize{}, QIcon::Normal, QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap( QWizard::WizardPixmap::LogoPixmap, icon.pixmap( 64 ) );
  }

  connect(
    ui->settings,
    &UploadOperationSettingsPage::targetSelected,
    this,
    &UploadOperationWizard::targetSelected );
  connect(
    ui->settings,
    &UploadOperationSettingsPage::mediaSetSelected,
    this,
    &UploadOperationWizard::mediaSetSelected );
  connect(
    ui->settings,
    &UploadOperationSettingsPage::loadsSelected,
    this,
    &UploadOperationWizard::loadsSelected );

  connect(
    ui->status,
    &UploadOperationStatusPage::startOperation,
    this,
    &UploadOperationWizard::startOperation );
  connect(
    ui->status,
    &UploadOperationStatusPage::abortOperation,
    operationV.get(),
    &UploadOperation::abortOperation );

  connect(
    operationV.get(),
    &UploadOperation::operationDeferred,
    this,
    &UploadOperationWizard::operationDeferred,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &UploadOperation::operationInitialised,
    ui->status,
    &UploadOperationStatusPage::operationInitialised,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &UploadOperation::operationFinished,
    this,
    &UploadOperationWizard::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &UploadOperation::operationFinished,
    ui->status,
    &UploadOperationStatusPage::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &UploadOperation::operationFinished,
    ui->completed,
    &UploadOperationCompletedPage::operationFinished,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &UploadOperation::receivedStatus,
    this,
    &UploadOperationWizard::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &UploadOperation::receivedStatus,
    ui->status,
    &UploadOperationStatusPage::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &UploadOperation::receivedStatus,
    ui->completed,
    &UploadOperationCompletedPage::receivedStatus,
    Qt::QueuedConnection );
}

UploadOperationWizard::~UploadOperationWizard() = default;

void UploadOperationWizard::targetSelected(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId )
{
  targetAddressV = targetAddress;
  targetIdV = targetId;
}

void UploadOperationWizard::mediaSetSelected( const Arinc665::Media::ConstMediaSetPtr &mediaSet )
{
  mediaSetV = mediaSet;
}

void UploadOperationWizard::loadsSelected( const Arinc665::Media::ConstLoads &loads )
{
  loadsV = loads;
}

void UploadOperationWizard::startOperation()
{
  assert( operationV );
  operationV->startOperation( targetAddressV, targetIdV );
}

void UploadOperationWizard::operationDeferred( const std::chrono::seconds waitTime )
{
  ui->status->operationInitialised(
    Arinc615a::Information::InitializationResponse{
      Arinc615a::OperationAcceptanceStatusCode::OperationDeferred,
      std::format( "Deferred for {} seconds.", waitTime.count() ) } );
}

void UploadOperationWizard::operationFinished()
{
  // TODO Jump to last page?!?
  next();
}

void UploadOperationWizard::operationStatus( const Arinc615a::Information::UploadStatus &status )
{
  // Handle Load List Transmission
  if ( ( status.code() == Arinc615a::StatusCode::OperationAccepted ) && !loadListTransmitted )
  {
    operationV->transmitLoads( mediaSetV, loadsV );
    loadListTransmitted = true;
  }
}

}
