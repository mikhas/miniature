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

#include "accountselectiondlg.h"
#include "tpaccountitem.h"
#include "tpaccountmanager.h"

#include <TelepathyQt4/Account>

#include <QtDebug>

namespace TpGame
{

AccountSelectionDlg::
AccountSelectionDlg(AccountManager *account_manager, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      m_account_manager(account_manager),
      m_account(0),
      m_contacts(0)
{
    qDebug() << "AccountSelectionDlg::AccountSelectionDlg()";

    ui.setupUi(this);

    connect(m_account_manager, SIGNAL(accountNamesChanged(QStringList)),
            this,              SLOT(onAccountNamesChanged(QStringList)));
    connect(this,              SIGNAL(ensureChannel(QString, QString)),
            m_account_manager, SLOT(onEnsureChannel(QString, QString)));
}

AccountSelectionDlg::~AccountSelectionDlg()
{
    qDebug() << "AccountSelectionDlg::~AccountSelectionDlg()";
}

void AccountSelectionDlg::onAccountNamesChanged(const QStringList &account_names)
{
    qDebug() << "AccountSelectionDlg::onAccountListChanged()";

    if(m_account == NULL)
    {
        m_account = new TpAccountListModel();
    }

    m_account->setAccounts(account_names);
    ui.listView->setModel(m_account);
}

void AccountSelectionDlg::showEvent(QShowEvent *event)
{
    qDebug() << "AccountSelectionDlg::showEvent()";

    ui.chooseButton->setEnabled(false);
    ui.chooseButton->setText(tr("Choose account"));
    connect(ui.chooseButton, SIGNAL(clicked(bool)),
            this,            SLOT(chooseAccountClicked(bool)),
            Qt::UniqueConnection);

    connect(ui.listView, SIGNAL(clicked(const QModelIndex &)),
            this,        SLOT(listItemClicked(const QModelIndex &)),
            Qt::UniqueConnection);

    m_account_manager->ensureAccountNameList();

    QDialog::showEvent(event);
}

void AccountSelectionDlg::listItemClicked(const QModelIndex &index)
{
    qDebug() << "AccountSelectionDlg::listItemClicked()";

    ui.chooseButton->setEnabled(true);
    m_last_index = index;
}

void AccountSelectionDlg::chooseAccountClicked(bool /*checked*/)
{
    qDebug() << "AccountSelectionDlg::chooseAccountClicked()";

    ui.chooseButton->disconnect();
    ui.chooseButton->setText(tr("Choose contact"));
    ui.chooseButton->setEnabled(false);

    connect(ui.chooseButton, SIGNAL(clicked(bool)),
            this,            SLOT(chooseContactClicked(bool)),
            Qt::UniqueConnection);

    ui.listView->clearSelection();
    ui.listView->setDisabled(true);

    connect(m_account_manager, SIGNAL(contactsForAccountChanged(Tp::Contacts)),
            this,              SLOT(onContactsForAccountChanged(Tp::Contacts)),
            Qt::UniqueConnection);

    m_account_name = m_last_index.data().toString();
    m_account_manager->ensureContactListForAccount(m_account_name);
}

void AccountSelectionDlg::onContactsForAccountChanged(const Tp::Contacts &c)
{
    if(!m_contacts)
    {
        m_contacts = new TpContactsListModel();
    }

    m_contacts->setContacts(c);
    ui.listView->setModel(m_contacts);
    ui.listView->setEnabled(true);
    ui.label->setText(tr("Select contact:"));
}

void AccountSelectionDlg::chooseContactClicked(bool /*checked*/)
{
    qDebug() << "AccountSelectionDlg::chooseContactClicked()";

    Q_EMIT ensureChannel(m_account_name, m_last_index.data().toString());
}

} // namespace TpGame
