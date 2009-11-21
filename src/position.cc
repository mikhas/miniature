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

#include "position.h"


using namespace Miniature;

MPosition::MPosition()
: m_position(getDefaultStartPosition())
{
    m_position_list = convertFenToList(getDefaultStartPosition());//std::cout << "MPosition -> empty constructor" << std::endl;
}

MPosition::MPosition(QString fen)
: m_position(fen)
{
    m_position_list = convertFenToList(fen);
}

MPosition::~MPosition()
{}

void onPieceMoved(QPoint from, QPoint to)
{
    if (from && to)
    {
        if(verifyMove(from, to))
        {
            // update grid
            movePiece(from, to);
        }
        // TODO?: Emit some sort of warning, when move wasn't verified?
    }

    Q_EMIT confirmedPosition(MPosition::m_position_list);
}

void MPosition::movePiece(QPoint from, QPoint to)
{
    // TODO: Castling & pawn promotion
    MPosition::m_position_list[to.x()][to.y()] = MPosition::m_position_list[from.x()][from.y()];
    Position::m_position_list[from.x()][from.y()] = 0;
}

bool MPosition::verifyMove(QPoint from, QPoint to) const
{
    MPiece::MPieceTypes currentType = (MPosition::m_position_list[from.x()][from.y()])->getType();

    if (currentType == MPiece::PAWN) {return verifyMovePawn(from, to); }
    if (currentType == MPiece::ROOK) {return verifyMoveRook(from, to); }
    if (currentType == MPiece::KNIGHT) {return verifyMoveKnight(from, to); }
    if (currentType == MPiece::BISHOP) {return verifyMoveBishop(from, to); }
    if (currentType == MPiece::QUEEN) {return verifyMoveQueen(from, to); }
    if (currentType == MPiece::KING) {return verifyMoveKing(from, to); }
}

bool MPosition::verifyMovePawn(QPoint from, QPoint to) const
{
    return true;
}

bool MPosition::verifyMoveRook(QPoint from, QPoint to) const
{
    return true;
}

bool MPosition::verifyMoveKnight(QPoint from, QPoint to) const
{
    return true;
}

bool MPosition::verifyMoveBishop(QPoint from, QPoint to) const
{
    return true;
}

bool MPosition::verifyMoveQueen(QPoint from, QPoint to) const
{
    return true;
}

bool MPosition::verifyMoveKing(QPoint from, QPoint to) const
{
    return true;
}

QString MPosition::getDefaultStartPosition() const
{
    return QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

QString MPosition::convertToFen() const
{
    return m_position;
}

MPosition::MPiecesGrid MPosition::convertFenToList(QString fen) const
{
    MPosition::MPiecesGrid retPosition = new MPosition::MPiecesGrid;

    int x_pos = 0;
    int y_pos = 0;
    int count_cells = 0;

    for(int idx = 0; idx < fen.length(); ++idx)
    {
        QChar curr = fen.at(idx);
        if (count_cells < 64)
        {
            if (curr == '/')
            {
                x_pos = 0;
                ++y_pos;
            }
            else if (curr.isDigit())
            {
                for (int j = 0; j < curr; ++j)
                {
                    ++x_pos;
                    retPosition[x_pos][y_pos] = 0;
                    ++count_cells;
                }
            }
            else
            {
                retPosition[x_pos][y_pos] = new MPiece(curr);

                ++x_pos;
                ++count_cells;
            }
        }
        else // TODO Set player-to-move, castle options, etc.
        {}
    }

    return retPosition;
}

MPosition::MPiecesGrid MPosition::getPositionList() const
{
    return m_position_list;
}

MPosition::MPieceTypes MPosition::lookupPieceType(QChar fenPiece) const
{
    if ('r' == fenPiece) return BROOK;
    if ('n' == fenPiece) return BKNIGHT;
    if ('b' == fenPiece) return BBISHOP;
    if ('q' == fenPiece) return BQUEEN;
    if ('k' == fenPiece) return BKING;
    if ('p' == fenPiece) return BPAWN;

    if ('R' == fenPiece) return WROOK;
    if ('N' == fenPiece) return WKNIGHT;
    if ('B' == fenPiece) return WBISHOP;
    if ('Q' == fenPiece) return WQUEEN;
    if ('K' == fenPiece) return WKING;
    if ('P' == fenPiece) return WPAWN;

    // Complain?
    return UNKNOWN_PIECE;
}



