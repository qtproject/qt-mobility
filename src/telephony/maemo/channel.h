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

#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtCore>
#include <QtDBus/QDBusMessage>

#include "maemo/types.h"
#include "maemo/statefulldbusproxy.h"
#include "maemo/interfaces/ichannel.h"
#include "maemo/interfaces/ichannelcallstate.h"
#include "maemo/interfaces/ichannelchatstate.h"
#include "maemo/interfaces/ichanneldtmf.h"
#include "maemo/interfaces/ichannelgroup.h"
#include "maemo/interfaces/ichannelhold.h"
#include "maemo/interfaces/ichannelmediasignalling.h"
#include "maemo/interfaces/ichannelmessages.h"
#include "maemo/interfaces/ichannelpassword.h"
#include "maemo/interfaces/ichanneltube.h"
#include "maemo/interfaces/ichanneltypecontactlist.h"
#include "maemo/interfaces/ichanneltypefiletransfer.h"
#include "maemo/interfaces/ichanneltyperoomlist.h"
#include "maemo/interfaces/ichanneltypestreamedmedia.h"
#include "maemo/interfaces/ichanneltypestreamtube.h"
#include "maemo/interfaces/ichanneltypetext.h"
#include "maemo/interfaces/ichanneltypetubes.h"
#include "maemo/readyobject.h"
#include "maemo/connection.h"
#include "qtelephonycallinfo.h"
#include "qtelephony.h"

QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

