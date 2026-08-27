// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::UploadOperationStatusPage
 **/

#ifndef ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONSTATUSPAGE_HPP
#define ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONSTATUSPAGE_HPP

#include <arinc_615a_dla_qt/upload/Upload.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/Information.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class UploadOperationStatusPage;
}

/**
 * @brief Upload Operation Status Wizard Page
 **/
class UploadOperationStatusPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit UploadOperationStatusPage( QWidget * parent = nullptr );

    //! Destructor
    ~UploadOperationStatusPage() override;

    /**
     * @brief Overrides the isComplete() method.
     *
     * This method is used to activate/ deactivate the "next"-button.
     *
     * @return If all input fields are valid.
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Is called, when the page is initialised.
     *
     * Adds the Abort Operation Button and issues start operation signal.
     **/
    void initializePage() override;

  signals:
    /**
     * @brief User has started operation.
     **/
    void startOperation();

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
     * @brief Called, when Information Operation is completed.
     **/
    void operationFinished();

    /**
     * @brief ARINC 615A Information Operation Status Slot.
     *
     * @param[in] status
     *  Upload Operation Status
     **/
    void operationStatus( const Arinc615a::Information::UploadStatus &status );

  private slots:
    /**
     * @brief Slot called, when a Status Log Entry is selected.
     *
     * Updates the Upload Files Status.
     *
     * @param[in] index
     *   Model Index of Selected Status
     **/
    void statusSelected( const QModelIndex &index );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::UploadOperationStatusPage > ui;
    //! Loads Status Model
    std::unique_ptr< Arinc615aQt::UploadLoadsStatusModel > loadsStatusModelV;
    //! Upload Status Log Model
    std::unique_ptr< Arinc615aQt::UploadStatusLogModel > statusLogModelV;
    //! Loads Status Model for Status Log
    std::unique_ptr< Arinc615aQt::UploadLoadsStatusModel > statusLogLoadsStatusModelV;

    //! Completion indicator
    bool isCompleted{ false };
};

}

#endif
