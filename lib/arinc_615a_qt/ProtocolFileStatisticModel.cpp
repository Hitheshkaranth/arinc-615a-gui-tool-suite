// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::ProtocolFileStatisticModel.
 **/

#include "ProtocolFileStatisticModel.hpp"

#include <arinc_615a/files/ProtocolFileTypeDescription.hpp>

#include <helper_qt/String.hpp>

#include <utility>

namespace Arinc615aQt {

ProtocolFileStatisticModel::ProtocolFileStatisticModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

int ProtocolFileStatisticModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( statisticV.size() );
}

int ProtocolFileStatisticModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant ProtocolFileStatisticModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), statisticV.size() ) )
  {
    return {};
  }

  const auto packet{ std::next( statisticV.begin(), index.row() ) };

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns >( index.column() ) )
      {
        case Columns::FileType:
          return HelperQt::toQString( Arinc615a::Files::ProtocolFileTypeDescription::instance().name( packet->first ) );

        case Columns::FileCount:
          return QString::number( packet->second );

        default:
          return {};
      }

    default:
      return {};
  }
}

QVariant ProtocolFileStatisticModel::headerData(
  int const section,
  Qt::Orientation const orientation,
  int const role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  switch ( static_cast< Columns >( section ) )
  {
    case Columns::FileType:
      return tr( "File Type" );

    case Columns::FileCount:
      return tr( "File Count" );

    default:
      return {};
  }
}

void ProtocolFileStatisticModel::statistic( Arinc615a::Files::ProtocolFileStatistic::Statistic statistic )
{
  beginResetModel();
  statisticV = std::move( statistic );
  endResetModel();
}

}
