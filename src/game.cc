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
  m_logic_analyzer(0)
{
    m_player_info.white_name = QString("White");
    m_player_info.white_rating = QString("4321");
    m_player_info.white_material = computeWhiteMaterial();

    m_player_info.black_name = QString("Black");
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
    updateMoveInfo(m_game.last().getColourToMove());
}

void MGame::nextMove()
{
    setPositionTo(m_half_move + 1);
}

void MGame::prevMove()
{
    setPositionTo(m_half_move - 1);
}

void MGame::setupStartPosition()
{
    m_game.clear();
    Q_ASSERT(m_game.empty());

    MPosition pos;

    pos.addPieceAt(new MRook(MPiece::BLACK), QPoint(0,0));
    pos.addPieceAt(new MRook(MPiece::BLACK), QPoint(7,0));
    pos.addPieceAt(new MRook(MPiece::WHITE), QPoint(0,7));
    pos.addPieceAt(new MRook(MPiece::WHITE), QPoint(7,7));

    pos.addPieceAt(new MKnight(MPiece::BLACK), QPoint(1,0));
    pos.addPieceAt(new MKnight(MPiece::BLACK), QPoint(6,0));
    pos.addPieceAt(new MKnight(MPiece::WHITE), QPoint(1,7));
    pos.addPieceAt(new MKnight(MPiece::WHITE), QPoint(6,7));

    pos.addPieceAt(new MBishop(MPiece::BLACK), QPoint(2,0));
    pos.addPieceAt(new MBishop(MPiece::BLACK), QPoint(5,0));
    pos.addPieceAt(new MBishop(MPiece::WHITE), QPoint(2,7));
    pos.addPieceAt(new MBishop(MPiece::WHITE), QPoint(5,7));

    pos.addPieceAt(new MQueen(MPiece::BLACK), QPoint(3,0));
    pos.addPieceAt(new MQueen(MPiece::WHITE), QPoint(3,7));

    pos.addPieceAt(new MKing(MPiece::BLACK), QPoint(4,0));
    pos.addPieceAt(new MKing(MPiece::WHITE), QPoint(4,7));

    for (int i = 0; i < 8; ++i)
    {
        pos.addPieceAt(new MPawn(MPiece::BLACK), QPoint(i,1));
        pos.addPieceAt(new MPawn(MPiece::WHITE), QPoint(i,6));
    }


    m_game.append(pos);
    m_half_move = 0;
    Q_EMIT positionChanged(pos);
    updateMaterialInfo();

    Q_ASSERT(!m_game.empty());
}

bool MGame::isValidPosition(int half_move) const
{
    return (!m_game.empty() && -1 < half_move && half_move < m_game.size());
}

void MGame::setPositionTo(int half_move)
{
    if (isValidPosition(half_move))
    {
        m_half_move = half_move;
        updateMaterialInfo();
        Q_EMIT positionChanged(m_game[half_move]);
    }
}

void MGame::onPieceMoveRequested(QPoint from, QPoint to)
{
    static QTime profiling;
    Q_EMIT sendDebugInfo(QString("MGame::onPMR - time between moves: %1 ms").arg(profiling.elapsed()));
    profiling.restart();

    Q_ASSERT(isValidPosition(m_half_move));
    MPosition pos = MPosition(m_game[m_half_move]);

    MLogicAnalyzer::MStateFlags result = m_logic_analyzer.verifyMove(pos, from, to);
    if (MLogicAnalyzer::VALID & result)
    {
        MPiece::MColour colour = pos.pieceAt(from)->getColour(); // ugly
        QString notation = pos.movePiece(from, to);

        pos.nextColour();
        updateMoveInfo(pos.getColourToMove() == MPiece::WHITE);

        // TODO: remove capturing code in MPostion and control the relevant bits from here!
        /*
        if (MLogicAnalyzer::CAPTURE & result)
        {
            m_position.removePieceAt(to);
            Q_EMIT pieceCapturedAt(to);
        }
        */

        if ((MLogicAnalyzer::CHECK | MLogicAnalyzer::CHECKMATE) & result)
        {
            Q_EMIT check();
        }

        if (MLogicAnalyzer::PROMOTION & result)
        {
            pos.addPieceAt(new MQueen(colour), to);
            Q_EMIT pawnPromoted(to);
        }

        Q_EMIT pieceMoved(m_half_move, notation);
        Q_EMIT positionChanged(pos);

        // Important: increase counter first, so that for m_half_move ==
        // m_game.size() this will result in appending pos.
        ++m_half_move;
        m_game.insert(m_half_move, pos);
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

void MGame::updateMoveInfo(bool is_white_moving)
{
    // ugly ...
    const static QString base_white(m_player_info.white_name);
    const static QString base_black(m_player_info.black_name);

    if (is_white_moving)
    {
        m_player_info.white_name = QString("<span style=\"text-decoration:underline;\">%1</span>").arg(base_white);
        m_player_info.black_name = QString(base_black);
    }
    else
    {
        m_player_info.white_name = QString(base_white);
        m_player_info.black_name = QString("<span style=\"text-decoration:underline;\">%1</span>").arg(base_black);
    }

    Q_EMIT playerInfoChanged();
}
