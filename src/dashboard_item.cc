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
#include <dashboard_button.h>
#include <scene.h>

//#include <glib.h>
//#include <libosso-abook/osso-abook.h>

using namespace Miniature;

namespace
{

const int portrait_width = 480;
const int portrait_height = 160;

const int landscape_width = 320;
const int landscape_height = 240;

const QSize extent = QSize(portrait_width, portrait_height);

const int col_left_width = 120;
const int col_right_width = 120;
const int icon_size = 64;

const int fadeout_duration = 500;
const QColor flash_color = QColor(Qt::red);

}

MDashboardItem::
MDashboardItem(QGraphicsItem *parent)
: QGraphicsObject(parent),
  m_confirm(0),
  m_requests(0),
  m_takeback(0),
  m_avatar(0),
  m_requests_dialog(new QGraphicsProxyWidget(this)),
  m_game_dialog(new QGraphicsProxyWidget(this)),
  m_offers_dialog(new QGraphicsProxyWidget(this)),
  m_status(new QGraphicsTextItem(this)),
  m_status_anim(new QPropertyAnimation(m_status, "opacity", this)),
  m_last_moves(new QGraphicsTextItem(this))
{
    setEnabled(false);
    setupUi();
    applyPortraitLayout();
    setTransformOriginPoint(portrait_width * 0.5, portrait_height * 0.5);
    show();
}

MDashboardItem::
~MDashboardItem()
{}

void MDashboardItem::
setupUi()
{
    // Maemo5-specific icons, handle press states by replacing pixmap, or have two pixmap per item?
    const int centered_portrait_width  = (portrait_width  - icon_size) * 0.5;
    const int centered_portrait_height = 40 + ((portrait_height - icon_size - 40) * 0.5); // there is a 1st row w/ 40 pixels.

    const int centered_landscape_width = (landscape_width - icon_size) * 0.5;
    //const int centered_landscape_height = 40 + ((landscape_height - icon_size -40) * 0.5);

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black); // TODO: This - the use of black - is not ok, either.
    m_requests_dialog->setPalette(palette);
    m_requests_dialog->setZValue(1); // make sure it is drawn on top of its siblings, the buttons.
    m_requests_dialog->setEnabled(false);
    m_requests_dialog->hide();

    m_game_dialog->setPalette(palette);
    m_game_dialog->setZValue(1);
    m_game_dialog->setEnabled(false);
    m_game_dialog->hide();

    m_offers_dialog->setPalette(palette);
    m_offers_dialog->setZValue(1);
    m_offers_dialog->setEnabled(false);
    m_offers_dialog->hide();

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
    m_status->setTextWidth(portrait_width - col_left_width - col_right_width);
    m_status->setPos(QPoint(col_left_width, 0)); // below the board, centered

    m_portrait[m_status]  = MLayout(m_status);
    m_landscape[m_status] = MLayout(QPointF((landscape_width - m_status->textWidth()) * 0.5,
                                            landscape_height - m_status->boundingRect().height()));

    QGraphicsRectItem *rect_status = new QGraphicsRectItem(m_status);
    rect_status->setFlags(QGraphicsItem::ItemStacksBehindParent);
    rect_status->setRect(QRect(0, 0, portrait_width - col_left_width - col_right_width, 40));
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
    m_last_moves->setPos(QPoint(portrait_width - col_right_width, 0)); // below the board, right-aligned

    m_confirm = new MDashboardButton(QPoint(centered_portrait_width - 16, centered_portrait_height - 16),
                                     QPixmap(":/icons/96x96/play.png"),
                                     QPixmap(":/icons/96x96/play-inactive.png"),
                                     QPixmap(":/icons/96x96/play-pressed.png"),
                                     QPixmap(":/icons/96x96/play-flash.png"),
                                     this);

    m_portrait[m_confirm]  = MLayout(m_confirm);
    m_landscape[m_confirm] = MLayout(QPointF(centered_landscape_width - 16, landscape_height - 96 - 40 - 16));

    m_confirm->setActive(false);

    // right-aligned
    m_requests = new MDashboardButton(QPoint(portrait_width - icon_size - col_right_width, centered_portrait_height),
                                      QPixmap(":/icons/64x64/stop.png"),
                                      QPixmap(":/icons/64x64/stop-inactive.png"),
                                      QPixmap(":/icons/64x64/stop-pressed.png"),
                                      QPixmap(":/icons/64x64/stop-flash.png"),
                                      this);

    m_portrait[m_requests]  = MLayout(m_requests);
    m_landscape[m_requests] = MLayout(QPointF(landscape_width - icon_size - 16, landscape_height - icon_size - 32 - 40));

    m_requests->setActive(false);

    // left-aligned
    m_takeback = new MDashboardButton(QPoint(col_left_width, centered_portrait_height),
                                      QPixmap(":/icons/64x64/rewind.png"),
                                      QPixmap(":/icons/64x64/rewind-inactive.png"),
                                      QPixmap(":/icons/64x64/rewind-pressed.png"),
                                      QPixmap(":/icons/64x64/rewind-flash.png"),
                                      this);

    m_portrait[m_takeback]  = MLayout(m_takeback);
    m_landscape[m_takeback] = MLayout(QPointF(16, landscape_height - icon_size - 32 - 40));

    m_takeback->setActive(false); // TODO: not implemented yet

    QPixmap *avatar = getContactsAvatar(QString("qgil"));
    QPixmap empty;
    if (avatar)
        m_avatar = new MDashboardButton(QPoint(10, centered_portrait_height),
                                        *avatar, *avatar, *avatar, *avatar, this, 0);
    else
       m_avatar = new MDashboardButton(QPoint(10, centered_portrait_height),
                                       empty, empty, empty, empty, this, 0);

    m_portrait[m_avatar]  = MLayout(m_avatar);
    m_landscape[m_avatar] = MLayout(QPointF(16, landscape_height - (96 + icon_size + 32 + 40)));

    // now that the buttons exist we can connect their signals (or rather, forward them)
    connect(m_confirm, SIGNAL(pressed()), this, SIGNAL(confirmButtonPressed()));

    connect(m_requests, SIGNAL(pressed()), this, SIGNAL(requestsButtonPressed()));
    connect(m_requests, SIGNAL(pressed()), this, SLOT(showRequestsMenu()));

    connect(m_takeback, SIGNAL(pressed()), this, SIGNAL(takebackButtonPressed()));

    connect(m_avatar, SIGNAL(pressed()), this, SIGNAL(avatarButtonPressed()));
    connect(m_avatar, SIGNAL(pressed()), this, SLOT(showGameMenu()));
}

