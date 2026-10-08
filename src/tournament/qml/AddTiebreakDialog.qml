// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament.tournament

Controls.Dialog {
    id: root

    required property Tournament tournament

    property string tiebreak

    title: KI18n.i18nc("@title:window", "Add Tiebreak")
    implicitWidth: Math.min(Controls.ApplicationWindow.window.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 25)
    implicitHeight: Math.min(Controls.ApplicationWindow.window.height - Kirigami.Units.gridUnit * 4, contentItem.implicitHeight)
    anchors.centerIn: parent
    modal: true

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    contentItem: Kirigami.ScrollablePage {
        background: null

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        ColumnLayout {
            spacing: 0

            Repeater {
                model: root.tournament.availableTiebreaks()

                FormCard.FormButtonDelegate {
                    required property var modelData

                    text: modelData.name
                    onPressed: {
                        root.tiebreak = modelData.id;
                        root.accept();
                    }
                }
            }
        }
    }
}
