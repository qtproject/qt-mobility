/* Generated from Telepathy D-Bus Interface Specification, TelepathyQt4 copy, version 0.17.7
 */

#include <QtCore/QFlags>
#include <QtCore/QString>

#define PATH2BUS(path) path.mid(1).replace(QLatin1String("/"), QLatin1String("."))


/**
 * \addtogroup typesconstants Types and constants
 *
 * Enumerated, flag, structure, list and mapping types and utility constants.
 */

/**
 * \defgroup flagtypeconsts Flag type constants
 * \ingroup typesconstants
 *
 * Types generated from the specification representing bit flag constants and
 * combinations of them (bitfields).
 */

/**
 * \defgroup enumtypeconsts Enumerated type constants
 * \ingroup typesconstants
 *
 * Types generated from the specification representing enumerated types ie.
 * types the values of which are mutually exclusive integral constants.
 */

/**
 * \defgroup ifacestrconsts Interface string constants
 * \ingroup typesconstants
 *
 * D-Bus interface names of the interfaces in the specification.
 */

/**
 * \defgroup errorstrconsts Error string constants
 * \ingroup typesconstants
 *
 * Names of the D-Bus errors in the specification.
 */

namespace Tp
{
/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ConnMgrParamFlag
{
    /**
     * This parameter is required for connecting to the server.
     */
     ConnMgrParamFlagRequired = 1,

    /**
     * This parameter is required for registering an account on the server.
     */
     ConnMgrParamFlagRegister = 2,

    /**
     * This parameter has a default value, which is returned in GetParameters; 
     * not providing this parameter is equivalent to providing the default.
     */
     ConnMgrParamFlagHasDefault = 4,

    /**
     * <p>This parameter should be considered private or secret; for
     *   instance, clients should store it in a &quot;password safe&quot; like
     *   gnome-keyring or kwallet, omit it from debug logs, and use a
     *   text input widget that hides the value of the parameter.</p>
     * 
     * <p>(Clients that support older connection managers may also treat
     *   any parameter whose name contains &quot;password&quot; as though it had this
     *   flag.)</p>
     */
     ConnMgrParamFlagSecret = 8,

    /**
     * This parameter is also a D-Bus property on the resulting <tp:dbus-ref namespace="org.freedesktop.Telepathy">Connection</tp:dbus-ref>; a
     * parameter named <code>com.example.Duck.Macaroni</code> with this flag
     * corresponds to the <code>Macaroni</code> property on the
     * <code>com.example.Duck</code> interface.  Its value can be queried
     * and possibly changed on an existing Connection using methods on the
     * <code>org.freedesktop.DBus.Properties</code> interface.
     */
     ConnMgrParamFlagDBusProperty = 16
};

/**
 * \typedef QFlags<ConnMgrParamFlag> ConnMgrParamFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ConnMgrParamFlag values.
 */
typedef QFlags<ConnMgrParamFlag> ConnMgrParamFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ConnMgrParamFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ConnectionAliasFlag
{
    /**
     * <p>The aliases of contacts on this connection may be changed by the
     *   user of the service, not just by the contacts themselves. This is
     *   the case on Jabber, for instance.</p>
     * <p>It is possible that aliases can be changed by the contacts too -
     *   which alias takes precedence is not defined by this
     *   specification, and depends on the server and/or connection manager
     *   implementation.</p>
     * <p>This flag only applies to the aliases of &quot;globally valid&quot; contact
     *   handles. At this time, clients should not expect to be able to
     *   change the aliases corresponding to any channel-specific
     *   handles. If this becomes possible in future, a new flag will
     *   be defined.</p>
     */
     ConnectionAliasFlagUserSet = 1
};

/**
 * \typedef QFlags<ConnectionAliasFlag> ConnectionAliasFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ConnectionAliasFlag values.
 */
typedef QFlags<ConnectionAliasFlag> ConnectionAliasFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ConnectionAliasFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ConnectionCapabilityFlag
{
    /**
     * The given channel type and handle can be given to RequestChannel to 
     * create a new channel of this type.
     */
     ConnectionCapabilityFlagCreate = 1,

    /**
     * The given contact can be invited to an existing channel of this type.
     */
     ConnectionCapabilityFlagInvite = 2
};

/**
 * \typedef QFlags<ConnectionCapabilityFlag> ConnectionCapabilityFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ConnectionCapabilityFlag values.
 */
typedef QFlags<ConnectionCapabilityFlag> ConnectionCapabilityFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ConnectionCapabilityFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ContactInfoFlag
{
    /**
     * Indicates that SetContactInfo is supported on this connection.
     */
     ContactInfoFlagCanSet = 1,

    /**
     * Indicates that the protocol pushes all contacts&apos; information to 
     * the connection manager without prompting. If set, ContactInfoChanged 
     * will be emitted whenever contacts&apos; information changes.
     */
     ContactInfoFlagPush = 2
};

/**
 * \typedef QFlags<ContactInfoFlag> ContactInfoFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ContactInfoFlag values.
 * 
 * Flags defining the behaviour of contact information on this protocol. Some 
 * protocols provide no information on contacts without an explicit request; 
 * others always push information to the connection manager as and when it 
 * changes.
 */
typedef QFlags<ContactInfoFlag> ContactInfoFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ContactInfoFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ContactInfoFieldFlag
{
    /**
     * <p>If present, exactly the parameters indicated must be set on this
     *   field; in the case of an empty list of parameters, this implies that
     *   parameters may not be used.</p>
     * 
     * <p>If absent, and the list of allowed parameters is non-empty,
     *   any (possibly empty) subset of that list may be
     *   used.</p>
     * 
     * <p>If absent, and the list of allowed parameters is empty,
     *   any parameters may be used.</p>
     */
     ContactInfoFieldFlagParametersExact = 1
};

/**
 * \typedef QFlags<ContactInfoFieldFlag> ContactInfoFieldFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ContactInfoFieldFlag values.
 * 
 * Flags describing the behaviour of a vCard field.
 */
typedef QFlags<ContactInfoFieldFlag> ContactInfoFieldFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ContactInfoFieldFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum MediaStreamPending
{
    /**
     * The local user has been asked to send media by the remote user. Call 
     * RequestStreamDirection to indicate whether or not this is acceptable.
     */
     MediaStreamPendingLocalSend = 1,

    /**
     * The remote user has been asked to send media by the local user. The 
     * StreamDirectionChanged signal will be emitted when the remote user 
     * accepts or rejects this change.
     */
     MediaStreamPendingRemoteSend = 2
};

/**
 * \typedef QFlags<MediaStreamPending> MediaStreamPendingSend
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #MediaStreamPending values.
 */
typedef QFlags<MediaStreamPending> MediaStreamPendingSend;
Q_DECLARE_OPERATORS_FOR_FLAGS(MediaStreamPendingSend)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ChannelMediaCapability
{
    /**
     * The handle is capable of using audio streams within a media channel.
     */
     ChannelMediaCapabilityAudio = 1,

    /**
     * The handle is capable of using video streams within a media channel.
     */
     ChannelMediaCapabilityVideo = 2,

    /**
     * The handle is capable of performing STUN to traverse NATs.
     */
     ChannelMediaCapabilityNATTraversalSTUN = 4,

    /**
     * The handle is capable of establishing Google Talk peer-to-peer 
     * connections (as implemented in libjingle 0.3) to traverse NATs.
     */
     ChannelMediaCapabilityNATTraversalGTalkP2P = 8,

    /**
     * The handle is capable of establishing ICE UDP peer-to-peer connections 
     * (as defined by the IETF MMUSIC working group) to traverse NATs.
     */
     ChannelMediaCapabilityNATTraversalICEUDP = 16,

    /**
     * Channels whose target handle is this contact will have ImmutableStreams 
     * = True.
     */
     ChannelMediaCapabilityImmutableStreams = 32
};

/**
 * \typedef QFlags<ChannelMediaCapability> ChannelMediaCapabilities
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ChannelMediaCapability values.
 * 
 * The channel-type-specific capability flags used for 
 * Channel.Type.StreamedMedia in the Connection.Interface.Capabilities 
 * interface. See the InitialAudio property for details of the mechanisms that 
 * will replace this.
 */
typedef QFlags<ChannelMediaCapability> ChannelMediaCapabilities;
Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelMediaCapabilities)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ChannelTextMessageFlag
{
    /**
     * The incoming message was truncated to a shorter length by the server or 
     * the connection manager.
     */
     ChannelTextMessageFlagTruncated = 1,

    /**
     * <p>The incoming message contained non-text content which cannot be
     *   represented by this interface, but has been signalled
     *   in the <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.Interface">Messages</tp:dbus-ref>
     *   interface.</p>
     * 
     * <p>Connection managers SHOULD only set this flag if the non-text
     *   content appears to be relatively significant (exactly how
     *   significant is up to the implementor). The intention is that
     *   if this flag is set, clients using this interface SHOULD inform
     *   the user that part of the message was not understood.</p>
     */
     ChannelTextMessageFlagNonTextContent = 2,

    /**
     * <p>The incoming message was part of a replay of message history.</p>
     * 
     * <tp:rationale>
     *   <p>In XMPP multi-user chat, a few past messages are replayed
     *     when you join a chatroom. A sufficiently capable IRC connection
     *     manager could also set this flag on historical messages when
     *     connected to a proxy like bip or irssi-proxy. The existence
     *     of this flag allows loggers and UIs to use better heuristics
     *     when eliminating duplicates (a simple implementation made
     *     possible by this flag would be to avoid logging scrollback
     *     at all).</p>
     * </tp:rationale>
     */
     ChannelTextMessageFlagScrollback = 4,

    /**
     * <p>The incoming message has been seen in a previous channel during
     *   the lifetime of the <tp:dbus-ref namespace="org.freedesktop.Telepathy">Connection</tp:dbus-ref>, but
     *   had not been acknowledged
     *   when that channel closed, causing an identical channel (the
     *   channel in which the message now appears) to open.</p>
     * 
     * <tp:rationale>
     *   <p>This means that a logger (which should already have seen the
     *     message in the previous channel) is able to recognise and ignore
     *     these replayed messages.</p>
     * </tp:rationale>
     */
     ChannelTextMessageFlagRescued = 8
};

