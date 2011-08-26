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

private:
    QString m_id;
    QColor m_color;

public:
    explicit SideElement(QObject *parent = 0);

    Q_INVOKABLE void setId(const QString &id);
    Q_INVOKABLE QString id() const;
    Q_SIGNAL void idChanged(const QString &id);

    Q_INVOKABLE void setColor(const QColor &color);
    Q_INVOKABLE QColor color() const;
    Q_SIGNAL void colorChanged(const QColor &color);
};

}} // namespace Game, Frontend

#endif // SIDEELEMENT_H
