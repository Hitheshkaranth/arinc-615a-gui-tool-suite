// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::MediaDefinedDownloadOperationWizard.
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_MEDIADEFINEDDOWNLOADOPERATIONWIZARD_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_MEDIADEFINEDDOWNLOADOPERATIONWIZARD_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a/host/Host.hpp>

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/find/Find.hpp>

#include <arinc_615a/TargetId.hpp>

#include <QWizard>

#include <boost/asio/ip/address.hpp>

#include <filesystem>
#include <memory>
#include <string>

namespace Arinc615aDlaQt {

namespace Ui {
class MediaDefinedDownloadOperationWizard;
}

/**
 * @brief ARINC 615A Media Defined Download Operation Wizard.
 **/
class ARINC_615A_DLA_QT_EXPORT MediaDefinedDownloadOperationWizard final : public QWizard
{
    Q_OBJECT

  public:
    //! Wizard Pages
    enum class Pages : int
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
     * @param[in] downloadBaseDirectory
     *   Download Base Directory
     * @param[in] parent
     *   Parent Widget
     **/
    explicit MediaDefinedDownloadOperationWizard(
      const Arinc615a::Host::ProtocolPtr &protocol,
      const DataLoaderConfiguration &configuration,
      const Arinc615a::Find::TargetsAddressInformation &targetsInformation,
      const std::filesystem::path &downloadBaseDirectory,
      QWidget * parent = nullptr );

    //! Destructor
    ~MediaDefinedDownloadOperationWizard() override;

  private slots:
    /**
     * @brief Slot Called, when the Pages changed.
     *
     * This is used to handle single transfer steps:
     *  - Start Operation
     *
     * @param[in] pageId
     *   New Page ID
     **/
    void pageChanged( int pageId );

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
     * @brief Slot called when user selects download directory.
     *
     * @param[in] directory
     *   Download Base directory
     **/
    void downloadBaseDirectorySelected( const std::filesystem::path &directory );

    /**
     * @brief Slot called, when user has selected Files to download
     *
     * Stores the file list internally for later transmission,
     *
     * @param[in] files
     *   Selected Files
     **/
    void updateSelectedFiles( const Arinc615a::Information::DownloadFiles &files );

    /**
     * @brief Slot called when user provides user defined data.
     *
     * Stores the user-defined data internally for later transmission.
     *
     * @param[in] userDefinedData
     *   User Defined Data
     **/
    void updateUserDefinedData( const std::string &userDefinedData );

    /**
     * @brief Slot called, when user requests start of Operation.
     **/
    void startOperation();

    /**
     * @brief Slot called, when the operation is deferred.
     *
     * @param[in] waitTime
     *   Wait Time
     **/
    void operationDeferred( std::chrono::seconds waitTime );

    /**
     * @brief ARINC 615A Operation Finished Slot
     **/
    void operationFinished();

    /**
     * @brief ARINC 615A Download Operation Status Slot.
     *
     * @param[in] status
     *   Received Download Status
     **/
    void operationStatus( const Arinc615a::Information::DownloadStatus &status );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::MediaDefinedDownloadOperationWizard > uiV;

    //! ARINC 615A Media Defined Download Operation
    std::unique_ptr< MediaDefinedDownloadOperation > operationV;

    //! Decoded Target Address Information
    boost::asio::ip::address targetAddressV;
    //! Decoded Target ID Information
    Arinc615a::TargetId targetIdV;
    //! Download Base Directory
    std::filesystem::path downloadBaseDirectoryV;
    //! Create Download Directory
    bool createDownloadDirectoryV{ true };
    //! Download Files
    Arinc615a::Information::DownloadFiles filesV;
    //! User Defined Data
    std::string userDefinedDataV;

    //! Download File List Transferred Indicator
    bool fileListTransmitted{ false };
};

}

#endif
