// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.config as KConfig

import org.kde.chessament

Kirigami.ApplicationWindow {
    id: root

    property var pageCache: Object.create(null)

    title: Controller.hasOpenTournament ? KI18n.i18nc("@title:window", "%1 — Chessament", Controller.tournament.name) : KI18n.i18nc("@title:window", "Chessament")
    minimumWidth: Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Units.gridUnit * 20

    KConfig.WindowStateSaver {
        configGroupName: "Main"
    }

    ActionCollection {
        pageRow: root.pageStack
    }

    menuBar: MenuBar {}

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

    globalDrawer: Sidebar {}

    pageStack {
        defaultColumnWidth: root.width
        initialPage: WelcomePage {
            newTournamentAction: (root.menuBar as MenuBar).newTournamentAction
        }
    }

    function pageForView(view: string): var {
        if (!pageCache[view]) {
            console.log("Creating page", view);

            const component = Qt.createComponent("org.kde.chessament", view);
            if (component.status === Component.Error) {
                console.error(component.errorString());
                return;
            }

            const obj = component.createObject(root);
            pageCache[view] = obj;
        }
        return pageCache[view];
    }
}
