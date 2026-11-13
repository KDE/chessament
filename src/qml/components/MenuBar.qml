// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtCore
import QtQml
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs

import org.kde.coreaddons as CoreAddons
import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

Controls.MenuBar {
    id: root

    property alias newTournamentAction: newTournamentAction

    Component {
        id: fileDialog

        Dialogs.FileDialog {
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@title:menu", "File")

        Kirigami.Action {
            id: newTournamentAction
            ActionCollection.action: "new_tournament"
            ActionCollection.collection: "org.kde.chessament.file"
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament", "NewTournamentDialog").createObject(root) as NewTournamentDialog;
                dialog.create.connect((fileUrl, name, rounds) => {
                    Controller.newTournament(fileUrl, name, rounds);
                });
                dialog.open();
            }
        }

        Kirigami.Action {
            ActionCollection.action: "file_open"
            ActionCollection.collection: "org.kde.chessament.file"
            onTriggered: {
                const dialog = fileDialog.createObject(Controls.Overlay.overlay);
                dialog.nameFilters = [KI18n.i18nc("@label:listbox", "Chessament event (*.chessament)")];
                dialog.accepted.connect(() => {
                    Controller.openEvent(dialog.selectedFile);
                });
                dialog.open();
            }
        }

        Kirigami.Action {
            ActionCollection.action: "file_save_as"
            ActionCollection.collection: "org.kde.chessament.file"
            enabled: Controller.hasOpenTournament
            onTriggered: {
                const dialog = fileDialog.createObject(Controls.Overlay.overlay);
                dialog.fileMode = Dialogs.FileDialog.SaveFile;
                dialog.nameFilters = [KI18n.i18nc("@label:listbox", "Chessament event (*.chessament)")];
                dialog.accepted.connect(() => {
                    Controller.saveEventAs(dialog.selectedFile);
                });
                dialog.open();
            }
        }

        Kirigami.Action {
            ActionCollection.action: "import_trf"
            ActionCollection.collection: "org.kde.chessament.file"
            onTriggered: {
                const dialog = fileDialog.createObject(Controls.Overlay.overlay);
                dialog.nameFilters = [KI18n.i18nc("@label:listbox", "TRF files (*.trf *.txt)")];
                dialog.accepted.connect(() => {
                    Controller.importTrf(dialog.selectedFile);
                });
                dialog.open();
            }
        }

        Kirigami.Action {
            ActionCollection.action: "export_trf"
            ActionCollection.collection: "org.kde.chessament.file"
            enabled: Controller.hasOpenTournament
            onTriggered: {
                const dialog = fileDialog.createObject(Controls.Overlay.overlay);
                dialog.fileMode = Dialogs.FileDialog.SaveFile;
                dialog.nameFilters = [KI18n.i18nc("@label:listbox", "TRF files (*.trf *.txt)")];
                dialog.accepted.connect(() => {
                    Controller.exportTrf(dialog.selectedFile);
                });
                dialog.open();
            }
        }

        Controls.MenuSeparator {}

        Kirigami.Action {
            ActionCollection.action: "file_quit"
            ActionCollection.collection: "main"
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@title:menu", "View")

        Kirigami.Action {
            ActionCollection.action: "hide_finished_games"
            ActionCollection.collection: "org.kde.chessament.pairings"
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@title:menu", "Tournament")

        Kirigami.Action {
            ActionCollection.action: "update_players"
            ActionCollection.collection: "org.kde.chessament.tournament"
            enabled: Controller.hasOpenTournament && Controller.tournament.numberOfPlayers > 0
            onTriggered: function (): void {
                const dialog = Qt.createComponent("org.kde.chessament", "UpdateRatingsDialog").createObject(root.Controls.ApplicationWindow.window, {}) as UpdateRatingsDialog;
                dialog.open();
            }
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@title:menu", "Settings")

        Kirigami.Action {
            ActionCollection.action: "options_configure_keybinding"
            ActionCollection.collection: "main"
        }

        Kirigami.Action {
            ActionCollection.action: "options_configure"
            ActionCollection.collection: "main"
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@title:menu", "Help")

        Kirigami.Action {
            ActionCollection.action: "help_contents"
            ActionCollection.collection: "org.kde.chessament.help"
            onTriggered: Controller.openHelp()
        }

        Kirigami.Action {
            ActionCollection.action: "open_kcommand_bar"
            ActionCollection.collection: "main"
        }

        Controls.MenuSeparator {}

        Kirigami.Action {
            ActionCollection.action: "help_report_bug"
            ActionCollection.collection: "org.kde.chessament.help"
            onTriggered: Qt.openUrlExternally(`https://bugs.kde.org/enter_bug.cgi?format=guided&product=chessament&version=${CoreAddons.AboutData.version}`)
        }

        Controls.MenuSeparator {}

        Kirigami.Action {
            ActionCollection.action: "help_donate"
            ActionCollection.collection: "org.kde.chessament.help"
            onTriggered: Qt.openUrlExternally("https://kde.org/donate/?app=chessament")
        }

        Controls.MenuSeparator {}

        Kirigami.Action {
            ActionCollection.action: "open_about_page"
            ActionCollection.collection: "main"
        }

        Kirigami.Action {
            ActionCollection.action: "open_about_kde_page"
            ActionCollection.collection: "main"
        }
    }
}
