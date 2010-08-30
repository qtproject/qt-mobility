#include "maemo/interfaces/ichanneltube.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTube::IChannelTube(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTube::IChannelTube(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTube::IChannelTube(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTube::IChannelTube(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTube::IChannelTube(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTube::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(TubeChannelStateChanged(uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
