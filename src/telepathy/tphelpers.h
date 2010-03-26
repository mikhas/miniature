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

namespace Miniature
{

enum Command
{
    NewGame,
    NewGameStarted,
    NextMove,
    BackMove,
    PauseGame,
    ProposeDraw,
    AdjournGame,
    ResignGame,
    Nop
};

typedef std::map<Miniature::Command, QString> TCommandsMap;

TCommandsMap createCommandsMap();
};

QString commandToString(Miniature::Command c);
Miniature::Command commandFromString(QString c);



#endif // TPHELPERS_H
