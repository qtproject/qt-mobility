#include "maemo/interfaces/ichanneltypestreamtube.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeStreamTube::IChannelTypeStreamTube(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeStreamTube::IChannelTypeStreamTube(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeStreamTube::IChannelTypeStreamTube(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeStreamTube::IChannelTypeStreamTube(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeStreamTube::IChannelTypeStreamTube(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeStreamTube::invalidate(DBus::DBusProxy *proxy, const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(NewRemoteConnection(uint, const QDBusVariant&, uint)), NULL, NULL);
            disconnect(this, SIGNAL(NewLocalConnection(uint)), NULL, NULL);
            disconnect(this, SIGNAL(ConnectionClosed(uint, const QString&, const QString&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
