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

#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <QtCore>

namespace Game {

class Dispatcher;
class Registry;
class AbstractEngine;

namespace Frontend {
    class Miniature;
}

//! Specify the target for a command. Some commands, such as
//! TargetBackgroundGames, will be executed as a broadcast. It is the
//! responsbility of the command's exec override to filter the actual target
//! from a list of offered targets.
enum Target {
    TargetNone, // Commands with no target are skipped.
    TargetFrontend, // GUI, command line, ...
    TargetEngine, // The currently active engine, e.g., freechess.org
    TargetRegistry // The game registry. Knows about all (active) games.
};

//! Defines the interface for commands that can be send to/received from the
//! Dispatcher.
//! When a sender creates a command, enough context must be supplied so that
//! the Dispatcher can find the concrete receiver.
class AbstractCommand
{
private:
    Target m_target;

public:
    explicit AbstractCommand(Target target = TargetNone);
    virtual ~AbstractCommand() = 0;

    //! \returns the target for this command.
    virtual Target target() const;

    //! \returns whether command is valid.
    virtual bool valid() const;

    //! Execute command on registry.
    virtual void exec(Dispatcher *dispatcher,
                      Registry *target);

    //! Execute command on backend.
    virtual void exec(Dispatcher *dispatcher,
                      AbstractEngine *target);

    //! Execute command on frontend.
    virtual void exec(Dispatcher *dispatcher,
                      Frontend::Miniature *target);
};

} // namespace Game

#endif // ABSTRACTCOMMAND_H
