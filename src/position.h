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

#ifndef POSITION_H__
#define POSITION_H__

#include <QString>

namespace Miniature
{

class MPosition
{
public:
    MPosition(); // construct the start position
    explicit MPosition(QString fen);
    ~MPosition();

    QString convertToFen() const;

private:
    QString getDefaultStartPosition() const;
    // TODO: replace with a 8x8 grid
    QString m_position;
};

}; // namespace Miniature

#endif

