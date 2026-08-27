// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::UploadOperationSettingsPage.
 **/

#include "UploadOperationSettingsPage.hpp"

#include "ui_UploadOperationSettingsPage.h"

#include <arinc_665_qt/media/MediaSetsModel.hpp>
#include <arinc_665_qt/media/LoadsModel.hpp>

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <helper_qt/String.hpp>

namespace Arinc615aDlaQt {

UploadOperationSettingsPage::UploadOperationSettingsPage( QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::UploadOperationSettingsPage >() },
  mediaSetsModelV{ std::make_unique< Arinc665Qt::Media::MediaSetsModel >( this ) },
  availableLoadsModelV{ std::make_unique< Arinc665Qt::Media::LoadsModel >( this ) }
{
  uiV->setupUi( this );
  setCommitPage( true );

  uiV->mediaSet->setModel( mediaSetsModelV.get() );

  uiV->loads->setModel( availableLoadsModelV.get() );
  uiV->loads->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &UploadOperationSettingsPage::targetSelected );
  connect(
    uiV->targetSettings,
    &SelectTargetWidget::targetSelected,
    this,
    &UploadOperationSettingsPage::completeChanged );

  connect(
    uiV->mediaSet,
    QOverload< int >::of( &QComboBox::currentIndexChanged ),
    this,
    &UploadOperationSettingsPage::updateAvailableLoads );

  connect(
    uiV->loads->selectionModel(),
    &QItemSelectionModel::selectionChanged,
    this,
    &UploadOperationSettingsPage::updateLoads );
}

UploadOperationSettingsPage::~UploadOperationSettingsPage() = default;

bool UploadOperationSettingsPage::isComplete() const
{
  return QWizardPage::isComplete() && uiV->targetSettings->completed()
    && !uiV->loads->selectionModel()->selection().isEmpty();
}

void UploadOperationSettingsPage::targets( const Arinc615a::Find::TargetsAddressInformation &information )
{
  uiV->targetSettings->targets( information );
}

void UploadOperationSettingsPage::mediaSets( Arinc665::Media::ConstMediaSets mediaSets )
{
  mediaSetsModelV->mediaSets( std::move( mediaSets ) );
  uiV->mediaSet->setCurrentIndex( -1 );
  availableLoadsModelV->loads( {} );
}

void UploadOperationSettingsPage::updateAvailableLoads( const int index )
{
  auto const &mediaSet{ mediaSetsModelV->constMediaSet( mediaSetsModelV->mediaSet( index ) ) };

  if ( mediaSet )
  {
    availableLoadsModelV->loads( mediaSet->recursiveLoads() );
  }

  emit mediaSetSelected( mediaSet );
  // reset selected loads
  emit loadsSelected( {} );
  emit completeChanged();
}

void UploadOperationSettingsPage::updateLoads()
{
  Arinc665::Media::ConstLoads loads;
  for ( const auto &row : uiV->loads->selectionModel()->selectedRows() )
  {
    loads.emplace_back( availableLoadsModelV->constLoad( availableLoadsModelV->load( row ) ) );
  }

  emit loadsSelected( loads );
  emit completeChanged();
}

}
