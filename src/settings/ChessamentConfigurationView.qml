// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.kde.chessament

KirigamiSettings.ConfigurationView {
    id: root

    required property AbstractKirigamiApplication application

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "general"
            text: i18nc("@action:button", "General")
            icon.name: "preferences-system-symbolic"
            page: () => Qt.createComponent("org.kde.chessament.settings", "GeneralPage")
            initialProperties: () => {
                return {
                    application: root.application
                };
            }
        }
    ]
}
