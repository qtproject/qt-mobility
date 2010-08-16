#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-channel-dispatch-operation.h"

namespace Tp
{
namespace Client
{

ChannelDispatchOperationInterface::ChannelDispatchOperationInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelDispatchOperationInterface::ChannelDispatchOperationInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelDispatchOperationInterface::ChannelDispatchOperationInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void ChannelDispatchOperationInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(ChannelLost(const QDBusObjectPath&, const QString&, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(Finished()), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
