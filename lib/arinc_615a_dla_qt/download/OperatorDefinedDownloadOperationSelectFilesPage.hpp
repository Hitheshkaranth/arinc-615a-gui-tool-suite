// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::OperatorDefinedDownloadOperationSelectFilesPage.
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_OPERATORDEFINEDDOWNLOADOPERATIONSELECTFILESPAGE_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_OPERATORDEFINEDDOWNLOADOPERATIONSELECTFILESPAGE_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/DownloadFileInformation.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class OperatorDefinedDownloadOperationSelectFilesPage;
}

//! Operator Defined Download Operation Select Files Page
class OperatorDefinedDownloadOperationSelectFilesPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit OperatorDefinedDownloadOperationSelectFilesPage( QWidget *parent = nullptr );

    //! Destructor
    ~OperatorDefinedDownloadOperationSelectFilesPage() override;

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

    /**
     * @brief Signal emitted when the use has selected files
     *
     * @param[in] files
     *   Selected Files
     **/
    void selectedFiles( const Arinc615a::Information::DownloadFiles &files );

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
     *   Received Status
     **/
    void operationStatus( const Arinc615a::Information::DownloadStatus &status );

    /**
     * @brief ARINC 615A Operator Defined Download Operation Received
     *   Downloading List Slot.
     *
     * @param[in] list
     *   Available Files
     **/
    void downloadingList( const Arinc615a::Information::DownloadFilesInformation &list );

  private slots:
    /**
     * @brief Slot called when file selection has changed.
     *
     * Updates the selected files model.
     **/
    void updateSelectedFiles();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::OperatorDefinedDownloadOperationSelectFilesPage > uiV;

    //! Download Status Log Model
    std::unique_ptr< Arinc615aQt::DownloadStatusLogModel > statusLogModelV;
    //! Download Files Information Model
    std::unique_ptr< Arinc615aQt::DownloadFilesInformationModel > filesInformationModelV;

    //! Abort Indicator
    bool aborted{ false };
};

}

#endif
