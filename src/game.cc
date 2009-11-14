/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
 *
 * Miniature is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Miniature is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Miniature. If not, see <http://www.gnu.org/licenses/>.
 */

#include "game.h"
#include "position.h"

#include <cmath>

#include <QList>

using namespace Miniature;

MGame::MGame(QObject *parent)
: QObject(parent),
  m_game(createDummyGame()),
  m_half_move(-1)
{
    m_player_info.white_name = QString("andybehr");
    m_player_info.white_rating = QString("(4321)");
    m_player_info.white_turn = QString("1");

    m_player_info.black_name = QString("mikhas");
    m_player_info.black_rating = QString("(1234)");
    m_player_info.black_turn = QString("1");
}

MGame::~MGame()
{}

MPlayerInfo MGame:: getPlayerInfo() const
{
    return m_player_info;
}

void MGame::newGame()
{
    MPosition pos;
    Q_EMIT positionChanged(pos);

    m_half_move = -1;

    updateTurnInfo();
}

void MGame::nextMove()
{
    if (0 < m_game.size() && m_half_move < static_cast<int>(m_game.size()) - 1)
    {
        ++m_half_move;

        MPosition pos = MPosition(m_game[m_half_move]);
        Q_EMIT positionChanged(pos);

        updateTurnInfo();
    }
    else // Complain!
    {
    }
}

void MGame::prevMove()
{
    if (m_half_move > 0)
    {
       --m_half_move;

        MPosition pos = MPosition(m_game[m_half_move]);
        Q_EMIT positionChanged(pos);

        updateTurnInfo();
    }
    else // Complain!
    {
        newGame();
    }
}

int MGame::computeTurn() const
{
    return floor(m_half_move * 0.5) + 1;
}

void MGame::updateTurnInfo()
{
    m_player_info.white_turn = QString("%1").arg(computeTurn());
    m_player_info.black_turn = QString("%1").arg(computeTurn());

    Q_EMIT playerInfoChanged();
}

std::vector<QString> MGame::createDummyGame() const
{
    std::vector<QString> game;
    game.push_back(QString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"));
    game.push_back(QString("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"));
    return game;
}
