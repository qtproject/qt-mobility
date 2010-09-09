#include "maemo/interfaces/ichanneldispatcher.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelDispatcher::IChannelDispatcher(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelDispatcher::IChannelDispatcher(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelDispatcher::IChannelDispatcher(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IChannelDispatcher::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
