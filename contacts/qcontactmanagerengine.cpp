/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactmanagerengine.h"

#include "qcontactdetaildefinition.h"
#include "qcontactdetails.h"
#include "qcontactsortorder.h"
#include "qcontactfilters.h"
#include "qcontactaction.h"
#include "qcontactactiondescriptor.h"
#include "qcontactabstractrequest.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactrequests.h"
#include "qcontactrequests_p.h"

/*!
 * \class QContactManagerEngine
 * \preliminary
 * \brief This class is the interface for all implementations of
 * the contact manager backend functionality.
 *
 * Instances of this class are usually provided by a
 * \l QContactManagerEngineFactory, which is loaded from a plugin.
 *
 * The default implementation of this interface provides a basic
 * level of functionality for some functions so that specific engines
 * can simply implement the functionality that is supported by
 * the specific contacts engine that is being adapted.
 *
 * More information on writing a contacts engine plugin is TODO.
 *
 * \sa QContactManager, QContactManagerEngineFactory
 */

/*!
 * \fn QContactManagerEngine::QContactManagerEngine()
 *
 * A default, empty constructor.
 */

/*!
 * \fn QContactManagerEngine::deref()
 *
 * Notifies the engine that it is no longer required.  If this
 * engine can not be shared between managers, it is safe for the
 * engine to delete itself in this function.
 *
 * If the engine implementation can be shared, this function can use a
 * reference count and track lifetime that way.  The factory that
 * returns an instance of this engine should increment the reference
 * count in this case.
 */

/*!
 * \fn QContactManagerEngine::dataChanged()
 *
 * This signal is emitted some time after changes occur to the data managed by this
 * engine, and the engine is unable to determine which changes occurred, or if the
 * engine considers the changes to be radical enough to require clients to reload all data.
 *
 * If this signal is emitted, no other signals may be emitted for the associated changes.
 *
 * As it is possible that other processes (or other devices) may have caused the
 * changes, the timing can not be determined.
 *
 * \sa contactsAdded(), contactsChanged(), contactsRemoved()
 */

/*!
 * \fn QContactManagerEngine::contactsAdded(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been added to
 * this engine where the \l dataChanged() signal was not emitted for those changes.
 * As it is possible that other processes (or other devices) may
 * have added the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 *
 * \sa dataChanged()
 */

/*!
 * \fn QContactManagerEngine::contactsChanged(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been modified in
 * this engine where the \l dataChanged() signal was not emitted for those changes.
 * As it is possible that other processes (or other devices) may
 * have modified the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 *
 * \sa dataChanged()
 */

/*!
 * \fn QContactManagerEngine::contactsRemoved(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been removed from
 * this engine where the \l dataChanged() signal was not emitted for those changes.
 * As it is possible that other processes (or other devices) may
 * have removed the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 *
 * \sa dataChanged()
 */

/*!
 * Returns the parameters with which this engine was constructed.  Note that
 * the engine may have discarded unused or invalid parameters at the time of
 * construction, and these will not be returned.
 */
QMap<QString, QString> QContactManagerEngine::parameters() const
{
    return QMap<QString, QString>(); // default implementation requires no parameters.
}

/*!
 * Return the list of contact ids present in this engine, sorted according to the given \a sortOrders.
 *
 * Any errors encountered should be stored to \a error.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    Q_UNUSED(sortOrders);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrders.
 * Any error that occurs will be stored in \a error.
 *
 * The default implementation will retrieve all contacts and test them with testFilter.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    /* Slow way */
    QList<QUniqueId> ret;

    /* Retrieve each contact.. . . */
    const QList<QUniqueId>& all = contacts(sortOrders, error);
    if (error != QContactManager::NoError)
        return ret;

    if (filter.type() == QContactFilter::DefaultFilter)
        return all;

    for (int j = 0; j < all.count(); j++) {
        if (testFilter(filter, contact(all.at(j), error)))
            ret << all.at(j);
    }

    return ret;
}

/*!
 * Returns a list of ids of contacts of the given \a contactType, sorted according to the given list of \a sortOrders.
 * Any error which occurs is saved in \a error.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QString& contactType, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    df.setValue(contactType);
    return contacts(df, sortOrders, error);
}

/*!
 * Returns the contact in the database identified by \a contactId
 *
 * Any errors encountered should be stored to \a error.
 */
QContact QContactManagerEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    Q_UNUSED(contactId);
    error = QContactManager::NotSupportedError;
    return QContact();
}

/*!
 * Sets the id of the "self" contact to the given \a contactId.
 * Returns true if the "self" contact id was set successfully.
 * If the given \a contactId does not identify a contact
 * stored in this manager, the \a error will be set to
 * \c QContactManager::DoesNotExistError and the function will
 * return false; if the backend does not support the
 * concept of a "self" contact, the \a error will be set to
 * \c QContactManager::NotSupportedError and the function will
 * return false.
 */
