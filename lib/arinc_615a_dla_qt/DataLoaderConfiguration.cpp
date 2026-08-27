// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aDlaQt::DataLoaderConfiguration.
 **/

#include "DataLoaderConfiguration.hpp"

#include <boost/property_tree/ptree.hpp>

#include <QStandardPaths>

namespace Arinc615aDlaQt {

DataLoaderConfiguration::DataLoaderConfiguration():
  mediaSetConfigurationDirectory(
    QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation )
      .toStdString()
    + "/MediaSetManager" ),
  downloadBaseDirectory(
    QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation )
      .toStdString() )
{
}

DataLoaderConfiguration::DataLoaderConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void DataLoaderConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  // FIND Configuration
  findConfiguration = Arinc615a::Find::FindConfiguration{};
  if (
    const auto findProperties{ properties.get_child_optional( "arinc_615a_find" ) };
    findProperties )
  {
    findConfiguration.fromProperties( *findProperties );
  }
  std::chrono::seconds::rep findTimeoutInt{
    properties.get( "arinc_615a_find.timeout", Arinc615a::Find::DefaultReceiveTimeout.count() ) };
  findTimeout = std::chrono::seconds{ findTimeoutInt };

  dynamicFindPort = properties.get( "arinc_615a_find.dynamic_port", DynamicFindPortDefault );

  // HOST Protocol Configuration
  dataLoaderConfiguration = Arinc615a::Arinc615aConfiguration{};
  if (
    const auto dataLoaderProperties{ properties.get_child_optional( "arinc_615a" ) };
    dataLoaderProperties )
  {
    dataLoaderConfiguration.fromProperties( *dataLoaderProperties );
  }
  std::chrono::seconds::rep dlpTimeoutInt{
    properties.get( "arinc_615a.dlp_timeout", Arinc615a::DefaultArinc615aDlpTimeout.count() ) };
  dlpTimeout = std::chrono::seconds{ dlpTimeoutInt };

  portOption = properties.get( "arinc_615a.port_option", PortOptionDefault );

  mediaSetConfigurationDirectory = properties.get< std::string >(
    "media_set_manager",
    QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation )
      .toStdString()
    + "/MediaSetManager" );

  downloadBaseDirectory = properties.get< std::string >(
    "download_base_directory",
    QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ).toStdString() );

  verifyDownloadFiles = properties.get( "verify_download_files", true );
}

boost::property_tree::ptree DataLoaderConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties{};

  // FIND Configuration
  if (
    const auto findProperties{ findConfiguration.toProperties( full ) };
    full || !findProperties.empty() )
  {
    properties.add_child( "arinc_615a_find", findProperties );
  }

  if ( full || ( findTimeout != Arinc615a::Find::DefaultReceiveTimeout ) )
  {
    properties.add( "arinc_615a_find.timeout", findTimeout.count() );
  }

  if ( full || ( dynamicFindPort != DynamicFindPortDefault ) )
  {
    properties.add( "arinc_615a_find.dynamic_port", dynamicFindPort );
  }

  // HOST Protocol Configuration
  if (
    const auto arinc615aProperties{ dataLoaderConfiguration.toProperties( full ) };
    full || !arinc615aProperties.empty() )
  {
    properties.add_child( "arinc_615a", arinc615aProperties );
  }

  if ( full || ( dlpTimeout != Arinc615a::DefaultArinc615aDlpTimeout ) )
  {
    properties.add( "arinc_615a.dlp_timeout", dlpTimeout.count() );
  }

  if ( full || ( portOption != PortOptionDefault ) )
  {
    properties.add( "arinc_615a.port_option", portOption );
  }

  // Always store Media Set Manger Directory and Download Base Directory
  properties.add( "media_set_manager", mediaSetConfigurationDirectory.string() );
  properties.add( "download_base_directory", downloadBaseDirectory.string() );

  if ( full || ( !verifyDownloadFiles ) )
  {
    properties.add( "verify_download_files", verifyDownloadFiles );
  }

  return properties;
}

}
