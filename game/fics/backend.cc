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

#include "fics/backend.h"
#include "commands/record.h"
#include "commands/advertisement.h"
#include "linereader.h"

namespace {
    // %1 is a placeholder the game ad id.
    const QString play_command("play %1\n");

    // Matches: "GuestGZBJ (++++) seeking 15 12 unrated standard [white] m f ("play 160" to respond)"
    const QRegExp match_seek("\\s*(\\w+)\\s+\\(([0-9+]+)\\)\\s+seeking\\s+(\\d+)\\s+(\\d+)"
                             "\\s+(\\w+)\\s+(\\w+)\\s+(\\[(\\w+)\\])?\\s*(m|a)?\\s*(f)?"
                             "\\s*(\\[(\\d+)\\-(\\d+)\\])?\\s*\\(\"play\\s+(\\d+)\" to respond\\)");

    // Matches: "92 2370 playerABC     2383 playerDEF  [ br  5   5]   2:22 -  3:17 (18-18) W: 42"
    const QRegExp match_record("\\s*(\\d+)\\s+([0-9+]+)\\s+(\\w+)\\s+([0-9+]+)\\s+(\\w+)"
                               "\\s+\\[\\s*(\\w{1,3})\\s*(\\d+)\\s+(\\d+)\\s*\\]"
                               "\\s+(\\d+):(\\d+)\\s+\\-\\s+(\\d+):(\\d+)\\s+"
                               "\\((\\d+)\\-(\\d+)\\)\\s+(\\w):\\s+(\\d+)");

    bool parseRating(Game::RecordSeekBase *rsb,
                     uint *rating,
                     const QString &captured)
    {
        if (not rsb || not rating) {
            return false;
        }

        bool converted = false;
        *rating = captured.toInt(&converted);

        if (not converted) {
            if (captured == "++++") {
                *rating = 0; // unrated
            } else {
                return false;
            }
        }

        return true;
    }

    Game::Seek parseSeek(const QByteArray &token)
    {
        Game::Seek result;
        bool converted = false;

        result.valid = match_seek.exactMatch(token);
        if (not result.valid) {
            return result;
        }

        result.highlighted = false;
        result.id = match_seek.cap(14).toInt(&converted);
        result.valid = result.valid && converted;
        /*result.valid = result.valid &&*/ parseRating(&result, &result.rating, match_seek.cap(2));
        // TODO: parse game mode.
        result.player_name = match_seek.cap(1).toLatin1();
        result.time = match_seek.cap(3).toInt(&converted);
        result.valid = result.valid && converted;
        result.increment = match_seek.cap(4).toInt(&converted);
        result.valid = result.valid && converted;
        result.is_rated = (match_seek.cap(5) == "rated" || match_record.cap(5) == "r");
        result.color = Game::ColorAuto;
        const QString color(match_seek.cap(8).toLower());
        if (color == "white" || color == "w") {
            result.color = Game::ColorWhite;
        } else if (color == "black" || color == "b") {
            result.color = Game::ColorBlack;
        }

        result.is_auto_started = (match_seek.cap(9) != "m");
        result.uses_formula = (match_seek.cap(10) == "f");
        result.rating_range.first = match_seek.cap(11).toInt(&converted);
        result.rating_range.second = match_seek.cap(12).toInt(&converted);

        return result;
    }

    Game::Record parseRecord(const QByteArray &token)
    {
        Game::Record result;
        bool converted = false;

        result.valid = match_record.exactMatch(token);
        if (not result.valid) {
            return result;
        }

        result.id = match_record.cap(1).toInt(&converted);
        result.valid = result.valid && converted;
        result.valid = result.valid && parseRating(&result, &result.white.rating, match_record.cap(2));
        result.white.name = match_record.cap(3).toLatin1();
        result.valid = result.valid && parseRating(&result, &result.black.rating, match_record.cap(4));
        result.black.name = match_record.cap(5).toLatin1();
        // TODO: parse game mode.
        //result.mode = match_record.cap(6).toInt(&converted);
        result.white.time_control = match_record.cap(7).toInt(&converted) * 60;
        result.valid = result.valid && converted;
        result.black.time_control = match_record.cap(8).toInt(&converted) * 60;
        result.valid = result.valid && converted;
        result.white.clock_time = match_record.cap(9).toInt(&converted) * 60
                                + match_record.cap(10).toInt(&converted);
        result.valid = result.valid && converted;
        result.black.clock_time = match_record.cap(11).toInt(&converted) * 60
                                + match_record.cap(12).toInt(&converted);
        result.valid = result.valid && converted;
        result.white.material_strength = match_record.cap(13).toInt(&converted);
        result.valid = result.valid && converted;
        result.black.material_strength = match_record.cap(14).toInt(&converted);
        result.valid = result.valid && converted;
        result.white_to_move = match_record.cap(15) == "W";
        result.turn = match_record.cap(16).toInt(&converted);
        result.valid = result.valid && converted;

        return result;
    }

