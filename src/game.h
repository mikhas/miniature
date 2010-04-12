/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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
    virtual void newGame();
    virtual void jumpToStart();
    virtual void prevMove();
    virtual void nextMove();
    virtual void jumpToEnd();
    virtual void abortGame();
    virtual void onPositionPasted();

Q_SIGNALS:
    virtual void togglePieceRotations();

private Q_SLOTS: // can be overriden even with public inheritance:
    virtual void onWhiteToMove(const MPosition &position);
    virtual void onBlackToMove(const MPosition &position);
    virtual void onCandidatePieceSelected();
    virtual void onMoveDiscarded();
    virtual void onMoveConfirmationRequested();
    virtual void onConfirmButtonPressed();
    virtual void onInvalidTargetSelected();

protected:
    virtual void endTurn();
    virtual void startTurn(const MPosition &position);

    virtual bool isWhiteAtBottom() const;
    virtual bool isBlackAtBottom() const;

    virtual void setupBoardItem();
    virtual void setupDashboard();

    /*!
     *  Helper method to activate dashboard/game interaction.
     *  @param[in] dashboard the dashboard item that interacts with this MGame
     *  instance.
     */
    virtual void connectDashboardToGame(MDashboardItem *const dashboard);

    //!  Updates the player status in the UI from the given \a position.
    virtual void updatePlayerStatus(const MPosition &position);

    MBoardView *m_view; /*< A reference to the board view. >*/
    MGameLog *m_log; /*< A reference to the game log. >*/
    MGraphicsBoardItem *m_board; /* A container for all pieces in the
        scene graph, useful for coord mapping. >*/
    MGameStore *m_store; /*< A reference to the game store, owned by
        this class. >*/
    MDashboardItem *m_dashboard; /*< A reference to the dashboard of
        the currently moving player. >*/

private:
    void setupPositionPasting();
};

} // namespace Miniature
#endif
