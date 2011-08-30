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

#include "abstractengine.h"
#include "dispatcher.h"

#include <QtCore>

namespace Game {

class AbstractCommand;

//! Gnuchess engine
class GnuChess
    : public AbstractEngine
{
    Q_OBJECT
    Q_DISABLE_COPY(GnuChess)

private:
    QProcess m_proc;
    WeakDispatcher m_dispatcher;

public:
    //! \reimp
    explicit GnuChess(Dispatcher *dispatcher,
                      QObject *parent = 0);
    virtual ~GnuChess();
    virtual void setEnabled(bool enable);
    virtual void play(uint advertisement_id);
    virtual void movePiece(const MovedPiece &moved_piece);
    virtual void processToken(const QByteArray &token);
    //! \reimp_end

    virtual void runInBackground();
    virtual void runInForeground();

private:
    Q_SLOT void onReadyRead();
    void sendCommand(AbstractCommand *command);
};

} // namespace Game

#endif // GNUCHESS_H
