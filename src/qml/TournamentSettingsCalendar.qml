// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: i18nc("@title", "Calendar")

    Repeater {
        model: RoundModel {
            id: roundModel
            tournament: Controller.tournament
        }

        ColumnLayout {
            id: delegate

            required property int index
            required property var model

            FormCard.FormHeader {
                title: i18nc("round number", "Round %1", delegate.index + 1)
            }
            FormCard.FormCard {
                FormCard.FormDateTimeDelegate {
                    value: delegate.model.dateTime
                    onValueChanged: {
                        delegate.model.dateTime = value;
                    }
                }
            }
        }
    }
}
