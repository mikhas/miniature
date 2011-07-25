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

#include "ficsside.h"
#include "move.h"
#include "abstracttokenizer.h"

namespace Game {

AbstractLink::AbstractLink(QObject *parent)
    : AbstractParser(parent)
{}

AbstractLink::~AbstractLink()
{}

AbstractLink::State AbstractLink::state() const
{
    return StateIdle;
}

FicsLink::FicsLink(QObject *parent)
    : AbstractLink(parent)
    , m_channel()
    , m_buffer()
    , m_username()
    , m_password()
    , m_state(StateIdle)
    , m_enabled(false)
    , m_login_abort_timer()
    , m_extra_delimiter()
{
    m_login_abort_timer.setSingleShot(true);
    m_login_abort_timer.setInterval(10000);

    connect(&m_login_abort_timer, SIGNAL(timeout()),
            this,                 SLOT(abortLogin()));

    connect(&m_channel, SIGNAL(readyRead()),
            this,       SLOT(onReadyRead()));

    connect(&m_channel, SIGNAL(hostFound()),
            this,       SLOT(onHostFound()));
}

FicsLink::~FicsLink()
{}

// Not supported, we always read everything from FICS.
void FicsLink::setFlags(CommandFlags)
{}

void FicsLink::setEnabled(bool enable)
{
    m_enabled = enable;

    if (not m_enabled) {
        m_channel.disconnectFromHost();
        m_channel.waitForDisconnected();

        if (m_state != StateIdle) {
            m_state = StateIdle;
            emit stateChanged(m_state);
        }
    } else {
        m_channel.connectToHost("freechess.org", 5000, QIODevice::ReadWrite);
        m_channel.waitForConnected();

        if (m_state != StateReady) {
            m_state = StateReady;
            emit stateChanged(m_state);
        }
    }
}

AbstractLink::State FicsLink::state() const
{
    return m_state;
}

void FicsLink::login(const QString &username,
                     const QString &password)
{
    if (m_state != StateReady) {
        return;
    }

    m_state = StateLoginPending;
    emit stateChanged(m_state);
    m_login_abort_timer.start();

    m_username = username;
    m_password = password;

    m_channel.write(m_username.toLatin1());
    m_channel.write("\n");

    m_extra_delimiter.clear();
    m_extra_delimiter.append(':');
    m_extra_delimiter.append('%');
}

void FicsLink::processToken(const QByteArray &token)
{
    if (not m_enabled || token.isEmpty()) {
        return;
    }

    switch(m_state) {
    case StateLoginFailed:
    case StateLoginPending:
        processLogin(token);
        break;

    case StateIdle:
        (void) m_channel.readAll();
        break;

    case StateReady:
        m_buffer.append(m_channel.readAll());
        break;
    }
}

void FicsLink::onReadyRead()
{
    int next_newline_pos = -1;
    const bool enable_echo = true;
    do {
        processToken(scanLine(&next_newline_pos, &m_channel, &m_buffer, enable_echo, m_extra_delimiter));
    } while (next_newline_pos != -1);
}

void FicsLink::processLogin(const QByteArray &line)
{
    // TODO: write proper tokenizer?
    static const QByteArray confirm_login("Press return to enter the server as");
    static const QByteArray enter_password("password");
    static const QByteArray fics_prompt("fics");

    if (line.startsWith(confirm_login)) {
        m_login_abort_timer.stop();
        m_login_abort_timer.start();
        // Confirm login:
        m_channel.write("\n");

        if (m_username == "guest") {
            m_username = line.mid(confirm_login.length() + 2,
                                  line.length() - confirm_login.length() - 4);
        }
    } else if (line.startsWith(enter_password)) {
        m_login_abort_timer.stop();
        m_login_abort_timer.start();
        m_channel.write(m_password.toLatin1());
        m_channel.write("\n");
    } else if (line.startsWith(fics_prompt)) {
        m_login_abort_timer.stop();
        m_extra_delimiter.clear();
        configurePrompt();
        m_state = StateReady;
        emit stateChanged(m_state);
    }
}

void FicsLink::onHostFound()
{
    // TODO: Handle retry attempts here.
}

void FicsLink::abortLogin()
{
    if (m_state != StateLoginPending) {
        return;
    }

    qDebug() << "Failed to login in with as" << m_username;
    m_state = StateLoginFailed;
    emit stateChanged(m_state);
}

void FicsLink::configurePrompt()
{
    if (m_state != StateReady) {
        return;
    }

    m_channel.write("set style 12");
    m_channel.write("\n");
}

FicsSide::FicsSide(const QString &identifier,
                   const SharedLink &link)
    : AbstractSide(identifier, link)
    , m_identifier(identifier)
    , m_state(NotReady)
    , m_link(link)
{}

FicsSide::~FicsSide()
{}

void FicsSide::init()
{
    m_state = Ready;
    emit ready();
}

AbstractSide::SideState FicsSide::state() const
{
    return m_state;
}

const QString &FicsSide::identifier() const
{
    return m_identifier;
}

void FicsSide::runInBackground()
{
    if (m_state == NotReady) {
        return;
    }

    m_state = RunInBackground;
    disconnect(m_link.data(), SIGNAL(commandFound(Command, QByteArray)),
               this,          SLOT(onCommandFound(Command, QByteArray)));
}

void FicsSide::runInForeground()
{
    if (m_state == RunInBackground) {
        m_state = Ready;
        connect(m_link.data(), SIGNAL(commandFound(Command, QByteArray)),
                this,          SLOT(onCommandFound(Command, QByteArray)),
                Qt::UniqueConnection);
    }
}

void FicsSide::startTurn(const Move &move)
{
    Q_UNUSED(move)
}

void FicsSide::onCommandFound(Command command,
                               const QByteArray &data)
{
    Q_UNUSED(command)
    Q_UNUSED(data)
}

} // namespace Game
