# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

target_sources(
  arinc_615a_dla_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        Download.hpp
        DownloadOperationCompletedPage.hpp
        DownloadOperationSelectDownloadDirectoryWidget.hpp
        DownloadOperationStatusPage.hpp
        MediaDefinedDownloadOperationSettingsPage.hpp
        MediaDefinedDownloadOperationWizard.hpp
        OperatorDefinedDownloadOperationSelectFilesPage.hpp
        OperatorDefinedDownloadOperationSettingsPage.hpp
        OperatorDefinedDownloadOperationWizard.hpp

  PRIVATE
    DownloadOperationCompletedPage.cpp
    DownloadOperationCompletedPage.ui
    DownloadOperationSelectDownloadDirectoryWidget.cpp
    DownloadOperationSelectDownloadDirectoryWidget.ui
    DownloadOperationStatusPage.cpp
    DownloadOperationStatusPage.ui
    MediaDefinedDownloadOperationSettingsPage.cpp
    MediaDefinedDownloadOperationSettingsPage.ui
    MediaDefinedDownloadOperationWizard.cpp
    MediaDefinedDownloadOperationWizard.ui
    OperatorDefinedDownloadOperationSelectFilesPage.cpp
    OperatorDefinedDownloadOperationSelectFilesPage.ui
    OperatorDefinedDownloadOperationSettingsPage.cpp
    OperatorDefinedDownloadOperationSettingsPage.ui
    OperatorDefinedDownloadOperationWizard.cpp
    OperatorDefinedDownloadOperationWizard.ui )
