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

#ifndef ITYPES_H
#define ITYPES_H

#include <QtCore/QtGlobal>

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtDBus>

namespace DBus
{
    namespace Interfaces
    {
        struct ObjectPathList : public QList<QDBusObjectPath>
        {
            inline ObjectPathList() : QList<QDBusObjectPath>() {}
            inline ObjectPathList(const QList<QDBusObjectPath>& a) : QList<QDBusObjectPath>(a) {}

            inline ObjectPathList& operator=(const QList<QDBusObjectPath>& a)
            {
                *(static_cast<QList<QDBusObjectPath>*>(this)) = a;
                return *this;
            }
        };

        struct UIntList : public QList<uint>
        {
            inline UIntList() : QList<uint>() {}
            inline UIntList(const QList<uint>& a) : QList<uint>(a) {}

            inline UIntList& operator=(const QList<uint>& a)
            {
                *(static_cast<QList<uint>*>(this)) = a;
                return *this;
            }
        };

        struct AliasMap : public QMap<uint, QString>
        {
            inline AliasMap() : QMap<uint, QString>() {}
            inline AliasMap(const QMap<uint, QString>& a) : QMap<uint, QString>(a) {}

            inline AliasMap& operator=(const QMap<uint, QString>& a)
            {
                *(static_cast<QMap<uint, QString>*>(this)) = a;
                return *this;
            }
        };

        struct AliasPair
        {
            uint handle;
            QString alias;
        };

        bool operator==(const AliasPair& v1, const AliasPair& v2);
        inline bool operator!=(const AliasPair& v1, const AliasPair& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const AliasPair& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, AliasPair& val);

        typedef QList<AliasPair> AliasPairList;

        struct Avatar
        {
            QByteArray avatarData;
            QString MIMEType;
        };

        bool operator==(const Avatar& v1, const Avatar& v2);
        inline bool operator!=(const Avatar& v1, const Avatar& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const Avatar& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, Avatar& val);

        struct AvatarTokenMap : public QMap<uint, QString>
        {
            inline AvatarTokenMap() : QMap<uint, QString>() {}
            inline AvatarTokenMap(const QMap<uint, QString>& a) : QMap<uint, QString>(a) {}

            inline AvatarTokenMap& operator=(const QMap<uint, QString>& a)
            {
                *(static_cast<QMap<uint, QString>*>(this)) = a;
                return *this;
            }
        };

        struct CapabilityChange
        {
            uint handle;
            QString channelType;
            uint oldGenericFlags;
            uint newGenericFlags;
            uint oldTypeSpecificFlags;
            uint newTypeSpecificFlags;
        };

        bool operator==(const CapabilityChange& v1, const CapabilityChange& v2);
        inline bool operator!=(const CapabilityChange& v1, const CapabilityChange& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const CapabilityChange& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, CapabilityChange& val);

        typedef QList<CapabilityChange> CapabilityChangeList;

        struct CapabilityPair
        {
            QString channelType;
            uint typeSpecificFlags;
        };

        bool operator==(const CapabilityPair& v1, const CapabilityPair& v2);
        inline bool operator!=(const CapabilityPair& v1, const CapabilityPair& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const CapabilityPair& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, CapabilityPair& val);

        typedef QList<CapabilityPair> CapabilityPairList;

        struct ChannelCallStateMap : public QMap<uint, uint>
        {
            inline ChannelCallStateMap() : QMap<uint, uint>() {}
            inline ChannelCallStateMap(const QMap<uint, uint>& a) : QMap<uint, uint>(a) {}

            inline ChannelCallStateMap& operator=(const QMap<uint, uint>& a)
            {
                *(static_cast<QMap<uint, uint>*>(this)) = a;
                return *this;
            }
        };

        struct ChannelClass : public QMap<QString, QDBusVariant>
        {
            inline ChannelClass() : QMap<QString, QDBusVariant>() {}
            inline ChannelClass(const QMap<QString, QDBusVariant>& a) : QMap<QString, QDBusVariant>(a) {}

            inline ChannelClass& operator=(const QMap<QString, QDBusVariant>& a)
            {
                *(static_cast<QMap<QString, QDBusVariant>*>(this)) = a;
                return *this;
            }
        };

        typedef QList<ChannelClass> ChannelClassList;

