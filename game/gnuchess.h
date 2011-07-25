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

#ifndef GNUCHESS_H
#define GNUCHESS_H

#include "move.h"
#include "abstractside.h"
#include "abstractparser.h"

#include <QtCore>

namespace Game {

// Only a skeleton for now.
class GnuChessParser
    : public AbstractParser
{
    Q_OBJECT
    Q_DISABLE_COPY(GnuChessParser)

public:
    //! \reimp
    explicit GnuChessParser(QObject *parent = 0);
    virtual ~GnuChessParser();
    virtual void setFlags(CommandFlags flags);
    virtual void setEnabled(bool enable);
    virtual void processToken(const QByteArray &token);
    //! \reimp
};

//! Gnuchess backend.
class GnuChess
    : public AbstractSide
{
    Q_OBJECT
    Q_DISABLE_COPY(GnuChess)

private:
    const QString m_identifier;
    SideState m_state;
    QProcess m_proc;
    SharedParser m_parser;

public:
    //! \reimp
    explicit GnuChess(const QString &identifier,
                      const SharedParser &parser);
    virtual ~GnuChess();
    virtual void init();
    virtual SideState state() const;
    virtual const QString &identifier() const;
    virtual void runInBackground();
    virtual void runInForeground();
    virtual void startTurn(const Move &move);
    virtual void onCommandFound(Command command,
                                const QByteArray &data);
    //! \reimp_end

private:
    Q_SLOT void onReadyRead();
};

} // namespace Game

#endif // GNUCHESS_H
