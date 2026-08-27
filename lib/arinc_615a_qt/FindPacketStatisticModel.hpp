// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::FindPacketStatisticModel.
 **/

#ifndef ARINC_615A_QT_FINDPACKETSTATISTICMODEL_HPP
#define ARINC_615A_QT_FINDPACKETSTATISTICMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/packets/PacketStatistic.hpp>

#include <QAbstractTableModel>

namespace Arinc615aQt {

/**
 * @brief Qt Table Model listing the ARINC 615A FIND Packet Statistic.
 **/
class ARINC_615A_QT_EXPORT FindPacketStatisticModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Packet Type
      PacketType,
      //! Packet Count
      PacketCount,
      //! Packet Size
      PacketSize,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Constructs the Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit FindPacketStatisticModel( QObject * parent = nullptr );

    //! Destructor
    ~FindPacketStatisticModel() override = default;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Packet Types.
     * @retval 0
     *   If @p is valid (not root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Columns::ColumnsCount.
     * @retval 0
     *   If @p is valid (not root element)
     **/
    [[nodiscard]] int columnCount( const QModelIndex &parent = {} ) const override;

    /**
     * @brief Returns the requested data.
     *
     * @param[in] index
     *   Index of the requested item.
     * @param[in] role
     *   Requested role.
     *
     * @return Data dependent of the index and role.
     **/
    [[nodiscard]] QVariant data( const QModelIndex &index, int role ) const override;

    /**
     * @brief Returns the data for the given role and section in the header with the specified orientation.
     *
     * @param[in] section
     *   Section number
     * @param[in] orientation
     *   Orientation
     * @param[in] role
     *   Item role.
     *
     * @return Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    /**
     * @brief Update Packet Statistic of Model.
     *
     * @param[in] statistic
     *   New packet statistic.
     **/
    void statistic( Arinc615a::Find::Packets::PacketStatistic::Statistic statistic );

  private:
    //! Packet statistic
    Arinc615a::Find::Packets::PacketStatistic::Statistic statisticV;
};

}

#endif
