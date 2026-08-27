// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::InformationOperationSettingsPage.
 **/

#include "InformationOperationSettingsPage.hpp"

#include "ui_InformationOperationSettingsPage.h"

#include <arinc_615a/find/TargetInformation.hpp>

namespace Arinc615aDlaQt {

InformationOperationSettingsPage::InformationOperationSettingsPage( QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::InformationOperationSettingsPage >() }
{
  uiV->setupUi( this );
  setCommitPage( true );

  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &InformationOperationSettingsPage::targetSelected );
  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &InformationOperationSettingsPage::completeChanged );
}

InformationOperationSettingsPage::~InformationOperationSettingsPage() = default;

bool InformationOperationSettingsPage::isComplete() const
{
  return QWizardPage::isComplete() && uiV->targetSettings->completed();
}

void InformationOperationSettingsPage::targets( const Arinc615a::Find::TargetsAddressInformation &information )
{
  uiV->targetSettings->targets( information );
}

}
