/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>

#include <qtcontacts.h>

#include "qcontact_p.h"
#include "qcontactmanager_p.h"

#include "cntsymbianengine.h"
#include "cntsymbianengine_p.h"
#include "qcontactchangeset.h"
#include "cntsymbiandatabase.h"
#include "cnttransformcontact.h"
#include "cntsymbiantransformerror.h"

#include <flogger.h>

CntSymbianEngine::CntSymbianEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    error = QContactManager::NoError;

    m_dataBase = new CntSymbianDatabase(this, error);

    //Database opened successfully
    if(error == QContactManager::NoError) {
        m_managerUri = QContactManager::buildUri(CNT_SYMBIAN_MANAGER_NAME, parameters);
        d = new CntSymbianEnginePrivate(m_dataBase, parameters, error);
        m_transformContact = new CntTransformContact;
        connect(m_dataBase, SIGNAL(ownCardChanged(QContactLocalId,QContactLocalId)), this, SIGNAL(selfContactIdChanged(QContactLocalId,QContactLocalId)));
    }
}

CntSymbianEngine::CntSymbianEngine(const CntSymbianEngine& other)
    : QContactManagerEngine(),
      m_dataBase(other.m_dataBase),
      m_managerUri(other.m_managerUri),
      m_transformContact(other.m_transformContact),
      d(other.d)
{
}

CntSymbianEngine& CntSymbianEngine::operator=(const CntSymbianEngine& other)
{
    // assign
    m_dataBase = other.m_dataBase;
    m_managerUri = other.m_managerUri;
    m_transformContact = other.m_transformContact;
    d = other.d;

    return *this;
}

CntSymbianEngine::~CntSymbianEngine()
{
    delete m_dataBase;
    delete m_transformContact;
    delete d;
}

void CntSymbianEngine::deref()
{
 	delete this;
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrders.
 * Any error that occurs will be stored in \a error. Uses either the Symbian backend native filtering or in case of an
 * unsupported filter, the generic (slow) filtering of QContactManagerEngine.
 */
QList<QContactLocalId> CntSymbianEngine::contacts(
        const QContactFilter& filter,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    QList<QContactLocalId> result;

    // Check if the filter is supported by the underlying filter implementation
    CntAbstractContactFilter::FilterSupport filterSupport = d->filterSupported(filter);

    if (filterSupport == CntAbstractContactFilter::Supported) {
        // Filter supported, use as the result directly
        result = d->contacts(filter, sortOrders, error);
        // If sorting is not supported, we need to fallback to slow sorting
        if(!d->sortOrderSupported(sortOrders))
            result = slowSort(result, sortOrders, error);
    } else if (filterSupport == CntAbstractContactFilter::SupportedPreFilterOnly) {
        // Filter only does pre-filtering and may include false positives
        QList<QContactLocalId> contacts = d->contacts(filter, sortOrders, error);
        if(error == QContactManager::NoError)
            result = slowFilter(filter, contacts, error);
        // If sorting is not supported, we need to fallback to slow sorting
        if(!d->sortOrderSupported(sortOrders))
            result = slowSort(result, sortOrders, error);
    } else {
        // Filter not supported; fetch all contacts and remove false positives
        // one-by-one. Note: this is reeeeaally slow. Both sorting and
        // filtering are done the slow way.
        QList<QContactLocalId> sortedIds = contacts(sortOrders,error);
        if(error == QContactManager::NoError)
            result = slowFilter(filter, sortedIds, error);
    }
    return result;
}

QList<QContactLocalId> CntSymbianEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    // Check if sorting is supported by backend
    if(d->sortOrderSupported(sortOrders))
        return d->contacts(sortOrders,error);

    // Backend does not support this sorting.
    // Fall back to slow QContact-level sorting method.

    // Get unsorted contact ids
    QList<QContactSortOrder> noSortOrders;
    QList<QContactLocalId> unsortedIds = d->contacts(noSortOrders, error);
    if (error != QContactManager::NoError)
        return QList<QContactLocalId>();

    // Sort contacts
    return slowSort(unsortedIds, sortOrders, error);
}

