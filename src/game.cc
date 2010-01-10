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
  m_board_item(0),
  m_half_move_index(-1),
  m_trans_half_move(MPosition()),
  m_is_bottom_player_white(true)
{
    Q_ASSERT(m_view);

    // Process state transition requests.
    connect(&m_top_action_area, SIGNAL(moveConfirmed()),
            this, SLOT(onMoveConfirmed()));
    connect(&m_bottom_action_area, SIGNAL(moveConfirmed()),
            this, SLOT(onMoveConfirmed()));

    m_view->setTopActionArea(m_top_action_area.getProxyWidget());
    m_view->setBottomActionArea(m_bottom_action_area.getProxyWidget());

    m_top_action_area.setText(QString("qgil"));
    m_bottom_action_area.setText(QString("kore"));

    // Enable rotation for action areas.
    connect(this, SIGNAL(turnOfTopPlayer()),
            &m_top_action_area, SLOT(rotate180()));
    connect(this, SIGNAL(turnOfTopPlayer()),
            &m_bottom_action_area, SLOT(rotate180()));

    connect(this, SIGNAL(turnOfBottomPlayer()),
            &m_top_action_area, SLOT(rotate0()));
    connect(this, SIGNAL(turnOfBottomPlayer()),
            &m_bottom_action_area, SLOT(rotate0()));

    newGame();
}

MGame::~MGame()
{}

void MGame::setupBoardItem()
{
    Q_ASSERT(m_view);

    if (m_board_item)
    {
        delete m_board_item;
        m_board_item = 0;
    }

    m_board_item = new MGraphicsBoardItem;
    m_view->addBoardItem(m_board_item);

    // process state transition requests
    connect(m_board_item, SIGNAL(pieceClicked(MPiece *)),
            this, SLOT(onPieceClicked(MPiece *)));
    connect(m_board_item, SIGNAL(targetClicked(QPoint)),
            this, SLOT(onTargetClicked(QPoint)));

    // connecting signals to signals (which act as if a slot would re-emit
    // them) is a nice feature
    connect(this, SIGNAL(turnOfTopPlayer()),
            m_board_item, SIGNAL(rotatePieces180()));

    connect(this, SIGNAL(turnOfBottomPlayer()),
            m_board_item, SIGNAL(rotatePieces0()));

    connect(this, SIGNAL(togglePieceRotations()),
            m_board_item, SIGNAL(togglePieceRotations()));
}

void MGame::addPieceToPositionAt(MPiece *const piece, MPosition *const position, const QPoint &target)
{
    Q_CHECK_PTR(m_board_item);
    Q_CHECK_PTR(piece);
    Q_CHECK_PTR(position);

    // Does not take ownership of the piece!
    position->addPieceAt(piece, target);

    // Does take ownership of the piece.
    m_board_item->addPiece(piece);
}

void MGame::newGame()
{
    setupBoardItem();
    setupStartPosition();

    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
}

void MGame::jumpToStart()
{
    setPositionTo(0);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
}

void MGame::prevMove()
{
    setPositionTo(m_half_move_index - 1);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
}

void MGame::nextMove()
{
    setPositionTo(m_half_move_index + 1);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
}

void MGame::jumpToEnd()
{
    setPositionTo(m_game.size() - 1);
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
}

void MGame::setupStartPosition()
{
    m_game.clear();
    m_game = MPositionList();
    Q_ASSERT(m_game.empty());

    MPosition pos;

    addPieceToPositionAt(new MRook(MPiece::BLACK), &pos, QPoint(0,0));
    addPieceToPositionAt(new MRook(MPiece::BLACK), &pos, QPoint(7,0));
    addPieceToPositionAt(new MRook(MPiece::WHITE), &pos, QPoint(0,7));
    addPieceToPositionAt(new MRook(MPiece::WHITE), &pos, QPoint(7,7));

    addPieceToPositionAt(new MKnight(MPiece::BLACK), &pos, QPoint(1,0));
    addPieceToPositionAt(new MKnight(MPiece::BLACK), &pos, QPoint(6,0));
    addPieceToPositionAt(new MKnight(MPiece::WHITE), &pos, QPoint(1,7));
    addPieceToPositionAt(new MKnight(MPiece::WHITE), &pos, QPoint(6,7));

    addPieceToPositionAt(new MBishop(MPiece::BLACK), &pos, QPoint(2,0));
    addPieceToPositionAt(new MBishop(MPiece::BLACK), &pos, QPoint(5,0));
    addPieceToPositionAt(new MBishop(MPiece::WHITE), &pos, QPoint(2,7));
    addPieceToPositionAt(new MBishop(MPiece::WHITE), &pos, QPoint(5,7));

    addPieceToPositionAt(new MQueen(MPiece::BLACK), &pos, QPoint(3,0));
    addPieceToPositionAt(new MQueen(MPiece::WHITE), &pos, QPoint(3,7));

    addPieceToPositionAt(new MKing(MPiece::BLACK), &pos, QPoint(4,0));
    addPieceToPositionAt(new MKing(MPiece::WHITE), &pos, QPoint(4,7));

    for (int i = 0; i < 8; ++i)
    {
        addPieceToPositionAt(new MPawn(MPiece::BLACK), &pos, QPoint(i,1));
        addPieceToPositionAt(new MPawn(MPiece::WHITE), &pos, QPoint(i,6));
    }
    pos.resetCastling();

    pos.updatePieces();

    m_game.append(pos);
    m_half_move_index = 0;
    m_trans_half_move = mHalfMove(pos);

    Q_ASSERT(!m_game.empty());
}

