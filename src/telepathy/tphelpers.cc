#include "tphelpers.h"

namespace Miniature
{

CommandsMap createCommandsMap()
{
    CommandsMap commandsMap;

    commandsMap[Miniature::NewGame] = QString("new_game");
    commandsMap[Miniature::NewGameStarted] = QString("new_game_started");
    commandsMap[Miniature::NextMove] = QString("next_move");
    commandsMap[Miniature::BackMove] = QString("back_move");
    commandsMap[Miniature::PauseGame] = QString("pause_game");
    commandsMap[Miniature::ProposeDraw] = QString("propose_draw");
    commandsMap[Miniature::AdjournGame] = QString("adjourn_game");
    commandsMap[Miniature::ResignGame] = QString("resign_game");

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

QString commandToString(Miniature::Command command)
{
    Miniature::CommandsMap commandsMap = Miniature::createCommandsMap();
    for (Miniature::CommandsMap::const_iterator iter = commandsMap.begin();
         iter != commandsMap.end();
         ++iter)
    {
        if (command == iter->first)
        {
            return iter->second;
            break;
        }
    }

    return commandsMap[Miniature::Nop];
}
/*
NextMove,
BackMove,
PauseGame,
ProposeDraw,
AdjournGame,
ResignGame,
*/

Miniature::Command commandFromString(QString command)
{
    Miniature::CommandsMap commandsMap = Miniature::createCommandsMap();
    for (Miniature::CommandsMap::const_iterator iter = commandsMap.begin();
         iter != commandsMap.end();
         ++iter)
    {
        if (!command.compare(iter->second))
        {
            return iter->first;
            break;
        }
    }

    return Miniature::Nop;
}
