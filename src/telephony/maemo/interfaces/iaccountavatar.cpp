#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/iaccountavatar.h"

namespace DBus
{
    namespace Interfaces
    {
        IAccountAvatar::IAccountAvatar(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IAccountAvatar::IAccountAvatar(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IAccountAvatar::IAccountAvatar(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IAccountAvatar::IAccountAvatar(const DBus::Interfaces::IAccount& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IAccountAvatar::IAccountAvatar(const DBus::Interfaces::IAccount& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IAccountAvatar::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(AvatarChanged()), NULL, NULL);
            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
