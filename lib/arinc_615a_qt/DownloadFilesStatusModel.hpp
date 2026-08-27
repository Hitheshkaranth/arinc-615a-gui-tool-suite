// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::DownloadFilesStatusModel.
 **/

#ifndef ARINC_615A_QT_DOWNLOADFILESSTATUSMODEL_HPP
#define ARINC_615A_QT_DOWNLOADFILESSTATUSMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/DownloadFileStatus.hpp>

#include <QAbstractTableModel>

#include <list>

namespace Arinc615aQt {

/**
 * @brief ARINC 615A Download Files Status Model.
 *
 * Status of the file transfer during the Download Operation
 **/
class ARINC_615A_QT_EXPORT DownloadFilesStatusModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Filename
      Filename,
      //! File Status
      Status,

      //! Columns Count Indicator
      ColumnsCount
    };

    /**
     * @brief Constructs the Download Files Status Log Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit DownloadFilesStatusModel( QObject * parent = nullptr );

    //! Destructor
    ~DownloadFilesStatusModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Loads.
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
     * @brief Replace Download Files Status.
     *
     * @param[in] status
     *   Files Status Information.
     **/
    void status( Arinc615a::Information::DownloadFilesStatus status );

  private:
    //! Files Status
    Arinc615a::Information::DownloadFilesStatus filesStatusV;
};

}

#endif
