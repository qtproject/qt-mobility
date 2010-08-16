#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-channel.h"

namespace Tp
{
namespace Client
{

ChannelInterface::ChannelInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterface::ChannelInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterface::ChannelInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void ChannelInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(Closed()), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceCallStateInterface::ChannelInterfaceCallStateInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceCallStateInterface::ChannelInterfaceCallStateInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceCallStateInterface::ChannelInterfaceCallStateInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceCallStateInterface::ChannelInterfaceCallStateInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceCallStateInterface::ChannelInterfaceCallStateInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceCallStateInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(CallStateChanged(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceChatStateInterface::ChannelInterfaceChatStateInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceChatStateInterface::ChannelInterfaceChatStateInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceChatStateInterface::ChannelInterfaceChatStateInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceChatStateInterface::ChannelInterfaceChatStateInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceChatStateInterface::ChannelInterfaceChatStateInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceChatStateInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(ChatStateChanged(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceDTMFInterface::ChannelInterfaceDTMFInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceDTMFInterface::ChannelInterfaceDTMFInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceDTMFInterface::ChannelInterfaceDTMFInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceDTMFInterface::ChannelInterfaceDTMFInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceDTMFInterface::ChannelInterfaceDTMFInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceDTMFInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceGroupInterface::ChannelInterfaceGroupInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceGroupInterface::ChannelInterfaceGroupInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceGroupInterface::ChannelInterfaceGroupInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceGroupInterface::ChannelInterfaceGroupInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceGroupInterface::ChannelInterfaceGroupInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceGroupInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(HandleOwnersChanged(const Tp::HandleOwnerMap&, const Tp::UIntList&)), NULL, NULL);
    disconnect(this, SIGNAL(SelfHandleChanged(uint)), NULL, NULL);
    disconnect(this, SIGNAL(GroupFlagsChanged(uint, uint)), NULL, NULL);
    disconnect(this, SIGNAL(MembersChanged(const QString&, const Tp::UIntList&, const Tp::UIntList&, const Tp::UIntList&, const Tp::UIntList&, uint, uint)), NULL, NULL);
    disconnect(this, SIGNAL(MembersChangedDetailed(const Tp::UIntList&, const Tp::UIntList&, const Tp::UIntList&, const Tp::UIntList&, const QVariantMap&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceHoldInterface::ChannelInterfaceHoldInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceHoldInterface::ChannelInterfaceHoldInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceHoldInterface::ChannelInterfaceHoldInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceHoldInterface::ChannelInterfaceHoldInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceHoldInterface::ChannelInterfaceHoldInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceHoldInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(HoldStateChanged(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceMediaSignallingInterface::ChannelInterfaceMediaSignallingInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceMediaSignallingInterface::ChannelInterfaceMediaSignallingInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceMediaSignallingInterface::ChannelInterfaceMediaSignallingInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceMediaSignallingInterface::ChannelInterfaceMediaSignallingInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceMediaSignallingInterface::ChannelInterfaceMediaSignallingInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceMediaSignallingInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(NewSessionHandler(const QDBusObjectPath&, const QString&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceMessagesInterface::ChannelInterfaceMessagesInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceMessagesInterface::ChannelInterfaceMessagesInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceMessagesInterface::ChannelInterfaceMessagesInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceMessagesInterface::ChannelInterfaceMessagesInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceMessagesInterface::ChannelInterfaceMessagesInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceMessagesInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(MessageSent(const Tp::MessagePartList&, uint, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(PendingMessagesRemoved(const Tp::UIntList&)), NULL, NULL);
    disconnect(this, SIGNAL(MessageReceived(const Tp::MessagePartList&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfacePasswordInterface::ChannelInterfacePasswordInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfacePasswordInterface::ChannelInterfacePasswordInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfacePasswordInterface::ChannelInterfacePasswordInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfacePasswordInterface::ChannelInterfacePasswordInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfacePasswordInterface::ChannelInterfacePasswordInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfacePasswordInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(PasswordFlagsChanged(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelInterfaceTubeInterface::ChannelInterfaceTubeInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelInterfaceTubeInterface::ChannelInterfaceTubeInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelInterfaceTubeInterface::ChannelInterfaceTubeInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelInterfaceTubeInterface::ChannelInterfaceTubeInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelInterfaceTubeInterface::ChannelInterfaceTubeInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelInterfaceTubeInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(TubeChannelStateChanged(uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeContactListInterface::ChannelTypeContactListInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeContactListInterface::ChannelTypeContactListInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeContactListInterface::ChannelTypeContactListInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeContactListInterface::ChannelTypeContactListInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeContactListInterface::ChannelTypeContactListInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeContactListInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeFileTransferInterface::ChannelTypeFileTransferInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeFileTransferInterface::ChannelTypeFileTransferInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeFileTransferInterface::ChannelTypeFileTransferInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeFileTransferInterface::ChannelTypeFileTransferInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeFileTransferInterface::ChannelTypeFileTransferInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeFileTransferInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(FileTransferStateChanged(uint, uint)), NULL, NULL);
    disconnect(this, SIGNAL(TransferredBytesChanged(qulonglong)), NULL, NULL);
    disconnect(this, SIGNAL(InitialOffsetDefined(qulonglong)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeRoomListInterface::ChannelTypeRoomListInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeRoomListInterface::ChannelTypeRoomListInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeRoomListInterface::ChannelTypeRoomListInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeRoomListInterface::ChannelTypeRoomListInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeRoomListInterface::ChannelTypeRoomListInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeRoomListInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(GotRooms(const Tp::RoomInfoList&)), NULL, NULL);
    disconnect(this, SIGNAL(ListingRooms(bool)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeStreamTubeInterface::ChannelTypeStreamTubeInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeStreamTubeInterface::ChannelTypeStreamTubeInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeStreamTubeInterface::ChannelTypeStreamTubeInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeStreamTubeInterface::ChannelTypeStreamTubeInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeStreamTubeInterface::ChannelTypeStreamTubeInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeStreamTubeInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(NewRemoteConnection(uint, const QDBusVariant&, uint)), NULL, NULL);
    disconnect(this, SIGNAL(NewLocalConnection(uint)), NULL, NULL);
    disconnect(this, SIGNAL(ConnectionClosed(uint, const QString&, const QString&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeStreamedMediaInterface::ChannelTypeStreamedMediaInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeStreamedMediaInterface::ChannelTypeStreamedMediaInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeStreamedMediaInterface::ChannelTypeStreamedMediaInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeStreamedMediaInterface::ChannelTypeStreamedMediaInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeStreamedMediaInterface::ChannelTypeStreamedMediaInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeStreamedMediaInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(StreamAdded(uint, uint, uint)), NULL, NULL);
    disconnect(this, SIGNAL(StreamDirectionChanged(uint, uint, uint)), NULL, NULL);
    disconnect(this, SIGNAL(StreamError(uint, uint, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(StreamRemoved(uint)), NULL, NULL);
    disconnect(this, SIGNAL(StreamStateChanged(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeTextInterface::ChannelTypeTextInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeTextInterface::ChannelTypeTextInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeTextInterface::ChannelTypeTextInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeTextInterface::ChannelTypeTextInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeTextInterface::ChannelTypeTextInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeTextInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(LostMessage()), NULL, NULL);
    disconnect(this, SIGNAL(Received(uint, uint, uint, uint, uint, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(SendError(uint, uint, uint, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(Sent(uint, uint, const QString&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ChannelTypeTubesInterface::ChannelTypeTubesInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ChannelTypeTubesInterface::ChannelTypeTubesInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ChannelTypeTubesInterface::ChannelTypeTubesInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ChannelTypeTubesInterface::ChannelTypeTubesInterface(const Tp::Client::ChannelInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ChannelTypeTubesInterface::ChannelTypeTubesInterface(const Tp::Client::ChannelInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ChannelTypeTubesInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(NewTube(uint, uint, uint, const QString&, const QVariantMap&, uint)), NULL, NULL);
    disconnect(this, SIGNAL(TubeStateChanged(uint, uint)), NULL, NULL);
    disconnect(this, SIGNAL(TubeClosed(uint)), NULL, NULL);
    disconnect(this, SIGNAL(DBusNamesChanged(uint, const Tp::DBusTubeMemberList&, const Tp::UIntList&)), NULL, NULL);
    disconnect(this, SIGNAL(StreamTubeNewConnection(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
