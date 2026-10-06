// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: KI18n.i18nc("@title", "General")

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "General")
    }

    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            text: KI18n.i18nc("@option:check", "Automatically open exported PDF files")
            checked: Config.openExportedPdfFiles
            onToggled: {
                Config.openExportedPdfFiles = checked;
                Config.save();
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Result Entry")
    }

    FormCard.FormCard {
        FormCard.FormRadioDelegate {
            text: KI18n.i18nc("@option:radio", "Use 1-2-3 keys")
            checked: Config.resultEntry === 0
            onCheckedChanged: function (): void {
                if (checked) {
                    Config.resultEntry = 0;
                    Config.save();
                }
            }
        }

        FormCard.FormRadioDelegate {
            text: KI18n.i18nc("@option:radio", "Use 1-5-0 keys")
            checked: Config.resultEntry === 1
            onCheckedChanged: function (): void {
                if (checked) {
                    Config.resultEntry = 1;
                    Config.save();
                }
            }
        }
    }
}
