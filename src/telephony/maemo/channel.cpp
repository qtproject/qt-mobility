/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QDebug>

#include "qtelephony.h"
#include "maemo/channel.h"
#include "maemo/constants.h"
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/pendingoperation.h"

namespace DBus
{
    using namespace Interfaces;

    const Feature Channel::FeatureCore = Feature(QLatin1String(Channel::staticMetaObject.className()), 0, true);

    Channel::Channel(QDBusConnection busconnection, const QString busname, const QString &objectPath, const QVariantMap &properties, Connection* conn)
        : StatefullDBusProxy(busconnection, busname, objectPath)
        , ReadyObject(this, FeatureCore)
    {
        init();
        propertylist = properties;
        connection = conn;

        qDebug() << " Channel::Channel(...)";

        //Create Channel interface
        pIChannel = new DBus::Interfaces::IChannel(this->dbusConnection(),this->busName(), this->objectPath());

        //Set flag to indicate if values must be read
        if(propertylist.count() <= 0){
            createPropertiyList();
            wasExistingChannel = true;
        }

        //check if is a call
        bool iscall = false;
        if(propertylist.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"))){
            QString type = propertylist.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();
            iscall = Channel::isCall(type);
        }

        //get direction by chjecking the requested flag
        //if Requested then this channel was created in response to a local request => outgoing
        if(propertylist.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Requested"))){
            bool requested = propertylist.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Requested")).toBool();
            if(requested)
                direction = 2;
            else
                direction = 1;
        }

        //if its not a call we don't need to go further
        if(iscall){
            //set remote id
            remoteIdentifier = pIChannel->TargetID();
            connect(pIChannel, SIGNAL(Closed()), SLOT(onClose()));
            connectType();
            connectInterfaces();
            connect((QObject*)this->becomeReady(), SIGNAL(finished(DBus::PendingOperation *)), SLOT(onChannelReady(DBus::PendingOperation*)));
        }
    }

    bool Channel::isCall(QString channeltype)
    {
        /**************************
        check for Mediastream
        **************************/
        if(channeltype == TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA
           || channeltype == TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT){
            return true;
        }
        return false;
    }

    void Channel::init()
    {
        pIChannel = 0;
        pIChannelCallState = 0;
        pIChannelChatState = 0;
        pIChannelDTMF = 0;
        pIChannelGroup = 0;
        pIChannelHold = 0;
        pIChannelMediaSignalling = 0;
        pIChannelMessages = 0;
        pIChannelPassword = 0;
        pIChannelTube = 0;
        pIChannelTypeContactList = 0;
        pIChannelTypeFileTransfer = 0;
        pIChannelTypeRoomList = 0;
        pIChannelTypeStreamTube = 0;
        pIChannelTypeStreamedMedia = 0;
        pIChannelTypeText = 0;
        pIChannelTypeTubes = 0;
        status = QTelephony::Idle;
        direction = 0;
        wasExistingChannel = false;
        remoteIdentifier = "";
    }

    Channel::~Channel()
    {
        qDebug() << "Channel::~Channel";
        disconnectType();
        disconnectInterfaces();
        if(pIChannelCallState)
            delete pIChannelCallState;
        if(pIChannelChatState)
            delete pIChannelChatState;
        if(pIChannelDTMF)
            delete pIChannelDTMF;
        if(pIChannelGroup)
            delete pIChannelGroup;
        if(pIChannelHold)
            delete pIChannelHold;
        if(pIChannelMediaSignalling)
            delete pIChannelMediaSignalling;
        if(pIChannelMessages)
            delete pIChannelMessages;
        if(pIChannelPassword)
            delete pIChannelPassword;
        if(pIChannelTube)
            delete pIChannelTube;
        if(pIChannelTypeContactList)
            delete pIChannelTypeContactList;
        if(pIChannelTypeFileTransfer)
            delete pIChannelTypeFileTransfer;
        if(pIChannelTypeRoomList)
            delete pIChannelTypeRoomList;
        if(pIChannelTypeStreamTube)
            delete pIChannelTypeStreamTube;
        if(pIChannelTypeStreamedMedia)
            delete pIChannelTypeStreamedMedia;
        if(pIChannelTypeText)
            delete pIChannelTypeText;
        if(pIChannelTypeTubes)
            delete pIChannelTypeTubes;
        if(pIChannel)
            delete pIChannel;
    }

