// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.AbstractFormDelegate {
    id: root

    required property string label

    required property var suggestionModel

    property alias textRole: searchField.textRole

    property string subtitleRole

    property alias field: searchField

    property alias value: searchField.text

    background: null
    onClicked: searchField.forceActiveFocus()

    contentItem: ColumnLayout {
        Controls.Label {
            text: root.label
        }

        Controls.SearchField {
            id: searchField

            Layout.fillWidth: true

            delegate: Controls.ItemDelegate {
                id: delegate

                required property int index
                required property var model

                width: ListView.view.width
                highlighted: searchField.highlightedIndex === index
                hoverEnabled: searchField.hoverEnabled

                contentItem: Delegates.TitleSubtitle {
                    title: delegate.model[searchField.textRole]
                    subtitle: delegate.model[root.subtitleRole]
                    selected: delegate.highlighted || delegate.down
                }
            }

            suggestionModel: root.suggestionModel
        }
    }
}
