// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts as Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: i18nc("@title", "Rating Lists")

    FormCard.FormCardDialog {
        id: addListDialog
        parent: Controls.Overlay.overlay
        title: i18nc("@title:window", "Add Rating List")
        footer: Controls.DialogButtonBox {
            standardButtons: Controls.Dialog.Cancel

            Controls.Button {
                text: i18nc("@action:button", "Import")
                icon.name: "document-import-symbolic"
                Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
            }
        }

        onAccepted: {
            importingDialog.open();
            listsModel.importRatingList(nameField.text, urlField.editText).then(() => {
                importingDialog.finished = true;
            });
        }

        FormCard.FormTextFieldDelegate {
            id: nameField
            label: i18nc("@info:label", "Name")
        }

        FormCard.FormComboBoxDelegate {
            id: urlField
            text: i18nc("@info:label", "Rating list URL")
            editable: true
            model: {
                let lists = ["https://ratings.fide.com/download/players_list.zip"];

                if (Config.developer) {
                    lists.push("http://localhost:1234/players_list.zip");
                }

                return lists;
            }
        }
    }

    Kirigami.Dialog {
        id: importingDialog

        property bool finished: false

        parent: Controls.Overlay.overlay
        title: i18nc("@title:window", "Importing Rating List")
        closePolicy: Controls.Dialog.NoAutoClose
        showCloseButton: false
        padding: Kirigami.Units.largeSpacing
        standardButtons: {
            if (importingDialog.finished) {
                return Controls.Dialog.Close;
            }

            return Controls.Dialog.NoButton;
        }

        Layouts.ColumnLayout {
            Controls.BusyIndicator {
                Layouts.Layout.fillWidth: true
                Layouts.Layout.alignment: Qt.AlignHCenter

                visible: !importingDialog.finished
            }

            Controls.Label {
                Layouts.Layout.alignment: {
                    if (importingDialog.finished) {
                        return Qt.AlignLeading;
                    }

                    return Qt.AlignHCenter;
                }

                text: listsModel.status
            }
        }
    }

    Kirigami.PromptDialog {
        id: deleteListDialog

        property string listId

        parent: Controls.Overlay.overlay
        title: i18nc("@title", "Delete Rating List")

        onAccepted: listsModel.removeList(deleteListDialog.listId)

        footer: Controls.DialogButtonBox {
            standardButtons: Controls.Dialog.Cancel

            Controls.Button {
                text: i18nc("@action:button Delete Rating List", "Delete")
                icon.name: "delete-symbolic"
                Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title", "Rating Lists")
    }

    FormCard.FormCard {
        FormCard.FormPlaceholderMessageDelegate {
            text: i18nc("@info:status", "No rating lists yet.")
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
                    text: i18nc("@action:button", "Delete Rating List")
                    icon.name: "list-remove-symbolic"
                    flat: true
                    display: Controls.Button.IconOnly
                    onPressed: {
                        deleteListDialog.listId = listDelegate.row;
                        deleteListDialog.open();
                    }
                    Controls.ToolTip.text: i18nc("@action:button", "Delete rating list")
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Add rating list")
            onClicked: addListDialog.open()
        }
    }
}
