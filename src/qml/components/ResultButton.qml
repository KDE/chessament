// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.chessament

Controls.ToolButton {
    id: root

    required property int whiteResult
    required property int blackResult

    required property Pairing pairing

    signal setResult

    text: Controller.resultToString(root.whiteResult, root.blackResult)
    checkable: true
    checked: root.pairing?.whiteResult === root.whiteResult && root.pairing?.blackResult === root.blackResult
    onClicked: root.setResult()
}