bool QContactManagerEngine::setSelfContactId(const QUniqueId& contactId, QContactManager::Error& error)
{
    Q_UNUSED(contactId);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Returns the id of the "self" contact which has previously been set.
 * If no "self" contact has been set, or if the self contact was removed
 * from the manager after being set, or if the backend does not support
 * the concept of a "self" contact, an invalid id will be returned
 * and the \a error will be set to \c QContactManager::DoesNotExistError.
 */
QUniqueId QContactManagerEngine::selfContactId(QContactManager::Error& error) const
{
    error = QContactManager::DoesNotExistError;
    return QUniqueId();
}

/*!
 * Synthesises the display label of the given \a contact in a platform specific manner.
 * Any error that occurs will be stored in \a error.
 * Returns the synthesised display label.
 */
QString QContactManagerEngine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    // synthesise the display name from the name of the contact, or, failing that, the organisation of the contact.
    error = QContactManager::NoError;
    QList<QContactDetail> allNames = contact.details(QContactName::DefinitionName);

    const QLatin1String space(" ");

    // synthesise the display label from the name.
    for (int i=0; i < allNames.size(); i++) {
        const QContactName& name = allNames.at(i);

        QString result;
        if (!name.value(QContactName::FieldPrefix).trimmed().isEmpty()) {
           result += name.value(QContactName::FieldPrefix);
        }

        if (!name.value(QContactName::FieldFirst).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldFirst);
        }

        if (!name.value(QContactName::FieldMiddle).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldMiddle);
        }

        if (!name.value(QContactName::FieldLast).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldLast);
        }

        if (!name.value(QContactName::FieldSuffix).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldSuffix);
        }

        if (!result.isEmpty()) {
            return result;
        }
    }

    /* Well, we had no non empty names. if we have orgs, fall back to those */
    QList<QContactDetail> allOrgs = contact.details(QContactOrganization::DefinitionName);
    for (int i=0; i < allOrgs.size(); i++) {
        const QContactOrganization& org = allOrgs.at(i);
        if (!org.name().isEmpty()) {
            return org.name();
        }
    }

    error = QContactManager::UnspecifiedError;
    return QString();
}

/*!
 * Returns true if the given \a feature is supported by this engine
 */
bool QContactManagerEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    Q_UNUSED(feature);
    return false;
}

/*!
 * Returns a whether the supplied \a filter can be implemented
 * natively by this engine.  If not, the base class implementation
 * will emulate the functionality.
 */
bool QContactManagerEngine::filterSupported(const QContactFilter& filter) const
{
    Q_UNUSED(filter);
    return false;
}

/*!
 * Returns the list of data types supported by this engine.
 */
QList<QVariant::Type> QContactManagerEngine::supportedDataTypes() const
{
    return QList<QVariant::Type>();
}

