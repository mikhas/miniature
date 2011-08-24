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

#include "side.h"
#include "position.h"

namespace Game {

class SidePrivate
{
public:
    const QString identifier;
    Position result;
    MovedPiece moved_piece;
    Promotion auto_promotion;

    explicit SidePrivate(const QString &new_identifier)
        : identifier(new_identifier)
        , result()
        , moved_piece()
        , auto_promotion(PromotionQueen)
    {}
};

Side::Side(const QString &identifier)
    : QObject()
    , d_ptr(new SidePrivate(identifier))
{}

Side::~Side()
{}

QString Side::identifier() const
{
    Q_D(const Side);
    return d->identifier;
}

void Side::startTurn(const Position &position)
{
    Q_UNUSED(position)
}

bool Side::move(const MovedPiece &moved_piece,
                Promotion manual_promotion)
{
    Q_UNUSED(moved_piece)
    Q_UNUSED(manual_promotion)

    return true; // FIXME: assuming that move is valid, for now
}

void Side::confirmMove()
{}

void Side::setAutoPromotion(Promotion promotion)
{
    Q_D(Side);
    d->auto_promotion = promotion;
}

Position Side::position() const
{
    return Position();
}

} // namespace Game

