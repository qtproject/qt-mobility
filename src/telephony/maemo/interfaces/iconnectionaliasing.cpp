#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectionaliasing.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionAliasing::IConnectionAliasing(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionAliasing::IConnectionAliasing(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionAliasing::IConnectionAliasing(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionAliasing::IConnectionAliasing(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionAliasing::IConnectionAliasing(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionAliasing::invalidate(DBus::DBusProxy *proxy, const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(AliasesChanged(const DBus::Interfaces::AliasPairList&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
