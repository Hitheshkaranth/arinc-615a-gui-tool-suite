// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::TargetListDialog.
 **/

#include "TargetListDialog.hpp"

#include "ui_TargetListDialog.h"

#include <arinc_615a_qt/TargetDetailsDialog.hpp>
#include <arinc_615a_qt/TargetsInformationModel.hpp>

namespace Arinc615aQt {

TargetListDialog::TargetListDialog( QWidget * const parent ):
  QDialog{ parent },
  uiV{ std::make_unique< Ui::TargetListDialog >() },
  targetsInformationModelV{ std::make_unique< Arinc615aQt::TargetsInformationModel >( this ) }
{
  uiV->setupUi( this );

  uiV->targets->setModel( targetsInformationModelV.get() );
  uiV->targets->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::ResizeToContents );

  connect( uiV->addTarget, &QPushButton::clicked, this, &TargetListDialog::add );
  connect(
    uiV->editTarget,
    &QPushButton::clicked,
    this, &TargetListDialog::edit );
  connect(
    uiV->removeTarget,
    &QPushButton::clicked,
    this,
    &TargetListDialog::remove );
  connect(
    uiV->targets,
    &QTableView::doubleClicked,
    this,
    &TargetListDialog::edit );
}

TargetListDialog::~TargetListDialog() = default;

const Arinc615a::Find::TargetsAddressInformation& TargetListDialog::targets() const
{
  return targetsInformationModelV->targetsInformation();
}

void TargetListDialog::targets( Arinc615a::Find::TargetsAddressInformation targets )
{
  targetsInformationModelV->targetsInformation( std::move( targets ) );
}

void TargetListDialog::add()
{
  TargetDetailsDialog dialog( {}, this );
  if ( DialogCode::Accepted == dialog.exec() )
  {
    targetsInformationModelV->targetInformation( dialog.information() );
  }
}

void TargetListDialog::edit()
{
  auto targetInformation{ targetsInformationModelV->targetInformation( uiV->targets->currentIndex() ) };

  TargetDetailsDialog dialog{ targetInformation, this };
  if ( DialogCode::Accepted == dialog.exec() )
  {
    targetsInformationModelV->targetInformation( uiV->targets->currentIndex(), dialog.information() );
  }

  dialog.open();
}

void TargetListDialog::remove()
{
  const auto selected{ uiV->targets->currentIndex() };
  targetsInformationModelV->removeRow( selected.row() );
}

}
