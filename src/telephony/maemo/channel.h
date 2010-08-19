#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtCore>
#include <QtDBus/QDBusMessage>
#include "maemo/types.h"
#include "maemo/dbus-proxy.h"
#include "maemo/interfaces/cli-channel.h"
#include "maemo/ready-object.h"
#include "maemo/connection.h"
#include "qtelephonycallinfo.h"
#include "qtelephony.h"

QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

namespace Tp
{
    class PendingOperation;
    class Channel : public StatefulDBusProxy
                        , public ReadyObject
                        , public RefCounted
    {
        Q_OBJECT
        Q_DISABLE_COPY(Channel)
    public:
        static const Feature FeatureCore;
        static bool isCall(QString channeltype);

        Channel(QDBusConnection busconnection, const QString busname, const QString &objectPath, const QVariantMap &properties, Connection* connection);
        ~Channel();
        QVariantMap properties() const { return propertylist; }
        QTelephonyEvents::CallStatus getStatus() { return status; }
        int getDirection() { return direction; }
        QString getRemotePartyIdentifier() { return remoteIdentifier; }
        QTelephonyEvents::CallType getCalltype();
        QString getSubType() { return subtype; }

        bool isCall() {
            if(pChannelTypeStreamedMediaInterface || pChannelTypeTextInterface)
                return true;
            return false;
        }
        bool isOutgoing() { return ( direction == 2 ? true : false); }
        bool isIncomming() { return ( direction == 1 ? true : false); }
        bool isText() { return (pChannelTypeTextInterface == 0 ? false : true); }

    Q_SIGNALS:
        void StatusChanged(Tp::Channel* pchannel);

    private Q_SLOTS:
        //for ReadyObject
        void onChannelReady(Tp::PendingOperation* operation);

        //for ChannelInterface signals
        void onClose();

        //for ChannelInterfaceCallStateInterface signals
        void onCallStateChanged(uint contact, uint state);

        //for ChannelInterfaceChatStateInterface signals
        void onChatStateChanged(uint contact, uint state);

        //for ChannelInterfaceDTMFInterface signals
        //- no signals

        //for ChannelInterfaceGroupInterface signals
        void onHandleOwnersChanged(const Tp::HandleOwnerMap& added, const Tp::UIntList& removed);
        void onSelfHandleChanged(uint selfHandle);
        void onGroupFlagsChanged(uint added, uint removed);
        void onMembersChanged(const QString& message, const Tp::UIntList& added, const Tp::UIntList& removed, const Tp::UIntList& localPending, const Tp::UIntList& remotePending, uint actor, uint reason);
        void onMembersChangedDetailed(const Tp::UIntList& added, const Tp::UIntList& removed, const Tp::UIntList& localPending, const Tp::UIntList& remotePending, const QVariantMap& details);

        //for ChannelInterfaceHoldInterface signals
        void onHoldStateChanged(uint holdState, uint reason);

        //for ChannelInterfaceMediaSignallingInterface signals
        void onNewSessionHandler(const QDBusObjectPath& sessionHandler, const QString& sessionType);

        // for ChannelInterfaceMessagesInterface signals
        void onMessageSent(const Tp::MessagePartList& content, uint flags, const QString& messageToken);
        void onPendingMessagesRemoved(const Tp::UIntList& messageIDs);
        void onMessageReceived(const Tp::MessagePartList& message);

        //for ChannelInterfacePasswordInterface signals
        void onPasswordFlagsChanged(uint added, uint removed);

        //for ChannelInterfaceTubeInterface signals
        void onTubeChannelStateChanged(uint state);


        //for ChannelTypeContactListInterface signals
        //no signals

        //for ChannelTypeFileTransferInterface signals
        void onFileTransferStateChanged(uint state, uint reason);
        void onTransferredBytesChanged(qulonglong count);
        void onInitialOffsetDefined(qulonglong initialOffset);

        //for ChannelTypeRoomListInterface signals
        void onGotRooms(const Tp::RoomInfoList& rooms);
        void onListingRooms(bool listing);

        //for ChannelTypeStreamTubeInterface signals
        void onNewRemoteConnection(uint handle, const QDBusVariant& connectionParam, uint connectionID);
        void onNewLocalConnection(uint connectionID);
        void onConnectionClosed(uint connectionID, const QString& error, const QString& message);

        //for ChannelTypeStreamedMediaInterface signals
        void onStreamAdded(uint streamID, uint contactHandle, uint streamType);
        void onStreamDirectionChanged(uint streamID, uint streamDirection, uint pendingFlags);
        void onStreamError(uint streamID, uint errorCode, const QString& message);
        void onStreamRemoved(uint streamID);
        void onStreamStateChanged(uint streamID, uint streamState);

        //for ChannelTypeTextInterface signals
        void onLostMessage();
        void onReceived(uint ID, uint timestamp, uint sender, uint type, uint flags, const QString& text);
        void onSendError(uint error, uint timestamp, uint type, const QString& text);
        void onSent(uint timestamp, uint type, const QString& text);

        //for ChannelTypeTubesInterface signals
        void onNewTube(uint ID, uint initiator, uint type, const QString& service, const QVariantMap& parameters, uint state);
        void onTubeStateChanged(uint ID, uint state);
        void onTubeClosed(uint ID);
        void onDBusNamesChanged(uint ID, const Tp::DBusTubeMemberList& added, const Tp::UIntList& removed);
        void onStreamTubeNewConnection(uint ID, uint handle);


    private:
        void connectInterfaces();
        void disconnectInterfaces();
        void connectType();
        void disconnectType();
        void init();
        void createPropertiyList();

        QVariantMap propertylist;

    public:
        Tp::Client::ChannelInterface* pChannelInterface;
        Tp::Client::ChannelInterfaceCallStateInterface* pChannelInterfaceCallStateInterface;
        Tp::Client::ChannelInterfaceChatStateInterface* pChannelInterfaceChatStateInterface;
        Tp::Client::ChannelInterfaceDTMFInterface* pChannelInterfaceDTMFInterface;
        Tp::Client::ChannelInterfaceGroupInterface* pChannelInterfaceGroupInterface;
        Tp::Client::ChannelInterfaceHoldInterface* pChannelInterfaceHoldInterface;
        Tp::Client::ChannelInterfaceMediaSignallingInterface* pChannelInterfaceMediaSignallingInterface;
        Tp::Client::ChannelInterfaceMessagesInterface* pChannelInterfaceMessagesInterface;
        Tp::Client::ChannelInterfacePasswordInterface* pChannelInterfacePasswordInterface;
        Tp::Client::ChannelInterfaceTubeInterface* pChannelInterfaceTubeInterface;
        Tp::Client::ChannelTypeContactListInterface* pChannelTypeContactListInterface;
        Tp::Client::ChannelTypeFileTransferInterface* pChannelTypeFileTransferInterface;
        Tp::Client::ChannelTypeRoomListInterface* pChannelTypeRoomListInterface;
        Tp::Client::ChannelTypeStreamTubeInterface* pChannelTypeStreamTubeInterface;
        Tp::Client::ChannelTypeStreamedMediaInterface* pChannelTypeStreamedMediaInterface;
        Tp::Client::ChannelTypeTextInterface* pChannelTypeTextInterface;
        Tp::Client::ChannelTypeTubesInterface* pChannelTypeTubesInterface;

    private:
        QTelephonyEvents::CallStatus status;
        Connection* connection;
        /*
         0 = unknown
         1 = incomming
         2 = outgoing
         */
        int direction;
        bool wasExistingChannel;
        QString remoteIdentifier;
        QString subtype;
    };
    typedef SharedPtr<Channel> ChannelPtr;
}
#endif // CHANNEL_H
