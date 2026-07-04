// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts as Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.chessament

QQC2.ToolBar {
    id: root

    property Pairing pairing: null

    signal saveResult(pairing: Pairing, whiteResult: int, blackResult: int)

    position: QQC2.ToolBar.Footer
    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    implicitHeight: 40

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Header

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
    }

    QQC2.ButtonGroup {
        id: results
        buttons: row.children.filter(item => item instanceof QQC2.ToolButton)
    }

    Layouts.RowLayout {
        id: row

        QQC2.ToolButton {
            text: Controller.resultToString(Pairing.PartialResult.Unknown, Pairing.PartialResult.Unknown)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.Unknown && root.pairing?.blackResult === Pairing.PartialResult.Unknown
            onClicked: root.setResult(Pairing.PartialResult.Unknown, Pairing.PartialResult.Unknown)
        }
        Kirigami.Separator {
            Layouts.Layout.fillHeight: true
        }
        QQC2.ToolButton {
            id: whiteWins
            text: Controller.resultToString(Pairing.PartialResult.Win, Pairing.PartialResult.Lost)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.Win && root.pairing?.blackResult === Pairing.PartialResult.Lost
            onClicked: root.setResult(Pairing.PartialResult.Win, Pairing.PartialResult.Lost)
        }
        QQC2.ToolButton {
            id: draw
            text: Controller.resultToString(Pairing.PartialResult.Draw, Pairing.PartialResult.Draw)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.Draw && root.pairing?.blackResult === Pairing.PartialResult.Draw
            onClicked: root.setResult(Pairing.PartialResult.Draw, Pairing.PartialResult.Draw)
        }
        QQC2.ToolButton {
            id: blackWins
            text: Controller.resultToString(Pairing.PartialResult.Lost, Pairing.PartialResult.Win)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.Lost && root.pairing?.blackResult === Pairing.PartialResult.Win
            onClicked: root.setResult(Pairing.PartialResult.Lost, Pairing.PartialResult.Win)
        }
        Kirigami.Separator {
            Layouts.Layout.fillHeight: true
        }
        QQC2.ToolButton {
            id: whiteWinsForfeit
            text: Controller.resultToString(Pairing.PartialResult.WinForfeit, Pairing.PartialResult.LostForfeit)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.WinForfeit && root.pairing?.blackResult === Pairing.PartialResult.LostForfeit
            onClicked: root.setResult(Pairing.PartialResult.WinForfeit, Pairing.PartialResult.LostForfeit)
        }
        QQC2.ToolButton {
            id: blackWinsForfeit
            text: Controller.resultToString(Pairing.PartialResult.LostForfeit, Pairing.PartialResult.WinForfeit)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.LostForfeit && root.pairing?.blackResult === Pairing.PartialResult.WinForfeit
            onClicked: root.setResult(Pairing.PartialResult.LostForfeit, Pairing.PartialResult.WinForfeit)
        }
        QQC2.ToolButton {
            id: bothForfeit
            text: Controller.resultToString(Pairing.PartialResult.LostForfeit, Pairing.PartialResult.LostForfeit)
            checkable: true
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checked: root.pairing?.whiteResult === Pairing.PartialResult.LostForfeit && root.pairing?.blackResult === Pairing.PartialResult.LostForfeit
            onClicked: root.setResult(Pairing.PartialResult.LostForfeit, Pairing.PartialResult.LostForfeit)
        }
        Kirigami.Separator {
            Layouts.Layout.fillHeight: true
        }
        QQC2.ToolButton {
            text: KI18n.i18nc("@action:intoolbar Other game results", "Other")
            enabled: root.pairing !== null && root.pairing.blackPlayer !== null
            checkable: true
            checked: root.pairing && !whiteWins.checked && !draw.checked && !blackWins.checked && !whiteWinsForfeit.checked && !blackWinsForfeit.checked && !bothForfeit.checked
            down: pressed || otherMenu.opened
            onClicked: otherMenu.open()

            QQC2.Menu {
                id: otherMenu
                y: -otherMenu.height

                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.WinUnrated, Pairing.PartialResult.LostUnrated)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.WinUnrated && root.pairing?.blackResult === Pairing.PartialResult.LostUnrated
                    onClicked: root.setResult(Pairing.PartialResult.WinUnrated, Pairing.PartialResult.LostUnrated)
                }
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.DrawUnrated, Pairing.PartialResult.DrawUnrated)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.DrawUnrated && root.pairing?.blackResult === Pairing.PartialResult.DrawUnrated
                    onClicked: root.setResult(Pairing.PartialResult.DrawUnrated, Pairing.PartialResult.DrawUnrated)
                }
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.LostUnrated, Pairing.PartialResult.WinUnrated)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.LostUnrated && root.pairing?.blackResult === Pairing.PartialResult.WinUnrated
                    onClicked: root.setResult(Pairing.PartialResult.LostUnrated, Pairing.PartialResult.WinUnrated)
                }
                QQC2.MenuSeparator {}
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.DrawUnrated, Pairing.PartialResult.LostUnrated)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.DrawUnrated && root.pairing?.blackResult === Pairing.PartialResult.LostUnrated
                    onClicked: root.setResult(Pairing.PartialResult.DrawUnrated, Pairing.PartialResult.LostUnrated)
                }
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.LostUnrated, Pairing.PartialResult.DrawUnrated)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.LostUnrated && root.pairing?.blackResult === Pairing.PartialResult.DrawUnrated
                    onClicked: root.setResult(Pairing.PartialResult.LostUnrated, Pairing.PartialResult.DrawUnrated)
                }
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.LostUnrated, Pairing.PartialResult.LostUnrated)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.LostUnrated && root.pairing?.blackResult === Pairing.PartialResult.LostUnrated
                    onClicked: root.setResult(Pairing.PartialResult.LostUnrated, Pairing.PartialResult.LostUnrated)
                }
                QQC2.MenuSeparator {}
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.Draw, Pairing.PartialResult.Lost)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.Draw && root.pairing?.blackResult === Pairing.PartialResult.Lost
                    onClicked: root.setResult(Pairing.PartialResult.Draw, Pairing.PartialResult.Lost)
                }
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.Lost, Pairing.PartialResult.Lost)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.Lost && root.pairing?.blackResult === Pairing.PartialResult.Lost
                    onClicked: root.setResult(Pairing.PartialResult.Lost, Pairing.PartialResult.Lost)
                }
                QQC2.MenuItem {
                    text: Controller.resultToString(Pairing.PartialResult.Lost, Pairing.PartialResult.Draw)
                    checkable: true
                    autoExclusive: true
                    checked: root.pairing?.whiteResult === Pairing.PartialResult.Lost && root.pairing?.blackResult === Pairing.PartialResult.Draw
                    onClicked: root.setResult(Pairing.PartialResult.Lost, Pairing.PartialResult.Draw)
                }
            }
        }
    }

    function setResult(whiteResult: int, blackResult: int): void {
        root.saveResult(root.pairing, whiteResult, blackResult);
    }
}
