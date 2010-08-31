#include "maemo/interfaces/ichannelgroup.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelGroup::IChannelGroup(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelGroup::IChannelGroup(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelGroup::IChannelGroup(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelGroup::IChannelGroup(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelGroup::IChannelGroup(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelGroup::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(HandleOwnersChanged(const DBus::Interfaces::HandleOwnerMap&, const DBus::Interfaces::UIntList&)), NULL, NULL);
            disconnect(this, SIGNAL(SelfHandleChanged(uint)), NULL, NULL);
            disconnect(this, SIGNAL(GroupFlagsChanged(uint, uint)), NULL, NULL);
            disconnect(this, SIGNAL(MembersChanged(const QString&, const DBus::Interfaces::UIntList&, const DBus::Interfaces::UIntList&, const DBus::Interfaces::UIntList&, const DBus::Interfaces::UIntList&, uint, uint)), NULL, NULL);
            disconnect(this, SIGNAL(MembersChangedDetailed(const DBus::Interfaces::UIntList&, const DBus::Interfaces::UIntList&, const DBus::Interfaces::UIntList&, const DBus::Interfaces::UIntList&, const QVariantMap&)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
