#include "maemo/interfaces/iaccountmanager.h"

namespace DBus
{
    namespace Interfaces
    {
        IAccountManager::IAccountManager(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IAccountManager::IAccountManager(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IAccountManager::IAccountManager(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IAccountManager::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(AccountRemoved(const QDBusObjectPath&)), NULL, NULL);
            disconnect(this, SIGNAL(AccountValidityChanged(const QDBusObjectPath&, bool)), NULL, NULL);
            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
