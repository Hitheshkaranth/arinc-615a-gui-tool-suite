// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::InformationOperationWizard.
 **/

#ifndef ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONWIZARD_HPP
#define ARINC_615A_DLA_QT_INFORMATION_INFORMATIONOPERATIONWIZARD_HPP

#include <arinc_615a_dla_qt/information/Information.hpp>

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/find/Find.hpp>

#include <arinc_615a/TargetId.hpp>

#include <QWizard>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class InformationOperationWizard;
}

/**
 * @brief ARINC 615A Information Operation Wizard Dialog.
 *
 * This controller manages the ARINC 615A Information Operation.
 * It genrates and displays a wizard, which guides the user through the operation.
 **/
class ARINC_615A_DLA_QT_EXPORT InformationOperationWizard final : public QWizard
{
  Q_OBJECT

  public:
    //! Wizard Pages
    enum class Pages
    {
      Settings,
      Status,
      Completed
    };

    /**
     * @brief Initialises the Wizard.
     *
     * @param[in] protocol
     *   Host Data Loader Protocol Instance.
     * @param[in] configuration
     *   Configuration.
     * @param[in] targetsInformation
     *   Targets Information
     * @param[in] parent
     *   Parent Widget.
     **/
    explicit InformationOperationWizard(
      const Arinc615a::Host::ProtocolPtr &protocol,
      const DataLoaderConfiguration &configuration,
      const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
      QWidget * parent = nullptr );

    //! Destructor
    ~InformationOperationWizard() override;

  private slots:
    /**
     * @brief Slot called, when a Target has been selected or edited.
     *
     * @param[in] targetAddress
     *   Target Address
     * @param[in] targetId
     *   Target ID
     **/
    void targetSelected( const boost::asio::ip::address &targetAddress, const Arinc615a::TargetId &targetId );

    /**
     * @brief Slot called when user requests start of Operation.
     **/
    void startOperation();

    /**
     * @brief Slot called when the operation is deferred.
     *
     * Creates and emits a pseudo-initialised message to the status page.
     *
     * @param[in] waitTime
     *   Wait Time
     **/
    void operationDeferred( std::chrono::seconds waitTime );

    /**
     * @brief ARINC 615A Operation Initialised Slot
     *
     * @param[in] response
     *   Initialisation Response
     **/
    void operationInitialised( const Arinc615a::Information::InitializationResponse &response );

    /**
     * @brief ARINC 615A Operation Finished Slot
     **/
    void operationFinished();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::InformationOperationWizard > ui;

    //! Information Operation
    std::unique_ptr< InformationOperation > operationV;

    //! Decoded Target Address Information
    boost::asio::ip::address targetAddressV;
    //! Decoded Target ID Information
    Arinc615a::TargetId targetIdV;
};

}

#endif
