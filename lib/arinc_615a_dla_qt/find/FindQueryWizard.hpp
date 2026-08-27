// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::FindQueryWizard.
 **/

#ifndef ARINC_615A_DLA_QT_FIND_FINDQUERYWIZARD_HPP
#define ARINC_615A_DLA_QT_FIND_FINDQUERYWIZARD_HPP

#include <arinc_615a_dla_qt/find/Find.hpp>

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>


#include <arinc_615a/find/clients/Clients.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <QWizard>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class FindQueryWizard;
}

/**
 * @brief ARINC 615A FIND Query Wizard.
 **/
class ARINC_615A_DLA_QT_EXPORT FindQueryWizard final : public QWizard
{
    Q_OBJECT

  public:
    //! Wizard Pages
    enum class Pages : int
    {
      SelectAddressPage,
      ResultsPage
    };

    /**
     * @brief Initialises the Wizard
     *
     * @param[in] findClient
     *   ARINC 615A FIND Client Instance.
     * @param[in] configuration
     *   Data Loader Configuration
     * @param[in,out] targets
     *   Targets Information, which will be updated
     * @param[in] parent
     *   Parent Widget
     **/
    FindQueryWizard(
      Arinc615a::Find::Clients::ClientPtr findClient,
      DataLoaderConfiguration configuration,
      Arinc615a::Find::TargetsAddressInformation &targets,
      QWidget * parent = nullptr );

    //! Destructor
    ~FindQueryWizard() override;

  private slots:
    /**
     * @brief Slot called when user selects new Target Address.
     *
     * @param[in] address
     *   New Target IP Address
     **/
    void targetAddressSelected(  boost::asio::ip::address address );

    /**
     * @brief User has started query.
     **/
    void startQuery();

    /**
     * @brief Saves the FIND Target results.
     **/
    void saveResult();

    /**
     * @brief Slot for FIND Response from FIND Operation.
     *
     * @param[in] address
     *   Source of FIND Response
     * @param[in] information
     *   FIND Information
     **/
    void receivedFindResponse( boost::asio::ip::address address, Arinc615a::Find::TargetInformation information );

    /**
     * @brief Called When operation is finished.
     **/
    void operationFinished();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::FindQueryWizard > uiV;

    //! Target Address
    boost::asio::ip::address queryAddressV{ boost::asio::ip::address_v4::broadcast() };

    //! Targets Information
    Arinc615a::Find::TargetsAddressInformation &targetsV;
    //! Targets Information Model
    std::unique_ptr< Arinc615aQt::TargetsInformationModel > targetsInformationModelV;

    //! Information Query
    std::unique_ptr< FindQuery > queryV;
};

}

#endif
