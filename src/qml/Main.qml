// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp as StatefulApp

import org.kde.chessament
import org.kde.chessament.settings as Settings

StatefulApp.StatefulWindow {
    id: root

    property var pageCache: Object.create(null)

    title: i18nc("@title:window", "Chessament")

    windowName: "Main"

    readonly property bool isWidescreen: root.width >= 500

    application: ChessamentApplication {
        configurationView: Settings.ChessamentConfigurationView {
            application: root.application
            window: root
        }
    }

    menuBar: MenuBar {
        application: root.application as ChessamentApplication
    }

    Connections {
        target: root.application

        function onNewTournament() {
            const dialog = Qt.createComponent("org.kde.chessament", "NewTournamentDialog").createObject(root) as NewTournamentDialog;
            dialog.create.connect((fileUrl, name, rounds) => {
                Controller.newTournament(fileUrl, name, rounds);
            });
            dialog.open();
        }

        function onOpenTournament() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.accepted.connect(() => {
                Controller.openEvent(dialog.selectedFile);
            });
            dialog.open();
        }

        function onSaveTournamentAs() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.fileMode = Dialogs.FileDialog.SaveFile;
            dialog.accepted.connect(() => {
                Controller.saveEventAs(dialog.selectedFile);
            });
            dialog.open();
        }

        function onImportTrf() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.accepted.connect(() => {
                Controller.importTrf(dialog.selectedFile);
            });
            dialog.open();
        }

        function onExportTrf() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.fileMode = Dialogs.FileDialog.SaveFile;
            dialog.accepted.connect(() => {
                Controller.exportTrf(dialog.selectedFile);
            });
            dialog.open();
        }

        function onConnectAccount() {
            console.log("connect account");
            Controller.connectAccount();
        }
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
        title: i18nc("@title", "Error")
        subtitle: Controller.error
        standardButtons: Kirigami.Dialog.Ok
    }

    globalDrawer: sidebarLoader.item as OverlayDrawer

    Loader {
        id: sidebarLoader
        source: "Sidebar.qml"
        active: root.isWidescreen
    }

    pageStack {
        defaultColumnWidth: root.width
        initialPage: WelcomePage {
            application: root.application
        }
    }

    footer: tabBar.item as BottomNavigation

    Loader {
        id: tabBar
        source: "BottomNavigation.qml"
        active: !root.isWidescreen
    }

    function pageForView(view: string): var {
        if (!pageCache[view]) {
            console.log("Creating page", view);
            pageCache[view] = Qt.createComponent("org.kde.chessament", view).createObject(root);
        }
        return pageCache[view];
    }

    Component {
        id: fileDialog

        Dialogs.FileDialog {
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }
}
