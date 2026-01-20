// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs

import org.kde.kitemmodels as KItemModels
import org.kde.kirigami as Kirigami

import org.kde.chessament

TablePage {
    id: root

    Kirigami.ColumnView.fillWidth: true

    model: KItemModels.KSortFilterProxyModel {
        id: proxyModel
        sourceModel: Controller.pairingModel
        filterRowCallback: function (source_row, source_parent) {
            if (hideFinishedAction.checked) {
                const hasFinished = sourceModel.data(sourceModel.index(source_row, 0, source_parent), PairingModel.HasFinishedRole);
                return !hasFinished;
            }
            return true;
        }
    }

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
            id: hideFinishedAction
            text: i18nc("@option:check", "Hide finished games")
            checkable: true
            checked: false
            displayComponent: Controls.Switch {
                action: hideFinishedAction
            }
            onToggled: {
                proxyModel.invalidateFilter();
            }
        },
        Kirigami.Action {
            id: printAction
            icon.name: "document-print-symbolic"
            text: i18nc("@action:intoolbar", "Print…")
            onTriggered: Controller.printPairingsDocument()
        },
        Kirigami.Action {
            icon.name: "document-export-symbolic"
            text: i18nc("@action:intoolbar", "Export as PDF…")
            onTriggered: saveDialog.open()
        },
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.KeepVisible
            displayComponent: Controls.ComboBox {
                id: roundSelector
                model: Controller.tournament.numberOfRounds
                currentIndex: Controller.currentRound - 1
                displayText: i18n("Round %1", currentIndex + 1)
                flat: true
                delegate: Controls.ItemDelegate {
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
            text: i18nc("@action:intoolbar", "Remove pairings of this and following rounds…")
            icon.name: "edit-delete"
            displayHint: Kirigami.DisplayHint.AlwaysHide
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament", "DeletePairingsDialog").createObject(root.Controls.ApplicationWindow.window, {
                    round: Controller.currentRound
                }) as DeletePairingsDialog;
                dialog.open();
            }
        }
    ]

    header: Kirigami.InlineMessage {
        position: Kirigami.InlineMessage.Position.Header
        text: Controller.tournament.currentRound === 0 ? i18nc("@info", "The tournament hasn't started yet.") : i18nc("@info", "Round %1 has finished.", Controller.tournament.currentRound)
        visible: Controller.tournament.numberOfPlayers > 0 && (Controller.tournament.currentRound + 1 <= Controller.tournament.numberOfRounds) && Controller.hasCurrentRoundFinished
        actions: [
            Kirigami.Action {
                text: i18nc("@action:button", "Pair Round %1", Controller.tournament.currentRound + 1)
                onTriggered: {
                    const dialog = Qt.createComponent("org.kde.chessament", "PairRoundDialog").createObject(root.Controls.ApplicationWindow.window, {}) as PairRoundDialog;
                    dialog.open();
                }
            }
        ]
    }

    footer: ResultsFooter {
        id: footer
        visible: root.tableView.rows !== 0

        pairing: {
            const index = proxyModel.mapToSource(root.tableView.selectionModel.currentIndex);
            return root.model.sourceModel.getPairing(index.row);
        }

        onSaveResult: (pairing, whiteResult, blackResult) => {
            if (Controller.setResult(pairing.board, whiteResult, blackResult)) {
                root.selectNextBoard();
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
        parent: root
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: {
            if (proxyModel.sourceModel.rowCount() === 0) {
                return i18nc("@info:placeholder", "No pairings for round %1 yet", Controller.currentRound);
            }
            return i18nc("@info:placeholder", "All games of round %1 have finished", Controller.currentRound);
        }
        visible: proxyModel.count === 0
    }

    Keys.onPressed: event => {
        const selection = tableView.selectionModel.currentIndex;
        if (selection) {
            const index = proxyModel.mapToSource(selection);
            const pairing = root.model.sourceModel.getPairing(index.row);

            if (pairing.blackPlayer === null) {
                return;
            }

            const board = index.row + 1;
            if (board && Controller.setResult(board, event.key)) {
                event.accepted = true;
                root.selectNextBoard();
            }
        }
    }

    function selectNextBoard(): void {
        const currentRow = tableView.selectionModel.currentIndex.row;
        if (currentRow > 0 && hideFinishedAction.checked || currentRow >= 0 && !hideFinishedAction.checked) {
            const index = root.tableView.model.index(currentRow + 1, 0);
            tableView.selectionModel.clear();
            tableView.selectionModel.setCurrentIndex(index, ItemSelectionModel.SelectCurrent | ItemSelectionModel.Rows);
            tableView.itemAtIndex(index)?.forceActiveFocus();
            tableView.positionViewAtRow(index.row, TableView.Contain);
        }
    }
}
