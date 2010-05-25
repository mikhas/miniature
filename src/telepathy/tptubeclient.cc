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

    TpGame::Command c = commandFromString(command);

    switch(c)
    {
        case TpGame::NewGame:
        {
            qDebug() << "Receiving NewGame";

            QString testString;
            stream >> testString;

            qDebug() << testString;

            Q_EMIT receivedNewGame();

            sendNewGameAccept();
            break;
        }

        case TpGame::NewGameAccept:
        {
            qDebug() << "Receiving NewGameAccept";

            QString testString;
            stream >> testString;

            qDebug() << testString;
            break;
        }

        case TpGame::Move:
        {
            qDebug() << "Receiving Move";

            Q_EMIT receivedMove();

            break;
        }

        case TpGame::TakeBack:
        {
            qDebug() << "Receiving TakeBack";

            Q_EMIT receivedTakeBack();

            break;
        }

        case TpGame::TakeBackAccept:
        {
            qDebug() << "Receiving TakeBackAccept";

            Q_EMIT receivedTakeBackAccept();

            break;
        }

        case TpGame::Draw:
        {
            qDebug() << "Receiving Draw";

            Q_EMIT receivedDraw();

            break;
        }

        case TpGame::DrawAccept:
        {
            qDebug() << "Receiving DrawAccept";

            Q_EMIT receivedDrawAccept();

            break;
        }

        case TpGame::Resign:
        {
            qDebug() << "Receiving Resign";

            Q_EMIT receivedResign();

            break;
        }

        case TpGame::Adjourn:
        {
            qDebug() << "Receiving Adjourn";

            Q_EMIT receivedAdjourn();

            break;
        }

        case TpGame::AdjournAccept:
        {
            qDebug() << "Receiving AdjournAccept";

            Q_EMIT receivedAdjournAccept();

            break;
        }

        case TpGame::Nop:
        {
            // nothing;
            break;
        }
    }
}

void TubeClient::sendNewGame()
{
    qDebug() << "TpTubeClient::sendNewGame()";

    QDataStream stream(this);

    stream << commandToString(TpGame::NewGame);
    stream << QString("Start new game");
}

void TubeClient::sendNewGameAccept()
{
    qDebug() << "TpTubeClient::sendNewGameAccept()";

    QDataStream stream(this);

    stream << commandToString(TpGame::NewGameAccept);
    stream << QString("New game accept");
}

void TubeClient::sendMove()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::Move);
}

void TubeClient::sendTakeBack()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::TakeBack);
}

void TubeClient::sendTakeBackAccept()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::TakeBackAccept);
}

void TubeClient::sendDraw()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::Draw);
}

void TubeClient::sendDrawAccept()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::DrawAccept);
}

void TubeClient::sendResign()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::Resign);
}

void TubeClient::sendAdjourn()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::Adjourn);
}

void TubeClient::sendAdjournAccept()
{
    QDataStream stream(this);

    stream << commandToString(TpGame::AdjournAccept);
}


} // namespace TpGame
