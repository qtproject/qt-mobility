#include "maemo/interfaces/ichannel.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannel::IChannel(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannel::IChannel(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannel::IChannel(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IChannel::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(Closed()), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
