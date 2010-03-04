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

#include "tpcontactslistmodel.h"

namespace Miniature
{

TpContactsListModel::TpContactsListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TpContactsListModel::rowCount(const QModelIndex &) const
{
    return mContacts.size();
}

QVariant TpContactsListModel::data(const QModelIndex & index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole)
    {
        return QVariant(mContacts[index.row()]->id());
    }
    else
    {
        return QVariant();
    }
}

void TpContactsListModel::setContacts(const Tp::Contacts c)
{
    mContacts = c.toList();
    reset();
}

};

