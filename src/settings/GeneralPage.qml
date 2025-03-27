// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import dev.alcarazzam.chessament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: i18nc("@title", "General")

    FormCard.FormHeader {
        title: i18nc("@title:group", "Account")
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: i18nc("@label:button", "Connect Account")
            action: Kirigami.Action {
                fromQAction: root.application.action("connect_account")
            }
            visible: !Controller.account.isLogged
        }
        FormCard.FormTextDelegate {
            text: i18n("Logged in as %1", Controller.account.username)
            visible: Controller.account.isLogged
        }
        FormCard.FormButtonDelegate {
            icon.name: "im-kick-user"
            text: i18n("Logout")
            visible: Controller.account.isLogged
            onClicked: Controller.account.logout()
        }
    }
}
