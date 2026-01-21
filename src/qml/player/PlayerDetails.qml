// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick

import org.kde.ki18n
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.kde.chessament

KirigamiSettings.ConfigurationView {
    id: root

    required property Tournament tournament
    required property Player player

    title: KI18n.i18nc("@title:window", "Details of player \"%1\"", player.name)

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "pairings"
            text: KI18n.i18nc("@action:button", "Pairings")
            icon.name: "system-users-symbolic"
            page: () => Qt.createComponent("org.kde.chessament", "PlayerPairingsPage")
            initialProperties: () => {
                return {
                    "tournament": root.tournament,
                    "player": root.player
                };
            }
        }
    ]
}
