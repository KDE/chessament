# SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

project(chessament VERSION 0.1)

include(FeatureSummary)

set(QT6_MIN_VERSION 6.9.0)
set(KF6_MIN_VERSION 6.9.0)
set(KIRIGAMI_ADDONS_MIN_VERSION 1.6.0)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMAddAppIcon)

include(ECMSetupVersion)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMDeprecationSettings)

include(KDEGitCommitHooks)
include(KDEClangFormat)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX CHESSAMENT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/version-chessament.h"
)

ecm_set_disabled_deprecation_versions(
    QT ${QT6_MIN_VERSION}
    KF ${KF6_MIN_VERSION}
)

find_package(Qt6 ${QT6_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Sql Svg Network NetworkAuth PrintSupport)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Kirigami CoreAddons Config IconThemes I18n ItemModels)

find_package(QCoro6 0.11 REQUIRED COMPONENTS Core)
qcoro_enable_coroutines()

find_package(KF6KirigamiAddons ${KIRIGAMI_ADDONS_MIN_VERSION} REQUIRED)
set_package_properties(KF6KirigamiAddons PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components and runtime dependency"
)

find_package(Qt6Keychain)
set_package_properties(Qt6Keychain PROPERTIES
    TYPE REQUIRED
    PURPOSE "Store authentication credentials"
)

qt_policy(SET QTP0001 NEW)

ecm_find_qmlmodule(org.kde.kirigamiaddons.formcard)
ecm_find_qmlmodule(org.kde.kirigamiaddons.statefulapp)

add_subdirectory(src)

if(BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Test)
    add_subdirectory(autotests)
endif()

install(FILES org.kde.chessament.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.chessament.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.chessament.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
