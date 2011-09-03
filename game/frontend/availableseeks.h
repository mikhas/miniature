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

#ifndef AVAILABLESEEKS_H
#define AVAILABLESEEKS_H

#include "abstractengine.h" // FIXME: Seek

#include <QtCore>

namespace Game { namespace Frontend {

QString fromColor(Color color);

class AvailableSeeks
    : public QAbstractItemModel
{
    Q_OBJECT

private:
    QVector<Seek> m_available_seeks;

public:
    enum Roles {
        RoleId = Qt::UserRole + 1,
        RoleMode,
        RolePlayerName,
        RoleRating,
        RoleTime,
        RoleIncrement,
        RoleIsRated,
        RoleColor,
        RoleIsAutoStarted,
        RoleUsesFormula,
        RoleRatingLowerLimit,
        RoleRatingUpperLimit,
        RoleHighlighted
    };

    explicit AvailableSeeks(QObject *parent = 0);
    virtual ~AvailableSeeks();

    virtual void append(const Seek& s);
    Q_INVOKABLE virtual void removeAll();
    Q_INVOKABLE virtual void remove(int row);

    //! \reimp
    virtual int columnCount(const QModelIndex &index = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &index = QModelIndex()) const;
    virtual QModelIndex index(int row,
                              int column,
                              const QModelIndex &index = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index,
                          int role) const;
    virtual bool setData(const QModelIndex &index,
                         const QVariant &value,
                         int role);
};

}} // namespace Game, Frontend

#endif // AVAILABLESEEKS_H
