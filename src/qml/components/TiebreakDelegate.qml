// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts as Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.chessament

FormCard.FormTextDelegate {
    id: tiebreakDelegate

    required property var model
    required property int row

    required property int count
    required property TiebreakModel tiebreakModel

    text: model.display

    trailing: Layouts.RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Controls.Button {
            text: KI18n.i18nc("@action:button", "Configure tiebreak")
            icon.name: "settings-configure-symbolic"
            flat: true
            visible: tiebreakDelegate.model.isConfigurable
            display: Controls.Button.IconOnly
            Controls.ToolTip.text: KI18n.i18nc("@action:button", "Configure tiebreak")
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            onPressed: {
                const dialog = Qt.createComponent("org.kde.chessament", "TiebreakDialog").createObject(tiebreakDelegate, {
                    "options": tiebreakDelegate.model.options,
                    "parent": tiebreakDelegate.Controls.Overlay.overlay
                });
                dialog.accepted.connect(() => {
                    tiebreakDelegate.model.options = dialog.options;
                });
                dialog.open();
            }
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button Move tiebreak up", "Move Up")
            icon.name: "go-up-symbolic"
            flat: true
            enabled: tiebreakDelegate.row > 0
            display: Controls.Button.IconOnly
            Controls.ToolTip.text: KI18n.i18nc("@action:button Move tiebreak up", "Move up")
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            onPressed: tiebreakDelegate.tiebreakModel.moveUp(tiebreakDelegate.row)
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button Move tiebreak down", "Move Down")
            icon.name: "go-down-symbolic"
            flat: true
            enabled: tiebreakDelegate.row < tiebreakDelegate.count - 1
            display: Controls.Button.IconOnly
            Controls.ToolTip.text: KI18n.i18nc("@action:button Move tiebreak down", "Move down")
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            onPressed: tiebreakDelegate.tiebreakModel.moveDown(tiebreakDelegate.row)
        }

        Controls.Button {
            text: KI18n.i18nc("@action:button", "Delete Tiebreak")
            icon.name: "list-remove-symbolic"
            flat: true
            enabled: tiebreakDelegate.count > 1
            display: Controls.Button.IconOnly
            Controls.ToolTip.text: KI18n.i18nc("@action:button", "Delete tiebreak")
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            onPressed: tiebreakDelegate.tiebreakModel.remove(tiebreakDelegate.row)
        }
    }
}
