// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc615aQt.
 **/

#include "StatusCode.hpp"

namespace Arinc615aQt {

QBrush StatusCode_color( const Arinc615a::StatusCode code )
{
  switch ( code )
  {
    using enum Arinc615a::StatusCode;

    case OperationAccepted:
    case OperationInProgress:
    case OperationInProgressAdditionalInfo:
    case OperationDeferred:
      return QBrush{ Qt::yellow, Qt::SolidPattern };

    case OperationNotAccepted:
    case OperationNotSupported:
    case OperationAbortedByTargetHw:
    case OperationAbortedByDlp:
    case OperationAbortedByOperator:
    case LoadPartNumberOrDownloadFileFailed:
      return QBrush{ Qt::red, Qt::SolidPattern };

    case OperationCompleted:
      return QBrush{ Qt::green, Qt::SolidPattern };

    default:
      return QBrush{ Qt::transparent, Qt::SolidPattern };
  }
}

}
