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

#ifndef PAWN_H__
#define PAWN_H__

#include "piece.h"

#include <QList>
#include <QPoint>
#include <QSvgRenderer>

namespace Miniature
{

class MPawn
: public MPiece
{
public:
    explicit MPawn(MColour colour);
    virtual ~MPawn();

    virtual QList<QPoint> getPossibleSquares(const MPosition &pos, const QPoint &origin) const;
    virtual QChar getLetter() const;

private:
    /* We have two renderers as class variables because we did not go the full
     * distance regarding the inheritance tree of our chess pieces: one piece class
     * is used for both colours. Also, we have to delay loading of SVGs from
     * the resource file until Qt has finished initializing it.
     */
    void loadFromSvgFile(int pieceSize = 60);
    static bool hasFinishedLoading;
    static QSvgRenderer blackRenderer;
    static QSvgRenderer whiteRenderer;
};

}

#endif /* PAWN_H__ */
