// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.Page {
    id: root

    required property AbstractKirigamiApplication application

    title: KI18n.i18nc("@title", "Chessament")

    Kirigami.PlaceholderMessage {
        spacing: Kirigami.Units.gridUnit
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: KI18n.i18nc("@title", "Welcome to Chessament")
        explanation: KI18n.i18nc("@info", "Chessament is a chess tournament manager")
        helpfulAction: Kirigami.Action {
            fromQAction: root.application.action("file_new")
        }
        anchors.centerIn: parent
    }
}
