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

#include <iconic_button.h>

using namespace Miniature;

namespace
{
int min_button_size = 70;
int font_extra_width = 70;
int font_extra_height = 30;
int font_size = 20;
}

MIconicButton::
MIconicButton(const QPixmap &pixmap, const QString &label, QWidget *parent)
: QWidget(parent),
  m_pixmap(pixmap),
  m_label(label)
{
    resize(sizeHint());
    QSizePolicy fixed = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setSizePolicy(fixed);
}

MIconicButton::
~MIconicButton()
{}

QSize MIconicButton::
sizeHint() const
{
    return QSize(qMax(min_button_size, m_pixmap.width()) + font_extra_width,
                 qMax(min_button_size, m_pixmap.height()) + font_extra_height);
}

void MIconicButton::
enable()
{
    setEnabled(true);
}

void MIconicButton::
disable()
{
    setEnabled(false);
}

void MIconicButton::
paintEvent (QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);

    painter.drawPixmap(QRect(event->rect().x() + ((width() * 0.5) - (m_pixmap.width() * 0.5)),
                             event->rect().y(),
                             m_pixmap.width(),
                             m_pixmap.height()),
                       m_pixmap, m_pixmap.rect());

    QFont font;
    font.setPixelSize(font_size);
    font.setWeight(QFont::Bold);
    painter.setPen(QColor(Qt::white));
    painter.drawText(QRect(event->rect().x(),
                           event->rect().y() + m_pixmap.height(),
                           width(),
                           font_extra_height),
                     Qt::AlignCenter, m_label);
}

void MIconicButton::
mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    Q_EMIT pressed();
}
