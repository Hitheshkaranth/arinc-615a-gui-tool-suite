// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615aQt.
 **/

#ifndef ARINC_615A_QT_STATUSCODE_HPP
#define ARINC_615A_QT_STATUSCODE_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/Arinc615a.hpp>

#include <QBrush>

namespace Arinc615aQt {

/**
 * @brief Return Brush for given Status Code.
 *
 * @param[in] code
 *   Status Code.
 *
 * @return Brush for Status Code.
 **/
QBrush ARINC_615A_QT_EXPORT StatusCode_color( Arinc615a::StatusCode code );

}

#endif
