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

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "abstractcommand.h"
#include "abstractengine.h"

#include <QtCore>

namespace Game { namespace Command {

//! Login to remote server.
class Login
    : public AbstractCommand
{
private:
    Target m_target;
    QString m_username;
    QString m_password;

public:
    //! \reimp
    explicit Login(Target target,
                   const QString &username,
                   const QString &password);
    virtual ~Login();
    virtual Target target() const;
    virtual void exec(AbstractEngine *);
    //! \reimp_end
};

}} // namespace Command, Game

#endif // TEMPLATE_H
