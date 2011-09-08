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

#ifndef GAMEELEMENT_H
#define GAMEELEMENT_H

#include <QtCore>

namespace Game { namespace Frontend {

//! An element representing a game.
class GameElement
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GameElement)

    Q_PROPERTY(uint id READ id
                       WRITE setId
                       NOTIFY idChanged)

    Q_PROPERTY(uint time READ time
                                  WRITE setTime
                                  NOTIFY timeChanged)

    Q_PROPERTY(uint timeIncrement READ timeIncrement
                                  WRITE setTimeIncrement
                                  NOTIFY timeIncrementChanged)

private:
    uint m_id;
    uint m_time;
    uint m_time_increment;

public:
    explicit GameElement(QObject *parent = 0);

    Q_INVOKABLE void setId(uint id);
    Q_INVOKABLE uint id() const;
    Q_SIGNAL void idChanged(uint id);

    Q_INVOKABLE void setTime(uint time);
    Q_INVOKABLE uint time() const;
    Q_SIGNAL void timeChanged(uint time);

    Q_INVOKABLE void setTimeIncrement(uint time_increment);
    Q_INVOKABLE uint timeIncrement() const;
    Q_SIGNAL void timeIncrementChanged(uint time_increment);
};

}} // namespace Game, Frontend

#endif // GAMEELEMENT_H
