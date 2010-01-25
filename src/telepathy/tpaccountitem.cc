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

#include "tpaccountitem.h"

#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Types>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

#include <QDebug>
#include <QString>
#include <QtGlobal>
#include <QtMaemo5/QtMaemo5>

namespace Miniature
{

TpAccountItem::TpAccountItem(Tp::AccountManagerPtr am, const QString &path, QObject *parent) : QObject(parent), 
            mAcc(Tp::Account::create(am->dbusConnection(), am->busName(), path))
{
    QObject::connect(mAcc->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)), SLOT(onReady(Tp::PendingOperation *)));
}

TpAccountItem::~TpAccountItem()
{
}

};

#include "tpaccountitem.moc.cc"
