// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::TargetsInformationModel.
 **/

#ifndef ARINC_615A_QT_TARGETSINFORMATIONMODEL_HPP
#define ARINC_615A_QT_TARGETSINFORMATIONMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <QAbstractTableModel>

namespace Arinc615aQt {

/**
 * @brief Qt Table Model for ARINC 615A Targets Information.
 *
 * This model provides a Table of Target Information.
 * This model is editable.
 * If activated by the view, the fields can be modified except the **Target ID**.
 **/
class ARINC_615A_QT_EXPORT TargetsInformationModel final : public QAbstractTableModel
{
  Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Target Address
      TargetAddress,
      //! Target ID (THW ID + Position)
      TargetId,

      //! Target Hardware ID
      ThwId,
      //! Target Hardware Type Name
      ThwTypeName,
      //! Target Hardware Position
      ThwPosition,
      //! Literal Name
      LiteralName,
      //! Manufacturer Code
      ManufacturerCode,

      //! Column Count Indicator
      ColumnsCount
    };

    /**
     * @brief Constructs the Targets Information Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit TargetsInformationModel( QObject * parent = nullptr );

    //! Destructor
    ~TargetsInformationModel() override = default;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Target Information.
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
     * @brief Returns flags for the given index.
     *
     * This operation is used to inform if a model element can be edited.
     * If the @p index is in range and the column is not the **Target ID**, the element is marked as editable.
     *
     * @param[in] index
     *   Model Index.
     *
     * @return Item Flags
     **/
    [[nodiscard]] Qt::ItemFlags flags( const QModelIndex &index ) const override;

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
     * @brief Updates the given Data.
     *
     * Only the edit role is supported.
     *
     * @param[in] index
     *   Model Index
     * @param[in] value
     *   New Value
     * @param[in] role
     *   role.
     *
     * @return If the update was successful.
     **/
    [[nodiscard]] bool setData( const QModelIndex &index, const QVariant &value, int role ) override;

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
     * @brief Removes the given Rows.
     *
     * @param[in] row
     *   Row index.
     * @param[in] count
     *   Number of rows.
     * @param[in] parent
     *   Parent Index.
     *   Must be `{}`
     *
     * @return if Deletion was successful.
     */
    bool removeRows( int row, int count, const QModelIndex &parent = {} ) override;

    /**
     * @brief Returns the Targets Information.
     *
     * @return Targets Information.
     **/
    [[nodiscard]] const Arinc615a::Find::TargetsAddressInformation &targetsInformation() const;

    /**
     * @brief Updated the Target Information.
     *
     * The target information is copied and no direct link to source is present.
     *
     * @param[in] information
     *   Targets Information.
     **/
    void targetsInformation( const Arinc615a::Find::TargetsAddressInformation information );

    /**
     * @brief Returns the Target Information for the given Model Index.
     *
     * @param[in] index
     *   Model Index.
     *
     * @return Target Information for the given Model Index.
     * @retval {}
     *   When Model Index is invalid or out of range.
     **/
    [[nodiscard]] Arinc615a::Find::TargetAddressInformation targetInformation( const QModelIndex &index ) const;

    /**
     * @brief Add Given Target Information.
     *
     * @param[in] information
     *   Target Information to add.
     **/
    void targetInformation( const Arinc615a::Find::TargetAddressInformation information );

    /**
     * @brief Updates Given Target Information.
     *
     * The column field is ignored.
     *
     * @param[in] index
     *   Model Index.
     * @param[in] information
     *   Target Information to Update (Replace).
     **/
    void targetInformation( const QModelIndex &index, Arinc615a::Find::TargetAddressInformation information );

  private:
    //! Targets Address Information
    Arinc615a::Find::TargetsAddressInformation targetsInformationV;
};

}

#endif
