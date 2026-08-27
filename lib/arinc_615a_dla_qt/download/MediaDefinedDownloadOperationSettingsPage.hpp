// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::MediaDefinedDownloadOperationSettingsPage.
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_MEDIADEFINEDDOWNLOADOPERATIONSETTINGSPAGE_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_MEDIADEFINEDDOWNLOADOPERATIONSETTINGSPAGE_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/Find.hpp>

#include <arinc_615a/information/Information.hpp>

#include <QWizardPage>
#include <QFileDialog>

#include <boost/asio/ip/address.hpp>

#include <filesystem>
#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class MediaDefinedDownloadOperationSettingsPage;
}

/**
 * @brief Media Defined Download Operation Settings Wizard Page.
 **/
class MediaDefinedDownloadOperationSettingsPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit MediaDefinedDownloadOperationSettingsPage( QWidget * parent = nullptr );

    //! Destructor
    ~MediaDefinedDownloadOperationSettingsPage() override;

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
     * @brief Signal emitted, when user selects download base directory.
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

    /**
     * @brief Signal emitted when the use has selected files
     *
     * @param[in] files
     *   Selected Files
     **/
    void filesSelected( const Arinc615a::Information::DownloadFiles &files );

    /**
     * @brief Signal emitted, when user provides user defined data.
     *
     * @param[in] userDefinedData
     *   User Defined Data
     **/
    void userDefinedDataProvided( const std::string &userDefinedData );

  private slots:
    /**
     * @brief Slot activated, when user requests loading files list from LNR file.
     **/
    void loadLnr( const QString &file );

    /**
     * @brief Slot called to add file to Files List.
     **/
    void addFile();

    /**
     * @brief Slot called to remove file from Files List.
     **/
    void fileRemove();

    /**
     * @brief Slot called to move file upwards in files list.
     **/
    void fileUp();

    /**
     * @brief Slot called to move file downwards in files list.
     **/
    void fileDown();

    /**
     * @brief Slot called, when filename has changed.
     *
     * @param[in] text
     *   New filename
     **/
    void filenameChanged( const QString &text );

    /**
     * @brief Slot called, when user selects file in files list.
     *
     * @param[in] index
     *   Selected file.
     **/
    void fileSelected( const QModelIndex &index );

    /**
     * @brief Slot called, when user changed the user defined data.
     **/
    void userDefinedDataChanged();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::MediaDefinedDownloadOperationSettingsPage > uiV;
    //! Download Files Model
    std::unique_ptr< Arinc615aQt::DownloadFilesModel > filesModelV;
    //! Select LNR File Dialog
    std::unique_ptr< QFileDialog > selectLnrFileDialogV;
};

}

#endif
