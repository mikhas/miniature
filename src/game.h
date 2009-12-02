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

#include "player_info.h"
#include "position.h"
#include "logic_analyzer.h"

#include <QVector>
#include <QGraphicsView>
#include <QGraphicsScene>
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
    typedef QVector<MPosition> MPositionList;

    explicit MGame(QObject* parent = 0);
    virtual ~MGame();

    MPlayerInfo getPlayerInfo() const;

public Q_SLOTS:
    /* Reset the game's state and start a new game, next/prev nagivation.*/
    void newGame();

    void nextMove(); // unused
    void prevMove(); // unsued

    void onPieceMoveRequested(QPoint from, QPoint to);

Q_SIGNALS:
    void sendDebugInfo(QString msg);
    void playerInfoChanged();
    void positionChanged(const MPosition&);
    void pieceMoved(int half_move, const QString& last_move);
    void invalidMove(QPoint from, QPoint to);
    void check();
    void pawnPromoted(QPoint where);
    void pieceCapturedAt(QPoint where);

private:
    // Formats player info, as seen on the mock-up. I am not sure the last line means turn or sth else.
    QString formatPlayerInfo(QString name, int rating, int turn, QString alignment) const;
    QString formatTimerInfo(QString time_remaining, bool isWhite) const;
    void addBoardToSceneGraph(QGraphicsScene *scene);
    void setupStartPosition();

    /* Computes material for each side, using [1]
     * [1] http://en.wikipedia.org/wiki/Chess_piece_relative_value#Hans_Berliner.27s_system
     */
    int computeWhiteMaterial() const;
    int computeBlackMaterial() const;
    void updateMaterialInfo();
    void updateMoveInfo(bool is_white_moving);

    /* Creates a scene graph containing all UI elements of the main view
     * (player cards, timer, board).
     */
    //QGraphicsScene* createScene();

    int m_half_move;
    MPositionList m_game;

    // The current position + logic_analyzer
    //MPosition m_position;
    MLogicAnalyzer m_logic_analyzer;

    /* Stores player info, to be shown by the main window. */
    MPlayerInfo m_player_info;
};

}; // namespace Miniature
#endif