        struct ChannelDetails
        {
            QDBusObjectPath channel;
            QVariantMap properties;
        };

        bool operator==(const ChannelDetails& v1, const ChannelDetails& v2);
        inline bool operator!=(const ChannelDetails& v1, const ChannelDetails& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const ChannelDetails& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, ChannelDetails& val);

        typedef QList<ChannelDetails> ChannelDetailsList;

        struct ChannelInfo
        {
            QDBusObjectPath channel;
            QString channelType;
            uint handleType;
            uint handle;
        };

        bool operator==(const ChannelInfo& v1, const ChannelInfo& v2);
        inline bool operator!=(const ChannelInfo& v1, const ChannelInfo& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const ChannelInfo& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, ChannelInfo& val);

        typedef QList<ChannelInfo> ChannelInfoList;

        struct ContactAttributesMap : public QMap<uint, QVariantMap>
        {
            inline ContactAttributesMap() : QMap<uint, QVariantMap>() {}
            inline ContactAttributesMap(const QMap<uint, QVariantMap>& a) : QMap<uint, QVariantMap>(a) {}

            inline ContactAttributesMap& operator=(const QMap<uint, QVariantMap>& a)
            {
                *(static_cast<QMap<uint, QVariantMap>*>(this)) = a;
                return *this;
            }
        };

        struct ContactCapability
        {
            uint handle;
            QString channelType;
            uint genericFlags;
            uint typeSpecificFlags;
        };

        bool operator==(const ContactCapability& v1, const ContactCapability& v2);
        inline bool operator!=(const ContactCapability& v1, const ContactCapability& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const ContactCapability& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, ContactCapability& val);

        typedef QList<ContactCapability> ContactCapabilityList;

        struct ContactInfoField
        {

            QString fieldName;
            QStringList parameters;
            QStringList fieldValue;
        };

        bool operator==(const ContactInfoField& v1, const ContactInfoField& v2);
        inline bool operator!=(const ContactInfoField& v1, const ContactInfoField& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const ContactInfoField& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, ContactInfoField& val);

        typedef QList<ContactInfoField> ContactInfoFieldList;

        struct ContactLocations : public QMap<uint, QVariantMap>
        {
            inline ContactLocations() : QMap<uint, QVariantMap>() {}
            inline ContactLocations(const QMap<uint, QVariantMap>& a) : QMap<uint, QVariantMap>(a) {}

            inline ContactLocations& operator=(const QMap<uint, QVariantMap>& a)
            {
                *(static_cast<QMap<uint, QVariantMap>*>(this)) = a;
                return *this;
            }
        };

        struct CurrencyAmount
        {
            int amount;
            uint scale;
            QString currency;
        };

        bool operator==(const CurrencyAmount& v1, const CurrencyAmount& v2);
        inline bool operator!=(const CurrencyAmount& v1, const CurrencyAmount& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const CurrencyAmount& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, CurrencyAmount& val);

        struct DBusTubeMember
        {
            uint handle;
            QString uniqueName;
        };

        bool operator==(const DBusTubeMember& v1, const DBusTubeMember& v2);
        inline bool operator!=(const DBusTubeMember& v1, const DBusTubeMember& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const DBusTubeMember& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, DBusTubeMember& val);

        typedef QList<DBusTubeMember> DBusTubeMemberList;

        struct FieldSpec
        {
            QString name;
            QStringList parameters;
            uint flags;
            uint max;
        };

        bool operator==(const FieldSpec& v1, const FieldSpec& v2);
        inline bool operator!=(const FieldSpec& v1, const FieldSpec& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const FieldSpec& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, FieldSpec& val);

        typedef QList<FieldSpec> FieldSpecs;

        struct HandleIdentifierMap : public QMap<uint, QString>
        {
            inline HandleIdentifierMap() : QMap<uint, QString>() {}
            inline HandleIdentifierMap(const QMap<uint, QString>& a) : QMap<uint, QString>(a) {}

            inline HandleIdentifierMap& operator=(const QMap<uint, QString>& a)
            {
                *(static_cast<QMap<uint, QString>*>(this)) = a;
                return *this;
            }
        };

        struct HandleOwnerMap : public QMap<uint, uint>
        {
            inline HandleOwnerMap() : QMap<uint, uint>() {}
            inline HandleOwnerMap(const QMap<uint, uint>& a) : QMap<uint, uint>(a) {}

