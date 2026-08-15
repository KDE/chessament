// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: KI18n.i18nc("@title", "Calendar")

    Repeater {
        delegateModelAccess: DelegateModel.ReadWrite
        model: RoundModel {
            tournament: Controller.tournament
        }

        ColumnLayout {
            id: delegate

            required property int index
            required property date dateTime

            FormCard.FormHeader {
                title: KI18n.i18nc("round number", "Round %1", delegate.index + 1)
            }
            FormCard.FormCard {
                FormCard.FormDateTimeDelegate {
                    value: delegate.dateTime
                    onValueChanged: delegate.dateTime = value
                }
            }
        }
    }
}
