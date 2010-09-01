#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectionavatars.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionAvatars::IConnectionAvatars(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IConnectionAvatars::IConnectionAvatars(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IConnectionAvatars::IConnectionAvatars(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IConnectionAvatars::IConnectionAvatars(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IConnectionAvatars::IConnectionAvatars(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IConnectionAvatars::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(AvatarUpdated(uint, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(AvatarRetrieved(uint, const QString&, const QByteArray&, const QString&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
