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

#include "square.h"

namespace Game {

Square::Square(File new_file,
               Rank new_rank)
    : file(new_file)
    , rank(new_rank)
{}

bool operator==(const Square &a,
                const Square &b)
{
    return (a.file == b.file && a.rank == b.rank);
}

Square toSquare(uint file,
                uint rank)
{
    File f = FileCount;
    Rank r = RankCount;

    if (file < FileCount) {
        f = static_cast<File>(file);
    }

    if (rank < RankCount) {
        r = static_cast<Rank>(rank);
    }

    return Square(f, r);
}

Square toSquare(int index)
{
    return toSquare(static_cast<uint>(index % FileCount),
                    static_cast<uint>(index / qMax<int>(RankCount, 1)));
}

Square toSquare(const QByteArray &data)
{
    if (data.size() != 2) {
        return Square(FileCount, RankCount);
    }

    File f = FileCount;
    Rank r = RankCount;

    switch(data.toLower().at(0)) {
    case 'a': f = FileA; break;
    case 'b': f = FileB; break;
    case 'c': f = FileC; break;
    case 'd': f = FileD; break;
    case 'e': f = FileE; break;
    case 'f': f = FileF; break;
    case 'g': f = FileG; break;
    case 'h': f = FileH; break;

    default: f = FileCount; break;
    }

    switch(data.toLower().at(1)) {
    case '1': r = Rank1; break;
    case '2': r = Rank2; break;
    case '3': r = Rank3; break;
    case '4': r = Rank4; break;
    case '5': r = Rank5; break;
    case '6': r = Rank6; break;
    case '7': r = Rank7; break;
    case '8': r = Rank8; break;

    default: r = RankCount; break;
    }

    return Square(f, r);
}

} // namespace Game
