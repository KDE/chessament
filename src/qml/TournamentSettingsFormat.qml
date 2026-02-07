// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: KI18n.i18nc("@title", "Format")

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Format")
    }
    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: KI18n.i18n("Swiss system")
        }

        FormCard.FormSpinBoxDelegate {
            label: KI18n.i18n("Rounds")
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
        title: KI18n.i18nc("@title:group", "Pairings")
    }
    FormCard.FormCard {
        FormCard.FormLinkDelegate {
            text: KI18n.i18n("Pairing engine")
            description: KI18n.i18n("bbpPairings")
            url: "https://github.com/BieremaBoyzProgramming/bbpPairings"
        }
        FormCard.FormComboBoxDelegate {
            id: pairingBye
            visible: Config.developer
            text: KI18n.i18n("Pairing allocated bye")
            description: KI18n.i18n("The value of the pairing allocated bye")
            model: ["1 point", "0.5 points", "0 points"]
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Tiebreaks")
    }
    FormCard.FormCard {
        Repeater {
            id: repeater

            model: TiebreakModel {
                id: tiebreakModel
                tournament: Controller.tournament

                onDataChanged: Controller.areStandingsValid = false
                onRowsInserted: Controller.areStandingsValid = false
                onRowsRemoved: Controller.areStandingsValid = false
            }

            TiebreakDelegate {
                count: repeater.count
                tiebreakModel: tiebreakModel
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: KI18n.i18nc("@action:button", "Add Tiebreak")
            icon.name: "list-add-symbolic"
            onPressed: {
                const dialog = Qt.createComponent("org.kde.chessament", "AddTiebreakDialog").createObject(root, {
                    "parent": root.Controls.Overlay.overlay,
                    "tournament": Controller.tournament
                });
                dialog.accepted.connect(() => {
                    tiebreakModel.addTiebreak(dialog.tiebreak);
                });
                dialog.open();
            }
        }
    }
}
