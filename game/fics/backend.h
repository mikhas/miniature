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

#ifndef FICS_BACKEND_H
#define FICS_BACKEND_H

#include "namespace.h"
#include "abstractbackend.h"
#include "dispatcher.h"

#include <QtCore>
#include <QtNetwork/QTcpSocket>

namespace Game { namespace Fics {

//! A backend for FICS
class Backend
    : public AbstractBackend
{
    Q_OBJECT
    Q_DISABLE_COPY(Backend)

private:
    WeakDispatcher m_dispatcher;
    QTcpSocket m_channel;
    QByteArray m_buffer;
    QString m_username;
    QString m_password;
    State m_state;
    bool m_enabled;
    QTimer m_login_abort_timer;
    QVector<char> m_extra_delimiter;

public:
    //! \reimp
    explicit Backend(Dispatcher *dispatcher,
                     QObject *parent = 0);
    virtual ~Backend();
    virtual void setFlags(ParserCommandFlags flags);
    virtual void setEnabled(bool enable);
    virtual State state() const;
    virtual void login(const QString &username,
                       const QString &password);
    virtual void processToken(const QByteArray &token);
    //! \reimp_end

    virtual void enableTesting();

private:
    Q_SLOT void onReadyRead();
    void processLogin(const QByteArray &line);
    Q_SLOT void onHostFound();
    Q_SLOT void abortLogin();
    void configurePrompt();
};

} // namespace Fics
} // namespace Game

#endif // FICS_BACKEND_H
