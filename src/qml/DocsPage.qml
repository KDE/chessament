// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

import QtCore
import QtQuick
import QtQuick.Dialogs
import QtWebEngine

import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.Page {
    id: root

    padding: 0

    actions: [
        Kirigami.Action {
            id: addAction
            icon.name: "document-save-symbolic"
            text: i18nc("@action:button", "Save As…")
            onTriggered: saveDialog.open()
        }
    ]

    FileDialog {
        id: saveDialog
        fileMode: FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: ["PDF Files (*.pdf)"]
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            webView.printToPdf(new URL(selectedFile).pathname);
        }
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
    }

    Component.onCompleted: {
        webView.loadHtml(Controller.getPlayersListDocument(), "http://localhost");
    }
}
