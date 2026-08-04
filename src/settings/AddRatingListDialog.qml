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

    property string error
    property bool finished: false
    property bool importing: false
    property bool waiting: !(importing || finished)

    title: {
        if (error) {
            return KI18n.i18nc("@title", "Error");
        }
        if (importing) {
            return KI18n.i18nc("@title:window", "Importing Rating List");
        }
        if (finished) {
            return KI18n.i18nc("@title:window", "Rating List Imported");
        }
        return KI18n.i18nc("@title:window", "Add Rating List");
    }

    parent: Controls.Overlay.overlay
    closePolicy: dialog.waiting || dialog.finished ? Controls.Dialog.CloseOnEscape | Controls.Dialog.CloseOnPressOutside : Controls.Dialog.NoAutoClose
    implicitWidth: Kirigami.Units.gridUnit * 20

    function importList(): void {
        dialog.error = "";
        dialog.importing = true;

        dialog.model.importRatingList(nameField.text, urlField.editText).then(error => {
            if (error) {
                dialog.error = error;
            }
            dialog.importing = false;
            dialog.finished = true;
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
        visible: dialog.waiting
    }

    FormCard.FormComboBoxDelegate {
        id: urlField
        text: KI18n.i18nc("@info:label", "Rating list file or URL")
        visible: dialog.waiting
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
        visible: dialog.importing
        background: null

        contentItem: Controls.BusyIndicator {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
        }
    }

    FormCard.FormTextDelegate {
        text: dialog.error.length > 0 ? dialog.error : dialog.model.status
        visible: !dialog.waiting
        textItem.horizontalAlignment: dialog.importing ? Text.AlignHCenter : Text.AlignLeft
    }

    footer: Controls.DialogButtonBox {
        visible: !dialog.importing
        horizontalPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        topPadding: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.mediumSpacing

        standardButtons: if (dialog.waiting) {
            return Controls.Dialog.Cancel;
        } else {
            return Controls.Dialog.Close;
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button", "Import")
            icon.name: "document-import-symbolic"
            visible: dialog.waiting
            enabled: nameField.text.trim().length > 0 && dialog.model.isSupportedUrl(urlField.editText)
            onClicked: dialog.importList()
            Controls.DialogButtonBox.buttonRole: Controls.DialogButtonBox.ActionRole
        }
    }
}
