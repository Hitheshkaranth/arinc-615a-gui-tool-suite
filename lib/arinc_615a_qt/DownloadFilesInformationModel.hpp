// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::DownloadFilesInformationModel.
 **/

#ifndef ARINC_615A_QT_DOWNLOADFILESINFORMATIONMODEL_HPP
#define ARINC_615A_QT_DOWNLOADFILESINFORMATIONMODEL_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/DownloadFileInformation.hpp>

#include <QAbstractTableModel>

#include <list>

namespace Arinc615aQt {

/**
 * @brief ARINC 615A Download Files Information Model.
 *
 * This model is used to display Downloadable Files, which are advertised by the Target Hardware during to Operator
 * Defined Download Operation.
 **/
class ARINC_615A_QT_EXPORT DownloadFilesInformationModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Filename
      Filename,
      //! File Description
      Description,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Constructs the Download Files Information Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit DownloadFilesInformationModel( QObject * parent = nullptr );

    //! Destructor
    ~DownloadFilesInformationModel() override;

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
     * @return Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, ::Qt::Orientation orientation, int role ) const override;

    /**
     * @brief Replace Download Files Information.
     *
     * @param[in] files
     *   Files Information.
     **/
    void files( const Arinc615a::Information::DownloadFilesInformation &files );

    /**
     * @brief Return File Information for a given Model Index.
     *
     * @param[in] index
     *   Model Index
     *
     * @return File Information
     * @retval {}
     *   When @p index does not reference a file.
     **/
    Arinc615a::Information::DownloadFileInformation file( const QModelIndex &index );

  private:
    //! Files Status
    Arinc615a::Information::DownloadFilesInformation filesV;
};

}

#endif
