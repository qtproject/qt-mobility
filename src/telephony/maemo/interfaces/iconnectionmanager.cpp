#include "maemo/interfaces/iconnectionmanager.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionManager::IConnectionManager(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionManager::IConnectionManager(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionManager::IConnectionManager(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IConnectionManager::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(NewConnection(const QString&, const QDBusObjectPath&, const QString&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
