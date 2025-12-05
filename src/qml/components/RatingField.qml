// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

Controls.SpinBox {
    required property var model

    anchors.fill: parent

    value: model.edit
    from: 0
    to: 4000

    TableView.onCommit: {
        model.edit = value;
    }
}