/**
 * \typedef QFlags<ChannelTextMessageFlag> ChannelTextMessageFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ChannelTextMessageFlag values.
 */
typedef QFlags<ChannelTextMessageFlag> ChannelTextMessageFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelTextMessageFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ChannelCallState
{
    /**
     * The contact has been alerted about the call but has not responded (e.g. 
     * 180 Ringing in SIP).
     */
     ChannelCallStateRinging = 1,

    /**
     * The contact is temporarily unavailable, and the call has been placed in 
     * a queue (e.g. 182 Queued in SIP, or call-waiting in telephony).
     */
     ChannelCallStateQueued = 2,

    /**
     * The contact has placed the call on hold, and will not receive media 
     * from the local user or any other participants until they unhold the 
     * call again.
     */
     ChannelCallStateHeld = 4,

    /**
     * The initiator of the call originally called a contact other than the 
     * current recipient of the call, but the call was then forwarded or 
     * diverted.
     */
     ChannelCallStateForwarded = 8,

    /**
     * Progress has been made in placing the outgoing call, but the 
     * destination contact may not have been made aware of the call yet (so 
     * the Ringing state is not appropriate). This corresponds to SIP&apos;s 
     * status code 183 Session Progress, and could be used when the outgoing 
     * call has reached a gateway, for instance.
     */
     ChannelCallStateInProgress = 16
};

/**
 * \typedef QFlags<ChannelCallState> ChannelCallStateFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ChannelCallState values.
 * 
 * A set of flags representing call states.
 */
typedef QFlags<ChannelCallState> ChannelCallStateFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelCallStateFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ChannelGroupFlag
{
    /**
     * The AddMembers method can be used to add or invite members who are not 
     * already in the local pending list (which is always valid).
     */
     ChannelGroupFlagCanAdd = 1,

    /**
     * The RemoveMembers method can be used to remove channel members 
     * (removing those on the pending local list is always valid).
     */
     ChannelGroupFlagCanRemove = 2,

    /**
     * The RemoveMembers method can be used on people on the remote pending 
     * list.
     */
     ChannelGroupFlagCanRescind = 4,

    /**
     * A message may be sent to the server when calling AddMembers on contacts 
     * who are not currently pending members.
     */
     ChannelGroupFlagMessageAdd = 8,

    /**
     * A message may be sent to the server when calling RemoveMembers on 
     * contacts who are currently channel members.
     */
     ChannelGroupFlagMessageRemove = 16,

    /**
     * A message may be sent to the server when calling AddMembers on contacts 
     * who are locally pending.
     */
     ChannelGroupFlagMessageAccept = 32,

    /**
     * A message may be sent to the server when calling RemoveMembers on 
     * contacts who are locally pending.
     */
     ChannelGroupFlagMessageReject = 64,

    /**
     * A message may be sent to the server when calling RemoveMembers on 
     * contacts who are remote pending.
     */
     ChannelGroupFlagMessageRescind = 128,

    /**
     * <p>
     *   The members of this group have handles which are specific to
     *   this channel, and are not valid as general-purpose handles on
     *   the connection. Depending on the channel, it may be possible to
     *   check the <tp:member-ref>HandleOwners</tp:member-ref> property or
     *   call <tp:member-ref>GetHandleOwners</tp:member-ref> to find the
     *   owners of these handles, which should be done if you wish to (e.g.)
     *   subscribe to the contact's presence.
     * </p>
     * 
     * <p>
     *   Connection managers must ensure that any given handle is not
     *   simultaneously a general-purpose handle and a channel-specific
     *   handle.
     * </p>
     */
     ChannelGroupFlagChannelSpecificHandles = 256,

    /**
     * Placing a contact in multiple groups of this type is not allowed and 
     * will raise NotAvailable (on services where contacts may only be in one 
     * user-defined group, user-defined groups will have this flag).
     */
     ChannelGroupFlagOnlyOneGroup = 512,

    /**
     * In rooms with channel specific handles (ie Channel_Specific_Handles 
     * flag is set), this flag indicates that no handle owners are available, 
     * apart from the owner of the SelfHandle. This used to be an important 
     * optimization to avoid repeated GetHandleOwners calls, before we 
     * introduced the HandleOwners property and HandleOwnersChanged signal.
     */
     ChannelGroupFlagHandleOwnersNotAvailable = 1024,

    /**
     * This flag indicates that all the properties introduced in specification 
     * 0.17.6 are fully supported.
     */
     ChannelGroupFlagProperties = 2048,

    /**
     * Indicates that MembersChangedDetailed will be emitted for changes to 
     * this group&apos;s members in addition to MembersChanged. Clients can 
     * then connect to the former and ignore emission of the latter. This 
     * flag&apos;s state MUST NOT change over the lifetime of a channel. If it 
     * were allowed to change, client bindings would have to always connect to 
     * MembersChanged just in case the flag ever went away (and generally be 
     * unnecessarily complicated), which would mostly negate the point of 
     * having this flag in the first place.
     */
     ChannelGroupFlagMembersChangedDetailed = 4096,

    /**
     * A message may be sent to the server when calling RemoveMembers on the 
     * SelfHandle. This would be set for XMPP Multi-User Chat or IRC channels, 
     * but not for a typical implementation of streamed media calls.
     */
     ChannelGroupFlagMessageDepart = 8192
};

/**
 * \typedef QFlags<ChannelGroupFlag> ChannelGroupFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ChannelGroupFlag values.
 */
typedef QFlags<ChannelGroupFlag> ChannelGroupFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelGroupFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum MessagePartSupportFlag
{
    /**
     * SendMessage will accept messages containing a textual message body, 
     * plus a single attachment of any type listed in the 
     * SupportedContentTypes property. It does not make sense for this flag to 
     * be set if Message_Part_Support_Flag_Data_Only is not also set (because 
     * the connection manager can trivially provide an empty text part if 
     * necessary).
     */
     MessagePartSupportFlagOneAttachment = 1,

    /**
     * SendMessage will accept messages containing a textual message body, 
     * plus an arbitrary number of attachments of any type listed in the 
     * SupportedContentTypes property. It does not make sense for this flag to 
     * be set if Message_Part_Support_Flag_One_Attachment is not also set.
     */
     MessagePartSupportFlagMultipleAttachments = 2
};

/**
 * \typedef QFlags<MessagePartSupportFlag> MessagePartSupportFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #MessagePartSupportFlag values.
 * 
 * <p>Flags indicating the level of support for message parts on this
 *   channel. They are designed such that setting more flags always
 *   implies that the channel has more capabilities.</p>
 * 
 * <p>If no flags are set, this indicates that messages may contain
 *   a single message part whose content-type is any of the types
 *   from SupportedContentTypes, possibly with some alternatives.</p>
 * 
 * <p>There is no flag indicating support for alternatives. This is
 *   because the SendMessage implementation can always accept messages
 *   containing alternatives, even if the underlying protocol does not,
 *   by deleting all alternatives except the first (most preferred)
 *   that is supported.</p>
 * 
 * <tp:rationale>
 *   Each of the flags so far implies the previous flag, so we could
 *   have used a simple enumeration here; however, we've defined
 *   the message-part support indicator as a flag set for future
 *   expansion.
 * </tp:rationale>
 * 
 * <p>See <tp:member-ref>SupportedContentTypes</tp:member-ref> for some
 *   examples.</p>
 */
