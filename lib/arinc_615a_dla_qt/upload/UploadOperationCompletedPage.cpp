/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::UploadOperationCompletedPage.
 **/

#include "UploadOperationCompletedPage.hpp"

#include "ui_UploadOperationCompletedPage.h"

#include <arinc_615a_qt/StatusCode.hpp>
#include <arinc_615a_qt/UploadStatusLogModel.hpp>
#include <arinc_615a_qt/UploadLoadsStatusModel.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc615aDlaQt {

UploadOperationCompletedPage::UploadOperationCompletedPage( QWidget * const parent ) :
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::UploadOperationCompletedPage >() },
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
    &UploadOperationCompletedPage::statusSelected );
}

UploadOperationCompletedPage::~UploadOperationCompletedPage() = default;

void UploadOperationCompletedPage::operationFinished( const Arinc615a::StatusCode code, const std::string &description )
{
  QPalette palette{ ui->finalStatus->palette() };
  palette.setBrush( QPalette::ColorRole::Base, Arinc615aQt::StatusCode_color( code ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  ui->finalStatus->setText(
    HelperQt::toQString( Arinc615a::status( Arinc615a::OperationClass::Upload, code, description ) ) );
  ui->finalStatus->setToolTip( HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( code ) ) );
  ui->finalStatus->setPalette( palette );
}

void UploadOperationCompletedPage::receivedStatus( const Arinc615a::Information::UploadStatus &status )
{
  ui->statusCounter->setValue( status.counter() );

  loadsStatusModelV->status( status.loads() );
  ui->loadHeaderStatus->resizeColumnsToContents();

  statusLogModelV->status( status );
  ui->statusLog->resizeColumnsToContents();
  ui->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

void UploadOperationCompletedPage::statusSelected( const QModelIndex &index )
{
  statusLogLoadsStatusModelV->status( statusLogModelV->status( index ).loads() );
  ui->loadHeaderStatusLog->resizeColumnsToContents();
}

}
