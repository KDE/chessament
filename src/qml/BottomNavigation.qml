// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.NavigationTabBar {
    id: root

    visible: Controller.hasOpenTournament

    Layout.fillWidth: true

    actions: [
        Kirigami.Action {
            text: i18n("Players")
            icon.name: "user-symbolic"
            checked: Controller.currentView === "PlayersPage"
            onTriggered: root.goToPage("PlayersPage")
        },
        Kirigami.Action {
            text: i18n("Pairings")
            icon.name: "system-users-symbolic"
            checked: Controller.currentView === "PairingsPage"
            onTriggered: root.goToPage("PairingsPage")
        },
        Kirigami.Action {
            text: i18n("Standings")
            icon.name: "games-highscores-symbolic"
            checked: Controller.currentView === "StandingsPage"
            onTriggered: root.goToPage("StandingsPage")
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Settings")
            icon.name: "settings-configure-symbolic"
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament", "TournamentSettings").createObject(root.Controls.Overlay.overlay, {
                    window: root.Controls.ApplicationWindow.window
                }) as TournamentSettings;
                dialog.open();
            }
        }
    ]

    function goToPage(viewName: string): void {
        Controller.currentView = viewName;
    }
}
