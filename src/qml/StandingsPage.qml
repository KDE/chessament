// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
pragma ComponentBehavior: Bound

import QtQml
import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.chessament

TablePage {
    id: root

    Kirigami.ColumnView.fillWidth: true

    model: Controller.standingsModel

    selectionBehavior: TableView.SelectRows

    Connections {
        target: Controller

        function onAreStandingsValidChanged() {
            if (!Controller.areStandingsValid && Controller.currentView === "StandingsPage") {
                Controller.reloadStandings();
            }
        }
    }

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string displayName
        required property bool editing
        required selected
        required current

        text: displayName
    }

    Kirigami.PlaceholderMessage {
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "No standings yet")
        visible: root.tableView.rows === 0
    }
}
