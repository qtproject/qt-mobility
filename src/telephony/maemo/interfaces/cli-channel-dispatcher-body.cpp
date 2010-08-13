#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-channel-dispatcher.h"

namespace Tp
{
namespace Client
{

ChannelDispatcherInterface::ChannelDispatcherInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelDispatcherInterface::ChannelDispatcherInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelDispatcherInterface::ChannelDispatcherInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void ChannelDispatcherInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
