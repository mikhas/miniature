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

#include <mmainwindow.h>
#include <iconic_button.h>

#include <QtCore>
#include <QtGui>

namespace Miniature
{
/*!
 *  This class controls the start screen and forwards control to the correct
 *  MGame controller once the user made a choice
 */
class MPreGame
: public QObject
{
    Q_OBJECT

public:
    explicit MPreGame(QObject *parent = 0);
    virtual ~MPreGame();

    MGameLog * getLog() const;

public Q_SLOTS:
    /*!
     *  Shows the start screen where the user can choose one of the available game modi.
     *  In the near future, this method will also initialize the prerendering
     *  of all the needed QImages in a separate thread.
     *  @param[in] view the view where we place the start screen into.
     */
    void onStartScreenRequested();

    //! Starts a game in "pub mode"
    void onStartLocalGame();

    //! Hosts a new P2P game.
    void onHostGame();

    //! Join an existing P2P game.
    void onJoinGame();

    //! Join a game on FICS.
    void onJoinFicsGame();

private:
    /*!
     *  This class helps to configure a game, it does some basic setup and
     *  takes care of the ownership relationships between the interacting components:
     *  The MMainWindow -> a MIconicButton,
     *                  -> a QMainWindow   -> a MGame,
     *                                     -> a MBoardView.
     */
    class MGameConfig
    {

    public:
        explicit MGameConfig(const QString &button_label,
                             const QPixmap &button_icon,
                             MMainWindow *main);

        virtual ~MGameConfig();

        void setGame(MGame *game);

        QMainWindow * getWindow() const;
        MBoardView * getBoardView() const;
        MGame * getGame() const;
        MIconicButton * getButton() const;

    private:
        QPointer<MMainWindow> m_main;
        QPointer<QMainWindow> m_window;
        QPointer<MGame> m_game;
        QPointer<MIconicButton> m_button;
    };

    void setupGame(MGameConfig *config);

    MGameLog m_log;
    MMainWindow m_main;
    MGameConfig m_local_game;
    MGameConfig m_host_game;
    MGameConfig m_join_game;
    MGameConfig m_fics_game;
};

} // namespace Miniature
