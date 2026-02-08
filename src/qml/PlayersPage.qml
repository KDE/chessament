// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts as Layouts

import org.kde.ki18n
import org.kde.kitemmodels as KItemModels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.chessament

TablePage {
    id: root

    Kirigami.ColumnView.fillWidth: true

    columnWidths: [55, 55, 250, 60, 90, 90, 100, 100, 150, 50]
    selectionBehavior: TableView.SelectRows

    AddPlayerDialog {
        id: addPlayerDialog
    }

    Dialogs.FileDialog {
        id: saveDialog
        fileMode: Dialogs.FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: ["PDF Files (*.pdf)"]
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: Controller.savePlayersDocument(selectedFile)
    }

    model: KItemModels.KSortFilterProxyModel {
        id: proxyModel

        sourceModel: Controller.playersModel
        sortColumn: 0
        sortOrder: Qt.AscendingOrder
    }

    Connections {
        target: Controller

        function onEventChanged(): void {
            root.tableView.selectionModel.clear();
        }
    }

    function openPlayerDetails(): void {
        const index = proxyModel.mapToSource(root.tableView.selectionModel.currentIndex);
        const player = root.model.sourceModel.data(index, PlayersModel.PlayerRole);
        const dialog = Qt.createComponent("org.kde.chessament", "PlayerDetails").createObject(root.Controls.ApplicationWindow.window, {
            "tournament": Controller.tournament,
            "player": player
        }) as PlayerDetails;
        dialog.open();
    }

    actions: [
        Kirigami.Action {
            icon.name: "documentinfo-symbolic"
            text: KI18n.i18nc("@action:intoolbar Open player details", "Details…")
            enabled: root.tableView.currentRow >= 0
            onTriggered: root.openPlayerDetails()
        },
        Kirigami.Action {
            icon.name: "view-sort-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Sort Players…")
            enabled: Controller.tournament.numberOfPlayers > 1
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.chessament", "SortPlayersDialog").createObject(root.Controls.ApplicationWindow.window, {}) as SortPlayersDialog;
                dialog.open();
            }
        },
        Kirigami.Action {
            id: printAction
            icon.name: "document-print-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Print…")
            onTriggered: Controller.printPlayersDocument()
        },
        Kirigami.Action {
            icon.name: "document-export-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Export as PDF…")
            onTriggered: saveDialog.open()
        },
        Kirigami.Action {
            id: addAction
            icon.name: "list-add-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Add Player…")
            displayHint: Kirigami.DisplayHint.KeepVisible
            onTriggered: addPlayerDialog.open()
        }
    ]

    Components.ConvergentContextMenu {
        id: menu

        Controls.Action {
            icon.name: "documentinfo-symbolic"
            text: KI18n.i18nc("@action:inmenu", "Open Player Details")
            onTriggered: root.openPlayerDetails()
        }
    }

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property bool editing
        required property string iconSource
        required model
        required selected
        required current
        required column

        text: model.display
        icon.source: iconSource

        contextMenu: menu

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
                    model: [" ", "GM", "IM", "FM", "CM", "WGM", "WIM", "WFM", "WCM"]
                    editable: true

                    onActivated: index => {
                        delegate.model.edit = comboBox.currentText;
                    }

                    TableView.onCommit: {
                        if (!comboBox.editText.length && !comboBox.currentText.length && comboBox.highlightedIndex >= 0) {
                            delegate.model.edit = comboBox.textAt(comboBox.highlightedIndex);
                        } else {
                            delegate.model.edit = comboBox.editText;
                        }
                    }

                    Component.onCompleted: {
                        comboBox.editText = delegate.model.edit;
                        // FIXME: This breaks the Enter key
                        // comboBox.popup.open();
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
        text: KI18n.i18nc("@info:placeholder", "No players yet")
        visible: root.tableView.rows === 0
        helpfulAction: Kirigami.Action {
            icon.name: "list-add"
            text: KI18n.i18nc("@action:button", "Add Player…")
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
                    const players = KI18n.i18ncp("@info:status Number of players", "1 player", "%1 players", Controller.tournament.numberOfPlayers);
                    const rated = KI18n.i18ncp("@info:status Number of rated players", "1 rated", "%1 rated", Controller.tournament.numberOfRatedPlayers);

                    return KI18n.i18nc("@info:status Total number of players (Number of players rated)", "%1 (%2)", players, rated);
                }
            }
        }
    }
}