bool MGame::isValidPosition(int half_move) const
{
    return (!m_game.empty() && -1 < half_move && half_move < m_game.size());
}

void MGame::setPositionTo(int half_move)
{
    Q_CHECK_PTR(m_board_item);

    if (isValidPosition(half_move))
    {
        m_half_move_index = half_move;
        MPosition pos = m_game[m_half_move_index];

        m_board_item->hidePieces();
        pos.updatePieces();
        m_trans_half_move = mHalfMove(pos);
        updatePlayerStatus(pos);
    }
}

void MGame::setActionAreaStates(MActionArea::State s1, MActionArea::State s2, bool emit_turn_signal)
{
    // current mapping is white = bottom
    if (isTurnOfBottomPlayer())
    {
        m_top_action_area.setState(s1);
        m_bottom_action_area.setState(s2);

        if (emit_turn_signal)
        {
            Q_EMIT turnOfBottomPlayer();
        }
    }
    else
    {
        m_top_action_area.setState(s2);
        m_bottom_action_area.setState(s1);

        m_top_action_area.rotate(180);
        m_bottom_action_area.rotate(180);

        if (emit_turn_signal)
        {
            Q_EMIT turnOfTopPlayer();
        }
    }
}

bool MGame::isTurnOfTopPlayer() const
{
    return !isTurnOfBottomPlayer();
}


bool MGame::isTurnOfBottomPlayer() const
{
    return (m_half_move_index % 2 == (m_is_bottom_player_white ? 0 : 1));
}

void MGame::updatePlayerStatus(const MPosition &position)
{
    // assumption: white == bottom ...
    if (position.inCheck() && MPiece::WHITE == position.getColourToMove())
    {
        m_bottom_action_area.setText(QString("kore (check)"));
        m_top_action_area.setText(QString("qgil"));
    }
    else if (position.inCheck() && MPiece::BLACK == position.getColourToMove())
    {
        m_bottom_action_area.setText(QString("kore"));
        m_top_action_area.setText(QString("qgil (check)"));
    }
    else
    {
        m_bottom_action_area.setText(QString("kore"));
        m_top_action_area.setText(QString("qgil"));
    }
}

void MGame::onPieceClicked(MPiece *piece)
{
    // If invalid piece was selected => reset selection, purple flashing.
    // Currently, most piece selections are "valid" (because the logic analyzer
    // doesnt know how to handle that yet)
    Q_ASSERT(isValidPosition(m_half_move_index));

    // Only allow selection if piece has the correct colour.
    MPosition position = m_game[m_half_move_index];
    if (piece && piece->getColour() == position.getColourToMove())
    {
        m_trans_half_move.undo();
        m_trans_half_move = mHalfMove(position);
        m_trans_half_move.select(piece->mapToCell());

        setActionAreaStates(MActionArea::NONE, MActionArea::PIECE_SELECTED);
    }
    // Must be an attempt to capture an enemy piece. Let's forward it then!
    // Note: From my current understanding, this is the only case where
    // onTargetClicked is called when target points to a piece. Usually,
    // onTargetClicked is always only called when target points to an empty
    // cell.
    else if (piece)
    {
        onTargetClicked(piece->mapToCell());
    }

    // TODO: complain here
}

void MGame::onTargetClicked(const QPoint &target)
{
    // Ignores invalid mouse clicks, updates/reset action areas when move was
    // valid/invalid.
    if (m_trans_half_move.applyToTarget(target))
    {
        setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TARGET_SELECTED, true);
    }
    else
    {
        setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
    }
}

void MGame::onMoveConfirmed()
{
    // Assumption: The move is already visible on the board, hence we only add
    // the transitional position to the position list.
    MPosition position = MPosition(m_trans_half_move.getPosition());
    m_trans_half_move.deSelect();

    position.nextColour();
    ++m_half_move_index;
    m_game.insert(m_half_move_index, position);

    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED, true);
    updatePlayerStatus(position);
}
