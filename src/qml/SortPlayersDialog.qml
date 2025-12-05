// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormCardDialog {
    id: root

    title: i18nc("@title", "Sort Players")
    standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel

    onAccepted: {
        Controller.sortPlayers();
    }

    Kirigami.InlineMessage {
        type: Kirigami.MessageType.Warning
        text: i18nc("@info", "Changing the initial ranking after the fourth round has been paired is not allowed in official tournaments.")
        visible: Controller.tournament.currentRound >= 4
        implicitWidth: root.width
        position: Kirigami.InlineMessage.Position.Header
    }
}
