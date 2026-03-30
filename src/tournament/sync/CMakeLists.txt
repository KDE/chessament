# SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-2-Clause

target_include_directories(tournament PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(tournament PRIVATE
    account.cpp
    accountmanager.cpp
    syncengine.cpp
)

