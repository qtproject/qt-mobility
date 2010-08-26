#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-channel-request.h"

namespace Tp
{
namespace Client
{

ChannelRequestInterface::ChannelRequestInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelRequestInterface::ChannelRequestInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelRequestInterface::ChannelRequestInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void ChannelRequestInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(Failed(const QString&, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(Succeeded()), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
