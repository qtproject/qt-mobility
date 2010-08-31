#include "maemo/interfaces/ichanneltyperoomlist.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeRoomList::IChannelTypeRoomList(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeRoomList::IChannelTypeRoomList(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeRoomList::IChannelTypeRoomList(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeRoomList::IChannelTypeRoomList(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeRoomList::IChannelTypeRoomList(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeRoomList::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(GotRooms(const DBus::Interfaces::RoomInfoList&)), NULL, NULL);
            disconnect(this, SIGNAL(ListingRooms(bool)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
