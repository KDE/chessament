// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardDialog {
    id: root

    title: KI18n.i18nc("@title", "Update Ratings")

    onAccepted: {
        Controller.playersModel.updateRatings(ratingList.currentValue);
    }

    FormCard.FormComboBoxDelegate {
        id: ratingList
        text: KI18n.i18nc("@label:listbox", "Choose a rating list:")
        model: RatingListModel {}
        textRole: "name"
        valueRole: "listId"
    }

    footer: Controls.DialogButtonBox {
        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            text: KI18n.i18nc("@action:button Update players ratings", "Update")
            icon.name: "view-refresh-symbolic"
            enabled: ratingList.currentValue > 0
            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
        }
    }
}
