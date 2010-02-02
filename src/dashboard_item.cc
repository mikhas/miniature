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
#include <scene.h>

#include <QtCore>
#include <QtGui>

//#include <glib.h>
//#include <libosso-abook/osso-abook.h>

using namespace Miniature;

namespace
{

const int width = 480;
const int height = 160;
const int col_left_width = 120;
const int col_right_width = 120;
const int icon_size = 64;
const int flash_duration = 200;
const int fadeout_duration = 500;
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
  m_requests_dialog(new QGraphicsProxyWidget(this)),
  m_offers_dialog(new QGraphicsProxyWidget(this)),
  m_status(new QGraphicsTextItem(this)),
  m_status_anim(new QPropertyAnimation(m_status, "opacity", this)),
  m_last_moves(new QGraphicsTextItem(this))
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
    // TODO: use anchor layouts, this is a fantastic playground for them!

    // Maemo5-specific icons, handle press states by replacing pixmap, or have two pixmap per item?
    const int centered_width  = (width  - icon_size) * 0.5;
    const int centered_height = 60 + ((height - icon_size - 60) * 0.5); // there is a 1st row w/ 60 pixels.

    QWidget *request_widget = new QWidget;
    request_widget->setWindowTitle(tr("?? Choose a Game Resolution"));
    request_widget->resize(width, height);

    QWidget *offers_widget = new QWidget;
    offers_widget->setWindowTitle(tr("?? Accept Offer?"));
    offers_widget->resize(width, height);

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black); // TODO: This - the use of black - is not ok, either.
    m_requests_dialog->setPalette(palette);
    m_requests_dialog->setWidget(request_widget);
    m_requests_dialog->setZValue(1); // make sure it is drawn on top of its siblings, the buttons.
    m_requests_dialog->setEnabled(false);
    m_requests_dialog->hide();

    m_offers_dialog->setPalette(palette);
    m_offers_dialog->setWidget(offers_widget);
    m_offers_dialog->setZValue(1);
    m_offers_dialog->setEnabled(false);
    m_offers_dialog->hide();

    QGridLayout *grid = new QGridLayout;
    grid->setColumnMinimumWidth(0, width * 0.5 - 12);
    grid->setColumnMinimumWidth(1, width * 0.5 - 12);
    request_widget->setLayout(grid);

    QPushButton *pause_button = new QPushButton(tr("?? Pause Game"));
    grid->addWidget(pause_button, 0, 0);

    QPushButton *draw_button = new QPushButton(tr("?? Propose Draw"));
    grid->addWidget(draw_button, 0, 1);
    connect(draw_button, SIGNAL(pressed()),
            this, SIGNAL(drawButtonPressed()));
    connect(draw_button, SIGNAL(pressed()),
            request_widget, SLOT(hide()));

    QPushButton *adjourn_button = new QPushButton(tr("?? Adjourn Game"));
    grid->addWidget(adjourn_button, 1, 0);

    QPushButton *resign_button = new QPushButton(tr("?? Resign"));
    grid->addWidget(resign_button, 1, 1);

    QFont font;
    font.setWeight(QFont::Bold);
    font.setPointSize(18);

    // TODO: Clean up the colors mess. Since we draw on a yellow background,
    // the status text is black. But the moves are shown on a black background
    // again. Maybe replace with this contrast-finding color method.

    m_status->hide();
    m_status->setFont(font);
    m_status->setDefaultTextColor(QColor(Qt::black));
    m_status->setTextInteractionFlags(Qt::NoTextInteraction);
    m_status->setTextWidth(width - col_left_width - col_right_width);
    m_status->setPos(QPoint(col_left_width, 20)); // below the board, centered

    QGraphicsRectItem *rect_status = new QGraphicsRectItem(m_status);
    rect_status->setFlags(QGraphicsItem::ItemStacksBehindParent);
    rect_status->setRect(QRect(0, 0, width - col_left_width - col_right_width, 40));
    rect_status->setBrush(QBrush(Qt::yellow)); // TODO: how to get the notification bg color?
    rect_status->show();

    m_status_anim->setDuration(fadeout_duration);
    m_status_anim->setStartValue(1.0);
    m_status_anim->setEndValue(0.0);

    connect(m_status_anim, SIGNAL(finished()),
            this, SLOT(hideStatus()));

    m_last_moves->show();
    m_last_moves->setFont(font);
    m_last_moves->setDefaultTextColor(QColor(Qt::white));
    m_last_moves->setTextInteractionFlags(Qt::NoTextInteraction);
    m_last_moves->setTextWidth(col_right_width);
    m_last_moves->setPos(QPoint(width - col_right_width, 20)); // below the board, right-aligned

    m_confirm = createButtonWithBackground(QPoint(centered_width, centered_height),
                                           QIcon("/usr/share/themes/alpha/mediaplayer/Play.png"));

    // right-aligned
    m_request = createButtonWithBackground(QPoint(width - icon_size - col_right_width, centered_height),
                                           QIcon("/usr/share/themes/alpha/mediaplayer/Stop.png"));

    // left-aligned
    m_takeback = createButtonWithBackground(QPoint(col_left_width, centered_height),
                                            QIcon("/usr/share/themes/alpha/mediaplayer/Back.png"));

    QPixmap *avatar = getContactsAvatar(QString("qgil"));
    QPixmap empty;
    if (avatar)
        m_avatar = new MDashboardButton(*avatar, this, 0);
    else
       m_avatar = new MDashboardButton(empty, this, 0);

    m_avatar->setPos(QPoint(10, centered_height)); // left-aligned
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

    connect(this, SIGNAL(requestButtonPressed()),
            this, SLOT(showRequestsMenu()));
}

