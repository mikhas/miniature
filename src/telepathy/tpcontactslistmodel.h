/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 *              Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
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

#ifndef TPCONTACTSLISTMODEL_H
#define TPCONTACTSLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Contact>

namespace TpGame
{

class TpContactsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TpContactsListModel(QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void setContacts(const Tp::Contacts);

private:
    QList<Tp::ContactPtr> mContacts;
};

} // namespace TpGame

#endif // TPCONTACTSLISTMODEL_H
