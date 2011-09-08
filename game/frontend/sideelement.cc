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

#include "sideelement.h"

namespace Game { namespace Frontend {

SideElement::SideElement(QObject *parent)
    : QObject(parent)
    , m_id()
    , m_color()
    , m_active(false)
    , m_rating(0)
    , m_material_strength(0)
    , m_remaining_time(0)
{}

void SideElement::setId(const QString &id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged(m_id);
    }
}

QString SideElement::id() const
{
    return m_id;
}

void SideElement::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged(m_color);
    }
}

QColor SideElement::color() const
{
    return m_color;
}

void SideElement::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        emit activeChanged(m_active);
    }
}

bool SideElement::active() const
{
    return m_active;
}

void SideElement::setRating(uint rating)
{
    if (m_rating != rating) {
        m_rating = rating;
        emit ratingChanged(m_rating);
    }
}

uint SideElement::rating() const
{
    return m_rating;
}

void SideElement::setMaterialStrength(uint material_strength)
{
    if (m_material_strength != material_strength) {
        m_material_strength = material_strength;
        emit materialStrengthChanged(m_material_strength);
    }
}

uint SideElement::materialStrength() const
{
    return m_material_strength;
}

void SideElement::setRemainingTime(uint remaining_time)
{
    if (m_remaining_time != remaining_time) {
        m_remaining_time = remaining_time;
        emit remainingTimeChanged(m_remaining_time);
    }
}

uint SideElement::remainingTime() const
{
    return m_remaining_time;
}

}} // namespace GameGame, Frontend
