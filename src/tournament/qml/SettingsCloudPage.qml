// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: "Cloud"

    FormCard.FormHeader {
        title: "Cloud"
    }
    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: "Upload tournament"
            onClicked: Controller.uploadTournament()
        }
    }
}
