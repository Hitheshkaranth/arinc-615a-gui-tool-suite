// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::TargetListDialog.
 **/

#ifndef ARINC_615A_QT_TARGETLISTDIALOG_HPP
#define ARINC_615A_QT_TARGETLISTDIALOG_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/Find.hpp>

#include <QDialog>

#include <memory>

namespace Arinc615aQt {

namespace Ui{
class TargetListDialog;
}

/**
 * @brief Target List Dialog.
 **/
class ARINC_615A_QT_EXPORT TargetListDialog final : public QDialog
{
  Q_OBJECT

  public:
    /**
     * @brief Initialises the Media Set Dialog.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit TargetListDialog( QWidget *parent = nullptr );

    //! Destructor
    ~TargetListDialog() override;

    /**
     * @brief Return Targets information.
     *
     * @return Targets Information
     **/
    [[nodiscard]] const Arinc615a::Find::TargetsAddressInformation& targets() const;

    /**
     * @brief Set Targets Information
     *
     * @param[in] targets
     *   Targets Information
     **/
    void targets( Arinc615a::Find::TargetsAddressInformation targets );

  private slots:
    /**
     * Slot called, when the **add** Button is pushed.
     **/
    void add();

    /**
     * Slot called, when the **edit** Button is pushed.
     **/
    void edit();

    /**
     * Slot called, when the **remove** Button is pushed.
     **/
    void remove();

  private:
    //! UI (designer)
    std::unique_ptr< Ui::TargetListDialog > uiV;

    //! Targets Information Model
    std::unique_ptr< Arinc615aQt::TargetsInformationModel > targetsInformationModelV;
};

}

#endif
