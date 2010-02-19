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

#ifndef TPGAME_H
#define TPGAME_H

#include "tpaccountmanager.h"
#include "accountselectiondlg.h"

#include <QObject>

namespace Miniature
{

class TpGame : public QObject
{
    Q_OBJECT
public:
    TpGame(QObject *parent = 0);

public Q_SLOTS:
    void hostGame();
    void joinGame();

Q_SIGNALS:
    void initialized();

private:
    TpAccountManager *mTpAccountManager;
    AccountSelectionDlg *mAccountsDialog;
};

};

#endif // TPGAME_H
