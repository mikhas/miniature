#include "tphelpers.h"

namespace TpGame
{

CommandsMap createCommandsMap()
{
    CommandsMap commandsMap;

    commandsMap[TpGame::NewGame] = QString("new_game");
    commandsMap[TpGame::NewGameStarted] = QString("new_game_started");
    commandsMap[TpGame::NextMove] = QString("next_move");
    commandsMap[TpGame::BackMove] = QString("back_move");
    commandsMap[TpGame::PauseGame] = QString("pause_game");
    commandsMap[TpGame::ProposeDraw] = QString("propose_draw");
    commandsMap[TpGame::AdjournGame] = QString("adjourn_game");
    commandsMap[TpGame::ResignGame] = QString("resign_game");

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
/*
NextMove,
BackMove,
PauseGame,
ProposeDraw,
AdjournGame,
ResignGame,
*/

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
