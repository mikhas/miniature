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

#include "miniature.h"
#include "commands.h"
#include "directinputdevice.h"
#include "registry.h"
#include "frontend/chessboard.h"
#include "frontend/sideelement.h"
#include "frontend/gameelement.h"
#include "frontend/availableseeks.h"

#ifdef MINIATURE_GUI_ENABLED
#include <QtDeclarative/QtDeclarative>
#endif

namespace Game { namespace Frontend { namespace {
    const ParserCommandFlags all_commands(CommandPlay | CommandQuit | CommandLogin
                                    | CommandSeek | CommandJoin | CommandObserve
                                    | CommandMove);

    void updateSideElement(SideElement *elem,
                           const Side &side,
                           const QColor &color)
    {
        if (not elem) {
            return;
        }

        elem->setId(side.name);
        elem->setColor(color);
        elem->setRating(side.rating);
        elem->setMaterialStrength(side.material_strength);
        elem->setRemainingTime(side.remaining_time);
    }

    Miniature::Rating toRating(const QString &rating)
    {
        if (rating == "Unrated") {
            return Miniature::RatingDisabled;
        } else if (rating == "Rated") {
            return Miniature::RatingEnabled;
        }

        return Miniature::RatingAny;
    }

    Miniature::Color toColor(const QString &color)
    {
        if (color == "White") {
            return Miniature::White;
        } else if (color == "Black") {
            return Miniature::Black;
        }

        return Miniature::Auto;
    }
}

class MiniaturePrivate
{
public:
    WeakDispatcher dispatcher;
    CommandLine command_line;
    LineReader line_reader;
    AvailableSeeks available_seeks;
    ChessBoard chess_board;
    bool valid_move;
    Miniature::Mode mode;
    WeakGame game;
    SideElement local_side;
    SideElement remote_side;
    GameElement game_element;
#ifdef MINIATURE_GUI_ENABLED
    QDeclarativeView ui;
#endif

    explicit MiniaturePrivate(Dispatcher *new_dispatcher)
        : dispatcher(new_dispatcher)
        , command_line(new_dispatcher)
        , line_reader()
        , available_seeks()
        , chess_board()
        , valid_move(false)
        , mode(Miniature::TestFicsMode)
        , game()
        , local_side()
        , remote_side()
        , game_element()
#ifdef MINIATURE_GUI_ENABLED
        , ui()
#endif
    {}
};

Miniature::Miniature(Dispatcher *dispatcher,
                     QObject *parent)
    : QObject(parent)
    , d_ptr(new MiniaturePrivate(dispatcher))
{
    Q_D(Miniature);

#ifdef MINIATURE_GUI_ENABLED
    qmlRegisterUncreatableType<Miniature>("org.maemo.miniature", 1, 0, "Miniature",
                                         "Enables access to Miniature enums.");
    qmlRegisterType<SideElement>("org.maemo.miniature", 1, 0, "SideElement");

    d->ui.rootContext()->setContextProperty("availableSeeks", &d->available_seeks);
    d->ui.rootContext()->setContextProperty("chessBoard", &d->chess_board);
    d->ui.rootContext()->setContextProperty("miniature", this);
    d->ui.rootContext()->setContextProperty("localSide", &d->local_side);
    d->ui.rootContext()->setContextProperty("remoteSide", &d->remote_side);
    d->ui.rootContext()->setContextProperty("activeGame", &d->game_element);
#endif

    d->command_line.setFlags(all_commands);
    connect(&d->line_reader, SIGNAL(tokenFound(QByteArray)),
            &d->command_line, SLOT(processToken(QByteArray)),
            Qt::UniqueConnection);
}

Miniature::~Miniature()
{}

void Miniature::show(const QUrl &ui)
{
    Q_D(Miniature);

#ifndef MINIATURE_GUI_ENABLED
    Q_UNUSED(ui)
#else
    if (d->ui.source() != ui) {
        d->ui.setSource(ui);
        QFont font;
        // FIXME: Read font family from a QML property instead.
        font.setFamily("Nokia Pure Text");
        qApp->setFont(font);
    }

    d->ui.showFullScreen();
#endif

    d->line_reader.init(new DirectInputDevice);
    d->command_line.setEnabled(true);

    QTextStream out(stdout);
    out << "Welcome to Miniature!\n";
}

void Miniature::showBoard()
{}

void Miniature::handleRecord(const Record &)
{}

void Miniature::handleSeek(const Seek &s)
{
    Q_D(Miniature);
    d->available_seeks.append(s);
}

void Miniature::setMode(Mode mode)
{
    Q_D(Miniature);
    if (d->mode != mode) {
        qDebug() << __PRETTY_FUNCTION__
                 << "mode:" << mode;
        d->mode = mode;
        emit modeChanged(d->mode);
    }
}

Miniature::Mode Miniature::mode() const
{
    Q_D(const Miniature);
    return d->mode;
}

void Miniature::login(const QString &username,
                      const QString &password)
{
    qDebug() << __PRETTY_FUNCTION__;
    Command::Login login(TargetEngine, username, password);
    sendCommand(&login);
}

void Miniature::logout()
{
    Q_D(Miniature);
    d->available_seeks.removeAll();

    Command::Logout logout(TargetEngine);
    sendCommand(&logout);
}

void Miniature::seek(uint time,
                     uint increment,
                     Rating rating,
                     Color color)
{
    Q_D(const Miniature);
    switch(d->mode) {
    default:
    case FicsMode: {
        Command::Seek seek(TargetEngine, time, increment,
                           static_cast< ::Game::Rating >(rating),
                           static_cast< ::Game::Color >(color));
        sendCommand(&seek);
    } break;

    case TestFicsMode: {
        Command::CreateGame cg(TargetRegistry, 999u, "test123", "test456", LocalSideIsWhite);
        sendCommand(&cg);

        Command::Move m(TargetFrontend, 999u, createStartPosition());
        sendCommand(&m);
    } break;
    }
}

void Miniature::seek(uint time,
                     uint increment,
                     const QString &rating,
                     const QString &color)
{
    seek(time, increment, toRating(rating), toColor(color));
}

void Miniature::play(uint id)
{
    Q_D(const Miniature);
    switch(d->mode) {
    default:
    case FicsMode: {
        Command::Play play(TargetEngine, id);
        sendCommand(&play);
    } break;

    case TestFicsMode: {
        Command::CreateGame cg(TargetRegistry, 999u, "test123", "test456", LocalSideIsWhite);
        sendCommand(&cg);

        Command::Move m(TargetFrontend, 999u, createStartPosition());
        sendCommand(&m);
    } break;
    }
}

void Miniature::toggleGameAdvertisementHighlighting(uint id)
{
    Q_D(Miniature);

    for (int index = 0; index < d->available_seeks.rowCount(); ++index) {
        const QModelIndex mi(d->available_seeks.index(index, 0));
        const bool found(d->available_seeks.data(mi, AvailableSeeks::RoleId).toUInt() == id);
        const bool was_highlighted(d->available_seeks.data(mi, AvailableSeeks::RoleHighlighted).toBool());

        d->available_seeks.setData(mi, found && not was_highlighted, AvailableSeeks::RoleHighlighted);
    }
}

bool Miniature::selectSquare(int target)
{
    Q_D(Miniature);

    const ::Game::Color local_color(d->mode == FicsMode ? (d->local_side.color() == Qt::white ? ColorWhite
                                                                                              : ColorBlack)
                                                        : d->chess_board.position().nextToMove());

    if (not d->chess_board.selectSquare(target, local_color)) {
        return false;
    } else {
        const bool was_valid(d->valid_move);
        d->valid_move = d->chess_board.isValidMove();
        if (d->valid_move != was_valid) {
            emit validMoveChanged(d->valid_move);
        }
    }

    return true;
}

bool Miniature::validMove() const
{
    Q_D(const Miniature);
    return d->valid_move;
}

void Miniature::confirmMove()
{
    Q_D(Miniature);
    if (not d->valid_move) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Invalid move confirmed. Ignored.";
        return;
    }

