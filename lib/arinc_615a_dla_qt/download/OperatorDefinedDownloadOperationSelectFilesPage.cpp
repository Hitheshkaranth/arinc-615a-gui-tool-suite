// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class
 *   Arinc615aDlaQt::OperatorDefinedDownloadOperationSelectFilesPage.
 **/

#include "OperatorDefinedDownloadOperationSelectFilesPage.hpp"

#include "ui_OperatorDefinedDownloadOperationSelectFilesPage.h"

#include <arinc_615a_qt/DownloadStatusLogModel.hpp>
#include <arinc_615a_qt/DownloadFilesStatusModel.hpp>
#include <arinc_615a_qt/DownloadFilesInformationModel.hpp>
#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

#include <QAbstractButton>

namespace Arinc615aDlaQt {

OperatorDefinedDownloadOperationSelectFilesPage::
OperatorDefinedDownloadOperationSelectFilesPage(
  QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::OperatorDefinedDownloadOperationSelectFilesPage >() },
  statusLogModelV{ std::make_unique< Arinc615aQt::DownloadStatusLogModel >( this ) },
  filesInformationModelV{ std::make_unique< Arinc615aQt::DownloadFilesInformationModel >( this ) }
{
  uiV->setupUi( this );
  setCommitPage( true );

  uiV->statusLog->setModel( statusLogModelV.get() );
  uiV->statusLog->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  uiV->availableFiles->setModel( filesInformationModelV.get() );
  uiV->availableFiles->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  connect(
    uiV->availableFiles->selectionModel(),
    &QItemSelectionModel::selectionChanged,
    this,
    &OperatorDefinedDownloadOperationSelectFilesPage::updateSelectedFiles );
}

OperatorDefinedDownloadOperationSelectFilesPage::~OperatorDefinedDownloadOperationSelectFilesPage() = default;

bool OperatorDefinedDownloadOperationSelectFilesPage::isComplete() const
{
  return aborted || !uiV->availableFiles->selectionModel()->selectedRows().isEmpty();
}

void OperatorDefinedDownloadOperationSelectFilesPage::initializePage()
{
  // add custom button -> Abort Operation
  wizard()->setOption( QWizard::HaveCustomButton1, true );
  // disable default cancel button
  wizard()->setOption( QWizard::NoCancelButton, true );

  connect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &OperatorDefinedDownloadOperationSelectFilesPage::abortOperation );
}

void OperatorDefinedDownloadOperationSelectFilesPage::operationInitialised(
  const Arinc615a::Information::InitializationResponse &response )
{
  QPalette palette{ uiV->status->palette() };
  palette.setBrush(
    QPalette::ColorRole::Base,
    Arinc615aQt::StatusCode_color( static_cast< Arinc615a::StatusCode >( response.code() ) ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  uiV->status->setText( HelperQt::toQString( Arinc615a::status(
    Arinc615a::OperationClass::Download,
    response.code(),
    response.description() ) ) );
  uiV->status->setToolTip(
    HelperQt::toQString(
      Arinc615a::StatusCodeDescription::instance().name( static_cast< Arinc615a::StatusCode >( response.code() ) ) ) );
  uiV->status->setPalette( palette );
}

void OperatorDefinedDownloadOperationSelectFilesPage::operationFinished()
{
  disconnect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &OperatorDefinedDownloadOperationSelectFilesPage::abortOperation );

  wizard()->setOption( QWizard::HaveCustomButton1, false );

  emit completeChanged();
}

void OperatorDefinedDownloadOperationSelectFilesPage::operationStatus(
  const Arinc615a::Information::DownloadStatus &status )
{
  QPalette palette{ uiV->status->palette() };
  palette.setBrush( QPalette::ColorRole::Base, Arinc615aQt::StatusCode_color( status.code() ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  uiV->statusCounter->setValue( status.counter() );
  uiV->status->setText( HelperQt::toQString( Arinc615a::status(
    Arinc615a::OperationClass::Download,
    status.code(),
    status.description() ) ) );
  uiV->status->setToolTip( HelperQt::toQString(
    Arinc615a::StatusCodeDescription::instance().name( status.code() ) ) );
  uiV->status->setPalette( palette );
  uiV->listRatio->setValue( static_cast< int >( status.listRatio().value() ) );

  statusLogModelV->status( status );
  uiV->statusLog->resizeColumnsToContents();
  uiV->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

void OperatorDefinedDownloadOperationSelectFilesPage::downloadingList(
  const Arinc615a::Information::DownloadFilesInformation &list )
{
  filesInformationModelV->files( list );
}

void OperatorDefinedDownloadOperationSelectFilesPage::updateSelectedFiles()
{
  Arinc615a::Information::DownloadFiles files{};

  for ( const auto &fileIndex : uiV->availableFiles->selectionModel()->selectedRows() )
  {
    files.emplace_back( filesInformationModelV->file( fileIndex ).filename );
  }

  emit selectedFiles( files );
  emit completeChanged();
}

}
