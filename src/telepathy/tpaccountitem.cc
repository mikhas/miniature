/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
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
#include <TelepathyQt4/ChannelRequest>
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

    if(mAcc->haveConnection())
        mAcc->disconnect();
}

bool TpAccountItem::isInitialized()
{
    return mAcc->isReady() && mAcc->isValid();
}

void TpAccountItem::initialize()
{
    qDebug() << "TpAccountItem::initialize()";

    Tp::Features features;
    features << Tp::Account::FeatureCore
            << Tp::Account::FeatureAvatar
            << Tp::Account::FeatureProtocolInfo;

    QObject::connect(mAcc->becomeReady(features), SIGNAL(finished(Tp::PendingOperation *)), SLOT(onReady(Tp::PendingOperation *)));
}

void TpAccountItem::onReady(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onReady()";

    if(o->isError())
    {
        qDebug() << "Account ready error.";
        return;
    }

    QObject::connect(mAcc.data(), SIGNAL(connectionStatusChanged(Tp::ConnectionStatus, Tp::ConnectionStatusReason)),
                     this, SLOT(onConnectionStatusChanged(Tp::ConnectionStatus, Tp::ConnectionStatusReason)));

    Q_EMIT initialized();
}

QString TpAccountItem::getDisplayName()
{
    if(mAcc->isReady() && mAcc->isValid())
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
        QObject::connect(mAcc->setEnabled(true), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onAccountSetEnabled(Tp::PendingOperation *)));
    }
}

void TpAccountItem::onAccountSetEnabled(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onAccountSetEnabled()";

    if(o->isError())
    {
        qDebug() << "Account set enabled failed.";
        return;
    }

    QObject::connect(mAcc->setConnectsAutomatically(true), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onConnectsAutomatically(Tp::PendingOperation *)));
}

void TpAccountItem::onConnectsAutomatically(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onConnectsAutomatically()";

    if(o->isError())
    {
        qDebug() << "Account connection automatically failed.";
        return;
    }
}

void TpAccountItem::onConnectionStatusChanged(Tp::ConnectionStatus cs, Tp::ConnectionStatusReason)
{
    qDebug() << "TpAccountItem::onConnectionStatusChanged()";

    if(Tp::ConnectionStatusConnected == cs && mAcc->haveConnection())
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

    Q_EMIT contactsForAccount(contactsList);
}

void TpAccountItem::ensureChannel(const QVariantMap &req)
{
    qDebug() << "TpAccountItem::ensureChannel()";

    qDebug() << req;

    QObject::connect(mAcc->ensureChannel(req), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onEnsureChannelFinished(Tp::PendingOperation *)));
}

void TpAccountItem::onEnsureChannelFinished(Tp::PendingOperation *o)
{
    qDebug() << "TpAccountItem::onEnsureChannelFinished()";

    if(o->isError())
    {
        qDebug() << "Ensure channel failed: " << o->errorName() << o->errorMessage();
        return;
    }
}

#include "tpaccountitem.moc.cc"
};
