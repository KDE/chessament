// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pairingengine.h"
#include "pairing.h"

#include <KLocalizedString>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <chrono>
#include <iostream>

using namespace std::literals::chrono_literals;

PairingEngine::PairingEngine()
    : QObject()
{
}

QCoro::Task<std::expected<QList<Pairing *>, QString>> PairingEngine::pair(int round, Tournament *tournament)
{
    const auto &path = QStandardPaths::findExecutable(u"bbpPairings"_s);
    if (path.isEmpty()) {
        co_return std::unexpected(
            i18nc("bbpPairings is the name of a program, should not be translated", "The pairing engine bbpPairings could not be found. Is it installed?"));
    }

    QProcess process;
    auto proc = qCoro(process);

    QTemporaryFile file;
    file.open();

    // TODO: initial color
    auto trf = tournament->toTrf(Tournament::TrfOption::NumberOfRounds | Tournament::TrfOption::InitialColorWhite, round);
    file.write(trf.toUtf8());
    file.flush();

    std::cout << trf.toStdString();

    co_await proc.start(u"bbpPairings"_s, {u"--dutch"_s, file.fileName(), u"-p"_s});
    co_await proc.waitForFinished(std::chrono::milliseconds(3s));

    if (process.exitCode() != 0) {
        co_return std::unexpected(u"Non zero exit code "_s + QString::number(process.exitCode()));
    }

    QList<Pairing *> pairings;
    const auto players = tournament->getPlayersByStartingRank();

    const auto output = QString::fromUtf8(process.readAll());
    const auto lines = output.split(u'\n');

    uint board = 1;
    for (const auto &line : lines.mid(1)) {
        if (line.isEmpty()) {
            continue;
        }
        const auto playerIds = line.split(u' ');

        if (playerIds.size() != 2) {
            co_return std::unexpected(u"Invalid pairing: "_s + line);
        }

        // TODO: check for multiple byes
        const auto whiteId = playerIds[0].toUInt();
        const auto blackId = playerIds[1].toUInt();

        Pairing::Result result = std::make_pair(Pairing::PartialResult::PairingBye, Pairing::PartialResult::Unknown);

        const auto white = players[whiteId];
        Player *black = nullptr;

        if (blackId != 0) {
            black = players[blackId];
            result.first = Pairing::PartialResult::Unknown;
        }

        pairings << new Pairing(board++, white, black, result.first, result.second);
    }

    co_return pairings;
}