    void Channel::createPropertiyList()
    {
        //read the channel interfaces
        QStringList interfaces = pIChannel->Interfaces();
        propertylist.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Interfaces"), QVariant(interfaces));

        QString channeltype = pIChannel->ChannelType();
        propertylist.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"), QVariant(channeltype));

        bool requested = pIChannel->Requested();
        propertylist.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Requested"), QVariant(requested));
    }

    void Channel::connectInterfaces()
    {
        if(propertylist.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Interfaces"))){
            QStringList varlist = propertylist.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Interfaces")).toStringList();
            foreach(const QString& supportedinterface, varlist)
            {
                if(supportedinterface == DBus::Interfaces::IChannelCallState::staticInterfaceName()){
                    pIChannelCallState = new DBus::Interfaces::IChannelCallState(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelChatState::staticInterfaceName()){
                    pIChannelChatState = new DBus::Interfaces::IChannelChatState(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelDTMF::staticInterfaceName()){
                    pIChannelDTMF = new DBus::Interfaces::IChannelDTMF(this->dbusConnection(), this->busName(), this->objectPath());
                    //no signals
                }
                else if(supportedinterface == DBus::Interfaces::IChannelGroup::staticInterfaceName()){
                    pIChannelGroup = new DBus::Interfaces::IChannelGroup(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelHold::staticInterfaceName()){
                    pIChannelHold = new DBus::Interfaces::IChannelHold(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelMediaSignalling::staticInterfaceName()){
                    pIChannelMediaSignalling = new DBus::Interfaces::IChannelMediaSignalling(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelMessages::staticInterfaceName()){
                    pIChannelMessages = new DBus::Interfaces::IChannelMessages(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelPassword::staticInterfaceName()){
                    pIChannelPassword = new DBus::Interfaces::IChannelPassword(this->dbusConnection(), this->busName(), this->objectPath());
                }
                else if(supportedinterface == DBus::Interfaces::IChannelTube::staticInterfaceName()){
                    pIChannelTube = new DBus::Interfaces::IChannelTube(this->dbusConnection(), this->busName(), this->objectPath());
                }
            }

            if(pIChannelCallState){
                connect(pIChannelCallState, SIGNAL(CallStateChanged(uint,uint)), SLOT(onCallStateChanged(uint,uint)));
            }
            if(pIChannelChatState){
                connect(pIChannelChatState, SIGNAL(ChatStateChanged(uint,uint)), SLOT(onChatStateChanged(uint,uint)));
            }
            if(pIChannelDTMF){
                pIChannelDTMF = new DBus::Interfaces::IChannelDTMF(this->dbusConnection(), this->busName(), this->objectPath());
                //no signals
            }
            if(pIChannelGroup){
                connect(pIChannelGroup, SIGNAL(HandleOwnersChanged(DBus::Interfaces::HandleOwnerMap,DBus::Interfaces::UIntList)), SLOT(onHandleOwnersChanged(DBus::Interfaces::HandleOwnerMap,DBus::Interfaces::UIntList)));
                connect(pIChannelGroup, SIGNAL(SelfHandleChanged(uint)), SLOT(onSelfHandleChanged(uint)));
                connect(pIChannelGroup, SIGNAL(GroupFlagsChanged(uint,uint)), SLOT(onGroupFlagsChanged(uint,uint)));
                connect(pIChannelGroup, SIGNAL(MembersChanged(QString,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,uint,uint)), SLOT(onMembersChanged(QString,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,uint,uint)));
                connect(pIChannelGroup, SIGNAL(MembersChangedDetailed(DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,QVariantMap)), SLOT(onMembersChangedDetailed(DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,QVariantMap)));
            }
            if(pIChannelHold){
                connect(pIChannelHold, SIGNAL(HoldStateChanged(uint,uint)), SLOT(onHoldStateChanged(uint,uint)));
            }
            if(pIChannelMediaSignalling){
                connect(pIChannelMediaSignalling, SIGNAL(NewSessionHandler(QDBusObjectPath,QString)), SLOT(onNewSessionHandler(QDBusObjectPath,QString)));
            }
            if(pIChannelMessages){
                connect(pIChannelMessages, SIGNAL(MessageReceived(DBus::Interfaces::MessagePartList)), SLOT(onMessageReceived(DBus::Interfaces::MessagePartList)));
                connect(pIChannelMessages, SIGNAL(MessageSent(DBus::Interfaces::MessagePartList,uint,QString)), SLOT(onMessageSent(DBus::Interfaces::MessagePartList,uint,QString)));
                connect(pIChannelMessages, SIGNAL(PendingMessagesRemoved(DBus::Interfaces::UIntList)), SLOT(onPendingMessagesRemoved(DBus::Interfaces::UIntList)));
            }
            if(pIChannelPassword){
                connect(pIChannelPassword, SIGNAL(PasswordFlagsChanged(uint,uint)), SLOT(onPasswordFlagsChanged(uint,uint)));
            }
            if(pIChannelTube){
                connect(pIChannelTube, SIGNAL(TubeChannelStateChanged(uint)), SLOT(onTubeChannelStateChanged(uint)));
            }
        }
    }

    void Channel::disconnectInterfaces()
    {
        if(pIChannelCallState){
            disconnect(pIChannelCallState, SIGNAL(CallStateChanged(uint,uint)), this, SLOT(onCallStateChanged(uint,uint)));
        }
        if(pIChannelDTMF){
            //no signals
        }
        if(pIChannelGroup){
            disconnect(pIChannelGroup, SIGNAL(HandleOwnersChanged(DBus::Interfaces::HandleOwnerMap,DBus::Interfaces::UIntList)), this, SLOT(onHandleOwnersChanged(DBus::Interfaces::HandleOwnerMap,DBus::Interfaces::UIntList)));
            disconnect(pIChannelGroup, SIGNAL(SelfHandleChanged(uint)), this, SLOT(onSelfHandleChanged(uint)));
            disconnect(pIChannelGroup, SIGNAL(GroupFlagsChanged(uint,uint)), this, SLOT(onGroupFlagsChanged(uint,uint)));
            disconnect(pIChannelGroup, SIGNAL(MembersChanged(QString,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,uint,uint)), this, SLOT(onMembersChanged(QString,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,uint,uint)));
            disconnect(pIChannelGroup, SIGNAL(MembersChangedDetailed(DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,QVariantMap)), this, SLOT(onMembersChangedDetailed(DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,DBus::Interfaces::UIntList,QVariantMap)));
        }
        if(pIChannelHold){
            disconnect(pIChannelHold, SIGNAL(HoldStateChanged(uint,uint)), this, SLOT(onHoldStateChanged(uint,uint)));
        }
        if(pIChannelMediaSignalling){
            disconnect(pIChannelMediaSignalling, SIGNAL(NewSessionHandler(QDBusObjectPath,QString)), this, SLOT(onNewSessionHandler(QDBusObjectPath,QString)));
        }
        if(pIChannelMessages){
            disconnect(pIChannelMessages, SIGNAL(MessageReceived(DBus::Interfaces::MessagePartList)), this, SLOT(onMessageReceived(DBus::Interfaces::MessagePartList)));
            disconnect(pIChannelMessages, SIGNAL(MessageSent(DBus::Interfaces::MessagePartList,uint,QString)), this, SLOT(onMessageSent(DBus::Interfaces::MessagePartList,uint,QString)));
            disconnect(pIChannelMessages, SIGNAL(PendingMessagesRemoved(DBus::Interfaces::UIntList)), this, SLOT(onPendingMessagesRemoved(DBus::Interfaces::UIntList)));
        }
        if(pIChannelPassword){
            disconnect(pIChannelPassword, SIGNAL(PasswordFlagsChanged(uint,uint)), this, SLOT(onPasswordFlagsChanged(uint,uint)));
        }
        if(pIChannelTube){
            disconnect(pIChannelTube, SIGNAL(TubeChannelStateChanged(uint)), this, SLOT(onTubeChannelStateChanged(uint)));
        }
    }

    void Channel::connectType()
    {
        if( propertylist.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"))){
            subtype = propertylist.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();
            if(subtype == DBus::Interfaces::IChannelTypeContactList::staticInterfaceName()){
                pIChannelTypeContactList = new DBus::Interfaces::IChannelTypeContactList(this->dbusConnection(), this->busName(), this->objectPath());
                // no signals
            }
            else if(subtype == DBus::Interfaces::IChannelTypeFileTransfer::staticInterfaceName()){
                pIChannelTypeFileTransfer = new DBus::Interfaces::IChannelTypeFileTransfer(this->dbusConnection(), this->busName(), this->objectPath());
            }
            else if(subtype == DBus::Interfaces::IChannelTypeRoomList::staticInterfaceName()){
                pIChannelTypeRoomList = new DBus::Interfaces::IChannelTypeRoomList(this->dbusConnection(), this->busName(), this->objectPath());
            }
            else if(subtype == DBus::Interfaces::IChannelTypeStreamTube::staticInterfaceName()){
                pIChannelTypeStreamTube = new DBus::Interfaces::IChannelTypeStreamTube(this->dbusConnection(), this->busName(), this->objectPath());
            }
            else if(subtype == DBus::Interfaces::IChannelTypeStreamedMedia::staticInterfaceName()){
                //read the type of the call before the signals get connected
                pIChannelTypeStreamedMedia = new DBus::Interfaces::IChannelTypeStreamedMedia(this->dbusConnection(), this->busName(), this->objectPath());
            }
            else if(subtype == DBus::Interfaces::IChannelTypeText::staticInterfaceName()){
                pIChannelTypeText = new DBus::Interfaces::IChannelTypeText(this->dbusConnection(), this->busName(), this->objectPath());
            }
            else if(subtype == DBus::Interfaces::IChannelTypeTubes::staticInterfaceName()){
                pIChannelTypeTubes = new DBus::Interfaces::IChannelTypeTubes(this->dbusConnection(), this->busName(), this->objectPath());
            }

            //Connect signals
            //if(pIChannelTypeContactList){
            //   no signals
            //}
            if(pIChannelTypeFileTransfer){
                connect(pIChannelTypeFileTransfer, SIGNAL(FileTransferStateChanged(uint,uint)), SLOT(onFileTransferStateChanged(uint,uint)));
                connect(pIChannelTypeFileTransfer, SIGNAL(TransferredBytesChanged(qulonglong)), SLOT(onTransferredBytesChanged(qulonglong)));
                connect(pIChannelTypeFileTransfer, SIGNAL(InitialOffsetDefined(qulonglong)), SLOT(onInitialOffsetDefined(qulonglong)));
            }
            if(pIChannelTypeRoomList){
                connect(pIChannelTypeRoomList, SIGNAL(GotRooms(DBus::Interfaces::RoomInfoList)), SLOT(onGotRooms(DBus::Interfaces::RoomInfoList)));
                connect(pIChannelTypeRoomList, SIGNAL(ListingRooms(bool)), SLOT(onListingRooms(bool)));
            }
            if(pIChannelTypeStreamTube){
                connect(pIChannelTypeStreamTube, SIGNAL(NewRemoteConnection(uint,QDBusVariant,uint)), SLOT(onNewRemoteConnection(uint,QDBusVariant,uint)));
                connect(pIChannelTypeStreamTube, SIGNAL(NewLocalConnection(uint)), SLOT(onNewLocalConnection(uint)));
                connect(pIChannelTypeStreamTube, SIGNAL(ConnectionClosed(uint,QString,QString)), SLOT(onConnectionClosed(uint,QString,QString)));
            }
            if(pIChannelTypeStreamedMedia){
                connect(pIChannelTypeStreamedMedia, SIGNAL(StreamAdded(uint,uint,uint)), SLOT(onStreamAdded(uint,uint,uint)));
                connect(pIChannelTypeStreamedMedia, SIGNAL(StreamDirectionChanged(uint,uint,uint)), SLOT(onStreamDirectionChanged(uint,uint,uint)));
                connect(pIChannelTypeStreamedMedia, SIGNAL(StreamError(uint,uint,QString)), SLOT(onStreamError(uint,uint,QString)));
                connect(pIChannelTypeStreamedMedia, SIGNAL(StreamRemoved(uint)), SLOT(onStreamRemoved(uint)));
                connect(pIChannelTypeStreamedMedia, SIGNAL(StreamStateChanged(uint,uint)), SLOT(onStreamStateChanged(uint,uint)));
            }
            if(pIChannelTypeText){
                connect(pIChannelTypeText, SIGNAL(LostMessage()), SLOT(onLostMessage()));
                connect(pIChannelTypeText,SIGNAL(Received(uint,uint,uint,uint,uint,QString)), SLOT(onReceived(uint,uint,uint,uint,uint,QString)));
                connect(pIChannelTypeText, SIGNAL(SendError(uint,uint,uint,QString)), SLOT(onSendError(uint,uint,uint,QString)));
                connect(pIChannelTypeText, SIGNAL(Sent(uint,uint,QString)), SLOT(onSent(uint,uint,QString)));
            }
            if(pIChannelTypeTubes){
                connect(pIChannelTypeTubes, SIGNAL(DBusNamesChanged(uint,DBus::Interfaces::DBusTubeMemberList,DBus::Interfaces::UIntList)), SLOT(onDBusNamesChanged(uint,DBus::Interfaces::DBusTubeMemberList,DBus::Interfaces::UIntList)));
                connect(pIChannelTypeTubes, SIGNAL(NewTube(uint,uint,uint,QString,QVariantMap,uint)), SLOT(onNewTube(uint,uint,uint,QString,QVariantMap,uint)));
                connect(pIChannelTypeTubes, SIGNAL(StreamTubeNewConnection(uint,uint)), SLOT(onStreamTubeNewConnection(uint,uint)));
                connect(pIChannelTypeTubes, SIGNAL(TubeClosed(uint)), SLOT(onTubeClosed(uint)));
                connect(pIChannelTypeTubes, SIGNAL(TubeStateChanged(uint,uint)), SLOT(onTubeStateChanged(uint,uint)));
            }
        }
    }

    void Channel::disconnectType()
    {
        if(pIChannelTypeContactList){
            // no signals
        }
        if(pIChannelTypeFileTransfer){
            disconnect(pIChannelTypeFileTransfer, SIGNAL(FileTransferStateChanged(uint,uint)), this, SLOT(onFileTransferStateChanged(uint,uint)));
            disconnect(pIChannelTypeFileTransfer, SIGNAL(TransferredBytesChanged(qulonglong)), this, SLOT(onTransferredBytesChanged(qulonglong)));
            disconnect(pIChannelTypeFileTransfer, SIGNAL(InitialOffsetDefined(qulonglong)), this, SLOT(onInitialOffsetDefined(qulonglong)));
        }
        if(pIChannelTypeRoomList){
            disconnect(pIChannelTypeRoomList, SIGNAL(GotRooms(DBus::Interfaces::RoomInfoList)), this, SLOT(onGotRooms(DBus::Interfaces::RoomInfoList)));
            disconnect(pIChannelTypeRoomList, SIGNAL(ListingRooms(bool)), this, SLOT(onListingRooms(bool)));
        }
        if(pIChannelTypeStreamTube){
            disconnect(pIChannelTypeStreamTube, SIGNAL(NewRemoteConnection(uint,QDBusVariant,uint)), this, SLOT(onNewRemoteConnection(uint,QDBusVariant,uint)));
            disconnect(pIChannelTypeStreamTube, SIGNAL(NewLocalConnection(uint)), this, SLOT(onNewLocalConnection(uint)));
            disconnect(pIChannelTypeStreamTube, SIGNAL(ConnectionClosed(uint,QString,QString)), this, SLOT(onConnectionClosed(uint,QString,QString)));
        }
        if(pIChannelTypeStreamedMedia){
            disconnect(pIChannelTypeStreamedMedia, SIGNAL(StreamAdded(uint,uint,uint)), this, SLOT(onStreamAdded(uint,uint,uint)));
            disconnect(pIChannelTypeStreamedMedia, SIGNAL(StreamDirectionChanged(uint,uint,uint)), this, SLOT(onStreamDirectionChanged(uint,uint,uint)));
            disconnect(pIChannelTypeStreamedMedia, SIGNAL(StreamError(uint,uint,QString)), this, SLOT(onStreamError(uint,uint,QString)));
            disconnect(pIChannelTypeStreamedMedia, SIGNAL(StreamRemoved(uint)), this, SLOT(onStreamRemoved(uint)));
            disconnect(pIChannelTypeStreamedMedia, SIGNAL(StreamStateChanged(uint,uint)), this, SLOT(onStreamStateChanged(uint,uint)));
        }
        if(pIChannelTypeText){
            disconnect(pIChannelTypeText, SIGNAL(LostMessage()), this, SLOT(onLostMessage()));
            disconnect(pIChannelTypeText,SIGNAL(Received(uint,uint,uint,uint,uint,QString)), this, SLOT(onReceived(uint,uint,uint,uint,uint,QString)));
            disconnect(pIChannelTypeText, SIGNAL(SendError(uint,uint,uint,QString)), this, SLOT(onSendError(uint,uint,uint,QString)));
            disconnect(pIChannelTypeText, SIGNAL(Sent(uint,uint,QString)), this, SLOT(onSent(uint,uint,QString)));
        }
        if(pIChannelTypeTubes){
            disconnect(pIChannelTypeTubes, SIGNAL(DBusNamesChanged(uint,DBus::Interfaces::DBusTubeMemberList,DBus::Interfaces::UIntList)), this, SLOT(onDBusNamesChanged(uint,DBus::Interfaces::DBusTubeMemberList,DBus::Interfaces::UIntList)));
            disconnect(pIChannelTypeTubes, SIGNAL(NewTube(uint,uint,uint,QString,QVariantMap,uint)), this, SLOT(onNewTube(uint,uint,uint,QString,QVariantMap,uint)));
            disconnect(pIChannelTypeTubes, SIGNAL(StreamTubeNewConnection(uint,uint)), this, SLOT(onStreamTubeNewConnection(uint,uint)));
            disconnect(pIChannelTypeTubes, SIGNAL(TubeClosed(uint)), this, SLOT(onTubeClosed(uint)));
            disconnect(pIChannelTypeTubes, SIGNAL(TubeStateChanged(uint,uint)), this, SLOT(onTubeStateChanged(uint,uint)));
        }
    }

    unsigned int Channel::getCallType()
    {
        unsigned int ret = 0;
        if(pIChannelTypeStreamedMedia){
            QDBusPendingReply<DBus::Interfaces::MediaStreamInfoList> lst = pIChannelTypeStreamedMedia->ListStreams();
            lst.waitForFinished();
            //Type 0=Audio, 1=Video
            foreach(const DBus::Interfaces::MediaStreamInfo& info, lst.value()){
                if(info.type == 0 && ret == QTelephony::Other)
                    ret |= QTelephony::Voice;
                if(info.type == 1)
                    ret |= QTelephony::Video;

            }
        }
        else if(pIChannelTypeText)
            ret |= QTelephony::Text;

        return ret;
    }

    //for ReadyObject
    void Channel::onChannelReady(DBus::PendingOperation* operation)
    {
        qDebug() << "Channel::onChannelReady(...)";
        QTelephony::CallStatus newstatus = status;

        //check if incoming direction
        if(isIncoming())
            newstatus = QTelephony::Incoming;
        //check for outgoing, don't make status changed in sms calls otherwise this status change would be to early
        else if(isOutgoing() && !isText())
            newstatus = QTelephony::Dialing;

        if(wasExistingChannel){
            //check for call state if its ringing (call state 0bit = 1)
            if(direction == 2){
                if(pIChannelCallState){
                    QDBusPendingReply<DBus::Interfaces::ChannelCallStateMap> ccsm = pIChannelCallState->GetCallStates();
                    ccsm.waitForFinished();
                    if(ccsm.isValid()){
                        DBus::Interfaces::ChannelCallStateMap map = ccsm.value();
                        QMapIterator<uint, uint> i(map);

                         while (i.hasNext()) {
                             i.next();
                             if(i.value() & 0x01){
                                 newstatus = QTelephony::Alerting;
                                 break;
                             }
                        }
                    }
                }
            }
            //Check if status is already connected => call state 0bit = ringing, Stream Directtion = 3 (Bidirectional)
            //This is valid for both incoming and outgoing calls
            if(pIChannelTypeStreamedMedia){
                QDBusPendingReply<DBus::Interfaces::MediaStreamInfoList> streams = pIChannelTypeStreamedMedia->ListStreams();
                streams.waitForFinished();
                if(streams.isValid()){
                    foreach(const MediaStreamInfo& msi, streams.value())
                    {
                        if(msi.direction == 3){
                            newstatus = QTelephony::Connected;
                            break;
                        }
                    }
                }
            }
            //Check if status is OnHold
            if(this->pIChannelHold)
            {
                QDBusPendingReply<uint, uint> holdingstate = pIChannelHold->GetHoldState();
                holdingstate.waitForFinished();
                if(holdingstate.isValid()){
                    uint holdstate = holdingstate.argumentAt<0>();
                    if(holdstate == 1)
                        newstatus = QTelephony::OnHold;
                }
            }
        }

        //Only emmit this status change if not sms call
        if(status != newstatus){
            status = newstatus;
            connection->channelStatusChanged(this);
        }
    }

    //for IChannel signals
    void Channel::onClose()
    {
        qDebug() << "IChannel - Channel::onClose()";
        if(isIncoming() || isOutgoing())
        {
            if(status != QTelephony::Disconnecting){
                status = QTelephony::Disconnecting;
                connection->channelStatusChanged(this);
            }
        }
    }

    //for IChannelCallState signals
    void Channel::onCallStateChanged(uint contact, uint state)
    {
        qDebug() << "IChannelCallState - Channel::onCallStateChanged() " << contact << " : " << state;

        //state: 1=Ringing, 2=Queued, 4=Held, 8=Forwarded, 16=In progress
        //Check if its ringing
        if(state & 0x01){
            //check for outgoing
            if(isOutgoing() && status != QTelephony::Alerting){
                status = QTelephony::Alerting;
                connection->channelStatusChanged(this);
            }
        }
    }

    //for IChannelChatState signals
    void  Channel::onChatStateChanged(uint contact, uint state)
    {
        qDebug() << "IChannelChatState - Channel::onChatStateChanged() " << contact << " : " << state;
    }

    //for IChannelDTMF signals
    //- no signals

    //for IChannelGroup signals
    void  Channel::onHandleOwnersChanged(const DBus::Interfaces::HandleOwnerMap& added, const DBus::Interfaces::UIntList& removed)
    {
        qDebug() << "IChannelGroup - Channel::onHandleOwnersChanged() ";
    }
    void  Channel::onSelfHandleChanged(uint selfHandle)
    {
        qDebug() << "IChannelGroup - Channel::onSelfHandleChanged() ";
    }
    void  Channel::onGroupFlagsChanged(uint added, uint removed)
    {
        qDebug() << "IChannelGroup - Channel::onGroupFlagsChanged() ";
    }
    void  Channel::onMembersChanged(const QString& message, const DBus::Interfaces::UIntList& added, const DBus::Interfaces::UIntList& removed, const DBus::Interfaces::UIntList& localPending, const DBus::Interfaces::UIntList& remotePending, uint actor, uint reason)
    {
        qDebug() << "IChannelGroup - Channel::onMembersChanged() ";
    }
    void  Channel::onMembersChangedDetailed(const DBus::Interfaces::UIntList& added, const DBus::Interfaces::UIntList& removed, const DBus::Interfaces::UIntList& localPending, const DBus::Interfaces::UIntList& remotePending, const QVariantMap& details)
    {
        qDebug() << "IChannelGroup - Channel::onMembersChangedDetailed() ";
    }

    //for IChannelHold signals
    void  Channel::onHoldStateChanged(uint holdState, uint reason)
    {
        qDebug() << "IChannelHold - Channel::onHoldStateChanged() ";
        if(holdState == 1){ //Held
            if(status != QTelephony::OnHold){
                status = QTelephony::OnHold;
                connection->channelStatusChanged(this);
            }
        }
        else if(holdState == 0){ //Unheld
            if(status != QTelephony::Connected){
                status = QTelephony::Connected;
                connection->channelStatusChanged(this);
            }
        }
    }

    //for IChannelMediaSignalling signals
    void  Channel::onNewSessionHandler(const QDBusObjectPath& sessionHandler, const QString& sessionType)
    {
        qDebug() << "IChannelMediaSignalling - Channel::onNewSessionHandler() ";
    }

    // for IChannelMessages signals
    void  Channel::onMessageSent(const DBus::Interfaces::MessagePartList& content, uint flags, const QString& messageToken)
    {
        qDebug() << "IChannelMessages - Channel::onMessageSent() ";
    }
    void  Channel::onPendingMessagesRemoved(const DBus::Interfaces::UIntList& messageIDs)
    {
        qDebug() << "IChannelMessages - Channel::onPendingMessagesRemoved() ";
    }
    void  Channel::onMessageReceived(const DBus::Interfaces::MessagePartList& message)
    {
        qDebug() << "IChannelMessages - Channel::onMessageReceived() ";
    }

    //for IChannelPassword signals
    void  Channel::onPasswordFlagsChanged(uint added, uint removed)
    {
        qDebug() << "IChannelPassword - Channel::onPasswordFlagsChanged() ";
    }

    //for IChannelTube signals
    void  Channel::onTubeChannelStateChanged(uint state)
    {
        qDebug() << "IChannelTube - Channel::onTubeChannelStateChanged() ";
    }

    //for IChannelTypeFileTransfer signals
    void Channel::onFileTransferStateChanged(uint state, uint reason)
    {
        qDebug() << "IChannelTypeFileTransfer - Channel::onFileTransferStateChanged() ";
    }

    void Channel::onTransferredBytesChanged(qulonglong count)
    {
        qDebug() << "IChannelTypeFileTransfer - Channel::onTransferredBytesChanged() ";
    }

    void Channel::onInitialOffsetDefined(qulonglong initialOffset)
    {
        qDebug() << "IChannelTypeFileTransfer - Channel::onInitialOffsetDefined() ";
    }

    //for IChannelTypeRoomList signals
    void Channel::onGotRooms(const DBus::Interfaces::RoomInfoList& rooms)
    {
        qDebug() << "IChannelTypeRoomList - Channel::onGotRooms() ";
    }

    void Channel::onListingRooms(bool listing)
    {
        qDebug() << "IChannelTypeRoomList - Channel::onListingRooms() ";
    }

    //for IChannelTypeStreamTube signals
    void Channel::onNewRemoteConnection(uint handle, const QDBusVariant& connectionParam, uint connectionID)
    {
        qDebug() << "IChannelTypeStreamTube - Channel::onNewRemoteConnection() ";
    }

    void Channel::onNewLocalConnection(uint connectionID)
    {
        qDebug() << "IChannelTypeStreamTube - Channel::onNewLocalConnection() ";
    }

    void Channel::onConnectionClosed(uint connectionID, const QString& error, const QString& message)
    {
        qDebug() << "IChannelTypeStreamTube - Channel::onConnectionClosed() ";
    }

    //for IChannelTypeStreamedMedia signals
    void Channel::onStreamAdded(uint streamID, uint contactHandle, uint streamType)
    {
        qDebug() << "IChannelTypeStreamedMedia - Channel::onStreamAdded()";
        qDebug() << " - streamID: " << streamID;
        qDebug() << " - contactHandle: " << contactHandle;
        qDebug() << " - streamType: " << streamType;
    }

    void Channel::onStreamDirectionChanged(uint streamID, uint streamDirection, uint pendingFlags)
    {
        qDebug() << "IChannelTypeStreamedMedia - Channel::onStreamDirectionChanged()";
        qDebug() << " - streamID: " << streamID;
        qDebug() << " - streamDirection: " << streamDirection;
        qDebug() << " - pendingFlags: " << pendingFlags;

        //streamDirection: 0=None, 1=Send, 2=Receive, 3=Bidirectional
        if(streamDirection == 3){
            //check for outgoing
            if(isIncoming() || isOutgoing()){
                if(status != QTelephony::Connected){
                    status = QTelephony::Connected;
                    connection->channelStatusChanged(this);
                }
            }
        }
    }

    void Channel::onStreamError(uint streamID, uint errorCode, const QString& message)
    {
        qDebug() << "IChannelTypeStreamedMedia - Channel::onStreamError()";
    }

    void Channel::onStreamRemoved(uint streamID)
    {
        qDebug() << "IChannelTypeStreamedMedia - Channel::onStreamRemoved() " << streamID;
    }

    void Channel::onStreamStateChanged(uint streamID, uint streamState)
    {
        qDebug() << "IChannelTypeStreamedMedia - Channel::onStreamStateChanged()";
        qDebug() << " - streamID: " << streamID;
        qDebug() << " - streamState: " << streamState;
    }

    //for IChannelTypeText signals
    void Channel::onLostMessage()
    {
        qDebug() << "IChannelTypeText - Channel::onLostMessage()";
    }

    void Channel::onReceived(uint ID, uint timestamp, uint sender, uint type, uint flags, const QString& text)
    {
        qDebug() << "IChannelTypeText - Channel::onReceived()";
        if(isIncoming() || isOutgoing()){
            if(status != QTelephony::Connected){
                status = QTelephony::Connected;
                connection->channelStatusChanged(this);
            }
        }
    }

    void Channel::onSendError(uint error, uint timestamp, uint type, const QString& text)
    {
        qDebug() << "IChannelTypeText - Channel::onSendError()";
    }

    void Channel::onSent(uint timestamp, uint type, const QString& text)
    {
        qDebug() << "IChannelTypeText - Channel::onSent()";

        //check if incoming direction
        if(isIncoming()){
            if(status != QTelephony::Connected){
                status = QTelephony::Connected;
                connection->channelStatusChanged(this);
            }
        }
        //check for outgoing, don't make status changed in sms calls otherwise this status change would be to early
        else if(isOutgoing()){
            if(status != QTelephony::Alerting){
                status = QTelephony::Alerting;
                connection->channelStatusChanged(this);
            }
        }
    }

    //for IChannelTypeTubes signals
    void Channel::onNewTube(uint ID, uint initiator, uint type, const QString& service, const QVariantMap& parameters, uint state)
    {
        qDebug() << "IChannelTypeTubes - Channel::onNewTube()";
    }

    void Channel::onTubeStateChanged(uint ID, uint state)
    {
        qDebug() << "IChannelTypeTubes - Channel::onTubeStateChanged()";
    }

    void Channel::onTubeClosed(uint ID)
    {
        qDebug() << "IChannelTypeTubes - Channel::onTubeClosed()";
    }

    void Channel::onDBusNamesChanged(uint ID, const DBus::Interfaces::DBusTubeMemberList& added, const DBus::Interfaces::UIntList& removed)
    {
        qDebug() << "IChannelTypeTubes - Channel::onDBusNamesChanged()";
    }

    void Channel::onStreamTubeNewConnection(uint ID, uint handle)
    {
        qDebug() << "IChannelTypeTubes - Channel::onStreamTubeNewConnection()";
    }
}
