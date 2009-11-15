/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
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

#ifndef PIECES_POOL_H__
#define PIECES_POOL_H__

#include "position.h"
#include "pieces_pool.h"

#include <QGraphicsSvgItem>


namespace Miniature
{

/* Manages several pools for pieces, one for each piece type. */
// TODO: test cases: 1. Does the pool exhaust correctly, that is, does it store
//                      its objects correctly (and the correct amount)?
//                   2. Do take && releaseAll satisfy their invariants?
class MPiecesPoolManager
{
public:
    MPiecesPoolManager(int pieceSize = 54);
    ~MPiecesPoolManager();

    /** Foward method for MPiecesPool::take(), therefore "keeps" ownership of
      * returned pointer. If the returned pointer is 0 then the pool for that type
      * is exhausted.
      */
    QGraphicsSvgItem* take(MPosition::MPieceTypes type);

    /** Forward method for MPiecesPool::release()
      */
    void releaseAll();

private:
    void addToPool(MPosition::MPieceTypes type, int times = 1);
    QString getFileNameForPieceType(MPosition::MPieceTypes type) const;

    // used to scale our pieces to the correct size
    const int m_piece_size;

    typedef QMap<MPosition::MPieceTypes, MPiecesPool*> MPiecesMap;
    /* Stores each created QGraphicsSvgItem, using a piece's FEN character as key. */
    MPiecesMap m_pieces_cache;
};

};

#endif