void MDashboardItem::
resetUi()
{
    m_requests_dialog->hide();

    disableConfirmButton();
    m_request->setBackgroundBrush(Qt::transparent);
    m_takeback->setBackgroundBrush(Qt::transparent);
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
    static_cast<MScene *>(scene())->setModalItem(m_requests_dialog);
    m_requests_dialog->show();
    m_requests_dialog->setEnabled(true);
}

void MDashboardItem::
drawOffered()
{
    static_cast<MScene *>(scene())->setModalItem(m_offers_dialog);
    m_offers_dialog->show();
    m_offers_dialog->setEnabled(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    m_offers_dialog->widget()->setLayout(vbox);

    QLabel *label = new QLabel(tr("?? Your opponent offered to draw."));
    label->setAlignment(Qt::AlignCenter);
    vbox->addWidget(label);

    QPushButton *accept_button = new QPushButton(tr("?? Accept Draw"));
    vbox->addWidget(accept_button);

    connect(accept_button, SIGNAL(pressed()),
            this, SIGNAL(drawAccepted()));

    connect(accept_button, SIGNAL(pressed()),
            m_offers_dialog->widget(), SLOT(hide()));
}

void MDashboardItem::
onDrawAccepted()
{
    setStatusText(tr("Game ended in a draw"));
}

void MDashboardItem::
flash()
{
    m_confirm->flash();
    m_takeback->flash();
    m_request->flash();
}

void MDashboardItem::
fadeOutStatus()
{
    m_status_anim->start();
}

void MDashboardItem::
hideStatus()
{
    m_status->hide();
}

void MDashboardItem::
setStatusText(const QString &text)
{
    m_status_anim->stop();
    m_status->setOpacity(1.0);
    QTimer::singleShot(3000, this, SLOT(fadeOutStatus()));

    // Oh dont look at me! It's the borken HTML subset for Qt's rich text that made me use tables.
    m_status->setHtml(QString("<table width=\"100%\" border=\"0\"><tr><td align=\"center\">%1</td></tr></table>").arg(text));
    m_status->show();
}

void MDashboardItem::
appendToLastMovesList(const QString &move_notation)
{
    // TODO: The idea is later to have a list of n moves to be shown in a
    // deque, the newest arriving on top, removing the last.
    m_last_moves->setHtml(move_notation);
}
