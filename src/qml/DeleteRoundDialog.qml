// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
pragma ComponentBehavior: Bound

import QtQuick.Controls as Controls

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardDialog {
    id: root

    title: i18nc("@title", "Delete round")

    onAccepted: {
        Controller.removePairings(byes.checked);
    }

    FormCard.FormCheckDelegate {
        id: byes
        text: i18nc("@action:check", "Keep byes")
    }

    footer: Controls.DialogButtonBox {
        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            text: i18nc("@action:button", "Delete")
            icon.name: "edit-delete"
            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
        }
    }
}
