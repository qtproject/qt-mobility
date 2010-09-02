#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectioncontacts.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionContacts::IConnectionContacts(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionContacts::IConnectionContacts(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionContacts::IConnectionContacts(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionContacts::IConnectionContacts(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionContacts::IConnectionContacts(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionContacts::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
