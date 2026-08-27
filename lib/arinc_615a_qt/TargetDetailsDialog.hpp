// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615aQt::TargetDetailsDialog.
 **/

#ifndef ARINC_615A_QT_TARGETDETAILSDIALOG_HPP
#define ARINC_615A_QT_TARGETDETAILSDIALOG_HPP

#include <arinc_615a_qt/Arinc615aQt.hpp>

#include <arinc_615a/find/TargetInformation.hpp>

#include <QDialog>

#include <memory>

namespace Arinc615aQt {

namespace Ui {
class TargetDetailsDialog;
}

//! Target Details Dialog
class ARINC_615A_QT_EXPORT TargetDetailsDialog final : public QDialog
{
  Q_OBJECT

  public:
    /**
     * @brief Initialises Targets Details Dialog.
     *
     * @param[in] information
     *   Target Address Information
     * @param[in] parent
     *   Widget parent.
     **/
    explicit TargetDetailsDialog( Arinc615a::Find::TargetAddressInformation information, QWidget * parent = nullptr );

    //! Destructor
    ~TargetDetailsDialog() override;

    /**
     * Returns the Target Information.
     *
     * @return Target Information.
     **/
    [[nodiscard]] const Arinc615a::Find::TargetAddressInformation& information() const;

  private slots:
    /**
     * @brief Slot Called when any text is updated within the dialog.
     *
     * Copies the data from the GUI-Fields to @p targetInformationV.
     **/
    void textUpdated();

  private:
    //! UI (designer)
    std::unique_ptr< Ui::TargetDetailsDialog > ui;
    //! Target Information
    Arinc615a::Find::TargetAddressInformation informationV;
};

}

#endif
