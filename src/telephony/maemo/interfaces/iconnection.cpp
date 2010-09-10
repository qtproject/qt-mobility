#include <QtCore/QDebug>
#include "maemo/interfaces/iconnection.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnection::IConnection(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "IConnection::IConnection(...)-1";
        }

        IConnection::IConnection(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "IConnection::IConnection(...)-2";
        }

        IConnection::IConnection(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "IConnection::IConnection(...)-3";
        }

        IConnection::~IConnection()
        {
            qDebug() << "IConnection::~IConnection()";
        }


        void IConnection::invalidate(DBus::DBusProxy *proxy, const QString &error, const QString &message)
        {
            qDebug() << "IConnection::~invalidate(...)";

            disconnect(this, SIGNAL(SelfHandleChanged(uint)), NULL, NULL);
            disconnect(this, SIGNAL(NewChannel(const QDBusObjectPath&, const QString&, uint, uint, bool)), NULL, NULL);
            disconnect(this, SIGNAL(ConnectionError(const QString&, const QVariantMap&)), NULL, NULL);
            disconnect(this, SIGNAL(StatusChanged(uint, uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
