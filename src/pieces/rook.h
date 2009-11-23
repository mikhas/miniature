/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Dennis Stötzel <kore@meeQ.de>
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

#ifndef ROOK_H__
#define ROOK_H__

#include "piece.h"

#include <QList>
#include <QPoint>
#include <QSvgRenderer>

namespace Miniature
{

class MRook
: public MPiece
{
public:
    MRook(MColour colour, int width = 8, int height = 8);
    ~MRook();

    virtual QList<QPoint> getPossibleSquares(QPoint) const;

    void hasMoved();
    bool canCastle() const;

    virtual MGraphicsChessPieceItem* takeChessPieceItem(int pieceSize = 60) const;

private:
    bool castle;

    /* We have two renderers as class variables because we did not go the full
     * distance regarding the inheritance tree of our chess pieces: one piece class
     * is used for both colours. Also, we have to delay loading of SVGs from
     * the resource file until Qt has finished initializing it.
     */
    static bool hasFinishedLoading;
    static QSvgRenderer blackRenderer;
    static QSvgRenderer whiteRenderer;
};

}

#endif /* ROOK_H__ */
