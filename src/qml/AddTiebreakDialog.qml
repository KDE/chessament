// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament.libtournament

FormCard.FormCardDialog {
    id: root

    required property Tournament tournament

    property string tiebreak

    title: i18nc("@title:window", "Add Tiebreak")

    Repeater {
        model: root.tournament.availableTiebreaks()

        FormCard.FormButtonDelegate {
            required property var modelData

            text: modelData.name
            onPressed: {
                root.tiebreak = modelData.id;
                root.accept();
            }
        }
    }
}
