// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::DataLoaderMainWindow.
 **/

#include "DataLoaderMainWindow.hpp"

#include "ui_DataLoaderMainWindow.h"

#include <arinc_615a_dla_qt/download/MediaDefinedDownloadOperationWizard.hpp>
#include <arinc_615a_dla_qt/download/OperatorDefinedDownloadOperationWizard.hpp>

#include <arinc_615a_dla_qt/find/FindQueryWizard.hpp>

#include <arinc_615a_dla_qt/information/InformationOperationWizard.hpp>

#include <arinc_615a_dla_qt/upload/UploadOperationWizard.hpp>

#include <arinc_615a_dla_qt/Configuration.hpp>
#include <arinc_615a_dla_qt/SettingsDialog.hpp>

#include <arinc_615a_qt/FindPacketStatisticModel.hpp>
#include <arinc_615a_qt/ProtocolFileStatisticModel.hpp>
#include <arinc_615a_qt/TargetListDialog.hpp>
#include <arinc_615a_qt/TargetsInformationModel.hpp>

#include <arinc_615a/find/clients/Client.hpp>

#include <arinc_615a/host/Protocol.hpp>

#include <arinc_615a/Version.hpp>

#include <arinc_665_qt/actions/LoadMediaSetManagerAction.hpp>
#include <arinc_665_qt/media_set_manager/MediaSetManagerWindow.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>

#include <tftp_qt/PacketStatisticModel.hpp>

#include <qt_icon_resources/QtIconResources.hpp>

#include <helper_qt/AboutDialog.hpp>

#include <boost/exception/all.hpp>

#include <QIcon>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>

namespace Arinc615aDlaQt {

DataLoaderMainWindow::DataLoaderMainWindow():
  ui{ std::make_unique< Ui::DataLoaderMainWindow>() },
  targetListDialog{ std::make_unique< Arinc615aQt::TargetListDialog >( this ) },
  mediaSetManagerWindow{ std::make_unique< Arinc665Qt::MediaSetManager::MediaSetManagerWindow >( this ) },
  settingsDialog{ std::make_unique< SettingsDialog >( this ) },
  aboutDialog{ std::make_unique< HelperQt::AboutDialog >( this ) },
  rxTftpPacketStatisticModelV{ std::make_unique< TftpQt::PacketStatisticModel >( this ) },
  txTftpPacketStatisticModelV{ std::make_unique< TftpQt::PacketStatisticModel >( this ) },
  rxFindPacketStatisticModelV{ std::make_unique< Arinc615aQt::FindPacketStatisticModel >( this ) },
  txFindPacketStatisticModelV{ std::make_unique< Arinc615aQt::FindPacketStatisticModel >( this ) },
  rxProtocolFileStatisticModelV{ std::make_unique< Arinc615aQt::ProtocolFileStatisticModel >( this ) },
  txProtocolFileStatisticModelV{ std::make_unique< Arinc615aQt::ProtocolFileStatisticModel >( this ) },
  workGuard{ ioContext.get_executor() },
  findClient{ Arinc615a::Find::Clients::Client::instance( ioContext ) },
  protocol{ Arinc615a::Host::Protocol::instance( ioContext ) }
{
  assert( findClient && protocol );

  ui->setupUi( this );

  ui->tftpPacketStatisticRx->setModel( rxTftpPacketStatisticModelV.get() );
  ui->tftpPacketStatisticRx->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
  ui->tftpPacketStatisticTx->setModel( txTftpPacketStatisticModelV.get() );
  ui->tftpPacketStatisticTx->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  ui->findPacketStatisticRx->setModel( rxFindPacketStatisticModelV.get() );
  ui->findPacketStatisticRx->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
  ui->findPacketStatisticTx->setModel( txFindPacketStatisticModelV.get() );
  ui->findPacketStatisticTx->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  ui->protocolFileStatisticRx->setModel( rxProtocolFileStatisticModelV.get() );
  ui->protocolFileStatisticRx->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
  ui->protocolFileStatisticTx->setModel( txProtocolFileStatisticModelV.get() );
  ui->protocolFileStatisticTx->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  mediaSetManagerWindow->setWindowTitle( tr( "Media Set Manager" ) );

