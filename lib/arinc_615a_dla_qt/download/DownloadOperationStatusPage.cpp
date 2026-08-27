// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::DownloadOperationStatusPage.
 **/

#include "DownloadOperationStatusPage.hpp"

#include "ui_DownloadOperationStatusPage.h"

#include <arinc_615a_qt/DownloadStatusLogModel.hpp>
#include <arinc_615a_qt/DownloadFilesStatusModel.hpp>
#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

#include <QAbstractButton>

namespace Arinc615aDlaQt {

DownloadOperationStatusPage::DownloadOperationStatusPage( QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::DownloadOperationStatusPage >() },
  filesStatusModelV{ std::make_unique< Arinc615aQt::DownloadFilesStatusModel >( this ) },
  statusLogModelV{ std::make_unique< Arinc615aQt::DownloadStatusLogModel >( this ) },
  statusLogFilesModelV{ std::make_unique< Arinc615aQt::DownloadFilesStatusModel >( this ) }
{
  uiV->setupUi( this );

  uiV->filesStatus->setModel( filesStatusModelV.get() );
  uiV->filesStatus->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  uiV->statusLog->setModel( statusLogModelV.get() );
  uiV->statusLog->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  uiV->statusLogFiles->setModel( statusLogFilesModelV.get() );
  uiV->statusLogFiles->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  connect(
    uiV->statusLog->selectionModel(),
    &QItemSelectionModel::currentChanged,
    this,
    &DownloadOperationStatusPage::statusSelected );
}

DownloadOperationStatusPage::~DownloadOperationStatusPage() = default;

bool DownloadOperationStatusPage::isComplete() const
{
  return isCompleted;
}

void DownloadOperationStatusPage::initializePage()
{
  // add custom button -> Abort Operation
  wizard()->setOption( QWizard::HaveCustomButton1, true );
  // disable default cancel button
  wizard()->setOption( QWizard::NoCancelButton, true );

  connect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &DownloadOperationStatusPage::abortOperation );
}

void DownloadOperationStatusPage::operationInitialised( const Arinc615a::Information::InitializationResponse &response )
{
  QPalette palette{ uiV->status->palette() };
  palette.setBrush(
    QPalette::ColorRole::Base,
    Arinc615aQt::StatusCode_color( static_cast< Arinc615a::StatusCode >( response.code() ) ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  uiV->status->setText( HelperQt::toQString( Arinc615a::status( Arinc615a::OperationClass::Download, response.code(), response.description() ) ) );
  uiV->status->setToolTip(
    HelperQt::toQString(
      Arinc615a::StatusCodeDescription::instance().name( static_cast< Arinc615a::StatusCode >( response.code() ) ) ) );
  uiV->status->setPalette( palette );
}

void DownloadOperationStatusPage::operationFinished()
{
  disconnect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &DownloadOperationStatusPage::abortOperation );

  wizard()->setOption( QWizard::HaveCustomButton1, false );

  isCompleted = true;
  emit completeChanged();
}

void  DownloadOperationStatusPage::operationStatus( const Arinc615a::Information::DownloadStatus &status )
{
  QPalette palette{ uiV->status->palette() };
  palette.setBrush( QPalette::ColorRole::Base, Arinc615aQt::StatusCode_color( status.code() ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  uiV->statusCounter->setValue( status.counter() );
  uiV->status->setText(
    HelperQt::toQString(
      Arinc615a::status( Arinc615a::OperationClass::Download, status.code(), status.description() ) ) );
  uiV->status->setToolTip( HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( status.code() ) ) );
  uiV->status->setPalette( palette );
  uiV->listRatio->setValue( static_cast< int >( status.listRatio().value() ) );

  filesStatusModelV->status( status.files() );
  uiV->filesStatus->resizeColumnsToContents();

  statusLogModelV->status( status );
  uiV->statusLog->resizeColumnsToContents();
  uiV->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

void DownloadOperationStatusPage::statusSelected( const QModelIndex &index )
{
  statusLogFilesModelV->status( statusLogModelV->status( index ).files() );
  uiV->statusLogFiles->resizeColumnsToContents();
}

}
