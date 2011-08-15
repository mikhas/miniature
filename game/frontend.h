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

#ifndef FRONTEND_H
#define FRONTEND_H

#include "dispatcher.h"
#include "commandline.h"
#include "linereader.h"
#include "abstractbackend.h"
#include "side.h"

#include <QtCore>

namespace Game {

class FrontendPrivate;

//! Frontend, the root context used for QML
class Frontend
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Frontend)
    Q_DECLARE_PRIVATE(Frontend)

private:
    const QScopedPointer<FrontendPrivate> d_ptr;

public:
    explicit Frontend(Dispatcher *dispatcher,
                      QObject *parent = 0);
    virtual ~Frontend();
    virtual void show(const QUrl &ui);

    virtual void handleRecord(const Record &r);
    virtual void handleSeek(const Seek &s);

    Q_INVOKABLE void login(const QString &username,
                           const QString &password);

    //! Respond to a game advertisement. Will trigger a match if id is valid
    //! (and opponent accepts, in case of manual starts).
    //! @param id the game advertisement id.
    Q_INVOKABLE void play(uint id,
                          const QString &local_identifier,
                          const QString &remote_identifier);

    //! Sets the model representing the local side.
    //! @param side the local side.
    void setLocalSide(const WeakSide &side);

    //! Sets the model representing the remote side.
    //! @param side the remote side.
    void setRemoteSide(const WeakSide &side);

private:
    void sendCommand(AbstractCommand *command);
};

} // namespace Game

#endif // FRONTEND_H
