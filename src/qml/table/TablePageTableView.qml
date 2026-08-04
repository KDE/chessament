// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models as Models

TableView {
    id: tableView

    property int hoveredRow: -1

    delegateModelAccess: DelegateModel.ReadWrite

    clip: true
    pixelAligned: true
    boundsBehavior: Flickable.StopAtBounds

    selectionMode: TableView.SingleSelection

    selectionModel: Models.ItemSelectionModel {
        onCurrentChanged: function (current, previous): void {
            tableView.itemAtIndex(current)?.forceActiveFocus();
        }
    }
}
