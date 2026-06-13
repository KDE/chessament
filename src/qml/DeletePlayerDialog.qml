// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.PromptDialog {
    id: root

    required property Player player

    title: KI18n.i18nc("@title", "Delete Player")
    subtitle: KI18n.i18nc("@label", "Permanently delete player \"%1\"?", root.player.name)

    footer: Controls.DialogButtonBox {
        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            text: KI18n.i18nc("@action:button Delete Player", "Delete")
            icon.name: "edit-delete-symbolic"
            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
        }
    }
}
