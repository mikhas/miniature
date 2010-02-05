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
#include <TelepathyQt4/ContactManager>

#include <QString>
#include <QtGlobal>
#include <QtMaemo5/QtMaemo5>

namespace Miniature
{

TpAccountItem::TpAccountItem(Tp::AccountManagerPtr am, const QString &path, QObject *parent) : QObject(parent), 
            mAcc(Tp::Account::create(am->dbusConnection(), am->busName(), path))
{
    qDebug() << "TpAccountItem::TpAccountItem()";
}

TpAccountItem::~TpAccountItem()
{
    qDebug() << "TpAccountItem::~TpAccountItem()";
}

void TpAccountItem::initialize()
{
    qDebug() << "TpAccountItem::initialize()";

    QObject::connect(mAcc->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)), SLOT(onReady(Tp::PendingOperation *)));
}

void TpAccountItem::onReady(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onReady()";

    if(o->isError())
    {
        qDebug() << "Account ready error.";
        return;
    }

    Q_EMIT initialized();
}

QString TpAccountItem::getDisplayName()
{
    if(mAcc->isReady())
    {
        return mAcc->normalizedName();
    }

    return QString();
}

void TpAccountItem::ensureContactsList()
{
    qDebug() << "TpAccountItem::ensureContactsList()";

    if(mAcc->haveConnection())
    {
        connectionReady();
    }
    else
    {
//        Tp::SimplePresence simplePresence = mAcc->currentPresence();
//        simplePresence.type = Tp::ConnectionPresenceTypeAvailable;
//        QObject::connect(mAcc->setRequestedPresence(simplePresence), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onAccountConnected(Tp::PendingOperation *)));

        QObject::connect(mAcc->setEnabled(true), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onSetEnabledFinished(Tp::PendingOperation *)));
    }
}

void TpAccountItem::onSetEnabledFinished(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onSetEnabledFinished()";

    if(o->isError())
    {
        qDebug() << "Account set enabled error";
        return;
    }

    connectionReady();
}

void TpAccountItem::connectionReady()
{
    qDebug() << "TpAccountItem::connectionReady()";

    if(!mAcc->haveConnection())
    {
        qDebug() << "Error: Could not find active connection or account";
        return;
    }

    mConnection = mAcc->connection();

    Tp::Features features;
    features << Tp::Connection::FeatureCore
             << Tp::Connection::FeatureRoster
             << Tp::Connection::FeatureRosterGroups;

    QObject::connect(mConnection->becomeReady(features), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onConnectionReady(Tp::PendingOperation *)));
}

void TpAccountItem::onConnectionReady(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onConnectionReady()";

    if(o->isError())
    {
        qDebug() << "Connection ready failed";
        return;
    }

    if(mConnection->status() != Tp::Connection::StatusConnected)
    {
        qDebug() << "connection.status() is not Connected!";
        return;
    }

    Tp::ContactManager *contactManager = mConnection->contactManager();
    Tp::Contacts contactsList = contactManager->allKnownContacts();

    Q_FOREACH(Tp::ContactPtr contact, contactsList)
    {
        qDebug() << contact->id();
    }
}

};

#include "tpaccountitem.moc.cc"
