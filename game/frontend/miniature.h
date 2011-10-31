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
#include "abstractengine.h"
#include "game.h"
#include "position.h"
#include "availableseeks.h"

#include <QtCore>

namespace Game { namespace Frontend {

class AvailableSeeks;
class MessageLog;
class MiniaturePrivate;
class SideElement;

//! This class communicates with the engine, handles the active game state
//! (together with the engine) and exports objects and properties to the
//! graphical frontend, our QML UI.
class Miniature
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Miniature)
    Q_DECLARE_PRIVATE(Miniature)

    Q_ENUMS(Mode)
    Q_ENUMS(Rating)
    Q_ENUMS(Result)
    Q_ENUMS(Reason)
    Q_ENUMS(SquareStyle)

    Q_PROPERTY(Mode mode READ mode
                         WRITE setMode
                         NOTIFY modeChanged)

    Q_PROPERTY(bool validMove READ validMove
                              NOTIFY validMoveChanged)

    Q_PROPERTY(QString storedUsername READ storedUsername
                                      NOTIFY storedUsernameChanged)

    Q_PROPERTY(QString storedPassword READ storedPassword
                                      NOTIFY storedPasswordChanged)

private:
    const QScopedPointer<MiniaturePrivate> d_ptr;

public:
    enum Mode {
        TestFicsMode,
        FicsMode
    };

    enum Color {
        Auto = ColorNone,
        White = ColorWhite,
        Black = ColorBlack
    };

    enum Rating {
        RatingAny = ::Game::RatingAny,
        RatingEnabled = ::Game::RatingEnabled,
        RatingDisabled = ::Game::RatingDisabled
    };

    enum Result {
        ResultWhiteWins = ::Game::ResultWhiteWins,
        ResultBlackWins = ::Game::ResultBlackWins,
        ResultDraw = ::Game::ResultDraw,
        ResultUnknown = ::Game::ResultUnknown,
        ResultAdjourned = ::Game::ResultAdjourned
    };

    enum Reason {
        ReasonUnknown = ::Game::ReasonUnknown,
        ReasonForfeitByDisconnect = ::Game::ReasonForfeitByDisconnect,
        ReasonForfeitOnTime = ::Game::ReasonForfeitOnTime,
        ReasonDrawnOnTime = ::Game::ReasonForfeitOnTime,
        ReasonAbortedByDisconnect = ::Game::ReasonAbortedByDisconnect,
        ReasonAdjournedByDisconnect = ::Game::ReasonAdjournedByDisconnect,
        ReasonSurrendered = ::Game::ReasonSurrendered,
        ReasonCheckmated = ::Game::ReasonCheckmated,
        ReasonDrawAccpeted = ::Game::ReasonDrawAccpeted,
        ReasonAdjourned = ::Game::ReasonAdjourned
    };

    enum SquareStyle {
        MoveOrigin,
        MoveTarget,
        LastOrigin,
        LastTarget
    };

    explicit Miniature(Dispatcher *dispatcher,
                       QObject *parent = 0);
    virtual ~Miniature();
    virtual void show(const QUrl &ui);
    virtual void showBoard();

    virtual void handleRecord(const Record &r);
    virtual void handleSeek(const Seek &s);

    //! Mode property
    Q_INVOKABLE void setMode(Mode mode);
    Q_INVOKABLE Mode mode() const;
    Q_SIGNAL void modeChanged(Mode mode);

    Q_INVOKABLE void login(const QString &username,
                           const QString &password);
    Q_INVOKABLE void logout();
    Q_SIGNAL void loginSucceeded();
    Q_SIGNAL void loginFailed();

    //! Retrieve username from last successful (registered) login.
    Q_INVOKABLE QString storedUsername() const;
    Q_SIGNAL void storedUsernameChanged(const QString &username);

    //! Retrieve password from last successful (registered) login.
    Q_INVOKABLE QString storedPassword() const;
    Q_SIGNAL void storedPasswordChanged(const QString &password);

    //! Sends out a game offer.
    //! @param time initial time.
    //! @param increment time increment for each turn.
    //! @param rating whether the game is rated.
    //! @param color the preferred color
    Q_INVOKABLE void seek(uint time,
                          uint increment,
                          Rating rating,
                          Color color);

    //! Sends out a game offer.
    //! @param time initial time.
    //! @param increment time increment for each turn.
    //! @param rating whether the game is rated.
    //! @param color the preferred color
    Q_INVOKABLE void seek(uint time,
                          uint increment,
                          const QString &rating,
                          const QString &color);

    //! Respond to a game advertisement. Will trigger a match if id is valid
    //! (and opponent accepts, in case of manual starts).
    //! @param id the game advertisement id.
    Q_INVOKABLE void play(uint id);

    //! Resign from active game.
    Q_INVOKABLE void resign();

    //! Toggles whether a game advertisement is highlighted. Only one ad can be
    //! highlighted at a time.
    //! @param id the game advertisement id
    Q_INVOKABLE void toggleGameAdvertisementHighlighting(uint id);

    //! Selects a square. Returns whether selection succeeded.
    //! @param index the index of the selected square [0-63].
    Q_INVOKABLE bool selectSquare(int index);

    //! Returns whether last move was valid.
    Q_INVOKABLE bool validMove() const;
    Q_SIGNAL void validMoveChanged(bool valid);

    //! Confirms current move.
    Q_INVOKABLE void confirmMove();

    //! Sends a message from player to current opponent.
    //! @param message the message.
    Q_INVOKABLE void sendMessage(const QString &message);

    //! Emitted when invalid move was submitted to engine.
    Q_SIGNAL void invalidMove(const QString &move);

    //! Emitted when both sides accepted game and it was started.
    Q_SIGNAL void gameStarted();

    //! Emitted when player responded to an invalid seek.
    Q_SIGNAL void seekCancelled();

    //! Emitted when the active game was ended.
    Q_SIGNAL void gameEnded(int result,
                            int reason);

    //! Sets the active game.
    //! @param game the game.
    void setActiveGame(Game *game);

    //! Returns agive game, can be 0.
    Game * activeGame() const;

    //! Returns the message log.
    Frontend::MessageLog * messageLog() const;

    //! Sets username.
    void setUsername(const QString &username);

    //! Updates the UI facing side element from local_side.
    //! @param local_side the update for local side.
    void updateLocalSide(const Side &local_side);

    //! Updates the UI facing side element from remote_side.
    //! @param remote_side the update for remote side.
    void updateRemoteSide(const Side &remote_side);

    //! Test API
    SideElement * localSide();
    SideElement * remoteSide();
    AvailableSeeks * availableSeeks();

private:    
    void sendCommand(AbstractCommand *command);
    Q_SLOT void onPositionChanged(const Position &position);
    Q_SLOT void onLoginSucceeded();
};

}} // namespace Game, Frontend

#endif // FRONTEND_H
