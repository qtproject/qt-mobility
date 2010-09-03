#include "itypes.h"

namespace DBus
{
    namespace Interfaces
    {
        bool operator==(const AliasPair& v1, const AliasPair& v2)
        {
            return ((v1.handle == v2.handle)
                    && (v1.alias == v2.alias)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const AliasPair& val)
        {
            arg.beginStructure();
            arg << val.handle << val.alias;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, AliasPair& val)
        {
            arg.beginStructure();
            arg >> val.handle >> val.alias;
            arg.endStructure();
            return arg;
        }

        bool operator==(const Avatar& v1, const Avatar& v2)
        {
            return ((v1.avatarData == v2.avatarData)
                    && (v1.MIMEType == v2.MIMEType)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const Avatar& val)
        {
            arg.beginStructure();
            arg << val.avatarData << val.MIMEType;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, Avatar& val)
        {
            arg.beginStructure();
            arg >> val.avatarData >> val.MIMEType;
            arg.endStructure();
            return arg;
        }

        bool operator==(const CapabilityChange& v1, const CapabilityChange& v2)
        {
            return ((v1.handle == v2.handle)
                    && (v1.channelType == v2.channelType)
                    && (v1.oldGenericFlags == v2.oldGenericFlags)
                    && (v1.newGenericFlags == v2.newGenericFlags)
                    && (v1.oldTypeSpecificFlags == v2.oldTypeSpecificFlags)
                    && (v1.newTypeSpecificFlags == v2.newTypeSpecificFlags)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const CapabilityChange& val)
        {
            arg.beginStructure();
            arg << val.handle << val.channelType << val.oldGenericFlags << val.newGenericFlags << val.oldTypeSpecificFlags << val.newTypeSpecificFlags;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, CapabilityChange& val)
        {
            arg.beginStructure();
            arg >> val.handle >> val.channelType >> val.oldGenericFlags >> val.newGenericFlags >> val.oldTypeSpecificFlags >> val.newTypeSpecificFlags;
            arg.endStructure();
            return arg;
        }

        bool operator==(const CapabilityPair& v1, const CapabilityPair& v2)
        {
            return ((v1.channelType == v2.channelType)
                    && (v1.typeSpecificFlags == v2.typeSpecificFlags)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const CapabilityPair& val)
        {
            arg.beginStructure();
            arg << val.channelType << val.typeSpecificFlags;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, CapabilityPair& val)
        {
            arg.beginStructure();
            arg >> val.channelType >> val.typeSpecificFlags;
            arg.endStructure();
            return arg;
        }

        bool operator==(const ChannelDetails& v1, const ChannelDetails& v2)
        {
            return ((v1.channel == v2.channel)
                    && (v1.properties == v2.properties)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const ChannelDetails& val)
        {
            arg.beginStructure();
            arg << val.channel << val.properties;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, ChannelDetails& val)
        {
            arg.beginStructure();
            arg >> val.channel >> val.properties;
            arg.endStructure();
            return arg;
        }

        bool operator==(const ChannelInfo& v1, const ChannelInfo& v2)
        {
            return ((v1.channel == v2.channel)
                    && (v1.channelType == v2.channelType)
                    && (v1.handleType == v2.handleType)
                    && (v1.handle == v2.handle)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const ChannelInfo& val)
        {
            arg.beginStructure();
            arg << val.channel << val.channelType << val.handleType << val.handle;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, ChannelInfo& val)
        {
            arg.beginStructure();
            arg >> val.channel >> val.channelType >> val.handleType >> val.handle;
            arg.endStructure();
            return arg;
        }

        bool operator==(const ContactCapability& v1, const ContactCapability& v2)
        {
            return ((v1.handle == v2.handle)
                    && (v1.channelType == v2.channelType)
                    && (v1.genericFlags == v2.genericFlags)
                    && (v1.typeSpecificFlags == v2.typeSpecificFlags)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const ContactCapability& val)
        {
            arg.beginStructure();
            arg << val.handle << val.channelType << val.genericFlags << val.typeSpecificFlags;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, ContactCapability& val)
        {
            arg.beginStructure();
            arg >> val.handle >> val.channelType >> val.genericFlags >> val.typeSpecificFlags;
            arg.endStructure();
            return arg;
        }

        bool operator==(const ContactInfoField& v1, const ContactInfoField& v2)
        {
            return ((v1.fieldName == v2.fieldName)
                    && (v1.parameters == v2.parameters)
                    && (v1.fieldValue == v2.fieldValue)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const ContactInfoField& val)
        {
            arg.beginStructure();
            arg << val.fieldName << val.parameters << val.fieldValue;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, ContactInfoField& val)
        {
            arg.beginStructure();
            arg >> val.fieldName >> val.parameters >> val.fieldValue;
            arg.endStructure();
            return arg;
        }

        bool operator==(const CurrencyAmount& v1, const CurrencyAmount& v2)
        {
            return ((v1.amount == v2.amount)
                    && (v1.scale == v2.scale)
                    && (v1.currency == v2.currency)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const CurrencyAmount& val)
        {
            arg.beginStructure();
            arg << val.amount << val.scale << val.currency;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, CurrencyAmount& val)
        {
            arg.beginStructure();
            arg >> val.amount >> val.scale >> val.currency;
            arg.endStructure();
            return arg;
        }

        bool operator==(const DBusTubeMember& v1, const DBusTubeMember& v2)
        {
            return ((v1.handle == v2.handle)
                    && (v1.uniqueName == v2.uniqueName)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const DBusTubeMember& val)
        {
            arg.beginStructure();
            arg << val.handle << val.uniqueName;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, DBusTubeMember& val)
        {
            arg.beginStructure();
            arg >> val.handle >> val.uniqueName;
            arg.endStructure();
            return arg;
        }

        bool operator==(const FieldSpec& v1, const FieldSpec& v2)
        {
            return ((v1.name == v2.name)
                    && (v1.parameters == v2.parameters)
                    && (v1.flags == v2.flags)
                    && (v1.max == v2.max)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const FieldSpec& val)
        {
            arg.beginStructure();
            arg << val.name << val.parameters << val.flags << val.max;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, FieldSpec& val)
        {
            arg.beginStructure();
            arg >> val.name >> val.parameters >> val.flags >> val.max;
            arg.endStructure();
            return arg;
        }

        bool operator==(const LocalPendingInfo& v1, const LocalPendingInfo& v2)
        {
            return ((v1.toBeAdded == v2.toBeAdded)
                    && (v1.actor == v2.actor)
                    && (v1.reason == v2.reason)
                    && (v1.message == v2.message)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const LocalPendingInfo& val)
        {
            arg.beginStructure();
            arg << val.toBeAdded << val.actor << val.reason << val.message;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, LocalPendingInfo& val)
        {
            arg.beginStructure();
            arg >> val.toBeAdded >> val.actor >> val.reason >> val.message;
            arg.endStructure();
            return arg;
        }

        bool operator==(const MediaSessionHandlerInfo& v1, const MediaSessionHandlerInfo& v2)
        {
            return ((v1.sessionHandler == v2.sessionHandler)
                    && (v1.mediaSessionType == v2.mediaSessionType)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const MediaSessionHandlerInfo& val)
        {
            arg.beginStructure();
            arg << val.sessionHandler << val.mediaSessionType;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaSessionHandlerInfo& val)
        {
            arg.beginStructure();
            arg >> val.sessionHandler >> val.mediaSessionType;
            arg.endStructure();
            return arg;
        }

        bool operator==(const MediaStreamHandlerTransport& v1, const MediaStreamHandlerTransport& v2)
        {
            return ((v1.componentNumber == v2.componentNumber)
                    && (v1.IPAddress == v2.IPAddress)
                    && (v1.port == v2.port)
                    && (v1.protocol == v2.protocol)
                    && (v1.subtype == v2.subtype)
                    && (v1.profile == v2.profile)
                    && (v1.preferenceValue == v2.preferenceValue)
                    && (v1.transportType == v2.transportType)
                    && (v1.username == v2.username)
                    && (v1.password == v2.password)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerTransport& val)
        {
            arg.beginStructure();
            arg << val.componentNumber << val.IPAddress << val.port << val.protocol << val.subtype << val.profile << val.preferenceValue << val.transportType << val.username << val.password;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerTransport& val)
        {
            arg.beginStructure();
            arg >> val.componentNumber >> val.IPAddress >> val.port >> val.protocol >> val.subtype >> val.profile >> val.preferenceValue >> val.transportType >> val.username >> val.password;
            arg.endStructure();
            return arg;
        }

        bool operator==(const MediaStreamInfo& v1, const MediaStreamInfo& v2)
        {
            return ((v1.identifier == v2.identifier)
                    && (v1.contact == v2.contact)
                    && (v1.type == v2.type)
                    && (v1.state == v2.state)
                    && (v1.direction == v2.direction)
                    && (v1.pendingSendFlags == v2.pendingSendFlags)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamInfo& val)
        {
            arg.beginStructure();
            arg << val.identifier << val.contact << val.type << val.state << val.direction << val.pendingSendFlags;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamInfo& val)
        {
            arg.beginStructure();
            arg >> val.identifier >> val.contact >> val.type >> val.state >> val.direction >> val.pendingSendFlags;
            arg.endStructure();
            return arg;
        }

        bool operator==(const ParamSpec& v1, const ParamSpec& v2)
        {
            return ((v1.name == v2.name)
                    && (v1.flags == v2.flags)
                    && (v1.signature == v2.signature)
                    && (v1.defaultValue.variant() == v2.defaultValue.variant())
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const ParamSpec& val)
        {
            arg.beginStructure();
            arg << val.name << val.flags << val.signature << val.defaultValue;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, ParamSpec& val)
        {
            arg.beginStructure();
            arg >> val.name >> val.flags >> val.signature >> val.defaultValue;
            arg.endStructure();
            return arg;
        }

        bool operator==(const PendingTextMessage& v1, const PendingTextMessage& v2)
        {
            return ((v1.identifier == v2.identifier)
                    && (v1.unixTimestamp == v2.unixTimestamp)
                    && (v1.sender == v2.sender)
                    && (v1.messageType == v2.messageType)
                    && (v1.flags == v2.flags)
                    && (v1.text == v2.text)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const PendingTextMessage& val)
        {
            arg.beginStructure();
            arg << val.identifier << val.unixTimestamp << val.sender << val.messageType << val.flags << val.text;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, PendingTextMessage& val)
        {
            arg.beginStructure();
            arg >> val.identifier >> val.unixTimestamp >> val.sender >> val.messageType >> val.flags >> val.text;
            arg.endStructure();
            return arg;
        }

        bool operator==(const PropertyFlagsChange& v1, const PropertyFlagsChange& v2)
        {
            return ((v1.propertyID == v2.propertyID)
                    && (v1.newFlags == v2.newFlags)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const PropertyFlagsChange& val)
        {
            arg.beginStructure();
            arg << val.propertyID << val.newFlags;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, PropertyFlagsChange& val)
        {
            arg.beginStructure();
            arg >> val.propertyID >> val.newFlags;
            arg.endStructure();
            return arg;
        }

        bool operator==(const PropertySpec& v1, const PropertySpec& v2)
        {
            return ((v1.propertyID == v2.propertyID)
                    && (v1.name == v2.name)
                    && (v1.signature == v2.signature)
                    && (v1.flags == v2.flags)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const PropertySpec& val)
        {
            arg.beginStructure();
            arg << val.propertyID << val.name << val.signature << val.flags;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, PropertySpec& val)
        {
            arg.beginStructure();
            arg >> val.propertyID >> val.name >> val.signature >> val.flags;
            arg.endStructure();
            return arg;
        }

        bool operator==(const PropertyValue& v1, const PropertyValue& v2)
        {
            return ((v1.identifier == v2.identifier)
                    && (v1.value.variant() == v2.value.variant())
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const PropertyValue& val)
        {
            arg.beginStructure();
            arg << val.identifier << val.value;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, PropertyValue& val)
        {
            arg.beginStructure();
            arg >> val.identifier >> val.value;
            arg.endStructure();
            return arg;
        }

        bool operator==(const RequestableChannelClass& v1, const RequestableChannelClass& v2)
        {
            return ((v1.fixedProperties == v2.fixedProperties)
                    && (v1.allowedProperties == v2.allowedProperties)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const RequestableChannelClass& val)
        {
            arg.beginStructure();
            arg << val.fixedProperties << val.allowedProperties;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, RequestableChannelClass& val)
        {
            arg.beginStructure();
            arg >> val.fixedProperties >> val.allowedProperties;
            arg.endStructure();
            return arg;
        }

        bool operator==(const RichPresenceAccessControl& v1, const RichPresenceAccessControl& v2)
        {
            return ((v1.type == v2.type)
                    && (v1.detail.variant() == v2.detail.variant())
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const RichPresenceAccessControl& val)
        {
            arg.beginStructure();
            arg << val.type << val.detail;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, RichPresenceAccessControl& val)
        {
            arg.beginStructure();
            arg >> val.type >> val.detail;
            arg.endStructure();
            return arg;
        }

        bool operator==(const RoomInfo& v1, const RoomInfo& v2)
        {
            return ((v1.handle == v2.handle)
                    && (v1.channelType == v2.channelType)
                    && (v1.info == v2.info)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const RoomInfo& val)
        {
            arg.beginStructure();
            arg << val.handle << val.channelType << val.info;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, RoomInfo& val)
        {
            arg.beginStructure();
            arg >> val.handle >> val.channelType >> val.info;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SimplePresence& v1, const SimplePresence& v2)
        {
            return ((v1.type == v2.type)
                    && (v1.status == v2.status)
                    && (v1.statusMessage == v2.statusMessage)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SimplePresence& val)
        {
            arg.beginStructure();
            arg << val.type << val.status << val.statusMessage;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SimplePresence& val)
        {
            arg.beginStructure();
            arg >> val.type >> val.status >> val.statusMessage;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SimpleStatusSpec& v1, const SimpleStatusSpec& v2)
        {
            return ((v1.type == v2.type)
                    && (v1.maySetOnSelf == v2.maySetOnSelf)
                    && (v1.canHaveMessage == v2.canHaveMessage)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SimpleStatusSpec& val)
        {
            arg.beginStructure();
            arg << val.type << val.maySetOnSelf << val.canHaveMessage;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SimpleStatusSpec& val)
        {
            arg.beginStructure();
            arg >> val.type >> val.maySetOnSelf >> val.canHaveMessage;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SocketAddressIP& v1, const SocketAddressIP& v2)
        {
            return ((v1.address == v2.address)
                    && (v1.port == v2.port)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIP& val)
        {
            arg.beginStructure();
            arg << val.address << val.port;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIP& val)
        {
            arg.beginStructure();
            arg >> val.address >> val.port;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SocketAddressIPv4& v1, const SocketAddressIPv4& v2)
        {
            return ((v1.address == v2.address)
                    && (v1.port == v2.port)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIPv4& val)
        {
            arg.beginStructure();
            arg << val.address << val.port;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIPv4& val)
        {
            arg.beginStructure();
            arg >> val.address >> val.port;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SocketAddressIPv6& v1, const SocketAddressIPv6& v2)
        {
            return ((v1.address == v2.address)
                    && (v1.port == v2.port)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIPv6& val)
        {
            arg.beginStructure();
            arg << val.address << val.port;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIPv6& val)
        {
            arg.beginStructure();
            arg >> val.address >> val.port;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SocketNetmaskIPv4& v1, const SocketNetmaskIPv4& v2)
        {
            return ((v1.address == v2.address)
                    && (v1.prefixLength == v2.prefixLength)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SocketNetmaskIPv4& val)
        {
            arg.beginStructure();
            arg << val.address << val.prefixLength;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketNetmaskIPv4& val)
        {
            arg.beginStructure();
            arg >> val.address >> val.prefixLength;
            arg.endStructure();
            return arg;
        }

        bool operator==(const SocketNetmaskIPv6& v1, const SocketNetmaskIPv6& v2)
        {
            return ((v1.address == v2.address)
                    && (v1.prefixLength == v2.prefixLength)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const SocketNetmaskIPv6& val)
        {
            arg.beginStructure();
            arg << val.address << val.prefixLength;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketNetmaskIPv6& val)
        {
            arg.beginStructure();
            arg >> val.address >> val.prefixLength;
            arg.endStructure();
            return arg;
        }

        bool operator==(const TubeInfo& v1, const TubeInfo& v2)
        {
            return ((v1.identifier == v2.identifier)
                    && (v1.initiator == v2.initiator)
                    && (v1.type == v2.type)
                    && (v1.service == v2.service)
                    && (v1.parameters == v2.parameters)
                    && (v1.state == v2.state)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const TubeInfo& val)
        {
            arg.beginStructure();
            arg << val.identifier << val.initiator << val.type << val.service << val.parameters << val.state;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, TubeInfo& val)
        {
            arg.beginStructure();
            arg >> val.identifier >> val.initiator >> val.type >> val.service >> val.parameters >> val.state;
            arg.endStructure();
            return arg;
        }

        bool operator==(const HandlerCapabilities& v1, const HandlerCapabilities& v2)
        {
            return ((v1.wellKnownName == v2.wellKnownName)
                    && (v1.channelClasses == v2.channelClasses)
                    && (v1.capabilities == v2.capabilities)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const HandlerCapabilities& val)
        {
            arg.beginStructure();
            arg << val.wellKnownName << val.channelClasses << val.capabilities;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, HandlerCapabilities& val)
        {
            arg.beginStructure();
            arg >> val.wellKnownName >> val.channelClasses >> val.capabilities;
            arg.endStructure();
            return arg;
        }

        bool operator==(const LastActivityAndStatuses& v1, const LastActivityAndStatuses& v2)
        {
            return ((v1.lastActivity == v2.lastActivity)
                    && (v1.statuses == v2.statuses)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const LastActivityAndStatuses& val)
        {
            arg.beginStructure();
            arg << val.lastActivity << val.statuses;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, LastActivityAndStatuses& val)
        {
            arg.beginStructure();
            arg >> val.lastActivity >> val.statuses;
            arg.endStructure();
            return arg;
        }

        bool operator==(const MediaStreamHandlerCandidate& v1, const MediaStreamHandlerCandidate& v2)
        {
            return ((v1.name == v2.name)
                    && (v1.transports == v2.transports)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerCandidate& val)
        {
            arg.beginStructure();
            arg << val.name << val.transports;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerCandidate& val)
        {
            arg.beginStructure();
            arg >> val.name >> val.transports;
            arg.endStructure();
            return arg;
        }

        bool operator==(const MediaStreamHandlerCodec& v1, const MediaStreamHandlerCodec& v2)
        {
            return ((v1.codecID == v2.codecID)
                    && (v1.name == v2.name)
                    && (v1.mediaType == v2.mediaType)
                    && (v1.clockRate == v2.clockRate)
                    && (v1.numberOfChannels == v2.numberOfChannels)
                    && (v1.parameters == v2.parameters)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerCodec& val)
        {
            arg.beginStructure();
            arg << val.codecID << val.name << val.mediaType << val.clockRate << val.numberOfChannels << val.parameters;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerCodec& val)
        {
            arg.beginStructure();
            arg >> val.codecID >> val.name >> val.mediaType >> val.clockRate >> val.numberOfChannels >> val.parameters;
            arg.endStructure();
            return arg;
        }

        bool operator==(const StatusSpec& v1, const StatusSpec& v2)
        {
            return ((v1.type == v2.type)
                    && (v1.maySetOnSelf == v2.maySetOnSelf)
                    && (v1.exclusive == v2.exclusive)
                    && (v1.parameterTypes == v2.parameterTypes)
                    );
        }

        QDBusArgument& operator<<(QDBusArgument& arg, const StatusSpec& val)
        {
            arg.beginStructure();
            arg << val.type << val.maySetOnSelf << val.exclusive << val.parameterTypes;
            arg.endStructure();
            return arg;
        }

        const QDBusArgument& operator>>(const QDBusArgument& arg, StatusSpec& val)
        {
            arg.beginStructure();
            arg >> val.type >> val.maySetOnSelf >> val.exclusive >> val.parameterTypes;
            arg.endStructure();
            return arg;
        }

        void _registerTypes()
        {
            static bool registered = false;
            if (registered)
                return;
            registered = true;

            qDBusRegisterMetaType<DBus::Interfaces::AliasMap>();
            qDBusRegisterMetaType<DBus::Interfaces::AliasPair>();
            qDBusRegisterMetaType<DBus::Interfaces::AliasPairList>();
            qDBusRegisterMetaType<DBus::Interfaces::Avatar>();
            qDBusRegisterMetaType<DBus::Interfaces::AvatarTokenMap>();
            qDBusRegisterMetaType<DBus::Interfaces::CapabilityChange>();
            qDBusRegisterMetaType<DBus::Interfaces::CapabilityChangeList>();
            qDBusRegisterMetaType<DBus::Interfaces::CapabilityPair>();
            qDBusRegisterMetaType<DBus::Interfaces::CapabilityPairList>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelCallStateMap>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelClass>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelClassList>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelDetails>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelDetailsList>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelInfo>();
            qDBusRegisterMetaType<DBus::Interfaces::ChannelInfoList>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactAttributesMap>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactCapabilitiesMap>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactCapabilitiesMapList>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactCapability>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactCapabilityList>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactInfoField>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactInfoFieldList>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactInfoMap>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactLocations>();
            qDBusRegisterMetaType<DBus::Interfaces::ContactPresences>();
            qDBusRegisterMetaType<DBus::Interfaces::CurrencyAmount>();
            qDBusRegisterMetaType<DBus::Interfaces::DBusTubeMember>();
            qDBusRegisterMetaType<DBus::Interfaces::DBusTubeMemberList>();
            qDBusRegisterMetaType<DBus::Interfaces::FieldSpec>();
            qDBusRegisterMetaType<DBus::Interfaces::FieldSpecs>();
            qDBusRegisterMetaType<DBus::Interfaces::HandleIdentifierMap>();
            qDBusRegisterMetaType<DBus::Interfaces::HandleOwnerMap>();
            qDBusRegisterMetaType<DBus::Interfaces::HandlerCapabilities>();
            qDBusRegisterMetaType<DBus::Interfaces::HandlerCapabilitiesList>();
            qDBusRegisterMetaType<DBus::Interfaces::LastActivityAndStatuses>();
            qDBusRegisterMetaType<DBus::Interfaces::LocalPendingInfo>();
            qDBusRegisterMetaType<DBus::Interfaces::LocalPendingInfoList>();
            qDBusRegisterMetaType<DBus::Interfaces::Location>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaSessionHandlerInfo>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaSessionHandlerInfoList>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamHandlerCandidate>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamHandlerCandidateList>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamHandlerCodec>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamHandlerCodecList>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamHandlerTransport>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamHandlerTransportList>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamInfo>();
            qDBusRegisterMetaType<DBus::Interfaces::MediaStreamInfoList>();
            qDBusRegisterMetaType<DBus::Interfaces::MessagePart>();
            qDBusRegisterMetaType<DBus::Interfaces::MessagePartContentMap>();
            qDBusRegisterMetaType<DBus::Interfaces::MessagePartList>();
            qDBusRegisterMetaType<DBus::Interfaces::MessagePartListList>();
            qDBusRegisterMetaType<DBus::Interfaces::MultipleStatusMap>();
            qDBusRegisterMetaType<DBus::Interfaces::ObjectPathList>();
            qDBusRegisterMetaType<DBus::Interfaces::ParamSpec>();
            qDBusRegisterMetaType<DBus::Interfaces::ParamSpecList>();
            qDBusRegisterMetaType<DBus::Interfaces::PendingTextMessage>();
            qDBusRegisterMetaType<DBus::Interfaces::PendingTextMessageList>();
            qDBusRegisterMetaType<DBus::Interfaces::PropertyFlagsChange>();
            qDBusRegisterMetaType<DBus::Interfaces::PropertyFlagsChangeList>();
            qDBusRegisterMetaType<DBus::Interfaces::PropertySpec>();
            qDBusRegisterMetaType<DBus::Interfaces::PropertySpecList>();
            qDBusRegisterMetaType<DBus::Interfaces::PropertyValue>();
            qDBusRegisterMetaType<DBus::Interfaces::PropertyValueList>();
            qDBusRegisterMetaType<DBus::Interfaces::QualifiedPropertyValueMap>();
            qDBusRegisterMetaType<DBus::Interfaces::QualifiedPropertyValueMapList>();
            qDBusRegisterMetaType<DBus::Interfaces::RequestableChannelClass>();
            qDBusRegisterMetaType<DBus::Interfaces::RequestableChannelClassList>();
            qDBusRegisterMetaType<DBus::Interfaces::RichPresenceAccessControl>();
            qDBusRegisterMetaType<DBus::Interfaces::RoomInfo>();
            qDBusRegisterMetaType<DBus::Interfaces::RoomInfoList>();
            qDBusRegisterMetaType<DBus::Interfaces::SimpleContactPresences>();
            qDBusRegisterMetaType<DBus::Interfaces::SimplePresence>();
            qDBusRegisterMetaType<DBus::Interfaces::SimpleStatusSpec>();
            qDBusRegisterMetaType<DBus::Interfaces::SimpleStatusSpecMap>();
            qDBusRegisterMetaType<DBus::Interfaces::SingleContactAttributesMap>();
            qDBusRegisterMetaType<DBus::Interfaces::SocketAddressIP>();
            qDBusRegisterMetaType<DBus::Interfaces::SocketAddressIPList>();
            qDBusRegisterMetaType<DBus::Interfaces::SocketAddressIPv4>();
            qDBusRegisterMetaType<DBus::Interfaces::SocketAddressIPv6>();
            qDBusRegisterMetaType<DBus::Interfaces::SocketNetmaskIPv4>();
            qDBusRegisterMetaType<DBus::Interfaces::SocketNetmaskIPv6>();
            qDBusRegisterMetaType<DBus::Interfaces::StatusSpec>();
            qDBusRegisterMetaType<DBus::Interfaces::StatusSpecMap>();
            qDBusRegisterMetaType<DBus::Interfaces::StringStringMap>();
            qDBusRegisterMetaType<DBus::Interfaces::StringStringMapList>();
            qDBusRegisterMetaType<DBus::Interfaces::StringVariantMap>();
            qDBusRegisterMetaType<DBus::Interfaces::StringVariantMapList>();
            qDBusRegisterMetaType<DBus::Interfaces::SupportedSocketMap>();
            qDBusRegisterMetaType<DBus::Interfaces::TubeInfo>();
            qDBusRegisterMetaType<DBus::Interfaces::TubeInfoList>();
            qDBusRegisterMetaType<DBus::Interfaces::UIntList>();
        }
    }//Interfaces
}//DBus
