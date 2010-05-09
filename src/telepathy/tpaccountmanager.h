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

#ifndef TPACCOUNTMANAGER_H_
#define TPACCOUNTMANAGER_H_

#include <QtCore>
#include <QtGui>

#include "tpaccountitem.h"

#include <TelepathyQt4/Types>

namespace Tp {
    class PendingOperation;
}

namespace TpGame
{

class AccountManager
    : public QObject
{
    Q_OBJECT

private:
    typedef QList<SharedAccountItem> AccountList;

public:
    explicit AccountManager(QObject *parent = 0);
    virtual ~AccountManager();

    void ensureAccountNameList();
    void ensureContactListForAccount(const QString &account_name);

Q_SIGNALS:
    void accountNamesChanged(const QStringList &account_names);
    void contactsForAccountChanged(const Tp::Contacts &contacts);

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *pending);
    void onAccountInitialized();
    void onEnsureChannel(const QString &account, const QString &name);
    void ensureContactsForAccount(const Tp::Contacts &contacts);
    void createChannel(const Tp::ContactPtr &contact);
    void createChatSession(const Tp::ContactPtr &contact);

private:
    Tp::AccountManagerPtr m_am;
    AccountList m_accounts;
    SharedAccountItem m_account;
    QString m_contact_name;
};

} // namespace TpGame

#endif //TPACCOUNTMANAGER_H_

