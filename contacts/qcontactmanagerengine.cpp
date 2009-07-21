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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QSet>

#include "qcontactmanagerengine.h"

#include "qcontactdetaildefinition.h"
#include "qcontactdetails.h"

#include "qcontactgroup_p.h"
#include "qcontactsortorder.h"

#include "qcontactfilters.h"
#include "qcontactabstractaction.h"

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
 * \fn QContactManagerEngine::contactsAdded(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been added to
 * this engine.  As it is possible that other processes (or other devices) may
 * have added the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::contactsChanged(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been modified in
 * this engine.  As it is possible that other processes (or other devices) may
 * have modified the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::contactsRemoved(const QList<QUniqueId>& contactIds);
 *
 * This signal is emitted some time after a set of contacts has been removed from
 * this engine.  As it is possible that other processes (or other devices) may
 * have removed the contacts, the timing cannot be determined.
 *
 * The list of ids added is given by \a contactIds.  There may be one or more
 * ids in the list.
 */


/*!
 * \fn QContactManagerEngine::groupsAdded(const QList<QUniqueId>& groupIds);
 *
 * This signal is emitted some time after a set of contact groups has been added to
 * this engine.  As it is possible that other processes (or other devices) may
 * have added the groups, the timing cannot be determined.
 *
 * The list of ids added is given by \a groupIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::groupsChanged(const QList<QUniqueId>& groupIds);
 *
 * This signal is emitted some time after a set of contact groups has been modified in
 * this engine.  As it is possible that other processes (or other devices) may
 * have modified the groups, the timing cannot be determined.
 *
 * The list of ids added is given by \a groupIds.  There may be one or more
 * ids in the list.
 */

/*!
 * \fn QContactManagerEngine::groupsRemoved(const QList<QUniqueId>& groupIds);
 *
 * This signal is emitted some time after a set of contact groups has been removed from
 * this engine.  As it is possible that other processes (or other devices) may
 * have removed the groups, the timing cannot be determined.
 *
 * The list of ids added is given by \a groupIds.  There may be one or more
 * ids in the list.
 */

/*!
 * Return the list of contact ids present in this engine, sorted according to the given \a sortOrder.
 *
 * Any errors encountered should be stored to \a error.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QContactSortOrder& sortOrder, QContactManager::Error& error) const
{
    Q_UNUSED(sortOrder);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrder.
 * Any error that occurs will be stored in \a error.
 *
 * The default implementation will retrieve all contacts and test them with testFilter.
 */
QList<QUniqueId> QContactManagerEngine::contacts(const QContactFilter& filter, const QContactSortOrder& sortOrder, QContactManager::Error& error) const
{
    /* Slow way */
    QList<QUniqueId> ret;

    /* Try to do some early out, just in case */
    if (filter.type() != QContactFilter::Invalid) {
        /* Retrieve each contact.. . . */
        const QList<QUniqueId>& all = contacts(sortOrder, error);
        if (error != QContactManager::NoError)
            return ret;
        for (int j = 0; j < all.count(); j++) {
            if (testFilter(filter, contact(all.at(j), error)))
                ret << all.at(j);
        }
    }

    return ret;
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
    QList<QContactDetail> allOrgs = contact.details(QContactOrganisation::DefinitionName);
    for (int i=0; i < allOrgs.size(); i++) {
        const QContactOrganisation& org = allOrgs.at(i);
        if (!org.displayLabel().isEmpty()) {
            return org.displayLabel();
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
    // the map we will eventually return
    QMap<QString, QContactDetailDefinition> retn;

    // local variables for reuse
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;

    // sync target
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactSyncTarget::DefinitionName);
    fields.insert(QContactSyncTarget::FieldSyncTarget, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(d.id(), d);

    // guid
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactGuid::DefinitionName);
    fields.insert(QContactGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(d.id(), d);

    // display label
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactDisplayLabel::DefinitionName);
    fields.insert(QContactDisplayLabel::FieldLabel, f);
    f.dataType = QVariant::Bool;
    f.allowableValues = QVariantList();
    fields.insert(QContactDisplayLabel::FieldSynthesised, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // email address
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactEmailAddress::DefinitionName);
    fields.insert(QContactEmailAddress::FieldEmailAddress, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // organisation
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactOrganisation::DefinitionName);
    fields.insert(QContactOrganisation::FieldLogo, f);
    fields.insert(QContactOrganisation::FieldDisplayLabel, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // phone number
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactPhoneNumber::DefinitionName);
    fields.insert(QContactPhoneNumber::FieldNumber, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // anniversary
    fields.clear();
    f.dataType = QVariant::Date;
    f.allowableValues = QVariantList();
    d.setId(QContactAnniversary::DefinitionName);
    fields.insert(QContactAnniversary::FieldOriginalDate, f);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactAnniversary::FieldCalendarId, f);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactAnniversary::FieldEvent, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // birthday
    fields.clear();
    f.dataType = QVariant::Date;
    f.allowableValues = QVariantList();
    d.setId(QContactBirthday::DefinitionName);
    fields.insert(QContactBirthday::FieldBirthday, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

#if 0 // leaf class not yet accepted into master
    // meeting
    fields.clear();
    f.dataType = QVariant::DateTime;
    f.allowableValues = QVariantList();
    d.setId(QContactMeeting::DefinitionName);
    fields.insert(QContactMeeting::FieldMeeting, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);
#endif

#if 0 // leaf class not yet accepted into master
    // tag
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactTag::DefinitionName);
    fields.insert(QContactTag::FieldTag, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);
#endif

    // url
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactUrl::DefinitionName);
    fields.insert(QContactUrl::FieldUrl, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // gender
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList() << QString(QLatin1String("Male")) << QString(QLatin1String("Female")) << QString(QLatin1String("Unspecified"));
    d.setId(QContactGender::DefinitionName);
    fields.insert(QContactGender::FieldGender, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // avatar
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactAvatar::DefinitionName);
    fields.insert(QContactAvatar::FieldAvatar, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

#if 0 // leaf class not yet accepted into master
    // geolocation
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactGeolocation::DefinitionName);
    fields.insert(QContactGeolocation::FieldLatitude, f);
    fields.insert(QContactGeolocation::FieldLongitude, f);
    fields.insert(QContactGeolocation::FieldAccuracy, f);
    fields.insert(QContactGeolocation::FieldAltitude, f);
    fields.insert(QContactGeolocation::FieldAltitudeAccuracy, f);
    fields.insert(QContactGeolocation::FieldSpeed, f);
    fields.insert(QContactGeolocation::FieldHeading, f);
    f.dataType = QVariant::QDateTime;
    fields.insert(QContactGeolocation::FieldUpdateTimestamp, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);
#endif

    // street address
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactAddress::DefinitionName);
    fields.insert(QContactAddress::FieldStreet, f);
    fields.insert(QContactAddress::FieldLocality, f);
    fields.insert(QContactAddress::FieldRegion, f);
    fields.insert(QContactAddress::FieldPostcode, f);
    fields.insert(QContactAddress::FieldCountry, f);
    fields.insert(QContactAddress::FieldDisplayLabel, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

    // name
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    d.setId(QContactName::DefinitionName);
    fields.insert(QContactName::FieldPrefix, f);
    fields.insert(QContactName::FieldFirst, f);
    fields.insert(QContactName::FieldMiddle, f);
    fields.insert(QContactName::FieldLast, f);
    fields.insert(QContactName::FieldSuffix, f);
    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::Any);
    retn.insert(d.id(), d);

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
 * The engine should add the id of any contacts that it has added
 * to \a contactsAdded, and the id of any contacts that is has changed
 * to \a contactsChanged.  If a contact has been added or removed to
 * any groups (via \l QContact::setGroups()), the id of any QContactGroup
 * affected should be added to \a groupsChanged.
 *
 * The framework will emit the \l contactsAdded(), \l contactsChanged()
 * and \l groupsChanged() signals at some point after calling this function.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    Q_UNUSED(contact);
    Q_UNUSED(contactsAdded);
    Q_UNUSED(contactsChanged);
    Q_UNUSED(groupsChanged);
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
            if (uniqueDefinitionIds.contains(def.id())) {
                error = QContactManager::AlreadyExistsError;
                return false; // can't have two of a unique detail.
            }
            uniqueDefinitionIds.append(def.id());
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
            if (!field.allowableValues.isEmpty()
                && !field.allowableValues.contains(values.value(key))) {
                error = QContactManager::InvalidDetailError;
                return false; // value not allowed.
            }
        }
    }

    // Check that any groups in the contact actually exist
    if (contact.groups().count() > 0) {
        QList<QUniqueId> allGroups = groups(error);
        if (error == QContactManager::NoError) {
            for (int i=0; i < contact.groups().count(); i++) {
                QUniqueId groupId = contact.groups().at(i);
                if (!allGroups.contains(groupId)) {
                    error = QContactManager::DoesNotExistError;
                    return false;
                }
            }
        } else {
            // pass the original error back
            return false;
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
    if (definition.id().isEmpty()) {
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
 * If the backend implementation wishes to have the base implementation emit any signals
 * as a result of this operation, it should add the affected id(s) to the
 * \a removed or \a changed sets as appropriate.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& removed, QSet<QUniqueId>& changed, QContactManager::Error& error)
{
    Q_UNUSED(contactId);
    Q_UNUSED(removed);
    Q_UNUSED(changed);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Return the list of group ids present in this manager.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groups(QContactManager::Error& error) const
{
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns the group which is identified by the given \a groupId,
 * or a default-constructed group if no such group exists
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QContactGroup QContactManagerEngine::group(const QUniqueId& groupId, QContactManager::Error& error) const
{
    Q_UNUSED(groupId);
    error = QContactManager::NotSupportedError;
    return QContactGroup();
}

/*!
 * Saves the group \a group in the database.  The id of the group is
 * used to determine the group to update.  If the group has no name set,
 * this function should fail.  If the group does not already
 * exist, it should be added to the database.
 *
 * Returns true on success, or false on failure.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::saveGroup(QContactGroup* group, QContactManager::Error& error)
{
    Q_UNUSED(group);
    error = QContactManager::NotSupportedError;
    return false;
}

/*!
 * Remove the group with the given id \a groupId from the database.
 *
 * Returns false if no group with that id exists, or the operation otherwise failed.
 * Returns true if the group was successfully deleted.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
bool QContactManagerEngine::removeGroup(const QUniqueId& groupId, QContactManager::Error& error)
{
    Q_UNUSED(groupId);
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
 * Returns true if the definition was saved successfully, and otherwise returns false
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
 * Returns a list of ids of contacts that have been added to this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::contactsAddedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of contact ids that have been modified in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::contactsModifiedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of contact ids that have been removed in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::contactsRemovedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of group ids that have been added in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groupsAddedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of group ids that have been modified in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groupsModifiedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
}

/*!
 * Returns a list of group ids that have been removed in this manager
 * since the given \a timestamp.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 */
QList<QUniqueId> QContactManagerEngine::groupsRemovedSince(const QDateTime& timestamp, QContactManager::Error& error) const
{
    Q_UNUSED(timestamp);
    error = QContactManager::NotSupportedError;
    return QList<QUniqueId>();
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
 * The engine should add the id of any contacts that it has added
 * to \a contactsAdded, and the id of any contacts that is has changed
 * to \a contactsChanged.  If a contact has been added or removed to
 * any groups (via \l QContact::setGroups()), the id of any QContactGroup
 * affected should be added to \a groupsChanged.
 *
 * The framework will emit the \l contactsAdded(), \l contactsChanged()
 * and \l groupsChanged() signals at some point after calling this function.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 *
 * \sa QContactManager::saveContact()
 */
QList<QContactManager::Error> QContactManagerEngine::saveContacts(QList<QContact>* contacts, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    if (!contacts) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contacts->count(); i++) {
            QContact current = contacts->at(i);
            if (!saveContact(&current, contactsAdded, contactsChanged, groupsChanged, error)) {
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
 * If the backend implementation wishes to have the base implementation emit
 * any signals as a result of this operation, it must add the id(s) of affected
 * groups and contacts to the \a removedContacts and \a changedGroups sets as
 * appropriate.
 *
 * Any errors encountered during this operation should be stored to
 * \a error.
 *
 * \sa QContactManager::removeContact()
 */
QList<QContactManager::Error> QContactManagerEngine::removeContacts(QList<QUniqueId>* contactIds, QSet<QUniqueId>& removedContacts, QSet<QUniqueId>& changedGroups, QContactManager::Error& error)
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
            if (!removeContact(current, removedContacts, changedGroups, error)) {
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
        case QContactFilter::Invalid:
            return false;

        case QContactFilter::ContactDetail:
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
                    return true;                    /* just testing for the presence of one of these details */

                /* Now figure out what tests we are doing */
                const bool valueTest = cdf.value().isValid();
                const bool presenceTest = !valueTest;

                /* See if we need to test any values at all */
                if (presenceTest) {
                    for(int j=0; j < details.count(); j++) {
                        const QContactDetail& detail = details.at(j);
                        if (detail.values().contains(cdf.detailFieldName()))
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

        case QContactFilter::ContactDetailRange:
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

        case QContactFilter::GroupMembership:
            {
                // check the specified group for membership.
                const QContactGroupMembershipFilter cgf(filter);

                if (contact.groups().contains(cgf.groupId()))
                    return true;

                // Fall through to end
            }
            break;

        case QContactFilter::ChangeLog:
            {
                // Well, the base implementation of this depends on having
                // the QContactTimestamp detail, which is NYI.
                // XXX TODO
            }
            break;

        case QContactFilter::Action:
            {
                // Find any matching actions, and do a union filter on their filter objects
                QContactActionFilter af(filter);
                QList<QContactAbstractAction*> actions = QContactManager::actions(af.actionName(), af.vendorName(), af.vendorVersion());

                // There's a small wrinkle if there's a value specified in the action filter
                // we have to adjust any contained QContactDetailFilters to have that value
                // or test if a QContactDetailRangeFilter contains this value already
                for (int j = 0; j < actions.count(); j++) {
                    QContactAbstractAction* action = actions.at(j);
                    if (action->contactFilter(af.value()) == af)
                        return false; // No recursion!

                    // Check for values etc...
                    if (testFilter(action->contactFilter(af.value()), contact))
                        return true;
                }
                // Fall through to end
            }
            break;

        case QContactFilter::Intersection:
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

        case QContactFilter::Union:
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
 * Performs insertion sort of the contact \a toAdd into the \a sorted list, according to the provided \a sortOrder
 */
void QContactManagerEngine::addSorted(QList<QContact>* sorted, const QContact& toAdd, const QContactSortOrder& sortOrder)
{
    if (!sortOrder.isValid()) {
        sorted->append(toAdd);
    } else {
        const QVariant& newValue = toAdd.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());

        if (newValue.isNull()) {
            if (sortOrder.blankPolicy() == QContactSortOrder::BlanksFirst)
                sorted->prepend(toAdd);
            else
                sorted->append(toAdd);
            return;
        }

        // XXX use a binary search or a proper insertion sort
        for (int i = 0; i < sorted->size(); i++) {
            const QContact& curr = sorted->at(i);

            // Multiple details of the same type are intrisically broken
            const QVariant& currValue = curr.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());
            if (sortOrder.direction() == Qt::AscendingOrder) {
                if (compareVariant(newValue, currValue, sortOrder.caseSensitivity()) < 0) {
                    sorted->insert(i, toAdd);
                    return;
                }
            } else {
                if (compareVariant(newValue, currValue, sortOrder.caseSensitivity()) > 0) {
                    sorted->insert(i, toAdd);
                    return;
                }
            }
        }
    }
    // couldn't find anywhere for it to go.  put it at the end.
    sorted->append(toAdd);
}

/*!
 * Allocates the memory required to store information regarding the given \a request, and uses \a request as a key in a map from request pointer to data structure.
 */
void QContactManagerEngine::createAsynchronousRequest(const QContactAbstractRequest* request)
{
    Q_UNUSED(request);
}

/*!
 * Cleans up the memory used to store information regarding the given \a request
 */
void QContactManagerEngine::destroyAsynchronousRequest(const QContactAbstractRequest* request)
{
    Q_UNUSED(request);
}

/*!
 * Returns true if the state of the given \a request is either QContactAbstractRequest::Finished or QContactAbstractRequest::Cancelled, otherwise returns false.
 */
bool QContactManagerEngine::asynchronousRequestIsFinished(const QContactAbstractRequest* request)
{
    Q_UNUSED(request);
    return false;
}

/*!
 * Returns the error associated with the most recent operation performed as part of the given asynchronous \a request
 */
QContactManager::Error QContactManagerEngine::asynchronousRequestError(const QContactAbstractRequest* request)
{
    Q_UNUSED(request);
    return QContactManager::NotSupportedError;
}

/*!
 * Returns the current status of the given \a request
 */
QContactAbstractRequest::Status QContactManagerEngine::asynchronousRequestStatus(const QContactAbstractRequest* request)
{
    Q_UNUSED(request);
    return QContactAbstractRequest::Inactive;
}

/*!
 * Waits for up to \a msecs milliseconds for the given \a request to enter the QContactAbstractRequest::Finished or QContactAbstractRequest::Cancelled state.  Returns true if the given \a request finishes within the period, else returns false
 */
bool QContactManagerEngine::asynchronousRequestWaitForFinished(QContactAbstractRequest* request, int msecs)
{
    Q_UNUSED(request);
    Q_UNUSED(msecs);
    return false;
}

/*!
 * Waits for up to \a msecs milliseconds for the given \a request to emit the QContactAbstractRequest::progress() signal.  Returns true if the given \a request emits the progress signal within the period, else returns false
 */
bool QContactManagerEngine::asynchronousRequestWaitForProgress(QContactAbstractRequest* request, int msecs)
{
    Q_UNUSED(request);
    Q_UNUSED(msecs);
    return false;
}

/*!
 * Attempts to cancel the most recent operation of the given \a request
 */
void QContactManagerEngine::cancelAsynchronousRequest(QContactAbstractRequest* request)
{
    Q_UNUSED(request);
}

/*!
 * Starts the asynchronous \a operation described in the given \a request
 */
void QContactManagerEngine::startAsynchronousRequest(QContactAbstractRequest* request, QContactAbstractRequest::Operation operation)
{
    Q_UNUSED(request);
    Q_UNUSED(operation);
}

/*!
 * Returns the list of errors which were returned from the most recent asynchronous operation performed according to the given \a request
 */
QList<QContactManager::Error> QContactManagerEngine::asynchronousRequestErrors(const QContactAbstractRequest* request)
{
    Q_UNUSED(request);
    return QList<QContactManager::Error>();
}

/*!
 * Limits the selection criteria of the contact \a request to contacts whose id is contained in \a ids
 */
void QContactManagerEngine::asynchronousRequestSelectById(QContactRequest* request, const QList<QUniqueId>& ids)
{
    Q_UNUSED(request);
    Q_UNUSED(ids);
}

/*!
 * Limits the selection criteria of the contact \a request to contacts contained in \a contacts
 */
void QContactManagerEngine::asynchronousRequestSelectByObject(QContactRequest* request, const QList<QContact>& contacts)
{
    Q_UNUSED(request);
    Q_UNUSED(contacts);
}

/*!
 * Limits the selection criteria of the contact \a request to contacts which match the given \a filter
 */
void QContactManagerEngine::asynchronousRequestSelectByFilter(QContactRequest* request, const QContactFilter& filter)
{
    Q_UNUSED(request);
    Q_UNUSED(filter);
}

/*!
 * Sets the sort order of the result of the next asynchronous operation for the given \a request to \a sortOrder.  Calling this function has no effect on asynchronous operations which have already been started.
 */
void QContactManagerEngine::asynchronousRequestSetSortOrder(QContactRequest* request, const QContactSortOrder& sortOrder)
{
    Q_UNUSED(request);
    Q_UNUSED(sortOrder);
}

/*!
 * Returns the sort order which will be used for the next asynchronous operation of \a request
 */
QContactSortOrder QContactManagerEngine::asynchronousRequestSortOrder(const QContactRequest* request)
{
    Q_UNUSED(request);
    return QContactSortOrder();
}

/*!
 * Clears any result restrictions which have been set for the next asynchronous operation of \a request
 */
void QContactManagerEngine::asynchronousRequestClearRestrictions(QContactRequest* request)
{
    Q_UNUSED(request);
}

/*!
 * Restricts the result of the next asynchronous operation of \a request to the ids of the contacts which would otherwise be returned
 */
void QContactManagerEngine::asynchronousRequestRestrictToIds(QContactRequest* request)
{
    Q_UNUSED(request);
}

/*!
 * Restricts the result of the next asynchronous operation of \a request to those details whose definition name is included in the given list of \a detailDefinitionNames.
 */
void QContactManagerEngine::asynchronousRequestRestrictToDetails(QContactRequest* request, const QStringList& detailDefinitionNames)
{
    Q_UNUSED(request);
    Q_UNUSED(detailDefinitionNames);
}

/*!
 * Returns the result of the most recent asynchronous operation of the given \a request as a list of contact ids
 */
QList<QUniqueId> QContactManagerEngine::asynchronousRequestIds(const QContactRequest* request)
{
    Q_UNUSED(request);
    return QList<QUniqueId>();
}

/*!
 * Returns the result of the most recent asynchronous operation of the given \a request as a list of contacts.
 * If the result had been restricted to ids previously using asynchronousRequestRestrictToIds(), the contacts
 * will be empty except for their id.
 */
QList<QContact> QContactManagerEngine::asynchronousRequestContacts(const QContactRequest* request)
{
    Q_UNUSED(request);
    return QList<QContact>();
}
