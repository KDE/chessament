// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtCore
import QtQml
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

TablePage {
    id: root

    property int round: Math.max(1, Controller.tournament.currentRound)

    Kirigami.ColumnView.fillWidth: true

    model: Controller.standingsModel
    content.visible: Controller.tournament.currentRound > 0

    selectionBehavior: TableView.SelectRows

    function defaultColumnWidth(column: int): int {
        if (column >= 4) {
            return 70;
        }
        const widths = [55, 55, 55, 250];
        return widths[column];
    }

    Dialogs.FileDialog {
        id: saveDialog
        fileMode: Dialogs.FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: ["PDF Files (*.pdf)"]
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: Controller.saveStandingsDocument(selectedFile, root.round)
    }

    actions: [
        Kirigami.Action {
            id: printAction
            icon.name: "document-print-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Print…")
            onTriggered: Controller.printStandingsDocument(root.round)
        },
        Kirigami.Action {
            icon.name: "document-export-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Export as PDF…")
            onTriggered: saveDialog.open()
        },
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.KeepVisible
            visible: root.content.visible
            displayComponent: Controls.ComboBox {
                id: roundSelector
                model: Math.max(1, Controller.tournament.currentRound)
                currentIndex: root.round - 1
                displayText: {
                    if (currentIndex === roundSelector.count - 1) {
                        return KI18n.i18nc("@item:inlistbox", "Final Standings");
                    }
                    return KI18n.i18nc("@item:inlistbox Standings After Round %1", "After Round %1", currentIndex + 1);
                }
                flat: true
                delegate: Controls.ItemDelegate {
                    required property int index

                    text: {
                        if (index === roundSelector.count - 1) {
                            return KI18n.i18nc("@item:inlistbox", "Final Standings");
                        }
                        return KI18n.i18nc("@item:inlistbox Standings After Round %1", "After Round %1", index + 1);
                    }
                    width: roundSelector.width
                    highlighted: roundSelector.highlightedIndex === index
                }
                onActivated: index => {
                    if (root.round !== index + 1) {
                        root.tableView.selectionModel.clear();
                        root.round = index + 1;
                        Controller.reloadStandings(root.round);
                    }
                }
            }
        }
    ]

    Connections {
        target: Controller

        function onAreStandingsValidChanged() {
            if (!Controller.areStandingsValid && Controller.currentView === "StandingsPage") {
                Controller.reloadStandings(root.round);
            }
        }

        function onCurrentViewChanged() {
            if (!Controller.areStandingsValid && Controller.currentView === "StandingsPage") {
                Controller.reloadStandings(root.round);
            }
        }
    }

    Component.onCompleted: Controller.reloadStandings(root.round)

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property bool editing
        required model
        required selected
        required current

        text: model.display
    }

    Kirigami.PlaceholderMessage {
        parent: root
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: KI18n.i18nc("@info:placeholder", "No standings yet")
        visible: Controller.tournament.currentRound < 1
    }
}
