// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::FindQuery.
 **/

#ifndef ARINC_615A_DLA_QT_OPERATIONS_FINDQUERY_HPP
#define ARINC_615A_DLA_QT_OPERATIONS_FINDQUERY_HPP

#include <arinc_615a_dla_qt/operations/Operations.hpp>

#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <arinc_615a/find/clients/Clients.hpp>

#include <QObject>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

/**
 * @brief ARINC 615A FIND Query.
 *
 * Performs the ARINC 615A FIND query.
 **/
class ARINC_615A_DLA_QT_EXPORT FindQuery : public QObject
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Information Operation
     *
     * @param[in] findClient
     *   ARINC 615A FIND Client Instance.
     * @param[in] configuration
     *   Data Loader Configuration.
     * @param[in] parent
     *   Parent Object.
     **/
    explicit FindQuery(
      Arinc615a::Find::Clients::ClientPtr findClient,
      DataLoaderConfiguration configuration,
      QObject *parent = nullptr );

  signals:
    /**
     * @brief Signals received FIND Response.
     *
     * @param[in] address
     *   Source of FIND Response
     * @param[in] targetInformation
     *   FIND target information.
     **/
    void receivedFindResponse( boost::asio::ip::address address, Arinc615a::Find::TargetInformation targetInformation );

    //! Signals Finished Query
    void queryFinished();

  public slots:
    /**
     * @brief Slot called, when the user requests start of operation.
     **/
    void startQuery( const boost::asio::ip::address &targetAddress );

    /**
     * @brief Slot called, when the user requests Cancellation.
     **/
    void abortQuery();

  private:
    //! @copydoc Arinc615a::Find::Clients::ResponseHandler
    void findResponse(
      const boost::asio::ip::address &address,
      const Arinc615a::Find::TargetInformation &targetInformation );

    //! @copydoc Arinc615a::Find::Clients::CompletionHandler
    void finished();

    //! FIND Client
    Arinc615a::Find::Clients::ClientPtr findClientV;
    //! Data Loader Configuration
    DataLoaderConfiguration configurationV;
    //! FIND Query
    Arinc615a::Find::Clients::QueryPtr queryV;
};

}

#endif