            inline HandleOwnerMap& operator=(const QMap<uint, uint>& a)
            {
                *(static_cast<QMap<uint, uint>*>(this)) = a;
                return *this;
            }
        };

        struct LocalPendingInfo
        {
            uint toBeAdded;
            uint actor;
            uint reason;
            QString message;
        };

        bool operator==(const LocalPendingInfo& v1, const LocalPendingInfo& v2);
        inline bool operator!=(const LocalPendingInfo& v1, const LocalPendingInfo& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const LocalPendingInfo& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, LocalPendingInfo& val);

        typedef QList<LocalPendingInfo> LocalPendingInfoList;

        struct Location : public QMap<QString, QDBusVariant>
        {
            inline Location() : QMap<QString, QDBusVariant>() {}
            inline Location(const QMap<QString, QDBusVariant>& a) : QMap<QString, QDBusVariant>(a) {}

            inline Location& operator=(const QMap<QString, QDBusVariant>& a)
            {
                *(static_cast<QMap<QString, QDBusVariant>*>(this)) = a;
                return *this;
            }
        };

        struct MediaSessionHandlerInfo
        {
            QDBusObjectPath sessionHandler;
            QString mediaSessionType;
        };

        bool operator==(const MediaSessionHandlerInfo& v1, const MediaSessionHandlerInfo& v2);
        inline bool operator!=(const MediaSessionHandlerInfo& v1, const MediaSessionHandlerInfo& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const MediaSessionHandlerInfo& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaSessionHandlerInfo& val);

        typedef QList<MediaSessionHandlerInfo> MediaSessionHandlerInfoList;

        struct MediaStreamHandlerTransport
        {
            uint componentNumber;
            QString IPAddress;
            uint port;
            uint protocol;
            QString subtype;
            QString profile;
            double preferenceValue;
            uint transportType;
            QString username;
            QString password;
        };

        bool operator==(const MediaStreamHandlerTransport& v1, const MediaStreamHandlerTransport& v2);
        inline bool operator!=(const MediaStreamHandlerTransport& v1, const MediaStreamHandlerTransport& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerTransport& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerTransport& val);

        typedef QList<MediaStreamHandlerTransport> MediaStreamHandlerTransportList;

        struct MediaStreamInfo
        {
            uint identifier;
            uint contact;
            uint type;
            uint state;
            uint direction;
            uint pendingSendFlags;
        };

        bool operator==(const MediaStreamInfo& v1, const MediaStreamInfo& v2);
        inline bool operator!=(const MediaStreamInfo& v1, const MediaStreamInfo& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamInfo& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamInfo& val);

        typedef QList<MediaStreamInfo> MediaStreamInfoList;

        struct MessagePart : public QMap<QString, QDBusVariant>
        {
            inline MessagePart() : QMap<QString, QDBusVariant>() {}
            inline MessagePart(const QMap<QString, QDBusVariant>& a) : QMap<QString, QDBusVariant>(a) {}

            inline MessagePart& operator=(const QMap<QString, QDBusVariant>& a)
            {
                *(static_cast<QMap<QString, QDBusVariant>*>(this)) = a;
                return *this;
            }
        };

        typedef QList<MessagePart> MessagePartList;

        typedef QList<MessagePartList> MessagePartListList;

        struct MessagePartContentMap : public QMap<uint, QDBusVariant>
        {
            inline MessagePartContentMap() : QMap<uint, QDBusVariant>() {}
            inline MessagePartContentMap(const QMap<uint, QDBusVariant>& a) : QMap<uint, QDBusVariant>(a) {}

            inline MessagePartContentMap& operator=(const QMap<uint, QDBusVariant>& a)
            {
                *(static_cast<QMap<uint, QDBusVariant>*>(this)) = a;
                return *this;
            }
        };

        struct MultipleStatusMap : public QMap<QString, QVariantMap>
        {
            inline MultipleStatusMap() : QMap<QString, QVariantMap>() {}
            inline MultipleStatusMap(const QMap<QString, QVariantMap>& a) : QMap<QString, QVariantMap>(a) {}

            inline MultipleStatusMap& operator=(const QMap<QString, QVariantMap>& a)
            {
                *(static_cast<QMap<QString, QVariantMap>*>(this)) = a;
                return *this;
            }
        };

