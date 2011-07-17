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

#include "move.h"

#include <QObject>

namespace Game {

class AbstractSide;
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
    //! C'tor
    //! @param identifier the identifier for this side.
    explicit AbstractSide(const QString &identifier);
    virtual ~AbstractSide() = 0;

    //! Returns identifier for this side.
    virtual const QString &identifier() const = 0;

    //! Emitted once move has ended.
    //! @param move the submitted move.
    Q_SIGNAL void moveEnded(const Move &move);

    //! Starts new move for this side.
    //! @param move the submitted move of the other side.
    void virtual startMove(const Move &move) = 0;
};

} // namespace GAME

#endif // SIDE_H
