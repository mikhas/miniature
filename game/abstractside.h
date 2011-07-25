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

#ifndef SIDE_H
#define SIDE_H

#include <QtCore>

namespace Game {

class Move;
class AbstractSide;
// TODO: Turn into a weak pointer, if side instances are really not supposed to outlive Game instances?
typedef QSharedPointer<AbstractSide> SharedAbstractSide;

// This class must not depend on the following classes:
// * Game::Game
// This class must not have member variables.
//! Represents a game side. Responsible for receiving and submitting moves.
//! Must be reimplemented for each backend (game mode).
class AbstractSide
    : public QObject
{
    Q_OBJECT

public:
    enum SideState {
        NotReady,
        Ready,
        RunInBackground
    };

    //! C'tor
    //! @param identifier the identifier for this side.
    explicit AbstractSide(const QString &identifier);
    virtual ~AbstractSide() = 0;

    //! Initialize parts of the backend that are costly or require a main loop.
    virtual void init() = 0;

    //! Notify others that this component is ready now, usually triggered after
    //! init() was called.
    Q_SIGNAL void ready();

    //! Returns current state.
    virtual SideState state() const = 0;

    //! Returns identifier for this side.
    virtual const QString &identifier() const = 0;

    //! Let this side run in background. Backends should stop costly
    //! computations and free resources where possible.
    virtual void runInBackground() = 0;

    //! Runs this side in foreground again, recovering from a previous
    //! runInBackground call.
    virtual void runInForeground() = 0;

    //! Emitted when a move has been submitted. If it is was this side's turn
    //! and the submitted move is invalid, then startMove will be called again
    //! (as if the turn had just started). Otherwise incorrectly sent moves
    //! will be silently discarded.
    //! @param move the submitted move.
    Q_SIGNAL void turnEnded(const Move &move);

    //! Starts new turn for this side. If move is the same as last move, then
    //! the submitted move was invalid.
    //! @param move the submitted move of the other side.
    void virtual startTurn(const Move &move) = 0;
};

} // namespace GAME

#endif // SIDE_H