/*! Returns the base schema definitions */
QMap<QString, QContactDetailDefinition> QContactManagerEngine::schemaDefinitions()
{
    // This implementation provides the base schema.
    // The schema documentation (contactsschema.qdoc)
    // MUST BE KEPT UP TO DATE as definitions are added here.

    // the map we will eventually return
    QMap<QString, QContactDetailDefinition> retn;

    // local variables for reuse
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;
    QVariantList contexts;
    contexts << QString(QLatin1String(QContactDetail::ContextHome)) << QString(QLatin1String(QContactDetail::ContextWork)) << QString(QLatin1String(QContactDetail::ContextOther));
    QVariantList subTypes;

    // sync target
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactSyncTarget::DefinitionName);
    fields.insert(QContactSyncTarget::FieldSyncTarget, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(d.name(), d);

    // timestamp
    fields.clear();
    f.dataType = QVariant::DateTime;
    f.allowableValues = QVariantList();
    d.setName(QContactTimestamp::DefinitionName);
    fields.insert(QContactTimestamp::FieldModificationTimestamp, f);
    fields.insert(QContactTimestamp::FieldCreationTimestamp, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::ReadOnly);
    retn.insert(d.name(), d);

    // type
    fields.clear();
    f.dataType = QVariant::String;
    subTypes.clear();
    subTypes << QString(QLatin1String(QContactType::TypeContact)) << QString(QLatin1String(QContactType::TypeGroup)) << QString(QLatin1String(QContactType::TypeMetacontact));
    f.allowableValues = subTypes;
    d.setName(QContactType::DefinitionName);
    fields.insert(QContactType::FieldType, f); // note: NO CONTEXT!!
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // guid
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactGuid::DefinitionName);
    fields.insert(QContactGuid::FieldGuid, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(d.name(), d);

    // display label
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactDisplayLabel::DefinitionName);
    fields.insert(QContactDisplayLabel::FieldLabel, f);
    f.dataType = QVariant::Bool;
    f.allowableValues = QVariantList();
    fields.insert(QContactDisplayLabel::FieldSynthesised, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // email address
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactEmailAddress::DefinitionName);
    fields.insert(QContactEmailAddress::FieldEmailAddress, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // organisation
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactOrganization::DefinitionName);
    fields.insert(QContactOrganization::FieldName, f);
    fields.insert(QContactOrganization::FieldLogo, f);
    fields.insert(QContactOrganization::FieldLocation, f);
    fields.insert(QContactOrganization::FieldDepartment, f);
    fields.insert(QContactOrganization::FieldTitle, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // phone number
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactPhoneNumber::DefinitionName);
    fields.insert(QContactPhoneNumber::FieldNumber, f);
    f.dataType = QVariant::StringList; // can implement multiple subtypes
    subTypes.clear();
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeAssistant));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeCar));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeDtmfMenu));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeFacsimile));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeLandline));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeMessagingCapable));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeModem));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypePager));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeVideo));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeVoice));
    f.allowableValues = subTypes;
    fields.insert(QContactPhoneNumber::FieldSubTypes, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // anniversary
    fields.clear();
    f.dataType = QVariant::Date;
    f.allowableValues = QVariantList();
    d.setName(QContactAnniversary::DefinitionName);
    fields.insert(QContactAnniversary::FieldOriginalDate, f);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactAnniversary::FieldCalendarId, f);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactAnniversary::FieldEvent, f);
    f.dataType = QVariant::String; // only allowed to be a single subtype.
    subTypes.clear();
    subTypes << QString(QLatin1String(QContactAnniversary::SubTypeEmployment));
    subTypes << QString(QLatin1String(QContactAnniversary::SubTypeEngagement));
    subTypes << QString(QLatin1String(QContactAnniversary::SubTypeHouse));
    subTypes << QString(QLatin1String(QContactAnniversary::SubTypeMemorial));
    subTypes << QString(QLatin1String(QContactAnniversary::SubTypeWedding));
    f.allowableValues = subTypes;
    fields.insert(QContactAnniversary::FieldSubType, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // birthday
    fields.clear();
    f.dataType = QVariant::Date;
    f.allowableValues = QVariantList();
    d.setName(QContactBirthday::DefinitionName);
    fields.insert(QContactBirthday::FieldBirthday, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // nickname
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactNickname::DefinitionName);
    fields.insert(QContactNickname::FieldNickname, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // url
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactUrl::DefinitionName);
    fields.insert(QContactUrl::FieldUrl, f);
    f.dataType = QVariant::String; // only allowed to be a single subtype
    subTypes.clear();
    subTypes << QString(QLatin1String(QContactUrl::SubTypeFavourite));
    subTypes << QString(QLatin1String(QContactUrl::SubTypeHomePage));
    f.allowableValues = subTypes;
    fields.insert(QContactUrl::FieldSubType, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // gender
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList() << QString(QLatin1String(QContactGender::GenderMale)) << QString(QLatin1String(QContactGender::GenderFemale)) << QString(QLatin1String(QContactGender::GenderUnspecified));
    d.setName(QContactGender::DefinitionName);
    fields.insert(QContactGender::FieldGender, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // online account
    fields.clear();
    f.dataType = QVariant::String;
    d.setName(QContactOnlineAccount::DefinitionName);
    fields.insert(QContactOnlineAccount::FieldAccountUri, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    f.allowableValues = QVariantList(); // allow any subtypes!
    fields.insert(QContactOnlineAccount::FieldSubTypes, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // presence
    fields.clear();
    f.dataType = QVariant::String;
    d.setName(QContactPresence::DefinitionName);
    fields.insert(QContactPresence::FieldAccountUri, f);
    fields.insert(QContactPresence::FieldNickname, f);
    fields.insert(QContactPresence::FieldStatusMessage, f);
    QVariantList presenceValues;
    presenceValues << QString(QLatin1String(QContactPresence::PresenceAvailable));
    presenceValues << QString(QLatin1String(QContactPresence::PresenceHidden));
    presenceValues << QString(QLatin1String(QContactPresence::PresenceBusy));
    presenceValues << QString(QLatin1String(QContactPresence::PresenceAway));
    presenceValues << QString(QLatin1String(QContactPresence::PresenceExtendedAway));
    presenceValues << QString(QLatin1String(QContactPresence::PresenceUnknown));
    presenceValues << QString(QLatin1String(QContactPresence::PresenceOffline));
    f.allowableValues = presenceValues;
    fields.insert(QContactPresence::FieldAccountUri, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::ReadOnly);
    retn.insert(d.name(), d);

    // relationship
    fields.clear();
    QVariantList relationshipTypes;
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeHasMember));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeIsMemberOf));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeAggregates));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeAggregatedBy));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeIs));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeWas));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeParent));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeFather));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeMother));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeChild));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeSon));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeDaughter));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeSibling));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeBrother));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeSister));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeCousin));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeFriend));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeSpouse));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeFiance));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeFiancee));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeHusband));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeWife));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeWidower));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeWidow));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeEmployer));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeEmployee));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeOther));
    relationshipTypes << QString(QLatin1String(QContactRelationship::RelationshipTypeUnknown));
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactRelationship::DefinitionName);
    fields.insert(QContactRelationship::FieldRelatedContactUid, f);
    fields.insert(QContactRelationship::FieldRelatedContactManagerUri, f);
    f.allowableValues = relationshipTypes;
    fields.insert(QContactRelationship::FieldRelationshipType, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // avatar
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactAvatar::DefinitionName);
    fields.insert(QContactAvatar::FieldAvatar, f);
    f.dataType = QVariant::String; // only allowed to be a single subtype
    subTypes.clear();
    subTypes << QString(QLatin1String(QContactAvatar::SubTypeImage));
    subTypes << QString(QLatin1String(QContactAvatar::SubTypeTexturedMesh));
    subTypes << QString(QLatin1String(QContactAvatar::SubTypeVideo));
    f.allowableValues = subTypes;
    fields.insert(QContactAvatar::FieldSubType, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // geolocation
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactGeolocation::DefinitionName);
    fields.insert(QContactGeolocation::FieldLabel, f);
    f.dataType = QVariant::Double;
    fields.insert(QContactGeolocation::FieldLatitude, f);
    fields.insert(QContactGeolocation::FieldLongitude, f);
    fields.insert(QContactGeolocation::FieldAccuracy, f);
    fields.insert(QContactGeolocation::FieldAltitude, f);
    fields.insert(QContactGeolocation::FieldAltitudeAccuracy, f);
    fields.insert(QContactGeolocation::FieldSpeed, f);
    fields.insert(QContactGeolocation::FieldHeading, f);
    f.dataType = QVariant::DateTime;
    fields.insert(QContactGeolocation::FieldTimestamp, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // street address
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactAddress::DefinitionName);
    fields.insert(QContactAddress::FieldPostOfficeBox, f);
    fields.insert(QContactAddress::FieldStreet, f);
    fields.insert(QContactAddress::FieldLocality, f);
    fields.insert(QContactAddress::FieldRegion, f);
    fields.insert(QContactAddress::FieldPostcode, f);
    fields.insert(QContactAddress::FieldCountry, f);
    f.dataType = QVariant::StringList; // can implement multiple subtypes
    subTypes.clear();
    subTypes << QString(QLatin1String(QContactAddress::SubTypeDomestic));
    subTypes << QString(QLatin1String(QContactAddress::SubTypeInternational));
    subTypes << QString(QLatin1String(QContactAddress::SubTypeParcel));
    subTypes << QString(QLatin1String(QContactAddress::SubTypePostal));
    f.allowableValues = subTypes;
    fields.insert(QContactAddress::FieldSubTypes, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    // name
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setName(QContactName::DefinitionName);
    fields.insert(QContactName::FieldPrefix, f);
    fields.insert(QContactName::FieldFirst, f);
    fields.insert(QContactName::FieldMiddle, f);
    fields.insert(QContactName::FieldLast, f);
    fields.insert(QContactName::FieldSuffix, f);
    f.dataType = QVariant::StringList;
    f.allowableValues = contexts;
    fields.insert(QContactDetail::FieldContext, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    retn.insert(d.name(), d);

    return retn;
}


/*!
 * Add the given \a contact to the database if it is a new contact,
 * or updates the existing contact. Returns false on failure, or true on
 * success.  If successful and the contact was a new contact, its UID should
 * be set to a new, valid id.
 *
 * If the \a contact contains one or more details whose definitions have
 * not yet been saved with the manager, the operation will fail and the
 * manager will return \c QContactManager::UnsupportedError.
 *
 * This function is called by the contacts framework in both the
 * single contact save and batch contact save, if the saveContacts
 * function is not overridden.
 *
 * The backend must emit the appropriate signals to inform clients of changes
 * to the database resulting from this operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    Q_UNUSED(contact);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Checks that the given contact \a contact does not have details which
 * don't conform to a valid definition, violate uniqueness constraints,
 * or contain values for nonexistent fields, and that the values contained are
 * of the correct type for each field, and are allowable values for that field.
 *
 * Note that this function is unable to ensure that the access constraints
 * (such as CreateOnly and ReadOnly) are observed; backend specific code
 * must be written if you wish to enforce these constraints.
 *
 * Returns true if the \a contact is valid according to the definitions for
 * its details, otherwise returns false.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::validateContact(const QContact& contact, QContactManager::Error& error) const
{
    QList<QString> uniqueDefinitionIds;

    // check that each detail conforms to its definition as supported by this manager.
    for (int i=0; i < contact.details().count(); i++) {
        const QContactDetail& d = contact.details().at(i);
        QVariantMap values = d.values();
        QContactDetailDefinition def = detailDefinition(d.definitionName(), error);

        // check that the definition is supported
        if (error != QContactManager::NoError) {
            error = QContactManager::InvalidDetailError;
            return false; // this definition is not supported.
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.name())) {
                error = QContactManager::AlreadyExistsError;
                return false; // can't have two of a unique detail.
            }
            uniqueDefinitionIds.append(def.name());
        }

        QList<QString> keys = values.keys();
        for (int i=0; i < keys.count(); i++) {
            const QString& key = keys.at(i);
            // check that no values exist for nonexistent fields.
            if (!def.fields().contains(key)) {
                error = QContactManager::InvalidDetailError;
                return false; // value for nonexistent field.
            }

            QContactDetailDefinition::Field field = def.fields().value(key);
            // check that the type of each value corresponds to the allowable field type
            if (field.dataType != values.value(key).type()) {
                error = QContactManager::InvalidDetailError;
                return false; // type doesn't match.
            }

            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues.isEmpty()) {
                // if the field datatype is a list, check that it contains only allowable values
                if (field.dataType == QVariant::List || field.dataType == QVariant::StringList) {
                    QList<QVariant> innerValues = values.value(key).toList();
                    for (int i = 0; i < innerValues.size(); i++) {
                        if (!field.allowableValues.contains(innerValues.at(i))) {
                            error = QContactManager::InvalidDetailError;
                            return false; // value not allowed.
                        }
                    }
                } else if (!field.allowableValues.contains(values.value(key))) {
                    // the datatype is not a list; the value wasn't allowed.
                    error = QContactManager::InvalidDetailError;
                    return false; // value not allowed.
                }
            }
        }
    }

    return true;
}


/*!
 * Checks that the given detail definition \a definition seems valid,
 * with a correct id, defined fields, and any specified value types
 * are supported by this engine.  This function is called before
 * trying to save a definition.
 *
 * Note that a ReadOnly definition will fail validation, since
 * they are intended to be provided by an engine directly.
 *
 * Returns true if the \a definition seems valid, otherwise returns
 * false.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::validateDefinition(const QContactDetailDefinition& definition, QContactManager::Error& error) const
{
    if (definition.name().isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    if (definition.fields().count() == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    if (definition.accessConstraint() == QContactDetailDefinition::ReadOnly) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // Check each field now
    QList<QVariant::Type> types = supportedDataTypes();
    QMapIterator<QString, QContactDetailDefinition::Field> it(definition.fields());
    while(it.hasNext()) {
        it.next();
        if (it.key().isEmpty()) {
            error = QContactManager::BadArgumentError;
            return false;
        }

        if (!types.contains(it.value().dataType)) {
            error = QContactManager::BadArgumentError;
            return false;
        }

        // Check that each allowed value is the same type
        for (int i=0; i < it.value().allowableValues.count(); i++) {
            if (it.value().allowableValues.at(i).type() != it.value().dataType) {
                error = QContactManager::BadArgumentError;
                return false;
            }
        }
    }
    error = QContactManager::NoError;
    return true;
}

/*!
 * Remove the contact identified by \a contactId from the database.
 * Returns true if the contact was removed successfully, otherwise
 * returns false.
 *
 * The backend must emit the appropriate signals to inform clients of changes
 * to the database resulting from this operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeContact(const QUniqueId& contactId, QContactManager::Error& error)
{
    Q_UNUSED(contactId);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Returns the registered detail definitions which are valid for contacts in this engine.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QMap<QString, QContactDetailDefinition> QContactManagerEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NotSupportedError;
    return QMap<QString, QContactDetailDefinition>();
}

/*!
 * Returns the definition identified by the given \a definitionName that
 * is valid for contacts in this store, or a default-constructed QContactDetailDefinition
 * if no such definition exists
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QContactDetailDefinition QContactManagerEngine::detailDefinition(const QString& definitionName, QContactManager::Error& error) const
{
    Q_UNUSED(definitionName);

    QMap<QString, QContactDetailDefinition> definitions = detailDefinitions(error);
    if (definitions.contains(definitionName))  {
        error = QContactManager::NoError;
        return definitions.value(definitionName);
    } else {
        error = QContactManager::DoesNotExistError;
        return QContactDetailDefinition();
    }
}

/*!
 * Persists the given definition \a def in the database.
 *
 * Returns true if the definition was saved successfully, and otherwise returns false.
 *
 * The backend must emit the appropriate signals to inform clients of changes
 * to the database resulting from this operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    Q_UNUSED(def);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Removes the definition identified by the given \a definitionName from the database.
 *
 * Returns true if the definition was removed successfully, otherwise returns false.
 *
 * The backend must emit the appropriate signals to inform clients of changes
 * to the database resulting from this operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeDetailDefinition(const QString& definitionName, QContactManager::Error& error)
{
    Q_UNUSED(definitionName);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Adds the list of contacts given by \a contacts to the database.
 * Returns a list of the error codes corresponding to the contacts in
 * the \a contacts.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were saved
 * successfully.
 *
 * For each newly saved contact that was successful, the uid of the contact
 * in the list will be updated with the new value.  If a failure occurs
 * when saving a new contact, the id will be cleared.  If a failure occurs
 * when updating a contact that already exists, then TODO.
 *
 * The backend must emit the appropriate signals to inform clients of changes
 * to the database resulting from this operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 *
 * \sa QContactManager::saveContact()
 */
QList<QContactManager::Error> QContactManagerEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contacts) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contacts->count(); i++) {
            QContact current = contacts->at(i);
            if (!saveContact(&current, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contacts)[i] = current;
                ret.append(QContactManager::NoError);
            }
        }

        error = functionError;
        return ret;
    }
}

