// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as Dialogs

import org.kde.kirigami as Kirigami

import org.kde.chessament

TablePage {
    id: root

    Kirigami.ColumnView.fillWidth: true

    model: Controller.pairingModel

    selectionBehavior: TableView.SelectRows
    columnWidths: [55, 55, 250, 90, 250, 55]

    Dialogs.FileDialog {
        id: saveDialog
        fileMode: Dialogs.FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: ["PDF Files (*.pdf)"]
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            const fileName = new URL(selectedFile).pathname;
            Controller.savePairingsDocument(fileName);
        }
    }

    actions: [
        Kirigami.Action {
            text: i18n("Pair Round %1", Controller.tournament.currentRound + 1)
            visible: Controller.tournament.numberOfPlayers > 0 && (Controller.tournament.currentRound + 1 <= Controller.tournament.numberOfRounds) && Controller.hasCurrentRoundFinished
            onTriggered: {
                Qt.createComponent("org.kde.chessament", "PairRoundDialog").createObject(root.QQC2.ApplicationWindow.window, {}).open();
            }
        },
        Kirigami.Action {
            id: printAction
            icon.name: "document-print-symbolic"
            text: i18nc("@action:button", "Print…")
            onTriggered: Controller.printPairingsDocument()
        },
        Kirigami.Action {
            icon.name: "document-export-symbolic"
            text: i18nc("@action:button", "Export as PDF…")
            onTriggered: saveDialog.open()
        },
        Kirigami.Action {
            displayComponent: QQC2.ComboBox {
                id: roundSelector
                model: Controller.tournament.numberOfRounds
                currentIndex: Controller.currentRound - 1
                displayText: i18n("Round %1", currentIndex + 1)
                flat: true
                delegate: QQC2.ItemDelegate {
                    required property int index

                    text: index + 1
                    width: roundSelector.width
                    highlighted: roundSelector.highlightedIndex === index
                }
                onActivated: index => {
                    if (Controller.currentRound != index + 1) {
                        root.tableView.selectionModel.clear();
                        Controller.currentRound = index + 1;
                    }
                }
            }
        },
        Kirigami.Action {
            text: i18nc("@action:inmenu", "Remove pairings of this and following rounds…")
            icon.name: "edit-delete"
            displayHint: Kirigami.DisplayHint.AlwaysHide
            onTriggered: {
                Qt.createComponent("org.kde.chessament", "DeletePairingsDialog").createObject(root.QQC2.ApplicationWindow.window, {
                    round: Controller.currentRound
                }).open();
            }
        }
    ]

    footer: ResultsFooter {
        id: footer
        visible: root.tableView.rows !== 0

        pairing: root.model.getPairing(root.tableView.selectionModel.currentIndex.row)

        onSaveResult: (pairing, whiteResult, blackResult) => {
            if (Controller.setResult(pairing.board, whiteResult, blackResult)) {
                root.selectBoard(pairing.board + 1);
            }
        }
    }

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
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "No pairings for round %1 yet", Controller.currentRound)
        visible: root.tableView.rows === 0
    }

    Keys.onPressed: event => {
        let selection = tableView.selectionModel.currentIndex;
        if (selection) {
            let board = selection.row + 1;
            if (board && Controller.setResult(board, event.key)) {
                event.accepted = true;
                root.selectBoard(board + 1);
            }
        }
    }

    function selectBoard(board: int) {
        const row = (board - 1) % root.model.rowCount();
        const index = root.tableView.model.index(row, 0);

        tableView.selectionModel.clear();
        tableView.selectionModel.setCurrentIndex(index, ItemSelectionModel.SelectCurrent | ItemSelectionModel.Rows);
        tableView.itemAtIndex(index)?.forceActiveFocus();
        if (board != 1) {
            // FIXME: column size streches to fit when moving to the first row
            tableView.positionViewAtRow(board - 1, TableView.Contain);
        }
    }
}
