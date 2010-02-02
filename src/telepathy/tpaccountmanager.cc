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

#include <QDebug>
#include <QString>
#include <QtGlobal>
#include <QtMaemo5/QtMaemo5>

namespace Miniature
{

TpAccountManager::TpAccountManager(QObject *parent)
    : QObject(parent)
{
    Tp::registerTypes();
    Tp::enableDebug(true);
    Tp::enableWarnings(true);
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

    m_Accounts.clear();

    Q_FOREACH(const QString &path, m_AM->allAccountPaths())
    {
        TpAccountItemPtr account = TpAccountItemPtr(new TpAccountItem(m_AM, path, this));
        m_Accounts.push_back(account);
    }

    m_select_account.exec();
}

void TpAccountManager::chooseAccount()
{
     m_AM = Tp::AccountManager::create();

     QObject::connect(m_AM->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onAMReady(Tp::PendingOperation *)));
}

};
