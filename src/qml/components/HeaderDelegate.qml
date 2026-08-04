// SPDX-FileCopyrightText: 2023 Evgeny Chesnokov <echesnokov@astralinux.ru>
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Controls.Control {
    id: delegate

    Accessible.role: Accessible.ColumnHeader

    required property string displayText

    property bool sortSupported: true
    property bool sortEnabled: false
    property int sortOrder: Qt.AscendingOrder

    signal clicked

    padding: Kirigami.Units.smallSpacing

    background: Rectangle {
        border.color: "transparent"
        color: {
            if (!delegate.enabled) {
                return "transparent";
            }
            if (delegate.hovered && delegate.sortSupported) {
                return Qt.alpha(Kirigami.Theme.hoverColor, 0.3);
            }
            return Kirigami.Theme.backgroundColor;
        }

        Kirigami.Separator {
            height: parent.height
            anchors.right: parent.right
        }

        Kirigami.Separator {
            width: parent.width
            anchors.bottom: parent.bottom
        }
    }

    contentItem: RowLayout {
        anchors.fill: parent
        spacing: delegate.spacing

        TapHandler {
            enabled: delegate.sortSupported
            onTapped: delegate.clicked()
        }

        Controls.Label {
            id: label
            text: delegate.displayText
            elide: Text.ElideRight
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignLeft
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true

            Controls.ToolTip.text: text
            Controls.ToolTip.visible: truncated && delegate.hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        }

        Kirigami.Icon {
            id: sortIndicator
            visible: delegate.sortEnabled
            source: "arrow-up-symbolic"
            implicitWidth: Kirigami.Units.iconSizes.small
            implicitHeight: Kirigami.Units.iconSizes.small

            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: Kirigami.Units.largeSpacing

            states: State {
                when: delegate.sortEnabled && delegate.sortOrder === Qt.DescendingOrder
                PropertyChanges {
                    sortIndicator {
                        rotation: 180
                    }
                }
            }

            transitions: Transition {
                RotationAnimation {
                    duration: Kirigami.Units.longDuration
                    direction: RotationAnimation.Counterclockwise
                }
            }
        }
    }
}
