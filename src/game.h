/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
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

#include "board.h"

#include <vector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

/* This class represents the data structure for a single game. It communicates
 * with the various backends (fics, freechess, chess engines, ...) and is also
 * responsible for sending the current chess position to the board. Therefore,
 * this class needs to be able to convert an internal chess position into a FEN
 * string.
 */
class MiniatureGame
: public QObject
{
    Q_OBJECT

public:
    explicit MiniatureGame(QObject* parent = 0);
    virtual ~MiniatureGame();

    /* Set the view where we draw our scene graph onto. The game class does not
     * take ownership of the view!
     */
    void setSceneView(QGraphicsView* view);

    QString getDefaultStartPosition() const;
    QString convertToFen(unsigned int half_move) const;

public Q_SLOTS:
    /* Reset the game's state and start a new game, next/prev nagivation.*/
    void newGame();
    void nextMove();
    void prevMove();

private:
    // Formats player info, as seen on the mock-up. I am not sure the last line means turn or sth else.
    QString formatPlayerInfo(QString name, int rating, int turn, QString alignment) const;
    QString formatTimerInfo(QString time_remaining, bool isWhite) const;
    void addBoardToSceneGraph(QGraphicsScene *scene);

    /* Creates a scene graph containing all UI elements of the main view
     * (player cards, timer, board).
     */
    QGraphicsScene* createScene();

    std::vector<QString> createDummyGame() const;

    // To simplify things, our current internal model is just a list of FEN
    // strings, one for each position. I fully expect this to change in the future.
    std::vector<QString> m_game;
    unsigned int m_half_move;

    /* A reference to the view we are supposed to use. */
    QGraphicsView* m_view;

    /* A reference to the board that is used by the internal scene graph. */
    MiniatureBoard* m_board;
};
#endif
