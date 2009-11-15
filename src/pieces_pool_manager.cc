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

#include "pieces_pool_manager.h"

using namespace Miniature;

MPiecesPoolManager::MPiecesPoolManager(int pieceSize)
: m_piece_size(pieceSize)
{
    // store max. amount of units ever needed in pool.
    // TODO: add to pool on demand.
    addToPool(MPosition::BROOK, 2 + 8);
    addToPool(MPosition::BKNIGHT, 2 + 8);
    addToPool(MPosition::BBISHOP, 2 + 8);
    addToPool(MPosition::BQUEEN, 1 + 8);
    addToPool(MPosition::BKING, 1);
    addToPool(MPosition::BPAWN, 8);

    addToPool(MPosition::WROOK, 2 + 8);
    addToPool(MPosition::WKNIGHT, 2 + 8);
    addToPool(MPosition::WBISHOP, 2 + 8);
    addToPool(MPosition::WQUEEN, 1 + 8);
    addToPool(MPosition::WKING, 1);
    addToPool(MPosition::WPAWN, 8);
}

MPiecesPoolManager::~MPiecesPoolManager()
{
    for(MPiecesMap::iterator iter = m_pieces_cache.begin();
        iter != m_pieces_cache.end();
        ++iter)
    {
        Q_CHECK_PTR(*iter);
        delete *iter;
        *iter = 0;
    }
}

MGraphicsChessPieceItem* MPiecesPoolManager::take(MPosition::MPieceTypes type)
{
    MPiecesPool* pool = m_pieces_cache[type];
    if (!pool)
    {
        return 0;
    }
    else
    {
        return pool->take();
    }
}

void MPiecesPoolManager::releaseAll()
{
    for(MPiecesMap::iterator iter = m_pieces_cache.begin();
        iter != m_pieces_cache.end();
        ++iter)
    {
        (*iter)->release();
    }
}

void MPiecesPoolManager::addToPool(MPosition::MPieceTypes type, int times)
{
    QString file_name = getFileNameForPieceType(type);
    if (file_name.isEmpty())
    {
        return; // nothing to do!
    }

    MPiecesPool *pool = new MPiecesPool;
    // cache takes ownership of pool
    m_pieces_cache[type] = pool;

    for (int idx = 0; idx < times; ++idx)
    {
        // pool takes ownership of svg item
        MGraphicsChessPieceItem *piece = 0;
        pool->add(piece = new MGraphicsChessPieceItem(file_name));

        // scale the svg piece
        QRectF extent = piece->boundingRect();
        qreal ratio = 1;
        if (0 < extent.width())
        {
            ratio = m_piece_size / extent.width();
        }
        piece->scale(ratio, ratio);
       //piece->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
       piece->setFlags(QGraphicsItem::ItemIsSelectable);
    }
}

QString MPiecesPoolManager::getFileNameForPieceType(MPosition::MPieceTypes type) const
{
    switch(type)
    {
        case MPosition::BROOK:   return QString(":pieces/black/rook.svg"); break;
        case MPosition::BKNIGHT: return QString(":pieces/black/knight.svg"); break;
        case MPosition::BBISHOP: return QString(":pieces/black/bishop.svg"); break;
        case MPosition::BQUEEN:  return QString(":pieces/black/queen.svg"); break;
        case MPosition::BKING:   return QString(":pieces/black/king.svg"); break;
        case MPosition::BPAWN:   return QString(":pieces/black/pawn.svg"); break;

        case MPosition::WROOK:   return QString(":pieces/white/rook.svg"); break;
        case MPosition::WKNIGHT: return QString(":pieces/white/knight.svg"); break;
        case MPosition::WBISHOP: return QString(":pieces/white/bishop.svg"); break;
        case MPosition::WQUEEN:  return QString(":pieces/white/queen.svg"); break;
        case MPosition::WKING:   return QString(":pieces/white/king.svg"); break;
        case MPosition::WPAWN:   return QString(":pieces/white/pawn.svg"); break;

        default: return QString(); break; // TODO: complain? I do not think it's an error at this level.
    }
}
