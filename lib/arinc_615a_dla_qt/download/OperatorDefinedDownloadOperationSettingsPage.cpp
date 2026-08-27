// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::OperatorDefinedDownloadOperationSettingsPage.
 **/

#include "OperatorDefinedDownloadOperationSettingsPage.hpp"

#include "ui_OperatorDefinedDownloadOperationSettingsPage.h"

#include <arinc_615a_qt/DownloadFilesModel.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <boost/exception/all.hpp>

namespace Arinc615aDlaQt {

OperatorDefinedDownloadOperationSettingsPage::OperatorDefinedDownloadOperationSettingsPage(
  QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::OperatorDefinedDownloadOperationSettingsPage>() }
{
  uiV->setupUi( this );
  setCommitPage( true );

  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &OperatorDefinedDownloadOperationSettingsPage::targetSelected );
  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &OperatorDefinedDownloadOperationSettingsPage::completeChanged );

  connect(
    uiV->downloadBaseDirectory,
    &DownloadOperationSelectDownloadDirectoryWidget::downloadBaseDirectorySelected,
    this,
    &OperatorDefinedDownloadOperationSettingsPage::downloadBaseDirectorySelected );
  connect(
    uiV->downloadBaseDirectory,
    &DownloadOperationSelectDownloadDirectoryWidget::downloadBaseDirectorySelected,
    this,
    &OperatorDefinedDownloadOperationSettingsPage::completeChanged );

  connect(
    uiV->downloadBaseDirectory,
    &DownloadOperationSelectDownloadDirectoryWidget::createDownloadDirectoryChanged,
    this,
    &OperatorDefinedDownloadOperationSettingsPage::createDownloadDirectoryChanged );
}

OperatorDefinedDownloadOperationSettingsPage::~OperatorDefinedDownloadOperationSettingsPage() = default;

bool OperatorDefinedDownloadOperationSettingsPage::isComplete() const
{
  return QWizardPage::isComplete() && uiV->targetSettings->completed() && uiV->downloadBaseDirectory->completed();
}

void OperatorDefinedDownloadOperationSettingsPage::targets(
  const Arinc615a::Find::TargetsAddressInformation &information )
{
  uiV->targetSettings->targets( information );
}

void OperatorDefinedDownloadOperationSettingsPage::downloadBaseDirectory( const std::filesystem::path &directory )
{
  uiV->downloadBaseDirectory->downloadBaseDirectory( directory );
}

void OperatorDefinedDownloadOperationSettingsPage::createDownloadDirectory( const bool createDownloadDirectory )
{
  uiV->downloadBaseDirectory->createDownloadDirectory( createDownloadDirectory );
}

}
