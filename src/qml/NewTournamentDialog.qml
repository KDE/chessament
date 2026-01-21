// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as Dialogs

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: root

    signal create(fileName: string, name: string, rounds: int)

    title: KI18n.i18nc("@title", "New tournament")
    standardButtons: QQC2.Dialog.Save | QQC2.Dialog.Cancel

    onAccepted: {
        let dialog = saveDialog.createObject(QQC2.Overlay.overlay);
        dialog.accepted.connect(() => {
            root.create(dialog.selectedFile, name.text, rounds.value);
        });
        dialog.open();
    }

    FormCard.FormTextFieldDelegate {
        id: name
        label: KI18n.i18nc("@label:textbox tournament name", "Name:")
    }

    FormCard.FormTextDelegate {
        text: KI18n.i18nc("@label", "Format")
        description: KI18n.i18nc("@label", "Swiss system")
    }

    FormCard.FormSpinBoxDelegate {
        id: rounds
        label: KI18n.i18nc("@label", "Number of rounds")
        from: 1
        to: 99
    }

    Component {
        id: saveDialog

        Dialogs.FileDialog {
            fileMode: Dialogs.FileDialog.SaveFile
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        }
    }
}
