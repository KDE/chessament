// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp as StatefulApp

import org.kde.chessament
import org.kde.chessament.settings as Settings

StatefulApp.StatefulWindow {
    id: root

    property var pageCache: Object.create(null)

    title: i18nc("@title:window", "Chessament")

    windowName: "Main"

    minimumWidth: Kirigami.Units.gridUnit * 40
    minimumHeight: Kirigami.Units.gridUnit * 30

    application: ChessamentApplication {
        configurationView: Settings.ChessamentConfigurationView {
            application: root.application
        }
    }

    Connections {
        target: root.application

        function onNewTournament() {
            let dialog = Qt.createComponent("org.kde.chessament", "NewTournamentDialog").createObject(root);
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
            root.pageStack.replace(page);
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

    globalDrawer: Kirigami.OverlayDrawer {
        id: drawer

        modal: false
        width: 120
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        contentItem: ColumnLayout {
            QQC2.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: root.pageStack.globalToolBar.preferredHeight

                leftPadding: 0
                rightPadding: 0

                contentItem: Item {
                    QQC2.ToolButton {
                        id: menuButton
                        icon.name: "application-menu"
                        onClicked: optionPopup.popup()

                        x: 60 - width / 2

                        QQC2.Menu {
                            id: optionPopup

                            Kirigami.Action {
                                fromQAction: root.application.action("file_new")
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("file_open")
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("file_save_as")
                                enabled: Controller.hasOpenTournament
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("import_trf")
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("export_trf")
                                enabled: Controller.hasOpenTournament
                            }
                            QQC2.MenuSeparator {}
                            Kirigami.Action {
                                fromQAction: root.application.action("options_configure")
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("options_configure_keybinding")
                            }
                            QQC2.MenuSeparator {}
                            Kirigami.Action {
                                fromQAction: root.application.action("open_about_page")
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("open_about_kde_page")
                            }
                            Kirigami.Action {
                                fromQAction: root.application.action("file_quit")
                            }
                        }
                    }
                }
            }
        }

        QQC2.ScrollView {
            id: scrollView

            visible: Controller.hasOpenTournament
            Layout.fillHeight: true
            Layout.fillWidth: true

            QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

            ColumnLayout {
                width: scrollView.width
                spacing: 0

                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    text: i18n("Players")
                    icon.name: "user-symbolic"
                    checked: Controller.currentView === "PlayersPage"
                    onClicked: root.goToPage("PlayersPage")
                }
                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    text: i18n("Pairings")
                    icon.name: "system-users-symbolic"
                    checked: Controller.currentView === "PairingsPage"
                    onClicked: root.goToPage("PairingsPage")
                }
                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    text: i18n("Standings")
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
                icon.name: "settings-configure"
                text: i18nc("@action:button", "Settings")
                onTriggered: tournamentSettings.open()
            }
        }
    }

    TournamentSettings {
        id: tournamentSettings
    }

    pageStack {
        defaultColumnWidth: root.width
        initialPage: WelcomePage {
            application: root.application
        }
    }

    function pageForView(view: string): var {
        if (!pageCache[view]) {
            pageCache[view] = Qt.createComponent("org.kde.chessament", view).createObject(root);
        }
        return pageCache[view];
    }

    function goToPage(viewName: string): void {
        if (applicationWindow().pageStack.depth > 1) {
            applicationWindow().pageStack.pop(null);
        }
        Controller.currentView = viewName;
    }

    Component {
        id: fileDialog

        Dialogs.FileDialog {
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }
}
