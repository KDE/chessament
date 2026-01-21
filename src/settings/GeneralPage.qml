// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: i18nc("@title", "General")

    FormCard.FormHeader {
        visible: Config.developer
        title: i18nc("@title:group", "Account")
    }

    FormCard.FormCard {
        visible: Config.developer

        FormCard.FormButtonDelegate {
            icon.name: "internet-services-symbolic"
            text: i18nc("@action:button", "Connect Account")
            visible: !Controller.account.isLogged
            onPressed: Controller.connectAccount()
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
