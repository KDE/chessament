# SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-2-Clause

target_include_directories(tournament PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(tournament PRIVATE
    account.cpp
    accountmanager.cpp
    syncengine.cpp
)

target_link_libraries(tournament PUBLIC
    Qt6::NetworkAuth
    Qt6::WebSockets
    ${QTKEYCHAIN_LIBRARIES}
)

