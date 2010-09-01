#include "maemo/interfaces/ichannelcallstate.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelCallState::IChannelCallState(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelCallState::IChannelCallState(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelCallState::IChannelCallState(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelCallState::IChannelCallState(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelCallState::IChannelCallState(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelCallState::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(CallStateChanged(uint, uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
