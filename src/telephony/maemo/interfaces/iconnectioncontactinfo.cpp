#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectioncontactinfo.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionContactInfo::IConnectionContactInfo(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionContactInfo::IConnectionContactInfo(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionContactInfo::IConnectionContactInfo(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionContactInfo::IConnectionContactInfo(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionContactInfo::IConnectionContactInfo(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionContactInfo::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(ContactInfoChanged(uint, const DBus::Interfaces::ContactInfoFieldList&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