        struct ParamSpec
        {
            QString name;
            uint flags;
            QString signature;
            QDBusVariant defaultValue;
        };

        bool operator==(const ParamSpec& v1, const ParamSpec& v2);
        inline bool operator!=(const ParamSpec& v1, const ParamSpec& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const ParamSpec& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, ParamSpec& val);

        typedef QList<ParamSpec> ParamSpecList;

        struct PendingTextMessage
        {
            uint identifier;
            uint unixTimestamp;
            uint sender;
            uint messageType;
            uint flags;
            QString text;
        };

        bool operator==(const PendingTextMessage& v1, const PendingTextMessage& v2);
        inline bool operator!=(const PendingTextMessage& v1, const PendingTextMessage& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const PendingTextMessage& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, PendingTextMessage& val);

        typedef QList<PendingTextMessage> PendingTextMessageList;

        struct PropertyFlagsChange
        {
            uint propertyID;
            uint newFlags;
        };

        bool operator==(const PropertyFlagsChange& v1, const PropertyFlagsChange& v2);
        inline bool operator!=(const PropertyFlagsChange& v1, const PropertyFlagsChange& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const PropertyFlagsChange& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, PropertyFlagsChange& val);

        typedef QList<PropertyFlagsChange> PropertyFlagsChangeList;

        struct PropertySpec
        {
            uint propertyID;
            QString name;
            QString signature;
            uint flags;
        };

        bool operator==(const PropertySpec& v1, const PropertySpec& v2);
        inline bool operator!=(const PropertySpec& v1, const PropertySpec& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const PropertySpec& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, PropertySpec& val);

        typedef QList<PropertySpec> PropertySpecList;

        struct PropertyValue
        {
            uint identifier;
            QDBusVariant value;
        };

        bool operator==(const PropertyValue& v1, const PropertyValue& v2);
        inline bool operator!=(const PropertyValue& v1, const PropertyValue& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const PropertyValue& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, PropertyValue& val);

        typedef QList<PropertyValue> PropertyValueList;

        struct QualifiedPropertyValueMap : public QMap<QString, QDBusVariant>
        {
            inline QualifiedPropertyValueMap() : QMap<QString, QDBusVariant>() {}
            inline QualifiedPropertyValueMap(const QMap<QString, QDBusVariant>& a) : QMap<QString, QDBusVariant>(a) {}

            inline QualifiedPropertyValueMap& operator=(const QMap<QString, QDBusVariant>& a)
            {
                *(static_cast<QMap<QString, QDBusVariant>*>(this)) = a;
                return *this;
            }
        };

        typedef QList<QualifiedPropertyValueMap> QualifiedPropertyValueMapList;

        struct RequestableChannelClass
        {
            QVariantMap fixedProperties;
            QStringList allowedProperties;
        };

        bool operator==(const RequestableChannelClass& v1, const RequestableChannelClass& v2);
        inline bool operator!=(const RequestableChannelClass& v1, const RequestableChannelClass& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const RequestableChannelClass& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, RequestableChannelClass& val);

        typedef QList<RequestableChannelClass> RequestableChannelClassList;

        struct RichPresenceAccessControl
        {
            uint type;
            QDBusVariant detail;
        };

        bool operator==(const RichPresenceAccessControl& v1, const RichPresenceAccessControl& v2);
        inline bool operator!=(const RichPresenceAccessControl& v1, const RichPresenceAccessControl& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const RichPresenceAccessControl& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, RichPresenceAccessControl& val);

        struct RoomInfo
        {
            uint handle;
            QString channelType;
            QVariantMap info;
        };

        bool operator==(const RoomInfo& v1, const RoomInfo& v2);
        inline bool operator!=(const RoomInfo& v1, const RoomInfo& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const RoomInfo& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, RoomInfo& val);

        typedef QList<RoomInfo> RoomInfoList;

        struct SimplePresence
        {
            uint type;
            QString status;
            QString statusMessage;
        };

        bool operator==(const SimplePresence& v1, const SimplePresence& v2);
        inline bool operator!=(const SimplePresence& v1, const SimplePresence& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SimplePresence& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SimplePresence& val);

        struct SimpleStatusSpec
        {
            uint type;
            bool maySetOnSelf;
            bool canHaveMessage;
        };

