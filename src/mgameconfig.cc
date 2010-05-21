/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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

#include <mgameconfig.h>

namespace Miniature
{

MGameConfig::
MGameConfig(MMainWindow *main)
    : m_main(main),
      m_window(0),
      m_game(0)
{
    Q_ASSERT(0 != m_main);
}

MGameConfig::
~MGameConfig()
{}

void MGameConfig::
setGame(MGame *game)
{
    delete m_game;
    m_game = game;

    delete m_window;
    m_window = new QMainWindow;

    m_game->setParent(m_window);
    m_game->setBoardView(new MBoardView(m_window));
}

QMainWindow * MGameConfig::
getWindow() const
{
    return m_window;
}

MBoardView * MGameConfig::
getBoardView() const
{
    return m_game->getBoardView();
}

MGame * MGameConfig::
getGame() const
{
    return m_game;
}

void MGameConfig::
setupGame()
{
    MMainWindow::setupGameUi(getWindow(), getBoardView());

    connect(getGame(),   SIGNAL(destroyed()),
            getWindow(), SLOT(close()),
            Qt::UniqueConnection);

    connect(getGame(), SIGNAL(destroyed()),
            m_main,           SLOT(show()),
            Qt::UniqueConnection);

    connect(getGame(), SIGNAL(disconnected()), SIGNAL(restart()), Qt::UniqueConnection);
    connect(getGame(), SIGNAL(connected()), SLOT(runGame()), Qt::UniqueConnection);

    getGame()->newGame();
}

void MGameConfig::
runGame()
{
    getWindow()->show();
    m_main->hide();
}

}
