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

#ifndef FICSSIDE_H
#define FICSSIDE_H

#include "namespace.h"
#include "abstractside.h"
#include "abstractbackend.h"
#include "dispatcher.h"

#include <QtCore>
#include <QtNetwork/QTcpSocket>

namespace Game
{

//! Represents one entry from a list of games.

struct SideRecord
{
    QByteArray name;
    uint rating;
    uint time_control; // in secs
    uint clock_time; // in secs
    uint material_strength;
};

struct Record
{
    bool valid;
    uint id;
    Mode mode;
    SideRecord white;
    SideRecord black;
    bool white_to_move;
    uint turn;
};

typedef QMap<uint, Record> GameTable;

//! A link for FICS
class FicsBackend
    : public AbstractBackend
{
    Q_OBJECT
    Q_DISABLE_COPY(FicsBackend)

private:
    WeakDispatcher m_dispatcher;
    QTcpSocket m_channel;
    QByteArray m_buffer;
    QString m_username;
    QString m_password;
    State m_state;
    bool m_enabled;
    QTimer m_login_abort_timer;
    QVector<char> m_extra_delimiter;

public:
    //! \reimp
    explicit FicsBackend(Dispatcher *dispatcher,
                         QObject *parent = 0);
    virtual ~FicsBackend();
    virtual void setFlags(CommandFlags flags);
    virtual void setEnabled(bool enable);
    virtual State state() const;
    virtual void login(const QString &username,
                       const QString &password);
    virtual void processToken(const QByteArray &token);
    virtual void listGames();
    //! \reimp_end

private:
    Q_SLOT void onReadyRead();
    void processLogin(const QByteArray &line);
    Q_SLOT void onHostFound();
    Q_SLOT void abortLogin();
    void configurePrompt();
};

//! Implement a backend for FICS (freechess.org).
//! Can be feeded by Game::FicsParser.
class FicsSide
    : public AbstractSide
{
    Q_OBJECT
    Q_DISABLE_COPY(FicsSide)

private:
    const QString m_identifier;
    const QString m_password;
    SideState m_state;
    SharedBackend m_link;

public:
    //! C'tor
    //! @param identifier the identifier for this side.
    //! @param link the shared FICS link.
    explicit FicsSide(const QString &identifier,
                      const SharedBackend &link);

    //! \reimp
    virtual ~FicsSide();
    virtual void init();
    virtual SideState state() const;
    virtual const QString &identifier() const;
    virtual void runInBackground();
    virtual void runInForeground();
    virtual void startTurn(const Move &move);
    virtual void onCommandFound(Command command,
                                const QByteArray &data);
    //! \reimp_end
};

} // namespace Game

#endif // FICSSIDE_H
