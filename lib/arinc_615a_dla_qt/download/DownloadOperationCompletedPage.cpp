// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::DownloadOperationCompletedPage.
 **/

#include "DownloadOperationCompletedPage.hpp"

#include "ui_DownloadOperationCompletedPage.h"

#include <arinc_615a_qt/DownloadStatusLogModel.hpp>
#include <arinc_615a_qt/DownloadFilesStatusModel.hpp>
#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc615aDlaQt {

DownloadOperationCompletedPage::DownloadOperationCompletedPage( QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::DownloadOperationCompletedPage >() },
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
    &DownloadOperationCompletedPage::statusSelected );
}

DownloadOperationCompletedPage::~DownloadOperationCompletedPage() = default;

void DownloadOperationCompletedPage::operationFinished(
  const Arinc615a::StatusCode code,
  const std::string &description )
{
  QPalette palette{ uiV->finalStatus->palette() };
  palette.setBrush( QPalette::ColorRole::Base, Arinc615aQt::StatusCode_color( code ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  uiV->finalStatus->setText(
    HelperQt::toQString( Arinc615a::status( Arinc615a::OperationClass::Download, code, description ) ) );
  uiV->finalStatus->setToolTip( HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( code ) ) );
  uiV->finalStatus->setPalette( palette );
}

void DownloadOperationCompletedPage::operationStatus( const Arinc615a::Information::DownloadStatus &status )
{
  uiV->statusCounter->setValue( status.counter() );

  filesStatusModelV->status( status.files() );
  uiV->filesStatus->resizeColumnsToContents();

  statusLogModelV->status( status );
  uiV->statusLog->resizeColumnsToContents();
  uiV->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

void DownloadOperationCompletedPage::statusSelected( const QModelIndex &index )
{
  statusLogFilesModelV->status( statusLogModelV->status( index ).files() );
  uiV->statusLogFiles->resizeColumnsToContents();
}

}
