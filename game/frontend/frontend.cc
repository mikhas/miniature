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

#include "frontend.h"
#include "commands.h"
#include "directinputdevice.h"
#include "registry.h"
#include "frontend/chessboard.h"
#include "frontend/chessboardelement.h"

#ifdef MINIATURE_GUI_ENABLED
#include <QtDeclarative/QtDeclarative>
#endif

namespace Game { namespace Frontend { namespace {
    const ParserCommandFlags all_commands(CommandPlay | CommandQuit | CommandLogin
                                    | CommandSeek | CommandJoin | CommandObserve
                                    | CommandMove);
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

class FrontendPrivate
{
public:
    WeakDispatcher dispatcher;
    CommandLine command_line;
    LineReader line_reader;
    Advertisements advertisements;
    ChessBoard chess_board;
    bool valid_move;
    WeakGame game;
#ifdef MINIATURE_GUI_ENABLED
    QDeclarativeView ui;
#endif

    explicit FrontendPrivate(Dispatcher *new_dispatcher)
        : dispatcher(new_dispatcher)
        , command_line(new_dispatcher)
        , line_reader()
        , advertisements()
        , chess_board()
        , valid_move(false)
        , game()
#ifdef MINIATURE_GUI_ENABLED
        , ui()
#endif
    {}
};

Frontend::Frontend(Dispatcher *dispatcher,
                   QObject *parent)
    : QObject(parent)
    , d_ptr(new FrontendPrivate(dispatcher))
{
    Q_D(Frontend);

#ifdef MINIATURE_GUI_ENABLED
    qmlRegisterType<ChessBoardElement>("org.maemo.miniature", 1, 0, "ChessBoardElement");
    d->ui.rootContext()->setContextProperty("advertisements", &d->advertisements);
    d->ui.rootContext()->setContextProperty("chessBoard", &d->chess_board);
    d->ui.rootContext()->setContextProperty("miniature", this);
#endif

    d->command_line.setFlags(all_commands);
    connect(&d->line_reader, SIGNAL(tokenFound(QByteArray)),
            &d->command_line, SLOT(processToken(QByteArray)),
            Qt::UniqueConnection);
}

Frontend::~Frontend()
{}

void Frontend::show(const QUrl &ui)
{
    Q_D(Frontend);

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

void Frontend::showBoard()
{}

void Frontend::handleRecord(const Record &)
{}

void Frontend::handleSeek(const Seek &s)
{
    Q_D(Frontend);
    d->advertisements.append(s);
}

void Frontend::login(const QString &username,
                     const QString &password)
{
    Command::Login login(TargetBackend, username, password);
    sendCommand(&login);
}

void Frontend::play(uint id)
{
    Command::Play play(TargetBackend, id);
    sendCommand(&play);
}

void Frontend::toggleGameAdvertisementHighlighting(uint id)
{
    Q_D(Frontend);

    for (int index = 0; index < d->advertisements.rowCount(); ++index) {
        const QModelIndex mi(d->advertisements.index(index, 0));
        const bool found(d->advertisements.data(mi, Advertisements::RoleId).toUInt() == id);
        const bool was_highlighted(d->advertisements.data(mi, Advertisements::RoleHighlighted).toBool());

        d->advertisements.setData(mi, found && not was_highlighted, Advertisements::RoleHighlighted);
    }
}

void Frontend::selectPiece(int target)
{
    Q_D(Frontend);
    d->chess_board.selectPiece(toSquare(target));
}

void Frontend::movePiece(int origin,
                         int target,
                         const QString &promotion)
{
    Q_UNUSED(promotion)
    if (origin == target) {
        return;
    }

    const Square o(toSquare(origin));
    const Square t(toSquare(target));

    if (not o.valid() || not t.valid()) {
        return;
    }

    Q_D(Frontend);

    // Undo last move and invalidate:
    const bool was_valid(d->valid_move);
    d->chess_board.undo();
    d->valid_move = false;

    // Find piece in current position:
    Position pos(d->chess_board.position());
    Piece p0(pos.pieceAt(o));
    const Piece &p1(pos.pieceAt(t));

    // Select piece:
    d->chess_board.selectPiece(o);
    p0.setSquare(t);

    // TODO: Send through validator, detect castling etc ... info needs to be set on Position.
    // Easier: send move to chess engine and just get new position ;-)

    d->valid_move = (p0.valid()
                     && p0.color() == pos.nextToMove()
                     && p0.color() != p1.color());

    if (was_valid != d->valid_move) {
        emit validMoveChanged(d->valid_move);
    }

    if (d->valid_move) {
        pos.setMovedPiece(MovedPiece(p0, o));
        d->chess_board.setPosition(pos);
    }
}

void Frontend::undoMove()
{
    Q_D(Frontend);
    d->chess_board.undo();

    d->valid_move = false;
    emit validMoveChanged(false);
}

bool Frontend::validMove() const
{
    Q_D(const Frontend);
    return d->valid_move;
}

void Frontend::confirmMove()
{
    Q_D(Frontend);
    if (not d->valid_move) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Invalid move confirmed. Ignored.";
        return;
    }

    d->chess_board.commitMove();
    const uint id(d->game.isNull() ? 999u : d->game.data()->id());
    Command::Move m(TargetRegistry, id, d->chess_board.position());
    sendCommand(&m);

    d->valid_move = false;
    emit validMoveChanged(false);
}

void Frontend::setActiveGame(Game *game)
{
    Q_D(Frontend);

    d->game = WeakGame(game);

    if (not game) {
        return;
    }

    disconnect(this, SLOT(onPositionChanged(Position)));
    connect(game, SIGNAL(positionChanged(Position)),
            this, SLOT(onPositionChanged(Position)),
            Qt::UniqueConnection);

    // Update current position manually:
    onPositionChanged(game->position());
}

void Frontend::sendCommand(AbstractCommand *command)
{
    Q_D(Frontend);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

void Frontend::onPositionChanged(const Position &position)
{
    Q_D(Frontend);
    d->chess_board.setPosition(position);
}

}} // namespace Game, Frontend
