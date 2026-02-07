// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament
import org.kde.chessament.tournament

FormCard.FormCardDialog {
    id: root

    readonly property list<Player> byes: Controller.tournament.voluntaryByes(Controller.tournament.currentRound + 1)
    readonly property string players: "\n<ul>" + byes.map(p => "<li>" + p.name + "</li>").join("\n") + "</ul>"

    title: KI18n.i18nc("@title %1 is a number >= 1", "Pair Round %1", Controller.tournament.currentRound + 1)
    standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel

    onAccepted: {
        Controller.pairRound(sortPlayers.checked, initialColor.currentValue);
    }

    FormCard.FormTextDelegate {
        text: root.byes.length === 0 ? KI18n.i18nc("@info", "All players will be paired.") : KI18n.i18ncp("@info %1 is a number", "The following player won't be paired: %2", "The following %1 players won't be paired: %2", root.byes.length, root.players)
        textItem {
            textFormat: Text.StyledText
            elide: Text.ElideNone
            wrapMode: Text.Wrap
        }
    }

    FormCard.FormCheckDelegate {
        id: sortPlayers
        text: KI18n.i18nc("@option:check", "Sort players")
        checked: true
        visible: Controller.tournament.currentRound === 0
    }

    FormCard.FormComboBoxDelegate {
        id: initialColor
        text: KI18n.i18nc("@label:listbox", "First player color:")
        visible: Controller.tournament.currentRound === 0
        currentIndex: 0
        textRole: "text"
        valueRole: "value"
        model: [
            {
                value: 2,
                text: KI18n.i18nc("@item:inlistbox As in 'Random color'", "Random")
            },
            {
                value: Tournament.InitialColor.White,
                text: KI18n.i18nc("@item:inlistbox As in 'White pieces'", "White")
            },
            {
                value: Tournament.InitialColor.Black,
                text: KI18n.i18nc("@item:inlistbox As in 'Black pieces'", "Black")
            }
        ]
    }
}
