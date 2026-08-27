// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::InformationOperationSettingsPage.
 **/

#ifndef ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONSETTINGSPAGE_HPP
#define ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONSETTINGSPAGE_HPP

#include <arinc_615a_dla_qt/information/Information.hpp>

#include <arinc_615a/find/Find.hpp>

#include <QWizardPage>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class InformationOperationSettingsPage;
}

/**
 * @brief Information Operation Settings Wizard Page.
 **/
class InformationOperationSettingsPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit InformationOperationSettingsPage( QWidget * parent = nullptr );

    //! Destructor
    ~InformationOperationSettingsPage() override;

    /**
     * @brief Overrides the isComplete() method.
     *
     * This method is used to activate/ deactivate the "next"-button.
     * Target Address and Target ID are checked for validity.
     *
     * @return If all input fields are valid.
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Sets the Targets Information.
     *
     * @param[in] information
     *   New Targets Information.
     **/
    void targets(
      const Arinc615a::Find::TargetsAddressInformation &information );

  signals:
    /**
     * @brief Signal emitted when a target has been selected.
     **/
    void targetSelected( const boost::asio::ip::address &targetAddress, const Arinc615a::TargetId &targetId );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::InformationOperationSettingsPage > uiV;
};

}

#endif
