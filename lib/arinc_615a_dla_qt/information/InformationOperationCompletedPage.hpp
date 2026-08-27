// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::InformationOperationCompletedPage.
 **/

#ifndef ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONCOMPLETEDPAGE_HPP
#define ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONCOMPLETEDPAGE_HPP

#include <arinc_615a_dla_qt/information/Information.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/TargetHardware.hpp>

#include <QWizardPage>
#include <QFileDialog>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class InformationOperationCompletedPage;
}

//! Information Operation Completed Wizard Page
class InformationOperationCompletedPage final : public QWizardPage
{
  Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit InformationOperationCompletedPage( QWidget * parent = nullptr );

    //! Destructor
    ~InformationOperationCompletedPage() override;

  public slots:
    /**
     * @brief Slot Called when the target ID has selected.
     *
     * The Target ID is used to form the Filename for exporting Targets Hardware Information.
     *
     * @param[in] targetId
     *   Selected Target ID
     **/
    void targetId( const QString &targetId );

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
     *   Received Status
     **/
    void operationStatus( const Arinc615a::Information::InformationStatus &status );

    /**
     * @brief ARINC 615A Information Operation Information Result Slot.
     *
     * @param[in] targetsHardware
     *   Received Targets Hardware Information
     **/
    void information( const Arinc615a::Information::TargetsHardware &targetsHardware );

  private slots:
    /**
     * @brief Slot called when user selects a Target Hardware.
     *
     * @param[in] index
     *   Selected Model Index.
     **/
    void selectTargetHardware( const QModelIndex &index );

    /**
     * @brief Slot called when the Select File Dialog has been accepted.
     *
     * Saves the targets hardware information to the given file.
     *
     * @param[in] file
     *   Filename
     **/
    void exportTargetsHardware( const QString &file );


    /**
     * @brief Slot called, when the User selects Copy Targets Hardware Button.
     *
     * Exports the targets hardware information to the clipboard.
     *
     **/
    void copyTargetsHardware();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::InformationOperationCompletedPage > ui;
    //! Status Log Model
    std::unique_ptr< Arinc615aQt::InformationStatusLogModel > statusLogModelV;
    //! Targets Hardware Model
    std::unique_ptr< Arinc615aQt::TargetsHardwareModel > targetsHardwareModelV;
    //! Part Numbers Model
    std::unique_ptr< Arinc615aQt::PartNumbersModel > partNumbersModelV;

    //! Select Export Targets Hardware Dialog
    std::unique_ptr< QFileDialog > selectExportFileDialogV;
};

}

#endif
