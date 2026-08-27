// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::DownloadFilesModel.
 **/

#include "DownloadFilesModel.hpp"

#include <helper_qt/String.hpp>

namespace Arinc615aQt {

DownloadFilesModel::DownloadFilesModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

DownloadFilesModel::~DownloadFilesModel() = default;

int DownloadFilesModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( filesV.size() );
}

int DownloadFilesModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant DownloadFilesModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), filesV.size() ) )
  {
    return {};
  }

  auto file{ std::next( filesV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::Filename:
          return QString::fromStdString( *file );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant DownloadFilesModel::headerData( const int section, const Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( Columns{ section } )
  {
    case Columns::Filename:
      return QString{ tr( "Filename" ) };

    default:
      return {};
  }
}

const Arinc615a::Information::DownloadFiles& DownloadFilesModel::files() const
{
  return filesV;
}

void DownloadFilesModel::files( Arinc615a::Information::DownloadFiles files )
{
  beginResetModel();
  filesV = std::move( files );
  endResetModel();
}

void DownloadFilesModel::file( std::string_view filename )
{
  beginInsertRows( QModelIndex{}, static_cast< int >( filesV.size() ), static_cast< int >( filesV.size() ) );
  filesV.emplace_back( filename );
  endInsertRows();
}

void DownloadFilesModel::file( std::string filename )
{
  beginInsertRows( QModelIndex{}, static_cast< int >( filesV.size() ), static_cast< int >( filesV.size() ) );
  filesV.emplace_back( std::move( filename ) );
  endInsertRows();
}

void DownloadFilesModel::remove( const QModelIndex &index )
{
  // out of range access
  if ( !index.isValid() || std::cmp_greater_equal( index.row(), filesV.size() ) )
  {
    return;
  }

  beginRemoveRows( QModelIndex{}, index.row(), index.row() );
  filesV.erase( std::next( filesV.begin(), index.row() ) );
  endRemoveRows();
}

void DownloadFilesModel::up( const QModelIndex &index )
{
  // out of range access
  if ( !index.isValid()
    || std::cmp_greater_equal( index.row(), filesV.size() )
    || ( index.row() == 0 ) )
  {
    return;
  }

  filesV.splice( std::next( filesV.begin(), index.row() - 1 ), filesV, std::next( filesV.begin(), index.row() ) );
}

void DownloadFilesModel::down( const QModelIndex &index )
{
  // out of range access
  if ( !index.isValid() || std::cmp_greater_equal( index.row(), filesV.size() - 1 ) )
  {
    return;
  }

  filesV.splice( std::next( filesV.begin(), index.row() ), filesV, std::next( filesV.begin(), index.row() + 1 ) );
}

}
