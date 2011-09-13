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

class AbstractEngine;
typedef QSharedPointer<AbstractEngine> SharedEngine;

//! Can retrieve input from command line or graphical user interface and
//! translate input into proper commands.
//!
//! For streamed input, it makes sense to use a tokenizer. Simply
//! connect a tokenizer's tokenFound signal to the processToken slot.
//! For each instance, the range of accepted commands can be specified through
//! setFlags. If two instances accept the same command - say, while reading
//! from the same tokenizer - then both instances may emit a commandFound,
//! should the command be found in the common input stream.
//! To retrieve commands, clients hook up to the commandFound signal.
//! A graphical user interface can inject commands through processToken.
class AbstractEngine
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEngine)

public:
    //! C'tor
    //! @param parent the owner of this instance (optional).
    explicit AbstractEngine(QObject *parent = 0);

    virtual ~AbstractEngine() = 0;

    //! Enables command parsing. Initializes input device backend for tokenizer
    //! if required.
    //! @param enable whether to enable command parsing.
    virtual void setEnabled(bool enable) = 0;

    //! Processes a token. Graphical user interface may want to call this
    //! method directly.
    //! @param token the token to be processed. If the token could be
    //!              translated into a command, commandFound will be emitted.
    Q_SLOT virtual void processToken(const QByteArray &token) = 0;

    //! Login to remote server.
    //! @param username the username of the account.
    //! @param password the password of the account.
    virtual void login(const QString &username,
                       const QString &password);

    //! Logs out from remote server.
    virtual void logout();

    //! Sends out a game offer.
    //! @param time initial time.
    //! @param increment time increment for each turn.
    //! @param rating whether the game is rated.
    //! @param color the preferred color
    virtual void seek(uint time,
                      uint increment,
                      Rating rating,
                      Color color);

    //! Play a game.
    //! @param advertisement_id the play request was the response to a previous
    //!        game advertisement; this is the ad's id (optional).
    virtual void play(uint advertisement_id = 0);

    //! Move piece.
    //! @param moved_piece the moved piece.
    virtual void movePiece(const MovedPiece &moved_piece);

    //! Sends a message from player to current opponent.
    //! @param player_name the player name.
    //! @param message the message.
    virtual void sendMessage(const QByteArray &player_name,
                             const QByteArray &message);
};

} // namespace Game

#endif // ABSTRACTENGINE_H
