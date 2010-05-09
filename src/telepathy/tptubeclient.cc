/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 *              Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
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

#include "tptubeclient.h"
#include "tphelpers.h"

#include <QHostAddress>
#include <QDataStream>

namespace TpGame
{

TubeClient::TubeClient(QObject *parent)
    : QTcpSocket(parent)
{
    qDebug() << "TpTubeClient::TpTubeClient()";

    connect(this, SIGNAL(readyRead()), this, SLOT(readyToTransfer()));
    connect(this, SIGNAL(connected()), this, SLOT(readyToTransfer()));
}

void TubeClient::readyToTransfer()
{
    qDebug() << "TpTubeClient::readyToTransfer()";

    QDataStream stream(this);

    QString command;
    stream >> command;
/*
    Command c = commandFromString(command);

    switch(c)
    {
        case Miniature::NewGame:
        {
            qDebug() << "switch NewGame";

            QString testString;
            stream >> testString;

            qDebug() << testString;

            newGameStarted();
            break;
        }
        case Miniature::NewGameStarted:
        {
            qDebug() << "switch NewGameStarted";

            QString testString;
            stream >> testString;

            qDebug() << testString;
            break;
        }
        case Miniature::Nop:
        {
            // nothing;
            break;
        }
    }
*/
}

void TubeClient::startNewGame()
{
    qDebug() << "TpTubeClient::startNewGame()";

    QDataStream stream(this);

    stream << commandToString(Miniature::NewGame);
    stream << QString("Start new game");
}

void TubeClient::newGameStarted()
{
    qDebug() << "TpTubeClient::newGameStarted()";

    QDataStream stream(this);

    stream << commandToString(Miniature::NewGameStarted);
    stream << QString("New game started");
}

} // namespace TpGame
