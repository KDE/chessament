// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pairingengine.h"

#include <KLocalizedString>
#include <QCoreApplication>
#include <QOperatingSystemVersion>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <chrono>
#include <iostream>

#include "trf/writer.h"

using namespace std::literals::chrono_literals;

QCoro::Task<std::expected<QList<std::pair<uint, uint>>, QString>> PairingEngine::pair(int round, Tournament *tournament)
{
    auto path = QStandardPaths::findExecutable(u"bbpPairings"_s);
    if constexpr (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows) {
        if (path.isEmpty()) {
            path = QStandardPaths::findExecutable(u"bbpPairings"_s, {QCoreApplication::applicationDirPath()});
        }
    }
    if (path.isEmpty()) {
        co_return std::unexpected(
            i18nc("bbpPairings is the name of a program, should not be translated", "The pairing engine bbpPairings could not be found. Is it installed?"));
    }

    QProcess process;
    auto proc = qCoro(process);

    QTemporaryFile file;
    if (!file.open()) {
        co_return std::unexpected(i18n("Could not create temporary file."));
    }

    Trf::Options options = Trf::Option::NumberOfRounds;
    qDebug() << tournament->initialColor();
    switch (tournament->initialColor()) {
    case Tournament::InitialColor::White:
        options |= Trf::Option::InitialColorWhite;
        break;
    case Tournament::InitialColor::Black:
        options |= Trf::Option::InitialColorBlack;
        break;
    }

    auto trf = tournament->toTrf(options, round);
    file.write(trf.toUtf8());
    file.flush();

    std::cout << trf.toStdString();

    co_await proc.start(u"bbpPairings"_s, {u"--dutch"_s, file.fileName(), u"-p"_s});
    co_await proc.waitForFinished(std::chrono::milliseconds(3s));

    if (process.exitCode() != 0) {
        co_return std::unexpected(i18nc("bbpPairings is the name of a program, should not be translated",
                                        "bbpPairings terminated with a non zero exit code: %1",
                                        QString::number(process.exitCode())));
    }

    auto pairings = QList<std::pair<uint, uint>>();
    const auto players = tournament->playersByStartingRank();

    const auto output = QString::fromUtf8(process.readAll());
    std::cout << output.toStdString();
    const auto lines = output.split(u'\n');

    for (const auto &line : lines.mid(1)) {
        if (line.isEmpty()) {
            continue;
        }
        const auto playerIds = line.split(u' ', Qt::SkipEmptyParts);

        if (playerIds.size() != 2) {
            co_return std::unexpected(i18n("Invalid pairing \"%1\".", line));
        }

        bool ok;
        const auto whiteId = playerIds[0].toUInt(&ok);
        if (!ok) {
            co_return std::unexpected(i18n("Invalid player \"%1\" on pairing \"%2\".", playerIds[0], line));
        }
        const auto blackId = playerIds[1].toUInt(&ok);
        if (!ok) {
            co_return std::unexpected(i18n("Invalid player \"%1\" on pairing \"%2\".", playerIds[1], line));
        }

        if (!players.contains(whiteId)) {
            co_return std::unexpected(i18n("Invalid player \"%1\" on pairing \"%2\".", QString::number(whiteId), line));
        }

        if (blackId != 0 && !players.contains(blackId)) {
            co_return std::unexpected(i18n("Invalid player \"%1\" on pairing \"%2\".", QString::number(blackId), line));
        }

        pairings.append({whiteId, blackId});
    }

    co_return pairings;
}

#include "moc_pairingengine.cpp"
