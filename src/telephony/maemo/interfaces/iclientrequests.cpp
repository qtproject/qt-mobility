#include "maemo/interfaces/iclientrequests.h"

namespace DBus
{
    namespace Interfaces
    {
        IClientRequests::IClientRequests(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. IClientRequests::IClientRequests(...)";
        }

        IClientRequests::IClientRequests(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. IClientRequests::IClientRequests(...)";
        }

        IClientRequests::IClientRequests(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "3. IClientRequests::IClientRequests(...)";
        }

        IClientRequests::IClientRequests(const DBus::Interfaces::IClient& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. IClientRequests::IClientRequests(...)";
        }

        IClientRequests::IClientRequests(const DBus::Interfaces::IClient& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. IClientRequests::IClientRequests(...)";
        }

        IClientRequests::~IClientRequests()
        {
            qDebug() << "IClientRequests::~IClientRequests()";
        }

        void IClientRequests::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "IClientRequests::invalidate(...)";

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus

