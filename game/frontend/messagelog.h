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

#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <QtCore>

namespace Game { namespace Frontend {

class MessageLog
    : public QAbstractItemModel
{
    Q_OBJECT

private:
    struct Message
    {
        QByteArray player_name;
        QByteArray data;
    };

    QVector<Message> m_log;

public:
    enum Roles {
        RolePlayerName = Qt::UserRole + 1,
        RoleMessage
    };

    explicit MessageLog(QObject *parent = 0);
    virtual ~MessageLog();

    virtual void append(const QByteArray &player_name,
                        const QByteArray &message);

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
    //! \reimp_end
};

}} // namespace Game, Frontend

#endif // MESSAGELOG_H
