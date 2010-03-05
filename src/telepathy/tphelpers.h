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

#endif // TPHELPERS_H
