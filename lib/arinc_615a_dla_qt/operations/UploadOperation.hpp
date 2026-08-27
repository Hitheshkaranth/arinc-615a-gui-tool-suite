// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::UploadOperation.
 **/

#ifndef ARINC_615A_DLA_QT_OPERATIONS_UPLOADOPERATION_HPP
#define ARINC_615A_DLA_QT_OPERATIONS_UPLOADOPERATION_HPP

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <arinc_615a/information/UploadLoad.hpp>

#include <arinc_615a/host/UploadOperationHandler.hpp>

#include <arinc_615a/tftp/servers/Servers.hpp>

#include <arinc_665/media/Media.hpp>
#include <arinc_665/utils/Utils.hpp>

#include <QObject>

#include <boost/asio/ip/address.hpp>

#include <forward_list>
#include <memory>

namespace Arinc615aDlaQt {

/**
 * @brief ARINC 615A Upload Operation.
 *
 * This class wraps the ARINC 615A Operation and converts callbacks into Qt signals.
 **/
class ARINC_615A_DLA_QT_EXPORT UploadOperation final :
  public QObject,
  private Arinc615a::Host::UploadOperationHandler
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Upload Operation
     *
     * @param[in] protocol
     *   Host Data Loader Protocol Instance.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] mediaSetManager
     *   ARINC 665 Media Set Manager
     * @param[in] parent
     *   Parent Object.
     **/
    explicit UploadOperation(
      Arinc615a::Host::ProtocolPtr protocol,
      DataLoaderConfiguration configuration,
      Arinc665::Utils::MediaSetManagerPtr mediaSetManager,
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
     *  Upload Operation Status
     **/
    void receivedStatus( const Arinc615a::Information::UploadStatus &status );

  public slots:
    /**
     * @brief Slot called when the user requests the start of operation.
     **/
    void startOperation( const boost::asio::ip::address &targetAddress, const Arinc615a::TargetId &targetId );

    /**
     * @brief Slot called when the user requests Cancellation.
     **/
    void abortOperation();

    /**
     * @brief Slot called, when the Loads are transmitted.
     *
     * @param[in] mediaSet
     *   Media Set.
     * @param[in] loads
     *   Loads to be requested.
     **/
    void transmitLoads( Arinc665::Media::ConstMediaSetPtr mediaSet, Arinc665::Media::ConstLoads loads );

  private:
    //! @copydoc Arinc615a::Host::UploadOperationHandler::initialisationDeferred
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::initialisationResponse
    void initialisationResponse( const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::finished
    void finished( Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::status
    void status( const Arinc615a::Information::UploadStatus &status ) override;

    //! @copydoc Arinc615a::Host::UploadOperationHandler::fileRequest
    void fileRequest(
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      const Tftp::Packets::TftpOptions &clientTftpOptions,
      std::string_view loadPartNumber,
      const Arinc649::CheckValue &checkValue ) override;

    /**
     * @brief File transfer completed handler.
     *
     * @param[in,out] operation
     *   TFTP Server Operation
     * @param[in] status
     *   Transfer status.
     **/
    void fileCompleted( const Arinc615a::Tftp::Servers::ReadOperationPtr &operation, Tftp::TransferStatus status );

    //! Host Data Loader Protocol
    Arinc615a::Host::ProtocolPtr protocolV;
    //! Data Loader Configuration
    DataLoaderConfiguration configurationV;
    //! Media Set Manager
    Arinc665::Utils::MediaSetManagerPtr mediaSetManagerV;

    //! Upload Operation
    Arinc615a::Host::UploadOperationPtr operationV;

    //! Loads to be loaded
    Arinc665::Media::ConstLoads loadsV;
    //! Check Values
    Arinc665::Media::CheckValues checkValuesV;
    //! File Transfer Operations
    std::forward_list< Arinc615a::Tftp::Servers::ReadOperationPtr > fileOperationsV;
};

}

#endif