QList<QContactLocalId> CntSymbianEngine::contacts(const QString& contactType, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QList<QContactLocalId> contactIds;

    //retrieve contacts
    if(contactType == QContactType::TypeContact) {
        return contacts(sortOrders, error);
    } else if(contactType == QContactType::TypeGroup) {
        //retrieve groups
        contactIds = d->groups(error);

        // Check if sorting is supported by backend
        if(d->sortOrderSupported(sortOrders)) {
            return d->sort(contactIds, sortOrders, error);
            //return contactIds;
        } else {
            // Backend does not support this sorting.
            // Fall back to slow QContact-level sorting method.
            return slowSort(contactIds, sortOrders, error);
        }
    }

    // Should never happen
    error = QContactManager::BadArgumentError;
    return contactIds; // empty
}

QContact CntSymbianEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
    return fetchContact(contactId, error);
}

bool CntSymbianEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    TBool ret = doSaveContact(contact, changeSet, error);
    changeSet.emitSignals(this);
    return ret;
}

QList<QContactManager::Error> CntSymbianEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    QList<QContactManager::Error> ret;
    if (!contacts) {
        error = QContactManager::BadArgumentError;
    } else {
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contacts->count(); i++) {
            QContact current = contacts->at(i);
            if (!doSaveContact(&current, changeSet, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contacts)[i] = current;
                ret.append(QContactManager::NoError);
            }
        }
        error = functionError;
    }
    changeSet.emitSignals(this);
    return ret;
}

QList<QContactLocalId> CntSymbianEngine::slowFilter(
        const QContactFilter& filter,
        const QList<QContactLocalId>& contacts,
        QContactManager::Error& error
        ) const
{
    QList<QContactLocalId> result;
    for (int i(0); i < contacts.count(); i++) {
        QContactLocalId id = contacts.at(i);

        // Check if this is a false positive. If not, add to the result set.
        if(QContactManagerEngine::testFilter(filter, fetchContact(id, error)))
            result << id;
    }
    return result;
}

QList<QContactLocalId> CntSymbianEngine::slowSort(
        const QList<QContactLocalId>& contactIds,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    // Get unsorted contacts
    QList<QContact> unsortedContacts;
    foreach (QContactLocalId id, contactIds) {
        QContact c = contact(id, error);
        if (error != QContactManager::NoError)
            return QList<QContactLocalId>();
        unsortedContacts << c;
    }
    return QContactManagerEngine::sortContacts(unsortedContacts, sortOrders);
}

bool CntSymbianEngine::doSaveContact(QContact* contact, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    bool ret = false;

    // If contact has GUid and no local Id, try to find it in database
    if (contact && !contact->localId() &&
        contact->details(QContactGuid::DefinitionName).count() > 0) {
        QContactDetailFilter guidFilter;
        guidFilter.setDetailDefinitionName(QContactGuid::DefinitionName, QContactGuid::FieldGuid);
        QContactGuid guidDetail = static_cast<QContactGuid>(contact->details(QContactGuid::DefinitionName).at(0));
        guidFilter.setValue(guidDetail.guid());

        QContactManager::Error err;
        QList<QContactLocalId> localIdList = contacts(guidFilter,
                QList<QContactSortOrder>(), err);
        if (err == QContactManager::NoError && localIdList.count() > 0) {
            QScopedPointer<QContactId> contactId(new QContactId());
            contactId->setLocalId(localIdList.at(0));
            contactId->setManagerUri(d->managerUri());
            contact->setId(*contactId);
        }
    }

    // Check parameters
    if(!contact) {
        error = QContactManager::BadArgumentError;
        ret = false;
    // Update an existing contact
    } else if(contact->localId()) {
        if(contact->id().managerUri() == managerUri()) {
            ret = updateContact(*contact, changeSet, error);
        } else {
            error = QContactManager::BadArgumentError;
            ret = false;
        }
    // Create new contact
    } else {
        ret = addContact(*contact, changeSet, error);
    }
    return ret;
}

