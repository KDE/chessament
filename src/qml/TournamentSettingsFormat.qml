// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts as Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

// import org.kde.chessament.libtournament

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
            visible: Config.developer
            text: i18n("Pairing allocated bye")
            description: i18n("The value of the pairing allocated bye")
            model: ["1 point", "0.5 points", "0 points"]
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Tiebreaks")
    }
    FormCard.FormCard {
        Repeater {
            model: TiebreakModel {
                tournament: Controller.tournament
            }

            FormCard.FormTextDelegate {
                text: model.display

                trailing: Layouts.RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.Button {
                        text: "Move up"
                        icon.name: "go-up-symbolic"
                        flat: true
                        display: Controls.Button.IconOnly
                    }

                    Controls.Button {
                        text: "Move down"
                        icon.name: "go-down-symbolic"
                        flat: true
                        display: Controls.Button.IconOnly
                    }

                    Controls.Button {
                        text: "Delete"
                        icon.name: "list-remove-symbolic"
                        flat: true
                        display: Controls.Button.IconOnly
                    }
                }
            }
        }
    }
}
