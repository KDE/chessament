// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: KI18n.i18nc("@title:window", "Rating Variation Calculator")

    implicitWidth: 15 * Kirigami.Units.gridUnit

    readonly property var results: [
        {
            result: Pairing.PartialResult.Win,
            text: KI18n.i18nc("@item Game result", "Win")
        },
        {
            result: Pairing.PartialResult.Draw,
            text: KI18n.i18nc("@item Game result", "Draw")
        },
        {
            result: Pairing.PartialResult.Lost,
            text: KI18n.i18nc("@item Game result", "Lost")
        },
    ]

    readonly property Elo elo: Elo {}

    property eloChange variation

    function recalculate(): void {
        if (!opponentRating || !gameResult) {
            return;
        }

        root.elo.rating = playerRating.value;
        root.elo.k = kFactor.value;

        const change = root.elo.change(opponentRating.value, root.results[gameResult.selectedIndex].result);
        root.variation = change;
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormSpinBoxDelegate {
            id: playerRating
            label: KI18n.i18nc("@label:spinbox", "Player’s rating:")
            from: 0
            to: 4000
            value: 1500
            onValueChanged: root.recalculate()
        }

        FormCard.FormSpinBoxDelegate {
            id: opponentRating
            label: KI18n.i18nc("@label:spinbox", "Opponent’s rating:")
            from: 0
            to: 4000
            value: 1500
            onValueChanged: root.recalculate()
        }

        FormCard.FormSpinBoxDelegate {
            id: kFactor
            label: KI18n.i18nc("@label:spinbox", "K factor:")
            from: 0
            to: 40
            value: 40
            onValueChanged: root.recalculate()
        }

        FormCard.FormRadioSelectorDelegate {
            id: gameResult
            actions: root.results.map(function (result): Kirigami.Action {
                return Qt.createComponent("org.kde.kirigami", "Action").createObject(gameResult, {
                    text: result.text
                }) as Kirigami.Action;
            })
            onSelectedIndexChanged: root.recalculate()
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@label", "Rating variation:")
            description: root.variation.change.toLocaleString()
            descriptionItem.color: if (root.variation.change > 0) {
                return Kirigami.Theme.positiveTextColor;
            } else if (root.variation.change < 0) {
                return Kirigami.Theme.negativeTextColor;
            } else {
                return Kirigami.Theme.neutralTextColor;
            }
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@label", "Expected score:")
            description: root.variation.expectedScore.toLocaleString()
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@label", "Rating difference:")
            description: root.variation.ratingDifference.toLocaleString()
        }
    }

    Component.onCompleted: recalculate()
}
