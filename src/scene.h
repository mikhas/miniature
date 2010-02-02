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

#ifndef SCENE_H__
#define SCENE_H__

#include <QGraphicsScene>

namespace Miniature
{

/*! This class adds support for "modal" QGraphicsItems, that is, to make it
 *  work similar to modal dialogs on Maemo 5 (no cancel button etc.). It
 *  doesn't have the blur effect yet, and might never will.
 */
class MScene
: public QGraphicsScene
{
    Q_OBJECT

public:
    explicit MScene(QObject *parent = 0);
    explicit MScene(const QRectF &region, QObject *parent = 0);
    virtual ~MScene();

    /*!
     *  In order to emulate the cancel behaviour of Maemo 5 we need to decide
     *  when to hide an "modal" item again. Here, it will be hidden once a
     *  click was received outside of the "modal" region.
     */
    void setModalItem(QGraphicsItem *item);

    /*!
     *  Convenience method for setModalItem(0).
     */
    void resetModalItem();

protected:
    /*!
     *  Overriden to check against modal regions.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    /*!
     *  Common initialization goes here.
     */
    void setup();

    QGraphicsItem *m_modal_item; /*!< The modal item. If non-zero, each click
                                      will be checked against the modal region
                                      set. */
    QGraphicsPixmapItem *m_blur_item; /*!< The item for the blurred vision of
                                           the view. */
};

} // namespace Miniature

#endif
