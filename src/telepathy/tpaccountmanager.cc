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

namespace Miniature
{

TpAccountManager::TpAccountManager(QObject *parent)
    : QObject(parent)
{
    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    Tp::registerTypes();

    mAM = Tp::AccountManager::create();

    QObject::connect(mAM->becomeReady(),
           SIGNAL(finished(Tp::PendingOperation *)),
           SLOT(onAMReady(Tp::PendingOperation *)));
}

TpAccountManager::~TpAccountManager()
{

}

void TpAccountManager::onAMReady(Tp::PendingOperation *o)
{
    if(o->isError())
    {
        qDebug() << "Account Manager error";
        return;
    }

    mAccounts.clear();

    Q_FOREACH(const QString &path, mAM->allAccountPaths())
    {
        TpAccountItemPtr account = TpAccountItemPtr(new TpAccountItem(mAM, path, this));
        QObject::connect(account.data(), SIGNAL(initialized()), this, SLOT(onAccountInitialized()));
        account->initialize();
        mAccounts.push_back(account);
    }
}

void TpAccountManager::onAccountInitialized()
{
    QList<QString> accountNamesList;

    Q_FOREACH(TpAccountItemPtr accItem, mAccounts)
    {
        if(accItem->isInitialized())
        {
            accountNamesList.push_back(accItem->getDisplayName());
        }
    }

    Q_EMIT onAccountNameListChanged(accountNamesList);
}

void TpAccountManager::ensureAccountNameList()
{
    onAccountInitialized();
}

void TpAccountManager::ensureContactListForAccount(QString accountName)
{
    Q_FOREACH(TpAccountItemPtr accItem, mAccounts)
    {
        if(accountName == accItem->getDisplayName())
        {
            accItem.data()->disconnect(SIGNAL(contactsForAccount(Tp::Contacts)));
            QObject::connect(accItem.data(), SIGNAL(contactsForAccount(const Tp::Contacts)), this, SIGNAL(onContactsForAccount(const Tp::Contacts)));
            accItem->ensureContactsList();
            break;
        }
    }
}

void TpAccountManager::onEnsureChannel(QString accountName, QString contactName)
{
    mContactName = contactName;

    Q_FOREACH(TpAccountItemPtr accItem, mAccounts)
    {
        if(accountName == accItem->getDisplayName())
        {
            accItem.data()->disconnect(SIGNAL(contactsForAccount(Tp::Contacts)));
            QObject::connect(accItem.data(), SIGNAL(contactsForAccount(const Tp::Contacts)), this, SLOT(ensureContactsForAccount(const Tp::Contacts)));
            accItem->ensureContactsList();
            mAccount = accItem;
            break;
        }
    }
}

void TpAccountManager::ensureContactsForAccount(const Tp::Contacts contacts)
{
    qDebug();

    Q_FOREACH(Tp::ContactPtr contact, contacts)
    {
        if(contact->id() == mContactName)
        {
            createChannel(contact);
            break;
        }
    }
}

void TpAccountManager::createChannel(const Tp::ContactPtr contact)
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

    mAccount->ensureChannel(req);
}

};
