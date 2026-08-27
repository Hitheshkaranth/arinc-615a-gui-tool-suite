# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

# ARINC 615A protocol core - shared with the command-line tool suite.
add_subdirectory( arinc_615a )

# Graphical section: Qt models/dialogues, then the data loader application library.
add_subdirectory( arinc_615a_qt )
add_subdirectory( arinc_615a_dla_qt )
