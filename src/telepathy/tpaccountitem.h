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

#ifndef TPACCOUNTITEM_H_
#define TPACCOUNTITEM_H_

#include <QObject>
#include <QSharedPointer>

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Connection>

namespace Tp {
    class PendingOperation;
}

namespace Miniature
{

class TpAccountItem : public QObject
{
    Q_OBJECT
public:
    TpAccountItem(Tp::AccountManagerPtr am, const QString &path, QObject *parent = 0);
    ~TpAccountItem();

    QString getDisplayName();
    void ensureContactsList();
    void initialize();
    bool isInitialized();
    void ensureChannel(const QVariantMap req);

Q_SIGNALS:
    void initialized();
    void contactsForAccount(const Tp::Contacts);

private Q_SLOTS:
    void onReady(Tp::PendingOperation *);
    void onConnectionReady(Tp::PendingOperation *);
    void onAccountSetEnabled(Tp::PendingOperation *);
    void onConnectsAutomatically(Tp::PendingOperation *);
    void onConnectionStatusChanged(Tp::ConnectionStatus, Tp::ConnectionStatusReason);
    void onEnsureChannel(Tp::PendingOperation *);

private:
    void connectionReady();

    Tp::AccountPtr mAcc;
    Tp::ConnectionPtr mConnection;
};

typedef QSharedPointer<TpAccountItem> TpAccountItemPtr;

};

#endif // TPACCOUNTITEM_H_
