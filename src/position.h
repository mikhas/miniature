/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef POSITION_H__
#define POSITION_H__

#include <QString>

namespace Miniature
{

class MPosition
{
public:
    MPosition(); // construct the start position
    explicit MPosition(QString fen);
    ~MPosition();

    enum MPieceTypes {BROOK, BKNIGHT, BBISHOP, BQUEEN, BKING, BPAWN,
                      WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WPAWN,
                      UNKNOWN_PIECE};

    QString convertToFen() const;
    MPieceTypes lookupPieceType(QChar fenPiece) const;

private:
    QString getDefaultStartPosition() const;
    // TODO: replace with a 8x8 grid
    QString m_position;
};

}; // namespace Miniature

#endif

