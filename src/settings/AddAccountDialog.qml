// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardDialog {
    id: root

    title: KI18n.i18nc("@title", "Add Account")

    onAccepted: {
        Controller.accounts.createAccount(serverUrl.text);
    }

    FormCard.FormTextFieldDelegate {
        id: serverUrl
        label: KI18n.i18nc("@label:textbox", "Server URL")
        onTextChanged: {
            Controller.accounts.isChessamentServer(serverUrl.text).then(valid => continueButton.enabled = valid);
        }
    }

    footer: Controls.DialogButtonBox {
        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            id: continueButton
            text: KI18n.i18nc("@action:button", "Continue")
            icon.name: "go-next-symbolic"
            enabled: false
            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
        }
    }
}
