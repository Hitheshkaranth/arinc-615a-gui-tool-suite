// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::UploadOperationStatusPage.
 **/

#include "UploadOperationStatusPage.hpp"

#include "ui_UploadOperationStatusPage.h"

#include <arinc_615a_qt/StatusCode.hpp>
#include <arinc_615a_qt/UploadStatusLogModel.hpp>
#include <arinc_615a_qt/UploadLoadsStatusModel.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

#include <QAbstractButton>

namespace Arinc615aDlaQt {

UploadOperationStatusPage::UploadOperationStatusPage( QWidget * const parent ) :
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::UploadOperationStatusPage >() },
  loadsStatusModelV{ std::make_unique< Arinc615aQt::UploadLoadsStatusModel >( this ) },
  statusLogModelV{ std::make_unique< Arinc615aQt::UploadStatusLogModel >( this ) },
  statusLogLoadsStatusModelV{ std::make_unique< Arinc615aQt::UploadLoadsStatusModel >( this ) }
{
  ui->setupUi( this );

  ui->loadHeaderStatus->setModel( loadsStatusModelV.get() );
  ui->loadHeaderStatus->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  ui->statusLog->setModel( statusLogModelV.get() );
  ui->statusLog->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  ui->loadHeaderStatusLog->setModel( statusLogLoadsStatusModelV.get() );
  ui->loadHeaderStatusLog->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  connect(
    ui->statusLog->selectionModel(),
    &QItemSelectionModel::currentChanged,
    this,
    &UploadOperationStatusPage::statusSelected );
}

UploadOperationStatusPage::~UploadOperationStatusPage() = default;

bool UploadOperationStatusPage::isComplete() const
{
  return isCompleted;
}

void UploadOperationStatusPage::initializePage()
{
  // add custom button -> Abort Operation
  wizard()->setOption( QWizard::HaveCustomButton1, true );
  // disable default cancel button
  wizard()->setOption( QWizard::NoCancelButton, true );

  connect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &UploadOperationStatusPage::abortOperation );

  emit startOperation();
}

void UploadOperationStatusPage::operationInitialised( const Arinc615a::Information::InitializationResponse &response )
{
  QPalette palette{ ui->status->palette() };
  palette.setBrush(
    QPalette::ColorRole::Base,
    Arinc615aQt::StatusCode_color( static_cast< Arinc615a::StatusCode >( response.code() ) ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  ui->status->setText(
    HelperQt::toQString(
      Arinc615a::status( Arinc615a::OperationClass::Upload, response.code(), response.description() ) ) );
  ui->status->setToolTip(
    HelperQt::toQString(
      Arinc615a::StatusCodeDescription::instance().name( static_cast< Arinc615a::StatusCode >( response.code() ) ) ) );
  ui->status->setPalette( palette );
}

void UploadOperationStatusPage::operationFinished()
{
  disconnect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &UploadOperationStatusPage::abortOperation );

  wizard()->setOption( QWizard::HaveCustomButton1, false );

  isCompleted = true;
  emit completeChanged();
}

void UploadOperationStatusPage::operationStatus( const Arinc615a::Information::UploadStatus &status )
{
  QPalette palette{ ui->status->palette() };
  palette.setBrush( QPalette::ColorRole::Base, Arinc615aQt::StatusCode_color( status.code() ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  ui->statusCounter->setValue( status.counter() );
  ui->status->setText(
    HelperQt::toQString(
      Arinc615a::status( Arinc615a::OperationClass::Upload, status.code(), status.description() ) ) );
  ui->status->setToolTip( HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( status.code() ) ) );
  ui->status->setPalette( palette );
  ui->listRatio->setValue( static_cast< int >( status.listRatio().value() ) );

  loadsStatusModelV->status( status.loads() );
  ui->loadHeaderStatus->resizeColumnsToContents();

  statusLogModelV->status( status );
  ui->statusLog->resizeColumnsToContents();
  ui->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

void UploadOperationStatusPage::statusSelected( const QModelIndex &index )
{
  statusLogLoadsStatusModelV->status( statusLogModelV->status( index ).loads() );
  ui->loadHeaderStatusLog->resizeColumnsToContents();
}

}
