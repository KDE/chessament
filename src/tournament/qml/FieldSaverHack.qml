// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

import QtQuick
import QtQuick.Controls as Controls

// This is a dummy item to be used in ConfigurationView pages so that when it
// closes the onEditingFinished signal gets emited by the focused text field.
// Otherwise we'll may lose the data of the focused text field.
Item {
    id: root

    Connections {
        target: root.Controls.ApplicationWindow.window

        function onClosing(event): void {
            root.forceActiveFocus();
        }
    }
}
