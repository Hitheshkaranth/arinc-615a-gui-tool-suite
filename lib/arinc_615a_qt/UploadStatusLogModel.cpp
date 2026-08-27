// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::UploadStatusLogModel.
 **/

#include "UploadStatusLogModel.hpp"

#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc615aQt {

UploadStatusLogModel::UploadStatusLogModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

UploadStatusLogModel::~UploadStatusLogModel() = default;

int UploadStatusLogModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( statusLogV.size() );
}

int UploadStatusLogModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return std::to_underlying( Columns::ColumnsCount );
}

QVariant UploadStatusLogModel::data( const QModelIndex &index, const int role ) const
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

  const auto status{ std::next( statusLogV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( Columns{ index.column() } )
      {
        case Columns::Counter:
          return status->counter();

        case Columns::Status:
          return HelperQt::toQString(
            Arinc615a::status( Arinc615a::OperationClass::Upload, status->code(), status->description() ) );

        case Columns::ListRatio:
          return QString{ "%1 %" }.arg( static_cast< unsigned int >( status->listRatio() ), 3 );

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
      if ( Columns{ index.column() } == Columns::ListRatio )
      {
        return static_cast< int >( Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter );
      }
      return {};

    default:
      return {};
  }
}

QVariant UploadStatusLogModel::headerData(
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

    case Columns::ListRatio:
      return QString{ tr( "List Ratio" ) };

    default:
      return {};
  }
}

const UploadStatusLogModel::UploadStatusLog& UploadStatusLogModel::statusLog() const
{
  return statusLogV;
}

Arinc615a::Information::UploadStatus UploadStatusLogModel::status( const QModelIndex &index ) const
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

void UploadStatusLogModel::status( const Arinc615a::Information::UploadStatus &status )
{
  beginInsertRows( QModelIndex{}, static_cast< int >( statusLogV.size() ), static_cast< int >( statusLogV.size() ) );
  statusLogV.push_back( status );
  endInsertRows();
}

}
