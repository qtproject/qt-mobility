#include "maemo/interfaces/iclientobserver.h"

namespace DBus
{
    namespace Interfaces
    {
        IClientObserver::IClientObserver(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. IClientObserver::IClientObserver(...)";
        }

        IClientObserver::IClientObserver(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. IClientObserver::IClientObserver(...)";
        }

        IClientObserver::IClientObserver(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "3. IClientObserver::IClientObserver(...)";
        }

        IClientObserver::IClientObserver(const DBus::Interfaces::IClient& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. IClientObserver::IClientObserver(...)";
        }

        IClientObserver::IClientObserver(const DBus::Interfaces::IClient& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. IClientObserver::IClientObserver(...)";
        }

        IClientObserver::~IClientObserver()
        {
            qDebug() << "IClientObserver::~IClientObserver()";
        }

        void IClientObserver::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "IClientObserver::invalidate(...)";

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus

