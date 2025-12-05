// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament
import org.kde.chessament.tournament

FormCard.FormCardDialog {
    id: root

    title: i18nc("@title %1 is a number >= 1", "Pair Round %1", Controller.tournament.currentRound + 1)
    standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel

    onAccepted: {
        Controller.pairRound(sortPlayers.checked, initialColor.currentValue);
    }

    FormCard.FormCheckDelegate {
        id: sortPlayers
        text: i18nc("@option:check", "Sort players")
        checked: true
        visible: Controller.tournament.currentRound === 0
    }

    FormCard.FormComboBoxDelegate {
        id: initialColor
        text: i18nc("@label:listbox", "First player color:")
        visible: Controller.tournament.currentRound === 0
        currentIndex: 0
        textRole: "text"
        valueRole: "value"
        model: [
            {
                value: 2,
                text: i18nc("@item:inlistbox As in 'Random color'", "Random")
            },
            {
                value: Tournament.InitialColor.White,
                text: i18nc("@item:inlistbox As in 'White pieces'", "White")
            },
            {
                value: Tournament.InitialColor.Black,
                text: i18nc("@item:inlistbox As in 'Black pieces'", "Black")
            }
        ]
    }
}