/*!
 * Remove the list of contacts identified in \a contactIds.
 * Returns a list of the error codes corresponding to the contact ids in
 * the \a contactIds.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were removed
 * successfully.
 *
 * For each contact that was removed succesfully, the corresponding
 * id in the list will be retained but set to zero.  The id of contacts
 * that were not successfully removed will be left alone.
 *
 * The backend must emit the appropriate signals to inform clients of changes
 * to the database resulting from this operation.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 *
 * \sa QContactManager::removeContact()
 */
QList<QContactManager::Error> QContactManagerEngine::removeContacts(QList<QUniqueId>* contactIds, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QList<QUniqueId> removedList;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contactIds->count(); i++) {
            QUniqueId current = contactIds->at(i);
            if (!removeContact(current, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contactIds)[i] = 0;
                ret.append(QContactManager::NoError);
            }
        }

        error = functionError;
        return ret;
    }
}

/*!
 * Compares \a first against \a second.  If the types are
 * strings (QVariant::String), the \a sensitivity argument controls
 * case sensitivity when comparing.
 *
 * Returns:
 * <0 if \a first is less than \a second
 *  0 if \a first is equal to \a second
 * >0 if \a first is greater than \a second.
 *
 * The results are undefined if the variants are different types, or
 * cannot be compared.
 */
