// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami
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
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Arbiters")
        actions: [
            Kirigami.Action {
                icon.name: "list-add-symbolic"
                text: KI18n.i18nc("@action:button", "Add Arbiter")
                onTriggered: arbitersModel.addArbiter()
            }
        ]
    }
    FormCard.FormCard {
        FormCard.FormPlaceholderMessageDelegate {
            text: KI18n.i18nc("@info:status", "No arbiters yet.")
            icon.name: "view-list-details-symbolic"
            visible: repeater.count === 0
        }

        Repeater {
            id: repeater
            model: ArbitersModel {
                id: arbitersModel
                tournament: Controller.tournament
            }
            delegateModelAccess: DelegateModel.ReadWrite
            delegate: ArbiterDelegate {
                onDeleteArbiter: row => arbitersModel.deleteArbiter(row)
            }
        }
    }
}