/*!
 * Read a contact from the contact database.
 *
 * Internal implementation to read a conact, called by
 * QContactManager::contact().
 *
 * \param contactId The Id of the contact to be retrieved.
 * \param qtError Qt error code.
 * \return A QContact for the requested QUniquId value or 0 if the read
 *  operation was unsuccessful (e.g. contact not found).
 */
QContact CntSymbianEngine::fetchContact(const QContactLocalId& contactId, QContactManager::Error& qtError) const
{
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContact* contact = new QContact();
    TRAPD(err, QT_TRYCATCH_LEAVING(*contact = fetchContactL(contactId)));
    CntSymbianTransformError::transformError(err, qtError);
    return *QScopedPointer<QContact>(contact);
}

/*!
 * Private leaving implementation for contact()
 */
QContact CntSymbianEngine::fetchContactL(const QContactLocalId &localId) const
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
    if(localId == 0)
        User::Leave(KErrNotFound);

    // Read the contact from the CContactDatabase
    CContactItem* symContact = m_dataBase->contactDatabase()->ReadContactL(localId);
    CleanupStack::PushL(symContact);

    // Convert to a QContact
    QContact contact = m_transformContact->transformContactL(*symContact, *m_dataBase->contactDatabase());

    // Convert id
    QContactId contactId;
    contactId.setLocalId(localId);
    contactId.setManagerUri(m_managerUri);
    contact.setId(contactId);

    CleanupStack::PopAndDestroy(symContact);

    return contact;
}

/*!
 * Add the specified contact item to the persistent contacts store.
 *
 * \param contact The QContact to be saved.
 * \param id The Id of new contact
 * \param qtError Qt error code.
 * \return Error status
 */
bool CntSymbianEngine::addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    // Attempt to persist contact, trapping errors
    int err(0);
    QContactLocalId id(0);
    TRAP(err, QT_TRYCATCH_LEAVING(id = addContactL(contact)));
    if(err == KErrNone)
    {
        changeSet.addedContacts().insert(id);
        m_dataBase->appendContactEmitted(id);
    }
    CntSymbianTransformError::transformError(err, qtError);

    return (err==KErrNone);
}

/*!
 * Private leaving implementation for addContact()
 *
 * \param contact The contact item to save in the database.
 * \return The new contact ID.
 */
int CntSymbianEngine::addContactL(QContact &contact)
{
    CContactItem* contactItem(0);
    int id(0);

    //handle normal contact
    if(contact.type() == QContactType::TypeContact)
    {
        // Create a new contact card.
        contactItem = CContactCard::NewLC();
        m_transformContact->transformContactL(contact, *contactItem);
        // Add to the database
        id = m_dataBase->contactDatabase()->AddNewContactL(*contactItem);
        CleanupStack::PopAndDestroy(contactItem);

        // Update the changed values to the QContact
        // id
        QScopedPointer<QContactId> contactId(new QContactId());
        contactId->setLocalId(id);
        contactId->setManagerUri(m_managerUri);
        contact.setId(*contactId);
        contactItem = m_dataBase->contactDatabase()->ReadContactLC(id);
        // Guid
        QContactDetail* detail = m_transformContact->transformGuidItemFieldL(*contactItem, *m_dataBase->contactDatabase());
        contact.saveDetail(detail);
        // Timestamp
        detail = m_transformContact->transformTimestampItemFieldL(*contactItem, *m_dataBase->contactDatabase());
        contact.saveDetail(detail);
        CleanupStack::PopAndDestroy(contactItem);
    }
    //group contact
    else if(contact.type() == QContactType::TypeGroup)
    {
        // Create a new group, which is added to the database
        contactItem = m_dataBase->contactDatabase()->CreateContactGroupLC();

        //set the id for the contact, needed by update
        id = contactItem->Id();
        QScopedPointer<QContactId> contactId(new QContactId());
        contactId->setLocalId(QContactLocalId(id));
        contactId->setManagerUri(m_managerUri);
        contact.setId(*contactId);

        //update contact, will add the fields to the already saved group
        updateContactL(contact);

        CleanupStack::PopAndDestroy(contactItem);
    }
    // Leave with an error
    else
    {
        User::Leave(KErrInvalidContactDetail);
    }

    // Return the new ID.
    return id;
}

