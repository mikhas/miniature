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

#include <position.h>
#include <board_view.h>
#include <graphics_board_item.h>
#include <game_log.h>
#include <game_store.h>

#include <QtCore>
#include <QtGui>

namespace Miniature
{

/*!
 *  This class represents the data structure for a single game. It communicates
 *  with the various backends (fics, freechess, chess engines, ...) and is also
 *  responsible for sending the current chess position to the board. Therefore,
 *  this class needs to be able to convert an internal chess position into a
 *  FEN string.
 */
class MGame
: public QObject
{
    Q_OBJECT

public:

    explicit MGame(MBoardView *view, MGameLog *log, QObject *parent = 0);
    virtual ~MGame();

public Q_SLOTS:
    void newGame();
    void jumpToStart();
    void prevMove();
    void nextMove();
    void jumpToEnd();
    void abortGame();
    void onPositionPasted();

Q_SIGNALS:
    void sendDebugInfo(QString msg);
    void flipBoard();
    void turnOfTopPlayer();
    void turnOfBottomPlayer();
    void togglePieceRotations();

private Q_SLOTS:
    void onWhiteToMove(const MPosition &position);
    void onBlackToMove(const MPosition &position);
    void onCandidatePieceSelected();
    void onMoveDiscarded();
    void onMoveConfirmationRequested();
    void onInvalidTargetSelected();

private:
    void startTurn(const MPosition &position, MDashboardItem *const dashboard);

    bool isWhiteAtBottom() const;
    bool isBlackAtBottom() const;

    void setupPositionPasting();
    void setupBoardItem();

    /*!
     *  Helper method to activate dashboard/game interaction.
     *  @param[in] dashboard the dashboard item that interacts with this MGame
     *  instance.
     */
    void connectDashboardToGame(MDashboardItem *const dashboard);

    /*!
     *  Helper method to activate the interaction between the two dashboard items.
     *  To connect the items in both ways, call the method with swapped parameters.
     *  @param[in] first the first dashboard item
     *  @param[in] second the second dashboard item
     */
    void connectDashboards(MDashboardItem *const first, MDashboardItem *const second);
    void updatePlayerStatus(const MPosition &position);

    MBoardView *m_view; /*< A reference to the board view. >*/
    MGameLog *m_log; /*< A reference to the game log. >*/
    MGraphicsBoardItem *m_board_item; /* A container for all pieces in the
        scene graph, useful for coord mapping. >*/
    MGameStore *m_game_store; /*< A reference to the game store, owned by
        this class. >*/
    MDashboardItem *m_current_dashboard; /*< A reference to the dashboard of
        the currently moving player. >*/
};

} // namespace Miniature
#endif
