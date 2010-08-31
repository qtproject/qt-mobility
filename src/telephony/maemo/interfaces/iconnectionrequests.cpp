#include <QtCore/QDebug>
#include "maemo/interfaces/iconnectionrequests.h"

namespace DBus
{
    namespace Interfaces
    {
        IConnectionRequests::IConnectionRequests(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "IConnectionRequests::IConnectionRequests(...)-1";
        }

        IConnectionRequests::IConnectionRequests(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "IConnectionRequests::IConnectionRequests(...)-2";
        }

        IConnectionRequests::IConnectionRequests(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "IConnectionRequests::IConnectionRequests(...)-3";
        }

        IConnectionRequests::IConnectionRequests(const DBus::Interfaces::IConnection& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "IConnectionRequests::IConnectionRequests(...)-4";
        }

        IConnectionRequests::IConnectionRequests(const DBus::Interfaces::IConnection& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "IConnectionRequests::IConnectionRequests(...)-5";
        }

        IConnectionRequests::~IConnectionRequests()
        {
            qDebug() << "IConnectionRequests::~IConnectionRequests(...)";
        }

        void IConnectionRequests::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(NewChannels(const DBus::Interfaces::ChannelDetailsList&)), NULL, NULL);
            disconnect(this, SIGNAL(ChannelClosed(const QDBusObjectPath&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
