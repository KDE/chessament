// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.actions as Actions

import org.kde.chessament
import org.kde.chessament.settings as Settings

Actions.StatefulWindow {
    id: root

    property var pageCache: Object.create(null)

    title: Controller.hasOpenTournament ? KI18n.i18nc("@title:window", "%1 — Chessament", Controller.tournament.name) : KI18n.i18nc("@title:window", "Chessament")
    minimumWidth: Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Units.gridUnit * 20

    windowName: "Main"

    application: Actions.Application {
        configurationView: Settings.ChessamentConfigurationView {}
    }

    ChessamentActions {
        application: root.application as Actions.Application
    }

    menuBar: MenuBar {
        id: appMenuBar
    }

    Connections {
        target: Controller

        function onCurrentViewChanged() {
            const view = Controller.currentView;
            const page = root.pageForView(view);
            root.pageStack.clear();
            root.pageStack.layers.clear();
            root.pageStack.push(page);
        }

        function onErrorChanged() {
            errorDialog.open();
        }
    }

    Kirigami.PromptDialog {
        id: errorDialog
        title: KI18n.i18nc("@title", "Error")
        subtitle: Controller.error
        standardButtons: Kirigami.Dialog.Ok
    }

    globalDrawer: Sidebar {
        drawerOpen: Controller.hasOpenTournament
    }

    pageStack {
        defaultColumnWidth: root.width
        initialPage: WelcomePage {
            newTournamentAction: appMenuBar.newTournamentAction
        }
    }

    function pageForView(view: int): var {
        let page = pageCache[view];
        if (!page) {
            const components = {
                [Controller.View.Players]: "PlayersPage",
                [Controller.View.Pairings]: "PairingsPage",
                [Controller.View.Standings]: "StandingsPage"
            };
            const pageName = components[view];

            console.log("Creating page", pageName);

            const component = Qt.createComponent("org.kde.chessament", pageName);
            if (component.status === Component.Error) {
                console.error(component.errorString());
                return;
            }

            page = component.createObject(root);
            pageCache[view] = page;
        }
        return page;
    }
}
