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

#include <QtCore>
#include <memory>

namespace Game {

//! Controls game.
class Game
    : public QObject
{
    Q_OBJECT

private:
    typedef std::auto_ptr<AbstractSide> AutoAbstractSide;
    AutoAbstractSide m_local; //!< Side of the local player.
    AutoAbstractSide m_remote; //!< Side of the remote player.
    AbstractSide *m_active; //!< Pointer to active side.

public:
    explicit Game(QObject *parent = 0);
    virtual ~Game();

    //! Starts game.
    void start();

    //! Active side.
    const AbstractSide &activeSide() const;

    //! One side ended and submitted a move.
    Q_SLOT void onMoveEnded(const AbstractSide &side,
                            const Move &move);

    //! Waits for input on command line.
    Q_SLOT void waitForInput();
};

} // namespace Game

#endif // GAME_H
