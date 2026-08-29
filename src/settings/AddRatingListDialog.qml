// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtCore
import QtQml
import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardDialog {
    id: dialog

    required property RatingListModel model

    property string error: ""

    StateGroup {
        id: stateGroup
        states: [
            State {
                name: "importing"
                PropertyChanges {
                    dialog.closePolicy: Controls.Dialog.NoAutoClose
                    nameField.visible: false
                    urlField.visible: false
                    busyIndicator.visible: true
                    label {
                        visible: true
                        textItem.horizontalAlignment: Text.AlignHCenter
                    }
                    dialog.footer.visible: false
                    importButton.visible: false
                }
            },
            State {
                name: "finished"
                PropertyChanges {
                    nameField.visible: false
                    urlField.visible: false
                    label.visible: true
                    importButton.visible: false
                    buttonBox.standardButtons: Controls.Dialog.Close
                }
            }
        ]
    }

    title: {
        if (error) {
            return KI18n.i18nc("@title", "Error");
        }
        if (stateGroup.state === "importing") {
            return KI18n.i18nc("@title:window", "Importing Rating List");
        }
        if (stateGroup.state === "finished") {
            return KI18n.i18nc("@title:window", "Rating List Imported");
        }
        return KI18n.i18nc("@title:window", "Add Rating List");
    }

    parent: Controls.Overlay.overlay
    implicitWidth: Kirigami.Units.gridUnit * 20

    function importList(): void {
        stateGroup.state = "importing";

        dialog.model.importRatingList(nameField.text, urlField.editText).then(error => {
            if (error) {
                dialog.error = error;
            }
            stateGroup.state = "finished";
        });
    }

    Dialogs.FileDialog {
        id: fileDialog
        nameFilters: [KI18n.i18nc("@label:listbox", "All Supported Files (*.zip, *.xls)")]
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

    FormCard.AbstractFormDelegate {
        id: busyIndicator
        visible: false
        background: null

        contentItem: Controls.BusyIndicator {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
        }
    }

    FormCard.FormTextDelegate {
        id: label
        text: dialog.error.length > 0 ? dialog.error : dialog.model.status
        textItem {
            elide: Text.ElideNone
            wrapMode: Text.Wrap
        }
        visible: false
    }

    footer: Controls.DialogButtonBox {
        id: buttonBox
        horizontalPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        topPadding: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.mediumSpacing

        standardButtons: Controls.Dialog.Cancel

        Controls.Button {
            id: importButton
            text: KI18n.i18nc("@action:button", "Import")
            icon.name: "document-import-symbolic"
            enabled: nameField.text.trim().length > 0 && dialog.model.isSupportedUrl(urlField.editText)
            onClicked: dialog.importList()

            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.ActionRole
        }
    }
}
