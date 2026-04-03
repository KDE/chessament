// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament
import org.kde.chessament.tournament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: KI18n.i18nc("@title", "Accounts")

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Accounts")
    }

    FormCard.FormCard {
        Repeater {
            id: accountsRepeater
            model: AccountManager

            delegate: ColumnLayout {
                id: accountDelegate

                required property int row
                required property Account account
                required property string username
                required property string domain

                FormCard.FormTextDelegate {
                    text: accountDelegate.username
                    description: accountDelegate.domain
                    trailing: Controls.Button {
                        icon.name: "im-kick-user"
                        text: KI18n.i18nc("@action:button", "Logout")
                        onClicked: accountsRepeater.model.logout(accountDelegate.row)
                    }
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            icon.name: "list-add-user-symbolic"
            text: KI18n.i18nc("@action:button", "Add Account")
            onPressed: {
                const dialog = Qt.createComponent("org.kde.chessament.settings", "AddAccountDialog").createObject(root, {
                    "parent": root.Controls.Overlay.overlay
                }) as AddAccountDialog;
                dialog.open();
            }
        }
    }
}
