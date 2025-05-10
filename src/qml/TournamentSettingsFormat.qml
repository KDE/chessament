// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: i18nc("@title", "Format")

    FormCard.FormHeader {
        title: i18nc("@title:group", "Format")
    }
    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: i18n("Swiss system")
        }

        FormCard.FormSpinBoxDelegate {
            label: i18n("Rounds")
            from: 1
            to: 99
            value: Controller.tournament.numberOfRounds
            onValueChanged: {
                if (Controller.tournament.numberOfRounds === value) {
                    return;
                }
                Controller.tournament.numberOfRounds = value;
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Pairings")
    }
    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: i18n("Pairing engine")
            description: i18n("bbpPairings")
        }
        FormCard.FormComboBoxDelegate {
            id: pairingBye
            text: i18n("Pairing allocated bye")
            description: i18n("The value of the pairing allocated bye")
            model: ["1 point", "0.5 points", "0 points"]
        }
    }
}
