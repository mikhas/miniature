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

#ifndef TPACCOUNTITEM_H_
#define TPACCOUNTITEM_H_

#include <QtCore>
#include <QtGui>

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Connection>
#include <TelepathyQt4/PendingChannelRequest>

namespace Tp {
    class PendingOperation;
}

namespace TpGame
{

class AccountItem;
typedef QSharedPointer<AccountItem> SharedAccountItem;

class AccountItem
    : public QObject
{
    Q_OBJECT

public:
    explicit AccountItem(Tp::AccountManagerPtr am, const QString &path, QObject *parent = 0);
    virtual ~AccountItem();

    QString getDisplayName();
    void ensureContactsList();
    void initialize();
    bool isInitialized();
    void ensureChannel(const QVariantMap &req);

Q_SIGNALS:
    void initialized();
    void contactsForAccountChanged(const Tp::Contacts &contacts);

private Q_SLOTS:
    void onFinished(Tp::PendingOperation *pending);
    void onConnectionReady(Tp::PendingOperation *pending);
    void onConnectionStatusChanged(const Tp::ConnectionStatus &status, const Tp::ConnectionStatusReason &reason);
    void onEnsureChannelFinished(Tp::PendingOperation *pending);

protected:
    // TODO: just make the parts that are needed by the AM public. Dont use friends.
    friend class AccountManager;

private:
    void connectionReady();

    Tp::AccountPtr m_account;
    Tp::ConnectionPtr m_connection;
    Tp::PendingChannelRequest *m_pending_request;
    Tp::ChannelRequestPtr m_channel_request;
};

} // namespace TpGame

#endif // TPACCOUNTITEM_H_
