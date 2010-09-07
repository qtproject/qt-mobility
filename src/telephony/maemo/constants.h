/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtCore/QFlags>
#include <QtCore/QString>

#define PATH2BUS(path) path.mid(1).replace(QLatin1String("/"), QLatin1String("."))

namespace DBus
{
    enum ConnMgrParamFlag
    {
         ConnMgrParamFlagRequired = 1,
         ConnMgrParamFlagRegister = 2,
         ConnMgrParamFlagHasDefault = 4,
         ConnMgrParamFlagSecret = 8,
         ConnMgrParamFlagDBusProperty = 16
    };
    typedef QFlags<ConnMgrParamFlag> ConnMgrParamFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ConnMgrParamFlags)

    enum ConnectionAliasFlag
    {
         ConnectionAliasFlagUserSet = 1
    };

    typedef QFlags<ConnectionAliasFlag> ConnectionAliasFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ConnectionAliasFlags)

    enum ConnectionCapabilityFlag
    {
         ConnectionCapabilityFlagCreate = 1,
         ConnectionCapabilityFlagInvite = 2
    };
    typedef QFlags<ConnectionCapabilityFlag> ConnectionCapabilityFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ConnectionCapabilityFlags)

    enum ContactInfoFlag
    {
         ContactInfoFlagCanSet = 1,
         ContactInfoFlagPush = 2
    };

    typedef QFlags<ContactInfoFlag> ContactInfoFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ContactInfoFlags)

    enum ContactInfoFieldFlag
    {
         ContactInfoFieldFlagParametersExact = 1
    };
    typedef QFlags<ContactInfoFieldFlag> ContactInfoFieldFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ContactInfoFieldFlags)

    enum MediaStreamPending
    {
         MediaStreamPendingLocalSend = 1,
         MediaStreamPendingRemoteSend = 2
    };
    typedef QFlags<MediaStreamPending> MediaStreamPendingSend;
    Q_DECLARE_OPERATORS_FOR_FLAGS(MediaStreamPendingSend)

    enum ChannelMediaCapability
    {
         ChannelMediaCapabilityAudio = 1,
         ChannelMediaCapabilityVideo = 2,
         ChannelMediaCapabilityNATTraversalSTUN = 4,
         ChannelMediaCapabilityNATTraversalGTalkP2P = 8,
         ChannelMediaCapabilityNATTraversalICEUDP = 16,
         ChannelMediaCapabilityImmutableStreams = 32
    };
    typedef QFlags<ChannelMediaCapability> ChannelMediaCapabilities;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelMediaCapabilities)

    enum ChannelTextMessageFlag
    {
         ChannelTextMessageFlagTruncated = 1,
         ChannelTextMessageFlagNonTextContent = 2,
         ChannelTextMessageFlagScrollback = 4,
         ChannelTextMessageFlagRescued = 8
    };
    typedef QFlags<ChannelTextMessageFlag> ChannelTextMessageFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelTextMessageFlags)

    enum ChannelCallState
    {
         ChannelCallStateRinging = 1,
         ChannelCallStateQueued = 2,
         ChannelCallStateHeld = 4,
         ChannelCallStateForwarded = 8,
         ChannelCallStateInProgress = 16
    };
    typedef QFlags<ChannelCallState> ChannelCallStateFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelCallStateFlags)

    enum ChannelGroupFlag
    {
         ChannelGroupFlagCanAdd = 1,
         ChannelGroupFlagCanRemove = 2,
         ChannelGroupFlagCanRescind = 4,
         ChannelGroupFlagMessageAdd = 8,
         ChannelGroupFlagMessageRemove = 16,
         ChannelGroupFlagMessageAccept = 32,
         ChannelGroupFlagMessageReject = 64,
         ChannelGroupFlagMessageRescind = 128,
         ChannelGroupFlagChannelSpecificHandles = 256,
         ChannelGroupFlagOnlyOneGroup = 512,
         ChannelGroupFlagHandleOwnersNotAvailable = 1024,
         ChannelGroupFlagProperties = 2048,
         ChannelGroupFlagMembersChangedDetailed = 4096,
         ChannelGroupFlagMessageDepart = 8192
    };
    typedef QFlags<ChannelGroupFlag> ChannelGroupFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelGroupFlags)

    enum MessagePartSupportFlag
    {
         MessagePartSupportFlagOneAttachment = 1,
         MessagePartSupportFlagMultipleAttachments = 2
    };
    typedef QFlags<MessagePartSupportFlag> MessagePartSupportFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(MessagePartSupportFlags)

    enum MessageSendingFlag
    {
         MessageSendingFlagReportDelivery = 1
    };
    typedef QFlags<MessageSendingFlag> MessageSendingFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(MessageSendingFlags)

    enum DeliveryReportingSupportFlag
    {
         DeliveryReportingSupportFlagReceiveFailures = 1,
         DeliveryReportingSupportFlagReceiveSuccesses = 2
    };
    typedef QFlags<DeliveryReportingSupportFlag> DeliveryReportingSupportFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(DeliveryReportingSupportFlags)

    enum ChannelPasswordFlag
    {
         ChannelPasswordFlagProvide = 8
    };
    typedef QFlags<ChannelPasswordFlag> ChannelPasswordFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelPasswordFlags)

    enum PropertyFlag
    {
         PropertyFlagRead = 1,
         PropertyFlagWrite = 2
    };
    typedef QFlags<PropertyFlag> PropertyFlags;
    Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyFlags)

    enum HandleType
    {
         HandleTypeNone = 0,
         HandleTypeContact = 1,
         HandleTypeRoom = 2,
         HandleTypeList = 3,
         HandleTypeGroup = 4
    };

    const int NUM_HANDLE_TYPES = (4+1);

    enum ConnectionStatus
    {
         ConnectionStatusConnected = 0,
         ConnectionStatusConnecting = 1,
         ConnectionStatusDisconnected = 2
    };

    const int NUM_CONNECTION_STATUSES = (2+1);

    enum ConnectionStatusReason
    {
         ConnectionStatusReasonNoneSpecified = 0,
         ConnectionStatusReasonRequested = 1,
         ConnectionStatusReasonNetworkError = 2,
         ConnectionStatusReasonAuthenticationFailed = 3,
         ConnectionStatusReasonEncryptionError = 4,
         ConnectionStatusReasonNameInUse = 5,
         ConnectionStatusReasonCertNotProvided = 6,
         ConnectionStatusReasonCertUntrusted = 7,
         ConnectionStatusReasonCertExpired = 8,
         ConnectionStatusReasonCertNotActivated = 9,
         ConnectionStatusReasonCertHostnameMismatch = 10,
         ConnectionStatusReasonCertFingerprintMismatch = 11,
         ConnectionStatusReasonCertSelfSigned = 12,
         ConnectionStatusReasonCertOtherError = 13
    };

    const int NUM_CONNECTION_STATUS_REASONS = (13+1);

    enum ConnectionPresenceType
    {
         ConnectionPresenceTypeUnset = 0,
         ConnectionPresenceTypeOffline = 1,
         ConnectionPresenceTypeAvailable = 2,
         ConnectionPresenceTypeAway = 3,
         ConnectionPresenceTypeExtendedAway = 4,
         ConnectionPresenceTypeHidden = 5,
         ConnectionPresenceTypeBusy = 6,
         ConnectionPresenceTypeUnknown = 7,
         ConnectionPresenceTypeError = 8
    };

    const int NUM_CONNECTION_PRESENCE_TYPES = (8+1);

    enum RichPresenceAccessControlType
    {
         RichPresenceAccessControlTypeWhitelist = 0,
         RichPresenceAccessControlTypePublishList = 1,
         RichPresenceAccessControlTypeGroup = 2,
         RichPresenceAccessControlTypeOpen = 3
    };

    const int NUM_RICH_PRESENCE_ACCESS_CONTROL_TYPES = (3+1);

    enum FileTransferState
    {
         FileTransferStateNone = 0,
         FileTransferStatePending = 1,
         FileTransferStateAccepted = 2,
         FileTransferStateOpen = 3,
         FileTransferStateCompleted = 4,
         FileTransferStateCancelled = 5
    };

    const int NUM_FILE_TRANSFER_STATES = (5+1);

    enum FileTransferStateChangeReason
    {
         FileTransferStateChangeReasonNone = 0,
         FileTransferStateChangeReasonRequested = 1,
         FileTransferStateChangeReasonLocalStopped = 2,
         FileTransferStateChangeReasonRemoteStopped = 3,
         FileTransferStateChangeReasonLocalError = 4,
         FileTransferStateChangeReasonRemoteError = 5
    };

    const int NUM_FILE_TRANSFER_STATE_CHANGE_REASONS = (5+1);

    enum FileHashType
    {
         FileHashTypeNone = 0,
         FileHashTypeMD5 = 1,
         FileHashTypeSHA1 = 2,
         FileHashTypeSHA256 = 3
    };

    const int NUM_FILE_HASH_TYPES = (3+1);

    enum MediaStreamType
    {
         MediaStreamTypeAudio = 0,
         MediaStreamTypeVideo = 1
    };

    const int NUM_MEDIA_STREAM_TYPES = (1+1);

    enum MediaStreamState
    {
         MediaStreamStateDisconnected = 0,
         MediaStreamStateConnecting = 1,
         MediaStreamStateConnected = 2
    };

    const int NUM_MEDIA_STREAM_STATES = (2+1);

    enum MediaStreamDirection
    {
         MediaStreamDirectionNone = 0,
         MediaStreamDirectionSend = 1,
         MediaStreamDirectionReceive = 2,
         MediaStreamDirectionBidirectional = 3
    };

    const int NUM_MEDIA_STREAM_DIRECTIONS = (3+1);

    enum ChannelTextSendError
    {
         ChannelTextSendErrorUnknown = 0,
         ChannelTextSendErrorOffline = 1,
         ChannelTextSendErrorInvalidContact = 2,
         ChannelTextSendErrorPermissionDenied = 3,
         ChannelTextSendErrorTooLong = 4,
         ChannelTextSendErrorNotImplemented = 5
    };

    const int NUM_CHANNEL_TEXT_SEND_ERRORS = (5+1);

    enum ChannelTextMessageType
    {
         ChannelTextMessageTypeNormal = 0,
         ChannelTextMessageTypeAction = 1,
         ChannelTextMessageTypeNotice = 2,
         ChannelTextMessageTypeAutoReply = 3,
         ChannelTextMessageTypeDeliveryReport = 4
    };

    const int NUM_CHANNEL_TEXT_MESSAGE_TYPES = (4+1);

    enum TubeType
    {
         TubeTypeDBus = 0,
         TubeTypeStream = 1
    };

    const int NUM_TUBE_TYPES = (1+1);

    enum TubeState
    {
         TubeStateLocalPending = 0,
         TubeStateRemotePending = 1,
         TubeStateOpen = 2
    };

    const int NUM_TUBE_STATES = (2+1);

    enum ChannelChatState
    {
         ChannelChatStateGone = 0,
         ChannelChatStateInactive = 1,
         ChannelChatStateActive = 2,
         ChannelChatStatePaused = 3,
         ChannelChatStateComposing = 4
    };

    const int NUM_CHANNEL_CHAT_STATES = (4+1);

    enum DTMFEvent
    {
         DTMFEventDigit0 = 0,
         DTMFEventDigit1 = 1,
         DTMFEventDigit2 = 2,
         DTMFEventDigit3 = 3,
         DTMFEventDigit4 = 4,
         DTMFEventDigit5 = 5,
         DTMFEventDigit6 = 6,
         DTMFEventDigit7 = 7,
         DTMFEventDigit8 = 8,
         DTMFEventDigit9 = 9,
         DTMFEventAsterisk = 10,
         DTMFEventHash = 11,
         DTMFEventLetterA = 12,
         DTMFEventLetterB = 13,
         DTMFEventLetterC = 14,
         DTMFEventLetterD = 15
    };

    const int NUM_DTMF_EVENTS = (15+1);

    enum ChannelGroupChangeReason
    {
         ChannelGroupChangeReasonNone = 0,
         ChannelGroupChangeReasonOffline = 1,
         ChannelGroupChangeReasonKicked = 2,
         ChannelGroupChangeReasonBusy = 3,
         ChannelGroupChangeReasonInvited = 4,
         ChannelGroupChangeReasonBanned = 5,
         ChannelGroupChangeReasonError = 6,
         ChannelGroupChangeReasonInvalidContact = 7,
         ChannelGroupChangeReasonNoAnswer = 8,
         ChannelGroupChangeReasonRenamed = 9,
         ChannelGroupChangeReasonPermissionDenied = 10,
         ChannelGroupChangeReasonSeparated = 11
    };

    const int NUM_CHANNEL_GROUP_CHANGE_REASONS = (11+1);

    enum LocalHoldState
    {
         LocalHoldStateUnheld = 0,
         LocalHoldStateHeld = 1,
         LocalHoldStatePendingHold = 2,
         LocalHoldStatePendingUnhold = 3
    };

    const int NUM_LOCAL_HOLD_STATES = (3+1);

    enum LocalHoldStateReason
    {
         LocalHoldStateReasonNone = 0,
         LocalHoldStateReasonRequested = 1,
         LocalHoldStateReasonResourceNotAvailable = 2
    };

    const int NUM_LOCAL_HOLD_STATE_REASONS = (2+1);

    enum DeliveryStatus
    {
         DeliveryStatusUnknown = 0,
         DeliveryStatusDelivered = 1,
         DeliveryStatusTemporarilyFailed = 2,
         DeliveryStatusPermanentlyFailed = 3,
         DeliveryStatusAccepted = 4
    };

    const int NUM_DELIVERY_STATUSES = (4+1);

    enum TubeChannelState
    {
         TubeChannelStateLocalPending = 0,
         TubeChannelStateRemotePending = 1,
         TubeChannelStateOpen = 2,
         TubeChannelStateNotOffered = 3
    };

    const int NUM_TUBE_CHANNEL_STATES = (3+1);

    enum SocketAddressType
    {
         SocketAddressTypeUnix = 0,
         SocketAddressTypeAbstractUnix = 1,
         SocketAddressTypeIPv4 = 2,
         SocketAddressTypeIPv6 = 3
    };

    const int NUM_SOCKET_ADDRESS_TYPES = (3+1);

    enum SocketAccessControl
    {
         SocketAccessControlLocalhost = 0,
         SocketAccessControlPort = 1,
         SocketAccessControlNetmask = 2,
         SocketAccessControlCredentials = 3
    };

    const int NUM_SOCKET_ACCESS_CONTROLS = (3+1);

    enum MediaStreamError
    {
         MediaStreamErrorUnknown = 0,
         MediaStreamErrorEOS = 1,
         MediaStreamErrorCodecNegotiationFailed = 2,
         MediaStreamErrorConnectionFailed = 3,
         MediaStreamErrorNetworkError = 4,
         MediaStreamErrorNoCodecs = 5,
         MediaStreamErrorInvalidCMBehavior = 6,
         MediaStreamErrorMediaError = 7
    };

    const int NUM_MEDIA_STREAM_ERRORS = (7+1);

    enum MediaStreamBaseProto
    {
         MediaStreamBaseProtoUDP = 0,
         MediaStreamBaseProtoTCP = 1
    };

    const int NUM_MEDIA_STREAM_BASE_PROTOS = (1+1);

    enum MediaStreamTransportType
    {
         MediaStreamTransportTypeLocal = 0,
         MediaStreamTransportTypeDerived = 1,
         MediaStreamTransportTypeRelay = 2
    };
    const int NUM_MEDIA_STREAM_TRANSPORT_TYPES = (2+1);
}//DBus

