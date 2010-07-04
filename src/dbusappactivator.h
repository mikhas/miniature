/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
 *               2010 Dariusz Mikulski <dariusz.mikulski@gmail.com>
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

#ifndef DBUSAPPACTIVATOR_H
#define DBUSAPPACTIVATOR_H

#include <QDBusAbstractAdaptor>
#include <QPointer>

class MMainWindow;

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
    //! This slot gets called via D-Bus and brings Miniature into the
    //! foreground.
    void top_application();

private:
    QPointer<MMainWindow> m_window; /*!< A back pointer to the main view. */
};

#endif // DBUSAPPACTIVATOR_H
