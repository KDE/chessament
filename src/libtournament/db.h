// SPDX-FileCopyrightText: 2024 Manuel Alcaraz Zambrano <manuelalcarazzam@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

using namespace Qt::StringLiterals;

const QString TOURNAMENTS_TABLE_SCHEMA =
    u"CREATE TABLE IF NOT EXISTS tournaments("_s
    u"id TEXT PRIMARY KEY"_s
    u");"_s;

const QString ADD_TOURNAMENT_QUERY = u"INSERT INTO tournaments(id) VALUES (:id);"_s;

const QString GET_TOURNAMENTS_QUERY = u"SELECT * FROM tournaments;"_s;

const QString OPTIONS_TABLE_SCHEMA =
    u"CREATE TABLE IF NOT EXISTS options("_s
    u"tournament TEXT NOT NULL,"_s
    u"name TEXT NOT NULL,"_s
    u"value TEXT,"_s
    u"PRIMARY KEY (tournament, name),"_s
    u"FOREIGN KEY (tournament) REFERENCES tournaments(id)"_s
    u");"_s;

const QString GET_OPTION_QUERY = u"SELECT value FROM options WHERE tournament = :tournament AND name = :name LIMIT 1;"_s;

const QString UPDATE_OPTION_QUERY = u"INSERT OR REPLACE INTO options(tournament, name, value) VALUES (:tournament, :name, :value);"_s;

const QString PLAYERS_TABLE_SCHEMA =
    u"CREATE TABLE IF NOT EXISTS players("_s
    u"id INTEGER PRIMARY KEY,"_s
    u"startingRank INTEGER,"_s
    u"title TEXT,"_s
    u"name TEXT,"_s
    u"rating INTEGER,"_s
    u"nationalRating INTEGER,"_s
    u"playerId TEXT,"_s
    u"birthDate TEXT,"_s
    u"federation TEXT,"_s
    u"origin TEXT,"_s
    u"sex TEXT,"_s
    u"tournament TEXT NOT NULL,"_s
    u"FOREIGN KEY (tournament) REFERENCES tournaments(id)"_s
    u");"_s;

const QString ADD_PLAYER_QUERY =
    u"INSERT INTO players(startingRank, title, name, rating, nationalRating, playerId, birthDate, federation, origin, sex, tournament) "_s
    u"VALUES (:startingRank, :title, :name, :rating, :nationalRating, :playerId, :birthDate, :federation, :origin, :sex, :tournament);"_s;

const QString GET_PLAYERS_QUERY = u"SELECT * FROM players WHERE tournament = :tournament;"_s;

const QString UPDATE_PLAYER_QUERY =
    u"UPDATE players SET (startingRank, title, name, rating, nationalRating, playerId, birthDate, federation, origin, sex, tournament) = "_s
    u"(:startingRank, :title, :name, :rating, :nationalRating, :playerId, :birthDate, :federation, :origin, :sex, :tournament) "_s
    u"WHERE id = :id;"_s;

const QString ROUNDS_TABLE_SCHEMA =
    u"CREATE TABLE IF NOT EXISTS rounds("_s
    u"id INTEGER PRIMARY KEY,"_s
    u"number INTEGER NOT NULL,"_s
    u"tournament TEXT NOT NULL,"_s
    u"FOREIGN KEY (tournament) REFERENCES tournaments(id),"_s
    u"UNIQUE (tournament, number)"_s
    u");"_s;

const QString ADD_ROUND_QUERY =
    u"INSERT INTO rounds(number, tournament) "_s
    u"VALUES (:number, :tournament);"_s;

const QString GET_ROUNDS_QUERY = u"SELECT * FROM rounds WHERE tournament = :tournament;"_s;

const QString PAIRINGS_TABLE_SCHEMA =
    u"CREATE TABLE IF NOT EXISTS pairings("_s
    u"id INTEGER PRIMARY KEY,"_s
    u"board INTEGER,"_s
    u"whitePlayer INTEGER NOT NULL,"_s
    u"blackPlayer INTEGER,"_s
    u"whiteResult INTEGER NOT NULL,"_s
    u"blackResult INTEGER NOT NULL,"_s
    u"round INTEGER NOT NULL,"_s
    u"FOREIGN KEY(whitePlayer) REFERENCES players(id),"_s
    u"FOREIGN KEY(blackPlayer) REFERENCES players(id),"_s
    u"FOREIGN KEY(round) REFERENCES rounds(id)"_s
    u");"_s;

const QString ADD_PAIRING_QUERY =
    u"INSERT INTO pairings(board, whitePlayer, blackPlayer, whiteResult, blackResult, round) "_s
    u"VALUES (:board, :whitePlayer, :blackPlayer, :whiteResult, :blackResult, :round);"_s;

const QString GET_PAIRINGS_QUERY =
    u"SELECT * FROM pairings "_s
    u"JOIN rounds ON pairings.round = rounds.id "_s
    u"WHERE rounds.tournament = :tournament "_s
    u"ORDER BY rounds.number, pairings.board;"_s;

const QString UPDATE_PAIRING_QUERY =
    u"UPDATE pairings SET (board, whitePlayer, blackPlayer, whiteResult, blackResult) = "_s
    u"(:board, :whitePlayer, :blackPlayer, :whiteResult, :blackResult) "
    u"WHERE id = :id;"_s;

const QString DELETE_PAIRINGS_QUERY = u"DELETE FROM pairings WHERE round = :round;"_s;

const QString DELETE_PAIRINGS_NO_BYES_QUERY = u"DELETE FROM pairings WHERE round = :round AND whiteResult NOT IN (9, 10, 11);"_s;
