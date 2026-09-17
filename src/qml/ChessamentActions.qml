// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.kirigamiaddons.actions as Actions

Item {
    id: root

    required property Actions.Application application

    Actions.ActionCollection {
        name: "org.kde.chessament.file"
        text: KI18n.i18nc("@title", "Chessament")
        application: root.application

        Actions.ActionData {
            name: "new_tournament"
            icon.name: "document-new-symbolic"
            text: KI18n.i18nc("@action:inmenu", "New tournament…")
            defaultShortcut: "Ctrl+N"
        }

        Actions.StandardActionData {
            standardAction: Actions.StandardActionData.Open
        }

        Actions.StandardActionData {
            standardAction: Actions.StandardActionData.SaveAs
        }

        Actions.ActionData {
            name: "import_trf"
            icon.name: "document-import-symbolic"
            text: KI18n.i18nc("@action:inmenu", "Import tournament report…")
            defaultShortcut: "Ctrl+I"
        }

        Actions.ActionData {
            name: "export_trf"
            icon.name: "document-export-symbolic"
            text: KI18n.i18nc("@action:inmenu", "Export tournament report…")
        }

        Actions.StandardActionData {
            standardAction: Actions.StandardActionData.Quit
        }
    }

    Actions.ActionCollection {
        name: "org.kde.chessament.tournament"
        text: KI18n.i18nc("@title:menu", "Tournament")
        application: root.application

        Actions.ActionData {
            name: "add_player"
            icon.name: "list-add-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Add Player…")
            defaultShortcut: "Ctrl+A"
        }

        Actions.ActionData {
            name: "print_document"
            icon.name: "document-print-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Print…")
            defaultShortcut: "Ctrl+P"
        }

        Actions.ActionData {
            name: "update_players"
            icon.name: "view-refresh-symbolic"
            text: KI18n.i18nc("@action Update players ratings", "Update ratings…")
        }
    }

    Actions.ActionCollection {
        name: "org.kde.chessament.pairings"
        text: KI18n.i18nc("@title:menu", "Pairings")
        application: root.application

        Actions.ActionData {
            name: "hide_finished_games"
            text: KI18n.i18nc("@option:check", "Hide finished games")
            checkable: true
        }
    }

    Actions.ActionCollection {
        name: "org.kde.chessament.help"
        text: KI18n.i18nc("@title:menu", "Help")
        application: root.application

        Actions.StandardActionData {
            standardAction: Actions.StandardActionData.HelpContents
        }

        Actions.StandardActionData {
            standardAction: Actions.StandardActionData.ReportBug
        }

        Actions.StandardActionData {
            standardAction: Actions.StandardActionData.Donate
        }
    }
}
