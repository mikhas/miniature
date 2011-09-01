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

#ifndef SEEK_H
#define SEEK_H

#include "abstractcommand.h"
#include "namespace.h"

#include <QtCore>

namespace Game { namespace Command {

class Seek
    : public AbstractCommand
{
private:
    uint m_time;
    uint m_increment;
    Rating m_rating;
    Color m_color;

public:
    //! \reimp
    explicit Seek(Target target,
                  uint time,
                  uint increment,
                  Rating rating,
                  Color color);
    virtual ~Seek();
    virtual void exec(Dispatcher *dispatcher,
                      AbstractEngine *target);
    //! \reimp_end
};

}} // namespace Command, Game

#endif // SEEK_H