#define TELEPATHY_INTERFACE_CONNECTION_MANAGER "org.freedesktop.Telepathy.ConnectionManager"
#define TELEPATHY_INTERFACE_CONNECTION "org.freedesktop.Telepathy.Connection"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_ALIASING "org.freedesktop.Telepathy.Connection.Interface.Aliasing"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_AVATARS "org.freedesktop.Telepathy.Connection.Interface.Avatars"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_BALANCE "org.freedesktop.Telepathy.Connection.Interface.Balance"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CAPABILITIES "org.freedesktop.Telepathy.Connection.Interface.Capabilities"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACTS "org.freedesktop.Telepathy.Connection.Interface.Contacts"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACT_CAPABILITIES "org.freedesktop.Telepathy.Connection.Interface.ContactCapabilities"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACT_INFO "org.freedesktop.Telepathy.Connection.Interface.ContactInfo"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_LOCATION "org.freedesktop.Telepathy.Connection.Interface.Location"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_PRESENCE "org.freedesktop.Telepathy.Connection.Interface.Presence"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_REQUESTS "org.freedesktop.Telepathy.Connection.Interface.Requests"
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_SIMPLE_PRESENCE "org.freedesktop.Telepathy.Connection.Interface.SimplePresence"
#define TELEPATHY_INTERFACE_CHANNEL "org.freedesktop.Telepathy.Channel"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_CONTACT_LIST "org.freedesktop.Telepathy.Channel.Type.ContactList"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_FILE_TRANSFER "org.freedesktop.Telepathy.Channel.Type.FileTransfer"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA "org.freedesktop.Telepathy.Channel.Type.StreamedMedia"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_ROOM_LIST "org.freedesktop.Telepathy.Channel.Type.RoomList"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT "org.freedesktop.Telepathy.Channel.Type.Text"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_TUBES "org.freedesktop.Telepathy.Channel.Type.Tubes"
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE "org.freedesktop.Telepathy.Channel.Type.StreamTube"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_CALL_STATE "org.freedesktop.Telepathy.Channel.Interface.CallState"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_CHAT_STATE "org.freedesktop.Telepathy.Channel.Interface.ChatState"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_DTMF "org.freedesktop.Telepathy.Channel.Interface.DTMF"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_GROUP "org.freedesktop.Telepathy.Channel.Interface.Group"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_HOLD "org.freedesktop.Telepathy.Channel.Interface.Hold"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_MEDIA_SIGNALLING "org.freedesktop.Telepathy.Channel.Interface.MediaSignalling"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_MESSAGES "org.freedesktop.Telepathy.Channel.Interface.Messages"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_PASSWORD "org.freedesktop.Telepathy.Channel.Interface.Password"
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_TUBE "org.freedesktop.Telepathy.Channel.Interface.Tube"
#define TELEPATHY_INTERFACE_CHANNEL_DISPATCHER "org.freedesktop.Telepathy.ChannelDispatcher"
#define TELEPATHY_INTERFACE_CHANNEL_DISPATCH_OPERATION "org.freedesktop.Telepathy.ChannelDispatchOperation"
#define TELEPATHY_INTERFACE_CHANNEL_REQUEST "org.freedesktop.Telepathy.ChannelRequest"
#define TELEPATHY_INTERFACE_MEDIA_SESSION_HANDLER "org.freedesktop.Telepathy.Media.SessionHandler"
#define TELEPATHY_INTERFACE_MEDIA_STREAM_HANDLER "org.freedesktop.Telepathy.Media.StreamHandler"
#define TELEPATHY_INTERFACE_DBUS_DAEMON "org.freedesktop.DBus"
#define TELEPATHY_INTERFACE_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define TELEPATHY_INTERFACE_PEER "org.freedesktop.DBus.Peer"
#define TELEPATHY_INTERFACE_PROPERTIES "org.freedesktop.DBus.Properties"
#define TELEPATHY_INTERFACE_PROPERTIES_INTERFACE "org.freedesktop.Telepathy.Properties"
#define TELEPATHY_INTERFACE_ACCOUNT_MANAGER "org.freedesktop.Telepathy.AccountManager"
#define TELEPATHY_INTERFACE_ACCOUNT "org.freedesktop.Telepathy.Account"
#define TELEPATHY_INTERFACE_ACCOUNT_INTERFACE_AVATAR "org.freedesktop.Telepathy.Account.Interface.Avatar"
#define TELEPATHY_INTERFACE_CLIENT "org.freedesktop.Telepathy.Client"
#define TELEPATHY_INTERFACE_CLIENT_OBSERVER "org.freedesktop.Telepathy.Client.Observer"
#define TELEPATHY_INTERFACE_CLIENT_APPROVER "org.freedesktop.Telepathy.Client.Approver"
#define TELEPATHY_INTERFACE_CLIENT_HANDLER "org.freedesktop.Telepathy.Client.Handler"
#define TELEPATHY_INTERFACE_CLIENT_INTERFACE_REQUESTS "org.freedesktop.Telepathy.Client.Interface.Requests"
#define TELEPATHY_ERROR_NETWORK_ERROR "org.freedesktop.Telepathy.Error.NetworkError"
#define TELEPATHY_ERROR_NOT_IMPLEMENTED "org.freedesktop.Telepathy.Error.NotImplemented"
#define TELEPATHY_ERROR_INVALID_ARGUMENT "org.freedesktop.Telepathy.Error.InvalidArgument"
#define TELEPATHY_ERROR_NOT_AVAILABLE "org.freedesktop.Telepathy.Error.NotAvailable"
#define TELEPATHY_ERROR_PERMISSION_DENIED "org.freedesktop.Telepathy.Error.PermissionDenied"
#define TELEPATHY_ERROR_DISCONNECTED "org.freedesktop.Telepathy.Error.Disconnected"
#define TELEPATHY_ERROR_INVALID_HANDLE "org.freedesktop.Telepathy.Error.InvalidHandle"
#define TELEPATHY_ERROR_CHANNEL_BANNED "org.freedesktop.Telepathy.Error.Channel.Banned"
#define TELEPATHY_ERROR_CHANNEL_FULL "org.freedesktop.Telepathy.Error.Channel.Full"
#define TELEPATHY_ERROR_CHANNEL_INVITE_ONLY "org.freedesktop.Telepathy.Error.Channel.InviteOnly"
#define TELEPATHY_ERROR_NOT_YOURS "org.freedesktop.Telepathy.Error.NotYours"
#define TELEPATHY_ERROR_CANCELLED "org.freedesktop.Telepathy.Error.Cancelled"
#define TELEPATHY_ERROR_AUTHENTICATION_FAILED "org.freedesktop.Telepathy.Error.AuthenticationFailed"
#define TELEPATHY_ERROR_ENCRYPTION_NOT_AVAILABLE "org.freedesktop.Telepathy.Error.EncryptionNotAvailable"
#define TELEPATHY_ERROR_ENCRYPTION_ERROR "org.freedesktop.Telepathy.Error.EncryptionError"
#define TELEPATHY_ERROR_CERT_NOT_PROVIDED "org.freedesktop.Telepathy.Error.Cert.NotProvided"
#define TELEPATHY_ERROR_CERT_UNTRUSTED "org.freedesktop.Telepathy.Error.Cert.Untrusted"
#define TELEPATHY_ERROR_CERT_EXPIRED "org.freedesktop.Telepathy.Error.Cert.Expired"
#define TELEPATHY_ERROR_CERT_NOT_ACTIVATED "org.freedesktop.Telepathy.Error.Cert.NotActivated"
#define TELEPATHY_ERROR_CERT_FINGERPRINT_MISMATCH "org.freedesktop.Telepathy.Error.Cert.FingerprintMismatch"
#define TELEPATHY_ERROR_CERT_HOSTNAME_MISMATCH "org.freedesktop.Telepathy.Error.Cert.HostnameMismatch"
#define TELEPATHY_ERROR_CERT_SELF_SIGNED "org.freedesktop.Telepathy.Error.Cert.SelfSigned"
#define TELEPATHY_ERROR_CERT_INVALID "org.freedesktop.Telepathy.Error.Cert.Invalid"
#define TELEPATHY_ERROR_NOT_CAPABLE "org.freedesktop.Telepathy.Error.NotCapable"
#define TELEPATHY_ERROR_OFFLINE "org.freedesktop.Telepathy.Error.Offline"
#define TELEPATHY_ERROR_CHANNEL_KICKED "org.freedesktop.Telepathy.Error.Channel.Kicked"
#define TELEPATHY_ERROR_BUSY "org.freedesktop.Telepathy.Error.Busy"
#define TELEPATHY_ERROR_NO_ANSWER "org.freedesktop.Telepathy.Error.NoAnswer"
#define TELEPATHY_ERROR_DOES_NOT_EXIST "org.freedesktop.Telepathy.Error.DoesNotExist"
#define TELEPATHY_ERROR_TERMINATED "org.freedesktop.Telepathy.Error.Terminated"
#define TELEPATHY_ERROR_CONNECTION_REFUSED "org.freedesktop.Telepathy.Error.ConnectionRefused"
#define TELEPATHY_ERROR_CONNECTION_FAILED "org.freedesktop.Telepathy.Error.ConnectionFailed"
#define TELEPATHY_ERROR_CONNECTION_LOST "org.freedesktop.Telepathy.Error.ConnectionLost"
#define TELEPATHY_ERROR_ALREADY_CONNECTED "org.freedesktop.Telepathy.Error.AlreadyConnected"
#define TELEPATHY_ERROR_CONNECTION_REPLACED "org.freedesktop.Telepathy.Error.ConnectionReplaced"
#define TELEPATHY_ERROR_REGISTRATION_EXISTS "org.freedesktop.Telepathy.Error.RegistrationExists"
#define TELEPATHY_ERROR_SERVICE_BUSY "org.freedesktop.Telepathy.Error.ServiceBusy"
#define TELEPATHY_ERROR_RESOURCE_UNAVAILABLE "org.freedesktop.Telepathy.Error.ResourceUnavailable"
#define TELEPATHY_DBUS_ERROR_NAME_HAS_NO_OWNER "org.freedesktop.DBus.Error.NameHasNoOwner"
#define TELEPATHY_DBUS_ERROR_UNKNOWN_METHOD "org.freedesktop.DBus.Error.UnknownMethod"
#define TELEPATHY_QT4_ERROR_OBJECT_REMOVED "org.freedesktop.Telepathy.Qt4.Error.ObjectRemoved"
#define TELEPATHY_QT4_ERROR_INCONSISTENT "org.freedesktop.Telepathy.Qt4.Error.Inconsistent"
#define TELEPATHY_ACCOUNT_MANAGER_BUS_NAME "org.freedesktop.Telepathy.AccountManager"
#define TELEPATHY_ACCOUNT_MANAGER_OBJECT_PATH "/org/freedesktop/Telepathy/AccountManager"

#endif//CONSTANTS_H
