// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::UploadOperationWizard.
 **/

#ifndef ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONWIZARD_HPP
#define ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONWIZARD_HPP

#include <arinc_615a_dla_qt/upload/Upload.hpp>

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/find/Find.hpp>

#include <arinc_615a/TargetId.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <QWizard>

#include <boost/asio/ip/address.hpp>

#include <memory>
#include <string>

namespace Arinc615aDlaQt {

namespace Ui {
class UploadOperationWizard;
}

/**
 * @brief ARINC 615A Upload Operation Wizard.
 **/
class ARINC_615A_DLA_QT_EXPORT UploadOperationWizard final : public QWizard
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
     * @brief Initialises the Wizard
     *
     * @param[in] protocol
     *   Host Data Loader Protocol Instance.
     * @param[in] configuration
     *   Configuration.
     * @param[in] targetsInformation
     *   Targets Information
     * @param[in] mediaSetManager
     *   ARINC 665 Media Set Manager
     * @param[in] parent
     *   Parent Widget
     **/
    explicit UploadOperationWizard(
      const Arinc615a::Host::ProtocolPtr &protocol,
      const DataLoaderConfiguration &configuration,
      const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
      const Arinc665::Utils::MediaSetManagerPtr &mediaSetManager,
      QWidget * parent = nullptr );

    //! Destructor
    ~UploadOperationWizard() override;

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
     * @brief Slot called, when a Media Set has been selected.
     *
     * @param[in] mediaSet
     *   Selected Media Set
     **/
    void mediaSetSelected( const Arinc665::Media::ConstMediaSetPtr &mediaSet );

    /**
     * @brief Slot called, when user has selected loads.
     *
     * @param[in] loads
     *   Selected Loads
     **/
    void loadsSelected( const Arinc665::Media::ConstLoads &loads );

    /**
     * @brief Slot called, when user requests start of Operation.
     **/
    void startOperation();

    /**
     * @brief Slot called, when the operation is deferred.
     *
     * Sends a "pseudo" operation initialised message to status page.
     *
     * @param[in] waitTime
     *   Wait Time
     **/
    void operationDeferred( std::chrono::seconds waitTime );

    /**
     * @brief ARINC 615A Operation Finished Slot
     *
     * Jumps to the "Completed" Page.
     **/
    void operationFinished();

    /**
     * @brief ARINC 615A Information Operation Status Slot.
     *
     * When operation is accepted and not already sent, the loads list is transmitted.
     *
     * @param[in] status
     *   Received Upload Status
     **/
    void operationStatus( const Arinc615a::Information::UploadStatus &status );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::UploadOperationWizard > ui;

    //! ARINC 615A Upload Operation
    std::unique_ptr< UploadOperation > operationV;

    //! Decoded Target Address Information
    boost::asio::ip::address targetAddressV;
    //! Decoded Target ID Information
    Arinc615a::TargetId targetIdV;
    //! Media Set, which contains the Loads.
    Arinc665::Media::ConstMediaSetPtr mediaSetV;
    //! List of Loads to Upload
    Arinc665::Media::ConstLoads loadsV;

    //! Load List Transferred Indicator
    bool loadListTransmitted{ false };
};

}

#endif
