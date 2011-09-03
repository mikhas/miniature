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

#include "fics/engine.h"
#include "commands.h"
#include "linereader.h"
#include "position.h"

namespace {
    struct GameInfo {
        uint id;
        bool valid;
        QByteArray white;
        QByteArray black;
        Game::Mode mode;
    };

    struct GameUpdate
            : public Game::RecordSeekBase
    {
        // Extracted from http://www.freechess.org/Help/HelpFiles/style12.html
        enum Role {
            IsolatedPosition = -3,
            ObservingExaminedGame = -2,
            Examiner = 2,
            OpponentToMove = -1,
            PlayerToMove = 1,
            ObservingGame = 0
        };

        Game::Side white;
        Game::Side black;
        Role role;
        Game::Position position;
    };

    struct InvalidMove
    {
        bool valid;
        QByteArray move;
    };

    const char * const not_your_move("It is not your move");
    const char * const seek_not_available("That seek is not available.");

    // %1 is a placeholder the game ad id.
    const QString play_command("play %1\n");

    // Matches: 'Illegal move (Qd2).'
    const QRegExp match_illegal_move("fics% Illegal move\\s+\\(([^)]*)\\)\\.");

    // Matches: 'Press return to enter the server as "GuestZCQM":'
    const QRegExp match_confirm_login("Press return to enter the server as \"(\\w*)\"");

    // Matches: "P/g2-g4"
    const QRegExp match_move("(\\w)/(\\w\\d)-(\\w\\d)");

    // Matches: "{Game 414 (GuestKSHN vs. testonetwo) Creating unrated standard match.}"
    const QRegExp match_create_game("\\s*\\{Game\\s+(\\d+)\\s\\((\\w+)\\s+vs\\.\\s+(\\w+)\\)"
                                    "\\s+Creating\\s+(\\w+)\\s+(\\w+)\\s+match\\.\\}");

    // Matches: "GuestGZBJ (++++) seeking 15 12 unrated standard [white] m f ("play 160" to respond)"
    const QRegExp match_seek("\\s*(\\w+)\\s+\\(([0-9+]+)\\)\\s+seeking\\s+(\\d+)\\s+(\\d+)"
                             "\\s+(\\w+)\\s+(\\w+)\\s+(\\[(\\w+)\\])?\\s*(m|a)?\\s*(f)?"
                             "\\s*(\\[(\\d+)\\-(\\d+)\\])?\\s*\\(\"play\\s+(\\d+)\" to respond\\)");

    // Matches: "92 2370 playerABC     2383 playerDEF  [ br  5   5]   2:22 -  3:17 (18-18) W: 42"
    const QRegExp match_record("\\s*(\\d+)\\s+([0-9+]+)\\s+(\\w+)\\s+([0-9+]+)\\s+(\\w+)"
                               "\\s+\\[\\s*(\\w{1,3})\\s*(\\d+)\\s+(\\d+)\\s*\\]"
                               "\\s+(\\d+):(\\d+)\\s+\\-\\s+(\\d+):(\\d+)\\s+"
                               "\\((\\d+)\\-(\\d+)\\)\\s+(\\w):\\s+(\\d+)");

    QString fromColor(Game::Color c)
    {
        switch (c) {
        case Game::ColorWhite: return "white";
        case Game::ColorBlack: return "black";
        default: return "";
        }

        return "";
    }

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

        result.highlighted = false;
        result.valid = match_seek.exactMatch(token);

        if (not result.valid) {
            return result;
        }

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
        result.color = Game::ColorNone;
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
        // TODO: Parse remaining_time
        result.time = match_record.cap(7).toInt(&converted) * 60;
        result.valid = result.valid && converted;
        result.increment = match_record.cap(9).toInt(&converted) * 60
                            + match_record.cap(10).toInt(&converted);
        result.valid = result.valid && converted;
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

    GameInfo parseCreateGame(const QByteArray &token)
    {
        GameInfo result;
        bool converted = false;

        result.valid = match_create_game.exactMatch(token);
        if (not result.valid) {
            return result;
        }

        // FIXME: Is it really local/remote, or white/black?
        result.id = match_create_game.cap(1).toUInt(&converted);
        result.valid = result.valid && converted;
        result.white = match_create_game.cap(2).toLatin1();
        result.black = match_create_game.cap(3).toLatin1();

        return result;
    }

