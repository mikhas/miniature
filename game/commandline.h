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

#ifndef LOCALPARSER_H
#define LOCALPARSER_H

#include "namespace.h"
#include "abstractbackend.h"

#include <QtCore>

namespace Game {

class CommandLinePrivate;

//! Reads input from local input device and translates it into commands.
//! Used for local sides and local command line interface.
class CommandLine
    : public AbstractBackend
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandLine)
    Q_DISABLE_COPY(CommandLine)

private:
    const QScopedPointer<CommandLinePrivate> d_ptr;

public:
    //! \reimp
    explicit CommandLine(QObject *parent = 0);
    virtual ~CommandLine();
    virtual void setFlags(ParserCommandFlags flags);
    virtual void setEnabled(bool enable);
    virtual void processToken(const QByteArray &token);
    //! \reimp_end
};

} // namespace Game

#endif // LOCALPARSER_H
