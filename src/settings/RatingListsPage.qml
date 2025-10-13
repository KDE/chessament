// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    required property ChessamentApplication application

    title: i18nc("@title", "Rating Lists")

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: i18nc("@label:button", "Download FIDE rating list")
            action: Kirigami.Action {
                onTriggered: Controller.downloadRatingList()
            }
        }
    }
}
