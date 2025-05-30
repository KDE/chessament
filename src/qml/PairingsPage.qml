// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQml.Models as Models

import org.kde.kirigami as Kirigami

import dev.alcarazzam.chessament
import dev.alcarazzam.chessament.PairingModel

Kirigami.Page {
    id: root

    padding: 0

    actions: [
        Kirigami.Action {
            text: i18n("Pair round %1", Controller.tournament.currentRound + 1)
            visible: (Controller.tournament.currentRound + 1 <= Controller.tournament.numberOfRounds) && Controller.tournament.isRoundFinished(Controller.tournament.currentRound)
            onTriggered: Controller.pairRound()
        },
        Kirigami.Action {
            text: i18n("Round %1", Controller.currentRound)
            onTriggered: roundDialog.open()
        }
    ]

    Kirigami.Dialog {
        id: roundDialog
        title: i18nc("@title", "Select round")

        ListView {
            id: listView
            implicitWidth: Kirigami.Units.gridUnit * 12
            implicitHeight: Kirigami.Units.gridUnit * 16

            model: Controller.tournament.numberOfRounds
            delegate: QQC2.RadioDelegate {
                required property int index

                topPadding: Kirigami.Units.smallSpacing * 2
                bottomPadding: Kirigami.Units.smallSpacing * 2
                implicitWidth: listView.width
                text: i18n("Round %1", index + 1)
                checked: Controller.currentRound == index + 1
                onClicked: {
                    Controller.currentRound = index + 1;
                    tableView.selectionModel.clearSelection();
                    roundDialog.close();
                }
            }
        }
    }

    QQC2.HorizontalHeaderView {
        id: header
        syncView: tableView
        selectionModel: Models.ItemSelectionModel {}

        anchors {
            left: parent.left
            top: parent.top
        }
    }

    QQC2.ScrollView {
        clip: true

        anchors {
            fill: parent
            topMargin: header.height
        }

        TableView {
            id: tableView
            model: Controller.pairingModel

            alternatingRows: true

            selectionBehavior: TableView.SelectRows
            selectionModel: Models.ItemSelectionModel {}
            selectionMode: TableView.SingleSelection
            interactive: true

            rowHeightProvider: () => Kirigami.Units.gridUnit * 2

            property var columnWidths: [60, 60, 300, 70, 300, 60]
            columnWidthProvider: column => {
                let explicitWidth = explicitColumnWidth(column);
                if (explicitWidth > 0) {
                    return explicitWidth;
                }
                return columnWidths[column];
            }

            Keys.onPressed: event => {
                let selection = tableView.selectionModel.selection;
                if (selection) {
                    let board = selection[0]?.topLeft.row + 1;
                    if (board && Controller.setResult(board, event.key)) {
                        root.selectBoard((board % tableView.model.rowCount()) + 1);
                    }
                }
            }

            delegate: QQC2.ItemDelegate {
                id: delegate

                required property var model
                required property int index
                required property int row
                required property int column
                required property bool current
                required property bool selected

                text: model.display
                highlighted: selected

                onClicked: {
                    delegate.forceActiveFocus();
                    root.selectBoard(row + 1);
                }
                onDoubleClicked: {
                    console.log(tableView.model.index(row, column));
                }
            }
        }
    }

    function selectBoard(board: int) {
        tableView.selectionModel.clearSelection();
        for (let c = 0; c <= 5; c++) {
            tableView.selectionModel.select(tableView.model.index(board - 1, c), ItemSelectionModel.Select);
        }
    }
}
