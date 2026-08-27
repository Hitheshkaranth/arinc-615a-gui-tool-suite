// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::UploadLoadsStatusModel.
 **/

#include "UploadLoadsStatusModel.hpp"

#include <arinc_615a_qt/StatusCode.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <helper_qt/String.hpp>
#include <utility>

namespace Arinc615aQt {

UploadLoadsStatusModel::UploadLoadsStatusModel( QObject * const parent ) : QAbstractTableModel{ parent }
{
}

UploadLoadsStatusModel::~UploadLoadsStatusModel() = default;

int UploadLoadsStatusModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( loadsStatusV.size() );
}

int UploadLoadsStatusModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return std::to_underlying( Columns::ColumnsCount );
}

QVariant UploadLoadsStatusModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), loadsStatusV.size() ) )
  {
    return {};
  }

  auto loadStatus{ std::next( loadsStatusV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( Columns{ index.column() } )
      {
        case Columns::HeaderFilename:
          return HelperQt::toQString( loadStatus->headerFilename() );

        case Columns::PartNumber:
          return HelperQt::toQString( loadStatus->partNumber() );

        case Columns::Ratio:
          return QString{ "%1 %" }.arg( static_cast< unsigned int >( loadStatus->ratio() ), 3 );

        case Columns::Status:
          return HelperQt::toQString(
            Arinc615a::status( Arinc615a::OperationClass::Upload, loadStatus->code(), loadStatus->description() ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::BackgroundRole:
      return Arinc615aQt::StatusCode_color( loadStatus->code() );

    case Qt::ItemDataRole::ForegroundRole:
      return QBrush{ Qt::black, Qt::SolidPattern };

    case Qt::ItemDataRole::ToolTipRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( Columns{ index.column() } )
      {
        case Columns::Status:
          return HelperQt::toQString( Arinc615a::StatusCodeDescription::instance().name( loadStatus->code() ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::TextAlignmentRole:
      if ( Columns{ index.column() } == Columns::Ratio )
      {
        return static_cast< int >(
          Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter );
      }
      return{};

    default:
      return {};
  }
}

QVariant UploadLoadsStatusModel::headerData(
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
    case Columns::HeaderFilename:
      return QString{ tr( "Header Filename" ) };

    case Columns::PartNumber:
      return QString{ tr( "Part Number" ) };

    case Columns::Ratio:
      return QString{ tr( "Ratio" ) };

    case Columns::Status:
      return QString{ tr( "Status" ) };

    default:
      return {};
  }
}

void UploadLoadsStatusModel::status( const Arinc615a::Information::UploadLoadsStatus &status )
{
  beginResetModel();
  loadsStatusV = status;
  endResetModel();
}

}
