# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

target_sources(
  arinc_615a_dla_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        FindQuery.hpp
        InformationOperation.hpp
        UploadOperation.hpp
        MediaDefinedDownloadOperation.hpp
        Operations.hpp
        OperatorDefinedDownloadOperation.hpp

  PRIVATE
    FindQuery.cpp
    InformationOperation.cpp
    UploadOperation.cpp
    MediaDefinedDownloadOperation.cpp
    OperatorDefinedDownloadOperation.cpp )
