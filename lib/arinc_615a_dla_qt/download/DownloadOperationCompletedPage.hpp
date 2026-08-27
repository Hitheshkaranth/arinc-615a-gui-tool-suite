// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::DownloadOperationCompletedPage.
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_DOWNLOADOPERATIONCOMPLETEDPAGE_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_DOWNLOADOPERATIONCOMPLETEDPAGE_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/Information.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class DownloadOperationCompletedPage;
}

/**
 * @brief Download Operation Completed Wizard Page
 **/
class DownloadOperationCompletedPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit DownloadOperationCompletedPage( QWidget * parent = nullptr );

    //! Destructor
    ~DownloadOperationCompletedPage() override;

  public slots:
    /**
     * @brief Called When operation is finished.
     *
     * @param[in] code
     *   Final Status Code
     * @param description
     *   Final Status Description
     **/
    void operationFinished( Arinc615a::StatusCode code, const std::string &description );

    /**
     * @brief ARINC 615A Information Operation Status Slot.
     *
     * @param[in] status
     *   Received Upload Status
     **/
    void operationStatus( const Arinc615a::Information::DownloadStatus &status );

  private slots:
    /**
     * @brief Slot called when a Status Log Entry is selected.
     *
     * The corresponding files status is displayed.
     *
     * @param[in] index
     *   Model Index of Selected Status
     **/
    void statusSelected( const QModelIndex &index );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::DownloadOperationCompletedPage > uiV;

    //! Files Status Model
    std::unique_ptr< Arinc615aQt::DownloadFilesStatusModel > filesStatusModelV;
    //! Download Status Log Model
    std::unique_ptr< Arinc615aQt::DownloadStatusLogModel > statusLogModelV;
    //! Files Status Model for Status Log
    std::unique_ptr< Arinc615aQt::DownloadFilesStatusModel > statusLogFilesModelV;
};

}

#endif
