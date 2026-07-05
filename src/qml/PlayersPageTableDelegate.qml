// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.chessament

TableDelegate {
    id: delegate

    required property int index
    required property bool editing

    required property string displayText
    required property var edit
    required property string iconSource

    text: displayText
    icon.source: iconSource

    TableView.editDelegate: DelegateChooser {
        DelegateChoice {
            column: PlayersModel.StartingRank

            StartingRankField {}
        }

        DelegateChoice {
            column: PlayersModel.Rating

            RatingField {}
        }

        DelegateChoice {
            column: PlayersModel.NationalRating

            RatingField {}
        }

        DelegateChoice {
            column: PlayersModel.Title

            Controls.ComboBox {
                id: comboBox

                anchors.fill: parent
                model: ["", "GM", "IM", "FM", "CM", "WGM", "WIM", "WFM", "WCM"]
                editable: true

                onActivated: index => {
                    delegate.edit = comboBox.currentText;
                }

                TableView.onCommit: {
                    if (!comboBox.editText.length && !comboBox.currentText.length && comboBox.highlightedIndex >= 0) {
                        delegate.edit = comboBox.textAt(comboBox.highlightedIndex);
                    } else {
                        delegate.edit = comboBox.editText;
                    }
                }

                Component.onCompleted: {
                    comboBox.editText = delegate.edit;
                    // FIXME: This breaks the Enter key
                    // comboBox.popup.open();
                }
            }
        }

        DelegateChoice {
            Controls.TextField {
                anchors.fill: parent
                text: delegate.edit
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter

                Component.onCompleted: selectAll()

                TableView.onCommit: {
                    delegate.edit = text;
                }
            }
        }
    }
}
