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

    property bool waiting: false
    property bool finished: false

    parent: Controls.Overlay.overlay
    title: KI18n.i18nc("@title", "Delete Rating List")
    closePolicy: Controls.Dialog.NoAutoClose
    showCloseButton: false
    padding: Kirigami.Units.largeSpacing

    Layouts.ColumnLayout {
        Controls.BusyIndicator {
            Layouts.Layout.fillWidth: true
            Layouts.Layout.alignment: Qt.AlignHCenter

            visible: dialog.waiting
        }
        Controls.Label {
            text: KI18n.i18nc("@label", "Permanently delete rating list \"%1\"?", dialog.name)
            visible: !(dialog.finished || dialog.waiting)
        }
    }

    footer: Controls.DialogButtonBox {
        visible: !(dialog.finished || dialog.waiting)

        standardButtons: if (dialog.finished && !dialog.waiting) {
            return Controls.Dialog.Close;
        } else if (!dialog.waiting) {
            return Controls.Dialog.Cancel;
        } else {
            return Controls.Dialog.NoButton;
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button Delete Rating List", "Delete")
            icon.name: "delete-symbolic"
            visible: !(dialog.finished || dialog.waiting)
            onClicked: function (): void {
                dialog.waiting = true;

                dialog.model.deleteList(dialog.row).then(() => {
                    dialog.close();
                });
            }
        }
    }
}
