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

#include "messagelog.h"

namespace Game { namespace Frontend {

MessageLog::MessageLog(QObject *parent)
    : QAbstractItemModel(parent)
    , m_log()
{
    // QML cannot cope with c-style-variable-names!
    QHash<int, QByteArray> roles;
    roles[RolePlayerName] = "playerName";
    roles[RoleMessage] = "message";
    setRoleNames(roles);
}

MessageLog::~MessageLog()
{}

void MessageLog::append(const QByteArray &player_name,
                        const QByteArray &message)
{
    if (message.isEmpty()) {
        // Nothing to append, ignore.
        return;
    }

    static Message empty;
    Message &prev_msg(m_log.isEmpty() ? empty
                                      : m_log[m_log.size() - 1]);

    // Check whether previous message was from same player:
    if (prev_msg.player_name == player_name) {
        prev_msg.data.append(' '); // Acts as separator.
        prev_msg.data.append(message);
        const QModelIndex &changed(index(m_log.size() - 1, 0));
        emit dataChanged(changed, changed);

        return;
    }

    beginInsertRows(QModelIndex(), m_log.size(), m_log.size());

    Message msg;
    msg.player_name = player_name;
    msg.data = message;
    m_log.append(msg);

    endInsertRows();

    // Manage a sliding window of no more than 1024 entries:
    while (m_log.size() > 1024) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_log.pop_front();
        endRemoveRows();
    }
}

void MessageLog::removeAll()
{
    if (m_log.isEmpty()) {
        return; // Nothing to do!
    }

    beginRemoveRows(QModelIndex(), 0, m_log.count() - 1);
    m_log.clear();
    endRemoveRows();
}

void MessageLog::remove(int row)
{
    if (not isValidIndex(row)) {
        return; // Nothing to do!
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_log.remove(row);
    endRemoveRows();
}

int MessageLog::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return 1;
}

int MessageLog::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_log.size();
}

QModelIndex MessageLog::index(int row,
                              int column,
                              const QModelIndex &index) const
{
    Q_UNUSED(index)
    return createIndex(row, column);
}

QModelIndex MessageLog::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

QVariant MessageLog::data(const QModelIndex &index,
                          int role) const
{
    if (m_log.size() < index.row()) {
        return QVariant();
    }

    const Message &m(m_log.at(index.row()));

    switch(role) {
    case RolePlayerName: return QVariant(m.player_name);
    case RoleMessage: return QVariant(m.data);

    default:
        return QVariant();
    }
}

bool MessageLog::isValidIndex(int index) const
{
    return (not m_log.isEmpty()
            && not (index < 0)
            && not (index > m_log.size() - 1));
}

}} // namespace Game, Frontend
