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

#define CAP_RATING_BRACKETS "\\(\\s*([0-9+]+)\\)"

namespace {
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

    struct GameEnded
    {
        bool valid;
        uint id;
        Game::Result result;
        Game::Reason reason;
        QByteArray player_name;
    };

    struct Message
    {
        bool valid;
        uint game_id;
        QByteArray player_name;
        QByteArray data;
    };

    const char * const login_prompt("login");
    const char * const password_prompt("password");
    const char * const fics_prompt("fics");
    const char * const not_your_move("It is not your move");
    const char * const seek_not_available("That seek is not available.");

    // %1 is a placeholder the game ad id.
    const QString play_command("play %1\n");

    // Matches: '{Game 266 (gzest vs. GuestJTXB) GuestJTXB forfeits by disconnection} 1-0'
    const QRegExp match_forfeit_by_disconnect("\\s*\\{Game\\s+(\\d+)\\s+\\(\\w+\\s+vs\\.\\s+\\w+\\)"
                                              "\\s+(\\w+)\\s+forfeits by disconnection\\}\\s+([012/-]+)");

    // Matches: '{Game 53 (QuimGil vs. MiniatureTest) QuimGil lost connection and too few moves; game aborted} *'
    const QRegExp match_aborted_by_disconnect("\\s*\\{Game\\s+(\\d+)\\s+\\(\\w+\\s+vs\\.\\s+\\w+\\)"
                                              "\\s+(\\w+)\\s+lost connection and too few moves; game aborted\\}\\s+\\*");

    // Matches: '{Game 506 (QuimGil vs. MiniatureTest) QuimGil lost connection; game adjourned} *'
    const QRegExp match_adjourned_by_disconnect("\\s*\\{Game\\s+(\\d+)\\s+\\(\\w+\\s+vs\\.\\s+\\w+\\)"
                                              "\\s+(\\w+)\\s+lost connection; game adjourned\\}\\s+\\*");

    // Matches: '{Game 328 (GuestNDFD vs. pleaseIgnoreMe) GuestNDFD checkmated} 0-1'
    // TODO: should also match draws.
    const QRegExp match_game_ended("\\s*\\{Game\\s+(\\d+)\\s+\\(\\w+\\s+vs\\.\\s+\\w+\\)"
                                   "\\s+(.*)\\}(\\s+(.*))?");
    
    // Matches: 'Illegal move (Qd2).'
    const QRegExp match_illegal_move("fics% Illegal move\\s+\\(([^)]*)\\)\\.");

    // Matches: 'Press return to enter the server as "GuestZCQM":'
    const QRegExp match_confirm_login("Press return to enter the server as \"(\\w*)\"");

    // Matches: "P/g2-g4" and "P/h7-g8=Q"
    const QRegExp match_move("(\\w)/(\\w\\d)-(\\w\\d)(=(\\w))?");

    // Matches: "o-o", "O-O", "0-0"
    const QRegExp match_short_castling("(o|O|0)-(o|O|0)");

    // Matches: "o-o-o", "O-O-O", "0-0-0"
    const QRegExp match_long_castling("(o|O|0)-(o|O|0)-(o|O|0)");

    // Matches: "Creating: ignoreme (++++) dontjoin (++++) unrated standard 999 999"
    const QRegExp match_creating_game("\\s*Creating:"
                                      "\\s+(\\w+)\\s+" CAP_RATING_BRACKETS
                                      "\\s+(\\w+)\\s+" CAP_RATING_BRACKETS
                                      "\\s+(\\w+)\\s+(\\w+)\\s+(\\d+)\\s+(\\d+)");

    // Matches: "{Game 414 (GuestKSHN vs. testonetwo) Creating unrated standard match.}"
    const QRegExp match_game_created("\\s*\\{Game\\s+(\\d+)\\s\\((\\w+)\\s+vs\\.\\s+(\\w+)\\)"
                                    "\\s+Creating\\s+(\\w+)\\s+(\\w+)\\s+match\\.\\}");