namespace DBus
{
    class PendingOperation;
    class Channel : public StatefullDBusProxy
                        , public ReadyObject
                        , public ReferenceCounter
    {
        Q_OBJECT
        Q_DISABLE_COPY(Channel)
    public:
        static const Feature FeatureCore;
        static bool isCall(QString channeltype);

        Channel(QDBusConnection busconnection, const QString busname, const QString &objectPath, const QVariantMap &properties, Connection* connection);
        ~Channel();
        QVariantMap properties() const { return propertylist; }
        QTelephony::CallStatus getStatus() { return status; }
        int getDirection() { return direction; }
        QString getRemotePartyIdentifier() { return remoteIdentifier; }
        QTelephony::CallType getCalltype();
        QString getSubType() { return subtype; }

        bool isCall() {
            if(pIChannelTypeStreamedMedia || pIChannelTypeText)
                return true;
            return false;
        }
        bool isOutgoing() { return ( direction == 2 ? true : false); }
        bool isIncomming() { return ( direction == 1 ? true : false); }
        bool isText() { return (pIChannelTypeText == 0 ? false : true); }

    Q_SIGNALS:
        void StatusChanged(DBus::Channel* pchannel);

    private Q_SLOTS:
        //for ReadyObject
        void onChannelReady(DBus::PendingOperation* operation);

        //for IChannel signals
        void onClose();

        //for IChannelCallState signals
        void onCallStateChanged(uint contact, uint state);

        //for IChannelChatState signals
        void onChatStateChanged(uint contact, uint state);

        //for IChannelDTMF signals
        //- no signals

        //for IChannelGroup signals
        void onHandleOwnersChanged(const DBus::Interfaces::HandleOwnerMap& added, const DBus::Interfaces::UIntList& removed);
        void onSelfHandleChanged(uint selfHandle);
        void onGroupFlagsChanged(uint added, uint removed);
        void onMembersChanged(const QString& message, const DBus::Interfaces::UIntList& added, const DBus::Interfaces::UIntList& removed, const DBus::Interfaces::UIntList& localPending, const DBus::Interfaces::UIntList& remotePending, uint actor, uint reason);
        void onMembersChangedDetailed(const DBus::Interfaces::UIntList& added, const DBus::Interfaces::UIntList& removed, const DBus::Interfaces::UIntList& localPending, const DBus::Interfaces::UIntList& remotePending, const QVariantMap& details);

        //for IChannelHold signals
        void onHoldStateChanged(uint holdState, uint reason);

        //for IChannelMediaSignalling signals
        void onNewSessionHandler(const QDBusObjectPath& sessionHandler, const QString& sessionType);

        // for IChannelMessages signals
        void onMessageSent(const DBus::Interfaces::MessagePartList& content, uint flags, const QString& messageToken);
        void onPendingMessagesRemoved(const DBus::Interfaces::UIntList& messageIDs);
        void onMessageReceived(const DBus::Interfaces::MessagePartList& message);

        //for IChannelPassword signals
        void onPasswordFlagsChanged(uint added, uint removed);

        //for IChannelTube signals
        void onTubeChannelStateChanged(uint state);


        //for IChannelTypeContactList signals
        //no signals

        //for IChannelTypeFileTransfer signals
        void onFileTransferStateChanged(uint state, uint reason);
        void onTransferredBytesChanged(qulonglong count);
        void onInitialOffsetDefined(qulonglong initialOffset);

        //for IChannelTypeRoomList signals
        void onGotRooms(const DBus::Interfaces::RoomInfoList& rooms);
        void onListingRooms(bool listing);

        //for IChannelTypeStreamTube signals
        void onNewRemoteConnection(uint handle, const QDBusVariant& connectionParam, uint connectionID);
        void onNewLocalConnection(uint connectionID);
        void onConnectionClosed(uint connectionID, const QString& error, const QString& message);

        //for IChannelTypeStreamedMedia signals
        void onStreamAdded(uint streamID, uint contactHandle, uint streamType);
        void onStreamDirectionChanged(uint streamID, uint streamDirection, uint pendingFlags);
        void onStreamError(uint streamID, uint errorCode, const QString& message);
        void onStreamRemoved(uint streamID);
        void onStreamStateChanged(uint streamID, uint streamState);

        //for IChannelTypeText signals
        void onLostMessage();
        void onReceived(uint ID, uint timestamp, uint sender, uint type, uint flags, const QString& text);
        void onSendError(uint error, uint timestamp, uint type, const QString& text);
        void onSent(uint timestamp, uint type, const QString& text);

        //for IChannelTypeTubes signals
        void onNewTube(uint ID, uint initiator, uint type, const QString& service, const QVariantMap& parameters, uint state);
        void onTubeStateChanged(uint ID, uint state);
        void onTubeClosed(uint ID);
        void onDBusNamesChanged(uint ID, const DBus::Interfaces::DBusTubeMemberList& added, const DBus::Interfaces::UIntList& removed);
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
        DBus::Interfaces::IChannel* pIChannel;
        DBus::Interfaces::IChannelCallState* pIChannelCallState;
        DBus::Interfaces::IChannelChatState* pIChannelChatState;
        DBus::Interfaces::IChannelDTMF* pIChannelDTMF;
        DBus::Interfaces::IChannelGroup* pIChannelGroup;
        DBus::Interfaces::IChannelHold* pIChannelHold;
        DBus::Interfaces::IChannelMediaSignalling* pIChannelMediaSignalling;
        DBus::Interfaces::IChannelMessages* pIChannelMessages;
        DBus::Interfaces::IChannelPassword* pIChannelPassword;
        DBus::Interfaces::IChannelTube* pIChannelTube;
        DBus::Interfaces::IChannelTypeContactList* pIChannelTypeContactList;
        DBus::Interfaces::IChannelTypeFileTransfer* pIChannelTypeFileTransfer;
        DBus::Interfaces::IChannelTypeRoomList* pIChannelTypeRoomList;
        DBus::Interfaces::IChannelTypeStreamTube* pIChannelTypeStreamTube;
        DBus::Interfaces::IChannelTypeStreamedMedia* pIChannelTypeStreamedMedia;
        DBus::Interfaces::IChannelTypeText* pIChannelTypeText;
        DBus::Interfaces::IChannelTypeTubes* pIChannelTypeTubes;

    private:
        QTelephony::CallStatus status;
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
    typedef SharedPointer<Channel> ChannelPtr;
}
#endif // CHANNEL_H