        bool operator==(const SimpleStatusSpec& v1, const SimpleStatusSpec& v2);
        inline bool operator!=(const SimpleStatusSpec& v1, const SimpleStatusSpec& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SimpleStatusSpec& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SimpleStatusSpec& val);

        struct SingleContactAttributesMap : public QMap<QString, QDBusVariant>
        {
            inline SingleContactAttributesMap() : QMap<QString, QDBusVariant>() {}
            inline SingleContactAttributesMap(const QMap<QString, QDBusVariant>& a) : QMap<QString, QDBusVariant>(a) {}

            inline SingleContactAttributesMap& operator=(const QMap<QString, QDBusVariant>& a)
            {
                *(static_cast<QMap<QString, QDBusVariant>*>(this)) = a;
                return *this;
            }
        };

        struct SocketAddressIP
        {
            QString address;
            ushort port;
        };

        bool operator==(const SocketAddressIP& v1, const SocketAddressIP& v2);
        inline bool operator!=(const SocketAddressIP& v1, const SocketAddressIP& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIP& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIP& val);

        typedef QList<SocketAddressIP> SocketAddressIPList;

        struct SocketAddressIPv4
        {
            QString address;
            ushort port;
        };

        bool operator==(const SocketAddressIPv4& v1, const SocketAddressIPv4& v2);
        inline bool operator!=(const SocketAddressIPv4& v1, const SocketAddressIPv4& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIPv4& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIPv4& val);

        struct SocketAddressIPv6
        {
            QString address;
            ushort port;
        };

        bool operator==(const SocketAddressIPv6& v1, const SocketAddressIPv6& v2);
        inline bool operator!=(const SocketAddressIPv6& v1, const SocketAddressIPv6& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIPv6& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIPv6& val);

        struct SocketNetmaskIPv4
        {
            QString address;
            uchar prefixLength;
        };

        bool operator==(const SocketNetmaskIPv4& v1, const SocketNetmaskIPv4& v2);
        inline bool operator!=(const SocketNetmaskIPv4& v1, const SocketNetmaskIPv4& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SocketNetmaskIPv4& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketNetmaskIPv4& val);

        struct SocketNetmaskIPv6
        {
            QString address;
            uchar prefixLength;
        };

        bool operator==(const SocketNetmaskIPv6& v1, const SocketNetmaskIPv6& v2);
        inline bool operator!=(const SocketNetmaskIPv6& v1, const SocketNetmaskIPv6& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const SocketNetmaskIPv6& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, SocketNetmaskIPv6& val);

        struct StringStringMap : public QMap<QString, QString>
        {
            inline StringStringMap() : QMap<QString, QString>() {}
            inline StringStringMap(const QMap<QString, QString>& a) : QMap<QString, QString>(a) {}

            inline StringStringMap& operator=(const QMap<QString, QString>& a)
            {
                *(static_cast<QMap<QString, QString>*>(this)) = a;
                return *this;
            }
        };

        typedef QList<StringStringMap> StringStringMapList;

        struct StringVariantMap : public QMap<QString, QDBusVariant>
        {
            inline StringVariantMap() : QMap<QString, QDBusVariant>() {}
            inline StringVariantMap(const QMap<QString, QDBusVariant>& a) : QMap<QString, QDBusVariant>(a) {}

            inline StringVariantMap& operator=(const QMap<QString, QDBusVariant>& a)
            {
                *(static_cast<QMap<QString, QDBusVariant>*>(this)) = a;
                return *this;
            }

            inline bool operator==(const QMap<QString, QDBusVariant>& a)
            {
                return (&a == this);
            }
        };

        typedef QList<StringVariantMap> StringVariantMapList;

        struct SupportedSocketMap : public QMap<uint, UIntList>
        {
            inline SupportedSocketMap() : QMap<uint, UIntList>() {}
            inline SupportedSocketMap(const QMap<uint, UIntList>& a) : QMap<uint, UIntList>(a) {}

            inline SupportedSocketMap& operator=(const QMap<uint, UIntList>& a)
            {
                *(static_cast<QMap<uint, UIntList>*>(this)) = a;
                return *this;
            }
        };

        struct TubeInfo
        {
            uint identifier;
            uint initiator;
            uint type;
            QString service;
            QVariantMap parameters;
            uint state;
        };

