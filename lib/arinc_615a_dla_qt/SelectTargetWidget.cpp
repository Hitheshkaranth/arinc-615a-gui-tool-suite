// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::SelectTargetWidget.
 **/

#include "SelectTargetWidget.hpp"

#include "ui_SelectTargetWidget.h"

#include <arinc_615a_qt/TargetsInformationModel.hpp>

#include <QModelIndex>

namespace Arinc615aDlaQt {

SelectTargetWidget::SelectTargetWidget(
  QWidget * const parent ):
  QWidget{ parent },
  uiV{ std::make_unique< Ui::SelectTargetWidget >() },
  targetsInformationModelV{ std::make_unique< Arinc615aQt::TargetsInformationModel >( this ) }
{
  uiV->setupUi( this );

  uiV->targets->setModel( targetsInformationModelV.get() );
  uiV->targets->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  using enum Arinc615aQt::TargetsInformationModel::Columns;
  for ( auto column :
        { ThwId,
          ThwTypeName,
          ThwPosition,
          ManufacturerCode } )
  {
    uiV->targets->hideColumn( static_cast< int >( column ) );
  }

  connect(
    uiV->targets,
    &QTableView::doubleClicked,
    this,
    &SelectTargetWidget::updateTarget );

  // TODO check if editingFinished or textChanged should be used
  connect(
    uiV->targetAddress,
    &QLineEdit::textEdited,
    this,
    &SelectTargetWidget::updateTargetAddress );
  connect(
    uiV->targetId,
    &QLineEdit::textEdited,
    this,
    &SelectTargetWidget::updateTargetId );
}

SelectTargetWidget::~SelectTargetWidget() = default;

bool SelectTargetWidget::completed() const
{
  return !targetAddressV.is_unspecified() && targetIdV;
}

void SelectTargetWidget::targets( Arinc615a::Find::TargetsAddressInformation information )
{
  targetsInformationModelV->targetsInformation( std::move( information ) );
}

void SelectTargetWidget::updateTarget( const QModelIndex &index )
{
  // get selected target information
  const auto [ targetAddress, targetInformation ]{ targetsInformationModelV->targetInformation( index ) };

  // update Target Address and ID Fields
  uiV->targetAddress->setText( QString::fromStdString( targetAddress.to_string() ) );
  uiV->targetId->setText( QString::fromStdString( targetInformation.targetId().toString() ) );

  targetAddressV = targetAddress;
  targetIdV = targetInformation.targetId();

  emit targetSelected( targetAddressV, targetIdV );
}

void SelectTargetWidget::updateTargetAddress( const QString &text )
{
  targetAddressV = boost::asio::ip::address{};

  boost::system::error_code errorCode{};
  auto address{ boost::asio::ip::make_address_v4( text.toStdString(), errorCode ) };
  if ( !errorCode )
  {
    targetAddressV = address;
  }

  emit targetSelected( targetAddressV, targetIdV );
}

void SelectTargetWidget::updateTargetId( const QString &text )
{
  targetIdV = Arinc615a::TargetId{};

  if ( Arinc615a::TargetId::isTargetId( text.toStdString() ) )
  {
    targetIdV = text.toStdString();
  }

  emit targetSelected( targetAddressV, targetIdV );
}

}
