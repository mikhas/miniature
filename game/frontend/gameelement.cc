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

#include "gameelement.h"

namespace Game { namespace Frontend {

GameElement::GameElement(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_time(0)
    , m_time_increment(0)
    , m_rating(0)
{}

void GameElement::setId(uint id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged(m_id);
    }
}

uint GameElement::id() const
{
    return m_id;
}

void GameElement::setTime(uint time)
{
    if (m_time != time) {
        m_time = time;
        emit timeChanged(m_time);
    }
}

uint GameElement::time() const
{
    return m_time;
}

void GameElement::setTimeIncrement(uint time_increment)
{
    if (m_time_increment != time_increment) {
        m_time_increment = time_increment;
        emit timeChanged(m_time_increment);
    }
}

uint GameElement::timeIncrement() const
{
    return m_time_increment;
}

void GameElement::setRating(uint rating)
{
    if (m_rating != rating) {
        m_rating = rating;
        emit ratingChanged(m_rating);
    }
}

uint GameElement::rating() const
{
    return m_rating;
}

}} // namespace GameGame, Frontend
