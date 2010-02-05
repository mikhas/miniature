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

#include <TelepathyQt4/Account>

#include <QtDebug>

namespace Miniature
{

AccountSelectionDlg::AccountSelectionDlg(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_accountListModel(NULL)
{
    ui.setupUi(this);
}

AccountSelectionDlg::~AccountSelectionDlg()
{
}

void AccountSelectionDlg::showEvent(QShowEvent *event)
{
    ui.chooseButton->setEnabled(false);
    ui.chooseButton->setText(QObject::tr("Choose account"));
    QObject::connect(ui.chooseButton, SIGNAL(clicked(bool)), this, SLOT(chooseAccountClicked(bool)));
    QObject::connect(ui.listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(listItemClicked(const QModelIndex &)));

    QDialog::showEvent(event);
}

void AccountSelectionDlg::setAccounts(QList<TpAccountItemPtr> accounts)
{
    mAccounts = accounts;

    Q_FOREACH(TpAccountItemPtr acc, mAccounts)
    {
        acc->disconnect();
        QObject::connect(acc.data(), SIGNAL(initialized()), this, SLOT(onAccountInitialized()));
        acc->initialize();
    }
}

void AccountSelectionDlg::onAccountInitialized()
{
    if(m_accountListModel == NULL)
    {
        m_accountListModel = new TpAccountListModel();
    }

    m_accountListModel->setAccounts(mAccounts);
    ui.listView->setModel(m_accountListModel);
}

void AccountSelectionDlg::listItemClicked(const QModelIndex &)
{
    ui.chooseButton->setEnabled(true);
}

void AccountSelectionDlg::chooseAccountClicked(bool /*checked*/)
{
    ui.chooseButton->disconnect();
    ui.chooseButton->setText(QObject::tr("Choose contact"));
    ui.chooseButton->setEnabled(false);
    QObject::connect(ui.chooseButton, SIGNAL(clicked(bool)), this, SLOT(chooseContactClicked(bool)));

    ui.listView->clearSelection();
}

void AccountSelectionDlg::chooseContactClicked(bool /*checked*/)
{

}

};
