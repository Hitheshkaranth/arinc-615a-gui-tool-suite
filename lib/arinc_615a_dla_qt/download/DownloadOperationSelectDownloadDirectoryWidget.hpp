// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::DownloadOperationSelectDownloadDirectoryWidget.
 **/

#ifndef ARINC_615A_DLA_QT_DOWNLOAD_DOWNLOADOPERATIONSETTINGSPAGE_HPP
#define ARINC_615A_DLA_QT_DOWNLOAD_DOWNLOADOPERATIONSETTINGSPAGE_HPP

#include <arinc_615a_dla_qt/download/Download.hpp>

#include <QGroupBox>
#include <QFileDialog>

#include <memory>
#include <filesystem>

namespace Arinc615aDlaQt {

namespace Ui {
class DownloadOperationSelectDownloadDirectoryWidget;
}

/**
 * @brief Download Operation Select Output Directory Widget.
 *
 * Presents a widget where the user can select the *Download Base Directory_ and can enable/ disable the
 * download-specific subdirectory creation.
 **/
class DownloadOperationSelectDownloadDirectoryWidget : public QGroupBox
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit DownloadOperationSelectDownloadDirectoryWidget( QWidget * parent = nullptr );

    //! Destructor
    ~DownloadOperationSelectDownloadDirectoryWidget() override;

    /**
     * @brief Checks if the  widget is completed.
     *
     * @return If all input fields are valid.
     **/
    [[nodiscard]] bool completed() const;

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
     * @brief Signal emitted when directory has been selected.
     *
     * @param[in] directory
     *   Selected download base directory.
     **/
    void downloadBaseDirectorySelected( const std::filesystem::path &directory );

    /**
     * @brief Signal emitted, when the create download subdirectory option has changed.
     *
     * @param[in] createDownloadDirectory
     *   If a download directory shall be created.
     **/
    void createDownloadDirectoryChanged( bool createDownloadDirectory );

  private slots:
    /**
     * @brief Select Directory Button Pressed slot.
     **/
    void selectedDirectory();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::DownloadOperationSelectDownloadDirectoryWidget > uiV;
    //! Select Download Directory File Dialog
    std::unique_ptr< QFileDialog > selectDirectoryDialogV;
};

}

#endif
