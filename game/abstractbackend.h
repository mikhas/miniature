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

#ifndef AbstractBackend_H
#define AbstractBackend_H

#include "namespace.h"
#include "position.h"

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

//! Represents one player side in a game record.
struct PlayerRecord
{
    bool valid;
    QByteArray name;
    uint rating;
    uint remaining_time; // in secs
    uint material_strength;

    explicit PlayerRecord();
};

//! Represents one entry from a list of games.
struct Record
    : public RecordSeekBase
{
    PlayerRecord white;
    PlayerRecord black;
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

class AbstractBackend;
typedef QSharedPointer<AbstractBackend> SharedBackend;

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
class AbstractBackend
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractBackend)

public:
    // FIXME: Use flags instead of states!
    // Allows a backend to say "I expect this, this and that message, but not that one.".
    //! The link's state.
    enum State {
        StateIdle,
        StateLoginPending,
        StateLoginFailed,
        StatePlayPending,
        StatePlayFailed,
        StateReady,
    };

    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    //! C'tor
    //! @param parent the owner of this instance (optional).
    explicit AbstractBackend(QObject *parent = 0);

    virtual ~AbstractBackend() = 0;

    //! Enables command parsing. Initializes input device backend for tokenizer
    //! if required.
    //! @param enable whether to enable command parsing.
    virtual void setEnabled(bool enable) = 0;

    //! Processes a token. Graphical user interface may want to call this
    //! method directly.
    //! @param token the token to be processed. If the token could be
    //!              translated into a command, commandFound will be emitted.
    Q_SLOT virtual void processToken(const QByteArray &token) = 0;

    //! Returns state of this link.
    virtual State state() const;

    //! Emitted whenever state changes.
    Q_SIGNAL void stateChanged(State state);

    //! Login to remote server.
    //! @param username the username of the account.
    //! @param password the password of the account.
    virtual void login(const QString &username,
                       const QString &password);

    //! Play a game.
    //! @param advertisement_id the play request was the response to a previous
    //!        game advertisement; this is the ad's id (optional).
    virtual void play(uint advertisement_id = 0);

    //! Move piece.
    //! @param moved_piece the moved piece.
    virtual void movePiece(const MovedPiece &moved_piece);
};

} // namespace Game

#endif // AbstractBackend_H
