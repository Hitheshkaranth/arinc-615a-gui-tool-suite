// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::TargetsHardwareModel.
 **/

#ifndef ARINC_615A_QT_TARGETSHARDWAREMODEL_HPP
#define ARINC_615A_QT_TARGETSHARDWAREMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/TargetHardware.hpp>

#include <QAbstractTableModel>

namespace Arinc615aQt {

/**
 * @brief Targets Hardware Model.
 *
 * This table model provides the Target Hardware, which are provided by the *ARINC 615A Information Operation*.
 *
 * @sa @ref Arinc615a::Information::TargetsHardware
 **/
class ARINC_615A_QT_EXPORT TargetsHardwareModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    /**
     * @brief Constructs the Targets Hardware Model
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit TargetsHardwareModel( QObject * parent = nullptr );

    //! Destructor
    ~TargetsHardwareModel() override;

    //! Columns of Model
    enum class Columns : int
    {
      //! Literal Name
      LiteralName,
      //! Serial Number
      SerialNumber,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *   Assumed to be the root element (invalid).
     *
     * @return Number of Target Hardware.
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Parent Model Index.
     *   Assumed to be the root element (invalid).
     *
     * @return Always Columns::ColumnsCount.
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
    [[nodiscard]] QVariant headerData( int section, ::Qt::Orientation orientation, int role ) const override;

    /**
     * @name Target Hardware
     * @{
     **/

    /**
     * @brief Returns the Targets Hardware.
     *
     * @return Targets Hardware
     **/
    [[nodiscard]] const Arinc615a::Information::TargetsHardware& targetsHardware() const;

    /**
     * @brief Updates the Targets Hardware.
     *
     * @param[in] targetsHardware
     *   Targets Hardware.
     *   The content is copied.
     **/
    void targetsHardware( Arinc615a::Information::TargetsHardware targetsHardware );

    /** @} **/

    /**
     * @brief Returns the Target Hardware for the given Index.
     *
     * @param[in] index
     *   Model Index.
     *
     * @return Target Hardware for given @p index.
     * @retval {}
     *   If @p index is invalid or out of range.
     **/
    Arinc615a::Information::TargetHardware targetHardware( const QModelIndex &index );

  private:
    //! Targets Hardware
    Arinc615a::Information::TargetsHardware targetsHardwareV;
};

}

#endif
