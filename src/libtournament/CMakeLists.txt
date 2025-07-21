# SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

add_library(libtournament STATIC)

target_sources(libtournament PRIVATE
    event.cpp
    pairing.cpp
    pairingengine.cpp
    player.cpp
    round.cpp
    state.cpp
    tournament.cpp
    trf.cpp
    utils.cpp

    tiebreaks/tiebreak.cpp
    tiebreaks/buchholz.cpp
    tiebreaks/points.cpp
)

ecm_add_qml_module(libtournament
    URI org.kde.chessament.libtournament
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/chessament/libtournament
    GENERATE_PLUGIN_SOURCE
)

target_link_libraries(libtournament PUBLIC
    Qt6::Core
    Qt6::Qml
    Qt6::Sql
    KF6::I18n
    QCoro6::Core
)

