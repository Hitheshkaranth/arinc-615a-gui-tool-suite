# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

# skip app when arinc_615a_dla_qt is not available
if( NOT TARGET arinc_615a_dla_qt )
  return()
endif()

add_executable( arinc_615a_data_loader_gui WIN32 )

target_sources(
  arinc_615a_data_loader_gui

  PRIVATE
    arinc_615a_data_loader_gui.cpp )

target_compile_features( arinc_615a_data_loader_gui PRIVATE cxx_std_23 )

target_compile_options(
  arinc_615a_data_loader_gui

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
  arinc_615a_data_loader_gui

  PRIVATE
    qt_icon_resources
    arinc_615a_dla_qt
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

install(
  TARGETS arinc_615a_data_loader_gui
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  COMPONENT runtime )
