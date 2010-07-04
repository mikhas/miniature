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

#ifndef MGAMECONFIG_H
#define MGAMECONFIG_H

#include <QObject>
#include <main_window.h>
#include <game.h>

/*!
 *  This class helps to configure a game, it does some basic setup and
 *  takes care of the ownership relationships between the interacting components:
 */
namespace Miniature
{

class MGameConfig : public QObject
{
    Q_OBJECT
public:
    explicit MGameConfig(MMainWindow *main);

    virtual ~MGameConfig();

    void setGame(MGame *game);
    void setupGame();

    QMainWindow * getWindow() const;
    MBoardView * getBoardView() const;
    MGame * getGame() const;

public Q_SLOTS:
    void runGame();

Q_SIGNALS:
    void restart();

private:
    QPointer<MMainWindow> m_main;
    QPointer<QMainWindow> m_window;
    QPointer<MGame> m_game;
};

};

#endif // MGAMECONFIG_H