    void debugOutput(const Game::Seek s)
    {
        qDebug() << s.valid
                 << "id:" << s.id << "mode:" << s.mode << "name:" << s.player_name
                 << "rating" << s.rating << "time:" << s.time << "inc:" << s.increment
                 << "is_rated:" << s.is_rated << "white_to_start:" << s.color
                 << "is_auto_started:" << s.is_auto_started
                 << "uses_formula:" << s.uses_formula
                 << "rating range:" << s.rating_range;
    }


    void debugOutput(const Game::Record r)
    {
        qDebug() << r.valid
                 << r.id << r.white.rating << r.white.name << r.black.rating << r.black.name
                 << r.white.time_control << r.black.time_control
                 << r.white.clock_time << r.black.clock_time
                 << r.white.material_strength << r.black.material_strength
                 << r.white_to_move << r.turn;
    }
}

namespace Game { namespace Fics {

Backend::Backend(Dispatcher *dispatcher,
                 QObject *parent)
    : AbstractBackend(parent)
    , m_dispatcher(dispatcher)
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

Backend::~Backend()
{}

void Backend::setEnabled(bool enable)
{
    m_enabled = enable;

    if (not m_enabled) {
        m_channel.disconnectFromHost();
        m_channel.waitForDisconnected();

        if (m_state != StateIdle) {
            m_state = StateIdle;
            emit stateChanged(m_state);
        }
    } else if (not m_channel.isOpen()) {
        m_channel.connectToHost("freechess.org", 5000, QIODevice::ReadWrite);
        m_channel.waitForConnected();

        if (m_state != StateReady) {
            m_state = StateReady;
            emit stateChanged(m_state);
        }
    }
}

AbstractBackend::State Backend::state() const
{
    return m_state;
}

void Backend::login(const QString &username,
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

void Backend::play(uint advertisement_id)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (m_state != StateReady) {
        return;
    }

    qDebug() << "play response to" << advertisement_id;

    m_state = StatePlayPending;
    emit stateChanged(m_state);

    m_channel.write(play_command.arg(advertisement_id).toLatin1());
}

void Backend::processToken(const QByteArray &token)
{
    if (not m_enabled || token.isEmpty()) {
        return;
    }

    switch(m_state) {
    case StateLoginFailed:
    case StateLoginPending:
        processLogin(token);
        break;

    case StatePlayFailed:
    case StatePlayPending:
        processPlay(token);
        break;

    case StateIdle:
        (void) m_channel.readAll();
        break;

    case StateReady: {
        const Seek &s(parseSeek(token));
        debugOutput(s);
        if (s.valid) {
            if (Dispatcher *dispatcher = m_dispatcher.data()) {
                Command::Advertisement ac(TargetFrontend, s);
                dispatcher->sendCommand(&ac);
            }
        } else {
            const Record &r(parseRecord(token));
            if (r.valid) {
                if (Dispatcher *dispatcher = m_dispatcher.data()) {
                    Command::Record rc(TargetFrontend, r);
                    dispatcher->sendCommand(&rc);
                }
            } else {
                qDebug() << "Unknown token:" << token;
            }
        }
    } break;
    }
}

void Backend::enableTesting()
{
    m_enabled = true;
    m_state = StateReady;
}

void Backend::onReadyRead()
{
    int next_newline_pos = -1;
    const bool enable_echo = false;
    do {
        processToken(scanLine(&next_newline_pos, &m_channel, &m_buffer, enable_echo, m_extra_delimiter));
    } while (next_newline_pos != -1);
}

void Backend::processLogin(const QByteArray &line)
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

void Backend::processPlay(const QByteArray &line)
{
    static const QByteArray wait_for_accept("Issuing match request since the seek was set to manual.");
    static const QByteArray offer_accepted("accepts the match offer.");

    if (line.startsWith(wait_for_accept)) {
        // OK, the seek included a manual game start, need to wait for acceptance of offer from other player.
        // TODO: allow to cancel
    } else if (line.endsWith(offer_accepted)) {
        m_state = StateReady; // Actually, state is "in game", but not sure I want to treat that as a special case.
        emit stateChanged(m_state);
    }
}

void Backend::onHostFound()
{
    // TODO: Handle retry attempts here.
}

void Backend::abortLogin()
{
    if (m_state != StateLoginPending) {
        return;
    }

    qDebug() << "Failed to login in with as" << m_username;
    m_state = StateLoginFailed;
    emit stateChanged(m_state);
}

void Backend::configurePrompt()
{
    if (m_state != StateReady) {
        return;
    }

    m_channel.write("set style 12");
    m_channel.write("\n");
}

} // namespace Fics
} // namespace Game

