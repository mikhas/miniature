/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef LOCAL_GAME_H__
#define LOCAL_GAME_H__

#include <position.h>
#include <board_view.h>
#include <graphics_board_item.h>

#include <game.h>
#include <game_log.h>
#include <game_store.h>

#include <QtCore>
#include <QtGui>

namespace Miniature
{

//! This class specializes MGame for 2-player local games on one device.
class MLocalGame
    : public MGame
{
    Q_OBJECT

public:
    explicit MLocalGame(const MSharedGameLog &log,
                        QObject *parent = 0);
    virtual ~MLocalGame();

private:
    virtual void setupDashboard();
    void startTurn(const MPosition &position);
    void connectDashboards(MDashboardItem *const first,
                           MDashboardItem *const second);
};

} // namespace Miniature
#endif // LOCAL_GAME_H__
