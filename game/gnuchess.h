/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef GNUCHESS_H
#define GNUCHESS_H

#include "move.h"
#include "abstractside.h"

#include <QtCore>

namespace Game {

//! Controls game.
class GnuChess
    : public AbstractSide
{
    Q_OBJECT
    Q_DISABLE_COPY(GnuChess)

private:
    const QString m_identifier;
    SideState m_state;
    QProcess m_proc;

public:
    //! \reimp
    explicit GnuChess(const QString &identifier);
    virtual ~GnuChess();
    virtual void init();
    virtual SideState state() const;
    virtual const QString &identifier() const;
    virtual void runInBackground();
    virtual void runInForeground();
    virtual void startTurn(const Move &move);
    //! \reimp_end

private:
    Q_SLOT void onReadyRead();
};

} // namespace Game

#endif // GNUCHESS_H
