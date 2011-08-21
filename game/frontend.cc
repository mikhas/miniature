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

#ifdef MINIATURE_GUI_ENABLED
#include <QtDeclarative/QtDeclarative>
#endif

namespace Game { namespace {
    const ParserCommandFlags all_commands(CommandPlay | CommandQuit | CommandLogin
                                    | CommandSeek | CommandJoin | CommandObserve
                                    | CommandMove);

    QString fromColor(Color color)
    {
        switch(color) {
        case ColorAuto: return "auto";
        case ColorWhite: return "white";
        case ColorBlack: return "black";
        }

        return "";
    }
}

class AdvertisementModel
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

    explicit AdvertisementModel(QObject *parent = 0)
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

    virtual ~AdvertisementModel()
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
    AdvertisementModel advertisements;
    Registry registry;
    WeakSide local_side;
    WeakSide remote_side;
#ifdef MINIATURE_GUI_ENABLED
    QDeclarativeView ui;
#endif

    explicit FrontendPrivate(Dispatcher *new_dispatcher)
        : dispatcher(new_dispatcher)
        , command_line(new_dispatcher)
        , line_reader()
        , advertisements()
        , registry(new_dispatcher)
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
    d->ui.rootContext()->setContextProperty("gameAdvertisements", &d->advertisements);
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
        const bool found(d->advertisements.data(mi, AdvertisementModel::RoleId).toUInt() == id);
        const bool was_highlighted(d->advertisements.data(mi, AdvertisementModel::RoleHighlighted).toBool());

        d->advertisements.setData(mi, found && not was_highlighted, AdvertisementModel::RoleHighlighted);
    }
}

void Frontend::registerGame(Game *game)
{
    if (not game) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Cannot register invalid game!";
        return;
    }

    Q_D(Frontend);
    d->local_side = game->localSide();
    d->remote_side = game->remoteSide();
    d->registry.registerGame(game);
}

void Frontend::sendCommand(AbstractCommand *command)
{
    Q_D(Frontend);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Game
