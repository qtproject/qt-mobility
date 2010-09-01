#include "maemo/interfaces/ichannelhold.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelHold::IChannelHold(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelHold::IChannelHold(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelHold::IChannelHold(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelHold::IChannelHold(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelHold::IChannelHold(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelHold::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(HoldStateChanged(uint, uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
