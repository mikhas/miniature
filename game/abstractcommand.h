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

class Game;
class AbstractLink;

// Specify the target for a command. Some commands, such as
// TargetBackgroundGames, will be executed as a broadcast. It is the
// responsbility of the command's exec override to filter the actual target
// from a list of offered targets.
enum Target {
    TargetUserInterface, // GUI, command line, ...
    TargetEngine, // Gnuchess, ...
    TargetServer, // FICS, ...
    TargetActiveGame,
    TargetBackgroundGames
};

//! Defines the interface for commands that can be send to/received from the
//! Dispatcher.
//! When a sender creates a command, enough context must be supplied so that
//! the Dispatcher can find the concrete receiver.
class AbstractCommand
{
public:
    explicit AbstractCommand(Target t);
    virtual ~AbstractCommand() = 0;

    // \returns the target for this command.
    virtual Target target() const = 0;

    // TODO: Define set of interfaces for command receivers, so that we can
    // have a set of exec overloads? Would allow to handle same command
    // differently depending on receiver. Such receivers could be:
    // - backends such as UI, local chess engine, remote server, PBeM
    // - current game
    // - local, remote, active side (of current game, but also of games running
    //   background).
    // \returns whether the command was consumed. If true, this command will
    //          not call exec on a potentially next target.
    virtual bool exec(Game *target);
    virtual bool exec(AbstractLink *target);

private:
    AbstractCommand(const AbstractCommand &);
    AbstractCommand &operator=(const AbstractCommand &);
};

} // namespace Game

#endif // ABSTRACTCOMMAND_H