int QContactManagerEngine::compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity)
{
    switch(first.type()) {
        case QVariant::Int:
            return first.toInt() - second.toInt();

        case QVariant::LongLong:
            return first.toLongLong() - second.toLongLong();

        case QVariant::Bool:
        case QVariant::Char:
        case QVariant::UInt:
            return first.toUInt() - second.toUInt();

        case QVariant::ULongLong:
            return first.toULongLong() - second.toULongLong();

       case QVariant::String:
            return first.toString().compare(second.toString(), sensitivity);

        case QVariant::Double:
            {
                const double a = first.toDouble();
                const double b = second.toDouble();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        case QVariant::DateTime:
            {
                const QDateTime a = first.toDateTime();
                const QDateTime b = second.toDateTime();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        case QVariant::Date:
            return first.toDate().toJulianDay() - second.toDate().toJulianDay();

        case QVariant::Time:
            {
                const QTime a = first.toTime();
                const QTime b = second.toTime();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        default:
            return 0;
    }
}
/*!
 * Returns true if the supplied \a filter matches the supplied \a contact.
 *
 * This function will test each condition in the filter, possibly recursing.
 */
bool QContactManagerEngine::testFilter(const QContactFilter &filter, const QContact &contact)
{
    switch(filter.type()) {
        case QContactFilter::InvalidFilter:
            return false;

        case QContactFilter::DefaultFilter:
            return true;

        case QContactFilter::IdListFilter:
            {
                const QContactIdListFilter idf(filter);
                if (idf.ids().contains(contact.id()))
                    return true;
            }
            // Fall through to end
            break;

        case QContactFilter::ContactDetailFilter:
            {
                const QContactDetailFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false;

                /* See if this contact has one of these details in it */
                const QList<QContactDetail>& details = contact.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* See if we need to check the values */
                if (cdf.detailFieldName().isEmpty())
                    return true;  /* just testing for the presence of a detail of the specified definition */

                /* Now figure out what tests we are doing */
                const bool valueTest = cdf.value().isValid();
                const bool presenceTest = !valueTest;

                /* See if we need to test any values at all */
                if (presenceTest) {
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);

                        /* Check that the field is present and has a non-empty value */
                        if (detail.values().contains(cdf.detailFieldName()) && !detail.value(cdf.detailFieldName()).isEmpty())
                            return true;
                    }
                    return false;
                }

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & Qt::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (Qt::MatchEndsWith | Qt::MatchStartsWith | Qt::MatchContains | Qt::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    bool matchStarts = (cdf.matchFlags() & 7) == Qt::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == Qt::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == Qt::MatchContains;

                    /* Value equality test */
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        const QString& needle = cdf.value().toString();
                        if (matchStarts && var.startsWith(needle, cs))
                            return true;
                        if (matchEnds && var.endsWith(needle, cs))
                            return true;
                        if (matchContains && var.contains(needle, cs))
                            return true;
                        if (QString::compare(var, needle, cs) == 0)
                            return true;
                    }
                    return false;
                } else {
                    /* Nope, testing the values as a variant */
                    /* Value equality test */
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QVariant& var = detail.variantValue(cdf.detailFieldName());
                        if (!var.isNull() && compareVariant(var, cdf.value(), cs) == 0)
                            return true;
                    }
                }
            }
            break;

        case QContactFilter::ContactDetailRangeFilter:
            {
                const QContactDetailRangeFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false; /* we do not know which field to check */

                /* See if this contact has one of these details in it */
                const QList<QContactDetail>& details = contact.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* Check for a detail presence test */
                if (cdf.detailFieldName().isEmpty())
                    return true;

                /* See if this is a field presence test */
                if (!cdf.minValue().isValid() && !cdf.maxValue().isValid()) {
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        if (detail.values().contains(cdf.detailFieldName()))
                            return true;
                    }
                    return false;
                }

                /* open or closed interval testing support */
                const int minComp = cdf.rangeFlags() & QContactDetailRangeFilter::ExcludeLower ? 1 : 0;
                const int maxComp = cdf.rangeFlags() & QContactDetailRangeFilter::IncludeUpper ? 1 : 0;

                const bool testMin = cdf.minValue().isValid();
                const bool testMax = cdf.maxValue().isValid();

                /* At this point we know that at least of testMin & testMax is true */

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & Qt::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (Qt::MatchEndsWith | Qt::MatchStartsWith | Qt::MatchContains | Qt::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    //bool matchStarts = (cdf.matchFlags() & 7) == Qt::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == Qt::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == Qt::MatchContains;

                    /* Min/Max and contains do not make sense */
                    if (matchContains)
                        return false;

                    QString minVal = cdf.minValue().toString();
                    QString maxVal = cdf.maxValue().toString();

                    /* Starts with is the normal compare case, endsWith is a bit trickier */
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        if (!matchEnds) {
                            // MatchStarts or MatchFixedString
                            if (testMin && QString::compare(var, minVal, cs) < minComp)
                                continue;
                            if (testMax && QString::compare(var, maxVal, cs) >= maxComp)
                                continue;
                            return true;
                        } else {
                            /* Have to test the length of min & max */
                            // using refs means the parameter order is backwards, so negate the result of compare
                            if (testMin && -QString::compare(minVal, var.rightRef(minVal.length()), cs) < minComp)
                                continue;
                            if (testMax && -QString::compare(maxVal, var.rightRef(maxVal.length()), cs) >= maxComp)
                                continue;
                            return true;
                        }
                    }
                    // Fall through to end
                } else {
                    /* Nope, testing the values as a variant */
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        const QVariant& var = detail.variantValue(cdf.detailFieldName());

                        if (testMin && compareVariant(var, cdf.minValue(), cs) < minComp)
                            continue;
                        if (testMax && compareVariant(var, cdf.maxValue(), cs) >= maxComp)
                            continue;
                        return true;
                    }
                    // Fall through to end
                }
            }
            break;

        case QContactFilter::GroupMembershipFilter:
            {
                // check the specified group for membership.
                const QContactGroupMembershipFilter cgf(filter);

                // oldcode:
                //if (contact.groups().contains(cgf.groupId()))
                //    return true;
                // newcode:
                //if (contact.containsRelationshipDetail(is, cfg.groupId())) return true.

                // Fall through to end
            }
            break;

        case QContactFilter::ChangeLogFilter:
            {
                QContactChangeLogFilter ccf(filter);

                // See what we can do...
                QContactTimestamp ts = contact.detail(QContactTimestamp::DefinitionName);

                // See if timestamps are even supported
                if (ts.isEmpty())
                    break;

                if (ccf.eventType() == QContactChangeLogFilter::EventAdded)
                    return ccf.since() <= ts.created();
                if (ccf.eventType() == QContactChangeLogFilter::EventChanged)
                    return ccf.since() <= ts.lastModified();

                // You can't emulate a removed..
                // Fall through
            }
            break;

        case QContactFilter::ActionFilter:
            {
                // Find any matching actions, and do a union filter on their filter objects
                QContactActionFilter af(filter);
                QList<QContactActionDescriptor> descriptors = QContactAction::actionDescriptors(af.actionName(), af.vendorName(), af.implementationVersion());

                // There's a small wrinkle if there's a value specified in the action filter
                // we have to adjust any contained QContactDetailFilters to have that value
                // or test if a QContactDetailRangeFilter contains this value already
                for (int j = 0; j < descriptors.count(); j++) {
                    QContactAction* action = QContactAction::action(descriptors.at(j));

                    // Action filters are not allowed to return action filters, at all
                    // it's too annoying to check for recursion
                    QContactFilter d = action->contactFilter(af.value());
                    delete action; // clean up.
                    if (!validateActionFilter(d))
                        return false;

                    // Check for values etc...
                    if (testFilter(d, contact))
                        return true;
                }
                // Fall through to end
            }
            break;

        case QContactFilter::IntersectionFilter:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QContactIntersectionFilter bf(filter);
                const QList<QContactFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (!testFilter(terms.at(j), contact)) {
                            return false;
                        }
                    }
                    return true;
                }
                // Fall through to end
            }
            break;

        case QContactFilter::UnionFilter:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QContactUnionFilter bf(filter);
                const QList<QContactFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (testFilter(terms.at(j), contact)) {
                            return true;
                        }
                    }
                    return false;
                }
                // Fall through to end
            }
            break;
    }
    return false;
}

