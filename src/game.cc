/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
 *
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
#include "pieces.h"

#include <cmath>
#include <QList>

#include <iostream>
#include <QTime>

using namespace Miniature;

MGame::MGame(QObject *parent)
: QObject(parent),
  m_half_move(-1),
  m_logic_analyzer(0, this)
{
    m_player_info.white_name = QString("andybehr");
    m_player_info.white_rating = QString("4321");
    m_player_info.white_material = computeWhiteMaterial();

    m_player_info.black_name = QString("mikhas");
    m_player_info.black_rating = QString("1234");
    m_player_info.black_material = computeBlackMaterial();
}

MGame::~MGame()
{}

MPlayerInfo MGame:: getPlayerInfo() const
{
    return m_player_info;
}

void MGame::newGame()
{
    m_half_move = -1;

    setupStartPosition();
}

void MGame::nextMove()
{
/*
    if (0 < m_game.size() && m_half_move < static_cast<int>(m_game.size()) - 1)
    {
        ++m_half_move;

        MPosition pos = MPosition(m_game[m_half_move]);
        Q_EMIT positionChanged(pos);

        updateMaterialInfo();
    }
    else // Complain!
    {
        newGame();
    }
*/
}

void MGame::prevMove()
{
/*
    if (m_half_move > 0)
    {
       --m_half_move;

        //MPosition pos = MPosition(m_game[m_half_move]);
        //Q_EMIT positionChanged(pos);

        updateMaterialInfo();
    }
    else // Complain!
    {
        newGame();
    }
*/
}

void MGame::setupStartPosition()
{
    m_position.addPieceAt(new MRook(MPiece::BLACK), QPoint(0,0));
    m_position.addPieceAt(new MRook(MPiece::BLACK), QPoint(7,0));
    m_position.addPieceAt(new MRook(MPiece::WHITE), QPoint(0,7));
    m_position.addPieceAt(new MRook(MPiece::WHITE), QPoint(7,7));

    m_position.addPieceAt(new MKnight(MPiece::BLACK), QPoint(1,0));
    m_position.addPieceAt(new MKnight(MPiece::BLACK), QPoint(6,0));
    m_position.addPieceAt(new MKnight(MPiece::WHITE), QPoint(1,7));
    m_position.addPieceAt(new MKnight(MPiece::WHITE), QPoint(6,7));

    updateMaterialInfo();
    Q_EMIT positionChanged(m_position);
}


void MGame::onPieceMoveRequested(QPoint from, QPoint to)
{
    static QTime profiling;
    Q_EMIT sendDebugInfo(QString("MGame::onPMR - time between moves: %1 ms").arg(profiling.elapsed()));
    profiling.restart();

    if(m_logic_analyzer.verifyMove(m_position, from, to))
    {
        bool captured = m_position.movePiece(from, to);
        Q_EMIT pieceMoved(from, to, captured);
        Q_EMIT positionChanged(m_position);
    }
    else
    {
        Q_EMIT invalidMove(from, to);
    }

    Q_EMIT sendDebugInfo(QString("MGame::onPMR - update duration: %1 ms").arg(profiling.restart()));
}

int MGame::computeWhiteMaterial() const
{
    return 39;
}

int MGame::computeBlackMaterial() const
{
    return 39;
}

void MGame::updateMaterialInfo()
{
    m_player_info.white_material = computeWhiteMaterial();
    m_player_info.black_material = computeBlackMaterial();

    Q_EMIT playerInfoChanged();
}
