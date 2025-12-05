// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.Page {
    id: root

    required property var model
    required property Component delegate
    readonly property alias tableView: tableView
    readonly property alias heading: heading

    property var selectionBehavior: TableView.SelectCells

    property list<int> columnWidths: []

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
        // The background color will show through the cell
        // spacing, and therefore become the grid line color.
        color: Kirigami.Theme.backgroundColor

        Controls.HorizontalHeaderView {
            id: heading

            visible: tableView.rows !== 0
            width: scrollView.width
            syncView: tableView
            clip: true
        }

        Controls.ScrollView {
            id: scrollView

            anchors.fill: parent
            anchors.topMargin: heading.height

            TableView {
                id: tableView

                property int hoveredRow: -1

                model: root.model

                clip: true
                pixelAligned: true
                boundsBehavior: Flickable.StopAtBounds

                selectionBehavior: root.selectionBehavior
                selectionMode: TableView.SingleSelection

                selectionModel: ItemSelectionModel {
                    id: selectionModel
                }

                delegate: root.delegate

                columnWidthProvider: function (column) {
                    const w = root.tableView.explicitColumnWidth(column);
                    if (w >= 0) {
                        return w;
                    }
                    if (root.columnWidths[column]) {
                        const headingWidth = heading.implicitColumnWidth(column);
                        return Math.max(headingWidth, root.columnWidths[column]);
                    }
                    return root.defaultColumnWidth(column);
                }
            }
        }
    }
}