    // Matches: "GuestGZBJ (++++) seeking 15 12 unrated standard [white] m f ("play 160" to respond)"
    const QRegExp match_seek("\\s*(\\w+)\\s+" CAP_RATING_BRACKETS
                             "\\s+seeking\\s+(\\d+)\\s+(\\d+)"
                             "\\s+(\\w+)\\s+(\\w+)\\s+(\\[(\\w+)\\])?\\s*(m|a)?\\s*(f)?"
                             "\\s*(\\[(\\d+)\\-(\\d+)\\])?\\s*\\(\"play\\s+(\\d+)\" to respond\\)");

    // Matches: "92 2370 playerABC     2383 playerDEF  [ br  5   5]   2:22 -  3:17 (18-18) W: 42"
    const QRegExp match_record("\\s*(\\d+)\\s+([0-9+]+)\\s+(\\w+)\\s+([0-9+]+)\\s+(\\w+)"
                               "\\s+\\[\\s*(\\w{1,3})\\s*(\\d+)\\s+(\\d+)\\s*\\]"
                               "\\s+(\\d+):(\\d+)\\s+\\-\\s+(\\d+):(\\d+)\\s+"
                               "\\((\\d+)\\-(\\d+)\\)\\s+(\\w):\\s+(\\d+)");

    // Matches "testPLEASEIGNORE declines the match offer."
    const QRegExp match_declines_match_offer("\\s*(\\w+)\\s+declines the match offer.");

    // Matches "testPLEASEIGNORE, whom you were challenging, has departed."
    const QRegExp match_has_departed("\\s*(\\w+),\\s+whom you were challenging, has departed.");

    // Matches "GuestBVNP offers you a draw."
    const QRegExp match_draw_proposed("\\s*(\\w+)\\s+offers you a draw.");

    // Matches "testPleaseIgnore declines the draw request."
    const QRegExp match_draw_rejected("\\s*(\\w+)\\s+declines the draw request.");

    // Matches "MiniatureTest[448] says: hi" and "GuestXYZ(U)[123] says: hi"
    const QRegExp match_chat_message("\\s*(\\w+)(\\(U\\))?\\[(\\d+)\\]\\s+says:\\s+(.*)");

    // Matches: "\    this chatline got wrapped"
    const QRegExp match_chat_message_wrapped("\\\\\\s+(.*)");

    QString fromColor(Game::Color c)
    {
        switch (c) {
        case Game::ColorWhite: return "white";
        case Game::ColorBlack: return "black";
        default: return "";
        }

        return "";
    }

    ::Game::Result toGameResult(const QString &token)
    {
        if (token == "1-0") {
            return Game::ResultWhiteWins;
        } else if (token == "1/2-1/2") {
            return Game::ResultDraw;
        } else if (token == "0-1") {
            return Game::ResultBlackWins;
        }

        return ::Game::ResultUnknown;
    }

