// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::FindQueryResultsPage.
 **/

#ifndef ARINC_615A_DLA_QT_FIND_FINDQUERYRESULTSPAGE_HPP
#define ARINC_615A_DLA_QT_FIND_FINDQUERYRESULTSPAGE_HPP

#include <arinc_615a_dla_qt/find/Find.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class FindQueryResultsPage;
}

//! FIND Query Results Wizard Page
class FindQueryResultsPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit FindQueryResultsPage( QWizard * parent = nullptr );

    //! Destructor
    ~FindQueryResultsPage() override;

    /**
     * @brief Initialise Page when displayed.
     *
     * Activate abort button
     **/
    void initializePage() override;

    /**
     * @brief Checks if Current Page is Complete.
     *
     * @return If current page is complete.
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Updates the Targets Information Model.
     *
     * @param[in] model
     *   Targets Information Model
     **/
    void targetsInformationModel( Arinc615aQt::TargetsInformationModel * model );

    /**
     * @brief Called, when the FIND Query is finished
     **/
    void queryFinished();

  signals:
    /**
     * @brief User has started the FIND Query.
     **/
    void startQuery();

    /**
     * @brief User has aborted the FIND Query.
     **/
    void abortQuery();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::FindQueryResultsPage > ui;
    //! Complete Indicator
    bool isCompleted{ false };
};

}

#endif
