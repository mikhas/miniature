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

namespace TpGame
{

AccountItem::AccountItem(Tp::AccountManagerPtr am, const QString &path, QObject *parent)
    : QObject(parent),
      m_account(Tp::Account::create(am->dbusConnection(), am->busName(), path))
{
    qDebug() << "TpAccountItem::TpAccountItem()";
}

AccountItem::~AccountItem()
{
    qDebug() << "TpAccountItem::~TpAccountItem()";

    if (m_account->haveConnection())
        m_account->disconnect();
}

bool AccountItem::isInitialized()
{
    return m_account->isReady() && m_account->isValid();
}

void AccountItem::initialize()
{
    qDebug() << "TpAccountItem::initialize()";

    Tp::Features features;
    features << Tp::Account::FeatureCore
            << Tp::Account::FeatureAvatar
            << Tp::Account::FeatureProtocolInfo;

    connect(m_account->becomeReady(features), SIGNAL(finished(Tp::PendingOperation *)),
            this,                             SLOT(onFinished(Tp::PendingOperation *)),
            Qt::UniqueConnection);
}

void AccountItem::onFinished(Tp::PendingOperation *pending)
{
    qDebug() << "TpAccountItem::onReady()";

    if (pending->isError())
    {
        // TODO: should be a warning?
        qDebug() << "Account ready error.";
        return;
    }

    connect(m_account.data(), SIGNAL(connectionStatusChanged(Tp::ConnectionStatus,
                                                             Tp::ConnectionStatusReason)),
            this,             SLOT(onConnectionStatusChanged(Tp::ConnectionStatus,
                                                             Tp::ConnectionStatusReason)),
            Qt::UniqueConnection);

    Q_EMIT initialized();
}

QString AccountItem::getDisplayName()
{
    if (m_account->isReady() && m_account->isValid())
        return m_account->normalizedName();

    // TODO: really only return empty string?!
    return QString();
}

void AccountItem::ensureContactsList()
{
    qDebug() << "TpAccountItem::ensureContactsList()";

    if (m_account->haveConnection())
        connectionReady();
/* FIXME: Miniature should not enable telepathy accounts and should not start them
          For now, just disable this and let refactor this later
    else
        // TODO: A setter should not return an instance?
        connect(m_account->setEnabled(true), SIGNAL(finished(Tp::PendingOperation *)),
                this,                        SLOT(onAccountSetEnabled(Tp::PendingOperation *)),
                Qt::UniqueConnection);
*/
}

void AccountItem::onAccountSetEnabled(Tp::PendingOperation *pending)
{
    qDebug() << "TpAccountItem::onAccountSetEnabled()";

    if (pending->isError())
    {
        // TODO: should be a warning?
        qDebug() << "Account set enabled failed.";
        return;
    }

    // TODO: A setter should not return an instance?
    connect(m_account->setConnectsAutomatically(true), SIGNAL(finished(Tp::PendingOperation *)),
            this,                                      SLOT(onConnectsAutomatically(Tp::PendingOperation *)),
            Qt::UniqueConnection);
}

void AccountItem::onConnectsAutomatically(Tp::PendingOperation *pending)
{
    qDebug() << "TpAccountItem::onConnectsAutomatically()";

    if (pending->isError())
    {
        // TODO: should be a warning?
        qDebug() << "Account connection automatically failed.";
        return;
    }
}

void AccountItem::onConnectionStatusChanged(const Tp::ConnectionStatus &cs, const Tp::ConnectionStatusReason &)
{
    qDebug() << "TpAccountItem::onConnectionStatusChanged()";

    if (Tp::ConnectionStatusConnected == cs && m_account->haveConnection())
        connectionReady();
}

void AccountItem::connectionReady()
{
    qDebug() << "TpAccountItem::connectionReady()";

    if (!m_account->haveConnection())
    {
        // TODO: should be a warning?
        qDebug() << "Error: Could not find active connection or account";
        return;
    }

    m_connection = m_account->connection();

    Tp::Features features;
    features << Tp::Connection::FeatureCore
             << Tp::Connection::FeatureRoster
             << Tp::Connection::FeatureRosterGroups;

    // TODO: should not return an instance?
    connect(m_connection->becomeReady(features), SIGNAL(finished(Tp::PendingOperation *)),
            this,                                SLOT(onConnectionReady(Tp::PendingOperation *)),
            Qt::UniqueConnection);
}

void AccountItem::onConnectionReady(Tp::PendingOperation *pending)
{
    qDebug() << "TpAccountItem::onConnectionReady()";

    if (pending->isError())
    {
        // TODO: should be a warning?
        qDebug() << "Connection ready failed";
        return;
    }

    if (m_connection->status() != Tp::Connection::StatusConnected)
    {
        // TODO: should be a warning?
        qDebug() << "connection.status() is not Connected!";
        return;
    }

    Q_EMIT contactsForAccountChanged(m_connection->contactManager()->allKnownContacts());
}

void AccountItem::ensureChannel(const QVariantMap &req)
{
    qDebug() << "TpAccountItem::ensureChannel() - "
             << req;

    // TODO: should not return an instance?
    connect(m_account->ensureChannel(req), SIGNAL(finished(Tp::PendingOperation *)),
            this,                          SLOT(onEnsureChannelFinished(Tp::PendingOperation *)),
            Qt::UniqueConnection);
}

void AccountItem::onEnsureChannelFinished(Tp::PendingOperation *pending)
{
    qDebug() << "TpAccountItem::onEnsureChannelFinished()";

    if(pending->isError())
    {
        // TODO: should be a warning?
        qDebug() << "Ensure channel failed: " << pending->errorName() << pending->errorMessage();
        return;
    }
}

Tp::AccountPtr AccountItem::getInternal()
{
    return m_account;
}

} // namespace TpGame
