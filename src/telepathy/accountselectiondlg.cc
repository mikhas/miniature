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

#include <TelepathyQt4/Account>

#include <QtDebug>

namespace Miniature
{

AccountSelectionDlg::AccountSelectionDlg(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui.setupUi(this);
}

AccountSelectionDlg::~AccountSelectionDlg()
{
}

void AccountSelectionDlg::setAccounts(QList<TpAccountItemPtr> accounts)
{
    mAccounts = accounts;

    Q_FOREACH(TpAccountItemPtr acc, mAccounts)
    {

    }
}

void AccountSelectionDlg::onAccountDisplayName(const QString &displayName)
{
    QString bla = displayName;
}

};
