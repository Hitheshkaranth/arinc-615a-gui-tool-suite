// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::OperatorDefinedDownloadOperation.
 **/

#ifndef ARINC_615A_DLA_QT_OPERATIONS_OPERATORDEFINEDDOWNLOADOPERATION_HPP
#define ARINC_615A_DLA_QT_OPERATIONS_OPERATORDEFINEDDOWNLOADOPERATION_HPP

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <arinc_615a/host/DownloadInformation.hpp>
#include <arinc_615a/host/OperatorDefinedDownloadOperationHandler.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <QObject>

#include <boost/asio/ip/address.hpp>

#include <filesystem>
#include <forward_list>
#include <memory>

namespace Arinc615aDlaQt {

/**
 * @brief ARINC 615A Operator Defined Download Operation.
 *
 * This class wraps the ARINC 615A Operation and converts callbacks into Qt signals.
 **/
class ARINC_615A_DLA_QT_EXPORT OperatorDefinedDownloadOperation final :
  public QObject,
  private Arinc615a::Host::OperatorDefinedDownloadOperationHandler
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Operator Defined Download Operation.
     *
     * @param[in] protocol
     *   Host Data Loader Protocol Instance.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] parent
     *   Parent Object.
     **/
    explicit OperatorDefinedDownloadOperation(
      Arinc615a::Host::ProtocolPtr protocol,
      DataLoaderConfiguration configuration,
      QObject * parent = nullptr );

  signals:
    /**
     * @brief Signals deferred operation execution
     *
     * @param[in] waitTime
     *   Wait Time
     **/
    void operationDeferred( std::chrono::seconds waitTime );

    /**
     * @brief Signals Initialisation of Operation.
     *
     * @param[in] response
     *   Initialisation Response
     **/
    void operationInitialised( const Arinc615a::Information::InitializationResponse &response );

    /**
     * @brief Signals Finished Operation.
     *
     * @param[in] code
     *   Final Status Code
     * @param[in] description
     *   Final Status Description
     **/
    void operationFinished( Arinc615a::StatusCode code, const std::string &description );

    /**
     * @brief Signals Operation Status
     *
     * @param[in] status
     *   Download Operation Status
     **/
    void receivedStatus( const Arinc615a::Information::DownloadStatus &status );

    /**
     * @brief Signals Received Files Information.
     *
     * @param[in] list
     *   Files Information
     **/
    void receivedDownloadingList( const Arinc615a::Information::DownloadFilesInformation &list );

  public slots:
    /**
     * @brief Slot called, when the user requests start of operation.
     **/
    void startOperation(
      const boost::asio::ip::address &targetAddress,
      const Arinc615a::TargetId &targetId,
      const std::filesystem::path &downloadBaseDirectory,
      bool createDownloadDirectory );

    /**
     * @brief Slot called, when the user requests Cancellation.
     **/
    void abortOperation();

    /**
     * @brief Slot called to transmit Downloading Files List
     */
    void transmitFiles( const Arinc615a::Information::DownloadFiles &files );

  private:
    //! @copydoc Arinc615a::Host::OperatorDefinedDownloadOperationHandler::initialisationDeferred
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::OperatorDefinedDownloadOperationHandler::initialisationResponse
    void initialisationResponse( const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::OperatorDefinedDownloadOperationHandler::finished
    void finished( Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::OperatorDefinedDownloadOperationHandler::status
    void status( const Arinc615a::Information::DownloadStatus &status ) override;

    //! @copydoc Arinc615a::Host::OperatorDefinedDownloadOperationHandler::fileRequest
    void fileRequest(
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view partNumber,
      const Arinc649::CheckValue &checkValue ) override;

    //! @copydoc Arinc615a::Host::OperatorDefinedDownloadOperationHandler::downloadingList
    void downloadingList( const Arinc615a::Information::DownloadFilesInformation &files ) override;

    /**
     * @brief File transfer completed handler.
     *
     * @param[in] filename
     *   Filename
     * @param[in] filePath
     *   Path where data is stored.
     * @param[in] expectedFileSize
     *   Expected File Size
     * @param[in,out] operation
     *   TFTP Server Operation
     * @param[in] status
     *   Transfer status.
     **/
    void fileCompleted(
      std::string_view filename,
      const std::filesystem::path &filePath,
      const std::optional< const std::size_t > &expectedFileSize,
      const Arinc615a::Tftp::Servers::WriteOperationPtr &operation,
      Tftp::TransferStatus status );

    //! Host Data Loader Protocol
    Arinc615a::Host::ProtocolPtr protocolV;
    //! Data Loader Configuration
    DataLoaderConfiguration configurationV;

    //! Actual Download Data Directory
    std::filesystem::path downloadDataPathV;
    //! Actual Download Information File
    std::filesystem::path downloadInformationPathV;
    //! Operation
    Arinc615a::Host::OperatorDefinedDownloadOperationPtr operationV;
    //! Download Information
    Arinc615a::Host::DownloadInformation downloadInformationV;
    //! File Transfer Operations
    std::forward_list< Arinc615a::Tftp::Servers::WriteOperationPtr > fileOperationsV;
};

}

#endif
