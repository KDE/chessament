// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: "Cloud"

    FormCard.FormHeader {
        title: "Cloud"
    }
    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            text: KI18n.i18nc("@option:check", "Sync tournament to server")
            checked: Controller.event.syncEnabled
            onToggled: Controller.event.syncEnabled = checked
        }
        FormCard.FormTextDelegate {
            text: Controller.event.syncStatusString
            visible: Controller.event.syncEnabled
        }
    }
}