/*!
 * Given a QContactFilter \a filter retrieved from a QContactAction,
 * check that it is valid and cannot cause infinite recursion.
 *
 * In particular, a filter from a QContactAction cannot contain
 * any instances of a QContactActionFilter.
 *
 * Returns true if \a filter seems ok, or false otherwise.
 */

bool QContactManagerEngine::validateActionFilter(const QContactFilter& filter)
{
    QList<QContactFilter> toVerify;
    toVerify << filter;

    while(toVerify.count() > 0) {
        QContactFilter f = toVerify.takeFirst();
        if (f.type() == QContactFilter::ActionFilter)
            return false;
        if (f.type() == QContactFilter::IntersectionFilter)
            toVerify.append(QContactIntersectionFilter(f).filters());
        if (f.type() == QContactFilter::UnionFilter)
            toVerify.append(QContactUnionFilter(f).filters());
    }

    return true;
}


/*!
 * Compares two contacts (\a a and \a b) using the given list of \a sortOrders.  Returns a negative number if \a a should appear
 * before \a b according to the sort order, a positive number if \a a should appear after \a b according to the sort order,
 * and zero if the two are unable to be sorted.
 */
int QContactManagerEngine::compareContact(const QContact& a, const QContact& b, const QList<QContactSortOrder>& sortOrders)
{
    QList<QContactSortOrder> copy = sortOrders;
    while (copy.size()) {
        // retrieve the next sort order in the list
        QContactSortOrder sortOrder = copy.takeFirst();
        if (!sortOrder.isValid())
            break;

        // obtain the values which this sort order concerns
        const QVariant& aVal = a.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());
        const QVariant& bVal = b.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());

        // early exit error checking
        if (aVal.isNull())
            return (sortOrder.blankPolicy() == QContactSortOrder::BlanksFirst ? -1 : 1);
        if (bVal.isNull())
            return (sortOrder.blankPolicy() == QContactSortOrder::BlanksFirst ? 1 : -1);

        // real comparison
        int comparison = compareVariant(aVal, bVal, sortOrder.caseSensitivity()) * (sortOrder.direction() == Qt::AscendingOrder ? 1 : -1);
        if (comparison == 0)
            continue;
        return comparison;
    }

    return 0; // or according to id? return (a.id() < b.id() ? -1 : 1);
}


