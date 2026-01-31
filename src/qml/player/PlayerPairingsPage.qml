// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    required property Tournament tournament
    required property Player player

    title: KI18n.i18nc("@title", "Pairings")

    actions: [
        Kirigami.Action {
            icon.name: "im-kick-user-symbolic"
            text: KI18n.i18nc("@action:button", "Retire player")
            enabled: root.tournament.currentRound < root.tournament.numberOfRounds
            onTriggered: byesModel.retire()
        }
    ]

    Component {
        id: statusChooser

        Controls.ComboBox {
            id: comboBox

            required property int round
            required property int result

            model: [
                {
                    text: KI18n.i18nc("@item:inlistbox Available to play", "Available"),
                    result: Pairing.PartialResult.Unknown
                },
                {
                    text: "Bye (1)",
                    result: Pairing.PartialResult.FullBye
                },
                {
                    text: "Bye (½)",
                    result: Pairing.PartialResult.HalfBye
                },
                {
                    text: "Bye (0)",
                    result: Pairing.PartialResult.ZeroBye
                }
            ]
            textRole: "text"
            valueRole: "result"
            currentValue: comboBox.result
            onActivated: {
                Controller.setBye(root.tournament, root.player, comboBox.round, comboBox.currentValue);
                byesModel.reload();
            }
        }
    }

    Component {
        id: resultLabel

        Controls.Label {
            required property string result

            text: result
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Pairings")
    }
    FormCard.FormCard {
        Repeater {
            id: repeater

            model: ByesModel {
                id: byesModel

                tournament: root.tournament
                player: root.player

                onErrorOcurred: function (error: string): void {
                    Controller.setError(error);
                }
            }

            FormCard.FormTextDelegate {
                id: pairing

                required property int row
                required property string name
                required property int result
                required property string resultString

                text: KI18n.i18nc("@info:label", "Round %1", pairing.row + 1)
                description: {
                    if (pairing.row >= repeater.model.tournament.currentRound) {
                        return "";
                    }
                    return pairing.name;
                }

                trailing: {
                    if (pairing.row >= repeater.model.tournament.currentRound) {
                        return statusChooser.createObject(null, {
                            "round": pairing.row + 1,
                            "result": Qt.binding(() => pairing.result)
                        });
                    }
                    return resultLabel.createObject(null, {
                        "result": Qt.binding(() => pairing.resultString)
                    });
                }
            }
        }
    }
}
