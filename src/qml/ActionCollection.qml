// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import org.kde.ki18n
import org.kde.kirigami.actioncollection as AC

AC.ActionCollectionManager {
    AC.ActionCollection {
        name: "org.kde.chessament.file"
        text: KI18n.i18nc("@action:menu", "File")

        AC.ActionData {
            name: "NewTournament"
            icon.name: "document-new-symbolic"
            text: KI18n.i18nc("@action:inmenu", "New tournament…")
            defaultShortcut: "Ctrl+N"
        }

        AC.StandardActionData {
            standardAction: AC.StandardActionData.Open
        }

        AC.StandardActionData {
            standardAction: AC.StandardActionData.SaveAs
        }

        AC.ActionData {
            name: "ImportTrf"
            icon.name: "document-import-symbolic"
            text: KI18n.i18nc("@action:inmenu", "Import tournament report…")
            defaultShortcut: "Ctrl+I"
        }

        AC.ActionData {
            name: "ExportTrf"
            icon.name: "document-export-symbolic"
            text: KI18n.i18nc("@action:inmenu", "Export tournament report…")
        }

        AC.StandardActionData {
            standardAction: AC.StandardActionData.Quit
        }
    }

    AC.ActionCollection {
        name: "org.kde.chessament.tournament"
        text: KI18n.i18nc("@action:menu", "Tournament")

        AC.ActionData {
            name: "AddPlayer"
            icon.name: "list-add-symbolic"
            text: KI18n.i18nc("@action:intoolbar", "Add Player…")
            defaultShortcut: "Ctrl+A"
        }
    }

    AC.ActionCollection {
        name: "org.kde.chessament.help"
        text: KI18n.i18nc("@action:menu", "Help")

        AC.StandardActionData {
            standardAction: AC.StandardActionData.HelpContents
        }
    }
}
