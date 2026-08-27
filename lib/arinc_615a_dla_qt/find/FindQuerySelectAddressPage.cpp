// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::FindQuerySelectAddressPage.
 **/

#include "FindQuerySelectAddressPage.hpp"

#include "ui_FindQuerySelectAddressPage.h"

#include <QNetworkInterface>

#include <boost/asio/ip/address.hpp>

namespace Arinc615aDlaQt {

FindQuerySelectAddressPage::FindQuerySelectAddressPage(
  QWizard * const parent ):
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::FindQuerySelectAddressPage >() }
{
  ui->setupUi( this );
  // add global broadcast and interface broadcast addresses to combobox
  ui->targetAddress->addItem( QHostAddress{ QHostAddress::SpecialAddress::Broadcast }.toString() );
  for ( const auto &networkInterface : QNetworkInterface::allInterfaces() )
  {
    for ( const auto &addressEntries : networkInterface.addressEntries() )
    {
      if ( addressEntries.broadcast().protocol() == QAbstractSocket::IPv4Protocol )
      {
        ui->targetAddress->addItem( addressEntries.broadcast().toString() );
      }
    }
  }

  connect(
    ui->targetAddress,
    &QComboBox::currentTextChanged,
    this,
    &QWizardPage::completeChanged );

  ui->targetAddress->setCurrentIndex( 0 );
}

FindQuerySelectAddressPage::~FindQuerySelectAddressPage() = default;

bool FindQuerySelectAddressPage::isComplete() const
{
  if ( !QWizardPage::isComplete() || ui->targetAddress->currentText().isEmpty() )
  {
    return false;
  }

  boost::system::error_code errorCode{};
  auto targetAddress{ boost::asio::ip::make_address_v4( ui->targetAddress->currentText().toStdString(), errorCode ) };

  if ( errorCode )
  {
    return false;
  }

  emit targetAddressChanged( targetAddress );

  return true;
}

}
