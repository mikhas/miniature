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

#include "config.h"
#include "miniature.h"
#include "board_view.h"

#include <QTreeWidgetItem>
#include <QStringList>

using namespace Miniature;

MMainWindow::MMainWindow()
: QMainWindow(),
  m_game(0)
{
    m_ui.setupUi(this);
    m_game = new MGame(m_ui.board_view, this);

    updateGameInfo();
    // Can be done in the Qt Designer, wow!
    //setupMoveListView();

    // Connect game logic w/ player info view.
    connect(m_game, SIGNAL(gameInfoChanged()),
            this, SLOT(updateGameInfo()));

    // Add debugging output to Miniature app.
    connect(m_game, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));
    connect(m_ui.board_view, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));

    // Connect actions.
    connect(m_ui.new_game, SIGNAL(triggered()),
            this, SLOT(clearMoveList()));
    connect(m_ui.new_game, SIGNAL(triggered()),
            m_game, SLOT(newGame()));

    // Connect game navi actions
    connect(m_ui.jump_to_start, SIGNAL(triggered()),
            m_game, SLOT(jumpToStart()));
    connect(m_ui.prev_move, SIGNAL(triggered()),
            m_game, SLOT(prevMove()));
    connect(m_ui.next_move, SIGNAL(triggered()),
            m_game, SLOT(nextMove()));
    connect(m_ui.jump_to_end, SIGNAL(triggered()),
            m_game, SLOT(jumpToEnd()));

    connect(m_ui.rotate_black_pieces, SIGNAL(triggered()),
            m_game, SLOT(rotateBlackPieces()));
    connect(m_ui.toggle_debug_output, SIGNAL(triggered()),
            this, SLOT(toggleDebugOutput()));

    // Chain up UI elements for game navi to their corresponding actions
    connect(m_ui.jump_to_start_button, SIGNAL(pressed()),
            m_ui.jump_to_start, SLOT(trigger()));
    connect(m_ui.prev_move_button, SIGNAL(pressed()),
            m_ui.prev_move, SLOT(trigger()));
    connect(m_ui.next_move_button, SIGNAL(pressed()),
            m_ui.next_move, SLOT(trigger()));
    connect(m_ui.jump_to_end_button, SIGNAL(pressed()),
            m_ui.jump_to_end, SLOT(trigger()));

    // Chain up UI elements for switching to the move list view/board view to a single actions.
    connect(m_ui.move_list_button, SIGNAL(pressed()),
            m_ui.show_move_list, SLOT(trigger()));
    connect(m_ui.board_button, SIGNAL(pressed()),
            m_ui.show_board, SLOT(trigger()));
    // switching a to a stacked widget via menu probably breaks Hildon UI spec, oh well ..
    connect(m_ui.new_game, SIGNAL(triggered()),
            m_ui.show_board, SLOT(trigger()));

    // Connect the show move list action to show the right stacked widget.
    connect(m_ui.show_move_list, SIGNAL(triggered()),
            this, SLOT(showMoveList()));
    connect(m_ui.show_board, SIGNAL(triggered()),
            this, SLOT(showBoard()));
    // Enable direct move selection from the move list.
    connect(m_ui.move_list, SIGNAL(itemPressed(QTreeWidgetItem *, int)),
            this, SLOT(onMoveListItemPressed(QTreeWidgetItem *, int)));

    // Connect action to show about dialog
    connect(m_ui.about, SIGNAL(triggered()),
            this, SLOT(showAboutDialog()));

    // Fix the font sizes, the Maemo5 style is totally wrong regarding that.
    QFont small_font("helvetica", 8, QFont::Light);
    QFont big_font("helvetica", 16, QFont::Bold);
    QFont normal_font("helvetica", 14, QFont::DemiBold);
    m_ui.white_name->setFont(big_font);
    m_ui.white_rating->setFont(normal_font);
    m_ui.white_material->setFont(normal_font);

    m_ui.black_name->setFont(big_font);
    m_ui.black_rating->setFont(normal_font);
    m_ui.black_material->setFont(normal_font);

    // Hide rating + material for both players - we haven't yet decided on the
    // kind of info we need.
    m_ui.info_box->hide();
    m_ui.player_box->hide();

    m_ui.debug->setFont(small_font);
    m_ui.debug->hide();

