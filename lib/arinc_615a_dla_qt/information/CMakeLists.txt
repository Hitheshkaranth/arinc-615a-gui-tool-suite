# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

target_sources(
  arinc_615a_dla_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        Information.hpp
        InformationOperationCompletedPage.hpp
        InformationOperationSettingsPage.hpp
        InformationOperationStatusPage.hpp
        InformationOperationWizard.hpp

  PRIVATE
    InformationOperationCompletedPage.cpp
    InformationOperationCompletedPage.ui
    InformationOperationSettingsPage.cpp
    InformationOperationSettingsPage.ui
    InformationOperationStatusPage.cpp
    InformationOperationStatusPage.ui
    InformationOperationWizard.cpp
    InformationOperationWizard.ui )
