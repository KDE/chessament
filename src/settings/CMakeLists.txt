# SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause

qt_add_library(chessament_settings STATIC)
ecm_add_qml_module(chessament_settings
    GENERATE_PLUGIN_SOURCE
    URI org.kde.chessament.settings
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/chessament/settings
    QML_FILES
        ChessamentConfigurationView.qml
        GeneralPage.qml
)
