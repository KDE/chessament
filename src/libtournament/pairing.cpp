// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pairing.h"

Pairing::Pairing(int board, Player *whitePlayer, Player *blackPlayer, PartialResult whiteResult, PartialResult blackResult)
    : QObject()
{
    m_board = board;
    m_whitePlayer = whitePlayer;
    m_blackPlayer = blackPlayer;
    m_whiteResult = whiteResult;
    m_blackResult = blackResult;
}

int Pairing::id()
{
    return m_id;
}

void Pairing::setId(int id)
{
    if (m_id == id) {
        return;
    }
    m_id = id;
    Q_EMIT idChanged();
}

int Pairing::board()
{
    return m_board;
}

void Pairing::setBoard(int board)
{
    if (m_board == board) {
        return;
    }
    m_board = board;
    Q_EMIT boardChanged();
}

Player *Pairing::whitePlayer()
{
    return m_whitePlayer;
}

void Pairing::setWhitePlayer(Player *whitePlayer)
{
    if (m_whitePlayer == whitePlayer) {
        return;
    }
    m_whitePlayer = whitePlayer;
    Q_EMIT whitePlayerChanged();
}

Pairing::PartialResult Pairing::whiteResult()
{
    return m_whiteResult;
}

void Pairing::setWhiteResult(Pairing::PartialResult whiteResult)
{
    if (m_whiteResult == whiteResult) {
        return;
    }
    m_whiteResult = whiteResult;
    Q_EMIT whiteResultChanged();
}

Player *Pairing::blackPlayer()
{
    return m_blackPlayer;
}

void Pairing::setBlackPlayer(Player *blackPlayer)
{
    if (m_blackPlayer == blackPlayer) {
        return;
    }
    m_blackPlayer = blackPlayer;
    Q_EMIT blackPlayerChanged();
}

Pairing::PartialResult Pairing::blackResult()
{
    return m_blackResult;
}

void Pairing::setBlackResult(Pairing::PartialResult blackResult)
{
    if (m_blackResult == blackResult) {
        return;
    }
    m_blackResult = blackResult;
    Q_EMIT blackResultChanged();
}

void Pairing::setResult(Pairing::Result result)
{
    setWhiteResult(result.first);
    setBlackResult(result.second);
}

void Pairing::setResult(PartialResult whiteResult, PartialResult blackResult)
{
    setWhiteResult(whiteResult);
    setBlackResult(blackResult);
}

QString Pairing::resultString()
{
    if (!Pairing::isBye(m_whiteResult)) {
        return Pairing::partialResultToString(m_whiteResult) + u"-"_s + Pairing::partialResultToString(m_blackResult);
    }
    return Pairing::partialResultToString(m_whiteResult);
}

QString Pairing::toTrf(Player *player)
{
    QString opponent = u"0000"_s;
    QString color;
    QString r = u" "_s;

    if (m_whitePlayer == player) {
        if (m_blackPlayer == nullptr) {
            color = Pairing::colorString(Pairing::Color::Unknown);
        } else {
            opponent = QString::number(m_blackPlayer->startingRank());
            color = Pairing::colorString(Pairing::Color::White);
        }
        r = Pairing::partialResultToTRF(m_whiteResult);
    } else if (m_blackPlayer == player) {
        opponent = QString::number(m_whitePlayer->startingRank());
        color = Pairing::colorString(Pairing::Color::Black);
        r = Pairing::partialResultToTRF(m_blackResult);
    }

    auto result = QString::fromStdString(std::format("  {:>4s} {} {}", opponent.toStdString(), color.toStdString(), r.toStdString()));

    return result;
}

QDebug operator<<(QDebug dbg, Pairing &pairing)
{
    dbg.nospace() << "Pairing(" << pairing.board() << "," << *pairing.whitePlayer() << "," << pairing.blackPlayer() << ")";
    return dbg;
}
