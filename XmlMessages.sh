# SPDX-FileCopyrightText: 2026 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
# SPDX-License-Identifier: BSD-2-Clause

function get_files {
    echo org.kde.chessament.xml
}

function po_for_file {
    case "$1" in
    org.kde.chessament.xml)
        echo chessament_mime.po
        ;;
    esac
}

function tags_for_file {
    case "$1" in
    org.kde.chessament.xml)
        echo comment
        ;;
    esac
}
