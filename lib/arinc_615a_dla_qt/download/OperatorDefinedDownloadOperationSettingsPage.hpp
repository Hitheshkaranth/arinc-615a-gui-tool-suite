// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::OperatorDefinedDownloadOperationSettingsPage.
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_OPERATORDEFINEDDOWNLOADOPERATIONSETTINGSPAGE_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_OPERATORDEFINEDDOWNLOADOPERATIONSETTINGSPAGE_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <arinc_615a/find/Find.hpp>

#include <QWizardPage>

#include <boost/asio/ip/address.hpp>

#include <memory>
#include <filesystem>

namespace Arinc615aDlaQt {

namespace Ui {
class OperatorDefinedDownloadOperationSettingsPage;
}

/**
 * @brief Operator Defined Download Operation Settings Wizard Page.
 **/
class OperatorDefinedDownloadOperationSettingsPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit OperatorDefinedDownloadOperationSettingsPage( QWidget * parent = nullptr );

    //! Destructor
    ~OperatorDefinedDownloadOperationSettingsPage() override;

    /**
     * @brief Overrides the isComplete() method.
     *
     * This method is used to activate/ deactivate the "next"-button.
     * Checks validity of Output Directory.
     *
     * @return If all input fields are valid.
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Sets the Targets Information.
     *
     * @param[in] information
     *   New Targets Information.
     **/
    void targets( const Arinc615a::Find::TargetsAddressInformation &information );

    /**
     * @brief Select Download Base Directory
     *
     * @param[in] directory
     *   Download Base Directory
     **/
    void downloadBaseDirectory( const std::filesystem::path &directory );

    /**
     * @brief Update create download directory option.
     *
     * @param[in] createDownloadDirectory
     *   If a download directory shall be created.
     **/
    void createDownloadDirectory( bool createDownloadDirectory );

  signals:
    /**
     * @brief Signal emitted when a target has been selected.
     **/
    void targetSelected( const boost::asio::ip::address &targetAddress, const Arinc615a::TargetId &targetId );

    /**
     * @brief Signal emitted when user selects download base directory.
     *
     * @param[in] directory
     *   Download Base Directory
     **/
    void downloadBaseDirectorySelected( const std::filesystem::path &directory );

    /**
     * @brief Signal emitted when the create download subdirectory option has changed.
     *
     * @param[in] createDownloadDirectory
     *   If a download directory shall be created.
     **/
    void createDownloadDirectoryChanged( bool createDownloadDirectory );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::OperatorDefinedDownloadOperationSettingsPage > uiV;
};

}

#endif