void MDashboardItem::
resetUi()
{
    m_requests_dialog->hide();
    m_offers_dialog->hide();

    m_confirm->restore();
    m_requests->restore();
    m_takeback->restore();
}

void MDashboardItem::
applyPortraitLayout()
{
    MLayout::applyToItem(m_status, m_portrait);
    MLayout::applyToItem(m_confirm, m_portrait);
    MLayout::applyToItem(m_requests, m_portrait);
    MLayout::applyToItem(m_takeback, m_portrait);
    MLayout::applyToItem(m_avatar, m_portrait);
}

void MDashboardItem::
applyLandscapeLayout()
{
    MLayout::applyToItem(m_status, m_landscape);
    MLayout::applyToItem(m_confirm, m_landscape);
    MLayout::applyToItem(m_requests, m_landscape);
    MLayout::applyToItem(m_takeback, m_landscape);
    MLayout::applyToItem(m_avatar, m_landscape);
}


// MDashboardItem private:
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

bool MDashboardItem::
isPortraitModeEnabled() const
{
    if (!scene())
        return true; // wild guess

    return (scene()->width() < scene()->height());
}


void MDashboardItem::
showConfirmationDialog(const QString &title, QPushButton *button)
{
    static_cast<MScene *>(scene())->setModalItem(m_offers_dialog);

    if (!m_offers_dialog->widget())
    {
        QWidget *dialog = new QWidget;
        dialog->resize(portrait_width, portrait_height);

        QVBoxLayout *vbox = new QVBoxLayout;
        dialog->setLayout(vbox);

        QLabel *label = new QLabel(title);
        label->setAlignment(Qt::AlignCenter);
        vbox->addWidget(label);

        if (button)
        {
            vbox->addWidget(button);
            connect(button, SIGNAL(pressed()), dialog, SLOT(hide()));
        }

        m_offers_dialog->setWidget(dialog);
    }

    // Need to manually transform proxy widgets, since the parent is a QGraphicsWidget
    // (managed by a layout), and this is wonderfully borken.
    m_offers_dialog->setTransformOriginPoint(m_offers_dialog->boundingRect().center());
    m_offers_dialog->setRotation(rotation());
    m_offers_dialog->show();
    m_offers_dialog->setEnabled(true);
}

