#ifndef TPHELPERS_H
#define TPHELPERS_H

#include <QtDBus/QDBusArgument>

struct StreamTubeAddress
{
    QString address;
    uint port;
};

Q_DECLARE_METATYPE(StreamTubeAddress);

QDBusArgument &operator<<(QDBusArgument &argument,
        const StreamTubeAddress &streamTubeAddress);
const QDBusArgument &operator>>(const QDBusArgument &argument,
        StreamTubeAddress &streamTubeAddress);

namespace TpGame
{

enum Command
{
    NewGame,
    NewGameAccept,
    Move,
    TakeBack,
    TakeBackAccept,
    Draw,
    DrawAccept,
    Resign,
    Adjourn,
    AdjournAccept,
    Nop
};

typedef std::map<TpGame::Command, QString> CommandsMap;

CommandsMap createCommandsMap();

struct MoveStruct
{
    QString mMove;
    QString mFen;
};

struct TakeBackStruct
{
    int mBackMoves;
    QString mFen;
};

} // namespace Miniature

QString commandToString(TpGame::Command c);
TpGame::Command commandFromString(QString c);



#endif // TPHELPERS_H
