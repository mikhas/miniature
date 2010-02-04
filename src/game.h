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
#include "half_move.h"
#include "board_view.h"
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
    void abortGame();

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

Q_SIGNALS:
    void sendDebugInfo(QString msg);
    void flipBoard();
    void turnOfTopPlayer();
    void turnOfBottomPlayer();
    void togglePieceRotations();

private:
    void setupBoardItem();

    /* Creates a MPosition with the default chess start position and stores it
     * in m_game. */
    void setupStartPosition();

    /* Connects signals of the piece to controller's slots, adds piece to a
     * position (which wraps it in a shared pointer and therefore, takes (shared)
     * ownership. One piece is shared among all positions of a game.
     */
    void addPieceToPositionAt(MPiece *const piece, MPosition *const position, const QPoint &target);

    /* Returns whether the position identified by half_move exists in m_game.
     */
    bool isValidPosition(int half_move) const;

    bool isTurnOfTopPlayer() const;
    bool isTurnOfBottomPlayer() const;

    /*!
     *  Helper method to activate the interaction between the two dashboard items.
     *  To connect the items in both ways, call the method with swapped parameters.
     *  @param[in] first the first dashboard item
     *  @param[in] second the second dashboard item
     */
    void connectDashboardItems(MDashboardItem *first, MDashboardItem *second);

    void updatePlayerStatus(const MPosition &position);

    /* A reference to the board view, we do not take ownership. */
    MBoardView *m_view;

    /* A container for all pieces in the scene graph, useful for coord mapping. */
    MGraphicsBoardItem *m_board_item;

    /* An index to the positions stored in m_game, conceptually each half move
     * is indexed. */
    int m_half_move_index;

    /* Stores the game history as a list of positions. */
    MPositionList m_game;

    /* Encapsulates all transitional data for a *potential* half move. Remember
     * that our UI requires a player to confirm moves so the UI needs to be
     * able to show candidate moves that will never be part of the game
     * history. MGame takes ownership. */
    // TODO: I *think* this one got auto_ptr semantics, all this delete/assign 0 stuff.
    mHalfMove m_trans_half_move;

    bool m_is_bottom_player_white;
};

}; // namespace Miniature
#endif
