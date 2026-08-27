// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::InformationOperationStatusPage.
 **/

#include "InformationOperationStatusPage.hpp"

#include "ui_InformationOperationStatusPage.h"

#include <arinc_615a_qt/InformationStatusLogModel.hpp>
#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/information/InitializationResponse.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

#include <QAbstractButton>

namespace Arinc615aDlaQt {

InformationOperationStatusPage::InformationOperationStatusPage( QWidget * const parent ) :
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::InformationOperationStatusPage >() },
  statusLogModelV{
    std::make_unique< Arinc615aQt::InformationStatusLogModel >( this ) }
{
  ui->setupUi( this );

  ui->statusLog->setModel( statusLogModelV.get() );
  ui->statusLog->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );
}

InformationOperationStatusPage::~InformationOperationStatusPage() = default;

bool InformationOperationStatusPage::isComplete() const
{
  return isCompleted;
}

void InformationOperationStatusPage::initializePage()
{
  // add custom button -> Abort Operation
  wizard()->setOption( QWizard::HaveCustomButton1, true );
  // disable default cancel button
  wizard()->setOption( QWizard::NoCancelButton, true );

  connect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &InformationOperationStatusPage::abortOperation );

  emit startOperation();
}

void InformationOperationStatusPage::operationInitialised(
  const Arinc615a::Information::InitializationResponse &response )
{
  QPalette palette{ ui->status->palette() };
  palette.setBrush(
    QPalette::ColorRole::Base,
    Arinc615aQt::StatusCode_color( static_cast< Arinc615a::StatusCode >( response.code() ) ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  ui->status->setText( HelperQt::toQString( Arinc615a::status(
    Arinc615a::OperationClass::Information,
    response.code(),
    response.description() ) ) );
  ui->status->setToolTip(
    HelperQt::toQString(
      Arinc615a::StatusCodeDescription::instance().name( static_cast< Arinc615a::StatusCode >( response.code() ) ) ) );
  ui->status->setPalette( palette );
}

void InformationOperationStatusPage::operationFinished()
{
  disconnect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &InformationOperationStatusPage::abortOperation );

  wizard()->setOption( QWizard::HaveCustomButton1, false );

  isCompleted = true;
  emit completeChanged();
}

void InformationOperationStatusPage::operationStatus( const Arinc615a::Information::InformationStatus &status )
{
  QPalette palette{ ui->status->palette() };
  palette.setBrush(
    QPalette::ColorRole::Base,
    Arinc615aQt::StatusCode_color( status.code() ) );
  palette.setColor(
    QPalette::ColorRole::Text,
    Qt::black );

  ui->statusCounter->setValue( status.counter() );
  ui->status->setText( HelperQt::toQString( Arinc615a::status(
    Arinc615a::OperationClass::Information,
    status.code(),
    status.description() ) ) );
  ui->status->setToolTip( HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( status.code() ) ) );
  ui->status->setPalette( palette );

  statusLogModelV->status( status );
  ui->statusLog->resizeColumnsToContents();
  ui->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

}
