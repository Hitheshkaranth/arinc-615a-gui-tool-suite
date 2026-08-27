// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::SettingsDialog.
 **/

#include "SettingsDialog.hpp"

#include "ui_SettingsDialog.h"

#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <tftp/packets/Packets.hpp>

#include <helper/SafeCast.hpp>

namespace Arinc615aDlaQt {

SettingsDialog::SettingsDialog( QWidget * const parent ):
  QDialog{ parent },
  uiV{ std::make_unique< Ui::SettingsDialog>() },
  selectMediaSetMangerDirectoryFileDialogV{ std::make_unique< QFileDialog >( this ) },
  selectDownloadBaseDirectoryFileDialogV{ std::make_unique< QFileDialog >( this ) }
{
  uiV->setupUi( this );

  selectMediaSetMangerDirectoryFileDialogV->setWindowTitle( tr( "Select ARINC 665 Media Set Manager Directory" ) );
  selectMediaSetMangerDirectoryFileDialogV->setFileMode( QFileDialog::Directory );
  selectMediaSetMangerDirectoryFileDialogV->setOption( QFileDialog::ShowDirsOnly );

  connect(
    uiV->selectMediaSetManagerDirectory,
    &QPushButton::pressed,
    selectMediaSetMangerDirectoryFileDialogV.get(),
    QOverload<>::of( &QDialog::open ) );
  connect(
    selectMediaSetMangerDirectoryFileDialogV.get(),
    &QFileDialog::fileSelected,
    uiV->mediaSetManagerDirectory,
    &QLineEdit::setText );

  selectDownloadBaseDirectoryFileDialogV->setWindowTitle( tr( "Select Download Base Directory" ) );
  selectDownloadBaseDirectoryFileDialogV->setFileMode( QFileDialog::Directory );
  selectDownloadBaseDirectoryFileDialogV->setOption( QFileDialog::ShowDirsOnly );

  connect(
    uiV->selectDownloadBaseDirectory,
    &QPushButton::pressed,
    selectDownloadBaseDirectoryFileDialogV.get(),
    QOverload<>::of( &QDialog::open ) );
  connect(
    selectDownloadBaseDirectoryFileDialogV.get(),
    &QFileDialog::fileSelected,
    uiV->downloadBaseDirectory,
    &QLineEdit::setText );
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::configuration( const DataLoaderConfiguration &configuration )
{
  uiV->findPort->setValue( Helper::safeCast< int >( configuration.findConfiguration.findPort ) );
  uiV->findTimeout->setValue( Helper::safeCast< int >( configuration.findTimeout.count() ) );
  uiV->findDynamicPort->setChecked( configuration.dynamicFindPort );

  uiV->tftpServerPort->setValue(
    Helper::safeCast< int >( configuration.dataLoaderConfiguration.tftpConfiguration.tftpServerPort ) );
  uiV->tftpTimeout->setValue(
    Helper::safeCast< int >( configuration.dataLoaderConfiguration.tftpConfiguration.tftpTimeout.count() ) );
  uiV->tftpRetries->setValue(
    Helper::safeCast< int >( configuration.dataLoaderConfiguration.tftpConfiguration.tftpRetries ) );
  uiV->tftpDally->setChecked( configuration.dataLoaderConfiguration.tftpConfiguration.dally );
  uiV->tftpTransferSizeOption->setChecked(
    configuration.dataLoaderConfiguration.tftpOptionsConfiguration.handleTransferSizeOption );
  uiV->tftpBlockSizeOption->setChecked(
    configuration.dataLoaderConfiguration.tftpOptionsConfiguration.blockSizeOption.has_value() );
  uiV->tftpBlockSizeOptionValue->setValue(
    Helper::safeCast< int >( configuration.dataLoaderConfiguration.tftpOptionsConfiguration.blockSizeOption.value_or(
      ::Tftp::Packets::BlockSizeOptionDefault ) ) );
  uiV->tftpTimeoutOption->setChecked(
    configuration.dataLoaderConfiguration.tftpOptionsConfiguration.timeoutOption.has_value() );
  uiV->tftpTimeoutOptionValue->setValue(
    Helper::safeCast< int >( configuration.dataLoaderConfiguration.tftpOptionsConfiguration.timeoutOption
      .value_or( Tftp::DefaultTftpReceiveTimeout )
      .count() ) );
  uiV->logProtocolFiles->setChecked( configuration.dataLoaderConfiguration.protocolFileLogging );

  uiV->arinc615aTimeout->setValue( Helper::safeCast< int >( configuration.dlpTimeout.count() ) );
  uiV->arinc615aRetries->setValue( Helper::safeCast< int >( configuration.dataLoaderConfiguration.dlpRetries ) );
  uiV->arinc615aPortOption->setChecked( configuration.portOption );

  uiV->mediaSetManagerDirectory->setText(
    QString::fromStdString( configuration.mediaSetConfigurationDirectory.string() ) );
  selectMediaSetMangerDirectoryFileDialogV->setDirectory(
    QString::fromStdString( configuration.mediaSetConfigurationDirectory.string() ) );

  uiV->downloadBaseDirectory->setText( QString::fromStdString( configuration.downloadBaseDirectory.string() ) );
  selectDownloadBaseDirectoryFileDialogV->setDirectory(
    QString::fromStdString( configuration.downloadBaseDirectory.string() ) );
  uiV->verifyDownloadFiles->setChecked( configuration.verifyDownloadFiles );
}

DataLoaderConfiguration SettingsDialog::configuration() const
{
  DataLoaderConfiguration configuration{};

  configuration.findConfiguration.findPort = Helper::safeCast< uint16_t >( uiV->findPort->value() );
  configuration.findTimeout = std::chrono::seconds { uiV->findTimeout->value() };
  configuration.dynamicFindPort = uiV->findDynamicPort->isChecked();

  configuration.dataLoaderConfiguration.tftpConfiguration.tftpServerPort =
    Helper::safeCast< uint16_t >( uiV->tftpServerPort->value() );
  configuration.dataLoaderConfiguration.tftpConfiguration.tftpTimeout =
    std::chrono::seconds{ uiV->tftpTimeout->value() };
  configuration.dataLoaderConfiguration.tftpConfiguration.tftpRetries =
    Helper::safeCast< uint16_t >( uiV->tftpRetries->value() );
  configuration.dataLoaderConfiguration.tftpConfiguration.dally = uiV->tftpDally->isChecked();
  configuration.dataLoaderConfiguration.tftpOptionsConfiguration.handleTransferSizeOption =
    uiV->tftpTransferSizeOption->isChecked();
  configuration.dataLoaderConfiguration.tftpOptionsConfiguration.blockSizeOption =
    uiV->tftpBlockSizeOption->isChecked() ? boost::make_optional( uiV->tftpBlockSizeOptionValue->value() ) : boost::none;
  configuration.dataLoaderConfiguration.tftpOptionsConfiguration.timeoutOption =
    uiV->tftpTimeoutOption->isChecked() ?
      boost::make_optional( std::chrono::seconds{ uiV->tftpTimeoutOptionValue->value() } ) : boost::none;
  configuration.dataLoaderConfiguration.protocolFileLogging = uiV->logProtocolFiles->isChecked();

  configuration.dlpTimeout = std::chrono::seconds{ uiV->arinc615aTimeout->value() };
  configuration.dataLoaderConfiguration.dlpRetries = Helper::safeCast< uint16_t >( uiV->arinc615aRetries->value() );
  configuration.portOption = uiV->arinc615aPortOption->isChecked();
  configuration.mediaSetConfigurationDirectory = uiV->mediaSetManagerDirectory->text().toStdString();

  configuration.downloadBaseDirectory = uiV->downloadBaseDirectory->text().toStdString();
  configuration.verifyDownloadFiles = uiV->verifyDownloadFiles->isChecked();

  return configuration;
}

}
