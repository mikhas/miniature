/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#include "testutils.h"
#include "game.h"
#include "side.h"
#include "gnuchess.h"
#include "commandline.h"
#include "dispatcher.h"
#include "position.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

using Game::Position;
using Game::MovedPiece;

Q_DECLARE_METATYPE(Position)
Q_DECLARE_METATYPE(MovedPiece)

namespace Test {

class GnuChess
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;
    QTextStream m_out;

    Q_SLOT void initTestCase()
    {
        qRegisterMetaType<Position>();
        qRegisterMetaType<MovedPiece>();
        m_app.reset(TestUtils::createApp("gnuchess"));
    }

    Q_SLOT void testShortMatch()
    {}

    Q_SLOT void testRunInBackgroundForeground()
    {}

    Q_SLOT void testIllegalMove()
    {}
};

} // namespace Test

QTEST_APPLESS_MAIN(Test::GnuChess)
#include ".moc/gnuchess.moc"
