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

#include <game.h>
#include <game_log.h>
#include <local_game.h>
#include <network_game.h>

#include <main_window.h>
#include <mgameconfig.h>
#include <iconic_button.h>

#include <QtCore>
#include <QtGui>

namespace Miniature
{
/*!
 *  This class controls the start screen. It also sets up the specific game
 *  controller and its UI, handing over the control flow once the user made a
 *  choice.
 */
class MPreGame
    : public QObject
{
    Q_OBJECT

public:
    explicit MPreGame(QObject *parent = 0);
    virtual ~MPreGame();

public Q_SLOTS:
    /*!
     *  Shows the start screen where the user can choose one of the available game modi.
     *  In the near future, this method will also initialize the prerendering
     *  of all the needed QImages in a separate thread.
     */
    void onStartScreenRequested();

    //! Starts a game in "pub mode"
    void onStartLocalGame();

    //! Join an existing P2P game.
    void onJoinGame();

    //! Join a game on FICS.
    void onJoinFicsGame();

private:
    MSharedGameLog m_log;
    MMainWindow m_main;
    MGameConfig m_game_config;

    QPointer<MIconicButton> m_local_game_button;
    QPointer<MIconicButton> m_join_game_button;
    QPointer<MIconicButton> m_fics_game_button;
};

} // namespace Miniature
