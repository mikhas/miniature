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
#include "commands/login.h"
#include "commands/logoutcommand.h"
#include "directinputdevice.h"

#ifdef MINIATURE_GUI_ENABLED
#include <QtDeclarative/QtDeclarative>
#endif

namespace Game { namespace {
    const ParserCommandFlags all_commands(CommandNew | CommandQuit | CommandLogin
                                    | CommandSeek | CommandJoin | CommandObserve
                                    | CommandMove);
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
        RoleWhiteToStart,
        RoleIsAutoStarted,
        RoleUsesFormula,
        RoleRatingLowerLimit,
        RoleRatingUpperLimit
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
        roles[RoleWhiteToStart] = "whiteToStart";
        roles[RoleIsAutoStarted] = "isAutoStarted";
        roles[RoleUsesFormula] = "usesFormula";
        roles[RoleRatingLowerLimit] = "ratingLowerLimit";
        roles[RoleRatingUpperLimit] = "ratingUpperLimit";
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

    virtual int columnCount(const QModelIndex &) const
    {
        return 1;
    }

    virtual int rowCount(const QModelIndex &) const
    {
        return m_advertisements.size();
    }

    virtual QModelIndex index(int row, int column, const QModelIndex &) const
    {
        return createIndex(row, column);
    }

    virtual QModelIndex parent(const QModelIndex &) const
    {
        return QModelIndex();
    }

    virtual QVariant data(const QModelIndex &index, int role) const
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
        case RoleWhiteToStart: return QVariant(s.white_to_start);
        case RoleIsAutoStarted: return QVariant(s.is_auto_started);
        case RoleUsesFormula: return QVariant(s.uses_formula);
        case RoleRatingLowerLimit: return QVariant(s.rating_range.first);
        case RoleRatingUpperLimit: return QVariant(s.rating_range.second);

        default:
            return QVariant();
        }
    }
};

class FrontendPrivate
{
public:
    WeakDispatcher dispatcher;
    CommandLine command_line;
    LineReader line_reader;
    AdvertisementModel advertisements;
#ifdef MINIATURE_GUI_ENABLED
    QDeclarativeView ui;
#endif

    explicit FrontendPrivate(Dispatcher *new_dispatcher)
        : dispatcher(new_dispatcher)
        , command_line()
        , line_reader()
        , advertisements()
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
    connect(&d->command_line, SIGNAL(commandFound(ParserCommand,QByteArray)),
            this,            SLOT(onCommandFound(ParserCommand,QByteArray)),
            Qt::UniqueConnection);

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

void Frontend::onCommandFound(ParserCommand cmd,
                              const QByteArray &data)
{
    Q_D(Frontend);

    Dispatcher *dispatcher = d->dispatcher.data();

    switch(cmd) {
    case CommandLogin: {
        QList<QByteArray> list = data.split(' ');
        const QString username(data.isEmpty() ? "guest" : list.at(0));
        const QString password(list.size() > 1 ? list.at(1) : "");

        if (dispatcher) {
            Command::Login login(TargetBackend, username, password);
            dispatcher->sendCommand(&login);
        }
    } break;

    case CommandJoin:
        break;

    case CommandNew:
        break;

    case CommandQuit: {
        if (dispatcher) {
            LogoutCommand logout(TargetBackend);
            dispatcher->sendCommand(&logout);
        }
        qApp->quit();
    } break;

    default:
        break;
    }
}

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
    Command::Login lc(TargetBackend, username, password);
    sendCommand(&lc);
}

void Frontend::play(int id)
{
    // Placeholder ...
    class PlayCommand
        : public AbstractCommand
    {
    public:
        PlayCommand(Target t, int)
            : AbstractCommand(t)
        {}

        ~PlayCommand() {}
        Target target() const {return TargetBackend;}
    };

    // TODO: Check for active backend (in dispatcher?), as this command does not feel FICS specific.
    PlayCommand pc(TargetBackend, id);
    sendCommand(&pc);
}

void Frontend::sendCommand(AbstractCommand *command)
{
    Q_D(Frontend);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Game
