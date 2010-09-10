#include "maemo/interfaces/ichanneltypestreamedmedia.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeStreamedMedia::IChannelTypeStreamedMedia(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeStreamedMedia::IChannelTypeStreamedMedia(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeStreamedMedia::IChannelTypeStreamedMedia(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeStreamedMedia::IChannelTypeStreamedMedia(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeStreamedMedia::IChannelTypeStreamedMedia(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeStreamedMedia::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(StreamAdded(uint, uint, uint)), NULL, NULL);
            disconnect(this, SIGNAL(StreamDirectionChanged(uint, uint, uint)), NULL, NULL);
            disconnect(this, SIGNAL(StreamError(uint, uint, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(StreamRemoved(uint)), NULL, NULL);
            disconnect(this, SIGNAL(StreamStateChanged(uint, uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
