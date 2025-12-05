// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

Controls.TextField {
    required property var model

    anchors.fill: parent
    text: model.edit
    horizontalAlignment: TextInput.AlignHCenter
    verticalAlignment: TextInput.AlignVCenter
    validator: IntValidator {
        bottom: 1
    }

    Component.onCompleted: selectAll()

    TableView.onCommit: {
        model.edit = text;
    }
}
