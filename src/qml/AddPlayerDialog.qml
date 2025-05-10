// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

QQC2.Dialog {
    id: root

    title: i18nc("@title:window", "Add player")
    standardButtons: Kirigami.Dialog.Cancel | Kirigami.Dialog.Ok
    implicitWidth: Math.min(QQC2.ApplicationWindow.window.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 35)
    implicitHeight: Math.min(QQC2.ApplicationWindow.window.height - Kirigami.Units.gridUnit * 4, contentItem.implicitHeight)
    anchors.centerIn: parent
    modal: true

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    onAccepted: {
        Controller.addPlayer(titleField.currentText, nameField.text, ratingField.value, nationalRatingField.value, playerIdField.text, birthDateField.text, originField.text, sexField.text);
    }

    header: Kirigami.DialogHeader {
        dialog: root

        contentItem: Kirigami.DialogHeaderTopContent {
            dialog: root
        }
    }

    contentItem: Kirigami.ScrollablePage {
        background: null

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        ColumnLayout {
            spacing: 0

            FormCard.FormComboBoxDelegate {
                id: titleField
                text: i18nc("@label:textbox", "Title")
                model: ["", "GM", "IM", "FM", "WGM", "CM", "WIM", "WFM", "WCM"]
            }

            FormCard.FormTextFieldDelegate {
                id: nameField
                label: i18nc("@label:textbox", "Name")
            }

            FormCard.FormSpinBoxDelegate {
                id: ratingField
                label: i18nc("@label:textbox", "Rating")
                from: 0
                to: 4000
            }

            FormCard.FormSpinBoxDelegate {
                id: nationalRatingField
                label: i18nc("@label:textbox", "National rating")
                from: 0
                to: 4000
            }

            FormCard.FormTextFieldDelegate {
                id: playerIdField
                label: i18nc("@label:textbox", "Player ID")
                placeholderText: i18n("Optional")
            }

            FormCard.FormTextFieldDelegate {
                id: birthDateField
                label: i18nc("@label:textbox", "Birth date")
                placeholderText: i18n("Optional")
            }

            FormCard.FormTextFieldDelegate {
                id: federationField
                label: i18nc("@label:textbox", "Federation")
                placeholderText: i18n("Optional")
            }

            FormCard.FormTextFieldDelegate {
                id: originField
                label: i18nc("@label:textbox", "Origin")
                placeholderText: i18n("Optional")
            }

            FormCard.FormTextFieldDelegate {
                id: sexField
                label: i18nc("@label:textbox", "Sex")
                placeholderText: i18n("Optional")
            }
        }
    }

    Kirigami.Separator {
        anchors {
            left: root.contentItem.left
            right: root.contentItem.right
            top: root.contentItem.top
        }
    }

    Kirigami.Separator {
        anchors {
            left: root.contentItem.left
            right: root.contentItem.right
            bottom: root.contentItem.bottom
        }
    }
}
