# SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(settings)

add_subdirectory(libtournament)

qt_add_library(chessament_static STATIC)
ecm_add_qml_module(chessament_static
    URI org.kde.chessament
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/chessament
    GENERATE_PLUGIN_SOURCE
    QML_FILES
        qml/components/RatingField.qml

        qml/Main.qml
        qml/TablePage.qml
        qml/TableDelegate.qml
        qml/AddPlayerDialog.qml
        qml/DeletePairingsDialog.qml
        qml/DocsPage.qml
        qml/NewTournamentDialog.qml
        qml/ResultsFooter.qml
        qml/SortPlayersDialog.qml
        qml/StandingsPage.qml
        qml/PairingsPage.qml
        qml/PairRoundDialog.qml
        qml/PlayersPage.qml
        qml/WelcomePage.qml

        # Tournament settings
        qml/TournamentSettings.qml
        qml/TournamentSettingsCalendar.qml
        qml/TournamentSettingsInformation.qml
        qml/TournamentSettingsFormat.qml
    DEPENDENCIES
        QtCore
        QtQuick
    SOURCES
        account.cpp
        controller.cpp
        document.cpp
        documents.cpp
        playersmodel.cpp
        pairingmodel.cpp
        standingsmodel.cpp
    IMPORTS
        org.kde.kirigamiaddons.formcard
        org.kde.kirigamiaddons.statefulapp
        org.kde.kirigami
        org.kde.chessament.libtournament
        org.kde.chessament.settings
)

target_sources(chessament_static PUBLIC
    chessamentapplication.cpp
    chessamentapplication.h
)

qt_add_resources(chessament_static "documents"
    PREFIX "/"
    FILES
        documents/default/document.html
        documents/default/theme.css
        documents/default/style.css
)

target_link_libraries(chessament_static PUBLIC
    libtournament
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql
    Qt6::Svg
    Qt6::WebEngineQuick
    Qt6::Widgets
    Qt6::Network
    Qt6::NetworkAuth
    Qt6::Pdf
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

