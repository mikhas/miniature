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

namespace Miniature
{

class TpAccountManager;

class AccountSelectionDlg : public QDialog
{
    Q_OBJECT
public:
    AccountSelectionDlg(TpAccountManager *, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~AccountSelectionDlg();

Q_SIGNALS:
    void ensureChannel(QString, QString);

protected:
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void onAccountNameListChanged(const QList<QString>);
    void listItemClicked(const QModelIndex &);
    void chooseAccountClicked(bool checked = false);
    void chooseContactClicked(bool checked = false);
    void onContactsForAccount(const Tp::Contacts);

private:
    QList<TpAccountItemPtr> mAccounts;
    Ui::SelectAccountDialog ui;
    TpAccountManager *mAccMgr;
    TpAccountListModel *mAccountListModel;
    TpContactsListModel *mContactsListModel;
    QPersistentModelIndex lastItemIndex;
    QString mAccountName;
};

};

#endif //ACCOUNTSELECTIONDLG_H_


