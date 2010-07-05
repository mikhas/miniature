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

#ifndef NETWORK_GAME_H__
#define NETWORK_GAME_H__

#include <game.h>
#include <telepathy/tpgame.h>

namespace Miniature
{

class MNetworkGame
    : public Miniature::MGame
{
    Q_OBJECT

public:
    explicit MNetworkGame(const MSharedGameLog &log,
                          QObject *parent = 0);
    virtual ~MNetworkGame();

public Q_SLOTS:
    void hostGame();
    void joinGame();
    void disconnect();
    void newGame();

Q_SIGNALS:
    void disconnected();
    void connected();

private Q_SLOTS:
    void hostGameConnected();
    void joinGameConnected();
    void onConfirmButtonPressed();
    void receivedNewGame(bool white_chose);
    void receivedMove(const QString& fen);

protected:
    virtual bool isWhiteAtBottom() const;
    virtual bool isBlackAtBottom() const;

private:
    void setupDashboard();
    void onWhiteToMove(const MPosition &position);
    void onBlackToMove(const MPosition &position);
    void endTurn();

    // TODO: turn this int some interface later, once we have inet + tp.
    TpGame::Game *m_tp_game;

    bool m_white_at_bottom;
};

} // namespace Miniature

#endif // NETWORK_GAME_H__
