# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

find_package( Qt6 COMPONENTS Widgets QUIET )
find_package( spdlog REQUIRED )

if( NOT TARGET Qt::Widgets )
  return()
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
#set( CMAKE_AUTORCC ON )

add_library( arinc_615a_qt )

include( GenerateExportHeader )
generate_export_header( arinc_615a_qt )

target_sources(
  arinc_615a_qt

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc615aQt.hpp
        DownloadFilesInformationModel.hpp
        DownloadFilesModel.hpp
        DownloadFilesStatusModel.hpp
        DownloadStatusLogModel.hpp
        FindPacketStatisticModel.hpp
        InformationStatusLogModel.hpp
        PartNumbersModel.hpp
        ProtocolFileStatisticModel.hpp
        ProtocolFileStatisticModel.hpp
        StatusCode.hpp
        TargetDetailsDialog.hpp
        TargetListDialog.hpp
        TargetsHardwareModel.hpp
        TargetsInformationModel.hpp
        UploadLoadsStatusModel.hpp
        UploadStatusLogModel.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_qt_export.h

  PRIVATE
    Arinc615aQt.cpp
    DownloadFilesInformationModel.cpp
    DownloadFilesModel.cpp
    DownloadFilesStatusModel.cpp
    DownloadStatusLogModel.cpp
    FindPacketStatisticModel.cpp
    InformationStatusLogModel.cpp
    PartNumbersModel.cpp
    ProtocolFileStatisticModel.cpp
    StatusCode.cpp
    TargetDetailsDialog.cpp
    TargetDetailsDialog.ui
    TargetListDialog.ui
    TargetListDialog.cpp
    TargetsHardwareModel.cpp
    TargetsInformationModel.cpp
    UploadLoadsStatusModel.cpp
    UploadStatusLogModel.cpp )

target_compile_features( arinc_615a_qt PUBLIC cxx_std_23 )

target_compile_options(
  arinc_615a_qt

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>
    # Disable Warning for exporting classes with std::* private members
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_615a_qt

  PUBLIC
    arinc_615a
    Qt::Widgets

  PRIVATE
    spdlog::spdlog_header_only
    helper_qt
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

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
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_qt_export.h )


install(
  TARGETS arinc_615a_qt
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  EXPORT arinc_615a_qt-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_615a_qt-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
