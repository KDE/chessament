# SPDX-FileCopyrightText: 2024-2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-3-Clause

enable_testing()

add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(
    tournamenttest.cpp
    LINK_LIBRARIES tournament Qt::Test
    TEST_NAME tournamenttest
)

ecm_add_test(
    tiebreakstest.cpp
    LINK_LIBRARIES tournament Qt::Test
    TEST_NAME tiebreakstest
)

ecm_add_test(
    utilstest.cpp
    LINK_LIBRARIES tournament Qt::Test
    TEST_NAME utilstest
)

