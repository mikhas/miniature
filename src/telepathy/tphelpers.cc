#include "tphelpers.h"

namespace Miniature
{

TCommandsMap createCommandsMap()
{
    TCommandsMap commandsMap;

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

};

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
    Miniature::TCommandsMap commandsMap = Miniature::createCommandsMap();
    for(Miniature::TCommandsMap::const_iterator c = commandsMap.begin(); c != commandsMap.end(); c++)
    {
        if(command == c->first)
        {
            return c->second;
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
    Miniature::TCommandsMap commandsMap = Miniature::createCommandsMap();
    for(Miniature::TCommandsMap::const_iterator c = commandsMap.begin(); c != commandsMap.end(); c++)
    {
        if(command.compare(c->second) == 0)
        {
            return c->first;
            break;
        }
    }

    return Miniature::Nop;
}

