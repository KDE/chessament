// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
pragma ComponentBehavior: Bound

import QtQuick.Controls as Controls

import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.ki18n

import org.kde.chessament

FormCard.FormCardDialog {
    id: root

    required property int round

    title: KI18n.i18nc("@title", "Remove pairings")

    onAccepted: {
        Controller.removePairings(byes.checked);
    }

    FormCard.FormCheckDelegate {
        id: byes
        text: KI18n.i18nc("@action:check", "Keep byes")
        checked: true
    }

    footer: Controls.DialogButtonBox {
        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            text: KI18n.i18nc("@action:button Remove pairings", "Remove")
            icon.name: "edit-delete"
            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
        }
    }
}
