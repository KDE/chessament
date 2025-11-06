// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.chessament

Controls.MenuBar {
    id: root

    required property ChessamentApplication application

    Controls.Menu {
        title: i18nc("@action:menu", "File")

        Kirigami.Action {
            fromQAction: root.application.action("file_new")
        }
        Kirigami.Action {
            fromQAction: root.application.action("file_open")
        }
        Kirigami.Action {
            fromQAction: root.application.action("file_save_as")
            enabled: Controller.hasOpenTournament
        }
        Kirigami.Action {
            fromQAction: root.application.action("import_trf")
        }
        Kirigami.Action {
            fromQAction: root.application.action("export_trf")
            enabled: Controller.hasOpenTournament
        }
    }

    Controls.Menu {
        title: i18nc("@action:menu", "Settings")

        Kirigami.Action {
            fromQAction: root.application.action("options_configure_keybinding")
        }
        Kirigami.Action {
            fromQAction: root.application.action("options_configure")
        }
    }

    Controls.Menu {
        title: i18nc("@action:menu", "Help")

        Kirigami.Action {
            fromQAction: root.application.action("open_about_page")
        }

        Kirigami.Action {
            fromQAction: root.application.action("open_about_kde_page")
        }
    }
}
