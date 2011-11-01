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

#ifndef ABSTRACTENGINE_H
#define ABSTRACTENGINE_H

#include "namespace.h"
#include "position.h"
#include "side.h"

#include <QtCore>

namespace Game {

//! Base structure for game records or seeks.
struct RecordSeekBase
{
    bool valid;
    uint id;
    Mode mode;
    uint time; // in secs
    uint increment; // in secs
};

//! Represents one entry from a list of games.
struct Record
    : public RecordSeekBase
{
    Side white;
    Side black;
    bool white_to_move;
    uint turn;
};

//! Represents an offer for a game.
struct Seek
    : public RecordSeekBase
{
    QByteArray player_name;
    uint rating;
    bool is_rated;
    Color color;
    bool is_auto_started;
    bool uses_formula;
    QPair<int, int> rating_range;
    bool highlighted;
};

struct GameInfo {
    uint id;
    bool valid;
    Side white;
    Side black;
    Rating rating;
    Mode mode;
    uint time;
    uint time_increment;

    GameInfo();
};

class Dispatcher;
//! \brief An interface that represents the communication between Miniature
//! and a chess engine, usually via the engine's text stream interface.
//!
//! The communication is bidirectional, with readData being the
//! engine-to-Miniature direction and writeData being the Miniature-to-engine
//! direction. Instead of forcing Miniature to send raw text requests, a set
//! of covenience methods provides Miniature a type-safe interface to send
//! requests to an engine. Engines can use the Dispatcher to send commands to
//! other Miniature components. Engines are strictly separated from each other,
//! that is, no direct communication between engine instances is assumed.
class AbstractEngine
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEngine)

public:
    //! C'tor
    //! @param parent the owner of this instance (optional).
    explicit AbstractEngine(QObject *parent = 0);

    //! D'tor
    virtual ~AbstractEngine() = 0;

    //! Sets the Dispatcher for this engine. Without dispatcher, an engine
    //! is considered to be disabled, that is, it might not read nor write
    //! data from/to its text stream interface.
    virtual void setDispatcher(Dispatcher *dispatcher) = 0;

    //! Reads data from an engine, using its text stream interface.
    //! @param data the data to be read.
    Q_SLOT virtual void readData(const QByteArray &data) = 0;

    //! Writes data to an engine, using its text stream interface.
    //! @param data the data to be written.
    Q_SLOT virtual void writeData(const QByteArray &data) = 0;

    //! Login to engine, for example a remote server.
    //! @param username the username of the account.
    //! @param password the password of the account.
    virtual void login(const QString &username,
                       const QString &password);

    //! Logs out from engine, for example a remote server.
    virtual void logout();

    //! Sends a game offer to the engine.
    //! @param time initial time.
    //! @param increment time increment for each turn.
    //! @param rating whether the game is rated.
    //! @param color the preferred color
    virtual void seek(uint time,
                      uint increment,
                      Rating rating,
                      Color color);

    //! Play a game by sending a response to a previous game offer to the
    //! engine.
    //! @param advertisement_id the play request was the response to a previous
    //!        game advertisement; this is the ad's id (optional).
    virtual void play(uint advertisement_id = 0);

    //! Ends a game. Send to the engine.
    //! @param reason why/how the game was ended.
    virtual void endGame(Reason reason);

    //! Proposes a game resolution. Send to the engine.
    //! @param resolution the proposed game resolution.
    virtual void proposeGameResolution(Resolution resolution);

    //! Accepts a game resolution. Send to the engine.
    //! @param resolution the accepted game resolution.
    virtual void acceptGameResolution(Resolution resolution);

    //! Rejects a game resolution. Send to the engine.
    //! @param resolution the rejected game resolution.
    virtual void rejectGameResolution(Resolution resolution);

    //! Move piece. Send to the engine.
    //! @param moved_piece the moved piece.
    virtual void movePiece(const MovedPiece &moved_piece);

    //! Sends player's message to current opponent to the engine.
    //! @param player_name the sender of the message.
    //! @param message the message.
    virtual void sendMessage(const QByteArray &player_name,
                             const QByteArray &message);
};

} // namespace Game

#endif // ABSTRACTENGINE_H
