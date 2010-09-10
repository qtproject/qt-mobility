#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectioncapabilities.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionCapabilities::IConnectionCapabilities(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionCapabilities::IConnectionCapabilities(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionCapabilities::IConnectionCapabilities(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionCapabilities::IConnectionCapabilities(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionCapabilities::IConnectionCapabilities(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionCapabilities::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(CapabilitiesChanged(const DBus::Interfaces::CapabilityChangeList&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