/*!
 * Update an existing contact entry in the database.
 *
 * \param contact The contact to update in the database.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool CntSymbianEngine::updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    int err(0);
    TRAP(err, QT_TRYCATCH_LEAVING(updateContactL(contact)));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged
        changeSet.changedContacts().insert(contact.localId());
        m_dataBase->appendContactEmitted(contact.localId());
    }
    CntSymbianTransformError::transformError(err, qtError);
    return (err==KErrNone);
}

/*!
 * Private leaving implementation for updateContact()
 *
 * \param contact The contact to update in the database.
 */
void CntSymbianEngine::updateContactL(QContact &contact)
{
    // Need to open the contact for write, leaving this item
    // on the cleanup stack to unlock the item in the event of a leave.
    CContactItem* contactItem = m_dataBase->contactDatabase()->OpenContactLX(contact.localId());
    CleanupStack::PushL(contactItem);

    // Copy the data from QContact to CContactItem
    m_transformContact->transformContactL(contact, *contactItem);

    // Write the entry using the converted  contact
    // note commitContactL removes empty fields from the contact
    m_dataBase->contactDatabase()->CommitContactL(*contactItem);

    // retrieve the contact in case of empty fields that have been removed, this could also be handled in transformcontact.
    contact = fetchContactL(contact.localId());

    // Update group memberships to contact database
    //updateMemberOfGroupsL(contact);

    CleanupStack::PopAndDestroy(contactItem);
    CleanupStack::PopAndDestroy(1); // commit lock
}

/*!
 * Remove the specified contact object from the database.
 *
 * The removal of contacts from the underlying contacts model database
 * is performed in transactions of maximum 50 items at a time. E.g.
 * deleting 177 contacts would be done in 3 transactions of 50 and a
 * final transaction of 27.
 *
 * \param contact The QContact to be removed.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool CntSymbianEngine::removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    // removeContactL() can't throw c++ exception
    TRAPD(err, removeContactL(id));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged?
        changeSet.removedContacts().insert(id);
        m_dataBase->appendContactEmitted(id);
    }
    CntSymbianTransformError::transformError(err, qtError);
    return (err==KErrNone);
}

/*!
 * Private leaving implementation for removeContact
 */
int CntSymbianEngine::removeContactL(QContactLocalId id)
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
    if(id == 0)
        User::Leave(KErrNotFound);

    //TODO: in future QContactLocalId will be a class so this will need to be changed.
    TContactItemId cId = static_cast<TContactItemId>(id);

    //TODO: add code to remove all relationships.

    m_dataBase->contactDatabase()->DeleteContactL(cId);

    return 0;
}


void CntSymbianEngine::updateDisplayLabel(QContact& contact) const
{
    QContactManager::Error error(QContactManager::NoError);
    QContactDisplayLabel label = contact.displayLabel();
    QString labelString = synthesizeDisplayLabel(contact, error);
    if(error == QContactManager::NoError) {
        label.setLabel(labelString);
        label.setSynthesized(true);
        contact.setDisplayLabel(label);
    }
}

bool CntSymbianEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
    QContactManager::Error err;
    QContactLocalId selfContactId = d->selfContactId(err); // err ignored
    QContactChangeSet changeSet;
    TBool ret = removeContact(contactId, changeSet, error);
    if (ret && contactId == selfContactId )
        emit selfContactIdChanged(selfContactId, QContactLocalId(0));
    changeSet.emitSignals(this);
    return ret;
}

QList<QContactManager::Error> CntSymbianEngine::removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    QList<QContactManager::Error> ret;
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QContactManager::Error err;
        QContactLocalId selfContactId = d->selfContactId(err); // err ignored
        QList<QContactLocalId> removedList;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contactIds->count(); i++) {
            QContactLocalId current = contactIds->at(i);
            if (!removeContact(current, changeSet, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contactIds)[i] = 0;
                ret.append(QContactManager::NoError);
                if (current == selfContactId ) {
                    emit selfContactIdChanged(selfContactId, QContactLocalId(0));
                }
            }
        }
        error = functionError;
    }
    changeSet.emitSignals(this);
    return ret;
}

/* relationships */

