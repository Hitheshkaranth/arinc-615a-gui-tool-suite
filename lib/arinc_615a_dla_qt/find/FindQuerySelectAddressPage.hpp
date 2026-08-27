// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::FindQuerySelectAddressPage.
 **/

#ifndef ARINC_615A_DLA_QT_FIND_FINDQUERYSELECTADDRESSPAGE_HPP
#define ARINC_615A_DLA_QT_FIND_FINDQUERYSELECTADDRESSPAGE_HPP

#include <arinc_615a_dla_qt/find/Find.hpp>

#include <QWizardPage>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class FindQuerySelectAddressPage;
}

/**
 * @brief FIND Query Select Address Wizard Page.
 *
 * This Page provides a Combo Box, where the use can select between the global broadcast IP V4 and all
 * interface-specific IP V4 broadcast address.
 * The user is also allowed to manually enter an IP V4 address.
 * The entered address is validated against a valid IP V4 address.
 **/
class FindQuerySelectAddressPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit FindQuerySelectAddressPage( QWizard * parent = nullptr );

    //! Destructor
    ~FindQuerySelectAddressPage() override;

    /**
     * @brief Validates the completeness page.
     *
     * Checks if selected address is valid.
     *
     * @return Is page complete.
     **/
    [[nodiscard]] bool isComplete() const override;

  signals:
    /**
     * @brief Signal emitted, when the selected address has changed.
     *
     * @param[in] address
     *   New IP address
     **/
    void targetAddressChanged( boost::asio::ip::address address ) const;

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::FindQuerySelectAddressPage > ui;
};

}

#endif
