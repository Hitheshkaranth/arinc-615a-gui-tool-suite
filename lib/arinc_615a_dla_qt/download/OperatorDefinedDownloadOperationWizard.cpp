// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::OperatorDefinedDownloadOperationWizard.
 **/

#include "OperatorDefinedDownloadOperationWizard.hpp"

#include "ui_OperatorDefinedDownloadOperationWizard.h"

#include <arinc_615a_dla_qt/operations/OperatorDefinedDownloadOperation.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <helper_qt/String.hpp>

#include <QIcon>

#include <format>

namespace Arinc615aDlaQt {

OperatorDefinedDownloadOperationWizard::OperatorDefinedDownloadOperationWizard(
  const Arinc615a::Host::ProtocolPtr &protocol,
  const DataLoaderConfiguration &configuration,
  const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
  const std::filesystem::path &downloadBaseDirectory,
  QWidget * const parent ) :
  QWizard{ parent },
  uiV{ std::make_unique< Ui::OperatorDefinedDownloadOperationWizard >() },
  operationV{ std::make_unique< OperatorDefinedDownloadOperation >( protocol, configuration, this ) },
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
    QString::fromUtf8( ":/arinc615a_operator_defined_download_operation.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap( QWizard::WizardPixmap::LogoPixmap, icon.pixmap( 64 ) );
  }

  connect(
    this,
    &OperatorDefinedDownloadOperationWizard::currentIdChanged,
    this,
    &OperatorDefinedDownloadOperationWizard::pageChanged );

  connect(
    uiV->settings,
    &OperatorDefinedDownloadOperationSettingsPage::targetSelected,
    this,
    &OperatorDefinedDownloadOperationWizard::targetSelected );
  connect(
    uiV->settings,
    &OperatorDefinedDownloadOperationSettingsPage::downloadBaseDirectorySelected,
    this,
    &OperatorDefinedDownloadOperationWizard::downloadBaseDirectorySelected );
  connect(
    uiV->settings,
    &OperatorDefinedDownloadOperationSettingsPage::createDownloadDirectoryChanged,
    this,
    &OperatorDefinedDownloadOperationWizard::createDownloadDirectoryChanged );

  connect(
    uiV->selectFiles,
    &OperatorDefinedDownloadOperationSelectFilesPage::selectedFiles,
    this,
    &OperatorDefinedDownloadOperationWizard::updateSelectedFiles );
  connect(
    uiV->selectFiles,
    &OperatorDefinedDownloadOperationSelectFilesPage::abortOperation,
    operationV.get(),
    &OperatorDefinedDownloadOperation::abortOperation );

  connect(
    uiV->status,
    &DownloadOperationStatusPage::abortOperation,
    operationV.get(),
    &OperatorDefinedDownloadOperation::abortOperation );

  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationDeferred,
    this,
    &OperatorDefinedDownloadOperationWizard::operationDeferred,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationInitialised,
    uiV->selectFiles,
    &OperatorDefinedDownloadOperationSelectFilesPage::operationInitialised,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationInitialised,
    uiV->status,
    &DownloadOperationStatusPage::operationInitialised,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationFinished,
    this,
    &OperatorDefinedDownloadOperationWizard::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationFinished,
    uiV->selectFiles,
    &OperatorDefinedDownloadOperationSelectFilesPage::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationFinished,
    uiV->status,
    &DownloadOperationStatusPage::operationFinished,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::operationFinished,
    uiV->completed,
    &DownloadOperationCompletedPage::operationFinished,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::receivedStatus,
    uiV->selectFiles,
    &OperatorDefinedDownloadOperationSelectFilesPage::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::receivedStatus,
    uiV->status,
    &DownloadOperationStatusPage::operationStatus,
    Qt::QueuedConnection );
  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::receivedStatus,
    uiV->completed,
    &DownloadOperationCompletedPage::operationStatus,
    Qt::QueuedConnection );

  connect(
    operationV.get(),
    &OperatorDefinedDownloadOperation::receivedDownloadingList,
    uiV->selectFiles,
    &OperatorDefinedDownloadOperationSelectFilesPage::downloadingList,
    Qt::QueuedConnection );
}

OperatorDefinedDownloadOperationWizard::~OperatorDefinedDownloadOperationWizard() = default;

void OperatorDefinedDownloadOperationWizard::pageChanged( const int pageId )
{
  switch ( Pages{ pageId } )
  {
    case Pages::SelectFiles:
      startOperation();
      break;

    case Pages::Status:
      filesSelected();
      break;

    default:
      break;
  }
}

void OperatorDefinedDownloadOperationWizard::targetSelected(
  const boost::asio::ip::address &targetAddress,
  const Arinc615a::TargetId &targetId )
{
  targetAddressV = targetAddress;
  targetIdV = targetId;
}

void OperatorDefinedDownloadOperationWizard::downloadBaseDirectorySelected( const std::filesystem::path &directory )
{
  downloadBaseDirectoryV = directory;
}

void OperatorDefinedDownloadOperationWizard::createDownloadDirectoryChanged( const bool createDownloadDirectory )
{
  createDownloadDirectoryV = createDownloadDirectory;
}

void OperatorDefinedDownloadOperationWizard::startOperation()
{
  assert( operationV );
  operationV->startOperation( targetAddressV, targetIdV, downloadBaseDirectoryV, createDownloadDirectoryV );
}

void OperatorDefinedDownloadOperationWizard::updateSelectedFiles(
  const Arinc615a::Information::DownloadFiles &files )
{
  filesV = files;
}

void OperatorDefinedDownloadOperationWizard::filesSelected()
{
  operationV->transmitFiles( filesV );
}

void OperatorDefinedDownloadOperationWizard::operationDeferred( const std::chrono::seconds waitTime )
{
  uiV->selectFiles->operationInitialised(
    Arinc615a::Information::InitializationResponse{
      Arinc615a::OperationAcceptanceStatusCode::OperationDeferred,
      std::format( "Deferred for {} seconds.", waitTime.count() ) } );
}

void OperatorDefinedDownloadOperationWizard::operationFinished()
{
  // TODO Jump to last page?!?
  next();
}

}
