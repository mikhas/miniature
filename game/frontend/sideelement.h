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

#ifndef SIDEELEMENT_H
#define SIDEELEMENT_H

#include <QtCore>
#include <QColor>

namespace Game { namespace Frontend {

//! An element representing a game side.
class SideElement
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SideElement)

    Q_PROPERTY(QString id READ id
                          WRITE setId
                          NOTIFY idChanged)

    Q_PROPERTY(QColor color READ color
                            WRITE setColor
                            NOTIFY colorChanged)

    Q_PROPERTY(bool active READ active
                           WRITE setActive
                           NOTIFY activeChanged)

    Q_PROPERTY(uint rating READ rating
                           WRITE setRating
                           NOTIFY ratingChanged)

    Q_PROPERTY(uint materialStrength READ materialStrength
                                     WRITE setMaterialStrength
                                     NOTIFY materialStrengthChanged)

    Q_PROPERTY(uint remainingTime READ remainingTime
                                  WRITE setRemainingTime
                                  NOTIFY remainingTimeChanged)

    Q_PROPERTY(uint remainingTimeClock READ remainingTimeClock
                                       NOTIFY remainingTimeClockChanged)

private:
    QString m_id;
    QColor m_color;
    bool m_active;
    uint m_rating;
    uint m_material_strength;
    uint m_remaining_time;
    uint m_remaining_time_clock;
    QTimer m_timer;

public:
    explicit SideElement(QObject *parent = 0);

    Q_INVOKABLE void setId(const QString &id);
    Q_INVOKABLE QString id() const;
    Q_SIGNAL void idChanged(const QString &id);

    Q_INVOKABLE void setColor(const QColor &color);
    Q_INVOKABLE QColor color() const;
    Q_SIGNAL void colorChanged(const QColor &color);

    Q_INVOKABLE void setActive(bool active);
    Q_INVOKABLE bool active() const;
    Q_SIGNAL void activeChanged(bool active);

    Q_INVOKABLE void setRating(uint rating);
    Q_INVOKABLE uint rating() const;
    Q_SIGNAL void ratingChanged(uint rating);

    Q_INVOKABLE void setMaterialStrength(uint material_strength);
    Q_INVOKABLE uint materialStrength() const;
    Q_SIGNAL void materialStrengthChanged(uint material_strength);

    Q_INVOKABLE void setRemainingTime(uint remaining_time);
    Q_INVOKABLE uint remainingTime() const;
    Q_SIGNAL void remainingTimeChanged(uint remaining_time);

    Q_INVOKABLE uint remainingTimeClock() const;
    Q_SIGNAL void remainingTimeClockChanged(uint remaining_time_clock);

private:
    Q_SLOT void adjustRemainingTimeClock();
};

}} // namespace Game, Frontend

#endif // SIDEELEMENT_H
