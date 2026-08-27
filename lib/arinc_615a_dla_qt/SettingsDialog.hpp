// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::SettingsDialog.
 **/

#ifndef ARINC_615A_DLA_QT_SETTINGSDIALOG_HPP
#define ARINC_615A_DLA_QT_SETTINGSDIALOG_HPP

#include <arinc_615a_dla_qt/Arinc615aDlaQt.hpp>

#include <QDialog>
#include <QFileDialog>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class SettingsDialog;
}

//! Settings Dialog
class ARINC_615A_DLA_QT_EXPORT SettingsDialog final : public QDialog
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Dialog
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit SettingsDialog( QWidget * parent = nullptr );

    //! Destructor
    ~SettingsDialog() override;

    /**
     * @brief Loads the dialog with the given configuration.
     *
     * @param[in] configuration
     *   Configuration to update the dialog.
     **/
    void configuration( const DataLoaderConfiguration &configuration );

    /**
     * @brief Returns a configuration for the current dialog settings
     *
     * @return Configuration with the dialog settings
     **/
    [[nodiscard]] DataLoaderConfiguration configuration() const;

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::SettingsDialog > uiV;
    //! Select Media Set Manager Directory File Dialog
    std::unique_ptr< QFileDialog > selectMediaSetMangerDirectoryFileDialogV;
    //! Select Download Base Directory File Dialog
    std::unique_ptr< QFileDialog > selectDownloadBaseDirectoryFileDialogV;
};

}

#endif
