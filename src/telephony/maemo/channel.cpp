#include "qtelephony.h"
#include "maemo/channel.h"
#include "maemo/cli-constants.h"
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/pending-operation.h"

#include <QtCore/QDebug>

namespace Tp
{
    const Feature Channel::FeatureCore = Feature(QLatin1String(Channel::staticMetaObject.className()), 0, true);

    Channel::Channel(QDBusConnection busconnection, const QString busname, const QString &objectPath, const QVariantMap &properties, Connection* conn)
        : StatefulDBusProxy(busconnection, busname, objectPath)
        , ReadyObject(this, FeatureCore)
        , propertylist(properties)
        , pChannelInterface(0)
        , pChannelInterfaceChatStateInterface(0)
        , pChannelInterfaceDTMFInterface(0)
        , pChannelInterfaceGroupInterface(0)
        , pChannelInterfaceHoldInterface(0)
        , pChannelInterfaceMediaSignallingInterface(0)
        , pChannelInterfaceMessagesInterface(0)
        , pChannelInterfacePasswordInterface(0)
        , pChannelInterfaceTubeInterface(0)
        , pChannelTypeContactListInterface(0)
        , pChannelTypeFileTransferInterface(0)
        , pChannelTypeRoomListInterface(0)
        , pChannelTypeStreamTubeInterface(0)
        , pChannelTypeStreamedMediaInterface(0)
        , pChannelTypeTextInterface(0)
        , pChannelTypeTubesInterface(0)
        , status(Telephony::Idle)
        , connection(conn)
        , direction(0)
    {
        qDebug() << " Channel::Channel(";
        qDebug() << "- QDBusConnection base service: " << busconnection.baseService();
        qDebug() << "- QDBusConnection name: " << busconnection.name();
        qDebug() << "- objectPath: " << objectPath;
        qDebug() << "- DBusProxy.busName: " << this->busName();
        qDebug() << "- DBusProxy.objectPath: " << this->objectPath();

        int idx = this->objectPath().indexOf(QString("/org/freedesktop/Telepathy/Connection/ring/tel/ring/incoming"));
        qDebug() << "idx: " << idx;
        if(idx == 0)
            direction = 1;
        else{
            idx = this->objectPath().indexOf(QString("/org/freedesktop/Telepathy/Connection/ring/tel/ring/outgoing"));
            if(idx == 0)
                direction = 2;
        }

        connect((QObject*)this->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)), SLOT(onChannelReady(Tp::PendingOperation*)));
        qDebug() << "Current hread: " << QThread::currentThreadId();
    }

    Channel::~Channel()
    {
        qDebug() << "Channel::~Channel";
        disconnectType();
        disconnectInterfaces();
        if(pChannelInterfaceCallStateInterface)
            delete pChannelInterfaceCallStateInterface;
        if(pChannelInterfaceChatStateInterface)
            delete pChannelInterfaceChatStateInterface;
        if(pChannelInterfaceDTMFInterface)
            delete pChannelInterfaceDTMFInterface;
        if(pChannelInterfaceGroupInterface)
            delete pChannelInterfaceGroupInterface;
        if(pChannelInterfaceHoldInterface)
            delete pChannelInterfaceHoldInterface;
        if(pChannelInterfaceMediaSignallingInterface)
            delete pChannelInterfaceMediaSignallingInterface;
        if(pChannelInterfaceMessagesInterface)
            delete pChannelInterfaceMessagesInterface;
        if(pChannelInterfacePasswordInterface)
            delete pChannelInterfacePasswordInterface;
        if(pChannelInterfaceTubeInterface)
            delete pChannelInterfaceTubeInterface;
        if(pChannelTypeContactListInterface)
            delete pChannelTypeContactListInterface;
        if(pChannelTypeFileTransferInterface)
            delete pChannelTypeFileTransferInterface;
        if(pChannelTypeRoomListInterface)
            delete pChannelTypeRoomListInterface;
        if(pChannelTypeStreamTubeInterface)
            delete pChannelTypeStreamTubeInterface;
        if(pChannelTypeStreamedMediaInterface)
            delete pChannelTypeStreamedMediaInterface;
        if(pChannelTypeTextInterface)
            delete pChannelTypeTextInterface;
        if(pChannelTypeTubesInterface)
            delete pChannelTypeTubesInterface;
        if(pChannelInterface)
            delete pChannelInterface;
    }

    void Channel::connectInterfaces()
    {
        qDebug() << "Channel::connectInterfaces()";

        pChannelInterface = new Tp::Client::ChannelInterface(this->dbusConnection(),this->busName(), this->objectPath());
        connect(pChannelInterface, SIGNAL(Closed()), SLOT(onClose()));

        if(propertylist.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Interfaces"))){
            QStringList varlist = propertylist.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Interfaces")).toStringList();
            foreach(const QString& supportedinterface, varlist)
            {
                qDebug() << "- supported interface: " << supportedinterface;
                if(supportedinterface == Tp::Client::ChannelInterfaceCallStateInterface::staticInterfaceName()){
                    pChannelInterfaceCallStateInterface = new Tp::Client::ChannelInterfaceCallStateInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceCallStateInterface, SIGNAL(CallStateChanged(uint,uint)), SLOT(onCallStateChanged(uint,uint)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceChatStateInterface::staticInterfaceName()){
                    pChannelInterfaceChatStateInterface = new Tp::Client::ChannelInterfaceChatStateInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceChatStateInterface, SIGNAL(ChatStateChanged(uint,uint)), SLOT(onChatStateChanged(uint,uint)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceDTMFInterface::staticInterfaceName()){
                    pChannelInterfaceDTMFInterface = new Tp::Client::ChannelInterfaceDTMFInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    //no signals
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceGroupInterface::staticInterfaceName()){
                    pChannelInterfaceGroupInterface = new Tp::Client::ChannelInterfaceGroupInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceGroupInterface, SIGNAL(HandleOwnersChanged(Tp::HandleOwnerMap,Tp::UIntList)), SLOT(onHandleOwnersChanged(Tp::HandleOwnerMap,Tp::UIntList)));
                    connect(pChannelInterfaceGroupInterface, SIGNAL(SelfHandleChanged(uint)), SLOT(onSelfHandleChanged(uint)));
                    connect(pChannelInterfaceGroupInterface, SIGNAL(GroupFlagsChanged(uint,uint)), SLOT(onGroupFlagsChanged(uint,uint)));
                    connect(pChannelInterfaceGroupInterface, SIGNAL(MembersChanged(QString,Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,uint,uint)), SLOT(onMembersChanged(QString,Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,uint,uint)));
                    connect(pChannelInterfaceGroupInterface, SIGNAL(MembersChangedDetailed(Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,QVariantMap)), SLOT(onMembersChangedDetailed(Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,QVariantMap)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceHoldInterface::staticInterfaceName()){
                    pChannelInterfaceHoldInterface = new Tp::Client::ChannelInterfaceHoldInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceHoldInterface, SIGNAL(HoldStateChanged(uint,uint)), SLOT(onHoldStateChanged(uint,uint)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceMediaSignallingInterface::staticInterfaceName()){
                    pChannelInterfaceMediaSignallingInterface = new Tp::Client::ChannelInterfaceMediaSignallingInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceMediaSignallingInterface, SIGNAL(NewSessionHandler(QDBusObjectPath,QString)), SLOT(onNewSessionHandler(QDBusObjectPath,QString)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceMessagesInterface::staticInterfaceName()){
                    pChannelInterfaceMessagesInterface = new Tp::Client::ChannelInterfaceMessagesInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceMessagesInterface, SIGNAL(MessageReceived(Tp::MessagePartList)), SLOT(onMessageReceived(Tp::MessagePartList)));
                    connect(pChannelInterfaceMessagesInterface, SIGNAL(MessageSent(Tp::MessagePartList,uint,QString)), SLOT(onMessageSent(Tp::MessagePartList,uint,QString)));
                    connect(pChannelInterfaceMessagesInterface, SIGNAL(PendingMessagesRemoved(Tp::UIntList)), SLOT(onPendingMessagesRemoved(Tp::UIntList)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfacePasswordInterface::staticInterfaceName()){
                    pChannelInterfacePasswordInterface = new Tp::Client::ChannelInterfacePasswordInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfacePasswordInterface, SIGNAL(PasswordFlagsChanged(uint,uint)), SLOT(onPasswordFlagsChanged(uint,uint)));
                }
                else if(supportedinterface == Tp::Client::ChannelInterfaceTubeInterface::staticInterfaceName()){
                    pChannelInterfaceTubeInterface = new Tp::Client::ChannelInterfaceTubeInterface(this->dbusConnection(), this->busName(), this->objectPath());
                    connect(pChannelInterfaceTubeInterface, SIGNAL(TubeChannelStateChanged(uint)), SLOT(onTubeChannelStateChanged(uint)));
                }
            }
        }
    }

    void Channel::disconnectInterfaces()
    {
        qDebug() << "Channel::disconnectInterfaces()";
        if(pChannelInterfaceCallStateInterface){
            disconnect(pChannelInterfaceCallStateInterface, SIGNAL(CallStateChanged(uint,uint)), this, SLOT(onCallStateChanged(uint,uint)));
        }
        if(pChannelInterfaceDTMFInterface){
            //no signals
        }
        if(pChannelInterfaceGroupInterface){
            disconnect(pChannelInterfaceGroupInterface, SIGNAL(HandleOwnersChanged(Tp::HandleOwnerMap,Tp::UIntList)), this, SLOT(onHandleOwnersChanged(Tp::HandleOwnerMap,Tp::UIntList)));
            disconnect(pChannelInterfaceGroupInterface, SIGNAL(SelfHandleChanged(uint)), this, SLOT(onSelfHandleChanged(uint)));
            disconnect(pChannelInterfaceGroupInterface, SIGNAL(GroupFlagsChanged(uint,uint)), this, SLOT(onGroupFlagsChanged(uint,uint)));
            disconnect(pChannelInterfaceGroupInterface, SIGNAL(MembersChanged(QString,Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,uint,uint)), this, SLOT(onMembersChanged(QString,Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,uint,uint)));
            disconnect(pChannelInterfaceGroupInterface, SIGNAL(MembersChangedDetailed(Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,QVariantMap)), this, SLOT(onMembersChangedDetailed(Tp::UIntList,Tp::UIntList,Tp::UIntList,Tp::UIntList,QVariantMap)));
        }
        if(pChannelInterfaceHoldInterface){
            disconnect(pChannelInterfaceHoldInterface, SIGNAL(HoldStateChanged(uint,uint)), this, SLOT(onHoldStateChanged(uint,uint)));
        }
        if(pChannelInterfaceMediaSignallingInterface){
            disconnect(pChannelInterfaceMediaSignallingInterface, SIGNAL(NewSessionHandler(QDBusObjectPath,QString)), this, SLOT(onNewSessionHandler(QDBusObjectPath,QString)));
        }
        if(pChannelInterfaceMessagesInterface){
            disconnect(pChannelInterfaceMessagesInterface, SIGNAL(MessageReceived(Tp::MessagePartList)), this, SLOT(onMessageReceived(Tp::MessagePartList)));
            disconnect(pChannelInterfaceMessagesInterface, SIGNAL(MessageSent(Tp::MessagePartList,uint,QString)), this, SLOT(onMessageSent(Tp::MessagePartList,uint,QString)));
            disconnect(pChannelInterfaceMessagesInterface, SIGNAL(PendingMessagesRemoved(Tp::UIntList)), this, SLOT(onPendingMessagesRemoved(Tp::UIntList)));
        }
        if(pChannelInterfacePasswordInterface){
            disconnect(pChannelInterfacePasswordInterface, SIGNAL(PasswordFlagsChanged(uint,uint)), this, SLOT(onPasswordFlagsChanged(uint,uint)));
        }
        if(pChannelInterfaceTubeInterface){
            disconnect(pChannelInterfaceTubeInterface, SIGNAL(TubeChannelStateChanged(uint)), this, SLOT(onTubeChannelStateChanged(uint)));
        }
    }

    void Channel::connectType()
    {
        qDebug() << "Channel::connectType()";
        if( propertylist.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"))){
            QString type = propertylist.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();
            if(type == Tp::Client::ChannelTypeContactListInterface::staticInterfaceName()){
                pChannelTypeContactListInterface = new Tp::Client::ChannelTypeContactListInterface(this->dbusConnection(), this->busName(), this->objectPath());
                // no signals
            }
            else if(type == Tp::Client::ChannelTypeFileTransferInterface::staticInterfaceName()){
                pChannelTypeFileTransferInterface = new Tp::Client::ChannelTypeFileTransferInterface(this->dbusConnection(), this->busName(), this->objectPath());
                connect(pChannelTypeFileTransferInterface, SIGNAL(FileTransferStateChanged(uint,uint)), SLOT(onFileTransferStateChanged(uint,uint)));
                connect(pChannelTypeFileTransferInterface, SIGNAL(TransferredBytesChanged(qulonglong)), SLOT(onTransferredBytesChanged(qulonglong)));
                connect(pChannelTypeFileTransferInterface, SIGNAL(InitialOffsetDefined(qulonglong)), SLOT(onInitialOffsetDefined(qulonglong)));
            }
            else if(type == Tp::Client::ChannelTypeRoomListInterface::staticInterfaceName()){
                pChannelTypeRoomListInterface = new Tp::Client::ChannelTypeRoomListInterface(this->dbusConnection(), this->busName(), this->objectPath());
                connect(pChannelTypeRoomListInterface, SIGNAL(GotRooms(Tp::RoomInfoList)), SLOT(onGotRooms(Tp::RoomInfoList)));
                connect(pChannelTypeRoomListInterface, SIGNAL(ListingRooms(bool)), SLOT(onListingRooms(bool)));
            }
            else if(type == Tp::Client::ChannelTypeStreamTubeInterface::staticInterfaceName()){
                pChannelTypeStreamTubeInterface = new Tp::Client::ChannelTypeStreamTubeInterface(this->dbusConnection(), this->busName(), this->objectPath());
                connect(pChannelTypeStreamTubeInterface, SIGNAL(NewRemoteConnection(uint,QDBusVariant,uint)), SLOT(onNewRemoteConnection(uint,QDBusVariant,uint)));
                connect(pChannelTypeStreamTubeInterface, SIGNAL(NewLocalConnection(uint)), SLOT(onNewLocalConnection(uint)));
                connect(pChannelTypeStreamTubeInterface, SIGNAL(ConnectionClosed(uint,QString,QString)), SLOT(onConnectionClosed(uint,QString,QString)));
            }
            else if(type == Tp::Client::ChannelTypeStreamedMediaInterface::staticInterfaceName()){
                pChannelTypeStreamedMediaInterface = new Tp::Client::ChannelTypeStreamedMediaInterface(this->dbusConnection(), this->busName(), this->objectPath());
                connect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamAdded(uint,uint,uint)), SLOT(onStreamAdded(uint,uint,uint)));
                connect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamDirectionChanged(uint,uint,uint)), SLOT(onStreamDirectionChanged(uint,uint,uint)));
                connect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamError(uint,uint,QString)), SLOT(onStreamError(uint,uint,QString)));
                connect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamRemoved(uint)), SLOT(onStreamRemoved(uint)));
                connect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamStateChanged(uint,uint)), SLOT(onStreamStateChanged(uint,uint)));
            }
            else if(type == Tp::Client::ChannelTypeTextInterface::staticInterfaceName()){
                pChannelTypeTextInterface = new Tp::Client::ChannelTypeTextInterface(this->dbusConnection(), this->busName(), this->objectPath());
                connect(pChannelTypeTextInterface, SIGNAL(LostMessage()), SLOT(onLostMessage()));
                connect(pChannelTypeTextInterface,SIGNAL(Received(uint,uint,uint,uint,uint,QString)), SLOT(onReceived(uint,uint,uint,uint,uint,QString)));
                connect(pChannelTypeTextInterface, SIGNAL(SendError(uint,uint,uint,QString)), SLOT(onSendError(uint,uint,uint,QString)));
                connect(pChannelTypeTextInterface, SIGNAL(Sent(uint,uint,QString)), SLOT(onSent(uint,uint,QString)));
            }
            else if(type == Tp::Client::ChannelTypeTubesInterface::staticInterfaceName()){
                pChannelTypeTubesInterface = new Tp::Client::ChannelTypeTubesInterface(this->dbusConnection(), this->busName(), this->objectPath());
                connect(pChannelTypeTubesInterface, SIGNAL(DBusNamesChanged(uint,Tp::DBusTubeMemberList,Tp::UIntList)), SLOT(onDBusNamesChanged(uint,Tp::DBusTubeMemberList,Tp::UIntList)));
                connect(pChannelTypeTubesInterface, SIGNAL(NewTube(uint,uint,uint,QString,QVariantMap,uint)), SLOT(onNewTube(uint,uint,uint,QString,QVariantMap,uint)));
                connect(pChannelTypeTubesInterface, SIGNAL(StreamTubeNewConnection(uint,uint)), SLOT(onStreamTubeNewConnection(uint,uint)));
                connect(pChannelTypeTubesInterface, SIGNAL(TubeClosed(uint)), SLOT(onTubeClosed(uint)));
                connect(pChannelTypeTubesInterface, SIGNAL(TubeStateChanged(uint,uint)), SLOT(onTubeStateChanged(uint,uint)));
            }
        }
    }

    void Channel::disconnectType()
    {
        qDebug() << "Channel::disconnectType()";
        if(pChannelTypeContactListInterface){
            // no signals
        }
        if(pChannelTypeFileTransferInterface){
            disconnect(pChannelTypeFileTransferInterface, SIGNAL(FileTransferStateChanged(uint,uint)), this, SLOT(onFileTransferStateChanged(uint,uint)));
            disconnect(pChannelTypeFileTransferInterface, SIGNAL(TransferredBytesChanged(qulonglong)), this, SLOT(onTransferredBytesChanged(qulonglong)));
            disconnect(pChannelTypeFileTransferInterface, SIGNAL(InitialOffsetDefined(qulonglong)), this, SLOT(onInitialOffsetDefined(qulonglong)));
        }
        if(pChannelTypeRoomListInterface){
            disconnect(pChannelTypeRoomListInterface, SIGNAL(GotRooms(Tp::RoomInfoList)), this, SLOT(onGotRooms(Tp::RoomInfoList)));
            disconnect(pChannelTypeRoomListInterface, SIGNAL(ListingRooms(bool)), this, SLOT(onListingRooms(bool)));
        }
        if(pChannelTypeStreamTubeInterface){
            disconnect(pChannelTypeStreamTubeInterface, SIGNAL(NewRemoteConnection(uint,QDBusVariant,uint)), this, SLOT(onNewRemoteConnection(uint,QDBusVariant,uint)));
            disconnect(pChannelTypeStreamTubeInterface, SIGNAL(NewLocalConnection(uint)), this, SLOT(onNewLocalConnection(uint)));
            disconnect(pChannelTypeStreamTubeInterface, SIGNAL(ConnectionClosed(uint,QString,QString)), this, SLOT(onConnectionClosed(uint,QString,QString)));
        }
        if(pChannelTypeStreamedMediaInterface){
            disconnect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamAdded(uint,uint,uint)), this, SLOT(onStreamAdded(uint,uint,uint)));
            disconnect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamDirectionChanged(uint,uint,uint)), this, SLOT(onStreamDirectionChanged(uint,uint,uint)));
            disconnect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamError(uint,uint,QString)), this, SLOT(onStreamError(uint,uint,QString)));
            disconnect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamRemoved(uint)), this, SLOT(onStreamRemoved(uint)));
            disconnect(pChannelTypeStreamedMediaInterface, SIGNAL(StreamStateChanged(uint,uint)), this, SLOT(onStreamStateChanged(uint,uint)));
        }
        if(pChannelTypeTextInterface){
            disconnect(pChannelTypeTextInterface, SIGNAL(LostMessage()), this, SLOT(onLostMessage()));
            disconnect(pChannelTypeTextInterface,SIGNAL(Received(uint,uint,uint,uint,uint,QString)), this, SLOT(onReceived(uint,uint,uint,uint,uint,QString)));
            disconnect(pChannelTypeTextInterface, SIGNAL(SendError(uint,uint,uint,QString)), this, SLOT(onSendError(uint,uint,uint,QString)));
            disconnect(pChannelTypeTextInterface, SIGNAL(Sent(uint,uint,QString)), this, SLOT(onSent(uint,uint,QString)));
        }
        if(pChannelTypeTubesInterface){
            disconnect(pChannelTypeTubesInterface, SIGNAL(DBusNamesChanged(uint,Tp::DBusTubeMemberList,Tp::UIntList)), this, SLOT(onDBusNamesChanged(uint,Tp::DBusTubeMemberList,Tp::UIntList)));
            disconnect(pChannelTypeTubesInterface, SIGNAL(NewTube(uint,uint,uint,QString,QVariantMap,uint)), this, SLOT(onNewTube(uint,uint,uint,QString,QVariantMap,uint)));
            disconnect(pChannelTypeTubesInterface, SIGNAL(StreamTubeNewConnection(uint,uint)), this, SLOT(onStreamTubeNewConnection(uint,uint)));
            disconnect(pChannelTypeTubesInterface, SIGNAL(TubeClosed(uint)), this, SLOT(onTubeClosed(uint)));
            disconnect(pChannelTypeTubesInterface, SIGNAL(TubeStateChanged(uint,uint)), this, SLOT(onTubeStateChanged(uint,uint)));
        }
    }
    //for ReadyObject
    void Channel::onChannelReady(Tp::PendingOperation* operation)
    {
        qDebug() << "Channel::onChannelReady(...)";
        qDebug() << "- this: " << this;
        //check if incomming direction
        if(direction == 1){
            qDebug() << "Current Thread: " << QThread::currentThreadId();
            status = Telephony::Incomming;
            connection->channelStatusChanged(this);
        }
        //check for outgoing
        else if(direction == 2){
            qDebug() << "Current Thread: " << QThread::currentThreadId();
            status = Telephony::Dialing;
            connection->channelStatusChanged(this);
        }
        connectType();
        connectInterfaces();
    }

    //for ChannelInterface signals
    void Channel::onClose()
    {
        qDebug() << "ChannelInterface - Channel::onClose()";
        qDebug() << "- this: " << this;
    }

    //for ChannelInterfaceCallStateInterface signals
    void Channel::onCallStateChanged(uint contact, uint state)
    {
        qDebug() << "ChannelInterfaceCallStateInterface - Channel::onCallStateChanged() " << contact << " : " << state;

        //state: 1=Ringing, 2=Queued, 4=Held, 8=Forwarded, 16=In progress
        //Check if its ringing
        if(state & 0x01){
            //check for outgoing
            if(direction == 2){
                qDebug() << "Current Thread: " << QThread::currentThreadId();
                status = Telephony::Alerting;
                connection->channelStatusChanged(this);
            }
        }
    }

    //for ChannelInterfaceChatStateInterface signals
    void  Channel::onChatStateChanged(uint contact, uint state)
    {
        qDebug() << "ChannelInterfaceChatStateInterface - Channel::onChatStateChanged() " << contact << " : " << state;
    }

    //for ChannelInterfaceDTMFInterface signals
    //- no signals

    //for ChannelInterfaceGroupInterface signals
    void  Channel::onHandleOwnersChanged(const Tp::HandleOwnerMap& added, const Tp::UIntList& removed)
    {
        qDebug() << "ChannelInterfaceGroupInterface - Channel::onHandleOwnersChanged() ";
    }
    void  Channel::onSelfHandleChanged(uint selfHandle)
    {
        qDebug() << "ChannelInterfaceGroupInterface - Channel::onSelfHandleChanged() ";
    }
    void  Channel::onGroupFlagsChanged(uint added, uint removed)
    {
        qDebug() << "ChannelInterfaceGroupInterface - Channel::onGroupFlagsChanged() ";
    }
    void  Channel::onMembersChanged(const QString& message, const Tp::UIntList& added, const Tp::UIntList& removed, const Tp::UIntList& localPending, const Tp::UIntList& remotePending, uint actor, uint reason)
    {
        qDebug() << "ChannelInterfaceGroupInterface - Channel::onMembersChanged() ";
    }
    void  Channel::onMembersChangedDetailed(const Tp::UIntList& added, const Tp::UIntList& removed, const Tp::UIntList& localPending, const Tp::UIntList& remotePending, const QVariantMap& details)
    {
        qDebug() << "ChannelInterfaceGroupInterface - Channel::onMembersChangedDetailed() ";
    }

    //for ChannelInterfaceHoldInterface signals
    void  Channel::onHoldStateChanged(uint holdState, uint reason)
    {
        qDebug() << "ChannelInterfaceHoldInterface - Channel::onHoldStateChanged() ";
    }

    //for ChannelInterfaceMediaSignallingInterface signals
    void  Channel::onNewSessionHandler(const QDBusObjectPath& sessionHandler, const QString& sessionType)
    {
        qDebug() << "ChannelInterfaceMediaSignallingInterface - Channel::onNewSessionHandler() ";
    }

    // for ChannelInterfaceMessagesInterface signals
    void  Channel::onMessageSent(const Tp::MessagePartList& content, uint flags, const QString& messageToken)
    {
        qDebug() << "ChannelInterfaceMessagesInterface - Channel::onMessageSent() ";
    }
    void  Channel::onPendingMessagesRemoved(const Tp::UIntList& messageIDs)
    {
        qDebug() << "ChannelInterfaceMessagesInterface - Channel::onPendingMessagesRemoved() ";
    }
    void  Channel::onMessageReceived(const Tp::MessagePartList& message)
    {
        qDebug() << "ChannelInterfaceMessagesInterface - Channel::onMessageReceived() ";
    }

    //for ChannelInterfacePasswordInterface signals
    void  Channel::onPasswordFlagsChanged(uint added, uint removed)
    {
        qDebug() << "ChannelInterfacePasswordInterface - Channel::onPasswordFlagsChanged() ";
    }

    //for ChannelInterfaceTubeInterface signals
    void  Channel::onTubeChannelStateChanged(uint state)
    {
        qDebug() << "ChannelInterfaceTubeInterface - Channel::onTubeChannelStateChanged() ";
    }

    //for ChannelTypeFileTransferInterface signals
    void Channel::onFileTransferStateChanged(uint state, uint reason)
    {
        qDebug() << "ChannelTypeFileTransferInterface - Channel::onFileTransferStateChanged() ";
    }

    void Channel::onTransferredBytesChanged(qulonglong count)
    {
        qDebug() << "ChannelTypeFileTransferInterface - Channel::onTransferredBytesChanged() ";
    }

    void Channel::onInitialOffsetDefined(qulonglong initialOffset)
    {
        qDebug() << "ChannelTypeFileTransferInterface - Channel::onInitialOffsetDefined() ";
    }

    //for ChannelTypeRoomListInterface signals
    void Channel::onGotRooms(const Tp::RoomInfoList& rooms)
    {
        qDebug() << "ChannelTypeRoomListInterface - Channel::onGotRooms() ";
    }

    void Channel::onListingRooms(bool listing)
    {
        qDebug() << "ChannelTypeRoomListInterface - Channel::onListingRooms() ";
    }

    //for ChannelTypeStreamTubeInterface signals
    void Channel::onNewRemoteConnection(uint handle, const QDBusVariant& connectionParam, uint connectionID)
    {
        qDebug() << "ChannelTypeStreamTubeInterface - Channel::onNewRemoteConnection() ";
    }

    void Channel::onNewLocalConnection(uint connectionID)
    {
        qDebug() << "ChannelTypeStreamTubeInterface - Channel::onNewLocalConnection() ";
    }

    void Channel::onConnectionClosed(uint connectionID, const QString& error, const QString& message)
    {
        qDebug() << "ChannelTypeStreamTubeInterface - Channel::onConnectionClosed() ";
    }

    //for ChannelTypeStreamedMediaInterface signals
    void Channel::onStreamAdded(uint streamID, uint contactHandle, uint streamType)
    {
        qDebug() << "ChannelTypeStreamedMediaInterface - Channel::onStreamAdded()";
        qDebug() << " - streamID: " << streamID;
        qDebug() << " - contactHandle: " << contactHandle;
        qDebug() << " - streamType: " << streamType;
    }

    void Channel::onStreamDirectionChanged(uint streamID, uint streamDirection, uint pendingFlags)
    {
        qDebug() << "ChannelTypeStreamedMediaInterface - Channel::onStreamDirectionChanged()";
        qDebug() << " - streamID: " << streamID;
        qDebug() << " - streamDirection: " << streamDirection;
        qDebug() << " - pendingFlags: " << pendingFlags;

        //streamDirection: 0=None, 1=Send, 2=Receive, 3=Bidirectional
        if(streamDirection == 3){
            //check for outgoing
            if(direction == 2 || direction == 1){
                qDebug() << "Current Thread: " << QThread::currentThreadId();
                status = Telephony::Connected;
                connection->channelStatusChanged(this);
            }
        }
    }

    void Channel::onStreamError(uint streamID, uint errorCode, const QString& message)
    {
        qDebug() << "ChannelTypeStreamedMediaInterface - Channel::onStreamError()";
    }

    void Channel::onStreamRemoved(uint streamID)
    {
        qDebug() << "ChannelTypeStreamedMediaInterface - Channel::onStreamRemoved() " << streamID;
        qDebug() << " - streamID: " << streamID;
        if(direction == 1 || direction == 2)
        {
            qDebug() << "Current hread: " << QThread::currentThreadId();
            status = Telephony::Disconnecting;
            connection->channelStatusChanged(this);
        }
    }

    void Channel::onStreamStateChanged(uint streamID, uint streamState)
    {
        qDebug() << "ChannelTypeStreamedMediaInterface - Channel::onStreamStateChanged()";
        qDebug() << " - streamID: " << streamID;
        qDebug() << " - streamState: " << streamState;
        qDebug() << "- this: " << this;
    }

    //for ChannelTypeTextInterface signals
    void Channel::onLostMessage()
    {
        qDebug() << "ChannelTypeTextInterface - Channel::onLostMessage()";
    }

    void Channel::onReceived(uint ID, uint timestamp, uint sender, uint type, uint flags, const QString& text)
    {
        qDebug() << "ChannelTypeTextInterface - Channel::onReceived()";
    }

    void Channel::onSendError(uint error, uint timestamp, uint type, const QString& text)
    {
        qDebug() << "ChannelTypeTextInterface - Channel::onSendError()";
    }

    void Channel::onSent(uint timestamp, uint type, const QString& text)
    {
        qDebug() << "ChannelTypeTextInterface - Channel::onSent()";
    }

    //for ChannelTypeTubesInterface signals
    void Channel::onNewTube(uint ID, uint initiator, uint type, const QString& service, const QVariantMap& parameters, uint state)
    {
        qDebug() << "ChannelTypeTubesInterface - Channel::onNewTube()";
    }

    void Channel::onTubeStateChanged(uint ID, uint state)
    {
        qDebug() << "ChannelTypeTubesInterface - Channel::onTubeStateChanged()";
    }

    void Channel::onTubeClosed(uint ID)
    {
        qDebug() << "ChannelTypeTubesInterface - Channel::onTubeClosed()";
    }

    void Channel::onDBusNamesChanged(uint ID, const Tp::DBusTubeMemberList& added, const Tp::UIntList& removed)
    {
        qDebug() << "ChannelTypeTubesInterface - Channel::onDBusNamesChanged()";
    }

    void Channel::onStreamTubeNewConnection(uint ID, uint handle)
    {
        qDebug() << "ChannelTypeTubesInterface - Channel::onStreamTubeNewConnection()";
    }
}
