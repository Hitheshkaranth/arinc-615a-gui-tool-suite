// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::DownloadOperationStatusPage
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_DOWNLOADOPERATIONSTATUSPAGE_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_DOWNLOADOPERATIONSTATUSPAGE_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/Information.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class DownloadOperationStatusPage;
}

//! Download Operation Status Wizard Page
class DownloadOperationStatusPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit DownloadOperationStatusPage( QWidget * parent = nullptr );

    //! Destructor
    ~DownloadOperationStatusPage() override;

    /**
     * @brief Overrides the isComplete() method.
     *
     * This method is used to activate/ deactivate the "next"-button.
     *
     * @return If all input fields are valid.
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Is called when the page is initialised.
     *
     * Adds the Abort Operation Button and issues the _start operation signal_.
     **/
    void initializePage() override;

  signals:
    /**
     * @brief User has aborted operation
     **/
    void abortOperation();

  public slots:
    /**
     * @brief ARINC 615A Operation Initialised Slot
     *
     * @param[in] response
     *   Initialisation Response
     **/
    void operationInitialised( const Arinc615a::Information::InitializationResponse &response );

    /**
     * @brief Called, when Download Operation is completed.
     **/
    void operationFinished();

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
    std::unique_ptr< Ui::DownloadOperationStatusPage > uiV;

    //! Files Status Model
    std::unique_ptr< Arinc615aQt::DownloadFilesStatusModel > filesStatusModelV;
    //! Download Status Log Model
    std::unique_ptr< Arinc615aQt::DownloadStatusLogModel > statusLogModelV;
    //! Files Status Model for Status Log
    std::unique_ptr< Arinc615aQt::DownloadFilesStatusModel > statusLogFilesModelV;

    //! Completion indicator
    bool isCompleted{ false };
};

}

#endif
