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

#include <config.h>
#include <mmainwindow.h>
#include <dbusappactivator.h>
#include <about.ui.h>

/*! \brief Container for Miniature main actions.
 */
class mActions
{
public:
    /*!
     *  @param[in] owner The owner of the contained actions.
     */
    explicit mActions(QObject *owner)
    : new_game(new QAction(owner)),
      next_move(new QAction(owner)),
      prev_move(new QAction(owner)),
      show_about_dialog(new QAction(owner)),
      toggle_rotation(new QAction(owner))
    {}

    virtual ~mActions()
    {}

    QAction* new_game;
    QAction* next_move;
    QAction* prev_move;
    QAction* show_about_dialog;
    QAction* toggle_rotation;
};

/*! \brief Container for Miniature dialogs.
 */
class mDialogs
{
public:
    mDialogs()
    {}

    virtual ~mDialogs()
    {}

    Ui::AboutDialog about_dialog;
};

MMainWindow::MMainWindow()
: QMainWindow(),
  m_game(0),
  m_view(0),
  m_actions(new mActions(this))
{
    setup();
    setupUi();
    registerActions();
    connectActions();

    m_actions->new_game->trigger();
}

void MMainWindow::setupUi()
{
    setWindowTitle(tr("Miniature"));
    resize(480, 800);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);

    QSizePolicy v_expand(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_view->setSizePolicy(v_expand);
    m_view->setMinimumSize(QSize(480, 650));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setLineWidth(0);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vbox->addWidget(m_view);

    QSpacerItem *v_spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vbox->addItem(v_spacer);
}

void MMainWindow::registerActions()
{
    m_actions->new_game->setText(tr("New Game"));
    m_actions->new_game->setShortcut(tr("Ctrl+N"));

    m_actions->next_move->setText(tr("Next Move"));
    m_actions->next_move->setShortcut(tr("Right"));

    m_actions->prev_move->setText(tr("Previous Move"));
    m_actions->prev_move->setShortcut(tr("Left"));

    m_actions->toggle_rotation->setText(tr("Toggle Rotation"));
    m_actions->toggle_rotation->setShortcut(tr("Ctrl+R"));

    m_actions->show_about_dialog->setText(tr("About"));
    m_actions->show_about_dialog->setShortcut(tr("Ctrl+?"));

    QMenuBar *menu_bar = new QMenuBar(this);
    menu_bar->setGeometry(QRect(0, 0, 480, 25));

    QMenu *menu = new QMenu(menu_bar);
    menu_bar->addAction(menu->menuAction());
    menu->addAction(m_actions->new_game);
    menu->addAction(m_actions->next_move);
    menu->addAction(m_actions->prev_move);
    menu->addAction(m_actions->toggle_rotation);

    QMenu *help = new QMenu(menu_bar);
    menu_bar->addAction(help->menuAction());
    help->addAction(m_actions->show_about_dialog);

    setMenuBar(menu_bar);
}

void MMainWindow::connectActions()
{
    // Connect actions with MGame controller.
    connect(m_actions->new_game, SIGNAL(triggered()),
            m_game, SLOT(newGame()));
    connect(m_actions->next_move, SIGNAL(triggered()),
            m_game, SLOT(nextMove()));
    connect(m_actions->prev_move, SIGNAL(triggered()),
            m_game, SLOT(prevMove()));
    connect(m_actions->toggle_rotation, SIGNAL(triggered()),
            m_game, SIGNAL(togglePieceRotations()));

    // Connect action to show about dialog
    // TODO: find out why the QDialog crashes in sbox! gdb reports instances of
    // QDialog as struct*, so it is probably a linker issue.
    connect(m_actions->show_about_dialog, SIGNAL(triggered()),
            this, SLOT(showAboutDialog()));
}

void MMainWindow::showAboutDialog()
{
    QFont font = QFont("helvetica", 14, QFont::Normal);
    QDialog dialog;
    m_dialogs->about_dialog.setupUi(&dialog);
    m_dialogs->about_dialog.slogan->setFont(font);
    m_dialogs->about_dialog.version->setFont(font);
    m_dialogs->about_dialog.version->setText(QString("%1: %2").arg(tr("Version"))
                                                              .arg(PACKAGE_VERSION));
    dialog.exec();
}