/*!
 * Performs insertion sort of the contact \a toAdd into the \a sorted list, according to the provided \a sortOrders list.
 * The first QContactSortOrder in the list has the highest priority; if the contact \a toAdd is deemed equal to another
 * in the \a sorted list, the second QContactSortOrder in the list is used (and so on until either the contact is inserted
 * or there are no more sort order objects in the list).
 */
void QContactManagerEngine::addSorted(QList<QContact>* sorted, const QContact& toAdd, const QList<QContactSortOrder>& sortOrders)
{
    for (int i = 0; i < sorted->size(); i++) {
        // check to see if the new contact should be inserted here
        int comparison = compareContact(sorted->at(i), toAdd, sortOrders);
        if (comparison > 0) {
            sorted->insert(i, toAdd);
            return;
        }
    }

    // hasn't been inserted yet?  append to the list.
    sorted->append(toAdd);
}

/*!
 * Notifies the manager engine that the given request \a req has been destroyed
 */
void QContactManagerEngine::requestDestroyed(QContactAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*!
 * Asks the manager engine to begin the given request \a req which
 * is currently in a (re)startable state.
 * Returns true if the request was started successfully, else returns false.
 *
 * \sa QContactAbstractRequest::start()
 */
bool QContactManagerEngine::startRequest(QContactAbstractRequest* req)
{
    Q_UNUSED(req);
    return false;
}

/*!
 * Asks the manager engine to cancel the given request \a req which was
 * previously started and is currently in a cancellable state.
 * Returns true if cancellation of the request was started successfully,
 * otherwise returns false.
 *
 * \sa startRequest(), QContactAbstractRequest::cancel()
 */
bool QContactManagerEngine::cancelRequest(QContactAbstractRequest* req)
{
    Q_UNUSED(req);
    return false;
}

/*!
 * Blocks until the manager engine has completed some part (or all) of the given request \a req
 * which was previously started, or until \a msecs milliseconds have passed.
 * Returns true if some progress was reported, and false if the request was not in the
 * \c QContactAbstractRequest::Active state or no progress could be reported.
 *
 * \sa startRequest()
 */
bool QContactManagerEngine::waitForRequestProgress(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);
    return false;
}

/*!
 * Blocks until the manager engine has completed the given request \a req
 * which was previously started, or until \a msecs milliseconds have passed.
 * Returns true if the request was completed, and false if the request was not in the
 * \c QContactAbstractRequest::Active state or no progress could be reported.
 *
 * \sa startRequest()
 */
bool QContactManagerEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);
    return false;
}

/*!
 * Updates the given asynchronous request \a req by setting the overall operation \a error, any individual \a errors that occurred during the operation, and the new \a status of the request.  It then causes the progress signal to be emitted by the request, with the \a appendOnly flag set (if required) to indicate result ordering stability.
 */
