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

#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <QtCore>

namespace Game
{

//! Available game commands.
enum Command
{
    CommandNone = 0, //!< Can be used as a default value or for unrecognized commands.
    CommandNew = 0x1,
    CommandMove = 0x2,
    CommandQuit = 0x4,
    CommandLogin = 0x8,
    CommandSeek = 0x10,
    CommandJoin = 0x20,
    CommandObserve = 0x40
};

//Q_ENUMS(Command)
Q_DECLARE_FLAGS(CommandFlags, Command)

//! Enumeration of rows - called 'ranks' - on a chess board. Range: [1-8].
enum Rank
{
    RankInvalid, //!< Can be used as a default value.
    Rank1,
    Rank2,
    Rank3,
    Rank4,
    Rank5,
    Rank6,
    Rank7,
    Rank8
};

//! Enumeration of columns - called 'files' - on a chess board. Range: [A-H].
enum File
{
    FileInvalid, //!< Can be used as a default value.
    FileA,
    FileB,
    FileC,
    FileD,
    FileE,
    FileF,
    FileG,
    FileH
};

} // namespace Game

Q_DECLARE_METATYPE(Game::CommandFlags)
Q_DECLARE_METATYPE(Game::Rank)
Q_DECLARE_METATYPE(Game::File)

#endif // NAMESPACE_H
