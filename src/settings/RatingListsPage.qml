// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: KI18n.i18nc("@title", "Rating Lists")

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
                    onPressed: function (): void {
                        const dialog = Qt.createComponent("org.kde.chessament.settings", "DeleteRatingListDialog").createObject(root, {
                            "model": listsModel,
                            "row": listDelegate.row,
                            "name": listDelegate.name
                        }) as DeleteRatingListDialog;
                        dialog.open();
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
