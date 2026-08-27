// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::UploadOperationSettingsPage.
 **/

#ifndef ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONSETTINGSPAGE_HPP
#define ARINC_615A_DLA_QT_UPLOAD_UPLOADOPERATIONSETTINGSPAGE_HPP

#include <arinc_615a_dla_qt/upload/Upload.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_615a/find/Find.hpp>

#include <QWizardPage>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class UploadOperationSettingsPage;
}

/**
 * @brief Upload Operation Settings Wizard Page.
 **/
class UploadOperationSettingsPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit UploadOperationSettingsPage( QWidget * parent = nullptr );

    //! Destructor
    ~UploadOperationSettingsPage() override;

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
     * @brief Update List of available Media Sets
     *
     * @param[in] mediaSets
     *   Media Sets
     **/
    void mediaSets( Arinc665::Media::ConstMediaSets mediaSets );

  signals:
    /**
     * @brief Signal emitted when a target has been selected.
     *
     * @param[in] targetAddress
     *   Target Address
     * @param[in] targetId
     *   Target ID
     **/
    void targetSelected( const boost::asio::ip::address &targetAddress, const Arinc615a::TargetId &targetId );

    /**
     * @brief Emitted when the user selects a Media Set.
     *
     * @param[in] mediaSet
     *   MediaSet, which contains the @p loads.
     **/
    void mediaSetSelected( const Arinc665::Media::ConstMediaSetPtr &mediaSet );

    /**
     * @brief Is emitted when the user changes the selected loads.
     *
     * @param[in] loads
     *   Selected Loads.
     **/
    void loadsSelected( const Arinc665::Media::ConstLoads &loads );

  private slots:
    /**
     * @brief Slot called when user selects a Media Set.
     *
     * @param[in] index
     *   Selected Model Index.
     **/
    void updateAvailableLoads( int index );

    /**
     * @brief Slot called, when user selects Loads.
     *
     * Updates the Selected Loads List according to user selection.
     **/
    void updateLoads();

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::UploadOperationSettingsPage > uiV;
    //! Media Sets Model
    std::unique_ptr< Arinc665Qt::Media::MediaSetsModel > mediaSetsModelV;
    //! Available Loads Model
    std::unique_ptr< Arinc665Qt::Media::LoadsModel > availableLoadsModelV;
};

}

#endif
