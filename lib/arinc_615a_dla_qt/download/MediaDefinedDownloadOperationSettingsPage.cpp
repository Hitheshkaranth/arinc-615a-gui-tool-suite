// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::MediaDefinedDownloadOperationSettingsPage.
 **/

#include "MediaDefinedDownloadOperationSettingsPage.hpp"

#include "ui_MediaDefinedDownloadOperationSettingsPage.h"

#include <arinc_615a_qt/DownloadFilesModel.hpp>

#include <arinc_615a/files/download/DownloadOperationRequestFile.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include <boost/exception/all.hpp>

#include <fstream>
#include <filesystem>

namespace Arinc615aDlaQt {

MediaDefinedDownloadOperationSettingsPage::MediaDefinedDownloadOperationSettingsPage(
  QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::MediaDefinedDownloadOperationSettingsPage >() },
  filesModelV{ std::make_unique< Arinc615aQt::DownloadFilesModel >( this ) },
  selectLnrFileDialogV{ std::make_unique< QFileDialog >( this ) }
{
  uiV->setupUi( this );
  setCommitPage( true );

  uiV->files->setModel( filesModelV.get() );

  QSettings settings;

  selectLnrFileDialogV->setWindowTitle( tr( "Select LNR File" ) );
  selectLnrFileDialogV->setNameFilter(tr( "ARINC 615A LNR Files (*.LNR);;Any Files (*)" ) );
  selectLnrFileDialogV->setFileMode( QFileDialog::ExistingFile );
  selectLnrFileDialogV->setDirectory(
    settings.value( "LastLnrDirectory", QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) )
      .toString() );

  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &MediaDefinedDownloadOperationSettingsPage::targetSelected );
  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &MediaDefinedDownloadOperationSettingsPage::completeChanged );

  connect(
    uiV->downloadBaseDirectory,
    &DownloadOperationSelectDownloadDirectoryWidget::downloadBaseDirectorySelected,
    this,
    &MediaDefinedDownloadOperationSettingsPage::downloadBaseDirectorySelected );
  connect(
    uiV->downloadBaseDirectory,
    &DownloadOperationSelectDownloadDirectoryWidget::downloadBaseDirectorySelected,
    this,
    &MediaDefinedDownloadOperationSettingsPage::completeChanged );

  connect(
    uiV->loadFromLnr,
    &QAbstractButton::clicked,
    selectLnrFileDialogV.get(),
    &QFileDialog::show );
  connect(
    selectLnrFileDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &MediaDefinedDownloadOperationSettingsPage::loadLnr );
  connect(
    uiV->addFile,
    &QAbstractButton::clicked,
    this,
    &MediaDefinedDownloadOperationSettingsPage::addFile );
  connect(
    uiV->removeFile,
    &QAbstractButton::clicked,
    this,
    &MediaDefinedDownloadOperationSettingsPage::fileRemove );
  connect(
    uiV->fileUp,
    &QAbstractButton::clicked,
    this,
    &MediaDefinedDownloadOperationSettingsPage::fileUp );
  connect(
    uiV->fileDown,
    &QAbstractButton::clicked,
    this,
    &MediaDefinedDownloadOperationSettingsPage::fileDown );

  connect(
    uiV->filename,
    &QLineEdit::textChanged,
    this,
    &MediaDefinedDownloadOperationSettingsPage::filenameChanged );

  connect(
    uiV->files->selectionModel(),
    &QItemSelectionModel::currentChanged,
    this,
    &MediaDefinedDownloadOperationSettingsPage::fileSelected );

  connect(
    uiV->userDefinedData,
    &QPlainTextEdit::textChanged,
    this,
    &MediaDefinedDownloadOperationSettingsPage::userDefinedDataChanged );
}

MediaDefinedDownloadOperationSettingsPage::
~MediaDefinedDownloadOperationSettingsPage() = default;

bool MediaDefinedDownloadOperationSettingsPage::isComplete() const
{
  return QWizardPage::isComplete() && uiV->targetSettings->completed() && uiV->downloadBaseDirectory->completed()
    && !filesModelV->files().empty();
}

void MediaDefinedDownloadOperationSettingsPage::targets( const Arinc615a::Find::TargetsAddressInformation &information )
{
  uiV->targetSettings->targets( information );
}

