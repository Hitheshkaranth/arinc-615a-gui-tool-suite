// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::PartNumbersModel.
 **/

#include "PartNumbersModel.hpp"

#include <helper_qt/String.hpp>

#include <utility>

namespace Arinc615aQt {

PartNumbersModel::PartNumbersModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

int PartNumbersModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( partNumbersV.size() );
}

int PartNumbersModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int>( Columns::ColumnsCount );
}

QVariant PartNumbersModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), partNumbersV.size() ) )
  {
    return {};
  }

  const auto targetHardware{ std::next( partNumbersV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::PartNumber:
          return HelperQt::toQString( targetHardware->partNumber );

        case Columns::PartDesignation:
          return HelperQt::toQString( targetHardware->partDesignation );

        case Columns::Amendment:
          return HelperQt::toQString( targetHardware->amendment );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant PartNumbersModel::headerData( const int section, const ::Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  switch ( Columns{ section } )
  {
    case Columns::PartNumber:
      return QString{ tr( "Part Number" ) };

    case Columns::PartDesignation:
      return QString{ tr( "Part Designation" ) };

    case Columns::Amendment:
      return QString{ tr( "Amendment" ) };

    default:
      return QVariant{};
  }
}

const Arinc615a::Information::PartNumbers& PartNumbersModel::partNumbers() const
{
  return partNumbersV;
}

void PartNumbersModel::partNumbers( Arinc615a::Information::PartNumbers partNumbers )
{
  beginResetModel();
  partNumbersV = std::move( partNumbers );
  endResetModel();
}

}
