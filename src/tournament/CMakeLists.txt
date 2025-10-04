# SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

add_library(tournament STATIC)

target_sources(tournament PRIVATE
    event.cpp
    pairing.cpp
    pairingengine.cpp
    player.cpp
    round.cpp
    standing.cpp
    state.cpp
    tournament.cpp
    trf.cpp
    utils.cpp
)

add_subdirectory(tiebreaks)

ecm_add_qml_module(tournament
    URI org.kde.chessament.tournament
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/chessament/tournament
    GENERATE_PLUGIN_SOURCE
)

target_link_libraries(tournament PUBLIC
    Qt6::Core
    Qt6::Qml
    Qt6::Sql
    KF6::I18n
    QCoro6::Core
)

