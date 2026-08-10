// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts as Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: KI18n.i18nc("@title", "Rating Lists")

    Kirigami.Dialog {
        id: deleteListDialog

        property int row
        property string name
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

                visible: deleteListDialog.waiting
            }
            Controls.Label {
                text: KI18n.i18nc("@label", "Permanently delete rating list \"%1\"?", deleteListDialog.name)
                visible: !(deleteListDialog.finished || deleteListDialog.waiting)
            }
        }

        footer: Controls.DialogButtonBox {
            visible: !(deleteListDialog.finished || deleteListDialog.waiting)

            standardButtons: if (deleteListDialog.finished && !deleteListDialog.waiting) {
                return Controls.Dialog.Close;
            } else if (!deleteListDialog.waiting) {
                return Controls.Dialog.Cancel;
            } else {
                return Controls.Dialog.NoButton;
            }

            Controls.Button {
                text: KI18n.i18nc("@action:button Delete Rating List", "Delete")
                icon.name: "delete-symbolic"
                visible: !(deleteListDialog.finished || deleteListDialog.waiting)
                onClicked: function (): void {
                    deleteListDialog.waiting = true;

                    listsModel.deleteList(deleteListDialog.row).then(() => {
                        deleteListDialog.close();
                    });
                }
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title", "Rating Lists")
    }

    FormCard.FormCard {
        FormCard.FormPlaceholderMessageDelegate {
            text: KI18n.i18nc("@info:status", "No rating lists yet.")
            icon.name: "view-list-details-symbolic"
            visible: listRepeater.count === 0
        }

        Repeater {
            id: listRepeater

            model: RatingListModel {
                id: listsModel
            }

            delegate: FormTextDelegate {
                id: listDelegate

                required property string name
                required property int row

                text: name
                trailing: Controls.Button {
                    text: KI18n.i18nc("@action:button", "Delete Rating List")
                    icon.name: "list-remove-symbolic"
                    flat: true
                    display: Controls.Button.IconOnly
                    onPressed: {
                        deleteListDialog.row = listDelegate.row;
                        deleteListDialog.name = listDelegate.name;
                        deleteListDialog.waiting = false;
                        deleteListDialog.finished = false;
                        deleteListDialog.open();
                    }
                    Controls.ToolTip.text: KI18n.i18nc("@action:button", "Delete rating list")
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: KI18n.i18nc("@action:button", "Add rating list")
            onClicked: {
                const dialog = Qt.createComponent("org.kde.chessament.settings", "AddRatingListDialog").createObject(root, {
                    "model": listsModel
                }) as AddRatingListDialog;
                dialog.open();
            }
        }
    }
}
