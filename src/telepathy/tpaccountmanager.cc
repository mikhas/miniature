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

#include "tpaccountmanager.h"
#include "tpaccountitem.h"
#include "accountselectiondlg.h"
#include "tpchatsession.h"

#include <select_account.ui.h>

#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Types>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ReferencedHandles>

#include <QDebug>
#include <QString>
#include <QtGlobal>

#if defined(Q_WS_MAEMO_5)
#include <QtMaemo5/QtMaemo5>
#endif

namespace TpGame
{

AccountManager::AccountManager(QObject *parent)
    : QObject(parent)
{
    qDebug() << "TpAccountManager::TpAccountManager()";

    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    Tp::registerTypes();

    m_am = Tp::AccountManager::create();

    connect(m_am->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)),
            this,                SLOT(onAccountManagerReady(Tp::PendingOperation *)));
}

AccountManager::~AccountManager()
{
    qDebug() << "TpAccountManager::~TpAccountManager()";
}

void AccountManager::onAccountManagerReady(Tp::PendingOperation *pending)
{
    qDebug() << "TpAccountManager::onAMReady()";

    if(pending->isError())
    {
        // TODO: Shouldn't this be a warning?
        qDebug() << "Account Manager error";
        return;
    }

    m_accounts.clear();

    Q_FOREACH(const QString &path, m_am->allAccountPaths())
    {
        SharedAccountItem account = SharedAccountItem(new AccountItem(m_am, path, this));
        connect(account.data(), SIGNAL(initialized()),
                this,           SLOT(onAccountInitialized()));
        account->initialize();
        m_accounts.push_back(account);
    }
}

void AccountManager::onAccountInitialized()
{
    qDebug() << "TpAccountManager::onAccountInitialized ()";

    QStringList account_names;

    Q_FOREACH(SharedAccountItem accItem, m_accounts)
    {
        if(accItem->isInitialized())
        {
            account_names.push_back(accItem->getDisplayName());
        }
    }

    Q_EMIT accountNamesChanged(account_names);
}

void AccountManager::ensureAccountNameList()
{
    qDebug() << "TpAccountManager::ensureAccountNameList ()";
    onAccountInitialized();
}

void AccountManager::ensureContactListForAccount(const QString &account_name)
{
    qDebug() << "TpAccountManager::ensureContactListForAccount ()" << account_name;

    Q_FOREACH(SharedAccountItem iter, m_accounts)
    {
        if (account_name == iter->getDisplayName())
        {
            connect(iter.data(), SIGNAL(contactsForAccountChanged(Tp::Contacts)),
                    this,        SIGNAL(contactsForAccountChanged(Tp::Contacts)),
                    Qt::UniqueConnection);

            iter->ensureContactsList();
            break;
        }
    }
}

void AccountManager::onEnsureChannel(const QString &account_name, const QString &contact_name)
{
    qDebug() << "TpAccountManager::onEnsureChannel ()" << account_name << contact_name;

    m_contact_name = contact_name;

    Q_FOREACH(SharedAccountItem iter, m_accounts)
    {
        if (account_name == iter->getDisplayName())
        {
            iter.data()->disconnect(SIGNAL(contactsForAccount(Tp::Contacts)));
            connect(iter.data(), SIGNAL(contactsForAccountChanged(Tp::Contacts)),
                    this,        SLOT(ensureContactsForAccount(Tp::Contacts)),
                    Qt::UniqueConnection);

            iter->ensureContactsList();
            m_account = iter;
            break;
        }
    }
}

void AccountManager::ensureContactsForAccount(const Tp::Contacts &contacts)
{
    qDebug() << "TpAccountManager::ensureContactsForAccount()" << contacts;

    Q_FOREACH(Tp::ContactPtr contact, contacts)
    {
        if(contact->id() == m_contact_name)
        {
            createChannel(contact);
            // TODO hook up properly
#ifdef ENABLE_CHAT_SESSION
            createChatSession(contact);
#endif
            break;
        }
    }
}

void AccountManager::createChannel(const Tp::ContactPtr &contact)
{
    qDebug() << "TpAccountManager::createChannel()";
    qDebug() << "!!!! CREATING CHANNEL for" << contact->id() << " !!!";

    QVariantMap req;

    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
               TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE);
    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
               Tp::HandleTypeContact);
    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandle"),
               contact->handle().at(0));
    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE ".Service"),
               "Miniature");

    m_account->ensureChannel(req);
}

void AccountManager::createChatSession(const Tp::ContactPtr &contact)
{
    qDebug() << "TpAccountManager::createChatSession()";
    qDebug() << "!!!! CREATING TEXT SESSION for" << contact->id() << " !!!";

    QVariantMap req;

    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
               TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT);
    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
               Tp::HandleTypeContact);
    req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandle"),
               contact->handle().at(0));

    m_account->ensureChannel(req);
}

} // namespace TpGame
