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

#ifndef ACCOUNTSELECTIONDLG_H_
#define ACCOUNTSELECTIONDLG_H_

#include <QDialog>
#include <QList>
#include <QPersistentModelIndex>

#include "tpaccountitem.h"
#include "tpaccountlistmodel.h"
#include "tpcontactslistmodel.h"
#include "select_account.ui.h"

#include <TelepathyQt4/Contact>

namespace TpGame
{

class AccountManager;

class AccountSelectionDlg
    : public QDialog
{
    Q_OBJECT
public:
    explicit AccountSelectionDlg(AccountManager *account_manager, QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AccountSelectionDlg();

Q_SIGNALS:
    void ensureChannel(const QString &, const QString &);

protected:
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void onAccountNamesChanged(const QStringList &account_names);
    void listItemClicked(const QModelIndex &index);
    void chooseAccountClicked(bool checked = false);
    void chooseContactClicked(bool checked = false);
    void onContactsForAccountChanged(const Tp::Contacts &contacts);

private:
    QList<SharedAccountItem> m_accounts;
    Ui::SelectAccountDialog ui;
    AccountManager *m_account_manager;
    TpAccountListModel *m_account;
    TpContactsListModel *m_contacts;
    QPersistentModelIndex m_last_index;
    QString m_account_name;
};

} // namespace TpGame

#endif //ACCOUNTSELECTIONDLG_H_