QList<QContactRelationship> CntSymbianEngine::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const
{
    //retrieve the relationships
    return d->relationships(relationshipType, participantId, role, error);
}

bool CntSymbianEngine::saveRelationship(QContactRelationship* relationship, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //save the relationship
    bool returnValue = d->saveRelationship(&changeSet, relationship, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QList<QContactManager::Error> CntSymbianEngine::saveRelationships(QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //save the relationships
    QList<QContactManager::Error> returnValue = d->saveRelationships(&changeSet, relationships, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

bool CntSymbianEngine::removeRelationship(const QContactRelationship& relationship, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //remove the relationship
    bool returnValue = d->removeRelationship(&changeSet, relationship, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QList<QContactManager::Error> CntSymbianEngine::removeRelationships(const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //remove the relationships
    QList<QContactManager::Error> returnValue = d->removeRelationships(&changeSet, relationships, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QMap<QString, QContactDetailDefinition> CntSymbianEngine::detailDefinitions(const QString& contactType, QContactManager::Error& error) const
{
    // TODO: update for SIM contacts later
    if (contactType != QContactType::TypeContact && contactType != QContactType::TypeGroup) {
        error = QContactManager::InvalidContactTypeError;
        return QMap<QString, QContactDetailDefinition>();
    }

    error = QContactManager::NoError;

    // Get the supported detail definitions from the contact transformer
    CntTransformContact *transformContact = new CntTransformContact;
    QMap<QString, QContactDetailDefinition> defMap = transformContact->detailDefinitions(error);
    delete transformContact;

    return defMap;
}

bool CntSymbianEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    bool returnValue(false);

    // TODO: update for SIM contacts later
    if (contactType != QContactType::TypeContact && contactType != QContactType::TypeGroup)
        return false;

    switch (feature) {
        /* TODO:
           How about the others? like:
           QContactManager::ActionPreferences,
           QContactManager::MutableDefinitions,
           QContactManager::Anonymous? */
    case QContactManager::Groups:
    case QContactManager::Relationships:
    case QContactManager::SelfContact: {
        returnValue = true;
        break;
    }

    default:
        returnValue = false;
    }

    return returnValue;
}

bool CntSymbianEngine::filterSupported(const QContactFilter& filter) const
{
    TBool result;

    // Map filter support into a boolean value
    CntAbstractContactFilter::FilterSupport filterSupport = d->filterSupported(filter);
    if (filterSupport == CntAbstractContactFilter::Supported
        || filterSupport == CntAbstractContactFilter::SupportedPreFilterOnly) {
        result = true;
    } else {
        result = false;
    }

    return result;
}

/* Synthesise the display label of a contact */
QString CntSymbianEngine::synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& /*error*/) const
{
    QContactName name = contact.detail<QContactName>();
    QContactOrganization org = contact.detail<QContactOrganization>();

    QString firstName = name.first();
    QString lastName = name.last();

    if (!name.last().isEmpty()) {
        if (!name.first().isEmpty()) {
            return QString(QLatin1String("%1, %2")).arg(name.last()).arg(name.first());
        } else {
            // Just last
            return name.last();
        }
    } else if (!name.first().isEmpty()) {
        return name.first();
    } else if (!org.name().isEmpty()) {
        return org.name();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}

bool CntSymbianEngine::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& error)
{
    return d->setSelfContactId(contactId, error);
}

QContactLocalId CntSymbianEngine::selfContactId(QContactManager::Error& error) const
{
    return d->selfContactId(error);
}

/*!
 * Returns the list of data types supported by the Symbian S60 engine
 */
QList<QVariant::Type> CntSymbianEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);

    return st;
}

QString CntSymbianEngine::managerName() const
{
    return CNT_SYMBIAN_MANAGER_NAME;
}

/* Factory lives here in the basement */
QContactManagerEngine* CntSymbianFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new CntSymbianEngine(parameters, error);
}

QString CntSymbianFactory::managerName() const
{
    return CNT_SYMBIAN_MANAGER_NAME;
}

Q_EXPORT_PLUGIN2(mobapicontactspluginsymbian, CntSymbianFactory);
