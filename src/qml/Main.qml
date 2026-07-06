// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as Dialogs

import org.kde.coreaddons as Core
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp as StatefulApp

import org.kde.chessament
import org.kde.chessament.settings as Settings

StatefulApp.StatefulWindow {
    id: root

    property var pageCache: Object.create(null)

    title: Controller.hasOpenTournament ? KI18n.i18nc("@title:window", "%1 — Chessament", Controller.tournament.name) : KI18n.i18nc("@title:window", "Chessament")
    minimumWidth: Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Units.gridUnit * 20

    windowName: "Main"

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
            dialog.nameFilters = [KI18n.i18nc("@label:listbox", "Chessament event (*.chessament)")];
            dialog.accepted.connect(() => {
                Controller.openEvent(dialog.selectedFile);
            });
            dialog.open();
        }

        function onSaveTournamentAs() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.fileMode = Dialogs.FileDialog.SaveFile;
            dialog.nameFilters = [KI18n.i18nc("@label:listbox", "Chessament event (*.chessament)")];
            dialog.accepted.connect(() => {
                Controller.saveEventAs(dialog.selectedFile);
            });
            dialog.open();
        }

        function onImportTrf() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.nameFilters = [KI18n.i18nc("@label:listbox", "TRF files (*.trf *.txt)")];
            dialog.accepted.connect(() => {
                Controller.importTrf(dialog.selectedFile);
            });
            dialog.open();
        }

        function onExportTrf() {
            const dialog = fileDialog.createObject(QQC2.Overlay.overlay);
            dialog.fileMode = Dialogs.FileDialog.SaveFile;
            dialog.nameFilters = [KI18n.i18nc("@label:listbox", "TRF files (*.trf *.txt)")];
            dialog.accepted.connect(() => {
                Controller.exportTrf(dialog.selectedFile);
            });
            dialog.open();
        }

        function onOpenHandbook(): void {
            Qt.openUrlExternally("help:/");
        }

        function onReportBug(): void {
            Qt.openUrlExternally(`https://bugs.kde.org/enter_bug.cgi?format=guided&product=Chessament&version=${Core.AboutData.version}`);
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
        title: KI18n.i18nc("@title", "Error")
        subtitle: Controller.error
        standardButtons: Kirigami.Dialog.Ok
    }

    globalDrawer: Sidebar {}

    pageStack {
        defaultColumnWidth: root.width
        initialPage: WelcomePage {
            application: root.application
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

    Component {
        id: fileDialog

        Dialogs.FileDialog {
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }
}
