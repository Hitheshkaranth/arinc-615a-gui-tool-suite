// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::UploadOperationCompletedPage.
 **/

#ifndef ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONCOMPLETEDPAGE_HPP
#define ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONCOMPLETEDPAGE_HPP

#include <arinc_615a_dla_qt/upload/Upload.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/Information.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class UploadOperationCompletedPage;
}

/**
 * @brief Upload Operation Completed Wizard Page
 **/
class UploadOperationCompletedPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit UploadOperationCompletedPage( QWidget * parent = nullptr );

    //! Destructor
    ~UploadOperationCompletedPage() override;

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
     *  Upload Operation Status
     **/
    void receivedStatus( const Arinc615a::Information::UploadStatus &status );

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
    std::unique_ptr< Ui::UploadOperationCompletedPage > ui;
    //! Loads Status Model
    std::unique_ptr< Arinc615aQt::UploadLoadsStatusModel > loadsStatusModelV;
    //! Upload Status Log Model
    std::unique_ptr< Arinc615aQt::UploadStatusLogModel > statusLogModelV;
    //! Loads Status Model for Status Log
    std::unique_ptr< Arinc615aQt::UploadLoadsStatusModel > statusLogLoadsStatusModelV;
};

}

#endif
