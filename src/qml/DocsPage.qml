// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>

pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Dialogs as Dialogs
import QtWebEngine as WebEngine

import org.kde.kirigami as Kirigami

import org.kde.chessament

Kirigami.Page {
    id: root

    padding: 0

    actions: [
        Kirigami.Action {
            icon.name: "document-print-symbolic"
            text: i18nc("@action:button", "Print…")
            visible: Config.developer
            onTriggered: {
                const fileName = Controller.createTempFile();
                content.item.printToPdf(fileName);
                Controller.printDocument();
            }
        },
        Kirigami.Action {
            id: addAction
            icon.name: "document-save-symbolic"
            text: i18nc("@action:button", "Save As…")
            onTriggered: saveDialog.open()
        }
    ]

    Dialogs.FileDialog {
        id: saveDialog
        fileMode: Dialogs.FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: ["PDF Files (*.pdf)"]
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            content.item.printToPdf(new URL(selectedFile).pathname);
        }
    }

    Loader {
        id: content
        anchors.fill: parent
        asynchronous: true
        sourceComponent: WebEngine.WebEngineView {
            backgroundColor: "transparent"
            settings.preferCSSMarginsForPrinting: true
            settings.javascriptEnabled: false
        }
        onLoaded: {
            const document = Controller.getStartingRankDocument();
            content.item.loadHtml(document, "http://localhost");
        }
    }
}
