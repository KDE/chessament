// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QString>

#include "player.h"

/*!
 * \class Pairing
 * \inmodule libtournament
 * \inheaderfile libtournament/pairing.h
 */
class Pairing : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(int board READ board NOTIFY boardChanged)
    Q_PROPERTY(Player *whitePlayer READ whitePlayer NOTIFY whitePlayerChanged)
    Q_PROPERTY(PartialResult whiteResult READ whiteResult NOTIFY whiteResultChanged)
    Q_PROPERTY(Player *blackPlayer READ blackPlayer NOTIFY blackPlayerChanged)
    Q_PROPERTY(PartialResult blackResult READ blackResult NOTIFY blackResultChanged)

public:
    /*!
     * \enum Pairing::Color
     *
     * This enum type specifies the piece color of a player in a game.
     *
     * \value Unknown Unknown, or the player doesn't have a color asigned (byes).
     * \value White White pieces.
     * \value Black Black pieces.
     */
    enum class Color {
        Unknown,
        White,
        Black
    };
    Q_ENUM(Color)

    static QString colorString(Color color)
    {
        switch (color) {
        case Color::White:
            return u"w"_s;
        case Color::Black:
            return u"b"_s;
        case Color::Unknown:
            return u"-"_s;
        }
        return {};
    }

    static Color colorForString(const QString &color)
    {
        auto c = color.toLower();
        if (c == QStringLiteral("w")) {
            return Color::White;
        }
        if (c == QStringLiteral("b")) {
            return Color::Black;
        }
        return Color::Unknown;
    }

    double getPointsOfPlayer(Player *p)
    {
        if (p == m_whitePlayer) {
            return Pairing::pointsForResult(m_whiteResult);
        }
        if (p == m_blackPlayer) {
            return Pairing::pointsForResult(m_blackResult);
        }
        Q_UNREACHABLE();
    }

    /*!
     * \enum Pairing::PartialResult
     *
     * This enum type specifies the result of a player in a game.
     *
     * \value Unknown Unknown
     * \value Win The player won.
     * \value Lost The player lost.
     * \value Draw The player drew.
     * \value WinForfeit The player won by forfeit.
     * \value LostForfeit The player lost by forfeit.
     * \value WinUnrated The player won but the game can't be rated.
     * \value LostUnrated The player lost but the game can't be rated.
     * \value DrawUnrated The player drew but the game can't be rated.
     * \value HalfBye The player had a half-point bye.
     * \value FullBye The player had a full-point bye.
     * \value ZeroBye The player had a zero-point bye.
     * \value PairingBye The player has a pairing allocated bye.
     */
    enum class PartialResult {
        Unknown,
        // Regular results
        Win,
        Lost,
        Draw,
        // Forfeit results
        WinForfeit,
        LostForfeit,
        // Unrated
        WinUnrated,
        LostUnrated,
        DrawUnrated,
        // Byes
        HalfBye,
        FullBye,
        ZeroBye,
        PairingBye,
    };
    Q_ENUM(PartialResult)

    using Result = std::pair<PartialResult, PartialResult>;

    inline static const QList<Result> ValidResults{
        // Regular results
        {PartialResult::Win, PartialResult::Lost},
        {PartialResult::Lost, PartialResult::Win},
        {PartialResult::Draw, PartialResult::Draw},
        // Forfeit results
        {PartialResult::WinForfeit, PartialResult::LostForfeit},
        {PartialResult::LostForfeit, PartialResult::WinForfeit},
        {PartialResult::LostForfeit, PartialResult::LostForfeit},
        // Unrated
        {PartialResult::WinUnrated, PartialResult::LostUnrated},
        {PartialResult::DrawUnrated, PartialResult::DrawUnrated},
        {PartialResult::LostUnrated, PartialResult::WinUnrated},
        {PartialResult::DrawUnrated, PartialResult::LostUnrated},
        {PartialResult::LostUnrated, PartialResult::DrawUnrated},
        {PartialResult::LostUnrated, PartialResult::LostUnrated},
        // Other
        {PartialResult::Draw, PartialResult::Lost},
        {PartialResult::Lost, PartialResult::Draw},
        {PartialResult::Lost, PartialResult::Lost},
        // Byes
        {PartialResult::HalfBye, PartialResult::Unknown},
        {PartialResult::FullBye, PartialResult::Unknown},
        {PartialResult::ZeroBye, PartialResult::Unknown},
        {PartialResult::PairingBye, PartialResult::Unknown},
    };

    inline static const QList<std::pair<QString, QList<Result>>> Results{
        {u"Normal"_s, {ValidResults[0], ValidResults[1], ValidResults[2]}},
        {u"Forfeit"_s, {ValidResults[3], ValidResults[4], ValidResults[5]}},
        {u"Unrated"_s, {ValidResults[6], ValidResults[7], ValidResults[8], ValidResults[9], ValidResults[10], ValidResults[11]}},
        {u"Other"_s, {ValidResults[12], ValidResults[13], ValidResults[14]}},
    };

    static bool isValidResult(std::pair<PartialResult, PartialResult> result)
    {
        return Pairing::ValidResults.contains(result);
    }

    static bool isUnplayed(PartialResult result)
    {
        switch (result) {
        case PartialResult::WinForfeit:
        case PartialResult::LostForfeit:
        case PartialResult::HalfBye:
        case PartialResult::FullBye:
        case PartialResult::ZeroBye:
        case PartialResult::PairingBye:
            return true;
        default:
            return false;
        }
    }

    static bool isBye(PartialResult result)
    {
        switch (result) {
        case PartialResult::HalfBye:
        case PartialResult::FullBye:
        case PartialResult::ZeroBye:
        case PartialResult::PairingBye:
            return true;
        default:
            return false;
        }
    }

    static bool isRequestedBye(PartialResult result)
    {
        switch (result) {
        case PartialResult::HalfBye:
        case PartialResult::ZeroBye:
            return true;
        default:
            return false;
        }
    }

    static bool isVUR(PartialResult result)
    {
        switch (result) {
        case PartialResult::LostForfeit:
        case PartialResult::HalfBye:
        case PartialResult::ZeroBye:
            return true;
        default:
            return false;
        }
    }

    static double pointsForResult(PartialResult result)
    {
        switch (result) {
        case PartialResult::Win:
        case PartialResult::WinForfeit:
        case PartialResult::WinUnrated:
        case PartialResult::FullBye:
        case PartialResult::PairingBye:
            return 1.;
        case PartialResult::Draw:
        case PartialResult::DrawUnrated:
        case PartialResult::HalfBye:
            return 0.5;
        default:
            return 0.;
        }
    }

    static QString partialResultToString(PartialResult result)
    {
        switch (result) {
        case PartialResult::Win:
            return QStringLiteral("1");
        case PartialResult::Draw:
            return QStringLiteral("½");
        case PartialResult::Lost:
            return QStringLiteral("0");
        case PartialResult::WinForfeit:
            return QStringLiteral("+");
        case PartialResult::LostForfeit:
            return QStringLiteral("-");
        case PartialResult::WinUnrated:
            return QStringLiteral("1U");
        case PartialResult::LostUnrated:
            return QStringLiteral("0U");
        case PartialResult::DrawUnrated:
            return QStringLiteral("½");
        case PartialResult::HalfBye:
            return QStringLiteral("Bye (½)");
        case PartialResult::FullBye:
            return QStringLiteral("Bye (1)");
        case PartialResult::ZeroBye:
            return QStringLiteral("Bye (0)");
        case PartialResult::PairingBye:
            return QStringLiteral("Bye (Pairing)");
        case PartialResult::Unknown:
            return QStringLiteral("?");
        }
        Q_UNREACHABLE();
    }

    static PartialResult partialResultForTRF(const QString &partialResult)
    {
        if (partialResult == QStringLiteral("1")) {
            return PartialResult::Win;
        }
        if (partialResult == QStringLiteral("0")) {
            return PartialResult::Lost;
        }
        if (partialResult == QStringLiteral("=")) {
            return PartialResult::Draw;
        }
        if (partialResult == QStringLiteral("+")) {
            return PartialResult::WinForfeit;
        }
        if (partialResult == QStringLiteral("-")) {
            return PartialResult::LostForfeit;
        }
        if (partialResult == QStringLiteral("W")) {
            return PartialResult::WinUnrated;
        }
        if (partialResult == QStringLiteral("L")) {
            return PartialResult::LostUnrated;
        }
        if (partialResult == QStringLiteral("D")) {
            return PartialResult::DrawUnrated;
        }
        if (partialResult == QStringLiteral("H")) {
            return PartialResult::HalfBye;
        }
        if (partialResult == QStringLiteral("F")) {
            return PartialResult::FullBye;
        }
        if (partialResult == QStringLiteral("Z")) {
            return PartialResult::ZeroBye;
        }
        if (partialResult == QStringLiteral("U")) {
            return PartialResult::PairingBye;
        }
        return PartialResult::Unknown;
    }

    static QString partialResultToTRF(PartialResult result)
    {
        switch (result) {
        case PartialResult::Win:
            return QStringLiteral("1");
        case PartialResult::Draw:
            return QStringLiteral("=");
        case PartialResult::Lost:
            return QStringLiteral("0");
        case PartialResult::WinForfeit:
            return QStringLiteral("+");
        case PartialResult::LostForfeit:
            return QStringLiteral("-");
        case PartialResult::WinUnrated:
            return QStringLiteral("W");
        case PartialResult::LostUnrated:
            return QStringLiteral("L");
        case PartialResult::DrawUnrated:
            return QStringLiteral("D");
        case PartialResult::HalfBye:
            return QStringLiteral("H");
        case PartialResult::FullBye:
            return QStringLiteral("F");
        case PartialResult::ZeroBye:
            return QStringLiteral("Z");
        case PartialResult::PairingBye:
            return QStringLiteral("U");
        case PartialResult::Unknown:
            return QStringLiteral("X"); // TODO: export unfinished games
        }
        Q_UNREACHABLE();
    }

    explicit Pairing(int board, Player *whitePlayer, Player *blackPlayer, PartialResult whiteResult, PartialResult blackResult);

    /*!
     * \property Pairing::id
     * \brief the ID of the pairing
     *
     * This property holds the database ID of the pairing.
     */
    [[nodiscard]] int id() const;

    /*!
     * \property Pairing::board
     * \brief the board of the pairing
     *
     * This property holds the board of the pairing.
     */
    [[nodiscard]] int board() const;

    /*!
     * \property Pairing::whitePlayer
     * \brief the player with the white pieces
     *
     * This property holds the player with the white pieces.
     */
    Player *whitePlayer();

    /*!
     * \property Pairing::whiteResult
     * \brief the result of the white player
     *
     * This property holds the result of the player with the white pieces.
     */
    PartialResult whiteResult();

    /*!
     * \property Pairing::blackPlayer
     * \brief the player with the black pieces
     *
     * This property holds the player with the black pieces.
     */
    Player *blackPlayer();

    /*!
     * \property Pairing::blackResult
     * \brief the result of the black player
     *
     * This property holds the result of the player with the black pieces.
     */
    PartialResult blackResult();

    Pairing::PartialResult getResultOfPlayer(Player *p)
    {
        if (p == m_whitePlayer) {
            return m_whiteResult;
        }
        if (p == m_blackPlayer) {
            return m_blackResult;
        }
        Q_UNREACHABLE();
    }

    /*!
     * Returns whether the game has finished.
     *
     * A game has finished if the result of the white player is known. Byes are always finished games.
     */
    bool hasFinished();

    QString resultString();
    QString toTrf(Player *player);

    friend QDebug operator<<(QDebug dbg, Pairing &pairing);

public Q_SLOTS:
    void setId(int id);
    void setBoard(int board);
    void setWhitePlayer(Player *whitePlayer);
    void setWhiteResult(Pairing::PartialResult whiteResult);
    void setBlackPlayer(Player *blackPlayer);
    void setBlackResult(Pairing::PartialResult blackResult);
    void setResult(Pairing::Result result);
    void setResult(Pairing::PartialResult whiteResult, Pairing::PartialResult blackResult);

Q_SIGNALS:
    void idChanged();
    void boardChanged();
    void whitePlayerChanged();
    void whiteResultChanged();
    void blackPlayerChanged();
    void blackResultChanged();

private:
    int m_id = 0;
    int m_board;
    Player *m_whitePlayer;
    Pairing::PartialResult m_whiteResult = PartialResult::Unknown;
    Player *m_blackPlayer;
    Pairing::PartialResult m_blackResult = PartialResult::Unknown;
};
