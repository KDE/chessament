// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts as Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.Dialog {
    id: dialog

    required property RatingListModel model
    required property int row
    required property string name

    parent: Controls.Overlay.overlay
    title: KI18n.i18nc("@title", "Delete Rating List")
    closePolicy: Controls.Dialog.NoAutoClose
    showCloseButton: false
    padding: Kirigami.Units.largeSpacing

    StateGroup {
        id: stateGroup
        states: [
            State {
                name: "deleting"
                PropertyChanges {
                    label.visible: false
                    busyIndicator.visible: true
                    dialog.footer.visible: false
                }
            }
        ]
    }

    Layouts.ColumnLayout {
        Controls.BusyIndicator {
            id: busyIndicator
            visible: false

            Layouts.Layout.fillWidth: true
            Layouts.Layout.alignment: Qt.AlignHCenter
        }
        Controls.Label {
            id: label
            text: KI18n.i18nc("@label", "Permanently delete rating list “%1”?", dialog.name)
        }
    }

    footer: Controls.DialogButtonBox {
        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            id: deleteButton
            text: KI18n.i18nc("@action:button Delete Rating List", "Delete")
            icon.name: "delete-symbolic"
            onClicked: function (): void {
                stateGroup.state = "deleting";

                dialog.model.deleteList(dialog.row).then(() => {
                    dialog.close();
                });
            }
        }
    }
}