typedef QFlags<MessagePartSupportFlag> MessagePartSupportFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(MessagePartSupportFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum MessageSendingFlag
{
    /**
     * <p>Provide a successful delivery report if possible, even if this is
     *   not the default for this protocol. Ignored if delivery reports are
     *   not possible on this protocol.</p>
     * 
     * <tp:rationale>
     *   <p>In some protocols, like XMPP, it is not conventional to request
     *     or send positive delivery notifications.</p>
     * </tp:rationale>
     * 
     * <p>Delivery failure reports SHOULD always be sent, but if this flag
     *   is present, the connection manager MAY also try harder to obtain
     *   failed delivery reports or allow them to be matched to outgoing
     *   messages.</p>
     */
     MessageSendingFlagReportDelivery = 1
};

/**
 * \typedef QFlags<MessageSendingFlag> MessageSendingFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #MessageSendingFlag values.
 * 
 * Flags altering the way a message is sent. The &quot;most usual&quot; action 
 * should always be to have these flags unset.
 */
typedef QFlags<MessageSendingFlag> MessageSendingFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(MessageSendingFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum DeliveryReportingSupportFlag
{
    /**
     * Clients MAY expect to receive negative delivery reports if 
     * Message_Sending_Flag_Report_Delivery is specified when sending. If 
     * senders want delivery reports, they should ask for them. If they 
     * don&apos;t want delivery reports, they can just ignore them, so 
     * there&apos;s no need to have capability discovery for what will happen 
     * if a delivery report isn&apos;t requested.
     */
     DeliveryReportingSupportFlagReceiveFailures = 1,

    /**
     * Clients MAY expect to receive positive delivery reports if 
     * Message_Sending_Flag_Report_Delivery is specified when sending. Same 
     * rationale as Receive_Failures.
     */
     DeliveryReportingSupportFlagReceiveSuccesses = 2
};

/**
 * \typedef QFlags<DeliveryReportingSupportFlag> DeliveryReportingSupportFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #DeliveryReportingSupportFlag values.
 * 
 * Flags indicating the level of support for delivery reporting on this 
 * channel. Any future flags added to this set will conform to the convention 
 * that the presence of an extra flag implies that more operations will 
 * succeed.
 */
typedef QFlags<DeliveryReportingSupportFlag> DeliveryReportingSupportFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(DeliveryReportingSupportFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum ChannelPasswordFlag
{
    /**
     * The ProvidePassword method must be called now for the user to join the 
     * channel
     */
     ChannelPasswordFlagProvide = 8
};

/**
 * \typedef QFlags<ChannelPasswordFlag> ChannelPasswordFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #ChannelPasswordFlag values.
 */
typedef QFlags<ChannelPasswordFlag> ChannelPasswordFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelPasswordFlags)

/**
 * \ingroup flagtypeconsts
 *
 * Flag type generated from the specification.
 */
enum PropertyFlag
{
    /**
     * The property can be read
     */
     PropertyFlagRead = 1,

    /**
     * The property can be written
     */
     PropertyFlagWrite = 2
};

/**
 * \typedef QFlags<PropertyFlag> PropertyFlags
 * \ingroup flagtypeconsts
 *
 * Type representing combinations of #PropertyFlag values.
 */
typedef QFlags<PropertyFlag> PropertyFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyFlags)

/**
 * \enum HandleType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum HandleType
{
    /**
     * A &quot;null&quot; handle type used to indicate the absence of a 
     * handle. When a handle type and a handle appear as a pair, if the handle 
     * type is zero, the handle must also be zero.
     */
     HandleTypeNone = 0,

    /**
     * A contact
     */
     HandleTypeContact = 1,

    /**
     * A chat room
     */
     HandleTypeRoom = 2,

    /**
     * A server-generated contact list (see Channel.Interface.Group)
     */
     HandleTypeList = 3,

    /**
     * A user-defined contact list (see Channel.Interface.Group)
     */
     HandleTypeGroup = 4
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of HandleType.
 */
const int NUM_HANDLE_TYPES = (4+1);

/**
 * \enum ConnectionStatus
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum ConnectionStatus
{
    /**
     * The connection is fully connected and all methods are available.
     */
     ConnectionStatusConnected = 0,

    /**
     * Connect has been called but the connection has not yet been 
     * established. Some methods may fail until the connection has been 
     * established.
     */
     ConnectionStatusConnecting = 1,

    /**
     * If this is retrieved from GetStatus or Status, it indicates that 
     * connection has not yet been attempted. If seen in a StatusChanged 
     * signal, it indicates that the connection has failed; the Connection 
     * object SHOULD be removed from D-Bus immediately, and all subsequent 
     * method calls SHOULD fail.
     */
     ConnectionStatusDisconnected = 2
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ConnectionStatus.
 */
const int NUM_CONNECTION_STATUSES = (2+1);

/**
 * \enum ConnectionStatusReason
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 * 
 * <p>A reason why the status of the connection changed. Apart from
 *   Requested, the values of this enumeration only make sense as
 *   reasons why the status changed to Disconnected.</p>
 */
enum ConnectionStatusReason
{
    /**
     * <p>There is no reason set for this state change. Unknown status
     *   reasons SHOULD be treated like this reason.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Disconnected</code>.</p>
     */
     ConnectionStatusReasonNoneSpecified = 0,

    /**
     * <p>The change is in response to a user request. Changes to the
     *   Connecting or Connected status SHOULD always indicate this reason;
     *   changes to the Disconnected status SHOULD indicate this reason
     *   if and only if the disconnection was requested by the user.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cancelled</code>.</p>
     */
     ConnectionStatusReasonRequested = 1,

    /**
     * <p>There was an error sending or receiving on the network socket.</p>
     * 
     * <p>When the status changes from Connecting to Disconnected for this
     *   reason, the equivalent D-Bus error is either
     *   <code>org.freedesktop.Telepathy.Error.NetworkError</code>,
     *   <code>org.freedesktop.Telepathy.Error.ConnectionRefused</code>,
     *   <code>org.freedesktop.Telepathy.Error.ConnectionFailed</code>
     *   or some more specific error.</p>
     * 
     * <p>When the status changes from Connected to Disconnected for this
     *   reason, the equivalent D-Bus error is either
     *   <code>org.freedesktop.Telepathy.Error.NetworkError</code>,
     *   <code>org.freedesktop.Telepathy.Error.ConnectionLost</code>
     *   or some more specific error.</p>
     */
     ConnectionStatusReasonNetworkError = 2,

    /**
     * <p>The username or password was invalid.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.AuthenticationFailed</code>.
     * </p>
     */
     ConnectionStatusReasonAuthenticationFailed = 3,

    /**
     * <p>There was an error negotiating SSL on this connection, or
     *   encryption was unavailable and require-encryption was set when the
     *   connection was created.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.EncryptionNotAvailable</code>
     *   if encryption was not available at all, or
     *   <code>org.freedesktop.Telepathy.Error.EncryptionError</code>
     *   if encryption failed.</p>
     */
     ConnectionStatusReasonEncryptionError = 4,

    /**
     * <p>In general, this reason indicates that the requested account
     *   name or other identification could not be used due to conflict
     *   with another connection. It can be divided into three cases:</p>
     * 
     * <ul>
     *   <li>If the status change is from Connecting to Disconnected
     *     and the 'register' parameter to RequestConnection was present
     *     and true, the requested account could not be created on the
     *     server because it already exists.
     *     The equivalent D-Bus error is
     *     <code>org.freedesktop.Telepathy.Error.RegistrationExists</code>.
     *   </li>
     * 
     *   <li>If the status change is from Connecting to Disconnected
     *     but the 'register' parameter is absent or false, the connection
     *     manager could not connect to the specified account because
     *     a connection to that account already exists.
     *     The equivalent D-Bus error is
     *     <code>org.freedesktop.Telepathy.Error.AlreadyConnected</code>.
     * 
     *     <tp:rationale>
     *       In some protocols, like XMPP (when connecting with the same
     *       JID and resource as an existing connection), the existing
     *       connection &quot;wins&quot; and the new one fails to connect.
     *     </tp:rationale>
     *   </li>
     * 
     *   <li>If the status change is from Connected to Disconnected,
     *     the existing connection was automatically disconnected because
     *     a new connection to the same account (perhaps from a different
     *     client or location) was established.
     *     The equivalent D-Bus error is
     *     <code>org.freedesktop.Telepathy.Error.ConnectionReplaced</code>.
     * 
     *     <tp:rationale>
     *       In some protocols, like MSNP (when connecting twice with the
     *       same Passport), the new connection &quot;wins&quot; and the
     *       existing one is automatically disconnected.
     *     </tp:rationale>
     *   </li>
     * </ul>
     */
     ConnectionStatusReasonNameInUse = 5,

    /**
     * <p>The server did not provide a SSL certificate.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.NotProvided</code>.
     * </p>
     */
     ConnectionStatusReasonCertNotProvided = 6,

    /**
     * <p>The server's SSL certificate is signed by an untrusted certifying
     *   authority. This error SHOULD NOT be used to represent a self-signed
     *   certificate: use the more specific Cert_Self_Signed reason for
     *   that.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.Untrusted</code>.
     * </p>
     */
     ConnectionStatusReasonCertUntrusted = 7,

    /**
     * <p>The server's SSL certificate has expired.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.Expired</code>.
     * </p>
     */
     ConnectionStatusReasonCertExpired = 8,

    /**
     * <p>The server's SSL certificate is not yet valid.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.NotActivated</code>.
     * </p>
     */
     ConnectionStatusReasonCertNotActivated = 9,

    /**
     * <p>The server's SSL certificate did not match its hostname.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.HostnameMismatch</code>.
     * </p>
     */
     ConnectionStatusReasonCertHostnameMismatch = 10,

    /**
     * <p>The server's SSL certificate does not have the expected
     *   fingerprint.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.FingerprintMismatch</code>.
     * </p>
     */
     ConnectionStatusReasonCertFingerprintMismatch = 11,

    /**
     * <p>The server's SSL certificate is self-signed.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.HostnameMismatch</code>.
     * </p>
     */
     ConnectionStatusReasonCertSelfSigned = 12,

    /**
     * <p>There was some other error validating the server's SSL
     *   certificate.</p>
     * 
     * <p>When disconnected for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cert.Invalid</code>.
     * </p>
     */
     ConnectionStatusReasonCertOtherError = 13
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ConnectionStatusReason.
 */
const int NUM_CONNECTION_STATUS_REASONS = (13+1);

/**
 * \enum ConnectionPresenceType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum ConnectionPresenceType
{
    /**
     * An invalid presence type used as a null value. This value MUST NOT 
     * appear in the Statuses property, or in the result of GetStatuses on the 
     * deprecated Presence interface.
     */
     ConnectionPresenceTypeUnset = 0,

    /**
     * Offline
     */
     ConnectionPresenceTypeOffline = 1,

    /**
     * Available
     */
     ConnectionPresenceTypeAvailable = 2,

    /**
     * Away
     */
     ConnectionPresenceTypeAway = 3,

    /**
     * Away for an extended time
     */
     ConnectionPresenceTypeExtendedAway = 4,

    /**
     * Hidden (invisible)
     */
     ConnectionPresenceTypeHidden = 5,

    /**
     * Busy, Do Not Disturb.
     */
     ConnectionPresenceTypeBusy = 6,

    /**
     * Unknown, unable to determine presence for this contact, for example if 
     * the protocol only allows presence of subscribed contacts.
     */
     ConnectionPresenceTypeUnknown = 7,

    /**
     * Error, an error occurred while trying to determine presence. The 
     * message, if set, is an error from the server.
     */
     ConnectionPresenceTypeError = 8
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ConnectionPresenceType.
 */
const int NUM_CONNECTION_PRESENCE_TYPES = (8+1);

/**
 * \enum RichPresenceAccessControlType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 * 
 * A type of access control for Rich_Presence_Access_Control. For most types, 
 * the exact access control is given by an associated variant. These are the 
 * access control types from XMPP publish/subscribe (XEP-0060).
 */
enum RichPresenceAccessControlType
{
    /**
     * The associated variant is a list of contacts (signature &apos;au&apos;, 
     * Contact_Handle[]) who can see the extended presence information.
     */
     RichPresenceAccessControlTypeWhitelist = 0,

    /**
     * All contacts in the user&apos;s &apos;publish&apos; contact list can 
     * see the extended presence information. The associated variant is 
     * ignored.
     */
     RichPresenceAccessControlTypePublishList = 1,

    /**
     * The associated variant is a handle of type Group (signature 
     * &apos;u&apos;, Group_Handle) representing a group of contacts who can 
     * see the extended presence information.
     */
     RichPresenceAccessControlTypeGroup = 2,

    /**
     * Anyone with access to the service can see the extended presence 
     * information.
     */
     RichPresenceAccessControlTypeOpen = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of RichPresenceAccessControlType.
 */
const int NUM_RICH_PRESENCE_ACCESS_CONTROL_TYPES = (3+1);

/**
 * \enum FileTransferState
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum FileTransferState
{
    /**
     * An invalid state type used as a null value. This value MUST NOT appear 
     * in the State property.
     */
     FileTransferStateNone = 0,

    /**
     * The file transfer is waiting to be accepted/closed by the receiver. The 
     * receiver has to call AcceptFile, then wait for the state to change to 
     * Open and check the offset value.
     */
     FileTransferStatePending = 1,

    /**
     * The receiver has accepted the transfer. The sender now has to call 
     * ProvideFile to actually start the transfer. The receiver should now 
     * wait for the state to change to Open and check the offset value.
     */
     FileTransferStateAccepted = 2,

    /**
     * The file transfer is open for traffic.
     */
     FileTransferStateOpen = 3,

    /**
     * The file transfer has been completed successfully.
     */
     FileTransferStateCompleted = 4,

    /**
     * The file transfer has been cancelled.
     */
     FileTransferStateCancelled = 5
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of FileTransferState.
 */
const int NUM_FILE_TRANSFER_STATES = (5+1);

/**
 * \enum FileTransferStateChangeReason
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum FileTransferStateChangeReason
{
    /**
     * No reason was specified.
     */
     FileTransferStateChangeReasonNone = 0,

    /**
     * The change in state was requested.
     */
     FileTransferStateChangeReasonRequested = 1,

    /**
     * The file transfer was cancelled by the local user.
     */
     FileTransferStateChangeReasonLocalStopped = 2,

    /**
     * The file transfer was cancelled by the remote user.
     */
     FileTransferStateChangeReasonRemoteStopped = 3,

    /**
     * The file transfer was cancelled because of a local error.
     */
     FileTransferStateChangeReasonLocalError = 4,

    /**
     * The file transfer was cancelled because of a remote error.
     */
     FileTransferStateChangeReasonRemoteError = 5
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of FileTransferStateChangeReason.
 */
const int NUM_FILE_TRANSFER_STATE_CHANGE_REASONS = (5+1);

/**
 * \enum FileHashType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum FileHashType
{
    /**
     * No hash.
     */
     FileHashTypeNone = 0,

    /**
     * MD5 digest as a string of 32 ASCII hex digits.
     */
     FileHashTypeMD5 = 1,

    /**
     * SHA1 digest as a string of ASCII hex digits.
     */
     FileHashTypeSHA1 = 2,

    /**
     * SHA256 digest as a string of ASCII hex digits.
     */
     FileHashTypeSHA256 = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of FileHashType.
 */
const int NUM_FILE_HASH_TYPES = (3+1);

/**
 * \enum MediaStreamType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum MediaStreamType
{
    /**
     * An audio stream
     */
     MediaStreamTypeAudio = 0,

    /**
     * A video stream
     */
     MediaStreamTypeVideo = 1
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of MediaStreamType.
 */
const int NUM_MEDIA_STREAM_TYPES = (1+1);

/**
 * \enum MediaStreamState
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum MediaStreamState
{
    /**
     * The stream is disconnected.
     */
     MediaStreamStateDisconnected = 0,

    /**
     * The stream is trying to connect.
     */
     MediaStreamStateConnecting = 1,

    /**
     * The stream is connected.
     */
     MediaStreamStateConnected = 2
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of MediaStreamState.
 */
const int NUM_MEDIA_STREAM_STATES = (2+1);

/**
 * \enum MediaStreamDirection
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum MediaStreamDirection
{
    /**
     * Media are not being sent or received
     */
     MediaStreamDirectionNone = 0,

    /**
     * Media are being sent, but not received
     */
     MediaStreamDirectionSend = 1,

    /**
     * Media are being received, but not sent
     */
     MediaStreamDirectionReceive = 2,

    /**
     * Media are being sent and received
     */
     MediaStreamDirectionBidirectional = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of MediaStreamDirection.
 */
const int NUM_MEDIA_STREAM_DIRECTIONS = (3+1);

/**
 * \enum ChannelTextSendError
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum ChannelTextSendError
{
    /**
     * An unknown error occurred
     */
     ChannelTextSendErrorUnknown = 0,

    /**
     * The requested contact was offline
     */
     ChannelTextSendErrorOffline = 1,

    /**
     * The requested contact is not valid
     */
     ChannelTextSendErrorInvalidContact = 2,

    /**
     * The user does not have permission to speak on this channel
     */
     ChannelTextSendErrorPermissionDenied = 3,

    /**
     * The outgoing message was too long and was rejected by the server
     */
     ChannelTextSendErrorTooLong = 4,

    /**
     * The channel doesn&apos;t support sending text messages to the requested 
     * contact
     */
     ChannelTextSendErrorNotImplemented = 5
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ChannelTextSendError.
 */
const int NUM_CHANNEL_TEXT_SEND_ERRORS = (5+1);

/**
 * \enum ChannelTextMessageType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 * 
 * The type of message.
 */
enum ChannelTextMessageType
{
    /**
     * An ordinary chat message. Unknown types SHOULD be treated like this.
     */
     ChannelTextMessageTypeNormal = 0,

    /**
     * An action which might be presented to the user as &quot;* 
     * &lt;sender&gt; &lt;action&gt;&quot;, such as an IRC CTCP ACTION 
     * (typically selected by the &quot;/me&quot; command). For example, the 
     * text of the message might be &quot;drinks more coffee&quot;.
     */
     ChannelTextMessageTypeAction = 1,

    /**
     * A one-off or automated message not necessarily expecting a reply
     */
     ChannelTextMessageTypeNotice = 2,

    /**
     * An automatically-generated reply message.
     */
     ChannelTextMessageTypeAutoReply = 3,

    /**
     * This message type MUST NOT appear unless the channel supports the 
     * DeliveryReporting interface. The message MUST be as defined by the 
     * DeliveryReporting interface.
     */
     ChannelTextMessageTypeDeliveryReport = 4
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ChannelTextMessageType.
 */
const int NUM_CHANNEL_TEXT_MESSAGE_TYPES = (4+1);

/**
 * \enum TubeType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum TubeType
{
    /**
     * <p>The tube is D-Bus tube as described by the
     *   org.freedesktop.Telepathy.Channel.Type.DBusTube interface.</p>
     */
     TubeTypeDBus = 0,

    /**
     * <p>The tube is stream tube as described by the
     *   org.freedesktop.Telepathy.Channel.Type.StreamTube interface.</p>
     */
     TubeTypeStream = 1
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of TubeType.
 */
const int NUM_TUBE_TYPES = (1+1);

/**
 * \enum TubeState
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum TubeState
{
    /**
     * The tube is waiting to be accepted/closed locally.
     */
     TubeStateLocalPending = 0,

    /**
     * The tube is waiting to be accepted/closed remotely.
     */
     TubeStateRemotePending = 1,

    /**
     * The tube is open for traffic.
     */
     TubeStateOpen = 2
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of TubeState.
 */
const int NUM_TUBE_STATES = (2+1);

/**
 * \enum ChannelChatState
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum ChannelChatState
{
    /**
     * The contact has effectively ceased participating in the chat.
     */
     ChannelChatStateGone = 0,

    /**
     * The contact has not been active for some time.
     */
     ChannelChatStateInactive = 1,

    /**
     * The contact is actively participating in the chat.
     */
     ChannelChatStateActive = 2,

    /**
     * The contact has paused composing a message.
     */
     ChannelChatStatePaused = 3,

    /**
     * The contact is composing a message to be sent to the chat.
     */
     ChannelChatStateComposing = 4
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ChannelChatState.
 */
const int NUM_CHANNEL_CHAT_STATES = (4+1);

/**
 * \enum DTMFEvent
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum DTMFEvent
{
    /**
     * 0
     */
     DTMFEventDigit0 = 0,

    /**
     * 1
     */
     DTMFEventDigit1 = 1,

    /**
     * 2
     */
     DTMFEventDigit2 = 2,

    /**
     * 3
     */
     DTMFEventDigit3 = 3,

    /**
     * 4
     */
     DTMFEventDigit4 = 4,

    /**
     * 5
     */
     DTMFEventDigit5 = 5,

    /**
     * 6
     */
     DTMFEventDigit6 = 6,

    /**
     * 7
     */
     DTMFEventDigit7 = 7,

    /**
     * 8
     */
     DTMFEventDigit8 = 8,

    /**
     * 9
     */
     DTMFEventDigit9 = 9,

    /**
     * *
     */
     DTMFEventAsterisk = 10,

    /**
     * #
     */
     DTMFEventHash = 11,

    /**
     * A
     */
     DTMFEventLetterA = 12,

    /**
     * B
     */
     DTMFEventLetterB = 13,

    /**
     * C
     */
     DTMFEventLetterC = 14,

    /**
     * D
     */
     DTMFEventLetterD = 15
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of DTMFEvent.
 */
const int NUM_DTMF_EVENTS = (15+1);

/**
 * \enum ChannelGroupChangeReason
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum ChannelGroupChangeReason
{
    /**
     * <p>No reason was provided for this change.</p>
     * 
     * <p>In particular, this reason SHOULD be used when representing
     *   users joining a named chatroom in the usual way, users leaving
     *   a chatroom by their own request, and normal termination of a
     *   StreamedMedia call by the remote user.</p>
     * 
     * <p>If the <tp:member-ref>SelfHandle</tp:member-ref> is removed from
     *   a group for this reason and the actor is not the SelfHandle, the
     *   equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Terminated</code>.</p>
     * 
     * <p>If the SelfHandle is removed from a group for this reason and
     *   the actor is also the SelfHandle, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Cancelled</code>.</p>
     */
     ChannelGroupChangeReasonNone = 0,

    /**
     * <p>The change is due to a user going offline. Also used when
     *   user is already offline, but this wasn't known previously.</p>
     * 
     * <p>If a one-to-one <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.Type">StreamedMedia</tp:dbus-ref>
     *   call fails because the contact being called is offline, the
     *   connection manager SHOULD indicate this by removing both the
     *   <tp:member-ref>SelfHandle</tp:member-ref> and the other contact's
     *   handle from the Group interface with reason Offline.</p>
     * 
     * <tp:rationale>
     *   For 1-1 calls, the call terminates as a result of removing the
     *   remote contact, so the SelfHandle should be removed at the same
     *   time as the remote contact and for the same reason.
     * </tp:rationale>
     * 
     * <p>If a handle is removed from a group for this reason, the
     *   equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Offline</code>.</p>
     */
     ChannelGroupChangeReasonOffline = 1,

    /**
     * <p>The change is due to a kick operation.</p>
     * 
     * <p>If the <tp:member-ref>SelfHandle</tp:member-ref> is removed
     *   from a group for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Channel.Kicked</code>.
     * </p>
     */
     ChannelGroupChangeReasonKicked = 2,

    /**
     * <p>The change is due to a busy indication.</p>
     * 
     * <p>If a one-to-one <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.Type">StreamedMedia</tp:dbus-ref>
     *   call fails because the contact being called is busy, the
     *   connection manager SHOULD indicate this by removing both the
     *   <tp:member-ref>SelfHandle</tp:member-ref> and the other contact's
     *   handle from the Group interface with reason Busy.</p>
     * 
     * <tp:rationale>
     *   For 1-1 calls, the call terminates as a result of removing the
     *   remote contact, so the SelfHandle should be removed at the same
     *   time as the remote contact and for the same reason.
     * </tp:rationale>
     * 
     * <p>If the <tp:member-ref>SelfHandle</tp:member-ref> is removed
     *   from a group for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Busy</code>.
     * </p>
     */
     ChannelGroupChangeReasonBusy = 3,

    /**
     * The change is due to an invitation. This reason SHOULD only be used 
     * when contacts are added to the remote-pending set (to indicate that the 
     * contact has been invited) or to the members (to indicate that the 
     * contact has accepted the invitation). Otherwise, what would it mean?
     */
     ChannelGroupChangeReasonInvited = 4,

    /**
     * <p>The change is due to a kick+ban operation.</p>
     * 
     * <p>If the <tp:member-ref>SelfHandle</tp:member-ref> is removed
     *   from a group for this reason, the equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.Channel.Banned</code>.
     * </p>
     */
     ChannelGroupChangeReasonBanned = 5,

    /**
     * The change is due to an error occurring.
     */
     ChannelGroupChangeReasonError = 6,

    /**
     * <p>The change is because the requested contact does not exist.</p>
     * 
     * <p>For instance, if the user invites a nonexistent contact to a
     *   chatroom or attempts to call a nonexistent contact, this could
     *   be indicated by the CM adding that contact's handle to
     *   remote-pending for reason None or Invited, then removing it for
     *   reason Invalid_Contact. In the case of a 1-1 StreamedMedia
     *   call, the CM SHOULD remove the self handle from the Group
     *   in the same signal.</p>
     * 
     * <tp:rationale>
     *   For 1-1 calls, the call terminates as a result of removing the
     *   remote contact, so the SelfHandle should be removed at the same
     *   time as the remote contact and for the same reason.
     * </tp:rationale>
     * 
     * <p>If a contact is removed from a group for this reason, the
     *   equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.DoesNotExist</code>.
     * </p>
     */
     ChannelGroupChangeReasonInvalidContact = 7,

    /**
     * <p>The change is because the requested contact did not respond.</p>
     * 
     * <p>If a one-to-one <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.Type">StreamedMedia</tp:dbus-ref>
     *   call fails because the contact being called did not respond, the
     *   connection manager SHOULD indicate this by removing both the
     *   <tp:member-ref>SelfHandle</tp:member-ref> and the other contact's
     *   handle from the Group interface with reason No_Answer.</p>
     * 
     * <tp:rationale>
     *   Documenting existing practice.
     * </tp:rationale>
     * 
     * <p>If a contact is removed from a group for this reason, the
     *   equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.NoAnswer</code>.
     * </p>
     */
     ChannelGroupChangeReasonNoAnswer = 8,

    /**
     * <p>The change is because a contact's unique identifier changed.
     * There must be exactly one handle in the removed set and exactly
     * one handle in one of the added sets. The <tp:dbus-ref namespace="org.freedesktop.Telepathy.Connection.Interface.Renaming">Renamed</tp:dbus-ref>
     * signal on the
     * <tp:dbus-ref namespace="org.freedesktop.Telepathy.Connection.Interface">Renaming</tp:dbus-ref>
     * interface will have been emitted for the same handles,
     * shortly before this <tp:member-ref>MembersChanged</tp:member-ref> signal is emitted.</p>
     */
     ChannelGroupChangeReasonRenamed = 9,

    /**
     * <p>The change is because there was no permission to contact the
     *   requested handle.</p>
     * 
     * <p>If a contact is removed from a group for this reason, the
     *   equivalent D-Bus error is
     *   <code>org.freedesktop.Telepathy.Error.PermissionDenied</code>.
     * </p>
     */
     ChannelGroupChangeReasonPermissionDenied = 10,

    /**
     * <p>If members are removed with this reason code, the change is
     *   because the group has split into unconnected parts which can only
     *   communicate within themselves (e.g. netsplits on IRC use this
     *   reason code).
     * </p>
     * <p>
     *   If members are added with this reason code, the change is because
     *   unconnected parts of the group have rejoined. If this channel
     *   carries messages (e.g. <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.Type">Text</tp:dbus-ref>
     *   or <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.Type">Tubes</tp:dbus-ref>
     *   channels) applications must
     *   assume that the contacts being added are likely to have missed some
     *   messages as a result of the separation, and that the contacts
     *   in the group are likely to have missed some messages from the
     *   contacts being added.
     * </p>
     * <p>Note that from the added contacts' perspective, they have been
     *   in the group all along, and the contacts we indicate to be in
     *   the group (including the local user) have just rejoined
     *   the group with reason Separated. Application protocols in Tubes
     *   should be prepared to cope with this situation.
     * </p>
     * 
     * <p>The <tp:member-ref>SelfHandle</tp:member-ref> SHOULD NOT be
     *   removed from channels with this reason.</p>
     */
     ChannelGroupChangeReasonSeparated = 11
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of ChannelGroupChangeReason.
 */
const int NUM_CHANNEL_GROUP_CHANGE_REASONS = (11+1);

/**
 * \enum LocalHoldState
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 * 
 * The hold state of a channel.
 */
enum LocalHoldState
{
    /**
     * All streams are unheld (the call is active). New channels SHOULD have 
     * this hold state.
     */
     LocalHoldStateUnheld = 0,

    /**
     * All streams are held (the call is on hold)
     */
     LocalHoldStateHeld = 1,

    /**
     * The connection manager is attempting to move to state Held, but has not 
     * yet completed that operation. It is unspecified whether any, all or 
     * none of the streams making up the channel are on hold.
     */
     LocalHoldStatePendingHold = 2,

    /**
     * The connection manager is attempting to move to state Held, but has not 
     * yet completed that operation. It is unspecified whether any, all or 
     * none of the streams making up the channel are on hold.
     */
     LocalHoldStatePendingUnhold = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of LocalHoldState.
 */
const int NUM_LOCAL_HOLD_STATES = (3+1);

/**
 * \enum LocalHoldStateReason
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 * 
 * The reason for a change to the Local_Hold_State. Clients MUST treat unknown 
 * values as equivalent to Local_Hold_State_Reason_None.
 */
enum LocalHoldStateReason
{
    /**
     * The reason cannot be described by any of the predefined values 
     * (connection managers SHOULD avoid this reason, but clients MUST handle 
     * it gracefully)
     */
     LocalHoldStateReasonNone = 0,

    /**
     * The change is in response to a user request
     */
     LocalHoldStateReasonRequested = 1,

    /**
     * The change is because some resource was not available
     */
     LocalHoldStateReasonResourceNotAvailable = 2
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of LocalHoldStateReason.
 */
const int NUM_LOCAL_HOLD_STATE_REASONS = (2+1);

/**
 * \enum DeliveryStatus
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 * 
 * The status of a message as indicated by a delivery report. If this enum is 
 * extended in future specifications, this should only be to add new, 
 * non-overlapping conditions (i.e. all failures should still be signalled as 
 * either Temporarily_Failed or Permanently_Failed). If additional detail is 
 * required (e.g. distinguishing between the various types of permanent 
 * failure) this will be done using additional keys in the Message_Part.
 */
enum DeliveryStatus
{
    /**
     * The message&apos;s disposition is unknown. Clients SHOULD consider all 
     * messages to have status Delivery_Status_Unknown unless otherwise 
     * specified; connection managers SHOULD NOT signal this delivery status 
     * explicitly.
     */
     DeliveryStatusUnknown = 0,

    /**
     * The message has been delivered to the intended recipient.
     */
     DeliveryStatusDelivered = 1,

    /**
     * Delivery of the message has failed. Clients SHOULD notify the user, but 
     * MAY automatically try sending another copy of the message. Similar to 
     * errors with type=&quot;wait&quot; in XMPP; analogous to 4xx errors in 
     * SMTP.
     */
     DeliveryStatusTemporarilyFailed = 2,

    /**
     * Delivery of the message has failed. Clients SHOULD NOT try again unless 
     * by specific user action. If the user does not modify the message or 
     * alter configuration before re-sending, this error is likely to happen 
     * again. Similar to errors with type=&quot;cancel&quot;, 
     * type=&quot;modify&quot; or type=&quot;auth&quot; in XMPP; analogous to 
     * 5xx errors in SMTP.
     */
     DeliveryStatusPermanentlyFailed = 3,

    /**
     * An intermediate server has accepted the message but the message has not 
     * been yet delivered to the ultimate recipient. The connection manager 
     * might send a Failed report or Delivered report later. Similar to 
     * &quot;202 Accepted&quot; success code in SIP; analogous to 251 and 252 
     * responses in SMTP.
     */
     DeliveryStatusAccepted = 4
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of DeliveryStatus.
 */
const int NUM_DELIVERY_STATUSES = (4+1);

/**
 * \enum TubeChannelState
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum TubeChannelState
{
    /**
     * The initiator offered the tube. The tube is waiting to be 
     * accepted/closed locally. If the client accepts the tube, the 
     * tube&apos;s state will be Open.
     */
     TubeChannelStateLocalPending = 0,

    /**
     * The tube is waiting to be accepted/closed remotely. If the recipient 
     * accepts the tube, the tube&apos;s state will be Open.
     */
     TubeChannelStateRemotePending = 1,

    /**
     * The initiator offered the tube and the recipient accepted it. The tube 
     * is open for traffic. The tube&apos;s state stays in this state until it 
     * is closed.
     */
     TubeChannelStateOpen = 2,

    /**
     * The tube channel has been requested but the tube is not yet offered. 
     * The client should offer the tube to the recipient and the tube&apos;s 
     * state will be Remote_Pending. The method used to offer the tube depends 
     * on the tube type.
     */
     TubeChannelStateNotOffered = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of TubeChannelState.
 */
const int NUM_TUBE_CHANNEL_STATES = (3+1);

/**
 * \enum SocketAddressType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum SocketAddressType
{
    /**
     * A Unix socket. The address variant contains a byte-array, signature 
     * &apos;ay&apos;, containing the path of the socket.
     */
     SocketAddressTypeUnix = 0,

    /**
     * An abstract Unix socket. The address variant contains a byte-array, 
     * signature &apos;ay&apos;, containing the path of the socket including 
     * the leading null byte.
     */
     SocketAddressTypeAbstractUnix = 1,

    /**
     * An IPv4 socket. The address variant contains a Socket_Address_IPv4, 
     * i.e. a structure with signature (sq) in which the string is an IPv4 
     * dotted-quad address literal (and must not be a DNS name), while the 
     * 16-bit unsigned integer is the port number.
     */
     SocketAddressTypeIPv4 = 2,

    /**
     * An IPv6 socket. The address variant contains a Socket_Address_IPv6, 
     * i.e. a structure with signature (sq) in which the string is an IPv6 
     * address literal as specified in RFC2373 (and must not be a DNS name), 
     * while the 16-bit unsigned integer is the port number.
     */
     SocketAddressTypeIPv6 = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of SocketAddressType.
 */
const int NUM_SOCKET_ADDRESS_TYPES = (3+1);

/**
 * \enum SocketAccessControl
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum SocketAccessControl
{
    /**
     * The IP or Unix socket can be accessed by any local user (e.g. a Unix 
     * socket that accepts all local connections, or an IP socket listening on 
     * 127.0.0.1 (or ::1) or rejecting connections not from that address). The 
     * associated variant must be ignored.
     */
     SocketAccessControlLocalhost = 0,

    /**
     * May only be used on IP sockets. The associated variant must contain a 
     * struct Socket_Address_IPv4 (or Socket_Address_IPv6) containing the 
     * string form of an IP address of the appropriate version, and a port 
     * number. The socket can only be accessed if the connecting process has 
     * that address and port number; all other connections will be rejected.
     */
     SocketAccessControlPort = 1,

    /**
     * May only be used on IP sockets. The associated variant must contain a 
     * struct Socket_Netmask_IPv4 (or Socket_Netmask_IPv6) with signature 
     * (sy), containing the string form of an IP address of the appropriate 
     * version, and a prefix length &quot;n&quot;. The socket can only be 
     * accessed if the first n bits of the connecting address match the first 
     * n bits of the given address.
     */
     SocketAccessControlNetmask = 2,

    /**
     * <p>May only be used on UNIX sockets.
     *   The connecting process must send a byte when
     *   it first connects, which is not considered to be part of the data
     *   stream. If the operating system uses sendmsg() with SCM_CREDS or
     *   SCM_CREDENTIALS to pass credentials over sockets, the connecting
     *   process must do so if possible; if not, it must still send the
     *   byte.</p>
     * 
     * <p>The listening process will disconnect the connection unless it
     *   can determine by OS-specific means that the connecting process
     *   has the same user ID as the listening process.</p>
     * 
     * <p>The associated variant must be ignored.</p>
     */
     SocketAccessControlCredentials = 3
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of SocketAccessControl.
 */
const int NUM_SOCKET_ACCESS_CONTROLS = (3+1);

/**
 * \enum MediaStreamError
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum MediaStreamError
{
    /**
     * An unknown error occured.
     */
     MediaStreamErrorUnknown = 0,

    /**
     * The end of the stream was reached.
     */
     MediaStreamErrorEOS = 1,

    /**
     * There are no common codecs between the local side and the other 
     * particpants in the call. The possible codecs are not signalled here: 
     * the streaming implementation is assumed to report them in an 
     * implementation-dependent way, e.g. Farsight should use 
     * GstMissingElement.
     */
     MediaStreamErrorCodecNegotiationFailed = 2,

    /**
     * A network connection for the Media could not be established or was lost.
     */
     MediaStreamErrorConnectionFailed = 3,

    /**
     * There was an error in the networking stack (other than the connection 
     * failure).
     */
     MediaStreamErrorNetworkError = 4,

    /**
     * There are no installed codecs for this media type.
     */
     MediaStreamErrorNoCodecs = 5,

    /**
     * The CM is doing something wrong.
     */
     MediaStreamErrorInvalidCMBehavior = 6,

    /**
     * There was an error in the media processing stack.
     */
     MediaStreamErrorMediaError = 7
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of MediaStreamError.
 */
const int NUM_MEDIA_STREAM_ERRORS = (7+1);

/**
 * \enum MediaStreamBaseProto
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum MediaStreamBaseProto
{
    /**
     * UDP (User Datagram Protocol)
     */
     MediaStreamBaseProtoUDP = 0,

    /**
     * TCP (Transmission Control Protocol)
     */
     MediaStreamBaseProtoTCP = 1
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of MediaStreamBaseProto.
 */
const int NUM_MEDIA_STREAM_BASE_PROTOS = (1+1);

/**
 * \enum MediaStreamTransportType
 * \ingroup enumtypeconsts
 *
 * Enumerated type generated from the specification.
 */
enum MediaStreamTransportType
{
    /**
     * A local address
     */
     MediaStreamTransportTypeLocal = 0,

    /**
     * An external address derived by a method such as STUN
     */
     MediaStreamTransportTypeDerived = 1,

    /**
     * An external stream relay
     */
     MediaStreamTransportTypeRelay = 2
};

/**
 * \ingroup enumtypeconsts
 *
 * 1 higher than the highest valid value of MediaStreamTransportType.
 */
const int NUM_MEDIA_STREAM_TRANSPORT_TYPES = (2+1);

}

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.ConnectionManager".
 */
#define TELEPATHY_INTERFACE_CONNECTION_MANAGER "org.freedesktop.Telepathy.ConnectionManager"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection".
 */
#define TELEPATHY_INTERFACE_CONNECTION "org.freedesktop.Telepathy.Connection"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Aliasing".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_ALIASING "org.freedesktop.Telepathy.Connection.Interface.Aliasing"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Avatars".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_AVATARS "org.freedesktop.Telepathy.Connection.Interface.Avatars"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Balance".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_BALANCE "org.freedesktop.Telepathy.Connection.Interface.Balance"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Capabilities".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CAPABILITIES "org.freedesktop.Telepathy.Connection.Interface.Capabilities"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Contacts".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACTS "org.freedesktop.Telepathy.Connection.Interface.Contacts"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.ContactCapabilities".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACT_CAPABILITIES "org.freedesktop.Telepathy.Connection.Interface.ContactCapabilities"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.ContactInfo".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACT_INFO "org.freedesktop.Telepathy.Connection.Interface.ContactInfo"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Location".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_LOCATION "org.freedesktop.Telepathy.Connection.Interface.Location"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Presence".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_PRESENCE "org.freedesktop.Telepathy.Connection.Interface.Presence"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.Requests".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_REQUESTS "org.freedesktop.Telepathy.Connection.Interface.Requests"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Connection.Interface.SimplePresence".
 */
#define TELEPATHY_INTERFACE_CONNECTION_INTERFACE_SIMPLE_PRESENCE "org.freedesktop.Telepathy.Connection.Interface.SimplePresence"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel".
 */
#define TELEPATHY_INTERFACE_CHANNEL "org.freedesktop.Telepathy.Channel"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.ContactList".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_CONTACT_LIST "org.freedesktop.Telepathy.Channel.Type.ContactList"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.FileTransfer".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_FILE_TRANSFER "org.freedesktop.Telepathy.Channel.Type.FileTransfer"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.StreamedMedia".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA "org.freedesktop.Telepathy.Channel.Type.StreamedMedia"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.RoomList".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_ROOM_LIST "org.freedesktop.Telepathy.Channel.Type.RoomList"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.Text".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT "org.freedesktop.Telepathy.Channel.Type.Text"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.Tubes".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_TUBES "org.freedesktop.Telepathy.Channel.Type.Tubes"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Type.StreamTube".
 */
#define TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE "org.freedesktop.Telepathy.Channel.Type.StreamTube"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.CallState".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_CALL_STATE "org.freedesktop.Telepathy.Channel.Interface.CallState"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.ChatState".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_CHAT_STATE "org.freedesktop.Telepathy.Channel.Interface.ChatState"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.DTMF".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_DTMF "org.freedesktop.Telepathy.Channel.Interface.DTMF"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.Group".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_GROUP "org.freedesktop.Telepathy.Channel.Interface.Group"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.Hold".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_HOLD "org.freedesktop.Telepathy.Channel.Interface.Hold"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.MediaSignalling".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_MEDIA_SIGNALLING "org.freedesktop.Telepathy.Channel.Interface.MediaSignalling"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.Messages".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_MESSAGES "org.freedesktop.Telepathy.Channel.Interface.Messages"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.Password".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_PASSWORD "org.freedesktop.Telepathy.Channel.Interface.Password"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Channel.Interface.Tube".
 */
#define TELEPATHY_INTERFACE_CHANNEL_INTERFACE_TUBE "org.freedesktop.Telepathy.Channel.Interface.Tube"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.ChannelDispatcher".
 */
#define TELEPATHY_INTERFACE_CHANNEL_DISPATCHER "org.freedesktop.Telepathy.ChannelDispatcher"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.ChannelDispatchOperation".
 */
#define TELEPATHY_INTERFACE_CHANNEL_DISPATCH_OPERATION "org.freedesktop.Telepathy.ChannelDispatchOperation"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.ChannelRequest".
 */
#define TELEPATHY_INTERFACE_CHANNEL_REQUEST "org.freedesktop.Telepathy.ChannelRequest"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Media.SessionHandler".
 */
#define TELEPATHY_INTERFACE_MEDIA_SESSION_HANDLER "org.freedesktop.Telepathy.Media.SessionHandler"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Media.StreamHandler".
 */
#define TELEPATHY_INTERFACE_MEDIA_STREAM_HANDLER "org.freedesktop.Telepathy.Media.StreamHandler"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.DBus".
 */
#define TELEPATHY_INTERFACE_DBUS_DAEMON "org.freedesktop.DBus"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.DBus.Introspectable".
 */
#define TELEPATHY_INTERFACE_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.DBus.Peer".
 */
#define TELEPATHY_INTERFACE_PEER "org.freedesktop.DBus.Peer"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.DBus.Properties".
 */
#define TELEPATHY_INTERFACE_PROPERTIES "org.freedesktop.DBus.Properties"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Properties".
 */
#define TELEPATHY_INTERFACE_PROPERTIES_INTERFACE "org.freedesktop.Telepathy.Properties"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.AccountManager".
 */
#define TELEPATHY_INTERFACE_ACCOUNT_MANAGER "org.freedesktop.Telepathy.AccountManager"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Account".
 */
#define TELEPATHY_INTERFACE_ACCOUNT "org.freedesktop.Telepathy.Account"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Account.Interface.Avatar".
 */
#define TELEPATHY_INTERFACE_ACCOUNT_INTERFACE_AVATAR "org.freedesktop.Telepathy.Account.Interface.Avatar"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Client".
 */
#define TELEPATHY_INTERFACE_CLIENT "org.freedesktop.Telepathy.Client"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Client.Observer".
 */
#define TELEPATHY_INTERFACE_CLIENT_OBSERVER "org.freedesktop.Telepathy.Client.Observer"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Client.Approver".
 */
#define TELEPATHY_INTERFACE_CLIENT_APPROVER "org.freedesktop.Telepathy.Client.Approver"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Client.Handler".
 */
#define TELEPATHY_INTERFACE_CLIENT_HANDLER "org.freedesktop.Telepathy.Client.Handler"

/**
 * \ingroup ifacestrconsts
 *
 * The interface name "org.freedesktop.Telepathy.Client.Interface.Requests".
 */
#define TELEPATHY_INTERFACE_CLIENT_INTERFACE_REQUESTS "org.freedesktop.Telepathy.Client.Interface.Requests"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.NetworkError".
 * 
 * Raised when there is an error reading from or writing to the network.
 */
#define TELEPATHY_ERROR_NETWORK_ERROR "org.freedesktop.Telepathy.Error.NetworkError"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.NotImplemented".
 * 
 * Raised when the requested method, channel, etc is not available on this 
 * connection.
 */
#define TELEPATHY_ERROR_NOT_IMPLEMENTED "org.freedesktop.Telepathy.Error.NotImplemented"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.InvalidArgument".
 * 
 * Raised when one of the provided arguments is invalid.
 */
#define TELEPATHY_ERROR_INVALID_ARGUMENT "org.freedesktop.Telepathy.Error.InvalidArgument"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.NotAvailable".
 * 
 * Raised when the requested functionality is temporarily unavailable.
 */
#define TELEPATHY_ERROR_NOT_AVAILABLE "org.freedesktop.Telepathy.Error.NotAvailable"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.PermissionDenied".
 * 
 * The user is not permitted to perform the requested operation.
 */
#define TELEPATHY_ERROR_PERMISSION_DENIED "org.freedesktop.Telepathy.Error.PermissionDenied"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Disconnected".
 * 
 * The connection is not currently connected and cannot be used. This error 
 * may also be raised when operations are performed on a Connection for which 
 * StatusChanged has signalled status Disconnected for reason None. The second 
 * usage corresponds to None in the Connection_Status_Reason enum; if a better 
 * reason is available, the corresponding error should be used instead.
 */
#define TELEPATHY_ERROR_DISCONNECTED "org.freedesktop.Telepathy.Error.Disconnected"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.InvalidHandle".
 * 
 * The handle specified is unknown on this channel or connection.
 */
#define TELEPATHY_ERROR_INVALID_HANDLE "org.freedesktop.Telepathy.Error.InvalidHandle"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Channel.Banned".
 * 
 * You are banned from the channel.
 */
#define TELEPATHY_ERROR_CHANNEL_BANNED "org.freedesktop.Telepathy.Error.Channel.Banned"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Channel.Full".
 * 
 * The channel is full.
 */
#define TELEPATHY_ERROR_CHANNEL_FULL "org.freedesktop.Telepathy.Error.Channel.Full"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Channel.InviteOnly".
 * 
 * The requested channel is invite-only.
 */
#define TELEPATHY_ERROR_CHANNEL_INVITE_ONLY "org.freedesktop.Telepathy.Error.Channel.InviteOnly"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.NotYours".
 * 
 * <p>The requested channel or other resource already exists, and another
 *   user interface in this session is responsible for it.</p>
 * 
 * <p>User interfaces SHOULD handle this error unobtrusively, since it
 *   indicates that some other user interface is already processing the
 *   channel.</p>
 */
#define TELEPATHY_ERROR_NOT_YOURS "org.freedesktop.Telepathy.Error.NotYours"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cancelled".
 * 
 * Raised by an ongoing request if it is cancelled by user request before it 
 * has completed, or when operations are performed on an object which the user 
 * has asked to close (for instance, a Connection where the user has called 
 * Disconnect, or a Channel where the user has called Close). The second form 
 * can be used to correspond to the Requested member in the 
 * Connection_Status_Reason enum, or to to represent the situation where 
 * disconnecting a Connection, closing a Channel, etc. has been requested by 
 * the user but this request has not yet been acted on, for instance because 
 * the service will only act on the request when it has finished processing an 
 * event queue.
 */
#define TELEPATHY_ERROR_CANCELLED "org.freedesktop.Telepathy.Error.Cancelled"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.AuthenticationFailed".
 * 
 * Raised when authentication with a service was unsuccessful. This 
 * corresponds to Authentication_Failed in the Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_AUTHENTICATION_FAILED "org.freedesktop.Telepathy.Error.AuthenticationFailed"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.EncryptionNotAvailable".
 * 
 * Raised if a user request insisted that encryption should be used, but 
 * encryption was not actually available. This corresponds to part of 
 * Encryption_Error in the Connection_Status_Reason enum. It&apos;s been 
 * separated into a distinct error here because the two concepts that were 
 * part of EncryptionError seem to be things that could reasonably appear 
 * differently in the UI.
 */
#define TELEPATHY_ERROR_ENCRYPTION_NOT_AVAILABLE "org.freedesktop.Telepathy.Error.EncryptionNotAvailable"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.EncryptionError".
 * 
 * Raised if encryption appears to be available, but could not actually be 
 * used (for instance if SSL/TLS negotiation fails). This corresponds to part 
 * of Encryption_Error in the Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_ENCRYPTION_ERROR "org.freedesktop.Telepathy.Error.EncryptionError"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.NotProvided".
 * 
 * Raised if the server did not provide a SSL/TLS certificate. This error MUST 
 * NOT be used to represent the absence of a client certificate provided by 
 * the Telepathy connection manager. This corresponds to Cert_Not_Provided in 
 * the Connection_Status_Reason enum. That error explicitly applied only to 
 * server SSL certificates, so this one is similarly limited; having the CM 
 * present a client certificate is a possible future feature, but it should 
 * have its own error handling.
 */
#define TELEPATHY_ERROR_CERT_NOT_PROVIDED "org.freedesktop.Telepathy.Error.Cert.NotProvided"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.Untrusted".
 * 
 * Raised if the server provided a SSL/TLS certificate signed by an untrusted 
 * certifying authority. This error SHOULD NOT be used to represent a 
 * self-signed certificate: see the Self Signed error for that. This 
 * corresponds to Cert_Untrusted in the Connection_Status_Reason enum, with a 
 * clarification to avoid ambiguity.
 */
#define TELEPATHY_ERROR_CERT_UNTRUSTED "org.freedesktop.Telepathy.Error.Cert.Untrusted"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.Expired".
 * 
 * Raised if the server provided an expired SSL/TLS certificate. This 
 * corresponds to Cert_Expired in the Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_CERT_EXPIRED "org.freedesktop.Telepathy.Error.Cert.Expired"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.NotActivated".
 * 
 * Raised if the server provided an SSL/TLS certificate that will become valid 
 * at some point in the future. This corresponds to Cert_Not_Activated in the 
 * Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_CERT_NOT_ACTIVATED "org.freedesktop.Telepathy.Error.Cert.NotActivated"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.FingerprintMismatch".
 * 
 * Raised if the server provided an SSL/TLS certificate that did not have the 
 * expected fingerprint. This corresponds to Cert_Fingerprint_Mismatch in the 
 * Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_CERT_FINGERPRINT_MISMATCH "org.freedesktop.Telepathy.Error.Cert.FingerprintMismatch"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.HostnameMismatch".
 * 
 * Raised if the server provided an SSL/TLS certificate that did not match its 
 * hostname. This corresponds to Cert_Hostname_Mismatch in the 
 * Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_CERT_HOSTNAME_MISMATCH "org.freedesktop.Telepathy.Error.Cert.HostnameMismatch"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.SelfSigned".
 * 
 * Raised if the server provided an SSL/TLS certificate that is self-signed 
 * and untrusted. This corresponds to Cert_Hostname_Mismatch in the 
 * Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_CERT_SELF_SIGNED "org.freedesktop.Telepathy.Error.Cert.SelfSigned"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Cert.Invalid".
 * 
 * Raised if the server provided an SSL/TLS certificate that is unacceptable 
 * in some way that does not have a more specific error. This corresponds to 
 * Cert_Other_Error in the Connection_Status_Reason enum.
 */
#define TELEPATHY_ERROR_CERT_INVALID "org.freedesktop.Telepathy.Error.Cert.Invalid"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.NotCapable".
 * 
 * Raised when requested functionality is unavailable due to contact not 
 * having required capabilities.
 */
#define TELEPATHY_ERROR_NOT_CAPABLE "org.freedesktop.Telepathy.Error.NotCapable"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Offline".
 * 
 * Raised when requested functionality is unavailable because a contact is 
 * offline. This corresponds to Offline in the Channel_Group_Change_Reason 
 * enum.
 */
#define TELEPATHY_ERROR_OFFLINE "org.freedesktop.Telepathy.Error.Offline"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Channel.Kicked".
 * 
 * Used to represent a user being ejected from a channel by another user, for 
 * instance being kicked from a chatroom. This corresponds to Kicked in the 
 * Channel_Group_Change_Reason enum.
 */
#define TELEPATHY_ERROR_CHANNEL_KICKED "org.freedesktop.Telepathy.Error.Channel.Kicked"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Busy".
 * 
 * Used to represent a user being removed from a channel because of a 
 * &quot;busy&quot; indication. This error SHOULD NOT be used to represent a 
 * server or other infrastructure being too busy to process a request - for 
 * that, see ServerBusy. This corresponds to Busy in the 
 * Channel_Group_Change_Reason enum.
 */
#define TELEPATHY_ERROR_BUSY "org.freedesktop.Telepathy.Error.Busy"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.NoAnswer".
 * 
 * Used to represent a user being removed from a channel because they did not 
 * respond, e.g. to a StreamedMedia call. This corresponds to No_Answer in the 
 * Channel_Group_Change_Reason enum.
 */
#define TELEPATHY_ERROR_NO_ANSWER "org.freedesktop.Telepathy.Error.NoAnswer"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.DoesNotExist".
 * 
 * Raised when the requested user does not, in fact, exist. This corresponds 
 * to Invalid_Contact in the Channel_Group_Change_Reason enum, but can also be 
 * used to represent other things not existing (like chatrooms, perhaps).
 */
#define TELEPATHY_ERROR_DOES_NOT_EXIST "org.freedesktop.Telepathy.Error.DoesNotExist"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.Terminated".
 * 
 * Raised when a channel is terminated for an unspecified reason. In 
 * particular, this error SHOULD be used whenever normal termination of a 1-1 
 * StreamedMedia call by the remote user is represented as a D-Bus error name. 
 * This corresponds to None in the Channel_Group_Change_Reason enum.
 */
#define TELEPATHY_ERROR_TERMINATED "org.freedesktop.Telepathy.Error.Terminated"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.ConnectionRefused".
 * 
 * Raised when a connection is refused.
 */
#define TELEPATHY_ERROR_CONNECTION_REFUSED "org.freedesktop.Telepathy.Error.ConnectionRefused"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.ConnectionFailed".
 * 
 * Raised when a connection can&apos;t be established.
 */
#define TELEPATHY_ERROR_CONNECTION_FAILED "org.freedesktop.Telepathy.Error.ConnectionFailed"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.ConnectionLost".
 * 
 * Raised when a connection is broken.
 */
#define TELEPATHY_ERROR_CONNECTION_LOST "org.freedesktop.Telepathy.Error.ConnectionLost"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.AlreadyConnected".
 * 
 * Raised when the user attempts to connect to an account but they are already 
 * connected (perhaps from another client or computer), and the protocol or 
 * account settings do not allow this. XMPP can have this behaviour if the 
 * user chooses the same resource in both clients (it is server-dependent 
 * whether the result is AlreadyConnected on the new connection, 
 * ConnectionReplaced on the old connection, or two successful connections).
 */
#define TELEPATHY_ERROR_ALREADY_CONNECTED "org.freedesktop.Telepathy.Error.AlreadyConnected"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.ConnectionReplaced".
 * 
 * Raised by an existing connection to an account if it is replaced by a new 
 * connection (perhaps from another client or computer). In MSNP, when 
 * connecting twice with the same Passport, the new connection 
 * &quot;wins&quot; and the old one is automatically disconnected. XMPP can 
 * also have this behaviour if the user chooses the same resource in two 
 * clients (it is server-dependent whether the result is AlreadyConnected on 
 * the new connection, ConnectionReplaced on the old connection, or two 
 * successful connections).
 */
#define TELEPATHY_ERROR_CONNECTION_REPLACED "org.freedesktop.Telepathy.Error.ConnectionReplaced"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.RegistrationExists".
 * 
 * Raised during in-band registration if the server indicates that the 
 * requested account already exists.
 */
#define TELEPATHY_ERROR_REGISTRATION_EXISTS "org.freedesktop.Telepathy.Error.RegistrationExists"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.ServiceBusy".
 * 
 * Raised if a server or some other piece of infrastructure cannot process
 * the request, e.g. due to resource limitations. Clients MAY try again
 * later.
 * 
 * <tp:rationale>
 *   This is not the same error as Busy, which indicates that a
 *   <em>user</em> is busy.
 * </tp:rationale>
 */
#define TELEPATHY_ERROR_SERVICE_BUSY "org.freedesktop.Telepathy.Error.ServiceBusy"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Error.ResourceUnavailable".
 * 
 * Raised if a request cannot be satisfied because a process local to the user 
 * has insufficient resources. Clients MAY try again later. For instance, the 
 * ChannelDispatcher might raise this error for some or all channel requests 
 * if it has detected that there is not enough free memory.
 */
#define TELEPATHY_ERROR_RESOURCE_UNAVAILABLE "org.freedesktop.Telepathy.Error.ResourceUnavailable"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.DBus.Error.NameHasNoOwner".
 *
 * Raised by the D-Bus daemon when looking up the owner of a well-known name,
 * if no process owns that name.
 *
 * Also used by DBusProxy to indicate that the owner of a well-known name
 * has disappeared (usually indicating that the process owning that name
 * exited or crashed).
 */
#define TELEPATHY_DBUS_ERROR_NAME_HAS_NO_OWNER \
    "org.freedesktop.DBus.Error.NameHasNoOwner"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.DBus.Error.UnknownMethod".
 *
 * Raised by the D-Bus daemon when the method name invoked isn't
 * known by the object you invoked it on.
 */
#define TELEPATHY_DBUS_ERROR_UNKNOWN_METHOD \
    "org.freedesktop.DBus.Error.UnknownMethod"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Qt4.Error.ObjectRemoved".
 */
#define TELEPATHY_QT4_ERROR_OBJECT_REMOVED \
    "org.freedesktop.Telepathy.Qt4.Error.ObjectRemoved"

/**
 * \ingroup errorstrconsts
 *
 * The error name "org.freedesktop.Telepathy.Qt4.Error.Inconsistent".
 */
#define TELEPATHY_QT4_ERROR_INCONSISTENT \
    "org.freedesktop.Telepathy.Qt4.Error.Inconsistent"

/**
 * The well-known bus name of the Account Manager.
 *
 * \see Tp::AccountManager
 */
#define TELEPATHY_ACCOUNT_MANAGER_BUS_NAME \
    "org.freedesktop.Telepathy.AccountManager"

/**
 * The object path of the Account Manager object.
 *
 * \see Tp::AccountManager
 */
#define TELEPATHY_ACCOUNT_MANAGER_OBJECT_PATH \
    "/org/freedesktop/Telepathy/AccountManager"

