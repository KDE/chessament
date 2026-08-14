# SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

project(chessament VERSION 0.1)

set(QT6_MIN_VERSION 6.10.0)
set(KF6_MIN_VERSION 6.23.0)
set(KIRIGAMI_ADDONS_MIN_VERSION 1.6.0)
set(QCORO_MIN_VERSION 0.11)

include(FeatureSummary)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set_package_properties(ECM PROPERTIES
    TYPE REQUIRED
    URL "https://api.kde.org/ecm/"
    DESCRIPTION "Extra CMake Modules")

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMAddTests)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMDeprecationSettings)

include(KDEGitCommitHooks)
include(KDEClangFormat)

option(BUILD_EXPERIMENTAL "Build experimental features" OFF)

if (BUILD_EXPERIMENTAL)
    add_compile_definitions(BUILD_EXPERIMENTAL)
endif()

ecm_setup_version(PROJECT
    VARIABLE_PREFIX CHESSAMENT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/version-chessament.h"
)

ecm_set_disabled_deprecation_versions(
    QT ${QT6_MIN_VERSION}
    KF ${KF6_MIN_VERSION}
)

find_package(Qt6 ${QT6_MIN_VERSION} COMPONENTS
    Concurrent
    Core
    Gui
    Network
    PrintSupport
    Qml
    QuickControls2
    Sql
    Svg
)
set_package_properties(Qt6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

if (BUILD_EXPERIMENTAL)
    find_package(Qt6 ${QT6_MIN_VERSION} COMPONENTS
        NetworkAuth
        WebSockets
    )
endif()

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

find_package(KF6 ${KF6_MIN_VERSION} COMPONENTS
    Archive
    Config
    CoreAddons
    I18n
    IconThemes
    ItemModels
    KIO
    Kirigami
)
set_package_properties(KF6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(QCoro6 ${QCORO_MIN_VERSION} COMPONENTS
    Core
    Network
    Qml
)
set_package_properties(QCoro6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(KF6KirigamiAddons ${KIRIGAMI_ADDONS_MIN_VERSION})
set_package_properties(KF6KirigamiAddons PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components and runtime dependency"
)

if (BUILD_EXPERIMENTAL)
    find_package(Qt6Keychain)
    set_package_properties(Qt6Keychain PROPERTIES
        TYPE REQUIRED
        PURPOSE "Store authentication credentials"
    )
endif()

find_package(SharedMimeInfo 1.3)
set_package_properties(SharedMimeInfo PROPERTIES
    TYPE REQUIRED
)

find_package(bbpPairings)
set_package_properties(bbpPairings PROPERTIES
    TYPE RUNTIME
    PURPOSE "Pairing engine"
    URL "https://github.com/BieremaBoyzProgramming/bbpPairings"
)

option(BUILD_DOCS "Build user documentation" ON)

if (BUILD_DOCS)
    find_package(KF6DocTools ${KF6_MIN_VERSION})
    set_package_properties(KF6DocTools PROPERTIES
        TYPE REQUIRED
        PURPOSE "User documentation"
    )
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

qcoro_enable_coroutines()

ecm_find_qmlmodule(org.kde.ki18n REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.components REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.delegates REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.formcard REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.settings REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.statefulapp REQUIRED)
ecm_find_qmlmodule(org.kde.kitemmodels REQUIRED)

add_subdirectory(src)

if (BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Test)
    add_subdirectory(autotests)
endif()

install(FILES org.kde.chessament.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.chessament.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.chessament.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
update_xdg_mimetypes(${KDE_INSTALL_MIMEDIR})
ki18n_install(po)

if (BUILD_DOCS)
    add_subdirectory(doc)
    kdoctools_install(po)
endif()

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
