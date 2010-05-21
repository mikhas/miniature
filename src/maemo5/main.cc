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

#include <QtCore>
#include <QtGui>

/*! \brief Container for Miniature main actions.
 */
class mActions
{
public:
    /*!
     *  @param[in] owner The owner of the contained actions.
     */
    explicit mActions(QObject *owner)
    :  show_about_dialog(new QAction(owner)),
       show_game_log(new QAction(owner))
    {}

    virtual ~mActions()
    {}

    QAction* show_about_dialog;
    QAction* show_game_log;
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
};

const char* const dbus_service_name = "org.maemo.miniature";
const char* const dbus_service_path = "/org/maemo/miniature";

MMainWindow::MMainWindow(MGameLog *log, QWidget *parent)
: QMainWindow(parent),
  m_log(log),
  m_actions(new mActions(this)),
  m_session(QDBusConnection::sessionBus())
{
    if (!m_session.registerService(dbus_service_name))
    {
        qWarning("Cannot register D-Bus service. Trying to activate other instance.");
        m_session.call(QDBusMessage::createMethodCall(dbus_service_name, dbus_service_path,
                                                      dbus_service_name, "top_application"));
        exit(EXIT_FAILURE);
    }

    if (!m_session.registerObject(dbus_service_path, this))
    {
        qFatal("Cannot register D-Bus service. Aborting.");
        exit(EXIT_FAILURE);
    }

    new QDBusAppActivator(this);

    registerActions();
    connectActions();
}

void MMainWindow::setupPreGameUi(QMainWindow *window, QWidget *subview)
{
    window->setWindowTitle(tr("Miniature"));
    window->resize(800, 480);

    window->setAttribute(Qt::WA_DeleteOnClose);
    window->setAttribute(Qt::WA_Maemo5StackedWindow);
    window->setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);

    window->setCentralWidget(subview);
}

void MMainWindow::setupGameUi(QMainWindow *main, QGraphicsView *view)
{
    main->setWindowTitle(tr("Miniature"));
    main->setAttribute(Qt::WA_DeleteOnClose);
    main->setAttribute(Qt::WA_Maemo5StackedWindow);
    main->setWindowState(view->windowState() | Qt::WindowFullScreen);
    main->setCentralWidget(view);

    QSizePolicy no_expand(QSizePolicy::Preferred, QSizePolicy::Preferred);
    view->setSizePolicy(no_expand);
    view->setFrameShape(QFrame::NoFrame);
    view->setLineWidth(0);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MMainWindow::registerActions()
{
    m_actions->show_about_dialog->setText(tr("About"));
    m_actions->show_about_dialog->setShortcut(tr("Ctrl+?"));

    m_actions->show_game_log->setText(tr("Show Game Log"));
    m_actions->show_game_log->setShortcut(tr("Ctrl+L"));

    QMenuBar *menu_bar = new QMenuBar(this);
    menu_bar->setGeometry(QRect(0, 0, 480, 25));

    QMenu *main = new QMenu(menu_bar);
    menu_bar->addAction(main->menuAction());
    main->addAction(m_actions->show_game_log);
    main->addAction(m_actions->show_about_dialog);

    setMenuBar(menu_bar);
}

void MMainWindow::connectActions()
{
    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(m_actions->show_game_log, this);

    connect(m_actions->show_game_log, SIGNAL(triggered()),
            mapper,                   SLOT(map()));

    connect(mapper, SIGNAL(mapped(QWidget *)),
            m_log,  SLOT(showLog(QWidget *)));

    connect(m_actions->show_about_dialog, SIGNAL(triggered()),
            this,                         SLOT(showAboutDialog()));
}

void MMainWindow::showAboutDialog()
{
    QFont font = QFont("helvetica", 14, QFont::Normal);
    QDialog *dialog = new QDialog(QApplication::activeWindow());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(tr("About Miniature"));
    dialog->resize(-1, 480);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(1);

    QVBoxLayout *vbox = new QVBoxLayout(dialog);

    QLabel *slogan = new QLabel(dialog);
    slogan->setText(tr("A chess board that always goes with you."));
    slogan->setAlignment(Qt::AlignCenter);
    vbox->addWidget(slogan);

    // TODO: Find correct color roles that are used in the QTextBrowser, and remove the HTML style hack.
    QPalette lightBg;
    lightBg.setColor(QPalette::Base, QColor(Qt::white));
    lightBg.setColor(QPalette::WindowText, QColor(Qt::black));

    QTextBrowser *description = new QTextBrowser(dialog);
    description->setPalette(lightBg);
    description->setSizePolicy(policy);

    description->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

    description->setOpenExternalLinks(true);
    description->setText(tr(
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head/><body style=\"color:black; background:white;\">\n"
"<div style=\"margin-bottom:1.5em;\">Miniature version: %1</div>\n"
"<div style=\"margin-bottom:1.5em;\">Check the <a href=\"http://wiki.maemo.org/Miniature\">website</a> for more information,\n"
"feedback and ways to get involved. Miniature is free software for you to enjoy\n"
"(<a href=\"http://creativecommons.org/licenses/GPL/2.0/\">GPL2 or later</a>).\n"
"<div>&#169; <a href=\"http://wiki.maemo.org/Miniature#Contributors\">Contributors</a>.</div>\n"
"</body></html>"
        ).arg(PACKAGE_VERSION));

    vbox->addWidget(description);

    QDialogButtonBox *buttons = new QDialogButtonBox(dialog);
    buttons->setOrientation(Qt::Horizontal);
    buttons->setStandardButtons(QDialogButtonBox::Close);
    vbox->addWidget(buttons);

    dialog->show();
}