        bool operator==(const TubeInfo& v1, const TubeInfo& v2);
        inline bool operator!=(const TubeInfo& v1, const TubeInfo& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const TubeInfo& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, TubeInfo& val);

        typedef QList<TubeInfo> TubeInfoList;

        struct ContactCapabilitiesMap : public QMap<uint, RequestableChannelClassList>
        {
            inline ContactCapabilitiesMap() : QMap<uint, RequestableChannelClassList>() {}
            inline ContactCapabilitiesMap(const QMap<uint, RequestableChannelClassList>& a) : QMap<uint, RequestableChannelClassList>(a) {}

            inline ContactCapabilitiesMap& operator=(const QMap<uint, RequestableChannelClassList>& a)
            {
                *(static_cast<QMap<uint, RequestableChannelClassList>*>(this)) = a;
                return *this;
            }
        };

        typedef QList<ContactCapabilitiesMap> ContactCapabilitiesMapList;

        struct ContactInfoMap : public QMap<uint, ContactInfoFieldList>
        {
            inline ContactInfoMap() : QMap<uint, ContactInfoFieldList>() {}
            inline ContactInfoMap(const QMap<uint, ContactInfoFieldList>& a) : QMap<uint, ContactInfoFieldList>(a) {}

            inline ContactInfoMap& operator=(const QMap<uint, ContactInfoFieldList>& a)
            {
                *(static_cast<QMap<uint, ContactInfoFieldList>*>(this)) = a;
                return *this;
            }
        };

        struct HandlerCapabilities
        {

            QString wellKnownName;
            StringVariantMapList channelClasses;
            QStringList capabilities;
        };

        bool operator==(const HandlerCapabilities& v1, const HandlerCapabilities& v2);
        inline bool operator!=(const HandlerCapabilities& v1, const HandlerCapabilities& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const HandlerCapabilities& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, HandlerCapabilities& val);

        typedef QList<HandlerCapabilities> HandlerCapabilitiesList;

        struct LastActivityAndStatuses
        {
            uint lastActivity;
            MultipleStatusMap statuses;
        };

        bool operator==(const LastActivityAndStatuses& v1, const LastActivityAndStatuses& v2);
        inline bool operator!=(const LastActivityAndStatuses& v1, const LastActivityAndStatuses& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const LastActivityAndStatuses& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, LastActivityAndStatuses& val);

        struct MediaStreamHandlerCandidate
        {
            QString name;
            MediaStreamHandlerTransportList transports;
        };

        bool operator==(const MediaStreamHandlerCandidate& v1, const MediaStreamHandlerCandidate& v2);
        inline bool operator!=(const MediaStreamHandlerCandidate& v1, const MediaStreamHandlerCandidate& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerCandidate& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerCandidate& val);

        typedef QList<MediaStreamHandlerCandidate> MediaStreamHandlerCandidateList;

        struct MediaStreamHandlerCodec
        {
            uint codecID;
            QString name;
            uint mediaType;
            uint clockRate;
            uint numberOfChannels;
            StringStringMap parameters;
        };

        bool operator==(const MediaStreamHandlerCodec& v1, const MediaStreamHandlerCodec& v2);
        inline bool operator!=(const MediaStreamHandlerCodec& v1, const MediaStreamHandlerCodec& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerCodec& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerCodec& val);

        typedef QList<MediaStreamHandlerCodec> MediaStreamHandlerCodecList;

        struct SimpleContactPresences : public QMap<uint, SimplePresence>
        {
            inline SimpleContactPresences() : QMap<uint, SimplePresence>() {}
            inline SimpleContactPresences(const QMap<uint, SimplePresence>& a) : QMap<uint, SimplePresence>(a) {}

            inline SimpleContactPresences& operator=(const QMap<uint, SimplePresence>& a)
            {
                *(static_cast<QMap<uint, SimplePresence>*>(this)) = a;
                return *this;
            }
        };

        struct SimpleStatusSpecMap : public QMap<QString, SimpleStatusSpec>
        {
            inline SimpleStatusSpecMap() : QMap<QString, SimpleStatusSpec>() {}
            inline SimpleStatusSpecMap(const QMap<QString, SimpleStatusSpec>& a) : QMap<QString, SimpleStatusSpec>(a) {}