    GameUpdate parseGameUpdate(const QByteArray &token)
    {
        // Need to parse
        // <12> rnbqkbnr pppppppp -------- -------- ------P- -------- PPPPPP-P RNBQKBNR B 6 1 1 1 1 0 414 GuestKSHN testonetwo -1 999 999 39 39 36000 36000 1 P/g2-g4 (0:00) g4 0 0 0
        // 0    1        2        3        4        5        6        7        8        9             16  17        18         19 20  21  22 23 24    25      27      28     29

        GameUpdate result;
        bool converted = false;

        const QList<QByteArray> &cols(token.split(' '));
        result.valid = (cols.size() == 33 && cols.at(0) == "<12>");

        if (not result.valid) {
            return result;
        }

        result.mode = Game::ModeNone; // n/a
        result.white.rating = 0; // n/a
        result.black.rating = 0; // n/a

        result.id = cols.at(16).toUInt(&converted);
        result.valid = result.valid && converted;

        result.time = cols.at(20).toUInt(&converted);
        result.valid = result.valid && converted;

        result.increment = cols.at(21).toUInt(&converted);
        result.valid = result.valid && converted;

        result.white.name = cols.at(17);
        result.black.name = cols.at(18);

        result.role = static_cast<GameUpdate::Role>(cols.at(19).toInt(&converted));
        result.valid = result.valid && converted;

        result.valid = result.valid && match_move.exactMatch(cols.at(27));

        int dpp_file = cols.at(19).toInt(&converted);
        result.valid = result.valid && converted;

        Game::Position &pos = result.position;
        pos.setDoublePawnPush(dpp_file == -1 ? Game::FileCount : static_cast<Game::File>(dpp_file));
        pos.setNextToMove(cols.at(9) == "W" ? Game::ColorWhite : Game::ColorBlack);

        // The move notation is always capital and does not encode the color of the piece ...
        Game::Piece p(Game::toPiece(match_move.cap(1).toLatin1().at(0),
                                    pos.nextToMove() == Game::ColorWhite ? Game::ColorBlack
                                                                         : Game::ColorWhite));
        p.setSquare(Game::toSquare(match_move.cap(3).toLatin1()));
        pos.setMovedPiece(Game::MovedPiece(p, Game::toSquare(match_move.cap(2).toLatin1())));

        Game::Position::CastlingFlags flags;

        if (cols.at(10).toInt() == 1) {
            flags |= Game::Position::CanWhiteCastleShort;
        }

        if (cols.at(11).toInt() == 1) {
            flags |= Game::Position::CanWhiteCastleLong;
        }

        if (cols.at(12).toInt() == 1) {
            flags |= Game::Position::CanBlackCastleShort;
        }

        if (cols.at(13).toInt() == 1) {
            flags |= Game::Position::CanBlackCastleLong;
        }

        pos.setCastlingFlags(flags);

        for (uint j = 0; j < Game::RankCount; ++j) {
            const QByteArray &row(cols.at(1 + j));
            for (uint i = 0; i < static_cast<uint>(row.size()); ++i) {
                Game::Piece p(Game::toPiece(row.at(i)));
                if (p.type() != Game::Piece::None) {
                    p.setSquare(Game::toSquare(i, j));
                    pos.addPiece(p);
                }
            }
        }

        return result;
    }

    InvalidMove parseInvalidMove(const QByteArray &token)
    {
        InvalidMove im;

        if (token.contains(not_your_move)) {
            im.valid = true;
            return im;
        }

        im.valid = match_illegal_move.exactMatch(token);
        im.move = match_illegal_move.cap(1).toLatin1();

        return im;
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
                 << r.white.material_strength << r.black.material_strength
                 << r.time << r.increment
                 << r.white_to_move << r.turn;
    }

    void debugOutput(const GameInfo gi)
    {
        qDebug() << gi.valid
                 << gi.id << gi.white << gi.black << gi.mode;
    }
}