void MDashboardItem::
showEndGameUi()
{
    //resetUi();

    // Hide all buttons
    m_confirm->hide();
    m_requests->hide();
    m_takeback->hide();
    m_avatar->hide();

    // Show a single button to end the game
    QGraphicsProxyWidget *dialog_proxy = new QGraphicsProxyWidget(this);
    QWidget *dialog = new QWidget;
    dialog->resize(portrait_width, portrait_height - 60);
    dialog_proxy->setWidget(dialog);

    // Need to manually transform proxy widgets, since the parent is a QGraphicsWidget
    // (managed by a layout), and this is wonderfully borken.
    dialog_proxy->setTransformOriginPoint(dialog_proxy->boundingRect().center());
    dialog_proxy->setRotation(rotation());
    dialog_proxy->show();

    QVBoxLayout *vbox = new QVBoxLayout;
    dialog->setLayout(vbox);

    QPushButton *end_game = new QPushButton("End game");
    vbox->addWidget(end_game);

    connect(end_game, SIGNAL(pressed()), this, SIGNAL(abortGameConfirmed()));
}

// MDashboardItem protected:

QRectF MDashboardItem::
boundingRect() const
{
    return QRectF(QPoint(0,0), QPoint(portrait_width, portrait_height));
}

// MDashboardItem public Q_SLOTS:

void MDashboardItem::
enableConfirmButton()
{
    Q_ASSERT(0 != m_confirm);

    m_confirm->setActive(true);
}

void MDashboardItem::
disableConfirmButton()
{
    Q_ASSERT(0 != m_confirm);

    m_confirm->setActive(false);
}

void MDashboardItem::
enableRequestsButton()
{
    Q_ASSERT(0 != m_requests);

    m_requests->setActive(true);
}

void MDashboardItem::
disableRequestsButton()
{
    Q_ASSERT(0 != m_requests);

    m_requests->setActive(false);
}

void MDashboardItem::
showRequestsMenu()
{
    static_cast<MScene *>(scene())->setModalItem(m_requests_dialog);

    if(!m_requests_dialog->widget())
    {
        QWidget *dialog = new QWidget;
        dialog->resize(portrait_width, portrait_height + 60);

        QVBoxLayout *vbox = new QVBoxLayout;
        dialog->setLayout(vbox);

        QLabel *title = new QLabel(tr("?? Request Menu"));
        title->setAlignment(Qt::AlignCenter);
        vbox->addWidget(title);

        QWidget *grid_widget = new QWidget;
        vbox->addWidget(grid_widget);

        QGridLayout *grid = new QGridLayout;
        grid->setColumnMinimumWidth(0, portrait_width * 0.5 - 12);
        grid->setColumnMinimumWidth(1, portrait_width * 0.5 - 12);
        grid_widget->setLayout(grid);

        QPushButton *pause_button = new QPushButton(tr("?? Pause Game"));
        grid->addWidget(pause_button, 0, 0);

        QPushButton *draw_button = new QPushButton(tr("?? Propose Draw"));
        connect(draw_button, SIGNAL(pressed()), dialog, SLOT(hide()));
        connect(draw_button, SIGNAL(pressed()), this, SIGNAL(drawButtonPressed()));
        grid->addWidget(draw_button, 0, 1);

        QPushButton *adjourn_button = new QPushButton(tr("?? Adjourn Game"));
        connect(adjourn_button, SIGNAL(pressed()), dialog, SLOT(hide()));
        connect(adjourn_button, SIGNAL(pressed()), this, SIGNAL(adjournButtonPressed()));
        grid->addWidget(adjourn_button, 1, 0);

        QPushButton *resign_button = new QPushButton(tr("?? Resign"));
        connect(resign_button, SIGNAL(pressed()), dialog, SLOT(hide()));
        connect(resign_button, SIGNAL(pressed()), this, SIGNAL(resignButtonPressed()));
        grid->addWidget(resign_button, 1, 1);

        m_requests_dialog->setWidget(dialog);
        dialog->show();
    }

    // Need to manually transform proxy widgets, since the parent is a QGraphicsWidget
    // (managed by a layout), and this is wonderfully borken.
    m_requests_dialog->setTransformOriginPoint(m_requests_dialog->boundingRect().center());
    m_requests_dialog->setRotation(rotation());
    m_requests_dialog->show();
    m_requests_dialog->setEnabled(true);
}

