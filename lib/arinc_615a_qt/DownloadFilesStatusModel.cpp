// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::DownloadFilesStatusModel.
 **/

#include "DownloadFilesStatusModel.hpp"

#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc615aQt {

DownloadFilesStatusModel::DownloadFilesStatusModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

DownloadFilesStatusModel::~DownloadFilesStatusModel() = default;

int DownloadFilesStatusModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( filesStatusV.size() );
}

int DownloadFilesStatusModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant DownloadFilesStatusModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), filesStatusV.size() ) )
  {
    return {};
  }

  auto fileStatus{ std::next( filesStatusV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( Columns{ index.column() } )
      {
        case Columns::Filename:
          return HelperQt::toQString( fileStatus->filename() );

        case Columns::Status:
          return HelperQt::toQString(
            Arinc615a::status( Arinc615a::OperationClass::Download, fileStatus->code(), fileStatus->description() ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::BackgroundRole:
      return Arinc615aQt::StatusCode_color( fileStatus->code() );

    case Qt::ItemDataRole::ForegroundRole:
      return QBrush{ Qt::black, Qt::SolidPattern };

    case Qt::ItemDataRole::ToolTipRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( Columns{ index.column() } )
      {
        case Columns::Status:
          return HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( fileStatus->code() ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant DownloadFilesStatusModel::headerData(
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
    case Columns::Filename:
      return QString{ tr( "Filename" ) };

    case Columns::Status:
      return QString{ tr( "Status" ) };

    default:
      return {};
  }
}

void DownloadFilesStatusModel::status( Arinc615a::Information::DownloadFilesStatus status )
{
  beginResetModel();
  filesStatusV = std::move( status );
  endResetModel();
}

}
