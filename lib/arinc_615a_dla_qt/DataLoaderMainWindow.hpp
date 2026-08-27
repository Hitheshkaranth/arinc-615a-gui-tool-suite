// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::DataLoaderMainWindow.
 **/

#ifndef ARINC_615A_DLA_QT_DATALOADERMAINWINDOW_HPP
#define ARINC_615A_DLA_QT_DATALOADERMAINWINDOW_HPP

#include <arinc_615a_dla_qt/Arinc615aDlaQt.hpp>
#include <arinc_615a_dla_qt/DataLoaderConfiguration.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/clients/Clients.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/host/Host.hpp>

#include <arinc_665_qt/media_set_manager/MediaSetManager.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <tftp_qt/TftpQt.hpp>

#include <helper_qt/HelperQt.hpp>

#include <QMainWindow>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <memory>
#include <thread>
#include <filesystem>

namespace Arinc615aDlaQt {

namespace Ui {
class DataLoaderMainWindow;
}

//! Data Loader Main Window
class ARINC_615A_DLA_QT_EXPORT DataLoaderMainWindow final : public QMainWindow
{
  Q_OBJECT

  public:
    //! Initialises the Data Loader Main Window
    DataLoaderMainWindow();

    //! Destructor
    ~DataLoaderMainWindow() override;

  private slots:
    //! ARINC 615A FIND Query Request
    void findQuery();

    //! information operation request
    void informationOperation();

    //! upload operation request
    void uploadOperation();

    //! Media Defined Download Operation
    void mediaDefinedDownloadOperation();

    //! Operator Defined Download Operation
    void operatorDefinedDownloadOperation();

    //! Manage Targets Request
    void manageTargets();

    /**
     * @brief Slot called, when Targets list shall be updated.
     **/
    void updateTargets();

    /**
     * @brief Slot called, when the user requests Updating the Data Loader
     *   Settings.
     *
     * Updates the Settings dialog with the current settings and shows them.
     **/
    void manageDataLoader();

    /**
     * @brief Slot called when the Dater Loader Settings has been updated.
     *
     * Updates and stores the settings.
     * Reloads the Media Set Manager
     **/
    void updateDataLoaderSettings();

  private:
    /**
     * @brief Loads the Data Loader Configuration.
     **/
    void loadConfiguration();

    /**
     * @brief Saves the Data Loader Configuration.
     **/
    void saveConfiguration();

    /**
     * @brief Initiates the asynchronous Media Set Manager Loading
     *
     **/
    void loadMediaSetManager();

    /**
     * @brief Updates the packet statistic.
     **/
    void updatePacketStatistic();

    //! Main Window
    std::unique_ptr< Ui::DataLoaderMainWindow > ui;

    //! Target List Dialog
    std::unique_ptr< Arinc615aQt::TargetListDialog > targetListDialog;
    //! Media Set Manager Dialog
    std::unique_ptr< Arinc665Qt::MediaSetManager::MediaSetManagerWindow > mediaSetManagerWindow;
    //! Settings Dialog
    std::unique_ptr< SettingsDialog > settingsDialog;
    //! About Dialog
    std::unique_ptr< HelperQt::AboutDialog > aboutDialog;

    //! RX TFTP Packet Statistic Model
    std::unique_ptr< TftpQt::PacketStatisticModel > rxTftpPacketStatisticModelV;
    //! TX TFTP Packet Statistic Model
    std::unique_ptr< TftpQt::PacketStatisticModel > txTftpPacketStatisticModelV;
    //! RX ARINC 615A FIND Packet Statistic Model
    std::unique_ptr< Arinc615aQt::FindPacketStatisticModel > rxFindPacketStatisticModelV;
    //! TX ARINC 615A FIND Packet Statistic Model
    std::unique_ptr< Arinc615aQt::FindPacketStatisticModel > txFindPacketStatisticModelV;
    //! RX ARINC 615A Protocol File Statistic Model
    std::unique_ptr< Arinc615aQt::ProtocolFileStatisticModel > rxProtocolFileStatisticModelV;
    //! TX ARINC 615A Protocol File Statistic Model
    std::unique_ptr< Arinc615aQt::ProtocolFileStatisticModel > txProtocolFileStatisticModelV;

    //! I/O Context
    boost::asio::io_context ioContext;
    //! Work Guard
    boost::asio::executor_work_guard< boost::asio::io_context::executor_type > workGuard;

    //! Data Loader Configuration
    DataLoaderConfiguration configuration;
    //! ARINC 615A Target List
    Arinc615a::Find::TargetsAddressInformation targetsV;

    //! FIND Client
    Arinc615a::Find::Clients::ClientPtr findClient;
    //! Host Data Loader Protocol
    Arinc615a::Host::ProtocolPtr protocol;
    //! Media Set Manager
    Arinc665::Utils::MediaSetManagerPtr mediaSetManagerV;

    //! I/O Service Thread
    std::jthread ioThread;
};

}

#endif
