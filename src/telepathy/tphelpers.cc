#include "tphelpers.h"

namespace TpGame
{

CommandsMap createCommandsMap()
{
    CommandsMap commandsMap;

    commandsMap[TpGame::NewGame] = QString("new_game");
    commandsMap[TpGame::NewGameAccept] = QString("new_game_accept");
    commandsMap[TpGame::Move] = QString("move");
    commandsMap[TpGame::TakeBack] = QString("take_back");
    commandsMap[TpGame::TakeBackAccept] = QString("take_back_accept");
    commandsMap[TpGame::Draw] = QString("draw");
    commandsMap[TpGame::DrawAccept] = QString("draw_accept");
    commandsMap[TpGame::Resign] = QString("resign");
    commandsMap[TpGame::Adjourn] = QString("adjourn");
    commandsMap[TpGame::AdjournAccept] = QString("adjourn_accept");

    return commandsMap;
}

} // namespace Miniature

QDBusArgument &operator<<(QDBusArgument &argument,
        const StreamTubeAddress &streamTubeAddress)
{
    argument.beginStructure();
    argument << streamTubeAddress.address << streamTubeAddress.port;
    argument.endStructure();
    return argument;
}

// Retrieve the StreamTubeAddress data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument,
        StreamTubeAddress &streamTubeAddress)
{
    argument.beginStructure();
    argument >> streamTubeAddress.address >> streamTubeAddress.port;
    argument.endStructure();
    return argument;
}

QString commandToString(TpGame::Command command)
{
    TpGame::CommandsMap commandsMap = TpGame::createCommandsMap();
    for (TpGame::CommandsMap::const_iterator iter = commandsMap.begin();
         iter != commandsMap.end();
         ++iter)
    {
        if (command == iter->first)
        {
            return iter->second;
            break;
        }
    }

    return commandsMap[TpGame::Nop];
}

TpGame::Command commandFromString(QString command)
{
    TpGame::CommandsMap commandsMap = TpGame::createCommandsMap();
    for (TpGame::CommandsMap::const_iterator iter = commandsMap.begin();
         iter != commandsMap.end();
         ++iter)
    {
        if (!command.compare(iter->second))
        {
            return iter->first;
            break;
        }
    }

    return TpGame::Nop;
}
