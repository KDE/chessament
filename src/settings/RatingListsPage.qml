// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQml
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
            Controller.importRatingList(nameField.text, urlField.text).then(() => {
                importingDialog.finished = true;
            });
        }

        FormCard.FormTextFieldDelegate {
            id: nameField
            label: i18nc("@info:label", "Name")
        }

        FormCard.FormTextFieldDelegate {
            id: urlField
            label: i18nc("@info:label", "Rating list URL")
            text: "http://localhost:1234/players_list.zip"
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

                text: Controller.status
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title", "Rating Lists")
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: i18nc("@label:button", "Download FIDE rating list")
            action: Kirigami.Action {
                onTriggered: addListDialog.open()
            }
        }
    }
}
