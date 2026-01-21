// SPDX-FileCopyrightText: 2024-2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.h"
#include "document.h"
#include "tournament/state.h"

#include <KLocalizedString>

#include <QTextDocument>

std::unique_ptr<Document> Controller::playersDocument()
{
    using enum PlayersModel::Columns;

    auto doc = std::make_unique<Document>();

    doc->addTitle(1, m_tournament->name());
    doc->addTitle(2, i18nc("@title", "Starting Rank"));

    auto players = m_tournament->players();
    std::ranges::sort(players, Player::SortByStartingRank);

    PlayersModel model;
    model.setPlayers(players);
    model.setColumns({StartingRank, Title, Name, Rating, NationalRating, PlayerId, Federation, Origin});
    doc->addTable(model);

    return doc;
}
void Controller::savePlayersDocument(const QString &fileName)
{
    auto doc = playersDocument();
    doc->saveAs(fileName);
}

void Controller::printPlayersDocument()
{
    auto doc = playersDocument();
    doc->print();
}

std::unique_ptr<Document> Controller::pairingsDocument()
{
    using enum PairingModel::Columns;

    auto doc = std::make_unique<Document>();

    doc->addTitle(1, m_tournament->name());
    doc->addTitle(2, i18nc("@title", "Round %1 Pairings", m_currentRound));

    PairingModel model;
    model.setPairings(m_tournament->getPairings(m_currentRound));
    model.setColumns({Board, WhiteStartingRank, WhiteName, Result, BlackName, BlackStartingRank});
    doc->addTable(model);

    return doc;
}

void Controller::savePairingsDocument(const QString &fileName)
{
    auto doc = pairingsDocument();
    doc->saveAs(fileName);
}

void Controller::printPairingsDocument()
{
    auto doc = pairingsDocument();
    doc->print();
}

std::unique_ptr<Document> Controller::standingsDocument(int round)
{
    Q_ASSERT(round >= 1);
    Q_ASSERT(round <= m_tournament->numberOfRounds());

    auto doc = std::make_unique<Document>();

    doc->addTitle(1, m_tournament->name());

    QString title;
    if (round < m_tournament->numberOfRounds()) {
        if (m_tournament->isRoundFinished(round)) {
            title = i18nc("@title", "Standings After Round %1", round);
        } else {
            title = i18nc("@title", "Provisional Standings After Round %1", round);
        }
    } else {
        if (m_tournament->isRoundFinished(round)) {
            title = i18nc("@title", "Final Standings");
        } else {
            title = i18nc("@title", "Provisional Final Standings", round);
        }
    }
    doc->addTitle(2, title);

    const auto state = m_tournament->getState(round);

    StandingsModel model;
    model.setTournament(m_tournament);
    model.setStandings(m_tournament->getStandings(state));
    doc->addTable(model);

    return doc;
}

void Controller::saveStandingsDocument(const QString &fileName, int round)
{
    const auto doc = standingsDocument(round);
    doc->saveAs(fileName);
}

void Controller::printStandingsDocument(int round)
{
    const auto doc = standingsDocument(round);
    doc->print();
}
