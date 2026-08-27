// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::InformationStatusLogModel.
 **/

#include "InformationStatusLogModel.hpp"

#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc615aQt {

InformationStatusLogModel::InformationStatusLogModel( QObject * const parent ) : QAbstractTableModel{ parent }
{
}

InformationStatusLogModel::~InformationStatusLogModel() = default;

int InformationStatusLogModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( statusLogV.size() );
}

int InformationStatusLogModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant InformationStatusLogModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), statusLogV.size() ) )
  {
    return {};
  }

  auto status{ std::next( statusLogV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( Columns{ index.column() } )
      {
        case Columns::Counter:
          return status->counter();

        case Columns::Status:
          return HelperQt::toQString(
            Arinc615a::status( Arinc615a::OperationClass::Information, status->code(), status->description() ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::BackgroundRole:
      return Arinc615aQt::StatusCode_color( status->code() );

    case Qt::ItemDataRole::ForegroundRole:
      return QBrush{ Qt::black, Qt::SolidPattern };

    case Qt::ItemDataRole::ToolTipRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( Columns{ index.column() } )
      {
        case Columns::Status:
          return HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( status->code() ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant InformationStatusLogModel::headerData(
  const int section,
  const Qt::Orientation orientation,
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

  switch ( Columns{ section } )
  {
    case Columns::Counter:
      return QString{ "#" };

    case Columns::Status:
      return QString{ tr( "Status" ) };

    default:
      return {};
  }
}

const InformationStatusLogModel::InformationStatusLog& InformationStatusLogModel::statusLog() const
{
  return statusLogV;
}

Arinc615a::Information::InformationStatus InformationStatusLogModel::status( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), statusLogV.size() ) )
  {
    return {};
  }

  return *std::next( statusLogV.begin(), index.row() );
}

void InformationStatusLogModel::status( const Arinc615a::Information::InformationStatus &status )
{
  beginInsertRows( QModelIndex{}, static_cast< int >( statusLogV.size() ), static_cast< int >( statusLogV.size() ) );
  statusLogV.push_back( status );
  endInsertRows();
}

}
