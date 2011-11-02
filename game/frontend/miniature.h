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

#ifndef FRONTEND_MINIATURE_H
#define FRONTEND_MINIATURE_H

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

//! \brief Represents an additional layer between QML UI and backend.
//!
//! The frontend facing API is the sum of Qt properties, signals, slots and
//! Q_INVOKABLE methods of this class. The remaining API is used by the C++
//! backend.
//! When compiled with qmake CONFIG+=enable-gui, the following root contexts
//! are available:
//! - miniature: An instance of this class.
//! - availableSeeks: An instance of AvailableSeeks. Shows all available seeks
//!   through an abstract list model.
//! - messageLog: An instance of MessageLog. Shows all messages during a game
//!   though an abstract list model.
//! - chessBoard: An instance of ChessBoard. Provides images and square styles
//!   for the board grid.
//! - localSide: An instance of SideElement. Exports all properties of the
//!   local player (shown data only valid during a game).
//! - remoteSide: An instance of SideElement. Exports all properties of the
//!   remote player (shown data only valid during a game).
//! - activeGame: An instance of GameElement. Exports all properties of the
//!   active game (shown data only valid during a game).
class Miniature
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Miniature)
    Q_DECLARE_PRIVATE(Miniature)

    Q_ENUMS(Mode)
    Q_ENUMS(GameMode)
    Q_ENUMS(Rating)
    Q_ENUMS(Result)
    Q_ENUMS(Reason)
    Q_ENUMS(GameResolution)
    Q_ENUMS(SquareStyle)

    Q_PROPERTY(Mode mode READ mode
                         WRITE setMode
                         NOTIFY modeChanged)

    Q_PROPERTY(bool validMove READ validMove
                              NOTIFY validMoveChanged)

    Q_PROPERTY(bool autoConfirmMoveEnabled READ autoConfirmMoveEnabled
                                           WRITE setAutoConfirmMoveEnabled
                                           NOTIFY autoConfirmMoveEnabledChanged)

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

    enum GameMode {
        GameModeNone = ::Game::ModeNone,
        GameModeBlitz = ::Game::ModeBlitz,
        GameModeLightning = ::Game::ModeLightning,
        GameModeStandard = ::Game::ModeStandard
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
        ReasonDrawAccepted = ::Game::ReasonDrawAccpeted,
        ReasonAdjourned = ::Game::ReasonAdjourned
    };

    enum GameResolution {
        GameResolutionDraw = ::Game::ResolutionDraw,
        GameResolutionAdjourn = ::Game::ResolutionAdjourn,
        GameResolutionAbort = ::Game::ResolutionAbort
    };

    enum SquareStyle {
        MoveOrigin,
        MoveTarget,
        LastOrigin,
        LastTarget
    };

    //! C'tor
    //! @param dispatcher the Dispatcher instance, used to communicate with
    //!        other Miniature components.
    //! @param parent the owner of this instance (optional).
    explicit Miniature(Dispatcher *dispatcher,
                       QObject *parent = 0);

    //! D'tor
    virtual ~Miniature();

    //! Loads and shows QML UI from the given QUrl.
    //! @param ui the QUrl to the QML UI.
    virtual void show(const QUrl &ui);

    //! Handles a new seek and appends it to list of available seeks.
    //! @param seek the new seek.
    virtual void handleSeek(const Seek &seek);

    //! Mode property.
    //! @param mode which mode to use for Miniature (test, FICS, ...).
    Q_INVOKABLE void setMode(Mode mode);
    Q_INVOKABLE Mode mode() const;
    Q_SIGNAL void modeChanged(Mode mode);

    //! Logs in to backend.
    //! @param username the username. If using FICS, then "guest" can be used
    //!        to login as guest (no password needed).
    //! @param password the password for the registered username. Leave empty
    //!        for "guest" login to FICS.
    Q_INVOKABLE void login(const QString &username,
                           const QString &password);

    //! Logs out from backend.
    Q_INVOKABLE void logout();

    //! Emitted when login was successful.
    Q_SIGNAL void loginSucceeded();

    //! Emitted when login failed (timeout, wrong password, ...).
    Q_SIGNAL void loginFailed();

    //! Retrieves username from last successful (registered) login.
    Q_INVOKABLE QString storedUsername() const;

    //! Emitted when stored username changed.
    //! @param username the changed username.
    Q_SIGNAL void storedUsernameChanged(const QString &username);

    //! Retrieves password from last successful (registered) login.
    Q_INVOKABLE QString storedPassword() const;

    //! Emitted when stored password changed.
    //! @param password the changed password.
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

    //! Propose a game resolution. Resolution maps to
    //! Miniature::GameResolution enum.
    //! @param resolution the proposed game resolution.
    Q_INVOKABLE void proposeGameResolution(int resolution);

    //! Emitted when opponent proposes a game resolution. Resolution maps to
    //! Miniature::GameResolution enum.
    //! @param resolution the proposed game resolution.
    Q_SIGNAL void gameResolutionProposed(int resolution);

    //! Accept a game resolution. Resolution maps to
    //! Miniature::GameResolution enum.
    //! @param resolution the accepted game resolution.
    Q_INVOKABLE void acceptGameResolution(int resolution);

    //! Emitted when opponent accepts a game resolution. Resolution maps to
    //! Miniature::GameResolution enum.
    //! @param resolution the proposed game resolution.
    Q_SIGNAL void gameResolutionAccepted(int resolution);

    //! Reject a game resolution. Resolution maps to
    //! Miniature::GameResolution enum.
    //! @param resolution the rejected game resolution.
    Q_INVOKABLE void rejectGameResolution(int resolution);

    //! Emitted when opponent rejects a game resolution. Resolution maps to
    //! Miniature::GameResolution enum.
    //! @param resolution the rejected game resolution.
    Q_SIGNAL void gameResolutionRejected(int resolution);

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
    //! \sa setAutoConfirmMoveEnabled
    Q_INVOKABLE void confirmMove();

    //! Whether moves are executed as soon as they become 'valid' or whether
    //! moves need to be confirmed first. Off by default, that is, moves
    //! require confirmation.
    //! \sa confirmMove
    //! @param enable the flag to enable/disable this option.
    Q_INVOKABLE void setAutoConfirmMoveEnabled(bool enable);

    //! Whether auto-confirmation of moves is enabled.
    Q_INVOKABLE bool autoConfirmMoveEnabled() const;
    Q_SIGNAL void autoConfirmMoveEnabledChanged(bool enabled);

    //! Sends a message from player to current opponent.
    //! @param message the message.
    Q_INVOKABLE void sendMessage(const QString &message);

    //! Emitted when invalid move was submitted to engine.
    //! @param move the invalid move.
    Q_SIGNAL void invalidMove(const QString &move);

    //! Emitted when both sides accepted game and it was started.
    Q_SIGNAL void gameStarted();

    //! Emitted when player responded to an invalid seek.
    Q_SIGNAL void seekCancelled();

    //! Emitted when the active game was ended.
    //! @param result the result, maps to Miniature::Result enum.
    //! @param reason the reason, maps to Miniature::Reason enum.
    Q_SIGNAL void gameEnded(int result,
                            int reason);

    //! Returns game mode.
    // TODO: Why is QML not capable of handling an enum as retval? Feels lame to have to fall back to strings here.
    Q_INVOKABLE QString gameMode() const;

    //! Sets the active game.
    //! @param game the game.
    void setActiveGame(Game *game);

    //! Returns agive game, can be 0.
    Game * activeGame() const;

    //! Returns the message log.
    Frontend::MessageLog * messageLog() const;

    //! Sets username.
    //! @param username the new username.
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

#endif // FRONTEND_MINIATURE_H
