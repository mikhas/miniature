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

#ifndef PIECES_POOL_ITEM_H__
#define PIECES_POOL_ITEM_H__

#include "chess_piece_item.h"

#include <QList>
#include <QGraphicsSvgItem>

namespace Miniature
{

class MPiecesPool
{
public:
     MPiecesPool();
     ~MPiecesPool();

     /* Adds item ptr to array and takes ownership of item! */
     void add(MGraphicsChessPieceItem* item);

     /* Hands out a ptr to the current array item, but does not transfer ownership. */
     // TODO: find better name, to indicate the no-ownership-transfer thing.
     MGraphicsChessPieceItem* take();

     /* Resets the internal counter, the next take() will get the first item of
      * the array again. */
     void release();

private:
     int m_counter;
     typedef QList<MGraphicsChessPieceItem*> MGraphicsChessPieceItemArray;
     MGraphicsChessPieceItemArray m_array;
};

};

#endif
