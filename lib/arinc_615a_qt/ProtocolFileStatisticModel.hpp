// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::ProtocolFileStatisticModel.
 **/

#ifndef ARINC_615A_QT_PROTOCOLFILESTATISTICMODEL_HPP
#define ARINC_615A_QT_PROTOCOLFILESTATISTICMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/files/ProtocolFileStatistic.hpp>

#include <QAbstractTableModel>

namespace Arinc615aQt {

/**
 * @brief Qt Table Model listing the ARINC 615A Protocol File Statistic.
 **/
class ARINC_615A_QT_EXPORT ProtocolFileStatisticModel final : public QAbstractTableModel
{
  Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Protocol File Type
      FileType,
      //! Protocol File Count
      FileCount,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Constructs the Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit ProtocolFileStatisticModel( QObject * parent = nullptr );

    //! Destructor
    ~ProtocolFileStatisticModel() override = default;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Packet Types.
     * @retval 0
     *   If @p is valid.
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Always Columns::ColumnsCount.
     * @retval 0
     *   If @p is valid.
     **/
    [[nodiscard]] int columnCount( const QModelIndex &parent ) const override;

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
     * @brief Returns the data for the given role and section in the header with
     *   the specified orientation.
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
     *   New Protocol File statistic.
     **/
    void statistic( Arinc615a::Files::ProtocolFileStatistic::Statistic statistic );

  private:
    //! Packet statistic
    Arinc615a::Files::ProtocolFileStatistic::Statistic statisticV;
};

}

#endif
