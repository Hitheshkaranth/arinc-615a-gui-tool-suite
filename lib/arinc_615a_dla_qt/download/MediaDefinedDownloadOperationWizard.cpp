// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::MediaDefinedDownloadOperationWizard.
 **/

#include "MediaDefinedDownloadOperationWizard.hpp"

#include "ui_MediaDefinedDownloadOperationWizard.h"

#include <arinc_615a_dla_qt/operations/MediaDefinedDownloadOperation.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>
#include <arinc_615a/information/DownloadStatus.hpp>

#include <helper_qt/String.hpp>

#include <QIcon>

#include <format>

namespace Arinc615aDlaQt {

MediaDefinedDownloadOperationWizard::MediaDefinedDownloadOperationWizard(
  const Arinc615a::Host::ProtocolPtr &protocol,
  const DataLoaderConfiguration &configuration,
  const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
  const std::filesystem::path &downloadBaseDirectory,
  QWidget * const parent ) :
  QWizard{ parent },
  uiV{ std::make_unique< Ui::MediaDefinedDownloadOperationWizard >() },
  operationV{ std::make_unique< MediaDefinedDownloadOperation >( protocol, configuration, this ) },
  downloadBaseDirectoryV{ downloadBaseDirectory }
{
  assert( operationV );

  uiV->setupUi( this );

  uiV->settings->downloadBaseDirectory( downloadBaseDirectory );
  uiV->settings->createDownloadDirectory( createDownloadDirectoryV );
  uiV->settings->targets( targetsInformation );

  setButtonText( CustomButton1, tr( "Abort Operation" ) );

  // set Logo
  QIcon icon;
  icon.addFile(
    QString::fromUtf8( ":/arinc615a_media_defined_download_operation.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap( QWizard::WizardPixmap::LogoPixmap, icon.pixmap( 64 ) );
  }

  connect(
    this,
    &MediaDefinedDownloadOperationWizard::currentIdChanged,
    this,
    &MediaDefinedDownloadOperationWizard::pageChanged );

  connect(
    uiV->settings,
    &MediaDefinedDownloadOperationSettingsPage::targetSelected,
    this,
    &MediaDefinedDownloadOperationWizard::targetSelected );
  connect(
    uiV->settings,
    &MediaDefinedDownloadOperationSettingsPage::downloadBaseDirectorySelected,
    this,
    &MediaDefinedDownloadOperationWizard::downloadBaseDirectorySelected );
  connect(
    uiV->settings,
    &MediaDefinedDownloadOperationSettingsPage::filesSelected,
    this,
    &MediaDefinedDownloadOperationWizard::updateSelectedFiles );
  connect(
    uiV->settings,
    &MediaDefinedDownloadOperationSettingsPage::userDefinedDataProvided,
    this,
    &MediaDefinedDownloadOperationWizard::updateUserDefinedData );

  connect(
    uiV->status,
    &DownloadOperationStatusPage::abortOperation,
    operationV.get(),
    &MediaDefinedDownloadOperation::abortOperation );

  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::operationDeferred,
    this,
    &MediaDefinedDownloadOperationWizard::operationDeferred,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::operationInitialised,
    uiV->status,
    &DownloadOperationStatusPage::operationInitialised,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::operationFinished,
    this,
    &MediaDefinedDownloadOperationWizard::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::operationFinished,
    uiV->status,
    &DownloadOperationStatusPage::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::operationFinished,
    uiV->completed,
    &DownloadOperationCompletedPage::operationFinished,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::receivedStatus,
    this,
    &MediaDefinedDownloadOperationWizard::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::receivedStatus,
    uiV->status,
    &DownloadOperationStatusPage::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &MediaDefinedDownloadOperation::receivedStatus,
    uiV->completed,
    &DownloadOperationCompletedPage::operationStatus,
    Qt::QueuedConnection );
}

MediaDefinedDownloadOperationWizard::~MediaDefinedDownloadOperationWizard() = default;

void MediaDefinedDownloadOperationWizard::pageChanged( const int pageId )
{
  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( Pages{ pageId } )
  {
    case Pages::Status:
      startOperation();
      break;

    default:
      break;
  }
}

void MediaDefinedDownloadOperationWizard::targetSelected(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId )
{
  targetAddressV = targetAddress;
  targetIdV = targetId;
}

void MediaDefinedDownloadOperationWizard::downloadBaseDirectorySelected( const std::filesystem::path &directory )
{
  downloadBaseDirectoryV = directory;
}

void MediaDefinedDownloadOperationWizard::updateSelectedFiles( const Arinc615a::Information::DownloadFiles &files )
{
  filesV = files;
}

void MediaDefinedDownloadOperationWizard::updateUserDefinedData( const std::string &userDefinedData )
{
  userDefinedDataV = userDefinedData;
}

void MediaDefinedDownloadOperationWizard::startOperation()
{
  assert( operationV );
  operationV->startOperation( targetAddressV, targetIdV, downloadBaseDirectoryV, createDownloadDirectoryV );
}

void MediaDefinedDownloadOperationWizard::operationDeferred( const std::chrono::seconds waitTime )
{
  uiV->status->operationInitialised(
    Arinc615a::Information::InitializationResponse{
      Arinc615a::OperationAcceptanceStatusCode::OperationDeferred,
      std::format( "Deferred for {} seconds.", waitTime.count() ) } );
}

void MediaDefinedDownloadOperationWizard::operationFinished()
{
  // TODO Jump to last page?!?
  next();
}

void MediaDefinedDownloadOperationWizard::operationStatus( const Arinc615a::Information::DownloadStatus &status )
{
  // Handle Load List Transmission
  if ( ( status.code() == Arinc615a::StatusCode::OperationAccepted ) && !fileListTransmitted )
  {
    auto userDefinedDataSpan{ std::as_bytes( std::span{ userDefinedDataV.data(), userDefinedDataV.size() } ) };

    operationV->transmitFiles(
      filesV,
      Helper::RawData{ userDefinedDataSpan.begin(), userDefinedDataSpan.end() } );

    fileListTransmitted = true;
  }
}

}
