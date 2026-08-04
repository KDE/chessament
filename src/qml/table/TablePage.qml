// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.Page {
    id: root

    required property var model
    required property Component delegate
    readonly property alias content: content
    readonly property alias tableView: tableView
    readonly property alias heading: heading

    property int sortColumn: -1
    property int sortOrder: Qt.AscendingOrder

    property var selectionBehavior: TableView.SelectRows

    property list<int> columnWidths: []

    signal columnClicked(int index)

    function defaultColumnWidth(column: int): int {
        const columnWidth = root.tableView.implicitColumnWidth(column);
        const headingWidth = heading.implicitColumnWidth(column);
        return Math.max(headingWidth, columnWidth);
    }

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    contentItem: Rectangle {
        id: content

        // The background color will show through the cell
        // spacing, and therefore become the grid line color.
        color: Kirigami.Theme.backgroundColor

        Controls.HorizontalHeaderView {
            id: heading

            textRole: "displayText"
            visible: tableView.rows !== 0
            width: scrollView.width
            syncView: tableView
            clip: true

            delegate: HeaderDelegate {
                required property int index
                required property bool enableSort

                sortSupported: enableSort
                sortEnabled: root.sortColumn === index
                sortOrder: root.sortOrder
                onClicked: root.columnClicked(index)
            }
        }

        Controls.ScrollView {
            id: scrollView

            anchors.fill: parent
            anchors.topMargin: heading.height

            TablePageTableView {
                id: tableView

                model: root.model
                delegate: root.delegate
                selectionBehavior: root.selectionBehavior

                columnWidthProvider: function (column) {
                    const w = tableView.explicitColumnWidth(column);
                    if (w >= 0) {
                        return w;
                    }
                    if (root.columnWidths[column]) {
                        return root.columnWidths[column];
                    }
                    return root.defaultColumnWidth(column);
                }
            }
        }
    }
}
