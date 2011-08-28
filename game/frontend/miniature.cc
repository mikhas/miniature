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
#include "frontend/chessboardelement.h"
#include "frontend/sideelement.h"

#ifdef MINIATURE_GUI_ENABLED
#include <QtDeclarative/QtDeclarative>
#endif

namespace Game { namespace Frontend { namespace {
    const ParserCommandFlags all_commands(CommandPlay | CommandQuit | CommandLogin
                                    | CommandSeek | CommandJoin | CommandObserve
                                    | CommandMove);

    void configureSideElement(SideElement *elem,
                              Side *side,
                              const QColor &color)
    {
        if (not elem || not side) {
            return;
        }

        elem->setId(side->identifier());
        elem->setColor(color);
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

QString fromColor(Color color)
{
    switch(color) {
    case ColorNone: return "auto";
    case ColorWhite: return "white";
    case ColorBlack: return "black";
    }

    return "";
}

class Advertisements
    : public QAbstractItemModel
{
private:
    QVector<Seek> m_advertisements;

public:
    enum Roles {
        RoleId = Qt::UserRole + 1,
        RoleMode,
        RolePlayerName,
        RoleRating,
        RoleTime,
        RoleIncrement,
        RoleIsRated,
        RoleColor,
        RoleIsAutoStarted,
        RoleUsesFormula,
        RoleRatingLowerLimit,
        RoleRatingUpperLimit,
        RoleHighlighted
    };

    explicit Advertisements(QObject *parent = 0)
        : QAbstractItemModel(parent)
    {
        // QML cannot cope with c-style-variable-names!
        QHash<int, QByteArray> roles;
        roles[RoleId] = "id";
        roles[RoleMode] = "mode";
        roles[RolePlayerName] = "playerName";
        roles[RoleRating] = "rating";
        roles[RoleTime] = "time";
        roles[RoleIncrement] = "increment";
        roles[RoleIsRated] = "isRated";
        roles[RoleColor] = "color";
        roles[RoleIsAutoStarted] = "isAutoStarted";
        roles[RoleUsesFormula] = "usesFormula";
        roles[RoleRatingLowerLimit] = "ratingLowerLimit";
        roles[RoleRatingUpperLimit] = "ratingUpperLimit";
        roles[RoleHighlighted] = "highlighted";
        setRoleNames(roles);
    }

    virtual ~Advertisements()
    {}

    virtual void append(const Seek& s)
    {
        beginInsertRows(QModelIndex(), m_advertisements.size(), m_advertisements.size());
        m_advertisements.append(s);
        endInsertRows();

        // Manage a sliding window of no more than 1024 entries:
        // TODO: delete ads after x secs?
        while (m_advertisements.size() > 1024) {
            beginRemoveRows(QModelIndex(), 0, 0);
            m_advertisements.pop_front();
            endRemoveRows();
        }
    }

    virtual int columnCount(const QModelIndex &index = QModelIndex()) const
    {
        Q_UNUSED(index)
        return 1;
    }

    virtual int rowCount(const QModelIndex &index = QModelIndex()) const
    {
        Q_UNUSED(index)
        return m_advertisements.size();
    }

    virtual QModelIndex index(int row,
                              int column,
                              const QModelIndex &index = QModelIndex()) const
    {
        Q_UNUSED(index)
        return createIndex(row, column);
    }

    virtual QModelIndex parent(const QModelIndex &index = QModelIndex()) const
    {
        Q_UNUSED(index)
        return QModelIndex();
    }

    virtual QVariant data(const QModelIndex &index,
                          int role) const
    {
        if (m_advertisements.size() < index.row()) {
            return QVariant();
        }

        const Seek &s(m_advertisements.at(index.row()));

        switch(role) {
        case RoleId: return QVariant(s.id);
        case RoleMode: return QVariant(s.mode);
        case RolePlayerName: return QVariant(s.player_name);
        case RoleRating: return QVariant(s.rating);
        case RoleTime: return QVariant(s.time);
        case RoleIncrement: return QVariant(s.increment);
        case RoleIsRated: return QVariant(s.is_rated);
        case RoleColor: return QVariant(fromColor(s.color));
        case RoleIsAutoStarted: return QVariant(s.is_auto_started);
        case RoleUsesFormula: return QVariant(s.uses_formula);
        case RoleRatingLowerLimit: return QVariant(s.rating_range.first);
        case RoleRatingUpperLimit: return QVariant(s.rating_range.second);
        case RoleHighlighted: return QVariant(s.highlighted);

        default:
            return QVariant();
        }
    }

    virtual bool setData(const QModelIndex &index,
                         const QVariant &value,
                         int role)
    {
        if (m_advertisements.size() < index.row()) {
            return false;
        }

        Seek &s(m_advertisements[index.row()]);


        switch(role) {
        case RoleHighlighted: s.highlighted = value.toBool();
            emit dataChanged(index, index);
            return true;

        // Other data is "read-only":
        default:
            break;
        }

        return false;
    }
};

class MiniaturePrivate
{
public:
    WeakDispatcher dispatcher;
    CommandLine command_line;
    LineReader line_reader;
    Advertisements advertisements;
    ChessBoard chess_board;
    bool valid_move;
    Miniature::Mode mode;
    WeakGame game;
    SideElement local_side;
    SideElement remote_side;
#ifdef MINIATURE_GUI_ENABLED
    QDeclarativeView ui;
#endif

    explicit MiniaturePrivate(Dispatcher *new_dispatcher)
        : dispatcher(new_dispatcher)
        , command_line(new_dispatcher)
        , line_reader()
        , advertisements()
        , chess_board()
        , valid_move(false)
        , mode(Miniature::TestFicsMode)
        , game()
        , local_side()
        , remote_side()
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

    d->ui.rootContext()->setContextProperty("advertisements", &d->advertisements);
    d->ui.rootContext()->setContextProperty("chessBoard", &d->chess_board);
    d->ui.rootContext()->setContextProperty("miniature", this);
    d->ui.rootContext()->setContextProperty("localSide", &d->local_side);
    d->ui.rootContext()->setContextProperty("remoteSide", &d->remote_side);
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
    d->advertisements.append(s);
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
    Command::Login login(TargetEngine, username, password);
    sendCommand(&login);
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
        Command::CreateGame cg(TargetRegistry, 999u, d->dispatcher.data(),
                               "test123", "test456", LocalSideIsBlack);
        sendCommand(&cg);
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
        Command::CreateGame cg(TargetRegistry, 999u, d->dispatcher.data(),
                               "test123", "test456", LocalSideIsBlack);
        sendCommand(&cg);
    } break;
    }
}

void Miniature::toggleGameAdvertisementHighlighting(uint id)
{
    Q_D(Miniature);

    for (int index = 0; index < d->advertisements.rowCount(); ++index) {
        const QModelIndex mi(d->advertisements.index(index, 0));
        const bool found(d->advertisements.data(mi, Advertisements::RoleId).toUInt() == id);
        const bool was_highlighted(d->advertisements.data(mi, Advertisements::RoleHighlighted).toBool());

        d->advertisements.setData(mi, found && not was_highlighted, Advertisements::RoleHighlighted);
    }
}

bool Miniature::selectSquare(int target)
{
    Q_D(Miniature);

    if (not d->chess_board.selectSquare(target)) {
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

    if (not d->chess_board.confirmMove()) {
        // TODO: recover from this
        qWarning() << __PRETTY_FUNCTION__
                   << "Unable to confirm move.";
        return;
    }

    const uint id(d->game.isNull() ? 999u : d->game.data()->id());

    if (d->mode == FicsMode) {
        Command::Move m(TargetEngine, id, d->chess_board.position());
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

    configureSideElement(&d->local_side, game->localSide(),
                         game->localSideColor() == LocalSideIsWhite ? Qt::white : Qt::black);
    configureSideElement(&d->remote_side, game->remoteSide(),
                         game->localSideColor() == LocalSideIsBlack ? Qt::white : Qt::black);
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
}

}} // namespace Game, Frontend
