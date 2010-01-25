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
#include <QDialog>

// platform-dependent!
#include <QtMaemo5>

//#include <glib.h>
//#include <libosso-abook/osso-abook.h>

using namespace Miniature;

namespace
{

const int width = 480;
const int height = 140;
const int col_left_width = 120;
const int col_right_width = 120;
const int icon_size = 64;
const int flash_duration = 250;
const QColor flash_color = QColor(Qt::red);

}

MDashboardButton::
MDashboardButton(const QIcon &icon, QGraphicsItem *item, QObject *parent)
: QObject(parent),
  QGraphicsPixmapItem(icon.pixmap(icon_size, icon_size), item),
  m_background(0),
  m_stored_background_brush(QBrush(Qt::transparent)),
  m_icon(icon),
  m_active(true)
{
    setEnabled(true); // receive scene events
}

MDashboardButton::
~MDashboardButton()
{}

void MDashboardButton::
setupButtonBackground()
{
    const QRect ellipse_extent = QRect(0,0, icon_size, icon_size);
    m_background = new QGraphicsEllipseItem(ellipse_extent, this);

    QPen ellipse_border = QPen(Qt::white);
    ellipse_border.setCosmetic(true);

    m_background->setPen(ellipse_border);
    m_background->setBrush(m_stored_background_brush);
    m_background->show();
    m_background->setEnabled(false);
    m_background->setFlags(QGraphicsItem::ItemStacksBehindParent);

    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(2);
    m_background->setGraphicsEffect(blur);
}

void MDashboardButton::
setBackgroundBrush(const QBrush &brush)
{
    if (!m_background)
        return; // nothing to do!

    m_background->setBrush(brush);
}

void MDashboardButton::
storeBackgroundBrush()
{
    // Make sure to never store the brush's flash color, else triggering flash
    // while a flash is already running becomes racy.
    if (!m_background || flash_color ==  m_background->brush().color())
        return; // nothing to do!

    m_stored_background_brush = m_background->brush();
}

void MDashboardButton::
restoreBackgroundBrush()
{
    setBackgroundBrush(m_stored_background_brush);
}

void MDashboardButton::
flash()
{
    storeBackgroundBrush();
    setBackgroundBrush(QBrush(flash_color));
    QTimer::singleShot(flash_duration, this, SLOT(restoreBackgroundBrush()));
}


void MDashboardButton::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept(); // stop event propagation

    if (m_background)
    {
        const QBrush flash = QBrush(Qt::red);
        m_background->setBrush(flash);
    }

    Q_EMIT pressed();
}

void MDashboardButton::
mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept(); // stop event propagation

    if (m_background)
    {
        const QBrush trans = QBrush(Qt::transparent);
        m_background->setBrush(trans);
    }

    Q_EMIT released();
}

MDashboardItem::
MDashboardItem(QGraphicsItem *parent)
: QGraphicsObject(parent),
  m_confirm(0),
  m_request(0),
  m_takeback(0),
  m_avatar(0),
  m_fullscreen(0),
  m_fullscreen_enabled(false)
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

    m_confirm = createButtonWithBackground(QPoint(centered_width, centered_height),
                                           QIcon("/usr/share/themes/alpha/mediaplayer/Play.png"));

    // right-aligned
    m_request = createButtonWithBackground(QPoint(width - icon_size - col_right_width, centered_height),
                                           QIcon("/usr/share/themes/alpha/mediaplayer/Stop.png"));

    // left-aligned
    m_takeback = createButtonWithBackground(QPoint(col_left_width, centered_height),
                                            QIcon("/usr/share/themes/alpha/mediaplayer/Back.png"));

    m_fullscreen = new MDashboardButton(QIcon::fromTheme("general_fullsize"), this, 0);
    m_fullscreen->setPos(QPoint(10, 76));

    QPixmap *avatar = getContactsAvatar(QString("qgil"));
    QPixmap empty;
    if (avatar)
        m_avatar = new MDashboardButton(*avatar, this, 0);
    else
       m_avatar = new MDashboardButton(empty, this, 0);

    m_avatar->setPos(QPoint(10, 6)); // left-aligned
    m_avatar->show();
    m_avatar->setEnabled(true);

    // now that the buttons exist we can connect their signals (or rather, forward them)
    connect(m_confirm, SIGNAL(pressed()),
            this, SIGNAL(confirmButtonPressed()));

    connect(m_request, SIGNAL(pressed()),
            this, SIGNAL(requestButtonPressed()));

    connect(m_takeback, SIGNAL(pressed()),
            this, SIGNAL(requestButtonPressed()));

    connect(m_avatar, SIGNAL(pressed()),
            this, SIGNAL(avatarButtonPressed()));

    connect(m_fullscreen, SIGNAL(pressed()),
            this, SIGNAL(fullscreenButtonPressed()));

    connect(this, SIGNAL(requestButtonPressed()),
            this, SLOT(showRequestsMenu()));

    connect(this, SIGNAL(fullscreenButtonPressed()),
            this, SLOT(toggleFullscreen()));
}

MDashboardButton * MDashboardItem::
createButtonWithBackground(const QPoint &origin, const QIcon &icon)
{
    MDashboardButton *button = new MDashboardButton(icon, this, 0);
    button->setPos(origin);
    button->show();
    button->setEnabled(true);
    button->setTransformationMode(Qt::SmoothTransformation);
    button->setupButtonBackground();

    return button;
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

    pixmap = new QPixmap(":/avatar_mockup.png");
//    pixmap->fromImage(image);

    return pixmap;
}

QRectF MDashboardItem::
boundingRect() const
{
    return QRectF(QPoint(0,0), QPoint(width, height));
}

void MDashboardItem::
enableConfirmButton()
{
    Q_CHECK_PTR(m_confirm);

    m_confirm->setEnabled(true);
    m_confirm->setBackgroundBrush(QBrush(Qt::green));
}

void MDashboardItem::
disableConfirmButton()
{
    Q_CHECK_PTR(m_confirm);

    m_confirm->setEnabled(false);
    m_confirm->setBackgroundBrush(QBrush(Qt::transparent));
}

void MDashboardItem::
showRequestsMenu()
{
    // TODO: this menu needs to be generated from the main menu actions, or not?

    QMaemo5ValueButton *fullscreen_button = new QMaemo5ValueButton(QIcon::fromTheme("general_fullsize"), tr("?? Toggle Fullscreen Mode"));

    QMaemo5ValueButton *new_p2p_game_button = new QMaemo5ValueButton(QIcon::fromTheme("general_default_avatar"), tr("?? New P2P Game"));

    fullscreen_button->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    connect(fullscreen_button, SIGNAL(pressed()),
            this, SIGNAL(toggleFullscreen()));

    QDialog *dialog = new QDialog(QApplication::activeWindow());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(tr("?? Choose a Game Resolution"));

    QVBoxLayout *vbox = new QVBoxLayout;
    dialog->setLayout(vbox);
    vbox->addWidget(new_p2p_game_button);
    vbox->addWidget(fullscreen_button);
    dialog->show();
}

void MDashboardItem::
toggleFullscreen()
{
    QWidget *window = QApplication::activeWindow();
    const Qt::WindowStates states = window->windowState();

    window->setWindowState(states & Qt::WindowFullScreen ? states & ~Qt::WindowFullScreen
                                                         : states | Qt::WindowFullScreen);

    m_fullscreen_enabled = !m_fullscreen_enabled;
}

void MDashboardItem::
flash()
{
    m_confirm->flash();
    m_takeback->flash();
    m_request->flash();
}
