// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::FindPacketStatisticModel.
 **/

#include "FindPacketStatisticModel.hpp"

#include <arinc_615a/find/packets/OpcodeDescription.hpp>

#include <helper_qt/String.hpp>

#include <utility>

namespace Arinc615aQt {

FindPacketStatisticModel::FindPacketStatisticModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

int FindPacketStatisticModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( statisticV.size() );
}

int FindPacketStatisticModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant FindPacketStatisticModel::data( const QModelIndex &index, const int role ) const
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
        case Columns::PacketType:
          return HelperQt::toQString( Arinc615a::Find::Packets::OpcodeDescription::instance().name( packet->first ) );

        case Columns::PacketCount:
          return QString::number( std::get< 0 >( packet->second ) );

        case Columns::PacketSize:
          return QString::number( std::get< 1 >( packet->second ) );

        default:
          return {};
      }

    default:
      return {};
  }
}
QVariant FindPacketStatisticModel::headerData(
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
    case Columns::PacketType:
      return tr( "Packet Type" );

    case Columns::PacketCount:
      return tr( "Packet Count" );

    case Columns::PacketSize:
      return tr( "Packet Size" );

    default:
      return {};
  }
}

void FindPacketStatisticModel::statistic( Arinc615a::Find::Packets::PacketStatistic::Statistic statistic )
{
  beginResetModel();
  statisticV = std::move( statistic );
  endResetModel();
}

}