void MDashboardItem::
showGameMenu()
{
    static_cast<MScene *>(scene())->setModalItem(m_game_dialog);

    if(!m_game_dialog->widget())
    {
        QWidget *dialog = new QWidget;
        dialog->resize(portrait_width, portrait_height);

        QVBoxLayout *vbox = new QVBoxLayout;
        dialog->setLayout(vbox);

        QLabel *title = new QLabel(tr("?? Game Menu"));
        title->setAlignment(Qt::AlignCenter);
        vbox->addWidget(title);

        QPushButton *abort_game_button = new QPushButton(tr("?? Abort Game"));
        vbox->addWidget(abort_game_button);

        connect(abort_game_button, SIGNAL(pressed()), dialog, SLOT(hide()));
        connect(abort_game_button, SIGNAL(pressed()), this, SIGNAL(abortGameButtonPressed()));

        QPushButton *show_game_log_button = new QPushButton(tr("?? Show Game Log"));
        vbox->addWidget(show_game_log_button);

        connect(show_game_log_button, SIGNAL(pressed()), dialog, SLOT(hide()));
        connect(show_game_log_button, SIGNAL(pressed()), this, SIGNAL(showGameLogButtonPressed()));

        m_game_dialog->setWidget(dialog);
        dialog->show();
    }

    // Need to manually transform proxy widgets, since the parent is a QGraphicsWidget
    // (managed by a layout), and this is wonderfully borken.
    m_game_dialog->setTransformOriginPoint(m_game_dialog->boundingRect().center());
    m_game_dialog->setRotation(rotation());
    m_game_dialog->show();
    m_game_dialog->setEnabled(true);
}

void MDashboardItem::
drawOffered()
{
    QPushButton *accept_button = new QPushButton(tr("?? Accept Draw"));
    showConfirmationDialog(tr("?? Draw offered"), accept_button);

    connect(accept_button, SIGNAL(pressed()), this, SIGNAL(drawAccepted()));
}

void MDashboardItem::
onDrawAccepted()
{
    setStatusText(tr("Game drawed"));
    showEndGameUi();
}

void MDashboardItem::
adjournOffered()
{
    QPushButton *accept_button = new QPushButton(tr("?? Adjourn Game"));
    showConfirmationDialog(tr("?? Request to Adjourn the Game"), accept_button);

    connect(accept_button, SIGNAL(pressed()), this, SIGNAL(adjournAccepted()));
}

void MDashboardItem::
onAdjournAccepted()
{
    setStatusText(tr("Game was adjourned"));
}

void MDashboardItem::
showResignConfirmation()
{
    QPushButton *confirm_button = new QPushButton(tr("?? Yes, resign"));
    showConfirmationDialog(tr("?? Do you want to resign?"), confirm_button);

    connect(confirm_button, SIGNAL(pressed()), this, SIGNAL(resignConfirmed()));
}

void MDashboardItem::
showAbortGameConfirmation()
{
    QPushButton *confirm_button = new QPushButton(tr("?? Yes, abort game"));
    showConfirmationDialog(tr("?? Do you want to abort the game?"), confirm_button);

    connect(confirm_button, SIGNAL(pressed()), this, SIGNAL(abortGameConfirmed()));
}

void MDashboardItem::
onGameWon()
{
    setStatusText(tr("Game won!"));
    showEndGameUi();
}

void MDashboardItem::
onGameLost()
{
    setStatusText(tr("Game lost!"));
    showEndGameUi();
}

void MDashboardItem::
flash()
{
    m_confirm->flash();
    m_takeback->flash();
    m_requests->flash();
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
    // Resetting the blur effect is quite hackish - we rely on moves setting a
    // status etc. TODO: find a better place!
    static_cast<MScene *>(scene())->resetModalItem();
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
