#include "maemo/interfaces/ichannelmessages.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelMessages::IChannelMessages(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelMessages::IChannelMessages(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelMessages::IChannelMessages(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelMessages::IChannelMessages(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelMessages::IChannelMessages(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelMessages::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(MessageSent(const DBus::Interfaces::MessagePartList&, uint, const QString&)), NULL, NULL);
            disconnect(this, SIGNAL(PendingMessagesRemoved(const DBus::Interfaces::UIntList&)), NULL, NULL);
            disconnect(this, SIGNAL(MessageReceived(const DBus::Interfaces::MessagePartList&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
