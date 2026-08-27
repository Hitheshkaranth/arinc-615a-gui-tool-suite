// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::FindQueryResultsPage.
 **/

#include "FindQueryResultsPage.hpp"

#include "ui_FindQueryResultsPage.h"

#include <arinc_615a_qt/TargetsInformationModel.hpp>

namespace Arinc615aDlaQt {

FindQueryResultsPage::FindQueryResultsPage( QWizard * const parent ):
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::FindQueryResultsPage >() }
{
  ui->setupUi( this );
  registerField( "AddToTargetList", ui->addToTargetsList );
}

FindQueryResultsPage::~FindQueryResultsPage() = default;

void FindQueryResultsPage::initializePage()
{
  // add custom button -> Abort Operation
  wizard()->setOption( QWizard::HaveCustomButton1, true );
  // disable default cancel button
  wizard()->setOption( QWizard::NoCancelButton, true );

  connect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &FindQueryResultsPage::abortQuery );

  emit startQuery();
}

bool FindQueryResultsPage::isComplete() const
{
  return isCompleted;
}

void FindQueryResultsPage::targetsInformationModel( Arinc615aQt::TargetsInformationModel * const model )
{
  ui->targets->setModel( model );

  connect(
    model,
    &Arinc615aQt::TargetsInformationModel::dataChanged,
    ui->targets,
    &QTableView::resizeColumnsToContents );
}

void FindQueryResultsPage::queryFinished()
{
  disconnect(
    wizard()->button( QWizard::CustomButton1 ),
    &QAbstractButton::clicked,
    this,
    &FindQueryResultsPage::abortQuery );

  wizard()->setOption( QWizard::HaveCustomButton1, false );

  isCompleted = true;

  emit completeChanged();
}

}
