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

#include <dashboard_item.h>
#include <QGraphicsEllipseItem>
#include <QGraphicsBlurEffect>
#include <QPen>
#include <QBrush>

//#include <glib.h>
//#include <libosso-abook/osso-abook.h>

using namespace Miniature;

namespace {
    const int width = 480;
    const int height = 140;
    const int col_left_width = 120;
    const int col_right_width = 120;
    const int icon_size = 64;
}

MDashboardItem::
MDashboardItem(QGraphicsItem *parent)
: QGraphicsObject(parent),
  m_confirm(0),
  m_request(0),
  m_take_back(0),
  m_avatar(0)
{
    setEnabled(false);
    setupUi();
    setTransformOriginPoint(width * 0.5, height * 0.5);
    show();
}

MDashboardItem::
~MDashboardItem()
{}

void MDashboardItem::
setupUi()
{
    // Maemo5-specific icons, handle press states by replacing pixmap, or have two pixmap per item?
    const int centered_width  = (width  - icon_size) * 0.5;
    const int centered_height = (height - icon_size) * 0.5;

    setupButton(m_confirm,
                QPoint(centered_width, centered_height),
                QPixmap("/usr/share/themes/alpha/mediaplayer/Play.png"));

    setupButton(m_request,
                QPoint(width - icon_size - col_right_width, centered_height), // right-aligned
                QPixmap("/usr/share/themes/alpha/mediaplayer/Stop.png"));

    setupButton(m_take_back,
                QPoint(col_left_width, centered_height), // left-aligned
                QPixmap("/usr/share/themes/alpha/mediaplayer/Back.png"));

    QPixmap *avatar = getContactsAvatar(QString("qgil"));
    if (avatar)
        m_avatar = new QGraphicsPixmapItem(*avatar, this);
    else
        m_avatar = new QGraphicsPixmapItem(this); // empty

    m_avatar->setPos(QPoint(0, 0)); // left-aligned
    m_avatar->show();
    m_avatar->setEnabled(true);
}

void MDashboardItem::
setupButton(QGraphicsPixmapItem *item, const QPoint &origin, const QPixmap &pixmap)
{
    item = new QGraphicsPixmapItem(pixmap, this);
    item->setPos(origin);
    item->show();
    item->setEnabled(true);
    item->setTransformationMode(Qt::SmoothTransformation);

    const QRect ellipse_extent = QRect(0,0, icon_size, icon_size);
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(ellipse_extent, item);

    QPen ellipse_border = QPen(Qt::white);
    ellipse_border.setCosmetic(true);

    ellipse->setPen(ellipse_border);
    ellipse->setBrush(QBrush(Qt::transparent));
    ellipse->show();
    ellipse->setEnabled(false);
    ellipse->setFlags(QGraphicsItem::ItemStacksBehindParent);

    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(2);
    ellipse->setGraphicsEffect(blur);
}

QPixmap * MDashboardItem::
getContactsAvatar(const QString &nick)
{
    Q_UNUSED(nick)

    QPixmap *pixmap = 0;

/*
    OssoABookContact *contact = 0;
    GdkPixbuf *pixbuf = osso_abook_contact_get_avatar_pixbuf(contact, QString(nick).toAscii().data(), 0);
    QImage image = QImage(static_cast<const uchar *>(gdk_pixbuf_get_pixels(pixbuf)), 20, 20, QImage::Format_ARGB32);
*/

    pixmap = new QPixmap();
//    pixmap->fromImage(image);

    return pixmap;
}

QRectF MDashboardItem::
boundingRect() const
{
    return QRectF(QPoint(0,0), QPoint(width, height));
}
