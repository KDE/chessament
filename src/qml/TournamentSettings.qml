// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick

import org.kde.ki18n
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.ConfigurationView {
    id: root

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "information"
            text: KI18n.i18nc("@action:button", "Information")
            icon.name: "documentinfo-symbolic"
            page: () => Qt.createComponent("org.kde.chessament", "TournamentSettingsInformation")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "format"
            text: KI18n.i18nc("@action:button", "Format")
            icon.name: "user-symbolic"
            page: () => Qt.createComponent("org.kde.chessament", "TournamentSettingsFormat")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "calendar"
            text: KI18n.i18nc("@action:button", "Calendar")
            icon.name: "view-calendar-symbolic"
            page: () => Qt.createComponent("org.kde.chessament", "TournamentSettingsCalendar")
        }
    ]
}
