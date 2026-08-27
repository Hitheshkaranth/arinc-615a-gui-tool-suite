// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::DownloadFilesInformationModel.
 **/

#include "DownloadFilesInformationModel.hpp"

#include <helper_qt/String.hpp>

namespace Arinc615aQt {

DownloadFilesInformationModel::DownloadFilesInformationModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

DownloadFilesInformationModel::~DownloadFilesInformationModel() = default;

int DownloadFilesInformationModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( filesV.size() );
}

int DownloadFilesInformationModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant DownloadFilesInformationModel::data( const QModelIndex &index, const int role ) const
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
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::Filename:
          return HelperQt::toQString( file->filename );

        case Columns::Description:
          return HelperQt::toQString( file->description );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant DownloadFilesInformationModel::headerData(
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

  switch ( Columns{ section } )
  {
    case Columns::Filename:
      return QString{ tr( "Filename" ) };

    case Columns::Description:
      return QString{ tr( "Description" ) };

    default:
      return {};
  }
}

void DownloadFilesInformationModel::files( const Arinc615a::Information::DownloadFilesInformation &files )
{
  beginResetModel();
  filesV = files;
  endResetModel();
}

Arinc615a::Information::DownloadFileInformation DownloadFilesInformationModel::file( const QModelIndex &index )
{
  if ( !index.isValid() || std::cmp_greater_equal( index.row(), filesV.size() ) )
  {
    return {};
  }

  return *std::next( filesV.begin(), index.row() );
}

}
