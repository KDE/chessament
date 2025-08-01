// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts as Layouts

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami

import org.kde.chessament

TablePage {
    id: root

    Kirigami.ColumnView.fillWidth: true

    columnWidths: [55, 55, 250, 60, 90, 90, 100, 80, 150, 50]

    AddPlayerDialog {
        id: addPlayerDialog
    }

    Dialogs.FileDialog {
        id: saveDialog
        fileMode: Dialogs.FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: ["PDF Files (*.pdf)"]
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            const fileName = new URL(selectedFile).pathname;
            Controller.savePlayersDocument(fileName);
        }
    }

    model: KSortFilterProxyModel {
        id: proxyModel

        sourceModel: Controller.playersModel
        sortColumn: 0
        sortOrder: Qt.AscendingOrder
    }

    actions: [
        Kirigami.Action {
            icon.name: "view-sort-symbolic"
            text: i18nc("@action:button", "Sort Players…")
            enabled: Controller.tournament.numberOfPlayers > 1
            onTriggered: {
                Qt.createComponent("org.kde.chessament", "SortPlayersDialog").createObject(root.Controls.ApplicationWindow.window, {}).open();
            }
        },
        Kirigami.Action {
            id: printAction
            icon.name: "document-print-symbolic"
            text: i18nc("@action:button", "Print…")
            onTriggered: Controller.printPlayersDocument()
        },
        Kirigami.Action {
            icon.name: "document-export-symbolic"
            text: i18nc("@action:button", "Export as PDF…")
            onTriggered: saveDialog.open()
        },
        Kirigami.Action {
            id: addAction
            icon.name: "list-add-symbolic"
            text: i18nc("@action:button", "Add Player…")
            displayHint: Kirigami.DisplayHint.KeepVisible
            onTriggered: addPlayerDialog.open()
        }
    ]

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property bool editing
        required model
        required selected
        required current

        text: model.display

        onDoubleClicked: {
            root.tableView.edit(proxyModel.index(delegate.row, delegate.column));
        }

        TableView.editDelegate: DelegateChooser {
            DelegateChoice {
                column: PlayersModel.StartingRank

                StartingRankField {}
            }

            DelegateChoice {
                column: PlayersModel.Rating

                RatingField {}
            }

            DelegateChoice {
                column: PlayersModel.NationalRating

                RatingField {}
            }

            DelegateChoice {
                column: PlayersModel.Title

                Controls.ComboBox {
                    id: comboBox

                    anchors.fill: parent
                    model: [" ", "GM", "IM", "FM", "WGM", "CM", "WIM", "WFM", "WCM"]

                    popup.onClosed: root.tableView.closeEditor()

                    onActivated: {
                        delegate.model.edit = comboBox.currentText;
                    }

                    Component.onCompleted: {
                        comboBox.currentIndex = comboBox.indexOfValue(delegate.model.edit);
                        comboBox.popup.open();
                    }
                }
            }

            DelegateChoice {
                Controls.TextField {
                    required property var model

                    anchors.fill: parent
                    text: model.edit
                    horizontalAlignment: TextInput.AlignHCenter
                    verticalAlignment: TextInput.AlignVCenter

                    Component.onCompleted: selectAll()

                    TableView.onCommit: {
                        model.edit = text;
                    }
                }
            }
        }
    }

    Kirigami.PlaceholderMessage {
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "No players yet")
        visible: root.tableView.rows === 0
        helpfulAction: Kirigami.Action {
            icon.name: "list-add-symbolic"
            text: i18nc("@action:button", "Add player")
            onTriggered: addPlayerDialog.open()
        }
    }

    footer: Controls.ToolBar {
        visible: Controller.tournament.numberOfPlayers !== 0

        Layouts.RowLayout {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                leftMargin: Kirigami.Units.smallSpacing
            }

            Controls.Label {
                text: {
                    const players = i18ncp("@info:status Number of players", "1 player", "%1 players", Controller.tournament.numberOfPlayers);
                    const rated = i18ncp("@info:status Number of rated players", "1 rated", "%1 rated", Controller.tournament.numberOfRatedPlayers);

                    return i18nc("@info:status players, rated", "%1, %2", players, rated);
                }
            }
        }
    }
}
