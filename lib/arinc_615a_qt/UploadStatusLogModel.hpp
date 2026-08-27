// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::UploadStatusLogModel.
 **/

#ifndef ARINC_615A_QT_UPLOADSTATUSLOGMODEL_HPP
#define ARINC_615A_QT_UPLOADSTATUSLOGMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/UploadStatus.hpp>

#include <QAbstractTableModel>

#include <list>

namespace Arinc615aQt {

/**
 * @brief Upload Status Log Model.
 *
 * This model adapts a log (list) of ARINC 615A Upload Status Information for usage in Qt Views.
 **/
class ARINC_615A_QT_EXPORT UploadStatusLogModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Upload Operation Status Log
    using UploadStatusLog = std::list< Arinc615a::Information::UploadStatus >;

    //! Columns of Model
    enum class Columns : int
    {
      //! Status Counter
      Counter,
      //! Status
      Status,
      //! List Ratio
      ListRatio,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Constructs the Upload Status Log Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit UploadStatusLogModel( QObject *parent = nullptr );

    //! Destructor
    ~UploadStatusLogModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Status Log Entries.
     * @retval 0
     *   If @p is valid (not root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent = {} ) const override;

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
     * @return
     *   Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    /**
     * @brief Returns the Status Log Information.
     *
     * @return Targets Information.
     **/
    [[nodiscard]] const UploadStatusLog& statusLog() const;

    /**
     * @brief Returns the Status Information for the given Model Index.
     *
     * @param[in] index
     *   Model Index.
     *
     * @return Status Information for the given Model Index.
     * @retval {}
     *   When Model Index is invalid or out of range.
     **/
    [[nodiscard]] Arinc615a::Information::UploadStatus status( const QModelIndex &index ) const;

    /**
     * @brief Add Status to Status Log.
     *
     * @param[in] status
     *   Status Information.
     **/
    void status( const Arinc615a::Information::UploadStatus &status );

  private:
    //! Upload Operation Status Log
    UploadStatusLog statusLogV;
};

}

#endif
