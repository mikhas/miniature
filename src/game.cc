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
  m_half_move(-1),
  m_trans_captured_piece(0),
  m_selected_piece(0),
  m_logic_analyzer(0),
  m_is_bottom_player_white(true)
{
    Q_ASSERT(m_view);

    setupBoardItem();
    setupStartPosition();

    // process state transition requests
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

    newGame();
}

MGame::~MGame()
{}

void MGame::setupBoardItem()
{
    Q_ASSERT(m_view);
    // If m_board_item was already setup we will leak memory here. We cannot
    // delete MPieces directly (which would happen if we delete a
    // MGraphicsBoardItem) as long as they are owned by MPosition instances
    // (shared pointers semantics w.r.t. MPieces).
    Q_ASSERT(!m_board_item);

    m_board_item = new MGraphicsBoardItem;
    m_view->addBoardItem(m_board_item);

    // process state transition requests
    connect(m_board_item, SIGNAL(pieceClicked(MPiece *)),
            this, SLOT(onPieceClicked(MPiece *)));
    connect(m_board_item, SIGNAL(targetClicked(QPoint)),
            this, SLOT(onTargetClicked(QPoint)));
    connect(m_board_item, SIGNAL(undoMoveRequest()),
            this, SLOT(onUndoMoveRequested()));
}

void MGame::newGame()
{
    jumpToStart();
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

// TODO: remove this method?
void MGame::rotateWhitePieces()
{
    MPosition pos = m_game[m_half_move];
}

// TODO: remove this method?
void MGame::rotateBlackPieces()
{
    MPosition pos = m_game[m_half_move];
}

void MGame::setupStartPosition()
{
    // Don't try to delete pieces individually from a MGame instance. The
    // MSharedPieces and the scene graph conflict w.r.t. to ownership!

    m_game.clear();
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
    m_half_move = 0;

    Q_ASSERT(!m_game.empty());
}

void MGame::addPieceToPositionAt(MPiece *piece, MPosition *pos, QPoint cell)
{
    Q_CHECK_PTR(m_board_item);
    Q_CHECK_PTR(piece);
    Q_CHECK_PTR(pos);

    pos->addPieceAt(piece, cell);

    piece->show();
    piece->setParentItem(m_board_item);
}

bool MGame::isValidPosition(int half_move) const
{
    return (!m_game.empty() && -1 < half_move && half_move < m_game.size());
}

void MGame::setPositionTo(int half_move)
{
    if (isValidPosition(half_move))
    {
        deSelectPiece();
        m_half_move = half_move;
        MPosition pos = m_game[m_half_move];

        pos.updatePieces();
    }
}

void MGame::setActionAreaStates(MActionArea::State s1, MActionArea::State s2)
{
    // current mapping is white = bottom
    if (isTurnOfBottomPlayer())
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

bool MGame::isTurnOfTopPlayer() const
{
    return !isTurnOfBottomPlayer();
}


bool MGame::isTurnOfBottomPlayer() const
{
    return (m_half_move % 2 == (m_is_bottom_player_white ? 0 : 1));
}

void MGame::undoMove()
{
    if (m_selected_piece)
    {
        m_selected_piece->moveTo(m_selected_piece->mapFromCell(m_selected_piece_cell));
        if (m_trans_captured_piece)
        {
            m_trans_captured_piece->show();
            m_trans_captured_piece = 0;
        }
    }
}

void MGame::deSelectPiece()
{
    if (m_selected_piece)
    {
        m_selected_piece->deSelect();
        m_selected_piece = 0;
        m_selected_piece_cell = QPoint(-1,-1);
    }
}

void MGame::selectPiece(MPiece *piece)
{
    if (piece)
    {
        undoMove();
        deSelectPiece();
        m_selected_piece = piece;
        m_selected_piece->select();
        m_selected_piece_cell = m_selected_piece->mapToCell();
    }
}

void MGame::onPieceClicked(MPiece *piece)
{
    // If invalid piece was selected => reset selection, purple flashing.
    // Currently, most piece selections are "valid" (because the logic analyzer
    // doesnt know how to handle that yet)
    Q_ASSERT(isValidPosition(m_half_move));

    // Only allow selection if piece has the correct colour.
    if (piece && piece->getColour() == (m_game[m_half_move]).getColourToMove())
    {
        selectPiece(piece);
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
}

void MGame::onTargetClicked(const QPoint &target)
{
    // ignore invalid mouse clicks
    if (!m_selected_piece || !m_selected_piece->isSelected())
    {
        return;
    }

    Q_ASSERT(isValidPosition(m_half_move));
    m_trans_position = MPosition(m_game[m_half_move]);
    // Undo any move that was maybe done from the previous m_trans_position.
    undoMove(); // TODO: refine this if moving back is visible to the user ... unlikely

    QPoint origin = m_selected_piece_cell;
    if (origin == target) // hum, cancel?
    {
        onUndoMoveRequested();
        // Dangerous return statement. Makes the surrounding code more fragile because
        // this special case is not aware of subtle changes in the following code.
        return; 
    }

    MLogicAnalyzer::MStateFlags result = m_logic_analyzer.verifyMove(m_trans_position, origin, target);
    if (MLogicAnalyzer::VALID & result)
    {
        // TODO: check for capture flag instead.
        MSharedPiece maybe_captured = m_trans_position.pieceAt(target);
        if(!maybe_captured.isNull())
        {
            m_trans_captured_piece = maybe_captured.data();
        }

        setActionAreaStates(MActionArea::NONE, MActionArea::TARGET_SELECTED);
        MPiece::MColour colour = m_trans_position.getColourToMove();
        m_trans_position.movePiece(origin, target);

        if ((MLogicAnalyzer::CHECK | MLogicAnalyzer::CHECKMATE) & result)
        {
            //Q_EMIT check();
        }

        if (MLogicAnalyzer::PROMOTION & result)
        {
            m_trans_position.addPieceAt(new MQueen(colour), target);
        }
    }
}

void MGame::onMoveConfirmed()
{
    // Assumption: The move is already visible on the board, hence we only add
    // the transitional position to the position list.
    m_trans_position.nextColour();
    ++m_half_move;
    m_game.insert(m_half_move, m_trans_position);

    m_trans_captured_piece = 0;
    deSelectPiece();
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);

    // TODO: mark m_trans_position as empty again, probably by using a smart pointer and resetting it here.
}

void MGame::onPieceSelectionCancelled()
{
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
    undoMove();
    deSelectPiece();
}

void MGame::onUndoMoveRequested()
{
    setActionAreaStates(MActionArea::TURN_ENDED, MActionArea::TURN_STARTED);
    undoMove();
    deSelectPiece();
}
