#include "maemo/interfaces/iclienthandler.h"

namespace DBus
{
    namespace Interfaces
    {
        IClientHandler::IClientHandler(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. IClientHandler::IClientHandler(...)";
        }

        IClientHandler::IClientHandler(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. IClientHandler::IClientHandler(...)";
        }

        IClientHandler::IClientHandler(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "3. IClientHandler::IClientHandler(...)";
        }

        IClientHandler::IClientHandler(const DBus::Interfaces::IClient& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. IClientHandler::IClientHandler(...)";
        }

        IClientHandler::IClientHandler(const DBus::Interfaces::IClient& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. IClientHandler::IClientHandler(...)";
        }

        void IClientHandler::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "IClientHandler::invalidate";

            DBus::IAbstract::invalidate(proxy, error, message);
        }

        IClientHandler::~IClientHandler()
        {
            qDebug() << "IClientHandler::~IClientHandler()";
        }
    }//Interfaces
}//DBus

