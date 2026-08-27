// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::InformationOperationCompletedPage.
 **/

#include "InformationOperationCompletedPage.hpp"

#include "ui_InformationOperationCompletedPage.h"

#include <arinc_615a_qt/InformationStatusLogModel.hpp>
#include <arinc_615a_qt/StatusCode.hpp>
#include <arinc_615a_qt/TargetsHardwareModel.hpp>
#include <arinc_615a_qt/PartNumbersModel.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

#include <QSettings>
#include <QStandardPaths>
#include <QClipboard>

#include <fstream>

namespace Arinc615aDlaQt {

InformationOperationCompletedPage::InformationOperationCompletedPage(
  QWidget * const parent ) :
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::InformationOperationCompletedPage >() },
  statusLogModelV{ std::make_unique< Arinc615aQt::InformationStatusLogModel >( this ) },
  targetsHardwareModelV{ std::make_unique< Arinc615aQt::TargetsHardwareModel >( this ) },
  partNumbersModelV{ std::make_unique< Arinc615aQt::PartNumbersModel >( this ) },
  selectExportFileDialogV{ std::make_unique< QFileDialog >( this ) }
{
  ui->setupUi( this );

  ui->statusLog->setModel( statusLogModelV.get() );
  ui->statusLog->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  ui->targetHardwares->setModel( targetsHardwareModelV.get() );

  ui->partNumbers->setModel( partNumbersModelV.get() );

  QSettings settings{};

  selectExportFileDialogV->setWindowTitle( tr( "Select Targets Hardware Information File" ) );
  selectExportFileDialogV->setAcceptMode( QFileDialog::AcceptMode::AcceptSave );
  selectExportFileDialogV->setNameFilter( tr( "Text Files (*.txt);;Any Files (*)" ) );
  selectExportFileDialogV->setFileMode( QFileDialog::FileMode::AnyFile );
  selectExportFileDialogV->setDirectory(
    settings
      .value(
        "LastTargetsHardwareListDirectory",
        QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) )
      .toString() );

  connect(
    ui->targetHardwares->selectionModel(),
    &QItemSelectionModel::currentChanged,
    this,
    &InformationOperationCompletedPage::selectTargetHardware );

  connect(
    ui->exportTargetHardwares,
    &QPushButton::clicked,
    selectExportFileDialogV.get(),
    &QFileDialog::show );
  connect(
    selectExportFileDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &InformationOperationCompletedPage::exportTargetsHardware );
  connect(
    ui->copyTargetHardwares,
    &QPushButton::clicked,
    this,
    &InformationOperationCompletedPage::copyTargetsHardware );
}

InformationOperationCompletedPage::~InformationOperationCompletedPage() = default;

void InformationOperationCompletedPage::targetId( const QString &targetId )
{
  selectExportFileDialogV->selectFile( QString{ "%0_Configuration.txt" }.arg( targetId ) );
}

void InformationOperationCompletedPage::operationFinished(
  const Arinc615a::StatusCode code,
  const std::string &description )
{
  QPalette palette{ ui->finalStatus->palette() };
  palette.setBrush( QPalette::ColorRole::Base, Arinc615aQt::StatusCode_color( code ) );
  palette.setColor( QPalette::ColorRole::Text, Qt::black );

  ui->finalStatus->setText( HelperQt::toQString( Arinc615a::status(
    Arinc615a::OperationClass::Information,
    code,
    description ) ) );
  ui->finalStatus->setToolTip( HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( code ) ) );
  ui->finalStatus->setPalette( palette );
}

void InformationOperationCompletedPage::operationStatus( const Arinc615a::Information::InformationStatus &status )
{
  ui->statusCounter->setValue( status.counter() );

  statusLogModelV->status( status );
  ui->statusLog->resizeColumnsToContents();
  ui->statusLog->selectRow( statusLogModelV->rowCount() - 1 );
}

void InformationOperationCompletedPage::information( const Arinc615a::Information::TargetsHardware &targetsHardware )
{
  targetsHardwareModelV->targetsHardware( targetsHardware );
  ui->targetHardwares->resizeColumnsToContents();
  ui->targetHardwares->selectRow( 0 );
}

void InformationOperationCompletedPage::selectTargetHardware( const QModelIndex &index )
{
  partNumbersModelV->partNumbers( targetsHardwareModelV->targetHardware( index ).partNumbers );
  ui->partNumbers->resizeColumnsToContents();
}

void InformationOperationCompletedPage::exportTargetsHardware( const QString &file )
{
  std::ofstream fileStream{ file.toStdString() };

  for ( const auto &targetHardware : targetsHardwareModelV->targetsHardware() )
  {
    fileStream << targetHardware.toString();
  }

  QSettings settings{};
  settings.setValue( "LastTargetsHardwareListDirectory", selectExportFileDialogV->directory().path() );
}

void InformationOperationCompletedPage::copyTargetsHardware()
{
  std::string targetsInformation{};

  for ( const auto &targetHardware : targetsHardwareModelV->targetsHardware() )
  {
    targetsInformation += targetHardware.toString();
  }

  QApplication::clipboard()->setText( QString::fromStdString( targetsInformation ) );
}

}
