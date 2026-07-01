// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.AbstractFormDelegate {
    id: delegate

    required property int role
    required property string title
    required property string name
    required property string arbiterId
    required property int row

    signal deleteArbiter(row: int)

    background: null

    contentItem: RowLayout {
        Controls.ComboBox {
            model: [
                {
                    value: Arbiter.Role.Chief,
                    text: KI18n.i18nc("@item:inlistbox Chief Arbiter", "Chief")
                },
                {
                    value: Arbiter.Role.Deputy,
                    text: KI18n.i18nc("@item:inlistbox Deputy Chief Arbiter", "Deputy Chief")
                },
                {
                    value: Arbiter.Role.Arbiter,
                    text: KI18n.i18nc("@item:inlistbox", "Arbiter")
                },
            ]
            textRole: "text"
            valueRole: "value"
            currentValue: delegate.role
            onActivated: delegate.role = currentValue
        }
        Controls.TextField {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 3
            text: delegate.title
            placeholderText: KI18n.i18nc("@info:placeholder Arbiter's Title", "Title")
            onTextEdited: delegate.title = text
        }

        Controls.TextField {
            Layout.fillWidth: true
            text: delegate.name
            placeholderText: KI18n.i18nc("@info:placeholder Arbiter's Name", "Name")
            onTextEdited: delegate.name = text
        }

        Controls.TextField {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 5
            text: delegate.arbiterId
            placeholderText: KI18n.i18nc("@info:placeholder Arbiter's ID", "ID")
            onTextEdited: delegate.arbiterId = text
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button", "Delete Arbiter")
            icon.name: "list-remove-symbolic"
            flat: true
            display: Controls.Button.IconOnly
            onPressed: delegate.deleteArbiter(delegate.row)

            Controls.ToolTip.text: KI18n.i18nc("@action:button", "Delete arbiter")
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        }
    }
}
