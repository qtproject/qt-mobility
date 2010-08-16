#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-connection-manager.h"

namespace Tp
{
namespace Client
{

ConnectionManagerInterface::ConnectionManagerInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionManagerInterface::ConnectionManagerInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionManagerInterface::ConnectionManagerInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void ConnectionManagerInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(NewConnection(const QString&, const QDBusObjectPath&, const QString&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
