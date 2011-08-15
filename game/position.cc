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

#include "position.h"

namespace {
    QString fromSquare(const Game::Square &square)
    {
        QString result;

        switch(square.file) {
        case Game::FileA:
            result.append("a"); break;
        case Game::FileB:
            result.append("b"); break;
        case Game::FileC:
            result.append("c"); break;
        case Game::FileD:
            result.append("d"); break;
        case Game::FileE:
            result.append("e"); break;
        case Game::FileF:
            result.append("f"); break;
        case Game::FileG:
            result.append("g"); break;
        case Game::FileH:
            result.append("h"); break;

        default:
            break;
        }

        switch(square.rank) {
        case Game::Rank1:
            result.append("1"); break;
        case Game::Rank2:
            result.append("2"); break;
        case Game::Rank3:
            result.append("3"); break;
        case Game::Rank4:
            result.append("4"); break;
        case Game::Rank5:
            result.append("5"); break;
        case Game::Rank6:
            result.append("6"); break;
        case Game::Rank7:
            result.append("7"); break;
        case Game::Rank8:
            result.append("8"); break;

        default:
            break;
        }

        return result;
    }
}

namespace Game {

Position::Position()
{}

QString moveNotation(const Position &result,
                     const MovedPiece &moved_piece)
{
    Q_UNUSED(result)
    // TODO: This is gnuchess-style notation, allow for different types?
    return QString("%1%2").arg(fromSquare(moved_piece.first))
                          .arg(fromSquare(moved_piece.second));
}

} // namespace Game
