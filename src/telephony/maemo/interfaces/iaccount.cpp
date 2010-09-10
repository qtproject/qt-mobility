#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/iaccount.h"

namespace DBus
{
    namespace Interfaces
    {
        IAccount::IAccount(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IAccount::IAccount(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IAccount::IAccount(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        void IAccount::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(Removed()), NULL, NULL);
            disconnect(this, SIGNAL(AccountPropertyChanged(const QVariantMap&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
