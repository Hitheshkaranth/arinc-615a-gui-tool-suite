// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::InformationOperationStatusPage.
 **/

#ifndef ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONSTATUSPAGE_HPP
#define ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONSTATUSPAGE_HPP

#include <arinc_615a_dla_qt/information/Information.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/information/Information.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class InformationOperationStatusPage;
}

//! Information Operation Status Wizard Page
class InformationOperationStatusPage final : public QWizardPage
{
  Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit InformationOperationStatusPage( QWidget * parent = nullptr );

    //! Destructor
    ~InformationOperationStatusPage() override;

    /**
     * @brief Overrides the isComplete() method.
     *
     * This method is used to activate/ deactivate the "next"-button.
     *
     * @return If the operation is completed.
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Is called when the page is initialised.
     *
     * Adds the Abort Operation Button and issues the _start operation signal_.
     **/
    void initializePage() override;

    /**
     * @brief ARINC 615A Operation Initialised Slot
     *
     * @param[in] response
     *   Initialisation Response
     **/
    void operationInitialised(
      const Arinc615a::Information::InitializationResponse &response );

    /**
     * @brief Called, when Information Operation is completed.
     **/
    void operationFinished();

    /**
     * @brief ARINC 615A Information Operation Status Slot.
     *
     * @param[in] status
     *   Received Status
     **/
    void operationStatus( const Arinc615a::Information::InformationStatus &status );

  signals:
    /**
     * @brief User has started operation.
     **/
    void startOperation();

    /**
     * @brief User has aborted operation
     **/
    void abortOperation();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::InformationOperationStatusPage > ui;
    //! Status Log Model
    std::unique_ptr< Arinc615aQt::InformationStatusLogModel > statusLogModelV;
    //! Completion indicator
    bool isCompleted{ false };
};

}

#endif
