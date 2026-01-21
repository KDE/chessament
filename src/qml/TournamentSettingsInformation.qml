// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardPage {
    id: root

    title: KI18n.i18nc("@title", "Information")

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Basic details")
    }
    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: name
            label: KI18n.i18nc("@label:textbox", "Tournament name")
            text: Controller.tournament.name
            onEditingFinished: {
                Controller.tournament.name = text;
            }
        }

        FormCard.FormTextFieldDelegate {
            id: city
            label: KI18n.i18nc("@label:textbox", "City")
            text: Controller.tournament.city
            onEditingFinished: {
                Controller.tournament.city = text;
            }
        }

        FormCard.FormTextFieldDelegate {
            id: federation
            label: KI18n.i18nc("@label:textbox", "Federation")
            text: Controller.tournament.federation
            onEditingFinished: {
                Controller.tournament.federation = text;
            }
        }

        FormCard.FormTextFieldDelegate {
            id: chiefArbiter
            label: KI18n.i18nc("@label:textbox", "Chief arbiter")
            text: Controller.tournament.chiefArbiter
            onEditingFinished: {
                Controller.tournament.chiefArbiter = text;
            }
        }

        FormCard.FormTextFieldDelegate {
            id: deputyChiefArbiter
            label: KI18n.i18nc("@label:textbox", "Deputy chief arbiter")
            text: Controller.tournament.deputyChiefArbiter
            onEditingFinished: {
                Controller.tournament.deputyChiefArbiter = text;
            }
        }
    }
}
