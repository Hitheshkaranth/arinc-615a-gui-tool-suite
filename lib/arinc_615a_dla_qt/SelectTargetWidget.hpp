// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aDlaQt::SelectTargetWidget.
 **/

#ifndef ARINC_615A_DLA_QT_SELECTTARGETWIDGET_HPP
#define ARINC_615A_DLA_QT_SELECTTARGETWIDGET_HPP

#include <arinc_615a_dla_qt/Arinc615aDlaQt.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/Find.hpp>

#include <arinc_615a/TargetId.hpp>

#include <QWidget>

#include <boost/asio/ip/address.hpp>

#include <memory>

namespace Arinc615aDlaQt {

namespace Ui {
class SelectTargetWidget;
}

/**
 * @brief Select Target Widget.
 *
 * A list of available targets is presented, which the use can directly select.
 * Additionally, the user can enter or modify the target address and Target ID manually.
 **/
class ARINC_615A_DLA_QT_EXPORT SelectTargetWidget final : public QWidget
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Wizard Page
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit SelectTargetWidget( QWidget * parent = nullptr );

    //! Destructor
    ~SelectTargetWidget() override;

    /**
     * @brief Indicates if a valid target has been selected
     *
     * @return If all input fields are valid.
     **/
    [[nodiscard]] bool completed() const;

    /**
     * @brief Sets the Targets Information.
     *
     * @param[in] information
     *   New Targets Information.
     **/
    void targets( Arinc615a::Find::TargetsAddressInformation information );

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

  private slots:
    /**
     * @brief Slot, which is called when a Target Information is selected.
     *
     * Copies Target IP and Target ID to the corresponding fields and emits targetSelected() signal.
     *
     * @param[in] index
     *   Model Index of selected Target Information.
     **/
    void updateTarget( const QModelIndex &index );

    /**
     * @brief Slot called, when the Target Address has been modified.
     *
     * Tries to decode text and emits targetSelected() signal.
     *
     * @param[in] text
     *   Edited text
     **/
    void updateTargetAddress( const QString &text );

    /**
     * @brief Slot called, when the Target ID has be modified.
     *
     * Tries to decode text and emits targetSelected() signal.
     *
     * @param[in] text
     *   Edited text
     **/
    void updateTargetId( const QString &text );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::SelectTargetWidget > uiV;
    //! Targets Information Model
    std::unique_ptr< Arinc615aQt::TargetsInformationModel > targetsInformationModelV;
    //! Decoded Target Address Information
    boost::asio::ip::address targetAddressV;
    //! Decoded Target ID Information
    Arinc615a::TargetId targetIdV;
};

}

#endif
