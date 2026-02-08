<!--
    SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
    SPDX-License-Identifier: CC0-1.0
-->

# <img src="org.kde.chessament.svg" width="40" alt="" /> Chessament

A chess tournament manager.

> [!note]
> This app is still in development and isn't released yet by the KDE community.

> [!warning]
> Until the first stable release is published, the file format used by
> Chessament may have breaking changes that prevent opening tournaments
> created with a previous version of the application.

## Features

- Register players.
- Pair rounds.
- Calculate standings.

## Get it

For Linux, a nightly Flatpak is available from [KDE's nightly Flatpak repositories](https://userbase.kde.org/Tutorials/Flatpak).

Nightly builds for Windows can be downloaded from [cdn.kde.org](https://cdn.kde.org/ci-builds/games/chessament/master/windows/).

## Links

- [Homepage](https://apps.kde.org/chessament/)
- [Source code](https://invent.kde.org/games/chessament)
- [Report a bug or feature request](https://bugs.kde.org/enter_bug.cgi?format=guided&product=chessament)

## Building

The easiest way to make changes and test Chessament during development is [using kde-builder](https://develop.kde.org/docs/getting-started/building/kde-builder-compile/).

### Requirements

The pairing engine [bbpPairings](https://github.com/BieremaBoyzProgramming/bbpPairings) is required at runtime.
