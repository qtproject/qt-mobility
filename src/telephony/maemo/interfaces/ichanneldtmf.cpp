#include "maemo/interfaces/ichanneldtmf.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelDTMF::IChannelDTMF(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelDTMF::IChannelDTMF(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelDTMF::IChannelDTMF(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelDTMF::IChannelDTMF(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelDTMF::IChannelDTMF(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelDTMF::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
