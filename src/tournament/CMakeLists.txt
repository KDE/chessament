# SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
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
    utils.cpp
)

add_subdirectory(ratinglists)
add_subdirectory(sync)
add_subdirectory(tiebreaks)
add_subdirectory(trf)

ecm_add_qml_module(tournament
    URI org.kde.chessament.tournament
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/chessament/tournament
    GENERATE_PLUGIN_SOURCE
)

target_link_libraries(tournament PUBLIC
    Qt6::Concurrent
    Qt6::Core
    Qt6::Network
    Qt6::NetworkAuth
    Qt6::Qml
    Qt6::Sql
    Qt6::WebSockets
    KF6::Archive
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::I18n
    QCoro6::Core
    QCoro6::Network
    QCoro6::Qml
    ${QTKEYCHAIN_LIBRARIES}
)

kde_source_files_enable_exceptions(
    ${CMAKE_CURRENT_SOURCE_DIR}/sync/accountmanager.cpp
)

kconfig_target_kcfg_file(tournament
    FILE accountconfig.kcfg
    CLASS_NAME AccountConfig

    MUTATORS
    GENERATE_PROPERTIES
    GENERATE_MOC
    PARENT_IN_CONSTRUCTOR
)

