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

//! Enumeration of rows - called 'ranks' - on a chess board. Range: [1-8].
enum Rank
{
    Rank1 = 0,
    Rank2,
    Rank3,
    Rank4,
    Rank5,
    Rank6,
    Rank7,
    Rank8,
    RankCount
};

//! Enumeration of columns - called 'files' - on a chess board. Range: [A-H].
enum File
{
    FileA = 0,
    FileB,
    FileC,
    FileD,
    FileE,
    FileF,
    FileG,
    FileH,
    FileCount
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

enum Promotion {
    PromotionNone = 0,
    PromotionKnight,
    PromotionBishop,
    PromotionRook,
    PromotionQueen
};

enum Color {
    ColorAuto = 0,
    ColorWhite,
    ColorBlack
};

} // namespace Game

Q_DECLARE_METATYPE(Game::Rank)
Q_DECLARE_METATYPE(Game::File)
Q_DECLARE_METATYPE(Game::Mode)
Q_DECLARE_METATYPE(Game::Promotion)
Q_DECLARE_METATYPE(Game::Color)

#endif // NAMESPACE_H
