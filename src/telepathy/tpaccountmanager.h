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

#ifndef TPACCOUNTMANAGER_H_
#define TPACCOUNTMANAGER_H_

#include <QObject>
#include <QList>

#include "tpaccountitem.h"

#include <TelepathyQt4/Types>

namespace Tp {
    class PendingOperation;
}

namespace Miniature
{

class TpAccountManager : public QObject
{
    Q_OBJECT
public:
    TpAccountManager(QObject *parent = 0);
    ~TpAccountManager();

    void ensureAccountNameList();
    void ensureContactListForAccount(QString);

Q_SIGNALS:
    void onAccountNameListChanged(const QList<QString>);
    void onContactsForAccount(const Tp::Contacts);

private Q_SLOTS:
    void onAMReady(Tp::PendingOperation *);
    void onAccountInitialized();
    void onEnsureChannel(QString, QString);
    void ensureContactsForAccount(const Tp::Contacts);
    void createChannel(const Tp::ContactPtr);

private:
    Tp::AccountManagerPtr mAM;
    QList<TpAccountItemPtr> mAccounts;
    TpAccountItemPtr mAccount;
    QString mContactName;
};

};

#endif //TPACCOUNTMANAGER_H_