#ifdef Q_WS_MAEMO_5
    // TODO: query orientation before
    setAttribute(Qt::WA_Maemo5ForcePortraitOrientation, true);
    setAttribute(Qt::WA_Maemo5ForceLandscapeOrientation, false);
#endif
}

MMainWindow::~MMainWindow()
{}

void MMainWindow::show()
{
    QMainWindow::show();
    m_game->newGame();
}

void MMainWindow::setupMoveListView()
{
    QStringList labels;
    labels << QString(tr("#"))
           << QString(tr("White"))
           << QString(tr("Black"));

    m_ui.move_list->setColumnCount(3);
    m_ui.move_list->setHeaderLabels(labels);
}

void MMainWindow::clearMoveList()
{
    m_ui.move_list->clear();
}

void MMainWindow::updateGameInfo()
{
    Q_CHECK_PTR(m_game);

    MPlayerInfo info = m_game->getPlayerInfo();

    m_ui.white_name->setText(info.white_name);
    m_ui.white_rating->setText(QString("(%1)").arg(info.white_rating));
    m_ui.white_material->setText(QString("%1").arg(info.white_material));

    m_ui.black_name->setText(info.black_name);
    m_ui.black_rating->setText(QString("(%1)").arg(info.black_rating));
    m_ui.black_material->setText(QString("%1").arg(info.black_material));

    m_ui.last_move->setText(info.full_notation);

    // since we start counting at 0 with our half moves, the first white move
    // is shown when half_move == 1
    const int full_move = ((info.half_move - 1) / 2);
    QTreeWidgetItem* item = m_ui.move_list->topLevelItem(full_move);
    if(!item)
    {
        item = new QTreeWidgetItem(m_ui.move_list);
        m_ui.move_list->insertTopLevelItem(full_move, item);
    }
    item->setText(0, QString("%1").arg(full_move + 1));
    item->setText((info.has_white_moved ? 1 : 2), info.notation);
}

void MMainWindow::appendDebugOutput(QString msg)
{
    m_ui.debug->append(msg);
}

void MMainWindow::toggleDebugOutput()
{
    static bool toggled = true;

    if (toggled)
    {
        m_ui.debug->show();
    }
    else
    {
        m_ui.debug->hide();
    }

    toggled = !toggled;
}

void MMainWindow::showBoard()
{
    m_ui.stack->setCurrentIndex(STACKED_BOARD);
}

void MMainWindow::showMoveList()
{
    m_ui.stack->setCurrentIndex(STACKED_MOVE_LIST);
}

void MMainWindow::showAboutDialog()
{
    QDialog dialog;
    m_about_dialog.setupUi(&dialog);
    m_about_dialog.slogan->setFont(QFont("helvetica", 14, QFont::Normal));
    dialog.exec();
}

void MMainWindow::onMoveListItemPressed(QTreeWidgetItem *item, int column)
{
    // First column contains move, depending on which column was selected
    // (white, black) we have to add an offset to compute the correct half
    // move. For simplicity, the first column also selects the white half move.
    const int full_move = item->data(0, Qt::DisplayRole).toInt() -1;
    const int half_move = (full_move * 2) + (2 == column ? 2 : 1);
    m_game->setPositionTo(half_move);
    showBoard();
}

MDBusAdaptor::MDBusAdaptor(MMainWindow *window) 
: QDBusAbstractAdaptor(window),
  m_window(window)
{}

void MDBusAdaptor::top_application()
{
    qDebug() << __func__;
    m_window->activateWindow();
}

int main (int argc, char ** argv)
{
    QApplication app(argc, argv);
    MMainWindow window;

    QDBusConnection session(QDBusConnection::sessionBus());

    if (!session.registerService(SERVICE_NAME))
    {
        qWarning("Cannot register D-Bus service. Trying to activate other instance.");
        session.call(QDBusMessage::createMethodCall(SERVICE_NAME, SERVICE_PATH,
                                                    SERVICE_NAME, "top_application"));
        return 1;
    }

    if (!session.registerObject(SERVICE_PATH, &window))
    {
        qFatal("Cannot register D-Bus service. Aborting.");
        return 1;
    }

    new MDBusAdaptor(&window);
    window.show();

    return app.exec();
}
