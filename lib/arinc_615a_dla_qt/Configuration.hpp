// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Configuration Handling Utilities.
 **/

#ifndef ARINC_615A_DLA_QT_CONFIGURATION_HPP
#define ARINC_615A_DLA_QT_CONFIGURATION_HPP

#include <arinc_615a_dla_qt/Arinc615aDlaQt.hpp>

#include <arinc_615a/find/Find.hpp>

namespace Arinc615aDlaQt {

/**
 * @brief Loads the Data Loader configuration from a JSON file.
 *
 * This function locates the configuration file defined by DataLoaderConfigFile within the application configuration
 * location.
 * If the file exists and is valid, it reads the file into a property tree and initialises a DataLoaderConfiguration
 * object with these properties.
 * If the file does not exist or is invalid, a DataLoaderConfiguration object with default values is returned.
 *
 * @return DataLoaderConfiguration object initialised with values from the configuration file, or default values if the
 *   file does not exist or is invalid.
 **/
[[nodiscard]] ARINC_615A_DLA_QT_EXPORT DataLoaderConfiguration loadDataLoaderConfiguration();

/**
 * @brief Saves the provided Data Loader configuration to a JSON file.
 *
 * This function serializes the given DataLoaderConfiguration object into a JSON format and writes it to the file
 * specified by the Data Loader Configuration file name in the application's configuration directory.
 *
 * @param[in] configuration
 *   The Data Loader configuration to be saved.
 **/
ARINC_615A_DLA_QT_EXPORT void saveDataLoaderConfiguration( const DataLoaderConfiguration &configuration );

/**
 * @brief Loads the target address information from the application configuration location.
 *
 * This function searches for a file named "Targets.json" in the application configuration location.
 *
 * If the file is found and is a regular file, it loads the JSON content and parses the target address information.
 *
 * @return A list of `TargetAddressInformation` objects containing the target address information extracted from the
 *   JSON file.
 *
 * @note
 * If the target configuration file is not found or is not a regular file, an empty list is returned.
 *
 * @throws Any exceptions thrown by the underlying file operations or JSON parsing are propagated to the caller.
 **/
[[nodiscard]] ARINC_615A_DLA_QT_EXPORT Arinc615a::Find::TargetsAddressInformation loadTargets();

/**
 * @brief Saves the provided target addresses information to a JSON file.
 *
 * This function serializes the given 'TargetsAddressInformation' into a JSON format and writes it to a file named
 * "Targets.json" located in the application configuration directory.
 *
 * @param[in] targets
 *   The list of target address information structures to be saved.
 **/
ARINC_615A_DLA_QT_EXPORT void saveTargets( const Arinc615a::Find::TargetsAddressInformation &targets );

}

#endif
