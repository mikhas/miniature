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

#ifndef MINIATURE_H__
#define MINIATURE_H__

#include <game_log.h>

#include <QtGui>
#include <QtDBus>

class mActions;
class mDialogs;

/*! \brief Miniature main view
 *
 *  This class represents Miniature's main view. It sets up the basic UI, the
 *  menu and delegates further control to MGame.
 *  The ctor implementation is platform-specific and therefore located in
 *  separate .cc files. Automake is used to decide which file to include for
 *  the given platform Miniature is compiled for.
 */
class MMainWindow
: public QMainWindow
{
    Q_OBJECT

public:
    explicit MMainWindow(MGameLog *log, QWidget *parent = 0);
    virtual ~MMainWindow();

    /*!
     *  Initializes the pre-game UI.
     */
    static void setupPreGameUi(QMainWindow *window, QWidget *subview);

    /*!
     *  Initializes the normal game mode UI.
     */
    static void setupGameUi(QMainWindow *window, QGraphicsView *subview);

    /*!
     *  Registers the main view actions in the menu, and connects them.
     */
    void registerActions();

    /*!
     *  Connects the main view actions to the game controller.
     */
    void connectActions();

public Q_SLOTS:
    /*!
     *  This slot can be used to send debug output to be shown in the main
     *  view. Useful when not run from a terminal.
     * @param[in] The debug message.
     */
    void appendDebugOutput(const QString &msg);

    /*!
     *  This slot can be used to fill the game log. It can also contain debug
     *  output.
     */
    //void appendToGameLog(const QString &msg, const LogLevelFlags &log_level);

    /*!
     *  This slot toggles whether the debug output is shown to the user at all.
     */
    void toggleDebugOutput();

    /*!
     *  Shows the about dialog.
     */
    void showAboutDialog();

private:
    MGameLog *m_log; /*<! The game log which is used for the game log window. */
    mActions *m_actions; /*!< The main actions used in the main view. */
    mDialogs *m_dialogs; /*!< The dialogs used in the main view. */
    QDBusConnection m_session; /*!< The D-Bus connection used for activating Miniature. */
};

/*! \brief D-Bus activation for Miniature
 *
 *  On Maemo, applications get started via D-Bus. Also, D-Bus will take care
 *  that there is always only one application instance running. This class
 *  provides the required D-Bus interface for Miniature.
 *
 *  For non-Maemo platforms the class' implementation will simply do nothing.
 */
class QDBusAppActivator
: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.miniature")

public:
    explicit QDBusAppActivator(MMainWindow *window);

public Q_SLOTS:
    /*!
     *  This slot gets called via D-Bus.
     */
    void top_application();

private:
    MMainWindow *m_window; /*!< A back pointer to the main view. */
};

#endif // MINIATURE_H__
