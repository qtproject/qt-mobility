#include "maemo/interfaces/ichannelpassword.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelPassword::IChannelPassword(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelPassword::IChannelPassword(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelPassword::IChannelPassword(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelPassword::IChannelPassword(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelPassword::IChannelPassword(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelPassword::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(PasswordFlagsChanged(uint, uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
