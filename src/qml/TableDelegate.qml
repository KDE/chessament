// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQml.Models
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components

T.ItemDelegate {
    id: root

    required property int row
    required property int column
    required property bool selected
    required property bool current
    property Components.ConvergentContextMenu contextMenu: null
    readonly property bool rowHovered: root.tableView.hoveredRow === row || hovered
    required property TableView tableView
    required property var model

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding, implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding, Kirigami.Units.gridUnit * 2)

    padding: Kirigami.Settings.tabletMode ? Kirigami.Units.largeSpacing : Kirigami.Units.mediumSpacing
    spacing: Kirigami.Settings.tabletMode ? Kirigami.Units.largeSpacing * 2 : Kirigami.Units.smallSpacing

    horizontalPadding: padding + Math.round(Kirigami.Units.smallSpacing / 2)
    leftPadding: horizontalPadding
    rightPadding: horizontalPadding

    verticalPadding: padding
    topPadding: verticalPadding
    bottomPadding: verticalPadding

    onClicked: {
        const selectionModel = root.tableView.selectionModel;
        selectionModel.clear();
        selectionModel.setCurrentIndex(root.tableView.model.index(root.row, root.column), ItemSelectionModel.SelectCurrent);
        root.forceActiveFocus();
    }

    icon {
        width: if (contentItem instanceof Delegates.SubtitleContentItem) {
            return Kirigami.Units.iconSizes.large;
        } else {
            return Kirigami.Settings.tabletMode ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.sizeForLabels;
        }

        height: if (contentItem instanceof Delegates.SubtitleContentItem) {
            return Kirigami.Units.iconSizes.large;
        } else {
            return Kirigami.Settings.tabletMode ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.sizeForLabels;
        }
    }

    background: Rectangle {
        color: if (root.highlighted || root.selected || root.current || (root.down && !root.checked) || root.visualFocus || (root.row == root.tableView.currentRow && root.tableView.selectionBehavior == TableView.SelectRows)) {
            const highlight = Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.highlightColor, 0.3);
            if (root.rowHovered) {
                return Kirigami.ColorUtils.tintWithAlpha(highlight, Kirigami.Theme.textColor, 0.10);
            } else if (highlight.valid) {
                return highlight;
            } else {
                return Kirigami.Theme.backgroundColor;
            }
        } else if (root.rowHovered) {
            return Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.10);
        } else {
            return root.row % 2 == 0 ? Kirigami.Theme.backgroundColor : Kirigami.Theme.alternateBackgroundColor;
        }

        border {
            color: Kirigami.Theme.highlightColor
            width: 0 // root.visualFocus || root.activeFocus ? 1 : 0
        }
    }

    onHoveredChanged: if (hovered) {
        root.tableView.hoveredRow = root.row;
    }

    hoverEnabled: true

    contentItem: Delegates.DefaultContentItem {
        itemDelegate: root
        labelItem.horizontalAlignment: root.model.textAlignment ?? Qt.AlignLeading
    }

    Accessible.role: Accessible.Cell

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: {
            const selectionModel = root.tableView.selectionModel;
            selectionModel.clear();
            selectionModel.setCurrentIndex(root.tableView.model.index(root.row, 0), ItemSelectionModel.SelectCurrent | ItemSelectionModel.Rows);
            if (root.contextMenu) {
                root.contextMenu.popup();
            }
        }
    }
}
