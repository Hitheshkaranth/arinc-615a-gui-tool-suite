// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::TargetsHardwareModel.
 **/

#include "TargetsHardwareModel.hpp"

#include <arinc_615a/information/PartNumber.hpp>

#include <helper_qt/String.hpp>

#include <utility>

namespace Arinc615aQt {

TargetsHardwareModel::TargetsHardwareModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

TargetsHardwareModel::~TargetsHardwareModel() = default;

int TargetsHardwareModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( targetsHardwareV.size() );
}

int TargetsHardwareModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int>( Columns::ColumnsCount );
}

QVariant TargetsHardwareModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), targetsHardwareV.size() ) )
  {
    return {};
  }

  const auto targetHardware{ std::next( targetsHardwareV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::LiteralName:
          return HelperQt::toQString( targetHardware->literalName );

        case Columns::SerialNumber:
          return HelperQt::toQString( targetHardware->serialNumber );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant TargetsHardwareModel::headerData(
  const int section,
  const ::Qt::Orientation orientation,
  const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  switch ( static_cast< Columns>( section ) )
  {
    case Columns::LiteralName:
      return QString{ "Literal Name" };

    case Columns::SerialNumber:
      return QString{ "Serial Number" };

    default:
      return QVariant{};
  }
}

const Arinc615a::Information::TargetsHardware& TargetsHardwareModel::targetsHardware() const
{
  return targetsHardwareV;
}

void TargetsHardwareModel::targetsHardware( Arinc615a::Information::TargetsHardware targetsHardware )
{
  beginResetModel();
  targetsHardwareV = std::move( targetsHardware );
  endResetModel();
}

Arinc615a::Information::TargetHardware TargetsHardwareModel::targetHardware( const QModelIndex &index )
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( ( index.row() < 0 )
    || std::cmp_greater_equal( index.row(), targetsHardwareV.size() ) )
  {
    return {};
  }

  const auto targetHardware{ std::next( targetsHardwareV.begin(), index.row() ) };

  return *targetHardware;
}

}
