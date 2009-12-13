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

MGame::MGame(MBoardView *view, QObject *parent)
: QObject(parent),
  m_view(view),
  m_half_move(-1),
  m_logic_analyzer(0),
  m_is_bottom_player_white(true)
{
    Q_ASSERT(m_view);

    // process state transition requests
    connect(m_view, SIGNAL(pieceSelectionRequest(QPoint)),
            this, SLOT(onPieceSelectionRequested(QPoint)));
    connect(m_view, SIGNAL(pieceMoveRequest(QPoint, QPoint)),
            this, SLOT(onPieceMoveRequested(QPoint, QPoint)));
    connect(m_view, SIGNAL(undoMoveRequest()),
            this, SLOT(onUndoMoveRequested()));
    connect(&m_top_action_area, SIGNAL(moveConfirmed()),
            this, SLOT(onMoveConfirmed()));
    connect(&m_bottom_action_area, SIGNAL(moveConfirmed()),
            this, SLOT(onMoveConfirmed()));
    connect(&m_top_action_area, SIGNAL(pieceSelectionCancelled()),
            this, SLOT(onPieceSelectionCancelled()));
    connect(&m_bottom_action_area, SIGNAL(pieceSelectionCancelled()),
            this, SLOT(onPieceSelectionCancelled()));

    m_view->setTopActionArea(m_top_action_area.createActionAreaProxyWidget(QString("qgil")));
    m_view->setBottomActionArea(m_bottom_action_area.createActionAreaProxyWidget(QString("kore")));
}

MGame::~MGame()
{}

void MGame::newGame()
{
    m_half_move = -1;
    MPosition pos = setupStartPosition();
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
}

void MGame::jumpToStart()
{
    setPositionTo(0);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
}

void MGame::prevMove()
{
    setPositionTo(m_half_move - 1);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
}

void MGame::nextMove()
{
    setPositionTo(m_half_move + 1);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
}

void MGame::jumpToEnd()
{
    setPositionTo(m_game.size() - 1);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
}

void MGame::rotateWhitePieces()
{
    m_view->rotateWhitePieces();
    MPosition pos = m_game[m_half_move];
    updateBoardView(pos);
}

void MGame::rotateBlackPieces()
{
    m_view->rotateBlackPieces();
    MPosition pos = m_game[m_half_move];
    updateBoardView(pos);
}

MPosition MGame::setupStartPosition()
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
    pos.resetCastling();


    m_game.append(pos);
    m_half_move = 0;
    m_view->resetCache();
    m_view->drawPosition(pos);

    Q_ASSERT(!m_game.empty());
    return m_game[m_half_move];
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
        MPosition pos = m_game[m_half_move];
        m_view->resetCache();
        updateBoardView(pos);
    }
}

void MGame::updateBoardView(const MPosition& pos)
{
    m_view->drawPosition(pos);
}

void MGame::setActionAreaStates(MActionArea::State s1, MActionArea::State s2)
{
    // current mapping is white = bottom
    if (isBottomPlayersTurn())
    {
        m_top_action_area.setState(s1);
        m_bottom_action_area.setState(s2);
    }
    else
    {
        m_top_action_area.setState(s2);
        m_bottom_action_area.setState(s1);
    }
}

bool MGame::isTopPlayersTurn() const
{
    return !isBottomPlayersTurn();
}


bool MGame::isBottomPlayersTurn() const
{
    return (m_half_move % 2 == (m_is_bottom_player_white ? 0 : 1));
}

void MGame::onPieceSelectionRequested(QPoint cell)
{
    // if invalid piece was selected => reset selection, purple flashing.
    // currently, all piece selections are "valid" (because the logic analyzer
    // doesnt know how to handle that yet)

    // assume request is valid
    Q_UNUSED(cell);
    setActionAreaStates(MActionArea::NONE, MActionArea::PIECE_SELECTED);
    m_view->drawPosition(m_game[m_half_move]);
}

void MGame::onPieceMoveRequested(QPoint from, QPoint to)
{
    static QTime profiling;
    Q_EMIT sendDebugInfo(QString("MGame::onPMR - time between moves: %1 ms").arg(profiling.elapsed()));
    profiling.restart();

    Q_ASSERT(isValidPosition(m_half_move));
    m_trans_position = MPosition(m_game[m_half_move]);

    MLogicAnalyzer::MStateFlags result = m_logic_analyzer.verifyMove(m_trans_position, from, to);
    if (MLogicAnalyzer::VALID & result)
    {
        setActionAreaStates(MActionArea::NONE, MActionArea::TARGET_SELECTED);
        MPiece::MColour colour = m_trans_position.getColourToMove();
        m_trans_position.movePiece(from, to);

        if ((MLogicAnalyzer::CHECK | MLogicAnalyzer::CHECKMATE) & result)
        {
            //Q_EMIT check();
        }

        if (MLogicAnalyzer::PROMOTION & result)
        {
            m_trans_position.addPieceAt(new MQueen(colour), to);
        }

        // Draw a temporary position, as this is not stored in the game yet.
        m_view->drawPosition(m_trans_position);
    }
    else
    {
        Q_EMIT invalidMove(from, to);
    }

    Q_EMIT sendDebugInfo(QString("MGame::onPMR - update duration: %1 ms").arg(profiling.restart()));
}

void MGame::onMoveConfirmed()
{
    m_trans_position.nextColour();
    ++m_half_move;
    m_game.insert(m_half_move, m_trans_position);

    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);

    m_view->resetPieceSelection();
    // TODO: Clean this up! Currently needs to be redrawn because now pieces of
    // the other colour can be selected (the side effect of
    // m_trans_position.nextColour) . Confirms that doing this
    // white/black-to-move thing in MGraphicsBoardItem was wrong, can be solved
    // w/ the valid piece selection check.
    m_view->drawPosition(m_trans_position);

    // TODO: mark m_trans_position as empty again, probably by using a smart pointer and resetting it here.
}

void MGame::onPieceSelectionCancelled()
{
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
    m_view->resetPieceSelection();
}

void MGame::onUndoMoveRequested()
{
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
    m_view->resetPieceSelection();
    m_view->drawPosition(m_game[m_half_move]);
}
