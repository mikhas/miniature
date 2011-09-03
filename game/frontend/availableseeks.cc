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

#include "availableseeks.h"

namespace Game { namespace Frontend {

QString fromColor(Color color)
{
    switch(color) {
    case ColorNone: return "auto";
    case ColorWhite: return "white";
    case ColorBlack: return "black";
    }

    return "";
}

AvailableSeeks::AvailableSeeks(QObject *parent)
    : QAbstractItemModel(parent)
{
    // QML cannot cope with c-style-variable-names!
    QHash<int, QByteArray> roles;
    roles[RoleId] = "id";
    roles[RoleMode] = "mode";
    roles[RolePlayerName] = "playerName";
    roles[RoleRating] = "rating";
    roles[RoleTime] = "time";
    roles[RoleIncrement] = "increment";
    roles[RoleIsRated] = "isRated";
    roles[RoleColor] = "color";
    roles[RoleIsAutoStarted] = "isAutoStarted";
    roles[RoleUsesFormula] = "usesFormula";
    roles[RoleRatingLowerLimit] = "ratingLowerLimit";
    roles[RoleRatingUpperLimit] = "ratingUpperLimit";
    roles[RoleHighlighted] = "highlighted";
    setRoleNames(roles);
}

AvailableSeeks::~AvailableSeeks()
{}

void AvailableSeeks::append(const Seek& s)
{
    beginInsertRows(QModelIndex(), m_available_seeks.size(), m_available_seeks.size());
    m_available_seeks.append(s);
    endInsertRows();

    // Manage a sliding window of no more than 1024 entries:
    // TODO: delete ads after x secs?
    while (m_available_seeks.size() > 1024) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_available_seeks.pop_front();
        endRemoveRows();
    }
}

void AvailableSeeks::removeAll()
{
    beginRemoveRows(QModelIndex(), 0, m_available_seeks.count() - 1);
    m_available_seeks.clear();
    endRemoveRows();
}

void AvailableSeeks::remove(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_available_seeks.remove(row);
    endRemoveRows();
}

int AvailableSeeks::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return 1;
}

int AvailableSeeks::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_available_seeks.size();
}

QModelIndex AvailableSeeks::index(int row,
                                  int column,
                                  const QModelIndex &index) const
{
    Q_UNUSED(index)
    return createIndex(row, column);
}

QModelIndex AvailableSeeks::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

QVariant AvailableSeeks::data(const QModelIndex &index,
                            int role) const
{
    if (m_available_seeks.size() < index.row()) {
        return QVariant();
    }

    const Seek &s(m_available_seeks.at(index.row()));

    switch(role) {
    case RoleId: return QVariant(s.id);
    case RoleMode: return QVariant(s.mode);
    case RolePlayerName: return QVariant(s.player_name);
    case RoleRating: return QVariant(s.rating);
    case RoleTime: return QVariant(s.time);
    case RoleIncrement: return QVariant(s.increment);
    case RoleIsRated: return QVariant(s.is_rated);
    case RoleColor: return QVariant(fromColor(s.color));
    case RoleIsAutoStarted: return QVariant(s.is_auto_started);
    case RoleUsesFormula: return QVariant(s.uses_formula);
    case RoleRatingLowerLimit: return QVariant(s.rating_range.first);
    case RoleRatingUpperLimit: return QVariant(s.rating_range.second);
    case RoleHighlighted: return QVariant(s.highlighted);

    default:
        return QVariant();
    }
}

bool AvailableSeeks::setData(const QModelIndex &index,
                             const QVariant &value,
                             int role)
{
    if (m_available_seeks.size() < index.row()) {
        return false;
    }

    Seek &s(m_available_seeks[index.row()]);


    switch(role) {
    case RoleHighlighted: s.highlighted = value.toBool();
        emit dataChanged(index, index);
        return true;

        // Other data is "read-only":
    default:
        break;
    }

    return false;
}

}} // namespace Game, Frontend
