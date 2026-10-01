# SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-2-Clause

find_file(bbpPairings bin/bbpPairings)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BBPPairings
    FOUND_VAR
        BBPPairings_FOUND
    REQUIRED_VARS
        bbpPairings
)

unset(bbpPairings)

