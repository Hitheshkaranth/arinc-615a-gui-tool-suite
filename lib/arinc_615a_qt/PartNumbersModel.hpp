// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::PartNumbersModel.
 **/

#ifndef ARINC_615A_QT_PARTNUMBERSMODEL_HPP
#define ARINC_615A_QT_PARTNUMBERSMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/PartNumber.hpp>

#include <QAbstractTableModel>

namespace Arinc615aQt {

/**
 * @brief Part Numbers Information.
 *
 * This table model provides the Part Numbers, which are provided by the *ARINC 615A Information Operation*.
 *
 * @sa @ref Arinc615a::Information::PartNumbers
 **/
class ARINC_615A_QT_EXPORT PartNumbersModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Part Number
      PartNumber,
      //! Amendment.
      Amendment,
      //! Part Designation.
      PartDesignation,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Constructs the Part Numbers Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit PartNumbersModel( QObject * parent = nullptr );

    //! Destructor
    ~PartNumbersModel() override = default;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of dumps.
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
     * @brief Returns the data for the given role and section in the header with the specified orientation.
     *
     * @param[in] section
     *   Section number
     * @param[in] orientation
     *   Orientation
     * @param[in] role
     *   Item role.
     *
     * @return
     *   Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    /**
     * @name Part Numbers
     * @{
     **/

    /**
     * @brief Returns the Part Numbers.
     *
     * @return Part Numbers
     **/
    [[nodiscard]] const Arinc615a::Information::PartNumbers& partNumbers() const;

    /**
     * @brief Updates the Part Numbers.
     *
     * @param[in] partNumbers
     *   Part Numbers
     *   The content is copied.
     **/
    void partNumbers( Arinc615a::Information::PartNumbers partNumbers );

    /** @} **/

  private:
    //! Part Numbers
    Arinc615a::Information::PartNumbers partNumbersV;
};

}

#endif
