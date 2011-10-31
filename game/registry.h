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

#ifndef REGISTRY_H
#define REGISTRY_H

#include "dispatcher.h"
#include <QtCore>

namespace Game {

class Game;

//! Registry for games.
class Registry
    : public QObject
{    
private:
    WeakDispatcher m_dispatcher;
    QVector<Game *> m_games;

public:
    //! C'tor
    //! @param parent the (optional) owner of this instance.
    explicit Registry(QObject *parent = 0);

    //! D'tor
    virtual ~Registry();

    //! Sets the Dispatcher for this instance. A dispatcher instance is
    //! required to make the Registry fully operational.
    //! @param dispatcher the dispatcher instance.
    void setDispatcher(Dispatcher *dispatcher);

    //! Registers a newly created game.
    //! @param game the game instance to register.
    void registerGame(Game *game);

    //! Unregisters a game, usually before game is deleted.
    //! @param game the game instance to unregister.
    void unregisterGame(Game *game);

    //! @returns game with given id, or 0 if not found.
    //! @param id the game id.
    Game * game(uint id) const;

    //! @returns whether game id is registered.
    //! @param id the game id
    bool isRegisteredGame(uint id) const;
};

} // namespace Game

#endif // REGISTRY_H
