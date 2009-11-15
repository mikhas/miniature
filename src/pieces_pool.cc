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

#include "pieces_pool.h"

using namespace Miniature;

MPiecesPool::MPiecesPool()
: m_counter(0)
{}

MPiecesPool::~MPiecesPool()
{
    for(QGraphicsSvgItemArray::iterator iter = m_array.begin();
        iter != m_array.end();
        ++iter)
    {
        Q_CHECK_PTR(*iter);
        delete *iter;
        *iter = 0;
    }
}

void MPiecesPool::add(QGraphicsSvgItem *item)
{
    m_array << item;
}

QGraphicsSvgItem* MPiecesPool::take()
{
    if(!(m_counter < m_array.size()))
    {
        QGraphicsSvgItem* empty = 0;
        return empty;
    }

    return m_array.at(m_counter++);
}

void MPiecesPool::release()
{
    m_counter = 0;
}