namespace Game { namespace Fics {

Engine::Engine(Dispatcher *dispatcher,
               QObject *parent)
    : AbstractEngine(parent)
    , m_dispatcher(dispatcher)
    , m_channel()
    , m_buffer()
    , m_username()
    , m_password()
    , m_filter(None)
    , m_enabled(false)
    , m_login_abort_timer()
    , m_extra_delimiter()
    , m_current_game_id(0)
    , m_current_advertisment_id(0)
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

Engine::~Engine()
{}

void Engine::setEnabled(bool enable)
{
    m_enabled = enable;

    if (not m_enabled) {
        m_channel.disconnectFromHost();
        m_channel.waitForDisconnected();
    } else if (not m_channel.isOpen()) {
        m_channel.connectToHost("freechess.org", 5000, QIODevice::ReadWrite);
        m_channel.waitForConnected();
    }
}

void Engine::login(const QString &username,
                   const QString &password)
{
    m_filter |= LoginRequest;
    m_login_abort_timer.start();

    m_username = username;
    m_password = password;

    m_channel.write(m_username.toLatin1());
    m_channel.write("\n");

    m_extra_delimiter.clear();
    m_extra_delimiter.append(':');
    m_extra_delimiter.append('%');
}

void Engine::seek(uint time,
                  uint increment,
                  Rating rating,
                  Color color)
{
    // Can only send out seeks when also waiting for them:
    if (not (m_filter & WaitingForSeeks)) {
        return;
    }

    QString r;
    if (rating == RatingEnabled) {
        r = "rated";
    } else if (rating == RatingDisabled) {
        r = "unrated";
    }

    m_filter |= PlayRequest;

    const QString &cmd(QString("seek %1 %2 %3 %4\n")
                       .arg(time)
                       .arg(increment)
                       .arg(r)
                       .arg(fromColor(color)));
    m_channel.write(cmd.toLatin1());
}

void Engine::play(uint advertisement_id)
{
    m_current_advertisment_id = advertisement_id;
    m_filter |= PlayRequest;
    m_channel.write(play_command.arg(advertisement_id).toLatin1());
}

void Engine::movePiece(const MovedPiece &moved_piece)
{
    m_channel.write(moveNotation(moved_piece).toLatin1());
    m_channel.write("\n");
}

void Engine::processToken(const QByteArray &token)
{
    if (not m_enabled || token.isEmpty()) {
        return;
    }

    qDebug() << "FICS:" << token;

    if (m_filter & LoginRequest) {
        processLogin(token);
    }

    if (m_filter & PlayRequest) {
        const GameInfo &gi(parseCreateGame(token));
        debugOutput(gi);
        if (gi.valid) {
            m_current_game_id = gi.id;
            const QByteArray &local_side(gi.white == m_username ? gi.white : gi.black);
            const QByteArray &remote_side(gi.white == local_side ? gi.black : gi.white);

            Command::CreateGame cg(TargetRegistry, gi.id, local_side, remote_side,
                                   (local_side == gi.white ? LocalSideIsWhite : LocalSideIsBlack));
            sendCommand(&cg);

            Command::Move m(TargetFrontend, gi.id, createStartPosition());
            sendCommand(&m);

            m_filter |= InGame;
            m_filter &= ~WaitingForSeeks;
            m_filter &= ~PlayRequest;
        } else if (token.contains(seek_not_available)) {
            Command::InvalidSeek is(TargetFrontend, m_current_advertisment_id);
            sendCommand(&is);

            m_filter |= WaitingForSeeks;
            m_filter &= ~PlayRequest;
        }
    }

    if (m_filter & InGame) {
        const GameUpdate &gu(parseGameUpdate(token));
        if (gu.valid) {
            Command::Move m(TargetFrontend, gu.id, gu.position);
            m.setWhite(gu.white);
            m.setBlack(gu.black);
            sendCommand(&m);
        } else {
            const InvalidMove &im(parseInvalidMove(token));
            if (im.valid) {
                Command::InvalidMove imc(TargetFrontend, m_current_game_id, im.move);
                sendCommand(&imc);
            }
        }
    }

    if (m_filter & WaitingForSeeks) {
        const Seek &s(parseSeek(token));
        if (s.valid) {
            Command::Advertisement ac(TargetFrontend, s);
            sendCommand(&ac);
        }
    }

    if (m_filter & WaitingForGames) {
        const Record &r(parseRecord(token));
        if (r.valid) {
            Command::Record rc(TargetFrontend, r);
            sendCommand(&rc);
        }
    }

    if (m_filter == None) {
        (void) m_channel.readAll();
    }
}

void Engine::setMessageFilter(const MessageFilterFlags &flags)
{
    m_enabled = true;
    m_filter = flags;
}

void Engine::onReadyRead()
{
    int next_newline_pos = -1;
    const bool enable_echo = false;
    do {
        processToken(scanLine(&next_newline_pos, &m_channel, &m_buffer, enable_echo, m_extra_delimiter));
    } while (next_newline_pos != -1);
}

void Engine::processLogin(const QByteArray &line)
{
    // TODO: write proper tokenizer?
    static const QByteArray enter_password("password");
    static const QByteArray fics_prompt("fics");

    if (match_confirm_login.exactMatch(line)) {
        m_login_abort_timer.stop();
        m_login_abort_timer.start();
        // Confirm login:
        m_channel.write("\n");
        configurePrompt();

        if (m_username == "guest") {
            m_username = match_confirm_login.cap(1);
        }

        Command::Login lc(TargetFrontend, m_username, "");
        sendCommand(&lc);
    } else if (line.startsWith(enter_password)) {
        m_login_abort_timer.stop();
        m_login_abort_timer.start();
        m_channel.write(m_password.toLatin1());
        m_channel.write("\n");
    } else if (line.startsWith(fics_prompt)) {
        m_login_abort_timer.stop();
        m_extra_delimiter.clear();
        m_filter |= WaitingForSeeks;
    }
}

void Engine::onHostFound()
{
    // TODO: Handle retry attempts here.
}

void Engine::abortLogin()
{
    if (not (m_filter & LoginRequest)) {
        return;
    }

    qDebug() << "Failed to login in with as" << m_username;
    m_filter &= ~LoginRequest;
}

void Engine::configurePrompt()
{
    m_channel.write("set style 12");
    m_channel.write("\n");
}

void Engine::sendCommand(AbstractCommand *command)
{
    if (Dispatcher *dispatcher = m_dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Fics
} // namespace Game

