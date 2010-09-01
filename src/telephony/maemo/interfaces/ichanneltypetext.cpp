#include "maemo/interfaces/ichanneltypetext.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeText::IChannelTypeText(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeText::IChannelTypeText(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeText::IChannelTypeText(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeText::IChannelTypeText(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeText::IChannelTypeText(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeText::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(LostMessage()), NULL, NULL);
            disconnect(this, SIGNAL(Received(uint, uint, uint, uint, uint, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(SendError(uint, uint, uint, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(Sent(uint, uint, const QString&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
