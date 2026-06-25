// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtCore
import QtQml
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts as Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: KI18n.i18nc("@title", "Rating Lists")

    FormCard.FormCardDialog {
        id: addListDialog
        parent: Controls.Overlay.overlay
        title: KI18n.i18nc("@title:window", "Add Rating List")
        footer: Controls.DialogButtonBox {
            standardButtons: Controls.Dialog.Cancel

            Controls.Button {
                text: KI18n.i18nc("@action:button", "Import")
                icon.name: "document-import-symbolic"
                enabled: nameField.text.trim().length > 0 && listsModel.isValidUrl(urlField.editText)
                Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
            }
        }

        onAccepted: {
            importingDialog.open();
            listsModel.importRatingList(nameField.text, urlField.editText).then(() => {
                importingDialog.finished = true;
            });
        }

        Dialogs.FileDialog {
            id: fileDialog
            nameFilters: [KI18n.i18nc("@label:listbox", "All Supported Files (*.zip *.xls)")]
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
            onAccepted: urlField.editText = selectedFile.toString().replace("file://", "")
        }

        FormCard.FormTextFieldDelegate {
            id: nameField
            label: KI18n.i18nc("@info:label", "Name")
        }

        FormCard.FormComboBoxDelegate {
            id: urlField
            text: KI18n.i18nc("@info:label", "Rating list file or URL")
            editable: true
            model: ["https://ratings.fide.com/download/players_list.zip"]
            trailing: Controls.Button {
                icon.name: "document-open-data-symbolic"
                text: KI18n.i18nc("@action:button", "Select file")
                display: Controls.AbstractButton.IconOnly
                onPressed: fileDialog.open()
                Controls.ToolTip.text: text
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }
    }

    Kirigami.Dialog {
        id: importingDialog

        property bool finished: false

        parent: Controls.Overlay.overlay
        title: finished ? KI18n.i18nc("@title:window", "Rating List Imported") : KI18n.i18nc("@title:window", "Importing Rating List")
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

        property int row
        property string name

        parent: Controls.Overlay.overlay
        title: KI18n.i18nc("@title", "Delete Rating List")
        subtitle: KI18n.i18nc("@label", "Permanently delete rating list \"%1\"?", deleteListDialog.name)

        onAccepted: listsModel.removeList(deleteListDialog.row)

        footer: Controls.DialogButtonBox {
            standardButtons: Controls.Dialog.Cancel

            Controls.Button {
                text: KI18n.i18nc("@action:button Delete Rating List", "Delete")
                icon.name: "delete-symbolic"
                Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.AcceptRole
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
            onClicked: addListDialog.open()
        }
    }
}
