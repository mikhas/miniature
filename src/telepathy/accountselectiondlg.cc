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

#include "accountselectiondlg.h"
#include "tpaccountitem.h"
#include "tpaccountmanager.h"

#include <TelepathyQt4/Account>

#include <QtDebug>

namespace Miniature
{

AccountSelectionDlg::AccountSelectionDlg(TpAccountManager *accountManager, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), mAccMgr(accountManager), mAccountListModel(NULL), mContactsListModel(NULL)
{
    qDebug() << "AccountSelectionDlg::AccountSelectionDlg()";

    ui.setupUi(this);

    QObject::connect(mAccMgr, SIGNAL(onAccountNameListChanged(const QList<QString>)), this, SLOT(onAccountNameListChanged(const QList<QString>)));
    QObject::connect(this, SIGNAL(ensureChannel(QString,QString)), mAccMgr, SLOT(onEnsureChannel(QString,QString)));
}

AccountSelectionDlg::~AccountSelectionDlg()
{
    qDebug() << "AccountSelectionDlg::~AccountSelectionDlg()";
}

void AccountSelectionDlg::onAccountNameListChanged(const QList<QString> accountNameList)
{
    qDebug() << "AccountSelectionDlg::onAccountListChanged()";

    if(mAccountListModel == NULL)
    {
        mAccountListModel = new TpAccountListModel();
    }

    mAccountListModel->setAccounts(accountNameList);
    ui.listView->setModel(mAccountListModel);
}

void AccountSelectionDlg::showEvent(QShowEvent *event)
{
    qDebug() << "AccountSelectionDlg::showEvent()";

    ui.chooseButton->setEnabled(false);
    ui.chooseButton->setText(QObject::tr("Choose account"));
    QObject::connect(ui.chooseButton, SIGNAL(clicked(bool)), this, SLOT(chooseAccountClicked(bool)));
    QObject::connect(ui.listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(listItemClicked(const QModelIndex &)));

    mAccMgr->ensureAccountNameList();

    QDialog::showEvent(event);
}

void AccountSelectionDlg::listItemClicked(const QModelIndex &index)
{
    qDebug() << "AccountSelectionDlg::listItemClicked()";

    ui.chooseButton->setEnabled(true);
    lastItemIndex = index;
}

void AccountSelectionDlg::chooseAccountClicked(bool /*checked*/)
{
    qDebug() << "AccountSelectionDlg::chooseAccountClicked()";

    ui.chooseButton->disconnect();
    ui.chooseButton->setText(QObject::tr("Choose contact"));
    ui.chooseButton->setEnabled(false);
    QObject::connect(ui.chooseButton, SIGNAL(clicked(bool)), this, SLOT(chooseContactClicked(bool)));

    ui.listView->clearSelection();
    ui.listView->setDisabled(true);

    QObject::connect(mAccMgr, SIGNAL(onContactsForAccount(const Tp::Contacts)), this, SLOT(onContactsForAccount(const Tp::Contacts)));
    mAccountName = lastItemIndex.data().toString();
    mAccMgr->ensureContactListForAccount(mAccountName);
}

void AccountSelectionDlg::onContactsForAccount(const Tp::Contacts c)
{
    if(mContactsListModel == NULL)
    {
        mContactsListModel = new TpContactsListModel();
    }

    mContactsListModel->setContacts(c);
    ui.listView->setModel(mContactsListModel);
    ui.listView->setEnabled(true);
    ui.label->setText(QObject::tr("Select contact:"));
}

void AccountSelectionDlg::chooseContactClicked(bool /*checked*/)
{
    qDebug() << "AccountSelectionDlg::chooseContactClicked()";

    Q_EMIT ensureChannel(mAccountName, lastItemIndex.data().toString());
}

};
