// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

pragma ComponentBehavior: Bound

import QtQml.Models as Models
import QtQuick
import QtQuick.Controls as Controls

import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: root

    required property var options

    title: i18nc("@title:window", "Edit Tiebreak")
    standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel

    Repeater {
        model: root.options

        Models.DelegateChooser {
            role: "type"

            Models.DelegateChoice {
                roleValue: "number"

                FormCard.FormSpinBoxDelegate {
                    required property var modelData
                    required property int row

                    label: modelData.name
                    value: modelData.value
                    onValueChanged: root.options[row].value = value
                }
            }

            Models.DelegateChoice {
                roleValue: "checkbox"

                FormCard.FormCheckDelegate {
                    required property var modelData
                    required property int row

                    text: modelData.name
                    checked: modelData.value
                    onCheckStateChanged: root.options[row].value = checked
                }
            }
        }
    }
}
