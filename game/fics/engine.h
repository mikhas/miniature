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

#ifndef FICS_BACKEND_H
#define FICS_BACKEND_H

#include "namespace.h"
#include "abstractengine.h"
#include "dispatcher.h"

#include <QtCore>
#include <QtNetwork/QTcpSocket>

namespace Game { namespace Fics {

//! The FICS engine. Responsible for receiving and transmitting commands to FICS.
class Engine
    : public AbstractEngine
{
    Q_OBJECT
    Q_DISABLE_COPY(Engine)

public:
    //! Engine uses MessageFilter for incoming messages from FICS.
    enum MessageFilter {
        None = 0x0,
        LoginRequest = 0x1,
        PlayRequest = 0x2,
        InGame = 0x4,
        WaitingForSeeks = 0x8,
        WaitingForGames = 0x10,
        WaitingForWrappedChatMessage = 0x20
    };

    Q_ENUMS(MessageFilter)
    Q_DECLARE_FLAGS(MessageFilterFlags, MessageFilter)

private:
    WeakDispatcher m_dispatcher;
    QTcpSocket m_channel;
    QByteArray m_buffer;
    QByteArray m_last_token;
    QString m_username; // FIXME: Change to QBA.
    QString m_password; // FIXME: Change to QBA.
    MessageFilterFlags m_filter;
    bool m_enabled;
    bool m_channel_enabled;
    bool m_logged_in;
    bool m_past_welcome_screen;
    uint m_login_count;
    QTimer m_login_abort_timer; // TODO: make this a general watchdog over the FICS connection.
    QVector<char> m_extra_delimiter;
    uint m_current_advertisment_id; // The ad id we have responed to.
    GameInfo m_current_game; // The game we currently play on FICS.

public:
    //! \reimp
    explicit Engine(Dispatcher *dispatcher,
                    QObject *parent = 0);
    virtual ~Engine();
    virtual void setEnabled(bool enable);
    virtual void login(const QString &username,
                       const QString &password);
    virtual void logout();
    virtual void seek(uint time,
                      uint increment,
                      Rating rating,
                      Color color);
    virtual void play(uint advertisement_id);
    virtual void endGame(Reason reason);
    virtual void movePiece(const MovedPiece &moved_piece);
    virtual void readData(const QByteArray &token);
    virtual void writeData(const QByteArray &data);
    virtual void sendMessage(const QByteArray &player_name,
                             const QByteArray &message);
    //! \reimp_end

    //! Test API
    virtual void setMessageFilter(const MessageFilterFlags &flags = MessageFilterFlags(InGame));
    virtual void setChannelEnabled(bool enabled);

private:
    Q_SLOT void onReadyRead();
    void processLogin(const QByteArray &line);
    Q_SLOT void onHostFound();
    Q_SLOT void abortLogin();
    Q_SLOT void reconnect();
    void configureFics();
    void sendLogin();
    void finalizeLogin();
    void sendCommand(AbstractCommand *command);
};

} // namespace Fics
} // namespace Game

#endif // FICS_BACKEND_H
