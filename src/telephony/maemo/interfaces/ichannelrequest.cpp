#include "maemo/interfaces/ichannelrequest.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelRequest::IChannelRequest(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelRequest::IChannelRequest(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelRequest::IChannelRequest(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IChannelRequest::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(Failed(const QString&, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(Succeeded()), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
