#include "maemo/interfaces/cli-client.h"

namespace Tp
{
    namespace Client
    {

        ClientInterface::ClientInterface(const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. ClientInterface::ClientInterface(...)";
        }

        ClientInterface::ClientInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. ClientInterface::ClientInterface(...)";
        }

        ClientInterface::ClientInterface(Tp::DBusProxy *proxy)
            : Tp::AbstractInterface(proxy, staticInterfaceName())
        {
            qDebug() << "3. ClientInterface::ClientInterface(...)";
        }

        ClientInterface::~ClientInterface()
        {
            qDebug() << "ClientInterface::~ClientInterface()";
        }

        void ClientInterface::invalidate(Tp::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "ClientInterface::invalidate";
            Tp::AbstractInterface::invalidate(proxy, error, message);
        }

        ClientApproverInterface::ClientApproverInterface(const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. ClientApproverInterface::ClientApproverInterface(...)";
        }

        ClientApproverInterface::ClientApproverInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. ClientApproverInterface::ClientApproverInterface(...)";
        }

        ClientApproverInterface::ClientApproverInterface(Tp::DBusProxy *proxy)
            : Tp::AbstractInterface(proxy, staticInterfaceName())
        {
            qDebug() << "3. ClientApproverInterface::ClientApproverInterface(...)";
        }

        ClientApproverInterface::ClientApproverInterface(const Tp::Client::ClientInterface& mainInterface)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. ClientApproverInterface::ClientApproverInterface(...)";
        }

        ClientApproverInterface::ClientApproverInterface(const Tp::Client::ClientInterface& mainInterface, QObject *parent)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. ClientApproverInterface::ClientApproverInterface(...)";
        }

        ClientApproverInterface::~ClientApproverInterface()
        {
            qDebug() << "ClientApproverInterface::~ClientApproverInterface()";
        }

        void ClientApproverInterface::invalidate(Tp::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "ClientApproverInterface::invalidate";

            Tp::AbstractInterface::invalidate(proxy, error, message);
        }

        ClientHandlerInterface::ClientHandlerInterface(const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. ClientHandlerInterface::ClientHandlerInterface(...)";
        }

        ClientHandlerInterface::ClientHandlerInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. ClientHandlerInterface::ClientHandlerInterface(...)";
        }

        ClientHandlerInterface::ClientHandlerInterface(Tp::DBusProxy *proxy)
            : Tp::AbstractInterface(proxy, staticInterfaceName())
        {
            qDebug() << "3. ClientHandlerInterface::ClientHandlerInterface(...)";
        }

        ClientHandlerInterface::ClientHandlerInterface(const Tp::Client::ClientInterface& mainInterface)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. ClientHandlerInterface::ClientHandlerInterface(...)";
        }

        ClientHandlerInterface::ClientHandlerInterface(const Tp::Client::ClientInterface& mainInterface, QObject *parent)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. ClientHandlerInterface::ClientHandlerInterface(...)";
        }

        void ClientHandlerInterface::invalidate(Tp::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "ClientHandlerInterface::invalidate";

            Tp::AbstractInterface::invalidate(proxy, error, message);
        }

        ClientHandlerInterface::~ClientHandlerInterface()
        {
            qDebug() << "ClientHandlerInterface::~ClientHandlerInterface()";
        }

        ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(...)";
        }

        ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(...)";
        }

        ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(Tp::DBusProxy *proxy)
            : Tp::AbstractInterface(proxy, staticInterfaceName())
        {
            qDebug() << "3. ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(...)";
        }

        ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(const Tp::Client::ClientInterface& mainInterface)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(...)";
        }

        ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(const Tp::Client::ClientInterface& mainInterface, QObject *parent)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. ClientInterfaceRequestsInterface::ClientInterfaceRequestsInterface(...)";
        }

        ClientInterfaceRequestsInterface::~ClientInterfaceRequestsInterface()
        {
            qDebug() << "ClientInterfaceRequestsInterface::~ClientInterfaceRequestsInterface()";
        }

        void ClientInterfaceRequestsInterface::invalidate(Tp::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "ClientInterfaceRequestsInterface::invalidate(...)";

            Tp::AbstractInterface::invalidate(proxy, error, message);
        }

        ClientObserverInterface::ClientObserverInterface(const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. ClientObserverInterface::ClientObserverInterface(...)";
        }

        ClientObserverInterface::ClientObserverInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. ClientObserverInterface::ClientObserverInterface(...)";
        }

        ClientObserverInterface::ClientObserverInterface(Tp::DBusProxy *proxy)
            : Tp::AbstractInterface(proxy, staticInterfaceName())
        {
            qDebug() << "3. ClientObserverInterface::ClientObserverInterface(...)";
        }

        ClientObserverInterface::ClientObserverInterface(const Tp::Client::ClientInterface& mainInterface)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. ClientObserverInterface::ClientObserverInterface(...)";
        }

        ClientObserverInterface::ClientObserverInterface(const Tp::Client::ClientInterface& mainInterface, QObject *parent)
            : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. ClientObserverInterface::ClientObserverInterface(...)";
        }

        ClientObserverInterface::~ClientObserverInterface()
        {
            qDebug() << "ClientObserverInterface::~ClientObserverInterface()";
        }

        void ClientObserverInterface::invalidate(Tp::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "ClientObserverInterface::invalidate(...)";

            Tp::AbstractInterface::invalidate(proxy, error, message);
        }
    }
}
