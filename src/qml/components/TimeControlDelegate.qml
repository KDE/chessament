// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.AbstractFormDelegate {
    id: delegate

    required property int row
    required property TimeControlModel timeControlModel

    required property int moves
    required property int time
    required property int increment

    background: null

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        ColumnLayout {
            enabled: delegate.row > 0

            Controls.Label {
                text: KI18n.i18nc("@label", "Moves")
            }
            Controls.SpinBox {
                Layout.fillWidth: true
                from: 0
                to: 100
                value: delegate.moves
                onValueModified: delegate.moves = value
            }
        }

        ColumnLayout {
            Controls.Label {
                text: KI18n.i18nc("@label", "Time (seconds)")
            }
            Controls.SpinBox {
                Layout.fillWidth: true
                from: 0
                to: 36000
                value: delegate.time
                onValueModified: delegate.time = value
            }
        }

        ColumnLayout {
            Controls.Label {
                text: KI18n.i18nc("@label", "Increment (seconds)")
            }
            Controls.SpinBox {
                Layout.fillWidth: true
                from: 0
                to: 600
                value: delegate.increment
                onValueModified: delegate.increment = value
            }
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button", "Delete")
            icon.name: "list-remove-symbolic"
            flat: true
            display: Controls.Button.IconOnly
            enabled: delegate.row > 0
            onPressed: delegate.timeControlModel.deletePeriod(delegate.row)

            Controls.ToolTip.text: text
            Controls.ToolTip.visible: enabled && hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        }
    }
}
