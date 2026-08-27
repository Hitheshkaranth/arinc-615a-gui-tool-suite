// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 615A Data Loader Qt Application.
 **/

#include <arinc_615a_dla_qt/DataLoaderMainWindow.hpp>
#include <arinc_615a_dla_qt/resources/Resources.hpp>

#include <arinc_665_qt/resources/Resources.hpp>

#include <qt_icon_resources/QtIconResources.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <QApplication>
#include <QIcon>
#include <QMessageBox>

#include <boost/exception/diagnostic_information.hpp>

#include <cstdlib>
#include <iostream>

/**
 * @brief Application Entry Point.
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Application exit status.
 **/
int main( int argc, char * argv[] );

int main( int argc, char * argv[] )
try
{
  spdlog::default_logger()->sinks().emplace_back(
    std::make_shared< spdlog::sinks::basic_file_sink_mt >(
      ( std::filesystem::temp_directory_path() / "arinc_615a_data_loader_gui.log" ).string() ) );
  spdlog::set_level( spdlog::level::level_enum::info );

  try
  {
    QtIconResources::initialise();
    Arinc665Qt::Resources::initialise();
    Arinc615aDlaQt::Resources::initialise();

    QApplication application{ argc, argv };
    QApplication::setApplicationDisplayName( QObject::tr( "ARINC 615A Data Loader" ) );
    QApplication::setOrganizationName( "Thomas Vogt" );
    QApplication::setOrganizationDomain( "thomas-vogt.de" );
    QApplication::setWindowIcon( QIcon{ ":/arinc615a_icon.svg" } );

    Arinc615aDlaQt::DataLoaderMainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
  }
  catch ( const boost::exception &e )
  {
    QMessageBox::critical( nullptr, "Error", QString::fromStdString( boost::diagnostic_information( e ) ) );

    SPDLOG_INFO( "Additional information: {}", boost::diagnostic_information( e, true ) );

    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    QMessageBox::critical( nullptr, "Error", QString::fromStdString( boost::diagnostic_information( e ) ) );

    SPDLOG_INFO( "Additional information: {}", boost::diagnostic_information( e, true ) );

    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    QMessageBox::critical( nullptr, "Error", "Unknown Error" );
    return EXIT_FAILURE;
  }
}
catch ( ... )
{
  std::cerr << "Very bad exception\n";
  return EXIT_FAILURE;
}
