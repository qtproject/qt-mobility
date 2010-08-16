/* Generated from Telepathy D-Bus Interface Specification, TelepathyQt4 copy, version 0.17.7 */

#ifndef CLI_TYPES_H
#define CLI_TYPES_H

#include <QtCore/QtGlobal>

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusSignature>
#include <QtDBus/QDBusVariant>


/**
 * \addtogroup typesconstants Types and constants
 *
 * Enumerated, flag, structure, list and mapping types and utility constants.
 */

/**
 * \defgroup struct Structure types
 * \ingroup typesconstants
 *
 * Structure types generated from the specification.
 */

/**
 * \defgroup list List types
 * \ingroup typesconstants
 *
 * List types generated from the specification.
 */

/**
 * \defgroup mapping Mapping types
 * \ingroup typesconstants
 *
 * Mapping types generated from the specification.
 */


namespace Tp
{
/**
 * \struct ObjectPathList
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Generic list type with QDBusObjectPath elements. Convertible with
 * QList<QDBusObjectPath>, but needed to have a discrete type in the Qt4 type system.
 */
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

/**
 * \struct UIntList
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Generic list type with uint elements. Convertible with
 * QList<uint>, but needed to have a discrete type in the Qt4 type system.
 */
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

/**
 * \struct AliasMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, QString>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A dictionary whose keys are contact handles and whose values are aliases.
 */
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

/**
 * \struct AliasPair
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A pair (contact handle, alias) as seen in the AliasesChanged signal.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of AliasPair values.
 */
typedef QList<AliasPair> AliasPairList;

/**
 * \struct Avatar
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * <p>A struct containing avatar data marked with its MIME type.</p>
 */
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

/**
 * \struct AvatarTokenMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, QString>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A dictionary whose keys are contact handles and whose values are avatar 
 * tokens.
 */
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

/**
 * \struct CapabilityChange
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (contact handle, channel type, old generic flags, new generic 
 * flags, old type-specific flags, new type-specific flags) representing a 
 * change to one of a contact&apos;s capabilities, as seen in the 
 * CapabilitiesChanged signal on the Capabilities interface.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of CapabilityChange values.
 */
typedef QList<CapabilityChange> CapabilityChangeList;

/**
 * \struct CapabilityPair
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A pair (channel type, type-specific flags) as passed to 
 * AdvertiseCapabilities on the Capabilities interface.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of CapabilityPair values.
 */
typedef QList<CapabilityPair> CapabilityPairList;

/**
 * \struct ChannelCallStateMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, uint>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A map from contacts to call states.
 */
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

/**
 * \struct ChannelClass
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 * <p>Mapping representing a class of channels that can be requested
 *   from a connection manager, can be handled by a user interface,
 *   are supported by a contact, etc.</p>
 * 
 * <p>Classes of channel are identified by the fixed values of
 *   a subset of their properties.</p>
 * 
 * <p>Channel classes SHOULD always include the keys
 *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.ChannelType</tp:dbus-ref>
 *   and
 *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.TargetHandleType</tp:dbus-ref>.
 *   </p>
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ChannelClass values.
 */
typedef QList<ChannelClass> ChannelClassList;

/**
 * \struct ChannelDetails
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * Enough details of a channel that clients can work out how to dispatch or 
 * handle it.
 */
struct ChannelDetails
{
    /**
     * The object path of the channel.
     */
    QDBusObjectPath channel;
    /**
     * <p>Properties of the channel.</p>
     * 
     * <p>Connection managers MUST NOT include properties in this mapping
     *   if their values can change. Clients MUST ignore properties
     *   that appear in this mapping if their values can change.</p>
     * 
     * <tp:rationale>
     *   <p>If properties that could change were included, the following
     *     race condition would be likely to exist in some cases:</p>
     * 
     *   <ul>
     *     <li>NewChannels or Get(&quot;Channels&quot;) includes a property P with
     *       value V1</li>
     *     <li>Client creates a proxy object for the channel</li>
     *     <li>The value of P changes to V2</li>
     *     <li>Client connects to PChanged signal</li>
     *     <li>Client should call Get(&quot;P&quot;) or GetAll here, to avoid the
     *       race, but client's author has forgotten to do so</li>
     *     <li>Proxy object thinks P == V1, but actually P == V2</li>
     *   </ul>
     * 
     *   <p>We've taken the opportunity to make the API encourage the
     *     client author to get it right. Where possible, we intend that
     *     properties whose value will be used in channel dispatching
     *     or other &quot;early&quot; processing will be defined so that they are
     *     immutable (can never change).</p>
     * </tp:rationale>
     * 
     * <p>Each dictionary MUST contain the keys
     *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.ChannelType</tp:dbus-ref>,
     *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.TargetHandleType</tp:dbus-ref>,
     *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.TargetHandle</tp:dbus-ref>,
     *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.TargetID</tp:dbus-ref>
     *   and
     *   <tp:dbus-ref>org.freedesktop.Telepathy.Channel.Requested</tp:dbus-ref>.
     * </p>
     * 
     * <tp:rationale>
     *   <p>We expect these to be crucial to the channel-dispatching
     *     process.</p>
     * </tp:rationale>
     */
    QVariantMap properties;
};

bool operator==(const ChannelDetails& v1, const ChannelDetails& v2);
inline bool operator!=(const ChannelDetails& v1, const ChannelDetails& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const ChannelDetails& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, ChannelDetails& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ChannelDetails values.
 */
typedef QList<ChannelDetails> ChannelDetailsList;

/**
 * \struct ChannelInfo
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct representing a channel, as returned by ListChannels on the 
 * Connection interface.
 */
struct ChannelInfo
{
    /**
     * The object path of the channel, which is on the same bus name as the 
     * connection
     */
    QDBusObjectPath channel;
    /**
     * The channel&apos;s type
     */
    QString channelType;
    /**
     * The type of the handle that the channel communicates with, or 
     * Handle_Type_None if there is no associated handle
     */
    uint handleType;
    /**
     * The handle that the channel communicates with, or 0 if there is no 
     * associated handle
     */
    uint handle;
};

bool operator==(const ChannelInfo& v1, const ChannelInfo& v2);
inline bool operator!=(const ChannelInfo& v1, const ChannelInfo& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const ChannelInfo& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, ChannelInfo& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ChannelInfo values.
 */
typedef QList<ChannelInfo> ChannelInfoList;

/**
 * \struct ContactAttributesMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, QVariantMap>, but needed to have a discrete type in the Qt4 type system.
 * 
 * Mapping returned by GetContactAttributes, representing a collection of 
 * Contacts and their requested attributes.
 */
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

/**
 * \struct ContactCapability
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (contact handle, channel type, generic flags, type-specific flags) 
 * representing a capability posessed by a contact, as returned by 
 * GetCapabilities on the Capabilities interface.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ContactCapability values.
 */
typedef QList<ContactCapability> ContactCapabilityList;

/**
 * \struct ContactInfoField
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 *         <p>Represents one piece of information about a contact, as modelled by
 *           a single vCard field. Of the fields defined in RFC 2426, common
 *           examples include:</p>
 * 
 *         <dl>
 *           <dt>fn</dt>
 *           <dd>The contact's full name, formatted to their liking</dd>
 * 
 *           <dt>n</dt>
 *           <dd>The contact's full name, divided into five parts: family name,
 *             given name, additional names, honorific prefixes, and honorific
 *             suffixes</dd>
 * 
 *           <dt>org</dt>
 *           <dd>The contact's organisation, divided into the organization's name
 *             possibly followed by one or more organizational unit names.</dd>
 * 
 *           <dt>adr</dt>
 *           <dd>A street address for the contact, divided into seven components:
 *             post office box, extended address, street address, locality (e.g.,
 *             city), region (e.g., state or province), the postal code, and the
 *             country name.</dd>
 * 
 *           <dt>label</dt>
 *           <dd>A free-form street address for the contact, formatted as a
 *             single value (with embedded newlines where necessary) suitable for
 *             printing on an address label</dd>
 * 
 *           <dt>tel</dt>
 *           <dd>A telephone number for the contact.</dd>
 * 
 *           <dt>email</dt>
 *           <dd>An email address for the contact.</dd>
 *         </dl>
 * 
 *         <p>For example, the following vCard:</p>
 * 
 *         <pre>
 *    BEGIN:vCard
 *    VERSION:3.0
 *    FN:Wee Ninja
 *    N;LANGUAGE=ja:Ninja;Wee;;;-san
 *    ORG:Collabora, Ltd.;Management Division;Human Resources\\; Company Policy Enforcement
 *    ADR;TYPE=WORK,POSTAL,PARCEL:;;11 Kings Parade;Cambridge;Cambridgeshire
 *     ;CB2 1SJ;UK
 *    LABEL;TYPE=WORK,POSTAL,PARCEL:11 Kings Parade\\nCambridge\\nCambridgeshire\\nUK\\nCB2 1SJ
 *    TEL;TYPE=VOICE,WORK:+44 1223 362967, +44 7700 900753
 *    EMAIL;TYPE=INTERNET,PREF:wee.ninja@collabora.co.uk
 *    EMAIL;TYPE=INTERNET:wee.ninja@example.com
 *    URL:http://www.thinkgeek.com/geektoys/plush/8823/
 *    NICKNAME:HR Ninja,Enforcement Ninja
 *    END:vCard</pre>
 * 
 *         <p>would be represented by (in Python-like syntax):</p>
 * 
 *         <pre>
 * [
 *   ('fn', [], ['Wee Ninja']),
 *   ('n', ['language=ja'], ['Ninja', 'Wee', '', '', '-san']),
 *   ('org', [], ['Collabora, Ltd.', 'Management Division',
 *     'Human Resources; Company Policy Enforcement']),
 *   ('adr', ['type=work','type=postal','type=parcel'],
 *    ['','','11 Kings Parade','Cambridge', 'Cambridgeshire','CB2 1SJ','UK']),
 *   ('label', ['type=work','type=postal','type=parcel'],
 *    ['''11 Kings Parade
 *   Cambridge
 *   Cambridgeshire
 *   UK
 *   CB2 1SJ''']),
 *   ('tel', ['type=voice','type=work'], ['+44 1223 362967']),
 *   ('tel', ['type=voice','type=work'], ['+44 7700 900753']),
 *   ('email', ['type=internet','type=pref'], ['wee.ninja@collabora.co.uk']),
 *   ('email', ['type=internet'], ['wee.ninja@example.com']),
 *   ('url', [], ['http://www.thinkgeek.com/geektoys/plush/8823/']),
 *   ('nickname', [], ['HR Ninja']),
 *   ('nickname', [], ['Enforcement Ninja'])
 * ]</pre>
 */
struct ContactInfoField
{
    /**
     * The name of the field; this is the lowercased name of a vCard field. 
     * For example, a field representing a contact&apos;s address would be 
     * named &quot;adr&quot;.
     */
    QString fieldName;
    /**
     * <p>A list of vCard type parameters applicable to this field, with their
     *   values. The type parameter names, and any values that are
     *   case-insensitive in vCard, MUST be in lower case. For example, a
     *   contact's preferred home address would have parameters
     *   'type=home' and 'type=pref'.</p>
     * 
     * <tp:rationale>
     *   The type parameter 'type' is likely to be the most common, but
     *   there can be others, such as 'language=en'.
     * </tp:rationale>
     * 
     * <p>Characters which are required to be escaped in vCard type
     *   parameters should not be escaped in this list. For instance,
     *   a field &quot;X-FOO;SEMICOLON=\\;:bar&quot; in a vCard would become
     *   ('x-foo', ['semicolon=;'], ['bar']) in this interface.</p>
     * 
     * <tp:rationale>
     *   This avoids Telepathy UIs having to understand the escaping and
     *   unescaping rules for vCards. The type parameter name is not
     *   allowed (by RFC 2425) to contain an '=' character, so no ambiguity
     *   is introduced.
     * </tp:rationale>
     */
    QStringList parameters;
    /**
     * <p>For unstructured vCard fields (such as 'fn', a formatted name
     *   field), a single-element array containing the field's value.</p>
     * 
     * <p>For structured fields (such as 'adr', an address field), an array
     *   corresponding to the semicolon-separated elements of the field (with
     *   empty strings for empty elements).</p>
     * 
     * <p>A vCard field with multiple comma-separated values, such as
     *   'nickname', should be represented by several
     *   <tp:type>Contact_Info_Field</tp:type>s.</p>
     * 
     * <p>Characters which are required to be escaped in vCard values, such as
     *   semi-colons and newlines, should not be escaped in this list (e.g. if
     *   a value contains a newline, the data passed over D-Bus should
     *   contain a literal newline character).</p>
     * 
     * <tp:rationale>
     *   An earlier draft of this interface split structured vCard fields
     *   into multiple Telepathy-level fields; for example, 'n' became
     *   'family-name', 'given-name', etc.  But under this representation,
     *   omitting empty components leads to difficulty identifying where one
     *   name ends and another begins.  Consider the fields ['given-name',
     *   'honorific-suffixes', 'family-name', 'honorific-prefixes']: does
     *   this represent two 'n' fields, or one with incorrect component
     *   ordering?
     * </tp:rationale>
     */
    QStringList fieldValue;
};

bool operator==(const ContactInfoField& v1, const ContactInfoField& v2);
inline bool operator!=(const ContactInfoField& v1, const ContactInfoField& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const ContactInfoField& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, ContactInfoField& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ContactInfoField values.
 */
typedef QList<ContactInfoField> ContactInfoFieldList;

/**
 * \struct ContactLocations
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, QVariantMap>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A map from contacts to their locations.
 */
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

/**
 * \struct CurrencyAmount
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * <p>An amount of money in a specified currency. For example,
 *   3.21 British pounds would conventionally be represented by
 *   (Amount = 321, Scale = 2, Currency = &quot;GBP&quot;), but could be
 *   represented by (Amount = 3210, Scale = 3, Currency = &quot;GBP&quot;)
 *   in a service that records balance in units of 0.001 pounds.</p>
 * 
 * <p>As a special case, if Amount = 0, Scale = 2**32 - 1 (i.e.
 *   the largest possible 32-bit unsigned integer) and Currency = &quot;&quot;,
 *   this indicates an unknown amount.</p>
 */
struct CurrencyAmount
{
    /**
     * <p>The amount, expressed as a fixed-point number with decimal scale
     *   defined by the Scale property; for instance, an Amount value of
     *   1234 with Scale of 2 represents 12.34 in the currency unit given
     *   by the Currency.</p>
     */
    int amount;
    /**
     * <p>The decimal scale for the fixed point value of the Amount
     *   property, defining the number of rightmost decimal digits from
     *   the integer value which form the fractional part of the resulting
     *   currency value.</p>
     * 
     * <p>As well as defining the interpretation of Amount, user interfaces
     *   may use this value to determine the precision with which to display
     *   the amount.</p>
     */
    uint scale;
    /**
     * The currency code represented by this amount, which SHOULD be an 
     * international currency code such as &quot;EUR&quot;, &quot;USD&quot;, 
     * or &quot;JPY&quot; if possible. An empty string can be used to indicate 
     * that the currency is not known.
     */
    QString currency;
};

bool operator==(const CurrencyAmount& v1, const CurrencyAmount& v2);
inline bool operator!=(const CurrencyAmount& v1, const CurrencyAmount& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const CurrencyAmount& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, CurrencyAmount& val);

/**
 * \struct DBusTubeMember
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * Represents a participant in a multi-user D-Bus tube, as returned by 
 * GetDBusNames and seen in the DBusNamesChanged signal.
 */
struct DBusTubeMember
{
    /**
     * The handle of a participant in this D-Bus tube.
     */
    uint handle;
    /**
     * That participant&apos;s unique name.
     */
    QString uniqueName;
};

bool operator==(const DBusTubeMember& v1, const DBusTubeMember& v2);
inline bool operator!=(const DBusTubeMember& v1, const DBusTubeMember& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const DBusTubeMember& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, DBusTubeMember& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of DBusTubeMember values.
 */
typedef QList<DBusTubeMember> DBusTubeMemberList;

/**
 * \struct FieldSpec
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct describing a vCard field, with parameters, that may be passed to 
 * SetContactInfo on this Connection.
 */
struct FieldSpec
{
    /**
     * A vCard field name, such as &apos;tel&apos;.
     */
    QString name;
    /**
     * The set of vCard type parameters which may be set on this field. If 
     * this list is empty and the Contact_Info_Field_Flag_Parameters_Exact 
     * flag is not set, any vCard type parameters may be used.
     */
    QStringList parameters;
    /**
     * Flags describing the behaviour of this field.
     */
    uint flags;
    /**
     * Maximum number of instances of this field which may be set. MAXUINT32 
     * is used to indicate that there is no limit.
     */
    uint max;
};

bool operator==(const FieldSpec& v1, const FieldSpec& v2);
inline bool operator!=(const FieldSpec& v1, const FieldSpec& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const FieldSpec& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, FieldSpec& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of FieldSpec values.
 */
typedef QList<FieldSpec> FieldSpecs;

/**
 * \struct HandleIdentifierMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, QString>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A map from handles to the corresponding normalized string identifier.
 */
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

/**
 * \struct HandleOwnerMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, uint>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A map from channel-specific handles to their owners.
 */
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

/**
 * \struct LocalPendingInfo
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A structure representing a contact whose attempt to join a group is to be 
 * confirmed by the local user using AddMembers.
 */
struct LocalPendingInfo
{
    /**
     * The contact to be added to the group
     */
    uint toBeAdded;
    /**
     * The contact requesting or causing the change
     */
    uint actor;
    /**
     * The reason for the change
     */
    uint reason;
    /**
     * A human-readable message from the Actor, or an empty string if there is 
     * no message
     */
    QString message;
};

bool operator==(const LocalPendingInfo& v1, const LocalPendingInfo& v2);
inline bool operator!=(const LocalPendingInfo& v1, const LocalPendingInfo& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const LocalPendingInfo& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, LocalPendingInfo& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of LocalPendingInfo values.
 */
typedef QList<LocalPendingInfo> LocalPendingInfoList;

/**
 * \struct Location
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A user&apos;s location, represented as an extensible mapping.
 */
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

/**
 * \struct MediaSessionHandlerInfo
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct representing a active session handler.
 */
struct MediaSessionHandlerInfo
{
    /**
     * The object path of the session handler, which is on the same bus name 
     * as the channel.
     */
    QDBusObjectPath sessionHandler;
    /**
     * The media session&apos;s type
     */
    QString mediaSessionType;
};

bool operator==(const MediaSessionHandlerInfo& v1, const MediaSessionHandlerInfo& v2);
inline bool operator!=(const MediaSessionHandlerInfo& v1, const MediaSessionHandlerInfo& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const MediaSessionHandlerInfo& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, MediaSessionHandlerInfo& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MediaSessionHandlerInfo values.
 */
typedef QList<MediaSessionHandlerInfo> MediaSessionHandlerInfoList;

/**
 * \struct MediaStreamHandlerTransport
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MediaStreamHandlerTransport values.
 */
typedef QList<MediaStreamHandlerTransport> MediaStreamHandlerTransportList;

/**
 * \struct MediaStreamInfo
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MediaStreamInfo values.
 */
typedef QList<MediaStreamInfo> MediaStreamInfoList;

/**
 * \struct MessagePart
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 *         <p>Part of a message's content. In practice, this mapping never
 *           appears in isolation - messages are represented by a list of
 *           <tp:type>Message_Part</tp:type> mappings.</p>
 * 
 *         <p>An example of how a rich-text message, with an embedded image, might
 *           look, in a Python-like syntax:</p>
 * 
 *           <pre>
 * [
 *   {
 *     'message-token': '9de9546a-3400-4419-a505-3ea270cb834c',
 *     'message-sender': 42,
 *     'message-sent': 1210067943,
 *     'message-received': 1210067947,
 *     'message-type': 0,              # = Channel_Text_Message_Type_Normal
 *     'pending-message-id': 437,
 *   },
 *   { 'alternative': 'main',
 *     'content-type': 'text/html',
 *     'content': 'Here is a photo of my cat:&lt;br /&gt;' +
 *                '&lt;img src=&quot;cid:catphoto&quot; alt=&quot;lol!&quot; /&gt;' +
 *                '&lt;br /&gt;Isn't it cute?',
 *   },
 *   { 'alternative': 'main',
 *     'content-type': 'text/plain',
 *     'content': 'Here is a photo of my cat:\\n[IMG: lol!]\\nIsn't it cute?',
 *   },
 *   { 'identifier': 'catphoto',
 *     'content-type': 'image/jpeg',
 *     'size': 101000,
 *     'needs-retrieval': True,
 *   },
 * ]</pre>
 * 
 *         <p>An example of how a non-text message — in particular, a vCard sent
 *           via SMS as implemented by telepathy-ring on Nokia's Maemo 5 —
 *           looks:</p>
 * 
 *         <pre>
 * [
 *   {
 *     'message-token': '9de9546a-3400-4419-a505-3ea270cb834c',
 *     'message-sender': 42,
 *     'message-sent': 1210067943,
 *     'message-received': 1210067947,
 *     'message-type': 0,              # = Channel_Text_Message_Type_Normal
 *     'pending-message-id': 437,
 *   },
 *   { 'content-type': 'text/x-vcard',
 *     'content': [ 0x66, 0x69, 0x71, ...], # vCard data as an array of bytes
 *   },
 * ]</pre>
 * 
 *         <div>
 *           <p>The first part of the message contains &quot;headers&quot; which refer
 *             to the entire message.</p>
 * 
 *           <p>It is an error for a connection manager to put keys referring
 *             to the message as a whole in the second or subsequent
 *             Message_Part, but clients MUST recover from this error by ignoring
 *             these keys in the second and subsequent parts.</p>
 * 
 *           <tp:rationale>
 *             <p>Instead of representing messages as aa{sv} where the first
 *               dictionary is special (a dictionary of headers), we could have
 *               used a signature like (a{sv}aa{sv}) to separate out the headers
 *               and the body parts.</p>
 * 
 *             <p>However, this would make access to the messages more awkward.
 *               In Python, the syntax for access to a header field would remain
 *               <code>message[0]['message-type']</code>, but access to a body
 *               field in the second body part would change from
 *               message[2]['content'] to message[1][1]['content']. In GLib,
 *               the message would change from being a
 *               GPtrArray(GHashTable) to being a
 *               GValueArray(GHashTable, GPtrArray(GHashTable)) which is rather
 *               inconvenient to dereference.</p>
 *           </tp:rationale>
 * 
 *           <p>Well-known keys for the message as a whole, and the corresponding
 *             value types, include:</p>
 * 
 *           <dl>
 *             <dt>message-token (s)</dt>
 *             <dd>
 *               <p>An opaque, globally-unique identifier for the entire message.
 *                 This MAY be treated as if it were a MIME Message-ID, e.g. for
 *                 the mid: and cid: URI schemes. If omitted, there is no suitable
 *                 token; the protocol-token key SHOULD be provided if the protocol
 *                 identifies messages in some less unique way.</p>
 *             </dd>
 * 
 *             <dt>protocol-token (s - <tp:type>Protocol_Message_Token</tp:type>)</dt>
 *             <dd>
 *               <p>An opaque token for the entire message, with whatever uniqueness
 *                 guarantee is provided by the underlying protocol. As described
 *                 for the Protocol_Message_Token type, this token is <em>not</em>
 *                 guaranteed to be unique between contacts, or even within the
 *                 scope of a Channel.</p>
 * 
 *               <tp:rationale>
 *                 <p>In practice, in most protocols there is no token with the
 *                   uniqueness guarantees demanded for message-token; the
 *                   definition of message-token was inappropriate, but must now
 *                   be preserved for the benefit of clients that rely on it, at
 *                   least until Telepathy breaks backwards compatibility.</p>
 *               </tp:rationale>
 * 
 *               <p>The message-token and protocol-token SHOULD NOT both be present;
 *                 clients requiring an identifier with the semantics of the
 *                 protocol-token SHOULD look for the message-token first, falling
 *                 back to the protocol-token.</p>
 * 
 *               <tp:rationale>
 *                 <p>This is for compatibility with CMs older than the
 *                   protocol-token key.</p>
 *               </tp:rationale>
 *             </dd>
 * 
 *             <dt>message-sent (x - <tp:type>Unix_Timestamp64</tp:type>)</dt>
 *             <dd>The time the message was sent (if unavailable, the time
 *               it arrived at a central server MAY be used). Omitted if no
 *               reasonable approximation is available; SHOULD always be present
 *               on outgoing messages.</dd>
 * 
 *             <dt>message-received (x - <tp:type>Unix_Timestamp64</tp:type>)</dt>
 *             <dd>The time the message was received locally. SHOULD always
 *               be present.</dd>
 * 
 *             <dt>message-sender (u - <tp:type>Contact_Handle</tp:type>)</dt>
 *             <dd>The contact who sent the message. If 0 or omitted, the contact
 *               who sent the message could not be determined.</dd>
 * 
 *             <dt>message-type (u - <tp:type>Channel_Text_Message_Type</tp:type>)
 *             </dt>
 *             <dd>The type of message; if omitted,
 *               Channel_Text_Message_Type_Normal MUST be assumed. MAY
 *               be omitted for normal chat messages.</dd>
 * 
 *             <dt>pending-message-id (u - <tp:type>Message_ID</tp:type>)</dt>
 *             <dd>The incoming message ID. This MUST NOT be present on outgoing
 *               messages. Clients SHOULD NOT store this key - it is only valid
 *               for as long as the message remains unacknowledged.</dd>
 * 
 *             <dt>interface (s - <tp:type>DBus_Interface</tp:type>)</dt>
 *             <dd>This message is specific to the given interface, which is
 *               neither Text nor Messages. It SHOULD be ignored if that
 *               interface is not supported. (Note that an 'interface' key
 *               can also appear on the second and subsequent parts, where
 *               it indicates that that part (only) should be ignored if
 *               unsupported.)</dd>
 * 
 *             <dt>scrollback (b)</dt>
 *             <dd>If present and true, the incoming message was part of a
 *               replay of message history (this matches the Scrollback flag in
 *               <tp:type>Channel_Text_Message_Flags</tp:type>). This flag
 *               does not make sense on outgoing messages and SHOULD NOT
 *               appear there.</dd>
 * 
 *             <dt>rescued (b)</dt>
 *             <dd>If present and true, the incoming message has been seen in
 *               a previous channel during the lifetime of the Connection,
 *               but had not been acknowledged when that channel closed, causing
 *               an identical channel (in which the message now appears) to open.
 *               This matches the Rescued flag in
 *               <tp:type>Channel_Text_Message_Flags</tp:type>; it
 *               does not make sense on outgoing messages, and SHOULD NOT
 *               appear there.</dd>
 *           </dl>
 *         </div>
 * 
 *         <div>
 *           <p>The second and subsequent parts contain the message's
 *             content, including plain text, formatted text and/or attached
 *             files.</p>
 * 
 *           <p>It is an error for a connection manager to put keys referring
 *             to the message body in the first Message_Part;
 *             clients MUST recover from this error by ignoring
 *             these keys in first part.</p>
 * 
 *           <p>In any group of parts with the same non-empty value for the
 *             &quot;alternative&quot; key (which represent alternative versions of the
 *             same content), more faithful versions of the intended message MUST
 *             come before less faithful versions (note that this order is the
 *             opposite of MIME &quot;multipart/alternative&quot; parts). Clients SHOULD
 *             display the first alternative that they understand.</p>
 * 
 *           <tp:rationale>
 *             <p>Specifying the preference order means that if the underlying
 *               protocol doesn't support alternatives, the CM can safely delete
 *               everything apart from the first supported alternative when
 *               sending messages.</p>
 * 
 *             <p>The order is the reverse of MIME because MIME's rationale for
 *               placing the &quot;plainest&quot; part first (legibility in pre-MIME UAs)
 *               does not apply to us, and placing the most preferred part
 *               first simplifies display (a client can iterate the message
 *               in order, display the first alternative that it understands,
 *               and skip displaying all subsequent parts with the same
 *               &quot;alternative&quot; key).</p>
 *           </tp:rationale>
 * 
 *           <p>Clients SHOULD present all parts that are not redundant
 *             alternatives in the order they appear in this array, possibly
 *             excluding parts that are referenced by another displayed part.
 *             It is implementation-specific how the parts are presented to the
 *             user.</p>
 * 
 *           <tp:rationale>
 *             <p>This allows CMs to assume that all parts are actually shown to
 *               the user, even if they are not explicitly referenced - we do
 *               not yet recommend formatted text, and there is no way for
 *               plain text to reference an attachment since it has no concept of
 *               markup or references. This also forces clients to do something
 *               sensible with messages that consist entirely of &quot;attachments&quot;,
 *               with no &quot;body&quot; at all.</p>
 * 
 *             <p>For instance, when displaying the above example, a client that
 *               understands the HTML part should display the JPEG image once,
 *               between the two lines &quot;Here is a photo of my cat:&quot; and
 *               &quot;Isn't it cute?&quot;; it may additionally present the image in some
 *               way for a second time, after &quot;Isn't it cute?&quot;, or may choose
 *               not to.</p>
 * 
 *             <p>A client that does not understand HTML, displaying the same
 *               message, should display the plain-text part, followed by the JPEG
 *               image.</p>
 *           </tp:rationale>
 * 
 *           <p>Well-known keys for the second and subsequent parts, and the
 *             corresponding value types, include:</p>
 * 
 *           <dl>
 *             <dt>identifier (s)</dt>
 *             <dd>An opaque identifier for this part.
 *               Parts of a message MAY reference other parts by treating
 *               this identifier as if it were a MIME Content-ID and using
 *               the cid: URI scheme.</dd>
 * 
 *             <dt>alternative (s)</dt>
 *             <dd>
 *               <p>If present, this part of the message is an alternative for
 *                 all other parts with the same value for &quot;alternative&quot;.
 *                 Clients SHOULD only display one of them (this is expected to
 *                 be used for XHTML messages in a future version of this
 *                 specification).</p>
 * 
 *               <p>If omitted, this part is not an alternative for any other
 *                 part.</p>
 * 
 *               <p>Parts of a message MAY reference the group of alternatives
 *                 as a whole (i.e. a reference to whichever of them is chosen)
 *                 by treating this identifier as if it were the MIME Content-ID
 *                 of a multipart/alternative part, and using the cid: URI
 *                 scheme.</p>
 *             </dd>
 * 
 *             <dt>content-type (s)</dt>
 *             <dd>
 *               <p>The MIME type of this part. See the documentation
 *                 for ReceivedMessage for notes on the special status of
 *                 &quot;text/plain&quot; parts.</p>
 * 
 *               <p>Connection managers MUST NOT signal parts without a
 *                 'content-type' key; if a protocol provides no way to determine
 *                 the MIME type, the connection manager is responsible for
 *                 guessing it, but MAY fall back to &quot;text/plain&quot; for text and
 *                 &quot;application/octet-stream&quot; for non-text.</p>
 * 
 *               <p>Clients MUST ignore parts without a 'content-type' key, which
 *                 are reserved for future expansion.</p>
 *             </dd>
 * 
 *             <dt>lang (s)</dt>
 *             <dd>The natural language of this part, identified by a
 *               RFC 3066 language tag.
 * 
 *               <tp:rationale>
 *                 XMPP allows alternative-selection by language as well as
 *                 by content-type.
 *               </tp:rationale>
 *             </dd>
 * 
 *             <dt>size (u)</dt>
 *             <dd>The size in bytes (if needs-retrieval is true, this MAY be an
 *               estimated or approximate size). SHOULD be omitted if 'content'
 *               is provided.
 * 
 *               <tp:rationale>
 *                 There's no point in providing the size if you're already
 *                 providing all the content.
 *               </tp:rationale>
 *             </dd>
 * 
 *             <dt>thumbnail (b)</dt>
 *             <dd>
 *               <p>This part is a thumbnail. To represent an image together with
 *                 its thumbnail in a single message, there should be one part for
 *                 the full image followed by a part for the thumbnail (following
 *                 the “more complete versions first” requirement), with the same
 *                 'alternative' value. For example:</p>
 * 
 *               <pre>
 * [ ... ,
 *   { 'alternative': 'catphoto',
 *     'content-type': 'image/jpeg',
 *     'size': 150000,
 *     'content': [0xFF, 0xD8, ... 0xFF 0xD9],
 *   },
 *   { 'alternative': 'catphoto',
 *     'content-type': 'image/jpeg'
 *     'size': 1024,
 *     'thumbnail': True,
 *     'content': [0xFF, 0xD8, ... 0xFF 0xD9],
 *   },
 *   ...
 * ]
 *               </pre>
 *             </dd>
 * 
 *             <dt>needs-retrieval (b)</dt>
 *             <dd>If false or omitted, the connection
 *               manager already holds this part in memory. If present and true,
 *               this part will be retrieved on demand (like MIME's
 *               message/external-body), so clients should expect retrieval to
 *               take time; if this specification is later extended to provide a
 *               streaming version of GetPendingMessageContent, clients should
 *               use it for parts with this flag.</dd>
 * 
 *             <dt>truncated (b)</dt>
 *             <dd>The content available via the 'content' key or
 *               GetPendingMessageContent has been truncated by the server
 *               or connection manager (equivalent to
 *               Channel_Text_Message_Flag_Truncated in the Text interface).
 *             </dd>
 * 
 *             <dt>content (s or ay)</dt>
 *             <dd>The part's content, if it is available and
 *               sufficiently small to include here (implies that
 *               'needs-retrieval' is false or omitted). Otherwise, omitted.
 *               If the part is human-readable text or HTML, the value for this
 *               key MUST be a UTF-8 string (D-Bus signature 's').
 *               If the part is not text, the value MUST be a byte-array
 *               (D-Bus signature 'ay'). If the part is a text-based format
 *               that is not the main body of the message (e.g. an iCalendar
 *               or an attached XML document), the value SHOULD be a UTF-8 string,
 *               transcoding from another charset to UTF-8 if necessary, but
 *               MAY be a byte-array (of unspecified character set) if
 *               transcoding fails or the source charset is not known.</dd>
 * 
 *               <!-- FIXME: "sufficiently small to include" is not currently
 *               defined; we should add some API so clients can tell the
 *                 CM how large a message it should emit in the signal.-->
 * 
 *             <dt>interface (s - <tp:type>DBus_Interface</tp:type>)</dt>
 *             <dd>This part is specific to the given interface, which is
 *               neither Text nor Messages. It SHOULD be ignored if that
 *               interface is not supported. (Note that an 'interface' key
 *               can also appear on the first part, where it indicates that the
 *               entire message should be ignored if unsupported.)</dd>
 *           </dl>
 * 
 *         </div>
 * 
 * 
 *         <div>
 *           <p>Delivery reports are also represented as messages, of type
 *             Channel_Text_Message_Type_Delivery_Report, with the
 *             Non_Text_Content flag in the Text interface.</p>
 * 
 *           <p>Whenever a message of type
 *             Channel_Text_Message_Type_Delivery_Report is signalled for a
 *             delivery error report, Channel.Type.Text.SendError SHOULD also
 *             be emitted; whenever Channel.Type.Text.SendError is emitted by a
 *             channel which supports this interface, a message of type
 *             Channel_Text_Message_Type_Delivery_Report MUST also be emitted.</p>
 * 
 *           <p>The corresponding message in the Messages interface MUST contain
 *             &quot;headers&quot; for the delivery report, as specified below, in its
 *             first Message_Part.</p>
 * 
 *           <dl>
 *             <dt>message-sender (u - Contact_Handle as defined above)</dt>
 *             <dd>MUST be the intended recipient of the original message, if
 *               available (zero or omitted if the intended recipient is
 *               unavailable or is not a contact, e.g. a chatroom), even if the
 *               delivery report actually came from an intermediate server.</dd>
 * 
 *             <dt>message-type (u - Channel_Text_Message_Type as defined
 *               above)</dt>
 *             <dd>MUST be Channel_Text_Message_Type_Delivery_Report.</dd>
 * 
 *             <dt>delivery-status (u - Delivery_Status)</dt>
 *             <dd>The status of the message. All delivery reports MUST contain
 *               this key in the first Message_Part.</dd>
 * 
 *             <dt>delivery-token (s - <tp:type>Protocol_Message_Token</tp:type>)</dt>
 * 
 *             <dd>
 *               <p>An identifier for the message to which this delivery report
 *                 refers. MUST NOT be an empty string. Omitted if not
 *                 available.</p>
 * 
 *               <p>Clients may match this against the token produced by the
 *                 SendMessage method and MessageSent signal. A status report
 *                 with no token could match any sent message, and a sent
 *                 message with an empty token could match any status report.
 *                 If multiple sent messages match, clients SHOULD use some
 *                 reasonable heuristic.</p>
 * 
 *               <tp:rationale>
 *                 In an ideal world, we could unambiguously match reports
 *                 against messages; however, deployed protocols are not ideal,
 *                 and not all reports and messages can be matched.
 *               </tp:rationale>
 *             </dd>
 * 
 *             <dt>delivery-error (u - Channel_Text_Send_Error)</dt>
 *             <dd>
 *               The reason for the failure. MUST be omitted if this was a
 *               successful delivery; SHOULD be omitted if it would be
 *               Channel_Text_Send_Error_Unknown.
 *             </dd>
 * 
 *             <dt>delivery-dbus-error (s - DBus_Error_Name)</dt>
 *             <dd>
 *               The reason for the failure, specified as a (possibly
 *               implementation-specific) D-Bus error. MUST be omitted if this was
 *               a successful delivery. If set, the 'delivery-error' key SHOULD be
 *               set to the closest available value.
 *             </dd>
 * 
 *             <dt>delivery-error-message (s)</dt>
 *             <dd>
 *               Debugging information on why the message could not be delivered.
 *               MUST be omitted if this was a successful delivery; MAY always be
 *               omitted.
 *             </dd>
 * 
 *             <dt>delivery-echo (aa{sv} - Message_Part[])</dt>
 *             <dd>
 *               <p>The message content, as defined by the Messages interface.
 *                 Omitted if no content is available. Content MAY have been
 *                 truncated, message parts MAY have been removed, and message
 *                 parts MAY have had their content removed (i.e. the message part
 *                 metadata is present, but the 'content' key is not).</p>
 * 
 *               <tp:rationale>
 *                 Some protocols, like XMPP, echo the failing message back to
 *                 the sender. This is sometimes the only way to match it
 *                 against the sent message, so we include it here.
 *               </tp:rationale>
 * 
 *               <p>Unlike in the Messages interface, content not visible
 *                 in the value for this key cannot be retrieved by another
 *                 means, so the connection manager SHOULD be more
 *                 aggressive about including (possibly truncated) message
 *                 content in the 'content' key.</p>
 * 
 *               <tp:rationale>
 *                 The Messages interface needs to allow all content to be
 *                 retrieved, but in this interface, the content we provide is
 *                 merely a hint; so some is better than none, and it doesn't
 *                 seem worth providing an API as complex as Messages'
 *                 GetPendingMessageContent for the echoed message.
 *               </tp:rationale>
 *             </dd>
 * 
 *           </dl>
 * 
 *           <p>The second and subsequent Message_Part dictionaries, if present,
 *             are a human-readable report from the IM service.</p>
 * 
 *           <p>Clients MUST NOT attempt to send delivery reports using the
 *             SendMessage method in the Messages API, and connection managers
 *             MUST NOT allow this to be done. If support for sending delivery
 *             reports is later added, it will be part of this interface.</p>
 * 
 *           <p>Some example delivery reports in a Python-like syntax (in which
 *             arrays are indicated by [a, b] and dictionaries by {k1: v1, k2: v2})
 *             follow.</p>
 * 
 *           <dl>
 *             <dt>A minimal delivery report indicating permanent failure of the
 *               sent message whose token was
 *               <code>b9a991bd-8845-4d7f-a704-215186f43bb4</code> for an unknown
 *               reason</dt>
 *             <dd><pre>
 * [{
 * # header
 * 'message-sender': 123,
 * 'message-type': Channel_Text_Message_Type_Delivery_Report,
 * 'delivery-status': Delivery_Status_Permanently_Failed,
 * 'delivery-token': 'b9a991bd-8845-4d7f-a704-215186f43bb4',
 * }
 * # no body
 * ]
 * </pre></dd>
 * 
 *             <dt>A delivery report where the failed message is echoed back to the
 *               sender rather than being referenced by ID, and the failure reason
 *               is that this protocol cannot send messages to offline contacts
 *               such as the contact with handle 123</dt>
 *             <dd><pre>
 * [{ # header
 * 'message-sender': 123,
 * 'message-type': Channel_Text_Message_Type_Delivery_Report,
 * 'delivery-status': Delivery_Status_Temporarily_Failed,
 * 'delivery-error': Channel_Text_Send_Error_Offline,
 * 'delivery-echo':
 *     [{ # header of original message
 *     'message-sender': 1,
 *     'message-sent': 1210067943,
 *     },
 *     { # body of original message
 *     'content-type': 'text/plain',
 *     'content': 'Hello, world!',
 *     }]
 *   ],
 * 
 * # no body
 * ]
 * </pre></dd>
 * 
 *             <dt>A maximally complex delivery report: the server reports a
 *               bilingual human-readable failure message because the user sent
 *               a message &quot;Hello, world!&quot; with token
 *               <code>b9a991bd-8845-4d7f-a704-215186f43bb4</code> to a contact
 *               with handle 123, but that handle represents a contact who does not
 *               actually exist</dt>
 *             <dd><pre>
 * [{ # header
 * 'message-sender': 123,
 * 'message-type': Channel_Text_Message_Type_Delivery_Report,
 * 'delivery-status': Delivery_Status_Permanently_Failed,
 * 'delivery-error': Channel_Text_Send_Error_Invalid_Contact,
 * 'delivery-token': 'b9a991bd-8845-4d7f-a704-215186f43bb4',
 * 'delivery-echo':
 *     [{ # header of original message
 *     'message-sender': 1,
 *     'message-sent': 1210067943,
 *     },
 *     { # body of original message
 *     'content-type': 'text/plain',
 *     'content': 'Hello, world!',
 *     }]
 *   ],
 * },
 * { # message from server (alternative in English)
 * 'alternative': '404',
 * 'content-type': 'text/plain',
 * 'lang': 'en',
 * 'content': 'I have no contact with that name',
 * },
 * { # message from server (alternative in German)
 * 'alternative': '404'.
 * 'content-type': 'text/plain',
 * 'lang': 'de',
 * 'content', 'Ich habe keinen Kontakt mit diesem Namen',
 * }
 * ]
 * </pre></dd>
 * 
 *             <dt>A minimal delivery report indicating successful delivery
 *               of the sent message whose token was
 *               <code>b9a991bd-8845-4d7f-a704-215186f43bb4</code></dt>
 *             <dd><pre>
 * [{
 * # header
 * 'message-sender': 123,
 * 'message-type': Channel_Text_Message_Type_Delivery_Report,
 * 'delivery-status': Delivery_Status_Delivered,
 * 'delivery-token': 'b9a991bd-8845-4d7f-a704-215186f43bb4',
 * }
 * # no body
 * ]
 * </pre></dd>
 * 
 *           </dl>
 * 
 *         </div>
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MessagePart values.
 */
typedef QList<MessagePart> MessagePartList;

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MessagePartList values.
 */
typedef QList<MessagePartList> MessagePartListList;

/**
 * \struct MessagePartContentMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A mapping from message part indexes to their content, as returned by 
 * GetPendingMessageContent.
 */
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

/**
 * \struct MultipleStatusMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QVariantMap>, but needed to have a discrete type in the Qt4 type system.
 * 
 * Mapping used in Last_Activity_And_Statuses and passed to SetStatus, 
 * representing a collection of statuses. Use of this mapping with more than 
 * one member is deprecated.
 */
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

/**
 * \struct ParamSpec
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct representing an allowed parameter, as returned by GetParameters on 
 * the ConnectionManager interface.
 */
struct ParamSpec
{
    /**
     * A string parameter name
     */
    QString name;
    /**
     * A bitwise OR of the parameter flags
     */
    uint flags;
    /**
     * A string containing the D-Bus type signature for this parameter
     */
    QString signature;
    /**
     * The default value (if the Has_Default flag is not present, there is no 
     * default and this takes some dummy value, which SHOULD be of the 
     * appropriate D-Bus type)
     */
    QDBusVariant defaultValue;
};

bool operator==(const ParamSpec& v1, const ParamSpec& v2);
inline bool operator!=(const ParamSpec& v1, const ParamSpec& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const ParamSpec& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, ParamSpec& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ParamSpec values.
 */
typedef QList<ParamSpec> ParamSpecList;

/**
 * \struct PendingTextMessage
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (message ID, timestamp in seconds since 1970-01-01 00:00 UTC, 
 * sender&apos;s handle, message type, flags, text) representing a pending 
 * text message, as returned by ListPendingMessages. The arguments of the 
 * Received signal also match this struct&apos;s signature.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of PendingTextMessage values.
 */
typedef QList<PendingTextMessage> PendingTextMessageList;

/**
 * \struct PropertyFlagsChange
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (property ID, flags) representing a change to a property&apos;s 
 * flags, as seen in the PropertyFlagsChanged signal on the Properties 
 * interface.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of PropertyFlagsChange values.
 */
typedef QList<PropertyFlagsChange> PropertyFlagsChangeList;

/**
 * \struct PropertySpec
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (property ID, property name, D-Bus signature, flags) representing 
 * a property, as returned by ListProperties on the Properties interface.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of PropertySpec values.
 */
typedef QList<PropertySpec> PropertySpecList;

/**
 * \struct PropertyValue
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (property ID, value) representing a property&apos;s value, as seen 
 * in the PropertiesChanged signal on the Properties interface, returned by 
 * the GetProperties method and passed to the SetProperties method.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of PropertyValue values.
 */
typedef QList<PropertyValue> PropertyValueList;

/**
 * \struct QualifiedPropertyValueMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A mapping from strings representing D-Bus properties (by their namespaced 
 * names) to their values.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of QualifiedPropertyValueMap values.
 */
typedef QList<QualifiedPropertyValueMap> QualifiedPropertyValueMapList;

/**
 * \struct RequestableChannelClass
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * <p>Structure representing a class of channels that can be requested,
 *   identified by a set of properties that identify that class of
 *   channel.</p>
 * 
 * <tp:rationale>
 *   <p>This will often just be the channel type and the handle type,
 *     but can include other properties of the channel - for instance,
 *     encrypted channels might require properties that
 *     unencrypted channels do not, like an encryption key.</p>
 * </tp:rationale>
 * 
 * <p>In some cases, these classes of channel may overlap, in the sense
 *   that one class fixes all the properties that another class does,
 *   plus some more properties.</p>
 * 
 * <tp:rationale>
 *   <p>For older clients to still be able to understand how to request
 *     channels in the presence of a hypothetical &quot;encryption&quot; interface,
 *     we'd need to represent it like this:</p>
 * 
 *   <ul>
 *     <li>class 1: ChannelType = Text, TargetHandleType = CONTACT</li>
 *     <li>class 2: Channel.ChannelType = Text,
 *       Channel.TargetHandleType = CONTACT,
 *       Encryption.Encrypted = TRUE</li>
 *   </ul>
 * </tp:rationale>
 */
struct RequestableChannelClass
{
    /**
     * <p>The property values that identify this requestable channel class.
     *   These properties MUST be included in requests for a channel of this
     *   class, and MUST take these values.</p>
     * 
     * <p>Clients that do not understand the semantics of all the
     *   Fixed_Properties MUST NOT request channels of this class, since
     *   they would be unable to avoid making an incorrect request.</p>
     * 
     * <p>This implies that connection managers wishing to make channels
     *   available to old or minimal clients SHOULD have a channel class
     *   with the minimum number of Fixed_Properties, and MAY additionally
     *   have channel classes with extra Fixed_Properties.</p>
     */
    QVariantMap fixedProperties;
    /**
     * <p>Properties that MAY be set when requesting a channel of this
     *   channel type and handle type.</p>
     * 
     * <p>This array MUST NOT include properties that are in the
     *   Fixed_Properties mapping.</p>
     * 
     * <p>Properties in this array may either be required or optional,
     *   according to their documented semantics.</p>
     * 
     * <tp:rationale>
     *   <p>For instance, if
     *     TargetHandleType takes a value that is not Handle_Type_None,
     *     one or the other of TargetHandle and TargetID is required.
     *     Clients are expected to understand the documented relationship
     *     between the properties, so we do not have separate arrays
     *     of required and optional properties.</p>
     * </tp:rationale>
     * 
     * <p>If this array contains the
     *   <tp:dbus-ref namespace="org.freedesktop.Telepathy.Channel.FUTURE">Bundle</tp:dbus-ref>
     *   property, then this class of channel can be combined with other
     *   channels with that property in a request, or added to an existing
     *   bundle. If not, this signifies that the connection manager is
     *   unable to mark channels of this class as part of a bundle - this
     *   means that to the remote contact they are likely to be
     *   indistinguishable from channels requested separately.</p>
     */
    QStringList allowedProperties;
};

bool operator==(const RequestableChannelClass& v1, const RequestableChannelClass& v2);
inline bool operator!=(const RequestableChannelClass& v1, const RequestableChannelClass& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const RequestableChannelClass& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, RequestableChannelClass& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of RequestableChannelClass values.
 */
typedef QList<RequestableChannelClass> RequestableChannelClassList;

/**
 * \struct RichPresenceAccessControl
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * An access control mode for extended presence items like geolocation. This 
 * type isn&apos;t actually used by the SimplePresence interface, but 
 * it&apos;s included here so it can be referenced by rich presence interfaces 
 * such as Location.
 */
struct RichPresenceAccessControl
{
    /**
     * The type of access control to apply.
     */
    uint type;
    /**
     * Any additional information required by the Type. The required type and 
     * semantics are defined for each Rich_Presence_Access_Control_Type.
     */
    QDBusVariant detail;
};

bool operator==(const RichPresenceAccessControl& v1, const RichPresenceAccessControl& v2);
inline bool operator!=(const RichPresenceAccessControl& v1, const RichPresenceAccessControl& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const RichPresenceAccessControl& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, RichPresenceAccessControl& val);

/**
 * \struct RoomInfo
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of RoomInfo values.
 */
typedef QList<RoomInfo> RoomInfoList;

/**
 * \struct SimplePresence
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct representing the presence of a contact.
 */
struct SimplePresence
{
    /**
     * The presence type, e.g. Connection_Presence_Type_Away.
     */
    uint type;
    /**
     * The string identifier of the status, e.g. &quot;brb&quot;, as defined 
     * in the Statuses property.
     */
    QString status;
    /**
     * <p>The user-defined status message, e.g. &quot;Back soon!&quot;.</p>
     * 
     * <p>Clients SHOULD set the status message for the local
     *   user to the empty string, unless the user has actually provided
     *   a specific message (i.e. one that conveys more information than the
     *   Status).</p>
     * 
     * <p>User interfaces SHOULD regard an empty status message as unset,
     *   and MAY replace it with a localized string corresponding to the
     *   Status or Type.</p>
     * 
     * <tp:rationale>
     *   Use case: Daf sets his status in Empathy by choosing the Welsh
     *   translation of &quot;Available&quot; from a menu.
     *   It is more informative for his English-speaking colleagues
     *   to see the English translation of &quot;Available&quot; (as localized
     *   by their own clients) than to see &quot;Ar Gael&quot; (which they don't
     *   understand anyway).
     * </tp:rationale>
     */
    QString statusMessage;
};

bool operator==(const SimplePresence& v1, const SimplePresence& v2);
inline bool operator!=(const SimplePresence& v1, const SimplePresence& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SimplePresence& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SimplePresence& val);

/**
 * \struct SimpleStatusSpec
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct containing information about a status.
 */
struct SimpleStatusSpec
{
    /**
     * The type of a presence. This SHOULD NOT be used as a way to set 
     * statuses that the client does not recognise (as explained in 
     * SetPresence), but MAY be used to check that the client&apos;s 
     * assumptions about a particular status name match the connection 
     * manager&apos;s.
     */
    uint type;
    /**
     * If true, the user can set this status on themselves using SetPresence.
     */
    bool maySetOnSelf;
    /**
     * If true, a non-empty message can be set for this status. Otherwise, the 
     * empty string is the only acceptable message. On IRC you can be Away 
     * with a status message, but if you are available you cannot set a status 
     * message.
     */
    bool canHaveMessage;
};

bool operator==(const SimpleStatusSpec& v1, const SimpleStatusSpec& v2);
inline bool operator!=(const SimpleStatusSpec& v1, const SimpleStatusSpec& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SimpleStatusSpec& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SimpleStatusSpec& val);

/**
 * \struct SingleContactAttributesMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 * Some of the attributes of a single contact.
 */
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

/**
 * \struct SocketAddressIP
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * An IP address and port.
 */
struct SocketAddressIP
{
    /**
     * Either a dotted-quad IPv4 address literal as for Socket_Address_IPv4, 
     * or an RFC2373 IPv6 address as for Socket_Address_IPv6.
     */
    QString address;
    /**
     * The TCP or UDP port number.
     */
    ushort port;
};

bool operator==(const SocketAddressIP& v1, const SocketAddressIP& v2);
inline bool operator!=(const SocketAddressIP& v1, const SocketAddressIP& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIP& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIP& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of SocketAddressIP values.
 */
typedef QList<SocketAddressIP> SocketAddressIPList;

/**
 * \struct SocketAddressIPv4
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * An IPv4 address and port.
 */
struct SocketAddressIPv4
{
    /**
     * A dotted-quad IPv4 address literal: four ASCII decimal numbers, each 
     * between 0 and 255 inclusive, e.g. &quot;192.168.0.1&quot;.
     */
    QString address;
    /**
     * The TCP or UDP port number.
     */
    ushort port;
};

bool operator==(const SocketAddressIPv4& v1, const SocketAddressIPv4& v2);
inline bool operator!=(const SocketAddressIPv4& v1, const SocketAddressIPv4& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIPv4& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIPv4& val);

/**
 * \struct SocketAddressIPv6
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * An IPv6 address and port.
 */
struct SocketAddressIPv6
{
    /**
     * An IPv6 address literal as specified by RFC2373 section 2.2, e.g. 
     * &quot;2001:DB8::8:800:200C:4171&quot;.
     */
    QString address;
    /**
     * The TCP or UDP port number.
     */
    ushort port;
};

bool operator==(const SocketAddressIPv6& v1, const SocketAddressIPv6& v2);
inline bool operator!=(const SocketAddressIPv6& v1, const SocketAddressIPv6& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SocketAddressIPv6& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SocketAddressIPv6& val);

/**
 * \struct SocketNetmaskIPv4
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * An IPv4 network or subnet.
 */
struct SocketNetmaskIPv4
{
    /**
     * A dotted-quad IPv4 address literal: four ASCII decimal numbers, each 
     * between 0 and 255 inclusive, e.g. &quot;192.168.0.1&quot;.
     */
    QString address;
    /**
     * The number of leading bits of the address that must match, for this 
     * netmask to be considered to match an address.
     */
    uchar prefixLength;
};

bool operator==(const SocketNetmaskIPv4& v1, const SocketNetmaskIPv4& v2);
inline bool operator!=(const SocketNetmaskIPv4& v1, const SocketNetmaskIPv4& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SocketNetmaskIPv4& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SocketNetmaskIPv4& val);

/**
 * \struct SocketNetmaskIPv6
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * An IPv6 network or subnet.
 */
struct SocketNetmaskIPv6
{
    /**
     * An IPv6 address literal as specified by RFC2373 section 2.2, e.g. 
     * &quot;2001:DB8::8:800:200C:4171&quot;.
     */
    QString address;
    /**
     * The number of leading bits of the address that must match, for this 
     * netmask to be considered to match an address.
     */
    uchar prefixLength;
};

bool operator==(const SocketNetmaskIPv6& v1, const SocketNetmaskIPv6& v2);
inline bool operator!=(const SocketNetmaskIPv6& v1, const SocketNetmaskIPv6& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const SocketNetmaskIPv6& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, SocketNetmaskIPv6& val);

/**
 * \struct StringStringMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QString>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A mapping from strings to strings representing extra key-value pairs.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of StringStringMap values.
 */
typedef QList<StringStringMap> StringStringMapList;

/**
 * \struct StringVariantMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, QDBusVariant>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A mapping from strings to variants representing extra key-value pairs.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of StringVariantMap values.
 */
typedef QList<StringVariantMap> StringVariantMapList;

/**
 * \struct SupportedSocketMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, UIntList>, but needed to have a discrete type in the Qt4 type system.
 * 
 * The supported socket address and access-control types for tubes. See 
 * GetAvailableStreamTubeTypes.
 */
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

/**
 * \struct TubeInfo
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A struct (tube ID, initiator handle, tube type, service name, parameters, 
 * state) representing a tube, as returned by ListTubes on the Tubes channel 
 * type.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of TubeInfo values.
 */
typedef QList<TubeInfo> TubeInfoList;

/**
 * \struct ContactCapabilitiesMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, RequestableChannelClassList>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A mapping from contact handle to their capabilities.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of ContactCapabilitiesMap values.
 */
typedef QList<ContactCapabilitiesMap> ContactCapabilitiesMapList;

/**
 * \struct ContactInfoMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, ContactInfoFieldList>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A dictionary whose keys are contact handles and whose values are contact 
 * information..
 */
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

/**
 * \struct HandlerCapabilities
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * A structure representing the capabilities of a single client.
 */
struct HandlerCapabilities
{
    /**
     * For implementations of the Client interface, the well-known bus name 
     * name of the client; for any other process, any other reversed domain 
     * name that uniquely identifies it.
     */
    QString wellKnownName;
    /**
     * An array of channel classes that can be handled by this client.
     * This will usually be a copy of the client's <tp:dbus-ref namespace="org.freedesktop.Telepathy.Client.Handler">HandlerChannelFilter</tp:dbus-ref>
     * property.
     */
    StringVariantMapList channelClasses;
    /**
     * An array of client capabilities supported by this client, to be
     * used by the connection manager to determine what capabilities to
     * advertise. This will usually be a copy of the client's <tp:dbus-ref namespace="org.freedesktop.Telepathy.Client.Handler">Capabilities</tp:dbus-ref>
     * property.
     */
    QStringList capabilities;
};

bool operator==(const HandlerCapabilities& v1, const HandlerCapabilities& v2);
inline bool operator!=(const HandlerCapabilities& v1, const HandlerCapabilities& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const HandlerCapabilities& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, HandlerCapabilities& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of HandlerCapabilities values.
 */
typedef QList<HandlerCapabilities> HandlerCapabilitiesList;

/**
 * \struct LastActivityAndStatuses
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * Structure representing a contact&apos;s presence, containing a 
 * last-activity time (deprecated) and a Multiple_Status_Map.
 */
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

/**
 * \struct MediaStreamHandlerCandidate
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 */
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

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MediaStreamHandlerCandidate values.
 */
typedef QList<MediaStreamHandlerCandidate> MediaStreamHandlerCandidateList;

/**
 * \struct MediaStreamHandlerCodec
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 * 
 * Information about a codec supported by a client or a peer&apos;s client.
 */
struct MediaStreamHandlerCodec
{
    /**
     * The codec&apos;s payload identifier, as per RFC 3551 (static or dynamic)
     */
    uint codecID;
    /**
     * The codec&apos;s name
     */
    QString name;
    /**
     * Type of stream this codec supports
     */
    uint mediaType;
    /**
     * Sampling frequency in Hertz
     */
    uint clockRate;
    /**
     * Number of supported channels
     */
    uint numberOfChannels;
    /**
     * Codec-specific optional parameters
     */
    StringStringMap parameters;
};

bool operator==(const MediaStreamHandlerCodec& v1, const MediaStreamHandlerCodec& v2);
inline bool operator!=(const MediaStreamHandlerCodec& v1, const MediaStreamHandlerCodec& v2)
{
    return !operator==(v1, v2);
}
QDBusArgument& operator<<(QDBusArgument& arg, const MediaStreamHandlerCodec& val);
const QDBusArgument& operator>>(const QDBusArgument& arg, MediaStreamHandlerCodec& val);

/**
 * \ingroup list
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Array of MediaStreamHandlerCodec values.
 */
typedef QList<MediaStreamHandlerCodec> MediaStreamHandlerCodecList;

/**
 * \struct SimpleContactPresences
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, SimplePresence>, but needed to have a discrete type in the Qt4 type system.
 * 
 * Mapping returned by GetPresences and signalled by PresencesChanged, 
 * indicating the presence of a number of contacts.
 */
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

/**
 * \struct SimpleStatusSpecMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, SimpleStatusSpec>, but needed to have a discrete type in the Qt4 type system.
 * 
 * A mapping describing possible statuses.
 */
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

/**
 * \struct StatusSpec
 * \ingroup struct
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Structure type generated from the specification.
 */
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

/**
 * \struct ContactPresences
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<uint, LastActivityAndStatuses>, but needed to have a discrete type in the Qt4 type system.
 * 
 * Mapping returned by GetPresence and signalled by PresenceUpdate, where the 
 * keys are contacts and the values represent their presences.
 */
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

/**
 * \struct StatusSpecMap
 * \ingroup mapping
 * \headerfile TelepathyQt4/types.h "../Types"
 *
 * Mapping type generated from the specification. Convertible with
 * QMap<QString, StatusSpec>, but needed to have a discrete type in the Qt4 type system.
 */
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

} // namespace Tp

Q_DECLARE_METATYPE(Tp::AliasMap)
Q_DECLARE_METATYPE(Tp::AliasPair)
Q_DECLARE_METATYPE(Tp::AliasPairList)
Q_DECLARE_METATYPE(Tp::Avatar)
Q_DECLARE_METATYPE(Tp::AvatarTokenMap)
Q_DECLARE_METATYPE(Tp::CapabilityChange)
Q_DECLARE_METATYPE(Tp::CapabilityChangeList)
Q_DECLARE_METATYPE(Tp::CapabilityPair)
Q_DECLARE_METATYPE(Tp::CapabilityPairList)
Q_DECLARE_METATYPE(Tp::ChannelCallStateMap)
Q_DECLARE_METATYPE(Tp::ChannelClass)
Q_DECLARE_METATYPE(Tp::ChannelClassList)
Q_DECLARE_METATYPE(Tp::ChannelDetails)
Q_DECLARE_METATYPE(Tp::ChannelDetailsList)
Q_DECLARE_METATYPE(Tp::ChannelInfo)
Q_DECLARE_METATYPE(Tp::ChannelInfoList)
Q_DECLARE_METATYPE(Tp::ContactAttributesMap)
Q_DECLARE_METATYPE(Tp::ContactCapabilitiesMap)
Q_DECLARE_METATYPE(Tp::ContactCapabilitiesMapList)
Q_DECLARE_METATYPE(Tp::ContactCapability)
Q_DECLARE_METATYPE(Tp::ContactCapabilityList)
Q_DECLARE_METATYPE(Tp::ContactInfoField)
Q_DECLARE_METATYPE(Tp::ContactInfoFieldList)
Q_DECLARE_METATYPE(Tp::ContactInfoMap)
Q_DECLARE_METATYPE(Tp::ContactLocations)
Q_DECLARE_METATYPE(Tp::ContactPresences)
Q_DECLARE_METATYPE(Tp::CurrencyAmount)
Q_DECLARE_METATYPE(Tp::DBusTubeMember)
Q_DECLARE_METATYPE(Tp::DBusTubeMemberList)
Q_DECLARE_METATYPE(Tp::FieldSpec)
Q_DECLARE_METATYPE(Tp::FieldSpecs)
Q_DECLARE_METATYPE(Tp::HandleIdentifierMap)
Q_DECLARE_METATYPE(Tp::HandleOwnerMap)
Q_DECLARE_METATYPE(Tp::HandlerCapabilities)
Q_DECLARE_METATYPE(Tp::HandlerCapabilitiesList)
Q_DECLARE_METATYPE(Tp::LastActivityAndStatuses)
Q_DECLARE_METATYPE(Tp::LocalPendingInfo)
Q_DECLARE_METATYPE(Tp::LocalPendingInfoList)
Q_DECLARE_METATYPE(Tp::Location)
Q_DECLARE_METATYPE(Tp::MediaSessionHandlerInfo)
Q_DECLARE_METATYPE(Tp::MediaSessionHandlerInfoList)
Q_DECLARE_METATYPE(Tp::MediaStreamHandlerCandidate)
Q_DECLARE_METATYPE(Tp::MediaStreamHandlerCandidateList)
Q_DECLARE_METATYPE(Tp::MediaStreamHandlerCodec)
Q_DECLARE_METATYPE(Tp::MediaStreamHandlerCodecList)
Q_DECLARE_METATYPE(Tp::MediaStreamHandlerTransport)
Q_DECLARE_METATYPE(Tp::MediaStreamHandlerTransportList)
Q_DECLARE_METATYPE(Tp::MediaStreamInfo)
Q_DECLARE_METATYPE(Tp::MediaStreamInfoList)
Q_DECLARE_METATYPE(Tp::MessagePart)
Q_DECLARE_METATYPE(Tp::MessagePartContentMap)
Q_DECLARE_METATYPE(Tp::MessagePartList)
Q_DECLARE_METATYPE(Tp::MessagePartListList)
Q_DECLARE_METATYPE(Tp::MultipleStatusMap)
Q_DECLARE_METATYPE(Tp::ObjectPathList)
Q_DECLARE_METATYPE(Tp::ParamSpec)
Q_DECLARE_METATYPE(Tp::ParamSpecList)
Q_DECLARE_METATYPE(Tp::PendingTextMessage)
Q_DECLARE_METATYPE(Tp::PendingTextMessageList)
Q_DECLARE_METATYPE(Tp::PropertyFlagsChange)
Q_DECLARE_METATYPE(Tp::PropertyFlagsChangeList)
Q_DECLARE_METATYPE(Tp::PropertySpec)
Q_DECLARE_METATYPE(Tp::PropertySpecList)
Q_DECLARE_METATYPE(Tp::PropertyValue)
Q_DECLARE_METATYPE(Tp::PropertyValueList)
Q_DECLARE_METATYPE(Tp::QualifiedPropertyValueMap)
Q_DECLARE_METATYPE(Tp::QualifiedPropertyValueMapList)
Q_DECLARE_METATYPE(Tp::RequestableChannelClass)
Q_DECLARE_METATYPE(Tp::RequestableChannelClassList)
Q_DECLARE_METATYPE(Tp::RichPresenceAccessControl)
Q_DECLARE_METATYPE(Tp::RoomInfo)
Q_DECLARE_METATYPE(Tp::RoomInfoList)
Q_DECLARE_METATYPE(Tp::SimpleContactPresences)
Q_DECLARE_METATYPE(Tp::SimplePresence)
Q_DECLARE_METATYPE(Tp::SimpleStatusSpec)
Q_DECLARE_METATYPE(Tp::SimpleStatusSpecMap)
Q_DECLARE_METATYPE(Tp::SingleContactAttributesMap)
Q_DECLARE_METATYPE(Tp::SocketAddressIP)
Q_DECLARE_METATYPE(Tp::SocketAddressIPList)
Q_DECLARE_METATYPE(Tp::SocketAddressIPv4)
Q_DECLARE_METATYPE(Tp::SocketAddressIPv6)
Q_DECLARE_METATYPE(Tp::SocketNetmaskIPv4)
Q_DECLARE_METATYPE(Tp::SocketNetmaskIPv6)
Q_DECLARE_METATYPE(Tp::StatusSpec)
Q_DECLARE_METATYPE(Tp::StatusSpecMap)
Q_DECLARE_METATYPE(Tp::StringStringMap)
Q_DECLARE_METATYPE(Tp::StringStringMapList)
Q_DECLARE_METATYPE(Tp::StringVariantMap)
Q_DECLARE_METATYPE(Tp::StringVariantMapList)
Q_DECLARE_METATYPE(Tp::SupportedSocketMap)
Q_DECLARE_METATYPE(Tp::TubeInfo)
Q_DECLARE_METATYPE(Tp::TubeInfoList)
Q_DECLARE_METATYPE(Tp::UIntList)

#endif //#ifndef CLI_TYPES_H
