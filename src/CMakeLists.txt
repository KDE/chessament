# SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(settings)

add_subdirectory(tournament)

qt_add_library(chessament_static STATIC)
ecm_add_qml_module(chessament_static
    URI org.kde.chessament
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/chessament
    GENERATE_PLUGIN_SOURCE
    QML_FILES
        qml/components/MenuBar.qml
        qml/components/RatingField.qml
        qml/components/StartingRankField.qml
        qml/components/TiebreakDelegate.qml

        qml/Main.qml
        qml/TablePage.qml
        qml/TableDelegate.qml
        qml/AddPlayerDialog.qml
        qml/AddTiebreakDialog.qml
        qml/DeletePairingsDialog.qml
        qml/NewTournamentDialog.qml
        qml/ResultsFooter.qml
        qml/Sidebar.qml
        qml/SortPlayersDialog.qml
        qml/StandingsPage.qml
        qml/PairingsPage.qml
        qml/PairRoundDialog.qml
        qml/PlayersPage.qml
        qml/WelcomePage.qml

        # Player details
        qml/player/PlayerDetails.qml
        qml/player/PlayerPairingsPage.qml

        # Tournament settings
        qml/TournamentSettings.qml
        qml/TournamentSettingsCalendar.qml
        qml/TournamentSettingsInformation.qml
        qml/TournamentSettingsFormat.qml
        qml/TiebreakDialog.qml
    DEPENDENCIES
        QtCore
        QtQuick
    SOURCES
        account.cpp
        controller.cpp
        byesmodel.cpp
        document.cpp
        documents.cpp
        playersmodel.cpp
        pairingmodel.cpp
        ratinglistmodel.cpp
        roundmodel.cpp
        standingsmodel.cpp
        tiebreakmodel.cpp
    IMPORTS
        org.kde.kirigamiaddons.formcard
        org.kde.kirigamiaddons.statefulapp
        org.kde.kirigami
        org.kde.chessament.tournament
        org.kde.chessament.settings
)

kde_source_files_enable_exceptions(
    controller.cpp
    ratinglistmodel.cpp
)

target_sources(chessament_static PUBLIC
    chessamentapplication.cpp
    chessamentapplication.h
)

target_link_libraries(chessament_static PUBLIC
    tournament
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql
    Qt6::Svg
    Qt6::Network
    Qt6::NetworkAuth
    Qt6::PrintSupport
    KF6::I18n
    KF6::I18nQml
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::IconThemes
    KF6::ItemModels
    KirigamiAddonsStatefulApp
    QCoro6::Core
    QCoro6::Qml
    ${QTKEYCHAIN_LIBRARIES}
)
target_include_directories(chessament_static PUBLIC ${CMAKE_BINARY_DIR})

kconfig_target_kcfg_file(chessament_static FILE config.kcfg CLASS_NAME Config MUTATORS GENERATE_PROPERTIES GENERATE_MOC DEFAULT_VALUE_GETTERS PARENT_IN_CONSTRUCTOR SINGLETON QML_REGISTRATION)

add_executable(chessament main.cpp)
target_link_libraries(chessament PRIVATE
    chessament_static
    chessament_staticplugin
    chessament_settings
    chessament_settingsplugin
)

ecm_add_app_icon(CHESSAMENT_ICON ICONS ${CMAKE_SOURCE_DIR}/128-logo.png)

target_sources(chessament PRIVATE ${CHESSAMENT_ICON})

install(TARGETS chessament ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_DOCS)
    include(ECMGenerateQDoc)
    ecm_generate_qdoc(chessament chessament.qdocconf)
endif()

