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

#include "game.h"
#include "localside.h"
#include "gnuchess.h"

#include <QtCore>
#include <QtGui>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    // TODO: Move into a proper Miniature class that can setup games.
    new Game::Game(new Game::LocalSide("white"),
                   new Game::GnuChess("black"),
                   &app);

    return app.exec();
}
