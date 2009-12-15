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

#ifndef GAME_H__
#define GAME_H__

#include "position.h"
#include "logic_analyzer.h"
#include "board_view.h"
#include "action_area.h"
#include "graphics_board_item.h"

#include <QList>
#include <QGraphicsView>
#include <QString>
#include <QObject>

namespace Miniature
{
/* This class represents the data structure for a single game. It communicates
 * with the various backends (fics, freechess, chess engines, ...) and is also
 * responsible for sending the current chess position to the board. Therefore,
 * this class needs to be able to convert an internal chess position into a FEN
 * string.
 */
class MGame
: public QObject
{
    Q_OBJECT

public:
    typedef QList<MPosition> MPositionList;

    explicit MGame(MBoardView* view, QObject* parent = 0);
    virtual ~MGame();

public Q_SLOTS:
    /* Reset the game's state and start a new game.*/
    void newGame();
    /* Game navigation slots. */
    void jumpToStart();
    void prevMove();
    void nextMove();
    void jumpToEnd();
    void rotateBlackPieces();
    void rotateWhitePieces();

    /* If a game was started, sets position to the turn specified by half_move
     * (one full move: white and black moved, hence half moves). If half_move
     * specifies an invalid position then the current position is unchanged.
     * Notice how half_move = 0 would yield the start position in a normal
     * game.
     */
    void setPositionTo(int half_move);

    /* Process user requests coming from the MGraphicsBoardItem or MActionAreas. */
    void onPieceClicked(MPiece *piece);
    void onTargetClicked(const QPoint &cell);

    void onMoveConfirmed();
    void onPieceSelectionCancelled();
    void onUndoMoveRequested();

Q_SIGNALS:
    void sendDebugInfo(QString msg);

private:
    void setupBoardItem();

    /* Creates a MPosition with the default chess start position and stores it
     * in m_game. */
    void setupStartPosition();

    /* Connects signals of the piece to controller's slots, adds piece to a
     * position (which wraps it in a shared pointer and therefore, takes (shared)
     * ownership. One piece is shared among all positions of a game.
     */
    void addPieceToPositionAt(MPiece *piece, MPosition *pos, QPoint cell);

    /* Returns whether the position identified by half_move exists in m_game.
     */
    bool isValidPosition(int half_move) const;

    /* Sets both action area states in one go. */
    void setActionAreaStates(MActionArea::State s1, MActionArea::State s2);

    bool isTurnOfTopPlayer() const;
    bool isTurnOfBottomPlayer() const;

    void undoMove();
    void deSelectPiece();
    void selectPiece(MPiece *piece);
    void updatePlayerStatus(const MPosition &position);

    /* A reference to the board view, we do not take ownership. */
    MBoardView *m_view;

    /* A container for all pieces in the scene graph, useful for coord mapping. */
    MGraphicsBoardItem *m_board_item;

    /* An index to the positions stored in m_game, conceptually each half move
     * is indexed. */
    int m_half_move;

    /* Stores the game history as a list of positions. */
    MPositionList m_game;

    /* Stores the  position used for a transition. After a valid transition was
     * completed it will be added to the game history. */
    MPosition m_trans_position;

    /* We need to remember the last captured piece during a turn so we can redraw it on undo. */
    MPiece *m_trans_captured_piece;

    /* We need to remember the currently selected piece on the board. */
    MPiece *m_selected_piece;

    /* We also remember *where* the piece was selected. This is not necessarily
     * the scene position of the piece, as it could have already been moved. */
    QPoint m_selected_piece_cell;

    /* The logic analyzer belonges to MGame and is used to validates moves and positions. */
    MLogicAnalyzer m_logic_analyzer;

    /* TODO: remove the silly assumption that white = bottom, black = top */
    /* Each player has an action area outside of the board, the local player is
     * always located at the bottom of the board. */
    MActionArea m_top_action_area;
    MActionArea m_bottom_action_area;

    bool m_is_bottom_player_white;
};

}; // namespace Miniature
#endif
