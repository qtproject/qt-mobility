#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectionlocation.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionLocation::IConnectionLocation(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionLocation::IConnectionLocation(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionLocation::IConnectionLocation(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionLocation::IConnectionLocation(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionLocation::IConnectionLocation(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionLocation::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(LocationUpdated(uint, const QVariantMap&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
