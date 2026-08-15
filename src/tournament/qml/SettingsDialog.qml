// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick

import org.kde.ki18n
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.kde.chessament

KirigamiSettings.ConfigurationView {
    id: root

    required property Tournament tournament

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "information"
            text: KI18n.i18nc("@action:button", "Information")
            icon.name: "documentinfo-symbolic"
            page: () => Qt.createComponent("org.kde.chessament.tournament", "SettingsInformationPage")
            initialProperties: () => {
                return {
                    "tournament": root.tournament
                };
            }
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "format"
            text: KI18n.i18nc("@action:button", "Format")
            icon.name: "user-symbolic"
            page: () => Qt.createComponent("org.kde.chessament.tournament", "SettingsFormatPage")
            initialProperties: () => {
                return {
                    "tournament": root.tournament
                };
            }
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "calendar"
            text: KI18n.i18nc("@action:button", "Calendar")
            icon.name: "view-calendar-symbolic"
            page: () => Qt.createComponent("org.kde.chessament.tournament", "SettingsCalendarPage")
            initialProperties: () => {
                return {
                    "tournament": root.tournament
                };
            }
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "cloud"
            text: "Cloud"
            icon.name: "cloudstatus-symbolic"
            visible: Controller.experimental
            page: () => Qt.createComponent("org.kde.chessament.tournament", "SettingsCloudPage")
        }
    ]
}