void MediaDefinedDownloadOperationSettingsPage::downloadBaseDirectory( const std::filesystem::path &directory )
{
  uiV->downloadBaseDirectory->downloadBaseDirectory( directory );
}

void MediaDefinedDownloadOperationSettingsPage::createDownloadDirectory( bool createDownloadDirectory )
{
  uiV->downloadBaseDirectory->createDownloadDirectory( createDownloadDirectory );
}

void MediaDefinedDownloadOperationSettingsPage::loadLnr( const QString &file )
{
  try
  {
    const auto fileSize{ std::filesystem::file_size( file.toStdString() ) };

    std::ifstream fileStream{ file.toStdString() };

    Helper::RawData rawData( fileSize );

    fileStream.read(
      reinterpret_cast< char * >( std::data( rawData ) ),
      static_cast< std::streamsize >( fileSize ) );

    Arinc615a::Files::DownloadOperationRequestFile lnr{ rawData };

    filesModelV->files( lnr.files() );
    auto userDefinedData{ lnr.userDefinedData() };
    std::string_view userDefinedDataView{
      reinterpret_cast< char const * >( userDefinedData.data() ),
      userDefinedData.size() };

    emit filesSelected( lnr.files() );
    emit userDefinedDataProvided( std::string{ userDefinedDataView } );
    emit completeChanged();

    QSettings settings{};
    settings.setValue( "LastLnrDirectory", selectLnrFileDialogV->directory().path() );
  }
  catch ( const boost::exception &e )
  {
    QMessageBox::critical(
      this,
      "Could not load LUR File",
      QString::fromStdString( boost::diagnostic_information( e ) ) );
  }
  catch ( const std::exception &e )
  {
    QMessageBox::critical(
      this,
      "Could not load LUR File",
      QString::fromStdString( boost::diagnostic_information( e ) ) );
  }
}

void MediaDefinedDownloadOperationSettingsPage::addFile()
{
  auto filename{ uiV->filename->text() };

  if ( !filename.isEmpty() )
  {
    filesModelV->file( filename.toStdString() );

    uiV->filename->selectAll();
    uiV->filename->setFocus();

    emit filesSelected( filesModelV->files() );
    emit completeChanged();
  }
}

void MediaDefinedDownloadOperationSettingsPage::fileRemove()
{
  auto selectedFile{ uiV->files->currentIndex() };

  if ( !selectedFile.isValid() )
  {
    return;
  }

  filesModelV->remove( selectedFile );

  uiV->files->setCurrentIndex( QModelIndex{} );

  emit filesSelected( filesModelV->files() );
  emit completeChanged();
}

void MediaDefinedDownloadOperationSettingsPage::fileUp()
{
  auto selectedFile{ uiV->files->currentIndex() };

  if ( !selectedFile.isValid() )
  {
    return;
  }

  filesModelV->up( selectedFile );

  emit filesSelected( filesModelV->files() );
}

void MediaDefinedDownloadOperationSettingsPage::fileDown()
{
  auto selectedFile{ uiV->files->currentIndex() };

  if ( !selectedFile.isValid() )
  {
    return;
  }

  filesModelV->down( selectedFile );

  emit filesSelected( filesModelV->files() );
}

void MediaDefinedDownloadOperationSettingsPage::filenameChanged(
  const QString &text )
{
  uiV->addFile->setEnabled( !text.isEmpty() );
}

void MediaDefinedDownloadOperationSettingsPage::fileSelected(
  const QModelIndex &index )
{
  if ( !index.isValid() )
  {
    uiV->fileUp->setEnabled( false );
    uiV->fileDown->setEnabled( false );
  }
  else
  {
    uiV->fileUp->setEnabled(
      ( filesModelV->files().size() > 1U ) && ( index.row() != 0 ) );
    uiV->fileDown->setEnabled(
      ( filesModelV->files().size() > 1U )
        && std::cmp_less( index.row(), filesModelV->files().size() - 1 ) );
  }
}

void MediaDefinedDownloadOperationSettingsPage::userDefinedDataChanged()
{
  emit userDefinedDataProvided( uiV->userDefinedData->toPlainText().toStdString() );
}

}
