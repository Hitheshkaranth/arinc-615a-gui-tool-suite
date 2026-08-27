// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::TargetDetailsDialog.
 **/

#include "TargetDetailsDialog.hpp"

#include "ui_TargetDetailsDialog.h"

#include <helper_qt/String.hpp>

namespace Arinc615aQt {

TargetDetailsDialog::TargetDetailsDialog(
  Arinc615a::Find::TargetAddressInformation information,
  QWidget * const parent ) :
  QDialog{ parent },
  ui{ std::make_unique< Ui::TargetDetailsDialog >() },
  informationV{ std::move( information ) }
{
  ui->setupUi( this );

  ui->targetAddress->setText( QString::fromStdString( std::get< 0 >( informationV ).to_string() ) );
  ui->thwId->setText( QString::fromStdString( std::get< 1 >( informationV ).thwId ) );
  ui->typeName->setText( QString::fromStdString( std::get< 1 >( informationV ).thwTypeName ) );
  ui->position->setText( QString::fromStdString( std::get< 1 >( informationV ).thwPosition ) );
  ui->literalName->setText( QString::fromStdString( std::get< 1 >( informationV ).literalName ) );
  ui->manufacturerCode->setText( QString::fromStdString( std::get< 1 >( informationV ).manufacturerCode ) );

  connect(
    ui->targetAddress,
    &QLineEdit::textChanged,
    this,
    &TargetDetailsDialog::textUpdated );
  connect(
    ui->thwId,
    &QLineEdit::textChanged,
    this,
    &TargetDetailsDialog::textUpdated );
  connect(
    ui->typeName,
    &QLineEdit::textChanged,
    this,
    &TargetDetailsDialog::textUpdated );
  connect(
    ui->position,
    &QLineEdit::textChanged,
    this,
    &TargetDetailsDialog::textUpdated );
  connect(
    ui->literalName,
    &QLineEdit::textChanged,
    this,
    &TargetDetailsDialog::textUpdated );
  connect(
    ui->manufacturerCode,
    &QLineEdit::textChanged,
    this,
    &TargetDetailsDialog::textUpdated );
}

TargetDetailsDialog::~TargetDetailsDialog() = default;

const Arinc615a::Find::TargetAddressInformation& TargetDetailsDialog::information() const
{
  return informationV;
}

void TargetDetailsDialog::textUpdated()
{
  boost::system::error_code errorCode{};
  auto address{ boost::asio::ip::make_address( ui->targetAddress->text().toStdString(), errorCode ) };
  if ( !errorCode )
  {
    std::get< 0 >( informationV ) = std::move( address );
  }

  std::get< 1 >( informationV ).thwId = ui->thwId->text().toStdString();
  std::get< 1 >( informationV ).thwTypeName = ui->typeName->text().toStdString();
  std::get< 1 >( informationV ).thwPosition = ui->position->text().toStdString();
  std::get< 1 >( informationV ).literalName = ui->literalName->text().toStdString();
  std::get< 1 >( informationV ).manufacturerCode = ui->manufacturerCode->text().toStdString();
}

}
