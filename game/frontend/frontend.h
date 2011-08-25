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
#include "game.h"
#include "position.h"

#include <QtCore>

namespace Game { namespace Frontend {

QString fromColor(Color color);

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
    virtual void showBoard();

    virtual void handleRecord(const Record &r);
    virtual void handleSeek(const Seek &s);

    Q_INVOKABLE void login(const QString &username,
                           const QString &password);

    //! Respond to a game advertisement. Will trigger a match if id is valid
    //! (and opponent accepts, in case of manual starts).
    //! @param id the game advertisement id.
    Q_INVOKABLE void play(uint id);

    //! Toggles whether a game advertisement is highlighted. Only one ad can be
    //! highlighted at a time.
    //! @param id the game advertisement id
    Q_INVOKABLE void toggleGameAdvertisementHighlighting(uint id);

    //! Performs a move. Will be sent once confirmed.
    //! @param origin the original square of the moved piece.
    //! @param target the target square of the moved piece.
    //! @param promotion the promotion to use (optional, defaults to queen).
    Q_INVOKABLE void move(int origin,
                          int target,
                          const QString &promotion = QString("queen"));

    //! Confirms current move.
    Q_INVOKABLE void confirmMove();

    //! Sets the active game.
    //! @param game the game.
    void setActiveGame(Game *game);

private:
    void sendCommand(AbstractCommand *command);
    Q_SLOT void onPositionChanged(const Position &position);
};

}} // namespace Game, Frontend

#endif // FRONTEND_H
