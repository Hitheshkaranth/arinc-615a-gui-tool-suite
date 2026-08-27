# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

target_sources(
  arinc_615a_dla_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        Upload.hpp
        UploadOperationCompletedPage.hpp
        UploadOperationSettingsPage.hpp
        UploadOperationStatusPage.hpp
        UploadOperationWizard.hpp

  PRIVATE
    UploadOperationCompletedPage.cpp
    UploadOperationCompletedPage.ui
    UploadOperationSettingsPage.cpp
    UploadOperationSettingsPage.ui
    UploadOperationStatusPage.cpp
    UploadOperationStatusPage.ui
    UploadOperationWizard.cpp
    UploadOperationWizard.ui )