            inline SimpleStatusSpecMap& operator=(const QMap<QString, SimpleStatusSpec>& a)
            {
                *(static_cast<QMap<QString, SimpleStatusSpec>*>(this)) = a;
                return *this;
            }
        };

        struct StatusSpec
        {
            uint type;
            bool maySetOnSelf;
            bool exclusive;
            StringStringMap parameterTypes;
        };

        bool operator==(const StatusSpec& v1, const StatusSpec& v2);
        inline bool operator!=(const StatusSpec& v1, const StatusSpec& v2)
        {
            return !operator==(v1, v2);
        }
        QDBusArgument& operator<<(QDBusArgument& arg, const StatusSpec& val);
        const QDBusArgument& operator>>(const QDBusArgument& arg, StatusSpec& val);

        struct ContactPresences : public QMap<uint, LastActivityAndStatuses>
        {
            inline ContactPresences() : QMap<uint, LastActivityAndStatuses>() {}
            inline ContactPresences(const QMap<uint, LastActivityAndStatuses>& a) : QMap<uint, LastActivityAndStatuses>(a) {}

            inline ContactPresences& operator=(const QMap<uint, LastActivityAndStatuses>& a)
            {
                *(static_cast<QMap<uint, LastActivityAndStatuses>*>(this)) = a;
                return *this;
            }
        };

        struct StatusSpecMap : public QMap<QString, StatusSpec>
        {
            inline StatusSpecMap() : QMap<QString, StatusSpec>() {}
            inline StatusSpecMap(const QMap<QString, StatusSpec>& a) : QMap<QString, StatusSpec>(a) {}

            inline StatusSpecMap& operator=(const QMap<QString, StatusSpec>& a)
            {
                *(static_cast<QMap<QString, StatusSpec>*>(this)) = a;
                return *this;
            }
        };

        void _registerTypes();
    }//Interfaces

} // namespace DBus

