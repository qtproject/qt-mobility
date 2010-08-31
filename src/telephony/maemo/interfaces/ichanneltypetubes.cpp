#include "maemo/interfaces/ichanneltypetubes.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeTubes::IChannelTypeTubes(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeTubes::IChannelTypeTubes(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeTubes::IChannelTypeTubes(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeTubes::IChannelTypeTubes(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeTubes::IChannelTypeTubes(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeTubes::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(NewTube(uint, uint, uint, const QString&, const QVariantMap&, uint)), NULL, NULL);
            disconnect(this, SIGNAL(TubeStateChanged(uint, uint)), NULL, NULL);
            disconnect(this, SIGNAL(TubeClosed(uint)), NULL, NULL);
            disconnect(this, SIGNAL(DBusNamesChanged(uint, const DBus::Interfaces::DBusTubeMemberList&, const DBus::Interfaces::UIntList&)), NULL, NULL);
            disconnect(this, SIGNAL(StreamTubeNewConnection(uint, uint)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
