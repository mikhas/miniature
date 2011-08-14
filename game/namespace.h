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

// TODO: move this enum into commandline.h, for anything outside the CommandLine class, use the Dispatcher instead.
//! Available game commands, used for example by command line parser.
enum ParserCommand
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

Q_DECLARE_FLAGS(ParserCommandFlags, ParserCommand)

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

enum Side {
    SideActive,
    SideLocal,
    SideRemote
};

enum Mode {
    ModeNone = 0x0,
    ModeBlitz = 0x1,
    ModeLightening = 0x2,
    ModeUntimed = 0x4,
    ModeExamined = 0x8,
    ModeStandard = 0x10,
    ModeWild = 0x20,
    ModeAtomic = 0x40,
    ModeCrazyHouse = 0x80,
    ModeBugHouse = 0x100,
    ModeLosers = 0x200,
    ModeSuicide = 0x400
};

Q_DECLARE_FLAGS(ModeFlags, Mode)

} // namespace Game

Q_DECLARE_METATYPE(Game::ParserCommandFlags)
Q_DECLARE_METATYPE(Game::Rank)
Q_DECLARE_METATYPE(Game::File)
Q_DECLARE_METATYPE(Game::Side)
Q_DECLARE_METATYPE(Game::Mode)

#endif // NAMESPACE_H
