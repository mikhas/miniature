/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
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

#ifndef MINIATURE_H__
#define MINIATURE_H__

#include "miniature.ui.h"
#include "game.h"

#include <QMainWindow>

namespace Miniature
{

class MMainWindow
: public QMainWindow
{
    Q_OBJECT

public:
    MMainWindow();
    virtual ~MMainWindow();

private:
    /* Our internal game controller */
    Miniature::MGame m_game;

    /* A reference to the internal scene view */
    //QGraphicsView* m_view;

    Ui::MMainWindow m_ui;
};

}; // namespace Miniature

#endif // MINIATURE_H__
