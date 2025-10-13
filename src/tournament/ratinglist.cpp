// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ratinglist.h"

#include <QCoreApplication>
#include <QCoroNetworkReply>
#include <QNetworkAccessManager>
#include <QTemporaryFile>

#include <KLocalizedString>
#include <KZip>

using namespace Qt::StringLiterals;

QCoro::Task<std::expected<std::unique_ptr<RatingList>, QString>> RatingList::importList(const QUrl &url)
{
    auto list = std::make_unique<RatingList>();

    if (const auto ok = co_await list->import(url); !ok) {
        co_return std::unexpected(ok.error());
    }

    co_return list;
}

QCoro::Task<std::expected<void, QString>> RatingList::import(const QUrl &url)
{
    QNetworkAccessManager manager;

    QNetworkRequest request{url};
    const QString userAgent = "Chessament/"_L1 + QCoreApplication::applicationVersion() + " (https://apps.kde.org/chessament/)"_L1;
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    auto *reply = co_await manager.get(request);
    // const auto data = reply->readAll();

    qDebug() << reply->headers();

    QTemporaryFile file;
    file.open();
    file.write(reply->readAll());
    reply->deleteLater();

    auto zip = KZip(file.fileName());
    zip.open(QIODevice::ReadOnly);

    const auto directory = zip.directory();
    if (directory->entries().size() != 1) {
        co_return std::unexpected(i18nc("@info", "File format not supported."));
    }

    const auto archiveFile = directory->file(directory->entries().constFirst());
    const auto device = archiveFile->createDevice();

    QTextStream stream(device);

    if (const auto ok = readPlayers(&stream); !ok) {
        co_return ok;
    }

    device->deleteLater();

    co_return {};
}

std::expected<void, QString> RatingList::readPlayers(QTextStream *stream)
{
    QString line;
    int count = 0;

    // Skip header
    stream->readLine();

    while (stream->readLineInto(&line)) {
        if (line.size() < 162) {
            continue;
        }

        bool ok;
        const int playerId = line.sliced(0, 15).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid player id of player" << line;
            continue;
        }

        const QString name = line.sliced(15, 61).trimmed();
        const QString federation = line.sliced(76, 3);
        const int standardRating = line.sliced(113, 4).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid standard rating of player" << line;
            continue;
        }

        const int rapidRating = line.sliced(126, 4).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid rapid rating of player" << line;
            continue;
        }

        const int blitzRating = line.sliced(139, 4).toInt(&ok);
        if (!ok) {
            qWarning() << "invalid blitz rating of player" << line;
            continue;
        }

        ++count;
    }

    qDebug() << "read " << count;

    return {};
}
