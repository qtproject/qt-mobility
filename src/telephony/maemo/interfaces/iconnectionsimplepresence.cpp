#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectionsimplepresence.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionSimplePresence::IConnectionSimplePresence(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionSimplePresence::IConnectionSimplePresence(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionSimplePresence::IConnectionSimplePresence(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionSimplePresence::IConnectionSimplePresence(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionSimplePresence::IConnectionSimplePresence(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionSimplePresence::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(PresencesChanged(const DBus::Interfaces::SimpleContactPresences&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
