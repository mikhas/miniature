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

#ifndef FICSSIDE_H
#define FICSSIDE_H

#include "namespace.h"
#include "abstractside.h"
#include "abstractparser.h"

#include <QtCore>
#include <QtNetwork/QTcpSocket>

namespace Game
{

class AbstractLink;
typedef QSharedPointer<AbstractLink> SharedLink;

class AbstractLink
    : public AbstractParser
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractLink)

public:
    enum State {
        StateIdle,
        StateLoginPending,
        StateLoginFailed,
        StateReady
    };

    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    //! \reimp
    explicit AbstractLink(QObject *parent = 0);
    virtual ~AbstractLink() = 0;
    //! \reimp_end

    virtual State state() const = 0;
    Q_SIGNAL void stateChanged(State state);

    virtual void login(const QString &username,
                       const QString &password) = 0;
};

class FicsLink
    : public AbstractLink
{
    Q_OBJECT
    Q_DISABLE_COPY(FicsLink)

private:
    QTcpSocket m_channel;
    QByteArray m_buffer;
    QString m_username;
    QString m_password;
    State m_state;
    bool m_enabled;

public:
    //! \reimp
    explicit FicsLink(QObject *parent = 0);
    virtual ~FicsLink();
    virtual void setEnabled(bool enable);
    virtual State state() const;
    virtual void login(const QString &username,
                       const QString &password);
    //! \reimp_end

private:
    Q_SLOT void onReadyRead();
    void processLogin(const QByteArray &line);
    Q_SLOT void onHostFound();
};

//! Implement a backend for FICS (freechess.org).
//! Can be feeded by Game::FicsParser.
class FicsSide
    : public AbstractSide
{
    Q_OBJECT
    Q_DISABLE_COPY(FicsSide)

private:
    const QString m_identifier;
    const QString m_password;
    SideState m_state;
    SharedLink m_link;

public:
    //! C'tor
    //! @param identifier the identifier for this side.
    //! @param link the shared FICS link.
    explicit FicsSide(const QString &identifier,
                      const SharedLink &link);

    //! \reimp
    virtual ~FicsSide();
    virtual void init();
    virtual SideState state() const;
    virtual const QString &identifier() const;
    virtual void runInBackground();
    virtual void runInForeground();
    virtual void startTurn(const Move &move);
    virtual void onCommandFound(Command command,
                                const QByteArray &data);
    //! \reimp_end
};

} // namespace Game

#endif // FICSSIDE_H