Q_DECLARE_METATYPE(DBus::Interfaces::AliasMap)
Q_DECLARE_METATYPE(DBus::Interfaces::AliasPair)
Q_DECLARE_METATYPE(DBus::Interfaces::AliasPairList)
Q_DECLARE_METATYPE(DBus::Interfaces::Avatar)
Q_DECLARE_METATYPE(DBus::Interfaces::AvatarTokenMap)
Q_DECLARE_METATYPE(DBus::Interfaces::CapabilityChange)
Q_DECLARE_METATYPE(DBus::Interfaces::CapabilityChangeList)
Q_DECLARE_METATYPE(DBus::Interfaces::CapabilityPair)
Q_DECLARE_METATYPE(DBus::Interfaces::CapabilityPairList)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelCallStateMap)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelClass)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelClassList)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelDetails)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelDetailsList)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelInfo)
Q_DECLARE_METATYPE(DBus::Interfaces::ChannelInfoList)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactAttributesMap)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactCapabilitiesMap)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactCapabilitiesMapList)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactCapability)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactCapabilityList)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactInfoField)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactInfoFieldList)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactInfoMap)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactLocations)
Q_DECLARE_METATYPE(DBus::Interfaces::ContactPresences)
Q_DECLARE_METATYPE(DBus::Interfaces::CurrencyAmount)
Q_DECLARE_METATYPE(DBus::Interfaces::DBusTubeMember)
Q_DECLARE_METATYPE(DBus::Interfaces::DBusTubeMemberList)
Q_DECLARE_METATYPE(DBus::Interfaces::FieldSpec)
Q_DECLARE_METATYPE(DBus::Interfaces::FieldSpecs)
Q_DECLARE_METATYPE(DBus::Interfaces::HandleIdentifierMap)
Q_DECLARE_METATYPE(DBus::Interfaces::HandleOwnerMap)
Q_DECLARE_METATYPE(DBus::Interfaces::HandlerCapabilities)
Q_DECLARE_METATYPE(DBus::Interfaces::HandlerCapabilitiesList)
Q_DECLARE_METATYPE(DBus::Interfaces::LastActivityAndStatuses)
Q_DECLARE_METATYPE(DBus::Interfaces::LocalPendingInfo)
Q_DECLARE_METATYPE(DBus::Interfaces::LocalPendingInfoList)
Q_DECLARE_METATYPE(DBus::Interfaces::Location)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaSessionHandlerInfo)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaSessionHandlerInfoList)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamHandlerCandidate)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamHandlerCandidateList)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamHandlerCodec)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamHandlerCodecList)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamHandlerTransport)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamHandlerTransportList)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamInfo)
Q_DECLARE_METATYPE(DBus::Interfaces::MediaStreamInfoList)
Q_DECLARE_METATYPE(DBus::Interfaces::MessagePart)
Q_DECLARE_METATYPE(DBus::Interfaces::MessagePartContentMap)
Q_DECLARE_METATYPE(DBus::Interfaces::MessagePartList)
Q_DECLARE_METATYPE(DBus::Interfaces::MessagePartListList)
Q_DECLARE_METATYPE(DBus::Interfaces::MultipleStatusMap)
Q_DECLARE_METATYPE(DBus::Interfaces::ObjectPathList)
Q_DECLARE_METATYPE(DBus::Interfaces::ParamSpec)
Q_DECLARE_METATYPE(DBus::Interfaces::ParamSpecList)
Q_DECLARE_METATYPE(DBus::Interfaces::PendingTextMessage)
Q_DECLARE_METATYPE(DBus::Interfaces::PendingTextMessageList)
Q_DECLARE_METATYPE(DBus::Interfaces::PropertyFlagsChange)
Q_DECLARE_METATYPE(DBus::Interfaces::PropertyFlagsChangeList)
Q_DECLARE_METATYPE(DBus::Interfaces::PropertySpec)
Q_DECLARE_METATYPE(DBus::Interfaces::PropertySpecList)
Q_DECLARE_METATYPE(DBus::Interfaces::PropertyValue)
Q_DECLARE_METATYPE(DBus::Interfaces::PropertyValueList)
Q_DECLARE_METATYPE(DBus::Interfaces::QualifiedPropertyValueMap)
Q_DECLARE_METATYPE(DBus::Interfaces::QualifiedPropertyValueMapList)
Q_DECLARE_METATYPE(DBus::Interfaces::RequestableChannelClass)
Q_DECLARE_METATYPE(DBus::Interfaces::RequestableChannelClassList)
Q_DECLARE_METATYPE(DBus::Interfaces::RichPresenceAccessControl)
Q_DECLARE_METATYPE(DBus::Interfaces::RoomInfo)
Q_DECLARE_METATYPE(DBus::Interfaces::RoomInfoList)
Q_DECLARE_METATYPE(DBus::Interfaces::SimpleContactPresences)
Q_DECLARE_METATYPE(DBus::Interfaces::SimplePresence)
Q_DECLARE_METATYPE(DBus::Interfaces::SimpleStatusSpec)
Q_DECLARE_METATYPE(DBus::Interfaces::SimpleStatusSpecMap)
Q_DECLARE_METATYPE(DBus::Interfaces::SingleContactAttributesMap)
Q_DECLARE_METATYPE(DBus::Interfaces::SocketAddressIP)
Q_DECLARE_METATYPE(DBus::Interfaces::SocketAddressIPList)
Q_DECLARE_METATYPE(DBus::Interfaces::SocketAddressIPv4)
Q_DECLARE_METATYPE(DBus::Interfaces::SocketAddressIPv6)
Q_DECLARE_METATYPE(DBus::Interfaces::SocketNetmaskIPv4)
Q_DECLARE_METATYPE(DBus::Interfaces::SocketNetmaskIPv6)
Q_DECLARE_METATYPE(DBus::Interfaces::StatusSpec)
Q_DECLARE_METATYPE(DBus::Interfaces::StatusSpecMap)
Q_DECLARE_METATYPE(DBus::Interfaces::StringStringMap)
Q_DECLARE_METATYPE(DBus::Interfaces::StringStringMapList)
Q_DECLARE_METATYPE(DBus::Interfaces::StringVariantMap)
Q_DECLARE_METATYPE(DBus::Interfaces::StringVariantMapList)
Q_DECLARE_METATYPE(DBus::Interfaces::SupportedSocketMap)
Q_DECLARE_METATYPE(DBus::Interfaces::TubeInfo)
Q_DECLARE_METATYPE(DBus::Interfaces::TubeInfoList)
Q_DECLARE_METATYPE(DBus::Interfaces::UIntList)

#endif //ITYPES_H
