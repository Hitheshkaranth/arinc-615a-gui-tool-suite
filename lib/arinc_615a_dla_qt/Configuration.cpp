// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Configuration Handling Utilities.
 **/

#include "Configuration.hpp"

#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <QStandardPaths>

namespace Arinc615aDlaQt {

//! Filename of Data Loader configuration file.
static constexpr auto DataLoaderConfigFile{ "DataLoader.json" };

//! Filename of Targets list.
static constexpr auto TargetsFile{ "Targets.json" };

DataLoaderConfiguration loadDataLoaderConfiguration()
{
  DataLoaderConfiguration configuration;

  if (
    const auto configurationFile{ QStandardPaths::locate( QStandardPaths::AppConfigLocation, DataLoaderConfigFile ) };
    !configurationFile.isEmpty() && std::filesystem::is_regular_file( configurationFile.toStdString() ) )
  {
    boost::property_tree::ptree properties;

    boost::property_tree::read_json( configurationFile.toStdString(), properties );

    configuration.fromProperties( properties );
  }

  return configuration;
}

void saveDataLoaderConfiguration( const DataLoaderConfiguration &configuration )
{
  const boost::property_tree::ptree configurationProperties{ configuration.toProperties( true ) };

  boost::property_tree::write_json(
    QString{ "%1/%2" }
      .arg( QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation ), DataLoaderConfigFile )
      .toStdString(),
    configurationProperties );
}

Arinc615a::Find::TargetsAddressInformation loadTargets()
{
  Arinc615a::Find::TargetsAddressInformation targets;

  if (
    const auto targetConfigurationFile{ QStandardPaths::locate( QStandardPaths::AppConfigLocation, TargetsFile ) };
    !targetConfigurationFile.isEmpty() && std::filesystem::is_regular_file( targetConfigurationFile.toStdString() ) )
  {
    boost::property_tree::ptree properties{};

    boost::property_tree::read_json( targetConfigurationFile.toStdString(), properties );

    targets = Arinc615a::Find::TargetInformation::targetsAddressInformation( properties );
  }

  return targets;
}

void saveTargets( const Arinc615a::Find::TargetsAddressInformation &targets )
{
  boost::property_tree::write_json(
    QString{ "%1/%2" }
      .arg( QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation ), TargetsFile )
      .toStdString(),
    Arinc615a::Find::TargetInformation::targetsAddressInformation( targets ) );
}

}