    const Position &pos(d->chess_board.confirmMove());
    if (not pos.valid()) {
        // TODO: recover from this
        qWarning() << __PRETTY_FUNCTION__
                   << "Unable to confirm move.";
        return;
    }

    uint id = 999u;
    if (Game *g = d->game.data()) {
        g->setPosition(pos);
        id = g->id();
    } else {
        qWarning() << __PRETTY_FUNCTION__
                   << "No active game found!";
        return;
    }

    if (d->mode == FicsMode) {
        qDebug() << __PRETTY_FUNCTION__
                 << "move confirmed ...";
        Command::Move m(TargetEngine, id, pos);
        sendCommand(&m);
    }

    d->valid_move = false;
    emit validMoveChanged(false);
}

void Miniature::setActiveGame(Game *game)
{
    Q_D(Miniature);
    d->game = WeakGame(game);

    if (not game) {
        return;
    }

    disconnect(this, SLOT(onPositionChanged(Position)));
    connect(game, SIGNAL(positionChanged(Position)),
            this, SLOT(onPositionChanged(Position)),
            Qt::UniqueConnection);

    // Update current position manually:
    d->chess_board.setOrientation(game->localSideColor() == LocalSideIsWhite ? ChessBoard::WhiteAtBottom
                                                                             : ChessBoard::WhiteAtTop);
    onPositionChanged(game->position());

    updateSideElement(&d->local_side, game->localSide(),
                         game->localSideColor() == LocalSideIsWhite ? Qt::white : Qt::black);
    updateSideElement(&d->remote_side, game->remoteSide(),
                         game->localSideColor() == LocalSideIsBlack ? Qt::white : Qt::black);

    d->game_element.setId(game->id());
    d->game_element.setTime(game->time());
    d->game_element.setTimeIncrement(game->timeIncrement());
}

Game * Miniature::activeGame() const
{
    Q_D(const Miniature);
    return d->game.data();
}

void Miniature::setUsername(const QString &username)
{
    Q_D(Miniature);
    d->local_side.setId(username);
}

void Miniature::updateLocalSide(const Side &local_side)
{
    Q_D(Miniature);
    updateSideElement(&d->local_side, local_side, d->local_side.color());

}

void Miniature::updateRemoteSide(const Side &remote_side)
{
    Q_D(Miniature);
    updateSideElement(&d->remote_side, remote_side, d->remote_side.color());
}

void Miniature::sendCommand(AbstractCommand *command)
{
    Q_D(Miniature);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

void Miniature::onPositionChanged(const Position &position)
{
    Q_D(Miniature);
    d->chess_board.setPosition(position);

    if (Game *g = d->game.data()) {
        const bool is_local_side_active((g->localSideColor() == LocalSideIsWhite
                                         && position.nextToMove() == ColorWhite)
                                        || (g->localSideColor() == LocalSideIsBlack
                                            && position.nextToMove() == ColorBlack));

        d->local_side.setActive(is_local_side_active);
        d->remote_side.setActive(not is_local_side_active);
    }
}

}} // namespace Game, Frontend
