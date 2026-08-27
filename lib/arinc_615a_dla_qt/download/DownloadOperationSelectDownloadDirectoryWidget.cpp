
// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::DownloadOperationSelectDownloadDirectoryWidget.
 **/

#include "DownloadOperationSelectDownloadDirectoryWidget.hpp"

#include "ui_DownloadOperationSelectDownloadDirectoryWidget.h"

#include <QAbstractButton>

#include <filesystem>

namespace Arinc615aDlaQt {

DownloadOperationSelectDownloadDirectoryWidget::DownloadOperationSelectDownloadDirectoryWidget(
  QWidget * const parent ) :
  QGroupBox{ parent },
  uiV{ std::make_unique< Ui::DownloadOperationSelectDownloadDirectoryWidget >() },
  selectDirectoryDialogV{ std::make_unique< QFileDialog >( this ) }
{
  uiV->setupUi( this );

  selectDirectoryDialogV->setWindowTitle( tr( "Select Download Base Directory" ) );
  selectDirectoryDialogV->setFileMode( QFileDialog::Directory );
  selectDirectoryDialogV->setOption( QFileDialog::ShowDirsOnly );

  connect(
    uiV->selectDownloadBaseDirectory,
    &QAbstractButton::clicked,
    selectDirectoryDialogV.get(),
    &QFileDialog::show );

  connect(
    selectDirectoryDialogV.get(),
    &QFileDialog::accepted,
    this,
    &DownloadOperationSelectDownloadDirectoryWidget::selectedDirectory );

  connect(
    uiV->createDownloadDirectory,
    &QCheckBox::toggled,
    this,
    &DownloadOperationSelectDownloadDirectoryWidget::createDownloadDirectory );
}

DownloadOperationSelectDownloadDirectoryWidget::~DownloadOperationSelectDownloadDirectoryWidget() = default;

bool DownloadOperationSelectDownloadDirectoryWidget::completed() const
{
  return selectDirectoryDialogV->directory().exists();
}

void DownloadOperationSelectDownloadDirectoryWidget::downloadBaseDirectory( const std::filesystem::path &directory )
{
  auto directoryString{ QString::fromStdString( directory.string() ) };

  selectDirectoryDialogV->setDirectory( directoryString );
  uiV->downloadBaseDirectory->setText( directoryString );

  emit downloadBaseDirectorySelected( directory );
}

void DownloadOperationSelectDownloadDirectoryWidget::createDownloadDirectory( const bool createDownloadDirectory )
{
  uiV->createDownloadDirectory->setChecked( createDownloadDirectory );
}

void DownloadOperationSelectDownloadDirectoryWidget::selectedDirectory()
{
  auto directoryString{ QString::fromStdString( selectDirectoryDialogV->directory().path().toStdString() ) };

  uiV->downloadBaseDirectory->setText( directoryString );

  emit downloadBaseDirectorySelected( directoryString.toStdString() );
}

}
