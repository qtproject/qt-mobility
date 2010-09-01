#include "maemo/interfaces/ichanneltypefiletransfer.h"

namespace DBus
{
    namespace Interfaces
    {
        IChannelTypeFileTransfer::IChannelTypeFileTransfer(const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
        {
        }

        IChannelTypeFileTransfer::IChannelTypeFileTransfer(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
            : DBus::IAbstract(busName, objectPath, staticInterfaceName(), connection, parent)
        {
        }

        IChannelTypeFileTransfer::IChannelTypeFileTransfer(DBus::DBusProxy *proxy)
            : DBus::IAbstract(proxy, staticInterfaceName())
        {
        }

        IChannelTypeFileTransfer::IChannelTypeFileTransfer(const DBus::Interfaces::IChannel& mainInterface)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
        {
        }

        IChannelTypeFileTransfer::IChannelTypeFileTransfer(const DBus::Interfaces::IChannel& mainInterface, QObject *parent)
            : DBus::IAbstract(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
        {
        }

        void IChannelTypeFileTransfer::invalidate(DBus::DBusProxy *proxy,
                const QString &error, const QString &message)
        {
            disconnect(this, SIGNAL(FileTransferStateChanged(uint, uint)), NULL, NULL);
            disconnect(this, SIGNAL(TransferredBytesChanged(qulonglong)), NULL, NULL);
            disconnect(this, SIGNAL(InitialOffsetDefined(qulonglong)), NULL, NULL);

            DBus::IAbstract::invalidate(proxy, error, message);
        }
    }
}
