// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::InformationOperation.
 **/

#ifndef ARINC_615A_DLA_QT_OPERATIONS_INFORMATIONOPERATION_HPP
#define ARINC_615A_DLA_QT_OPERATIONS_INFORMATIONOPERATION_HPP

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <arinc_615a/host/InformationOperationHandler.hpp>

#include <QObject>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

/**
 * @brief ARINC 615A Information Operation.
 *
 * This class wraps the ARINC 615A Operation and converts callbacks into Qt
 * signals.
 **/
class ARINC_615A_DLA_QT_EXPORT InformationOperation final :
  public QObject,
  private Arinc615a::Host::InformationOperationHandler
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Information Operation
     *
     * @param[in] protocol
     *   Host Data Loader Protocol Instance.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] parent
     *   Parent Object.
     **/
    explicit InformationOperation(
      Arinc615a::Host::ProtocolPtr protocol,
      DataLoaderConfiguration configuration,
      QObject *parent = nullptr );

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
     *   Information Operation Status
     **/
    void receivedStatus( const Arinc615a::Information::InformationStatus &status );

    /**
     * @brief Signals received Target Hardware Information.
     *
     * @param[in] targetsHardware
     *   Received Targets Hardware Information.
     * @param[in] integrity
     *   Information Integrity.
     **/
    void receivedInformation( const Arinc615a::Information::TargetsHardware &targetsHardware, bool integrity );

  public slots:
    /**
     * @brief Slot called, when the user requests start of operation.
     **/
    void startOperation( const boost::asio::ip::address &targetAddress, const Arinc615a::TargetId &targetId );

    /**
     * @brief Slot called, when the user requests Cancellation.
     **/
    void abortOperation();

  private:
    //! @copydoc Arinc615a::Host::InformationOperationHandler::initialisationDeferred
    void initialisationDeferred( std::chrono::seconds waitTime ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::initialisationResponse
    void initialisationResponse( const Arinc615a::Information::InitializationResponse &response ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::finished
    void finished( Arinc615a::StatusCode code, std::string_view description ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::status
    void status( const Arinc615a::Information::InformationStatus &status ) override;

    //! @copydoc Arinc615a::Host::InformationOperationHandler::targetInformation
    void targetInformation( const Arinc615a::Information::TargetsHardware &targetsHardware, bool integrity ) override;

    //! Host Data Loader Protocol
    Arinc615a::Host::ProtocolPtr protocolV;
    //! Data Loader Configuration
    DataLoaderConfiguration configurationV;

    //! Information Operation
    Arinc615a::Host::InformationOperationPtr operationV;
};

}

#endif