void QContactManagerEngine::updateRequestStatus(QContactAbstractRequest* req, QContactManager::Error error, QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly)
{
    // convenience function that simply sets the operation error and status
    req->d_ptr->m_error = error;
    req->d_ptr->m_errors = errors;
    req->d_ptr->m_status = status;

    switch (req->type()) {
        case QContactAbstractRequest::ContactFetchRequest:
        {
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(req);
            emit r->progress(r, appendOnly);
        }
        break;

        case QContactAbstractRequest::ContactIdFetchRequest:
        {
            QContactIdFetchRequest* r = static_cast<QContactIdFetchRequest*>(req);
            emit r->progress(r, appendOnly);
        }
        break;

        case QContactAbstractRequest::ContactSaveRequest:
        {
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(req);
            emit r->progress(r);
        }
        break;

        case QContactAbstractRequest::ContactRemoveRequest:
        {
            QContactRemoveRequest* r = static_cast<QContactRemoveRequest*>(req);
            emit r->progress(r);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionFetchRequest:
        {
            QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(req);
            emit r->progress(r, appendOnly);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionSaveRequest:
        {
            QContactDetailDefinitionSaveRequest* r = static_cast<QContactDetailDefinitionSaveRequest*>(req);
            emit r->progress(r);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QContactDetailDefinitionRemoveRequest* r = static_cast<QContactDetailDefinitionRemoveRequest*>(req);
            emit r->progress(r);
        }
        // fall through.

        default: // unknown request type.
        break;
    }
}

/*!
 * Updates the given asynchronous request \a req by setting its \a result, the overall operation \a error, any individual \a errors that occurred during the operation, and the new \a status of the request.  It then causes the progress signal to be emitted by the request, with the \a appendOnly flag set (if required) to indicate result ordering stability.  If the request is of a type which does not return a list of unique ids as a result, this function will return without doing anything.
 */
void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QUniqueId>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly)
{
    if (req->type() == QContactAbstractRequest::ContactIdFetchRequest) {
        req->d_ptr->m_error = error;
        req->d_ptr->m_errors = errors;
        req->d_ptr->m_status = status;
        QContactIdFetchRequestPrivate* rd = static_cast<QContactIdFetchRequestPrivate*>(req->d_ptr);
        rd->m_ids = result;
        QContactIdFetchRequest* r = static_cast<QContactIdFetchRequest*>(req);
        emit r->progress(r, appendOnly);
    }
}

/*!
 * Updates the given asynchronous request \a req by setting its \a result, the overall operation \a error, any individual \a errors that occurred during the operation, and the new \a status of the request.  It then causes the progress signal to be emitted by the request, with the \a appendOnly flag set (if required) to indicate result ordering stability. If the request is of a type which does not return a list of contacts as a result, this function will return without doing anything.
 */
void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QContact>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly)
{
    switch (req->type()) {
        case QContactAbstractRequest::ContactFetchRequest:
        {
            req->d_ptr->m_error = error;
            req->d_ptr->m_errors = errors;
            req->d_ptr->m_status = status;
            QContactFetchRequestPrivate* rd = static_cast<QContactFetchRequestPrivate*>(req->d_ptr);
            rd->m_contacts = result;
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(req);
            emit r->progress(r, appendOnly);
        }
        break;

        case QContactAbstractRequest::ContactSaveRequest:
        {
            req->d_ptr->m_error = error;
            req->d_ptr->m_errors = errors;
            req->d_ptr->m_status = status;
            QContactSaveRequestPrivate* rd = static_cast<QContactSaveRequestPrivate*>(req->d_ptr);
            rd->m_contacts = result;
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(req);
            emit r->progress(r);
        }
        break;

        default:
        {
            // this request type does not have a list of contacts to update...
            return;
        }
    }
}

/*!
 * Updates the given asynchronous request \a req by setting its \a result, the overall operation \a error, any individual \a errors that occurred during the operation, and the new \a status of the request.  It then causes the progress signal to be emitted by the request.  If the request is of a type which does not return a list of detail definition as a result, this function will return without doing anything.
 */
void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QContactDetailDefinition>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status)
{
    if (req->type() == QContactAbstractRequest::DetailDefinitionSaveRequest) {
        req->d_ptr->m_error = error;
        req->d_ptr->m_errors = errors;
        req->d_ptr->m_status = status;
        QContactDetailDefinitionSaveRequestPrivate* rd = static_cast<QContactDetailDefinitionSaveRequestPrivate*>(req->d_ptr);
        rd->m_definitions = result;
        QContactDetailDefinitionSaveRequest* r = static_cast<QContactDetailDefinitionSaveRequest*>(req);
        emit r->progress(r);
    }
}

/*!
 * Updates the given asynchronous request \a req by setting its \a result, the overall operation \a error, any individual \a errors that occurred during the operation, and the new \a status of the request.  It then causes the progress signal to be emitted by the request, with the \a appendOnly flag set (if required) to indicate result ordering stability.  If the request is of a type which does not return a map of string to detail definition as a result, this function will return without doing anything.
 */
void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QMap<QString, QContactDetailDefinition>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly)
{
    if (req->type() == QContactAbstractRequest::DetailDefinitionFetchRequest) {
        req->d_ptr->m_error = error;
        req->d_ptr->m_errors = errors;
        req->d_ptr->m_status = status;
        QContactDetailDefinitionFetchRequestPrivate* rd = static_cast<QContactDetailDefinitionFetchRequestPrivate*>(req->d_ptr);
        rd->m_definitions = result;
        QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(req);
        emit r->progress(r, appendOnly);
    }
}
