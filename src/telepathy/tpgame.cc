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

#include "tpgame.h"

namespace Miniature
{

TpGame::TpGame(QObject *parent) : QObject(parent)
{
    mTpAccountManager = new TpAccountManager(this);
    mAccountsDialog = new AccountSelectionDlg(mTpAccountManager);
}

void TpGame::hostGame()
{
    mAccountsDialog->show();

    Q_EMIT initialized();
}

void TpGame::joinGame()
{
    mAccountsDialog->show();

    Q_EMIT initialized();
}

};
