// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615aQt::TargetsInformationModel.
 **/

#include "TargetsInformationModel.hpp"

#include <utility>

namespace Arinc615aQt {

TargetsInformationModel::TargetsInformationModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

int TargetsInformationModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( targetsInformationV.size() );
}

int TargetsInformationModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

Qt::ItemFlags TargetsInformationModel::flags( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return Qt::NoItemFlags;
  }

  if ( std::cmp_greater_equal( index.row(), targetsInformationV.size() )
    || std::cmp_greater_equal( index.column(), static_cast< int >( Columns::ColumnsCount ) ) )
  {
    return Qt::NoItemFlags;
  }

  return ( index.column() == static_cast< int >( Columns::TargetId ) ) ?
    Qt::ItemIsSelectable | Qt::ItemIsEnabled :
    Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable ;
}

QVariant TargetsInformationModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), targetsInformationV.size() ) )
  {
    return {};
  }

  const auto &[ targetAddress, targetInformation ]{ *std::next( targetsInformationV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::EditRole:
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns >( index.column() ) )
      {
        case Columns::TargetAddress:
          return QString::fromStdString( targetAddress.to_string() );

        case Columns::TargetId:
          return QString::fromStdString( targetInformation.targetId().toString() );

        case Columns::ThwId:
          return QString::fromStdString( targetInformation.thwId );

        case Columns::ThwTypeName:
          return QString::fromStdString( targetInformation.thwTypeName );

        case Columns::ThwPosition:
          return QString::fromStdString( targetInformation.thwPosition );

        case Columns::LiteralName:
          return QString::fromStdString( targetInformation.literalName );

        case Columns::ManufacturerCode:
          return QString::fromStdString( targetInformation.manufacturerCode );

        default:
          return {};
      }

    default:
      return {};
  }
}

bool TargetsInformationModel::setData( const QModelIndex &index, const QVariant &value, const int role )
{
  if ( !index.isValid() )
  {
    return false;
  }

  if ( std::cmp_greater_equal( index.row(), targetsInformationV.size() ) )
  {
    return false;
  }

  auto &[ targetAddress, targetInformation ]{ *std::next( targetsInformationV.begin(), index.row() ) };

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( role )
  {
    case Qt::ItemDataRole::EditRole:
      switch ( static_cast< Columns >( index.column() ) )
      {
        case Columns::TargetAddress:
          targetAddress = boost::asio::ip::make_address( value.toString().toStdString() );
          break;

        case Columns::TargetId:
          return false;

        case Columns::ThwId:
          targetInformation.thwId = value.toString().toStdString();
          break;

        case Columns::ThwTypeName:
          targetInformation.thwTypeName = value.toString().toStdString();
          break;

        case Columns::ThwPosition:
          targetInformation.thwPosition = value.toString().toStdString();
          break;

        case Columns::LiteralName:
          targetInformation.literalName = value.toString().toStdString();
          break;

        case Columns::ManufacturerCode:
          targetInformation.manufacturerCode = value.toString().toStdString();
          break;

        default:
          return false;
      }
      break;

    default:
      return false;
  }

  emit dataChanged(
    createIndex( index.row(), 0 ),
    createIndex( index.row(), static_cast< int >( Columns::ColumnsCount ) - 1 ),
    { Qt::ItemDataRole::DisplayRole } );

  return true;
}

QVariant TargetsInformationModel::headerData(
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
    case Columns::TargetAddress:
      return "Target Address";

    case Columns::TargetId:
      return "Target ID";

    case Columns::ThwId:
      return "THW ID";

    case Columns::ThwTypeName:
      return "THW Type Name";

    case Columns::ThwPosition:
      return "THW Position";

    case Columns::LiteralName:
      return "Literal Name";

    case Columns::ManufacturerCode:
      return "Manufacturer Code";

    default:
      return {};
  }
}

bool TargetsInformationModel::removeRows( const int row, const int count, const QModelIndex &parent )
{
  if ( parent.isValid() )
  {
    return false;
  }

  if ( std::cmp_greater_equal( row, targetsInformationV.size() )
    || std::cmp_greater( row + count, targetsInformationV.size() ) )
  {
    return false;
  }

  beginRemoveRows( parent, row, row + count - 1 );
  targetsInformationV.erase(
    std::next( targetsInformationV.begin(), row ),
    std::next( targetsInformationV.begin(), row + count ) );
  endRemoveRows();

  return true;
}

const Arinc615a::Find::TargetsAddressInformation& TargetsInformationModel::targetsInformation() const
{
  return targetsInformationV;
}

void TargetsInformationModel::targetsInformation( Arinc615a::Find::TargetsAddressInformation information )
{
  beginResetModel();
  targetsInformationV = std::move( information );
  endResetModel();
}

Arinc615a::Find::TargetAddressInformation TargetsInformationModel::targetInformation( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), targetsInformationV.size() ) )
  {
    return {};
  }

  const auto targetIt{ std::next( targetsInformationV.begin(), index.row() ) };

  return *targetIt;
}

void TargetsInformationModel::targetInformation(
  Arinc615a::Find::TargetAddressInformation information )
{
  beginInsertRows(
    QModelIndex{},
    static_cast< int >( targetsInformationV.size() ),
    static_cast< int >( targetsInformationV.size() ) );
  targetsInformationV.push_back( std::move( information ) );
  endInsertRows();
}

void TargetsInformationModel::targetInformation(
  const QModelIndex &index,
  Arinc615a::Find::TargetAddressInformation information )
{
  if ( !index.isValid() )
  {
    return;
  }

  if ( std::cmp_greater_equal( index.row(), targetsInformationV.size() ) )
  {
    return;
  }

  const auto targetIt{ std::next( targetsInformationV.begin(), index.row() ) };

  *targetIt = std::move( information );

  emit dataChanged(
    createIndex( index.row(), 0 ),
    createIndex( index.row(), static_cast< int >( Columns::ColumnsCount ) - 1 ),
    { Qt::DisplayRole } );
}

}
