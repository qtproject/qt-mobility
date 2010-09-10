#include "maemo/interfaces/iclient.h"

namespace DBus
{
    namespace Interfaces
    {
        IClient::IClient(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. IClient::IClient(...)";
        }

        IClient::IClient(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. IClient::IClient(...)";
        }

        IClient::IClient(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "3. IClient::IClient(...)";
        }

        IClient::~IClient()
        {
            qDebug() << "IClient::~IClient()";
        }

        void IClient::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "IClient::invalidate";
            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus

