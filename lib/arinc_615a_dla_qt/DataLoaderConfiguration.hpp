// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::DataLoaderConfiguration.
 **/

#ifndef ARINC_615A_DLA_QT_DATALOADERCONFIGURATION_HPP
#define ARINC_615A_DLA_QT_DATALOADERCONFIGURATION_HPP

#include <arinc_615a_dla_qt/Arinc615aDlaQt.hpp>

#include <arinc_615a/find/FindConfiguration.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <filesystem>

namespace Arinc615aDlaQt {

/**
 * @brief Data Loader Configuration.
 **/
struct ARINC_615A_DLA_QT_EXPORT DataLoaderConfiguration
{
  //! Default Value for Dynamic FIND Port
  static constexpr bool DynamicFindPortDefault{ true };
  //! Default Value for ARINC 615A Port Option
  static constexpr bool PortOptionDefault{ false };

  /**
   * @brief Creates a Data Loader configuration with default values.
   **/
  DataLoaderConfiguration();

  /**
   * @brief Creates a Data Loader configuration with the values given by the property tree.
   *
   * Values not set by the property tree are filled with default values.
   *
   * @param[in] properties
   *   Configuration as property tree.
   **/
  explicit DataLoaderConfiguration( const boost::property_tree::ptree &properties );

  /**
   * @brief Load Configuration from a given Property Tree.
   *
   * @param[in] properties
   *   Configuration as Property Tree
   **/
  void fromProperties( const boost::property_tree::ptree &properties );

  /**
   * @brief Returns the configuration as a property tree.
   *
   * @param[in] full
   *   If set to true, all options are added to the property tree, even if defaulted.
   *
   * @return Configuration as a property tree.
   **/
  [[nodiscard]] boost::property_tree::ptree toProperties( bool full = false ) const;

  /**
   * @name ARINC 615A FIND Protocol
   * @{
   **/

  //! ARINC 615A FIND Configuration
  Arinc615a::Find::FindConfiguration findConfiguration;
  //! Default FIND Operation Timeout
  std::chrono::seconds findTimeout{ Arinc615a::Find::DefaultReceiveTimeout };
  //! Use Port Option
  bool dynamicFindPort{ DynamicFindPortDefault };

  /** @} **/

  /**
   * @name ARINC 615A Protocol (Except ARINC 615A FIND)
   * @{
   **/

  //! ARINC 615A Data Loader Configuration
  Arinc615a::Arinc615aConfiguration dataLoaderConfiguration;
  //! DLP Timeout
  std::chrono::seconds dlpTimeout{ Arinc615a::DefaultArinc615aDlpTimeout };
  //! Use Port Option
  bool portOption{ PortOptionDefault };

  /** @} **/

  //! ARINC 665 Media Set Manager Configuration Directory
  std::filesystem::path mediaSetConfigurationDirectory;

  //! Base Directory for ARINC 615A Download Operations
  std::filesystem::path downloadBaseDirectory;
  //! Check Validity of Downloaded Files
  bool verifyDownloadFiles{ true };

};

}

#endif
