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

#include <QtCore>
#include <QtGui>

namespace Miniature
{
/*!
 *  A helper class to create clickable images. Using pixmaps on push buttons
 *  doesn't quite yield the desired effect.
 */
class MIconicButton
: public QWidget
{
    Q_OBJECT

public:
    explicit MIconicButton(const QPixmap &pixmap, const QString &label, QWidget *parent = 0);
    virtual ~MIconicButton();
    virtual QSize sizeHint() const;

public Q_SLOTS:
    void enable();
    void disable();

Q_SIGNALS:
    void pressed();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    QPixmap m_pixmap;
    QString m_label;
};

}
