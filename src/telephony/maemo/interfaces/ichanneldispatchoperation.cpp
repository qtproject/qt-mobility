#include "maemo/interfaces/ichanneldispatchoperation.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelDispatchOperation::IChannelDispatchOperation(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelDispatchOperation::IChannelDispatchOperation(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelDispatchOperation::IChannelDispatchOperation(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IChannelDispatchOperation::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(ChannelLost(const QDBusObjectPath&, const QString&, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(Finished()), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
