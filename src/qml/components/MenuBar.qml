// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtCore
import QtQml
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC

import org.kde.chessament
import org.kde.chessament.settings as Settings

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
        title: KI18n.i18nc("@action:menu", "File")

        Kirigami.Action {
            id: newTournamentAction
            AC.ActionCollection.action: "NewTournament"
            AC.ActionCollection.collection: "org.kde.chessament.file"
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament", "NewTournamentDialog").createObject(root) as NewTournamentDialog;
                dialog.create.connect((fileUrl, name, rounds) => {
                    Controller.newTournament(fileUrl, name, rounds);
                });
                dialog.open();
            }
        }

        Kirigami.Action {
            AC.ActionCollection.action: "Open"
            AC.ActionCollection.collection: "org.kde.chessament.file"
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
            AC.ActionCollection.action: "SaveAs"
            AC.ActionCollection.collection: "org.kde.chessament.file"
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
            AC.ActionCollection.action: "ImportTrf"
            AC.ActionCollection.collection: "org.kde.chessament.file"
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
            AC.ActionCollection.action: "ExportTrf"
            AC.ActionCollection.collection: "org.kde.chessament.file"
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
            AC.ActionCollection.action: "Quit"
            AC.ActionCollection.collection: "org.kde.chessament.file"
            onTriggered: Qt.exit(0)
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@action:menu", "View")

        Kirigami.Action {
            AC.ActionCollection.action: "FindAction"
            AC.ActionCollection.collection: "org.kde.globalactions"
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@action:menu", "Settings")

        Kirigami.Action {
            AC.ActionCollection.action: "KeyBindings"
            AC.ActionCollection.collection: "org.kde.globalactions"
        }

        Kirigami.Action {
            AC.ActionCollection.action: "Preferences"
            AC.ActionCollection.collection: "org.kde.globalactions"
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament.settings", "ChessamentConfigurationView").createObject(root.Controls.Overlay.overlay, {
                    "window": root.Controls.ApplicationWindow.window as Kirigami.ApplicationWindow
                }) as Settings.ChessamentConfigurationView;
                dialog.open();
            }
        }
    }

    Controls.Menu {
        title: KI18n.i18nc("@action:menu", "Help")

        Kirigami.Action {
            AC.ActionCollection.action: "HelpContents"
            AC.ActionCollection.collection: "org.kde.chessament.help"
            onTriggered: Qt.openUrlExternally("help:/")
        }

        Kirigami.Action {
            AC.ActionCollection.action: "ReportBug"
            AC.ActionCollection.collection: "org.kde.globalactions"
        }

        Controls.MenuSeparator {}

        Kirigami.Action {
            AC.ActionCollection.action: "Donate"
            AC.ActionCollection.collection: "org.kde.globalactions"
        }

        Controls.MenuSeparator {}

        Kirigami.Action {
            AC.ActionCollection.action: "AboutApp"
            AC.ActionCollection.collection: "org.kde.globalactions"
        }

        Kirigami.Action {
            AC.ActionCollection.action: "AboutKDE"
            AC.ActionCollection.collection: "org.kde.globalactions"
        }
    }
}
