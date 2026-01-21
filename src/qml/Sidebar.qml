// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.OverlayDrawer {
    id: root

    readonly property AbstractKirigamiApplication application: (root.Controls.ApplicationWindow.window as StatefulWindow).application

    modal: false
    edge: Application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    handleVisible: false
    drawerOpen: true
    width: 120
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    contentItem: ColumnLayout {
        Controls.ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: (root.Controls.ApplicationWindow.window as StatefulWindow).pageStack.globalToolBar.preferredHeight

            leftPadding: 0
            rightPadding: 0
        }
    }

    Controls.ScrollView {
        id: scrollView

        visible: Controller.hasOpenTournament
        Layout.fillHeight: true
        Layout.fillWidth: true

        Controls.ScrollBar.vertical.policy: Controls.ScrollBar.AlwaysOff
        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff

        ColumnLayout {
            width: scrollView.width
            spacing: 0

            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                text: KI18n.i18n("Players")
                icon.name: "user-symbolic"
                checked: Controller.currentView === "PlayersPage"
                onClicked: root.goToPage("PlayersPage")
            }
            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                text: KI18n.i18n("Pairings")
                icon.name: "system-users-symbolic"
                checked: Controller.currentView === "PairingsPage"
                onClicked: root.goToPage("PairingsPage")
            }
            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                text: KI18n.i18n("Standings")
                icon.name: "games-highscores-symbolic"
                checked: Controller.currentView === "StandingsPage"
                onClicked: root.goToPage("StandingsPage")
            }
        }
    }

    // HACK: make the toolbar be at the top even when there's no navigation buttons.
    ColumnLayout {
        visible: !Controller.hasOpenTournament
    }

    Kirigami.Separator {
        Layout.fillWidth: true
        Layout.rightMargin: Kirigami.Units.smallSpacing
        Layout.leftMargin: Kirigami.Units.smallSpacing
        visible: Controller.hasOpenTournament
    }

    Kirigami.NavigationTabButton {
        Layout.fillWidth: true
        visible: Controller.hasOpenTournament
        action: Kirigami.Action {
            text: KI18n.i18nc("@action:button", "Settings")
            icon.name: "settings-configure"
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament", "TournamentSettings").createObject(root.Controls.Overlay.overlay, {
                    window: root.Controls.ApplicationWindow.window
                }) as TournamentSettings;
                dialog.open();
            }
        }
    }

    function goToPage(viewName: string): void {
        Controller.currentView = viewName;
    }
}
