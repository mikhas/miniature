/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef GAME_H
#define GAME_H

#include "abstractside.h"
#include "namespace.h"

#include <QtCore>

namespace Game {

class GamePrivate;

//! A Game instance represents exactly one game. A game goes from Idle =>
//! Started => Ended and cannot be restarted.
class Game
    : public QObject
{
    Q_OBJECT

private:
    const QScopedPointer<GamePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Game)
    Q_DISABLE_COPY(Game)

public:
    enum GameState {
        Idle,
        Started,
        Ended
    };

    //! C'tor
    //! @param local the local side, Game takes ownership.
    //! @param remote the remote side, Game takes ownership.
    //! @param parent the optional parent that owns this instance.
    explicit Game(AbstractSide *local,
                  AbstractSide *remote,
                  QObject *parent = 0);
    virtual ~Game();

    //! Starts game.
    void start();

    //! Active side.
    const SharedAbstractSide &activeSide() const;

private:
    //! One side ended turn and submitted a move.
    //! @param move the submitted move.
    Q_SLOT void onTurnEnded(const Move &move);

    //! Connects common parts for each side with controller.
    //! @param side the side to connect.
    void connectSide(const SharedAbstractSide &side);

    //! Used to sync with side backends; start when all backends report
    //! readiness.
    Q_SLOT void onSideReady();
};

} // namespace Game

#endif // GAME_H
