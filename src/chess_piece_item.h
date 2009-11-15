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

#ifndef CHESS_PIECE_ITEM_H__
#define CHESS_PIECE_ITEM_H__

#include <QGraphicsSvgItem>
#include <QGraphicsSceneMouseEvent>


namespace Miniature
{

/* This class is currently useless. I thought I would need it for fingertouch
 * event handling ... maybe remove at some later point, when we are sure.
 */
class MGraphicsChessPieceItem
: public QGraphicsSvgItem
{
public:
    MGraphicsChessPieceItem(QGraphicsItem *parent = 0);
    MGraphicsChessPieceItem(const QString &fileName, QGraphicsItem *parent = 0);
    ~MGraphicsChessPieceItem();
};

};

#endif
