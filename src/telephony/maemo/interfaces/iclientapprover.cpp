#include "maemo/interfaces/iclientapprover.h"

namespace DBus
{
    namespace Interfaces
    {
        IClientApprover::IClientApprover(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
            qDebug() << "1. IClientApprover::IClientApprover(...)";
        }

        IClientApprover::IClientApprover(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
            qDebug() << "2. IClientApprover::IClientApprover(...)";
        }

        IClientApprover::IClientApprover(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
            qDebug() << "3. IClientApprover::IClientApprover(...)";
        }

        IClientApprover::IClientApprover(const DBus::Interfaces::IClient& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
            qDebug() << "4. IClientApprover::IClientApprover(...)";
        }

        IClientApprover::IClientApprover(const DBus::Interfaces::IClient& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
            qDebug() << "5. IClientApprover::IClientApprover(...)";
        }

        IClientApprover::~IClientApprover()
        {
            qDebug() << "IClientApprover::~IClientApprover()";
        }

        void IClientApprover::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            qDebug() << "IClientApprover::invalidate";
            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }//Interfaces
}//DBus