  QIcon icon{};
  icon.addFile(
    QString::fromUtf8( ":/arinc615a_icon.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  aboutDialog->productLogo( icon.pixmap( 64 ) );
  aboutDialog->productName( QString{ "%1 (%2)" }.arg(
    tr( "ARINC 615A Data Loader" ),
    QString::fromStdString( Arinc615a::Version::Name ) ) );
  aboutDialog->productVersion( QString::fromStdString( Arinc615a::Version::VersionInformation ) );
  aboutDialog->productLicense( QString::fromStdString( Arinc615a::Version::License ) );
  aboutDialog->productUrl( QString::fromStdString( Arinc615a::Version::Url ) );
  aboutDialog->versions(
    {
      Arinc615a::arinc615aVersion(),
      Tftp::tftpVersion(),
      Arinc665::arinc665Version(),
      Arinc649::arinc649Version(),
      QtIconResources::qtIconResourcesVersion(),
      Helper::helperVersion(),
      Helper::boostVersion(),
      HelperQt::qtVersion()
    } );

  loadConfiguration();

  loadMediaSetManager();

  connect(
    ui->actionFindQuery,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::findQuery );

  connect(
    ui->actionInformationOperation,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::informationOperation );

  connect(
    ui->actionUploadOperation,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::uploadOperation );

  connect(
    ui->actionMediaDefinedDownloadOperation,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::mediaDefinedDownloadOperation );

  connect(
    ui->actionOperatorDefinedDownloadOperation,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::operatorDefinedDownloadOperation );

  connect(
    ui->actionManageTargets,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::manageTargets );
  connect(
    targetListDialog.get(),
    &Arinc615aQt::TargetListDialog::accepted,
    this,
    &DataLoaderMainWindow::updateTargets );

  connect(
    ui->actionManageMediaSets,
    &QAction::triggered,
    mediaSetManagerWindow.get(),
    &Arinc665Qt::MediaSetManager::MediaSetManagerWindow::show );

  connect(
    ui->actionManageDataLoader,
    &QAction::triggered,
    this,
    &DataLoaderMainWindow::manageDataLoader );
  connect(
    settingsDialog.get(),
    &SettingsDialog::accepted,
    this,
    &DataLoaderMainWindow::updateDataLoaderSettings );

  connect(
    ui->actionAboutDataLoader,
    &QAction::triggered,
    aboutDialog.get(),
    QOverload<>::of( &HelperQt::AboutDialog::open ) );

  ioThread = std::jthread{
    [ this ]
    {
      try
      {
        ioContext.run();
      }
      catch ( const boost::exception &e )
      {
        QMessageBox::critical(
          nullptr,
          tr( "ARINC 615A I/O Task Exception" ),
          QString::fromStdString( boost::diagnostic_information( e ) ) );

        QApplication::exit();
      }
      catch ( ... )
      {
        QMessageBox::critical(
          nullptr,
          tr( "ARINC 615A I/O Task Exception" ),
          "Unknown exception occurred" );

        QApplication::exit();
      }
    }
  };
}

DataLoaderMainWindow::~DataLoaderMainWindow()
{
  workGuard.reset();

  ioThread.join();

  // try store configuration
  try
  {
    saveConfiguration();

    // Media Set Configuration
    if ( mediaSetManagerV )
    {
      mediaSetManagerV->saveConfiguration();
    }
  }
  catch ( const boost::exception &e )
  {
    QMessageBox::warning(
      nullptr,
      tr( "Write Configuration" ),
      QString::fromStdString( boost::diagnostic_information( e ) ) );
  }
}

void DataLoaderMainWindow::findQuery()
{
  auto * const wizard{ new FindQueryWizard{ findClient, configuration, targetsV, this } };

  connect(
    wizard,
    &FindQueryWizard::finished,
    wizard,
    &FindQueryWizard::deleteLater );
  connect(
    wizard,
    &FindQueryWizard::finished,
    this,
    &DataLoaderMainWindow::updatePacketStatistic );

  wizard->show();
}

void DataLoaderMainWindow::informationOperation()
{
  auto * const wizard{ new InformationOperationWizard{ protocol, configuration, targetsV, this } };

  connect(
    wizard,
    &InformationOperationWizard::finished,
    wizard,
    &InformationOperationWizard::deleteLater );
  connect(
    wizard,
    &InformationOperationWizard::finished,
    this,
    &DataLoaderMainWindow::updatePacketStatistic );

  wizard->show();
}

void DataLoaderMainWindow::uploadOperation()
{
  if ( !mediaSetManagerV )
  {
    // prohibit starting operation, when media set manager is not available
    return;
  }

  auto * const wizard{ new UploadOperationWizard{ protocol, configuration, targetsV, mediaSetManagerV, this } };

  connect(
    wizard,
    &UploadOperationWizard::finished,
    wizard,
    &UploadOperationWizard::deleteLater );
  connect(
    wizard,
    &UploadOperationWizard::finished,
    this,
    &DataLoaderMainWindow::updatePacketStatistic );

  wizard->show();
}

void DataLoaderMainWindow::mediaDefinedDownloadOperation()
{
  auto * const wizard{ new MediaDefinedDownloadOperationWizard{
    protocol,
    configuration,
    targetsV,
    configuration.downloadBaseDirectory,
    this } };

  connect(
    wizard,
    &MediaDefinedDownloadOperationWizard::finished,
    wizard,
    &MediaDefinedDownloadOperationWizard::deleteLater );
  connect(
    wizard,
    &MediaDefinedDownloadOperationWizard::finished,
    this,
    &DataLoaderMainWindow::updatePacketStatistic );

  wizard->show();
}

void DataLoaderMainWindow::operatorDefinedDownloadOperation()
{
  auto * const wizard{ new OperatorDefinedDownloadOperationWizard{
    protocol,
    configuration,
    targetsV,
    configuration.downloadBaseDirectory,
    this } };

  connect(
    wizard,
    &OperatorDefinedDownloadOperationWizard::finished,
    wizard,
    &OperatorDefinedDownloadOperationWizard::deleteLater );
  connect(
    wizard,
    &OperatorDefinedDownloadOperationWizard::finished,
    this,
    &DataLoaderMainWindow::updatePacketStatistic );

  wizard->show();
}

void DataLoaderMainWindow::manageTargets()
{
  targetListDialog->targets( targetsV );
  targetListDialog->open();
}

void DataLoaderMainWindow::updateTargets()
{
  targetsV = targetListDialog->targets();
}

void DataLoaderMainWindow::manageDataLoader()
{
  settingsDialog->configuration( configuration );
  settingsDialog->open();
}

void DataLoaderMainWindow::updateDataLoaderSettings()
{
  configuration = settingsDialog->configuration();

  if ( mediaSetManagerV->directory() != configuration.mediaSetConfigurationDirectory )
  {
    loadMediaSetManager();
  }
}

void DataLoaderMainWindow::loadConfiguration()
{
  // Data Loader Configuration
  configuration = loadDataLoaderConfiguration();

  // Target List
  targetsV = loadTargets();
}

void DataLoaderMainWindow::saveConfiguration()
{
  // Data Loader Configuration
  saveDataLoaderConfiguration( configuration );

  // Target List
  saveTargets( targetsV );
}

void DataLoaderMainWindow::loadMediaSetManager()
{
  // disable actions which depend on Media Set Manager
  ui->actionUploadOperation->setEnabled( false );
  ui->actionManageMediaSets->setEnabled( false );

  auto * const mediaSetManagerLoader{ new Arinc665Qt::Actions::LoadMediaSetManagerAction{ this } };
  auto * const progressDialog{ new QProgressDialog{ this } };

  progressDialog->setWindowTitle( tr( "Load Media Sets" ) );
  progressDialog->setAutoReset( false );

  const QSettings settings{};

  mediaSetManagerLoader->mediaSetDirectory( configuration.mediaSetConfigurationDirectory );
  mediaSetManagerLoader->checkMediaSetIntegrity( settings.value( "CheckIntegrityOnStartup", true ).toBool() );

  connect(
    mediaSetManagerLoader,
    &Arinc665Qt::Actions::LoadMediaSetManagerAction::mediaSetManagerLoadProgress,
    this,
    [ progressDialog ]( const size_t currentMediaSet, const size_t numberOfMediaSets, const QString &partNumber ) {
      progressDialog->setMaximum( static_cast< int >( numberOfMediaSets ) );
      progressDialog->setValue( static_cast< int >(  currentMediaSet ) );
      progressDialog->setLabelText( QString{ tr( "Media Set<br/><tt>%1</tt>" ) }.arg( partNumber ) );
    } );
  connect(
    mediaSetManagerLoader,
    &Arinc665Qt::Actions::LoadMediaSetManagerAction::mediaSetManagerLoaded,
    this,
    [ this, progressDialog, mediaSetManagerLoader ](
      const Arinc665::Utils::MediaSetManagerPtr &mediaSetManager )
    {
      mediaSetManagerV = mediaSetManager;
      mediaSetManagerWindow->mediaSetManger( mediaSetManager );

      // enable actions which depend on Media Set Manager
      ui->actionUploadOperation->setEnabled( true );
      ui->actionManageMediaSets->setEnabled( true );

      progressDialog->reset();
      progressDialog->deleteLater();

      mediaSetManagerLoader->deleteLater();
    } );
  connect(
    mediaSetManagerLoader,
    &Arinc665Qt::Actions::LoadMediaSetManagerAction::failed,
    this,
    [ this, progressDialog, mediaSetManagerLoader ]( const QString &reason )
    {
      QMessageBox::critical(
        nullptr,
        tr( "Cannot open Media Set Manager" ),
        QString{ tr(
          "<b>Media Set Directory:</b><br/><i>%1</i><br/>"
          "<b>Error:</b><br/><tt>%2</tt>" ) }
          .arg( QString::fromStdString( configuration.mediaSetConfigurationDirectory.string() ), reason ) );

      progressDialog->reset();
      progressDialog->deleteLater();

      mediaSetManagerLoader->deleteLater();

      close();
    } );

  progressDialog->show();

  // QMetaObject::invokeMethod calls directly new()
  // NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)

  // call start asynchronous
  QMetaObject::invokeMethod(
    mediaSetManagerLoader,
    &Arinc665Qt::Actions::LoadMediaSetManagerAction::start,
    Qt::ConnectionType::QueuedConnection );

  // NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
}

void DataLoaderMainWindow::updatePacketStatistic()
{
  rxTftpPacketStatisticModelV->statistic( Tftp::Packets::PacketStatistic::globalReceive().statistic() );
  txTftpPacketStatisticModelV->statistic( Tftp::Packets::PacketStatistic::globalTransmit().statistic() );

  rxFindPacketStatisticModelV->statistic( Arinc615a::Find::Packets::PacketStatistic::globalReceive().statistic() );
  txFindPacketStatisticModelV->statistic( Arinc615a::Find::Packets::PacketStatistic::globalTransmit().statistic() );

  rxProtocolFileStatisticModelV->statistic( Arinc615a::Files::ProtocolFileStatistic::globalReceive().statistic() );
  txProtocolFileStatisticModelV->statistic( Arinc615a::Files::ProtocolFileStatistic::globalTransmit().statistic() );
}

}
