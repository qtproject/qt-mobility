#include "maemo/interfaces/ichanneltypecontactlist.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeContactList::IChannelTypeContactList(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeContactList::IChannelTypeContactList(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeContactList::IChannelTypeContactList(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeContactList::IChannelTypeContactList(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeContactList::IChannelTypeContactList(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeContactList::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