    bool parseRating(uint *rating,
                     const QString &captured)
    {
        if (not rating) {
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
        /*result.valid = result.valid &&*/ parseRating(&result.rating, match_seek.cap(2));
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
        result.valid = result.valid && parseRating(&result.white.rating, match_record.cap(2));
        result.white.name = match_record.cap(3).toLatin1();
        result.valid = result.valid && parseRating(&result.black.rating, match_record.cap(4));
        result.black.name = match_record.cap(5).toLatin1();
        // TODO: parse game mode.
        //result.mode = match_record.cap(6).toInt(&converted);
        // TODO: Parse remaining_time
        result.time = match_record.cap(7).toInt(&converted);
        result.valid = result.valid && converted;
        result.increment = match_record.cap(9).toInt(&converted)
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

    bool parseCreatingGame(const QByteArray &token,
                           Game::GameInfo *result)
    {
        const QRegExp &re(match_creating_game);
        if (not result || not re.exactMatch(token)) {
            return false;
        }

        result->valid = true;
        bool converted = false;

        result->white.name = re.cap(1).toLatin1();
        result->valid = result->valid && parseRating(&result->white.rating, re.cap(2));

        result->black.name = re.cap(3).toLatin1();
        result->valid = result->valid && parseRating(&result->black.rating, re.cap(4));

        result->rating = (re.cap(5) == "rated" ? Game::RatingEnabled
                                               : Game::RatingDisabled);

        const QString &mode = re.cap(6);
        if (mode == "lightning") {
            result->mode = Game::ModeLightning;
        } else if (mode == "blitz") {
            result->mode = Game::ModeBlitz;
        } else if (mode == "standard") {
            result->mode = Game::ModeStandard;
        } else {
            result->mode = Game::ModeNone; // Unknown game mode.
        }

        result->time = re.cap(7).toUInt(&converted) * 60; // reported in minutes here
        result->valid = result->valid && converted;

        result->time_increment = re.cap(8).toUInt(&converted);
        result->valid = result->valid && converted;

        result->white.remaining_time = result->time;
        result->black.remaining_time = result->time;

        return result->valid;
    }

    bool parseGameCreated(const QByteArray &token,
                         Game::GameInfo *result)
    {
        if (not result || not match_game_created.exactMatch(token)) {
            return false;
        }

        result->valid = true;
        bool converted = false;

        // FIXME: Is it really local/remote, or white/black?
        result->id = match_game_created.cap(1).toUInt(&converted);
        result->valid = result->valid && converted;
        result->white.name = match_game_created.cap(2).toLatin1();
        result->black.name = match_game_created.cap(3).toLatin1();

        return result->valid;
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

        result.white.material_strength = cols.at(22).toUInt(&converted);
        result.valid = result.valid && converted;

        result.black.material_strength = cols.at(23).toUInt(&converted);
        result.valid = result.valid && converted;

        result.white.remaining_time = cols.at(24).toUInt(&converted);
        result.valid = result.valid && converted;

        result.black.remaining_time = cols.at(25).toUInt(&converted);
        result.valid = result.valid && converted;

        result.white.name = cols.at(17);
        result.black.name = cols.at(18);

        result.role = static_cast<GameUpdate::Role>(cols.at(19).toInt(&converted));
        result.valid = result.valid && converted;

        Game::Position &pos = result.position;
        pos.setNextToMove(cols.at(9) == "W" ? Game::ColorWhite : Game::ColorBlack);

        int dpp_file = cols.at(19).toInt(&converted);
        result.valid = result.valid && converted;

        pos.setDoublePawnPush(dpp_file == -1 ? Game::FileCount : static_cast<Game::File>(dpp_file));

        const Game::Color moved_color(pos.nextToMove() == Game::ColorWhite ? Game::ColorBlack
                                                                           : Game::ColorWhite);

        const bool regular_move(match_move.exactMatch(cols.at(27)));
        if (not regular_move) {
            // Normalize castlings into king moves:
            const Game::Square king_origin(Game::toSquare(moved_color == Game::ColorWhite ? "e1" : "e8"));
            const Game::Square king_target_short(Game::toSquare(moved_color == Game::ColorWhite ? "g1" : "g8"));
            const Game::Square king_target_long(Game::toSquare(moved_color == Game::ColorWhite ? "c1" : "c8"));

            if (match_short_castling.exactMatch(cols.at(27))) {
                Game::Piece p(Game::Piece::King, moved_color, king_target_short);
                pos.setMovedPiece(Game::MovedPiece(p, king_origin));
            } else if (match_long_castling.exactMatch(cols.at(27))) {
                Game::Piece p(Game::Piece::King, moved_color, king_target_long);
                pos.setMovedPiece(Game::MovedPiece(p, king_origin));
            } else {
                result.valid = false;
            }
        } else {
            // The move notation is always (except for castlings) capital and
            // does not encode the color of the piece ...
            const QString &piece_encoding(match_move.cap(5).isEmpty() ? match_move.cap(1) : match_move.cap(5));
            Game::Piece p(Game::toPiece(piece_encoding.toLatin1().at(0),
                                        pos.nextToMove() == Game::ColorWhite ? Game::ColorBlack
                                                                             : Game::ColorWhite));

            p.setSquare(Game::toSquare(match_move.cap(3).toLatin1()));
            pos.setMovedPiece(Game::MovedPiece(p, Game::toSquare(match_move.cap(2).toLatin1())));
        }

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

    // TODO: also parse other instances that can lead to aborted games (currently only disconnect).
    // TODO: cleanup code duplication
    GameEnded parseGameEnded(const QByteArray &token)
    {
        GameEnded result;
        result.valid = false;
        result.reason = Game::ReasonUnknown;
        result.result = Game::ResultUnknown;

        if (match_forfeit_by_disconnect.exactMatch(token)) {
            result.valid = true;
            result.reason = Game::ReasonForfeitByDisconnect;

            bool converted = false;
            result.id = match_forfeit_by_disconnect.cap(1).toUInt(&converted);
            result.valid = result.valid && converted;
            result.player_name = match_forfeit_by_disconnect.cap(2).toLatin1();
            result.result = toGameResult(match_forfeit_by_disconnect.cap(3));
            result.valid = result.valid && (result.result != ::Game::ResultUnknown);
        } else if (match_aborted_by_disconnect.exactMatch(token)) {
            result.valid = true;
            result.reason = Game::ReasonAbortedByDisconnect;
            result.result = Game::ResultUnknown;

            bool converted = false;
            result.id = match_aborted_by_disconnect.cap(1).toUInt(&converted);
            result.valid = result.valid && converted;
            result.player_name = match_aborted_by_disconnect.cap(2).toLatin1();
        } else if (match_adjourned_by_disconnect.exactMatch(token)) {
            result.valid = true;
            result.reason = Game::ReasonAdjournedByDisconnect;
            result.result = Game::ResultAdjourned;

            bool converted = false;
            result.id = match_adjourned_by_disconnect.cap(1).toUInt(&converted);
            result.valid = result.valid && converted;
            result.player_name = match_adjourned_by_disconnect.cap(2).toLatin1();
        } else if(match_game_ended.exactMatch(token)) {
            result.valid = true;

            const QString &desc = match_game_ended.cap(2);
            if (desc.endsWith("checkmated")) {
                result.reason = Game::ReasonCheckmated;
            } else if (desc.endsWith("forfeits on time")) {
                result.reason = Game::ReasonForfeitOnTime;
            } else if (desc == "Game drawn because both players ran out of time") {
                result.reason = Game::ReasonDrawnOnTime;
            } else if (desc == "Game drawn by mutual agreement") {
                result.reason = Game::ReasonDrawAccpeted;
            } else {
                result.reason = Game::ReasonUnknown;
            }

            bool converted = false;
            result.id = match_game_ended.cap(1).toUInt(&converted);
            result.valid = result.valid && converted;
            result.player_name = match_game_ended.cap(2).toLatin1();
            result.result = toGameResult(match_game_ended.cap(4));
            result.valid = result.valid && (result.result != ::Game::ResultUnknown);
        } else {
            result.valid = false;
            result.reason = Game::ReasonUnknown;
            result.id = 0; // invalid
        }

        return result;
    }

    Message parseMessage(const QByteArray &token)
    {
        const QRegExp &re(match_chat_message);
        Message result;

        result.valid = re.exactMatch(token);
        if (not result.valid) {
            return result;
        }

        bool converted = false;
        result.game_id = re.cap(3).toUInt(&converted);
        result.valid = result.valid && converted;

        result.player_name = re.cap(1).toLatin1();
        result.data = re.cap(4).toLatin1();

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
                 << r.white.material_strength << r.black.material_strength
                 << r.time << r.increment
                 << r.white_to_move << r.turn;
    }

    void debugOutput(const Game::GameInfo &gi)
    {
        qDebug() << gi.valid
                 << gi.id << gi.white.name << gi.black.name << gi.mode;
    }
}

namespace Game { namespace Fics {

Engine::Engine(QObject *parent)
    : AbstractEngine(parent)
    , m_dispatcher()
    , m_channel()
    , m_buffer()
    , m_last_token()
    , m_username()
    , m_password()
    , m_filter(None)
    , m_channel_enabled(true)
    , m_logged_in(false)
    , m_past_welcome_screen(false)
    , m_game_resolution_pending(false)
    , m_login_count(0)
    , m_login_abort_timer()
    , m_extra_delimiter()
    , m_current_advertisment_id(0)
    , m_current_game()
{
    m_login_abort_timer.setSingleShot(true);
    m_login_abort_timer.setInterval(10000);

    connect(&m_login_abort_timer, SIGNAL(timeout()),
            this,                 SLOT(abortLogin()));

    connect(&m_channel, SIGNAL(readyRead()),
            this,       SLOT(onReadyRead()));

    connect(&m_channel, SIGNAL(hostFound()),
            this,       SLOT(onHostFound()));

    // First pings whether connection was recovered, then attempts relogin + resume.
    connect (&m_channel, SIGNAL(disconnected()),
             this,       SLOT(reconnect()));
}

Engine::~Engine()
{}

void Engine::setDispatcher(Dispatcher *dispatcher)
{
    m_dispatcher = WeakDispatcher(dispatcher);
}

void Engine::login(const QString &username,
                   const QString &password)
{
    if (m_logged_in) {
        return;
    }

    // Prevent FICS from tripping over trailing whitespace:
    m_username = username.trimmed();
    m_password = password.trimmed();

    m_extra_delimiter.clear();
    m_extra_delimiter.append(':');
    m_extra_delimiter.append('%');

    m_filter |= LoginRequest;

    if (0 == strcmp(m_last_token.data(), login_prompt)) {
        // A previous login attempt must have failed and FICS is showing us a
        // login prompt already:
        sendLogin();
    } else if (m_channel.state() != QAbstractSocket::ConnectedState) {
        m_login_count = 0;
        m_buffer.clear();

        if (m_channel_enabled) {
            m_channel.connectToHost("freechess.org", 5000, QIODevice::ReadWrite);
        }
    }
}

void Engine::logout()
{
    m_channel.close();
    m_logged_in = false;
    m_past_welcome_screen = false;

    Command::GameEnded gec(TargetFrontend, m_current_game.id,
                           ResultUnknown, ReasonUnknown, m_username.toLatin1());
    sendCommand(&gec);

    m_current_game = GameInfo();
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
    writeData(cmd.toLatin1());
}

void Engine::play(uint advertisement_id)
{
    m_current_advertisment_id = advertisement_id;
    m_filter |= PlayRequest;
    writeData(play_command.arg(advertisement_id).toLatin1());
}

void Engine::endGame(Reason reason)
{
    switch (reason) {
    case ReasonUnknown:
    case ReasonSurrendered:
        writeData("resign\n");

        m_current_game = GameInfo();
        m_filter |= WaitingForSeeks;
        m_filter &= ~InGame;
        break;

    default:
        break;
    }
}

void Engine::proposeGameResolution(Resolution resolution)
{

    switch(resolution) {
    case ResolutionDraw:
        m_game_resolution_pending = true;
        writeData("draw\n");
        break;

    case ResolutionAdjourn:
        m_game_resolution_pending = true;
        writeData("adjourn\n");
        break;

    case ResolutionAbort:
        m_game_resolution_pending = true;
        writeData("abort\n");
        break;
    }
}

void Engine::acceptGameResolution(Resolution)
{
    if (not m_game_resolution_pending) {
        return;
    }

    m_game_resolution_pending = false;
    writeData("accept\n");
}

void Engine::rejectGameResolution(Resolution)
{
    if (not m_game_resolution_pending) {
        return;
    }

    m_game_resolution_pending = false;
    writeData("decline\n");
}

void Engine::movePiece(const MovedPiece &moved_piece)
{
    writeData(moveNotation(moved_piece).toLatin1());
    writeData("\n");
}

// Do not return early from this method, toggle tokenProcessed flag instead:
void Engine::readData(const QByteArray &token)
{
    bool tokenProcessed = false;

    if (m_filter & LoginRequest) {
        processLogin(token);

        // There's no point in trying anything else at this point:
        tokenProcessed = true;
    }

    if (not tokenProcessed
        && (m_filter & WaitingForWrappedChatMessage)) {
        m_filter &= ~WaitingForWrappedChatMessage;
        if (match_chat_message_wrapped.exactMatch(token)) {
            const Message &last_msg(parseMessage(m_last_token));
            if (last_msg.valid) {
                if (last_msg.game_id != m_current_game.id) {
                        qWarning() << __PRETTY_FUNCTION__
                                   << "Received message from opponent, but game id's don't match:"
                                   << last_msg.game_id << "but expected" << m_current_game.id;
                } else {
                    Command::Message mc(TargetFrontend, last_msg.player_name,
                                        match_chat_message_wrapped.cap(1).toLatin1().trimmed());
                    sendCommand(&mc);
                    m_filter |= WaitingForWrappedChatMessage;

                    tokenProcessed = true;
                }
            }
        }
    }

    if (not tokenProcessed
        && (m_filter & PlayRequest)) {
        if (parseCreatingGame(token, &m_current_game)
            && m_current_game.valid) {
            // Wait for second part of message (match_game_created)
        } else  if (parseGameCreated(token, &m_current_game)
            && m_current_game.valid) {
            Command::CreateGame cg(TargetRegistry, m_current_game,
                                   (m_current_game.white.name.toLower() == m_username.toLower() ? LocalSideIsWhite
                                                                                                : LocalSideIsBlack));
            sendCommand(&cg);

            Command::Move m(TargetFrontend, m_current_game.id, createStartPosition());
            m.setWhite(m_current_game.white);
            m.setBlack(m_current_game.black);
            sendCommand(&m);

            m_filter |= InGame;
            m_filter &= ~WaitingForSeeks;
            m_filter &= ~PlayRequest;

            tokenProcessed = true;
        } else if (token.contains(seek_not_available)
              || match_declines_match_offer.exactMatch(token)
              || match_has_departed.exactMatch(token)) {
            Command::InvalidSeek is(TargetFrontend, m_current_advertisment_id);
            sendCommand(&is);

            m_filter |= WaitingForSeeks;
            m_filter &= ~PlayRequest;

            tokenProcessed = true;
        }
    }

    if (not tokenProcessed
        && (m_filter & InGame)) {
        GameUpdate gu(parseGameUpdate(token));
        if (gu.valid) {
            if (gu.id == m_current_game.id) {
                // FICS only sends rating at the beginning of a game, but we
                // decided that rating is part of a side's info. To keep things
                // easy, we just always send rating, too:
                gu.white.rating = m_current_game.white.rating;
                gu.black.rating = m_current_game.black.rating;
            }

            Command::Move m(TargetFrontend, gu.id, gu.position);
            m.setWhite(gu.white);
            m.setBlack(gu.black);
            sendCommand(&m);

            tokenProcessed = true;
        } else {
            const InvalidMove &im(parseInvalidMove(token));
            if (im.valid) {
                Command::InvalidMove imc(TargetFrontend, m_current_game.id, im.move);
                sendCommand(&imc);

                tokenProcessed = true;
            } else {
                const GameEnded &ge(parseGameEnded(token));
                if (ge.valid) {
                    if (ge.id != m_current_game.id) {
                        qWarning() << __PRETTY_FUNCTION__
                                   << "Received aborted-game message, but id's don't match:"
                                   << ge.id << "but expected" << m_current_game.id;
                    } else {
                        Command::GameEnded gec(TargetFrontend, m_current_game.id,
                                               ge.result, ge.reason, ge.player_name);
                        sendCommand(&gec);

                        m_current_game = GameInfo();
                        m_filter |= WaitingForSeeks;
                        m_filter &= ~InGame;

                        tokenProcessed = true;
                    }
                } else {
                    const Message &msg(parseMessage(token));
                    if (msg.valid) {
                        if (msg.game_id != m_current_game.id) {
                            qWarning() << __PRETTY_FUNCTION__
                                       << "Received message from opponent, but game id's don't match:"
                                       << msg.game_id << "but expected" << m_current_game.id;
                        } else {
                            Command::Message mc(TargetFrontend, msg.player_name, msg.data);
                            sendCommand(&mc);
                            m_filter |= WaitingForWrappedChatMessage;

                            tokenProcessed = true;
                        }
                    } else if (match_draw_proposed.exactMatch(token)) {
                        m_game_resolution_pending = true;
                        Command::GameResolution grc(TargetFrontend,
                                                    Command::GameResolution::Proposed,
                                                    ResolutionDraw);
                        sendCommand(&grc);
                    } else if (match_draw_rejected.exactMatch(token)) {
                        m_game_resolution_pending = false;
                        Command::GameResolution grc(TargetFrontend,
                                                    Command::GameResolution::Rejected,
                                                    ResolutionDraw);
                        sendCommand(&grc);
                    }
                }
            }
        }
    }

    if (not tokenProcessed
        && (m_filter & WaitingForSeeks)) {
        const Seek &s(parseSeek(token));
        if (s.valid) {
            Command::Advertisement ac(TargetFrontend, s);
            sendCommand(&ac);

            tokenProcessed = true;
        }
    }

    // Last token *needs* to be stored here, as we can only use this method in test:
    m_last_token = token;
}

void Engine::sendMessage(const QByteArray &,
                         const QByteArray &message)
{
    if (not m_filter & InGame) {
        return;
    }

    writeData("say ");
    writeData(message);
    writeData("\n");
}

void Engine::setMessageFilter(const MessageFilterFlags &flags)
{
    m_filter = flags;
}

void Engine::setChannelEnabled(bool enabled)
{
    m_channel_enabled = enabled;
}

void Engine::writeData(const QByteArray &data)
{
    m_channel.write(data);
}

void Engine::onReadyRead()
{
    const bool enable_echo = false;
    const bool has_dispatcher = not m_dispatcher.isNull();
    int next_newline_pos = -1;

    do {
        const QByteArray &token(scanLine(&next_newline_pos, &m_channel, &m_buffer, enable_echo, m_extra_delimiter));
        if (has_dispatcher && not token.isEmpty()) {
            qDebug() << "FICS:" << token;
            readData(token);
        }
    } while (next_newline_pos != -1);
}

void Engine::processLogin(const QByteArray &line)
{
    // TODO: write dedicated login class?
    if (line.startsWith(login_prompt)) {
        m_login_abort_timer.stop(); // TODO: find a less racy way.
        // Actually, login count will alternate between 0 and 1 ...
        if (m_login_count > 0) {
            Command::LoginFailed lf(TargetFrontend);
            sendCommand(&lf);
        } else {
            sendLogin();
        }

        ++m_login_count;
    } else if (line.startsWith(password_prompt)) {
        m_login_abort_timer.stop();
        m_login_abort_timer.start();
        writeData(m_password.toLatin1());
        writeData("\n");
    } else if (match_confirm_login.exactMatch(line)) {
        m_login_abort_timer.stop();

        // Was it an attempt to login as a registered user?
        if (not m_password.isEmpty()) {
            // FICS does not allow us to recover from this, we have to reset the connection:
            m_channel.close();
            Command::LoginFailed lf(TargetFrontend);
            sendCommand(&lf);
        } else {
            m_login_abort_timer.start();
            m_username = match_confirm_login.cap(1);
            m_password.clear();

            // Confirm guest login:
            writeData("\n");
        }
    } else if (line.startsWith(fics_prompt)) {
        finalizeLogin();
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
    m_logged_in = false;
    m_filter &= ~LoginRequest;

    Command::LoginFailed lf(TargetFrontend);
    sendCommand(&lf);
}

void Engine::reconnect()
{}

void Engine::configureFics()
{
    writeData("set style 12\n");
    writeData("set seek 1\n");
    writeData("set autoflag 1\n");
}

void Engine::sendLogin()
{
    m_login_abort_timer.start();
    writeData(m_username.toLatin1());
    writeData("\n");
}

void Engine::finalizeLogin()
{
    m_login_count = 0;
    m_login_abort_timer.stop();
    m_extra_delimiter.clear();

    configureFics();
    Command::Login lc(TargetFrontend, m_username, "");
    sendCommand(&lc);

    m_logged_in = true;
    m_filter &= ~LoginRequest;
    m_filter |= WaitingForSeeks;
}

void Engine::sendCommand(AbstractCommand *command)
{
    if (Dispatcher *dispatcher = m_dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Fics
} // namespace Game

