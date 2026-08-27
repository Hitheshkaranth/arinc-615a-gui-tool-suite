# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

find_package( Qt6 COMPONENTS Widgets Network QUIET )
find_package( spdlog REQUIRED )

if( NOT TARGET Qt::Widgets OR NOT TARGET Qt::Network )
  return()
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

add_library( arinc_615a_dla_qt )

include( GenerateExportHeader )
generate_export_header( arinc_615a_dla_qt )

target_sources(
  arinc_615a_dla_qt

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc615aDlaQt.hpp
        Configuration.hpp
        DataLoaderConfiguration.hpp
        DataLoaderMainWindow.hpp
        SelectTargetWidget.hpp
        SettingsDialog.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_dla_qt_export.h

  PRIVATE
    Arinc615aDlaQt.cpp
    Configuration.cpp
    DataLoaderConfiguration.cpp
    DataLoaderMainWindow.cpp
    DataLoaderMainWindow.ui
    SelectTargetWidget.cpp
    SelectTargetWidget.ui
    SettingsDialog.cpp
    SettingsDialog.ui )

target_compile_features( arinc_615a_dla_qt PUBLIC cxx_std_23 )

target_compile_options(
  arinc_615a_dla_qt

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>
    # Disable Warning for exporting classes with std::* private members
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    # Disable Warning for exporting classes, which derives from std::*
    $<$<CXX_COMPILER_ID:MSVC>:/wd4275>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_615a_dla_qt

  PUBLIC
    arinc_615a
    arinc_665_qt
    Qt::Widgets
    Qt::Network

  PRIVATE
    spdlog::spdlog_header_only
    helper_qt
    qt_icon_resources
    tftp_qt
    arinc_615a_qt
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

add_subdirectory( operations )
add_subdirectory( find )
add_subdirectory( information )
add_subdirectory( upload )
add_subdirectory( download )
add_subdirectory( resources )

# disable clang tidy in binary dir by placing dummy .clang-tidy file
file(
  WRITE
  ${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy
  "Checks: '-*,llvm-twine-local'" )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_dla_qt_export.h )


install(
  TARGETS arinc_615a_dla_qt
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  EXPORT arinc_615a_dla_qt-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_615a_dla_qt-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
