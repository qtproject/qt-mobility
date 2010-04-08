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
#include <QUuid>
#include <QTimer>

#include <QDebug>

#include <qtcontacts.h>

#include "cntsymbianengine.h"
#include "qcontactchangeset.h"
#include "cntsymbiandatabase.h"
#include "cnttransformcontact.h"
#include "cntsymbiantransformerror.h"
#include "cntsymbianfilterdbms.h"
#include "cntsymbianfiltersql.h"
#include "cntsymbiansorterdbms.h"
#include "cntrelationship.h"
#include "cntdisplaylabel.h"

typedef QList<QContactLocalId> QContactLocalIdList;
typedef QPair<QContactLocalId, QContactLocalId> QOwnCardPair;

// NOTE: There is a bug with RVCT compiler which causes the local stack
// variable to corrupt if the called function leaves. As a workaround we are
// reserving the objects from heap so it will not get corrupted.
// This of course applies only to those stack variables which are passed to
// the called function or the return value of the function is placed to the
// variable

/* ... The macros changed names */
#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

CntSymbianEngine::CntSymbianEngine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    *error = QContactManager::NoError;

    m_dataBase = new CntSymbianDatabase(this, error);

    //Database opened successfully
    if(*error == QContactManager::NoError) {
        m_managerUri = QContactManager::buildUri(CNT_SYMBIAN_MANAGER_NAME, parameters);
        m_transformContact = new CntTransformContact;
#ifdef SYMBIAN_BACKEND_USE_SQLITE
        m_contactFilter    = new CntSymbianFilter(*this, *m_dataBase->contactDatabase(), *m_transformContact);
#else
        m_contactFilter    = new CntSymbianFilter(*m_dataBase->contactDatabase());
#endif
        m_contactSorter    = new CntSymbianSorterDbms(*m_dataBase->contactDatabase(), *m_transformContact);
        m_relationship     = new CntRelationship(m_dataBase->contactDatabase(), m_managerUri);
        m_displayLabel     = new CntDisplayLabel();
    }
}

CntSymbianEngine::CntSymbianEngine(const CntSymbianEngine& other)
    : QContactManagerEngine(),
      m_dataBase(other.m_dataBase),
      m_managerUri(other.m_managerUri),
      m_transformContact(other.m_transformContact),
      m_contactFilter(other.m_contactFilter),
      m_contactSorter(other.m_contactSorter),
      m_relationship(other.m_relationship),
      m_displayLabel(other.m_displayLabel)
{
}

CntSymbianEngine::~CntSymbianEngine()
{
    delete m_contactFilter; // needs to be deleted before database
    delete m_dataBase;
    delete m_transformContact;
    delete m_contactSorter;
    delete m_relationship;
    delete m_displayLabel;
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrders.
 * Any error that occurs will be stored in \a error. Uses either the Symbian backend native filtering or in case of an
 * unsupported filter, the generic (slow) filtering of QContactManagerEngine.
 */
QList<QContactLocalId> CntSymbianEngine::contactIds(
        const QContactFilter& filter,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    QList<QContactLocalId> result;
    
    
    if (filter.type() == QContactFilter::RelationshipFilter)
    {
        QContactRelationshipFilter rf = static_cast<QContactRelationshipFilter>(filter);
        // XXX enum changed classes - engine API will reflect this eventually
        QList<QContactRelationship> relationshipsList = relationships(
            rf.relationshipType(), rf.relatedContactId(), rf.relatedContactRole(), error);
        if(*error == QContactManager::NoError) {
            foreach(const QContactRelationship& r, relationshipsList) {
                if(rf.relatedContactRole() == QContactRelationship::First) {
                    result += r.second().localId();
                } else if (rf.relatedContactRole() == QContactRelationship::Second) {
                    result += r.first().localId();
                } else if (rf.relatedContactRole() == QContactRelationship::Either) {
                    result += r.first().localId();
                    result += r.second().localId();
                }
            }
            
            //slow sorting until it's supported in SQL requests
            result = slowSort(result, sortOrders, error);
        }
    }
    else
    {
        bool filterSupported(true);
        result = m_contactFilter->contacts(filter, sortOrders, filterSupported, error);
            
#ifdef SYMBIAN_BACKEND_USE_SQLITE
    
        // Remove possible false positives
        if(!filterSupported && *error == QContactManager::NotSupportedError)
            {
            result = slowFilter(filter, result, error);
            
            //slow sorting until it's supported in SQL requests
            result = slowSort(result, sortOrders, error);
            }
        
#else
        // Remove possible false positives
        if(!filterSupported && *error == QContactManager::NoError)
            result = slowFilter(filter, result, error);
        
        // Sort the matching contacts
        if(!sortOrders.isEmpty()&& *error == QContactManager::NoError ) {
            if(m_contactSorter->sortOrderSupported(sortOrders)) {
                result = m_contactSorter->sort(result, sortOrders, error);
            } else {
                result = slowSort(result, sortOrders, error);
            }
        }
#endif
    }
    return result;
}

#if 0
// These functions are not used anymore - there is always a filter (which may be the default filter)
QList<QContactLocalId> CntSymbianEngine::contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    // Check if sorting is supported by backend
    if(m_contactSorter->sortOrderSupported(sortOrders))
        return m_contactSorter->contacts(sortOrders,error);

    // Backend does not support this sorting.
    // Fall back to slow QContact-level sorting method.

    // Get unsorted contact ids
    QList<QContactSortOrder> noSortOrders;
    QList<QContactLocalId> unsortedIds = m_contactSorter->contacts(noSortOrders, error);
    if (*error != QContactManager::NoError)
        return QList<QContactLocalId>();

    // Sort contacts
    return slowSort(unsortedIds, sortOrders, error);
}

QList<QContact> CntSymbianEngine::contacts(const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    QList<QContact> contacts;
    QList<QContactLocalId> contactIds = this->contactIds(sortOrders, error);
    if (*error == QContactManager::NoError ) {
        foreach (QContactLocalId id, contactIds) {
            QContact contact = this->contact(id, definitionRestrictions, error);
            if (*error != QContactManager::NoError) {
                return QList<QContact>(); // return empty list if error occurred
            }
            contacts.append(contact);
        }
    }
    return contacts;
}
#endif

QList<QContact> CntSymbianEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fh, QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    QList<QContact> contacts;
    QList<QContactLocalId> contactIds = this->contactIds(filter, sortOrders, error);
    if (*error == QContactManager::NoError ) {
        foreach (QContactLocalId id, contactIds) {
            QContact contact = this->contact(id, fh, error);
            if (*error != QContactManager::NoError) {
                return QList<QContact>(); // return empty list if error occurred
            }
            contacts.append(contact);
        }
    }
    return contacts;
}

/*!
 * Read a contact from the contact database.
 *
 * \param contactId The Id of the contact to be retrieved.
 * \param error Qt error code.
 * \return A QContact for the requested QContactLocalId value or 0 if the read
 *  operation was unsuccessful (e.g. contact not found).
 */
QContact CntSymbianEngine::contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    QContact* contact = new QContact();
    TRAPD(err, *contact = fetchContactL(contactId, fetchHint.detailDefinitionsHint()));
    CntSymbianTransformError::transformError(err, error);

    if(*error == QContactManager::NoError) {
        updateDisplayLabel(*contact);
        //check relationship only if there are no definition restrictions, otherwise
        //skip this time expensive operation.       
        if( (!fetchHint.optimizationHints() & QContactFetchHint::NoRelationships)) {
            QContactManager::Error relationshipError;
            // XXX can also consult fetchHint.relationships list
            QList<QContactRelationship> relationships = this->relationships(QString(), contact->id(), QContactRelationship::Either, &relationshipError);
            if (relationshipError != QContactManager::NoError &&
                relationshipError != QContactManager::DoesNotExistError) { // means that no relationships found
                *error = relationshipError;
            }
            QContactManagerEngine::setContactRelationships(contact, relationships);
        }
    }
    return *QScopedPointer<QContact>(contact);
}

bool CntSymbianEngine::saveContact(QContact* contact, QContactManager::Error* error)
{
    QContactChangeSet changeSet;
    TBool ret = doSaveContact(contact, changeSet, error);
    changeSet.emitSignals(this);
    return ret;
}

/*! \reimp */
bool CntSymbianEngine::saveContacts(QList<QContact> *contacts, QMap<int, QContactManager::Error> *errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    
    if (errorMap) {
        // if the errormap argument is null, we just don't do fine-grained reporting.            
        errorMap->clear();
    }    
    
    if (!contacts) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    QContactChangeSet changeSet;
    for (int i = 0; i < contacts->count(); i++) {
        QContact current = contacts->at(i);
        QContactManager::Error functionError = QContactManager::NoError;
        if (!doSaveContact(&current, changeSet, &functionError)) {
            *error = functionError;
            if (errorMap) {
                errorMap->insert(i, functionError);
            }
        } else {
            (*contacts)[i] = current;
        }
    }
    changeSet.emitSignals(this);
    return (*error == QContactManager::NoError);
}

/*!
 * Uses the generic filtering implementation of QContactManagerEngine to filter
 * contacts one-by-one. Really slow when filtering a lot of contacts because
 * every contact needs to be loaded from the database before filtering.
 */
QList<QContactLocalId> CntSymbianEngine::slowFilter(
        const QContactFilter& filter,
        const QList<QContactLocalId>& contacts,
        QContactManager::Error* error
        ) const
{
    QList<QContactLocalId> result;
    for (int i(0); i < contacts.count(); i++) {
        QContactLocalId id = contacts.at(i);

        // Check if this is a false positive. If not, add to the result set.
        if(QContactManagerEngine::testFilter(filter, contact(id, QContactFetchHint(), error)))
            result << id;
    }
    return result;
}

QList<QContactLocalId> CntSymbianEngine::slowSort(
        const QList<QContactLocalId>& contactIds,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error* error) const
{
    // Get unsorted contacts
    QList<QContact> unsortedContacts;
    foreach (QContactLocalId id, contactIds) {
        QContact c = contact(id, QContactFetchHint(), error);
        if (*error != QContactManager::NoError)
            return QList<QContactLocalId>();
        unsortedContacts << c;
    }
    return QContactManagerEngine::sortContacts(unsortedContacts, sortOrders);
}

bool CntSymbianEngine::doSaveContact(QContact* contact, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    bool ret = false;
    if(contact && !validateContact(*contact, error))
        return false;

    // If contact has GUid and no local Id, try to find it in database
    if (contact && !contact->localId() &&
        contact->details(QContactGuid::DefinitionName).count() > 0) {
        QContactDetailFilter guidFilter;
        guidFilter.setDetailDefinitionName(QContactGuid::DefinitionName, QContactGuid::FieldGuid);
        QContactGuid guidDetail = static_cast<QContactGuid>(contact->details(QContactGuid::DefinitionName).at(0));
        guidFilter.setValue(guidDetail.guid());

        QContactManager::Error err;
        QList<QContactLocalId> localIdList = contactIds(guidFilter,
                QList<QContactSortOrder>(), &err);
        if (err == QContactManager::NoError && localIdList.count() > 0) {
            QScopedPointer<QContactId> contactId(new QContactId());
            contactId->setLocalId(localIdList.at(0));
            contactId->setManagerUri(m_managerUri);
            contact->setId(*contactId);
        }
    }

    // Check parameters
    if(!contact) {
        *error = QContactManager::BadArgumentError;
        ret = false;
    // Update an existing contact
    } else if(contact->localId()) {
        if(contact->id().managerUri() == m_managerUri) {
            ret = updateContact(*contact, changeSet, error);
        } else {
            *error = QContactManager::BadArgumentError;
            ret = false;
        }
    // Create new contact
    } else {
        ret = addContact(*contact, changeSet, error);
    }

    if (ret)
        updateDisplayLabel(*contact);

    return ret;
}

/*!
 * Private leaving implementation for contact()
 */
QContact CntSymbianEngine::fetchContactL(const QContactLocalId &localId, const QStringList& definitionRestrictions) const
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
    if(localId == 0)
        User::Leave(KErrNotFound);

    // Read the contact from the CContactDatabase
    CContactItem* contactItem = m_dataBase->contactDatabase()->ReadContactL(localId);
    CleanupStack::PushL(contactItem);

    // Convert to a QContact
    QContact contact = m_transformContact->transformContactL(*contactItem, definitionRestrictions);

    // Transform details that are not available until the contact has been saved
    m_transformContact->transformPostSaveDetailsL(*contactItem, contact, *m_dataBase->contactDatabase(), m_managerUri);

    CleanupStack::PopAndDestroy(contactItem);

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
bool CntSymbianEngine::addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error* qtError)
{
    // Attempt to persist contact, trapping errors
    int err(0);
    QContactLocalId id(0);
    TRAP(err, id = addContactL(contact));
    if(err == KErrNone)
    {
        changeSet.insertAddedContact(id);
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
    int id(0);

    //handle normal contact
    if(contact.type() == QContactType::TypeContact)
    {
        // Create a new contact card.
        CContactItem* contactItem = CContactCard::NewLC();
        m_transformContact->transformContactL(contact, *contactItem);
        // Add to the database
        id = m_dataBase->contactDatabase()->AddNewContactL(*contactItem);
        // Reload contact item
        CleanupStack::PopAndDestroy(contactItem);
        contactItem = 0;
        contactItem = m_dataBase->contactDatabase()->ReadContactLC(id);
        // Transform details that are not available until the contact has been saved
        m_transformContact->transformPostSaveDetailsL(*contactItem, contact, *m_dataBase->contactDatabase(), m_managerUri);
        CleanupStack::PopAndDestroy(contactItem);
    }
    //group contact
    else if(contact.type() == QContactType::TypeGroup)
    {
        // Create a new group, which is added to the database
        CContactItem* contactItem = m_dataBase->contactDatabase()->CreateContactGroupLC();

        //set the id for the contact, needed by update
        id = contactItem->Id();
        QScopedPointer<QContactId> contactId(new QContactId());
        contactId->setLocalId(QContactLocalId(id));
        contactId->setManagerUri(m_managerUri);
        contact.setId(*contactId);

        //update contact, will add the fields to the already saved group
        updateContactL(contact);
        // Transform details that are not available until the contact has been saved
        m_transformContact->transformPostSaveDetailsL(*contactItem, contact, *m_dataBase->contactDatabase(), m_managerUri);

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
bool CntSymbianEngine::updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error* qtError)
{
    int err(0);
    TRAP(err, updateContactL(contact));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged
        changeSet.insertChangedContact(contact.localId());
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

    // Cannot update contact type. The client needs to do this itself.
    if ((contact.type() == QContactType::TypeContact && contactItem->Type() != KUidContactCard &&
            contactItem->Type() != KUidContactOwnCard) ||
        (contact.type() == QContactType::TypeGroup && contactItem->Type() != KUidContactGroup)){
        User::Leave(KErrAlreadyExists);
    }
    
    // Copy the data from QContact to CContactItem
    m_transformContact->transformContactL(contact, *contactItem);

    // Write the entry using the converted  contact
    // note commitContactL removes empty fields from the contact
    m_dataBase->contactDatabase()->CommitContactL(*contactItem);

    updateDisplayLabel(contact);

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
bool CntSymbianEngine::removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error* qtError)
{
    // removeContactL() can't throw c++ exception
    TRAPD(err, removeContactL(id));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged?
        changeSet.insertRemovedContact(id);
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
#ifdef SYMBIAN_BACKEND_S60_VERSION_32
    // In S60 3.2 hardware (observerd with N96) there is a problem when saving and 
    // deleting contacts in quick successive manner. At some point the database
    // starts leaving with KErrNotReady (-18). This happens randomly at either
    // DeleteContactL() or AddNewContactL(). The only only thing that seems to
    // help is to compress the database after deleting a contact. 
    // 
    // Needles to say that this will have a major negative effect on performance!
    // TODO: A better solution must be found.
    m_dataBase->contactDatabase()->CompactL();
#endif

    return 0;
}

bool CntSymbianEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
    QContactManager::Error err;
    QContactLocalId selfCntId = selfContactId(&err); // err ignored
    QContactChangeSet changeSet;
    TBool ret = removeContact(contactId, changeSet, error);
    if (ret && contactId == selfCntId ) {
        QOwnCardPair ownCard(selfCntId, QContactLocalId(0));
        changeSet.setOldAndNewSelfContactId(ownCard);
    }
    changeSet.emitSignals(this);
    return ret;
}

void CntSymbianEngine::updateDisplayLabel(QContact& contact) const
{
    QContactManager::Error error(QContactManager::NoError);
    QString label = synthesizedDisplayLabel(contact, &error);
    if(error == QContactManager::NoError) {
        setContactDisplayLabel(&contact, label);
    }
}

bool CntSymbianEngine::removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error> *errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    
    if (errorMap) {
        // if the errormap argument is null, we just don't do fine-grained reporting.            
        errorMap->clear();
    }
    
    if (contactIds.count() == 0) {
        *error = QContactManager::BadArgumentError;
        return false;
    }
    
    QContactManager::Error err;
    QContactLocalId selfCntId = selfContactId(&err); // err ignored

    QContactChangeSet changeSet;
    for (int i = 0; i < contactIds.count(); i++) {
        QContactLocalId current = contactIds.at(i);
        QContactManager::Error functionError = QContactManager::NoError;
        if (!removeContact(current, changeSet, &functionError)) {
            *error = functionError;
            if (errorMap) {
                errorMap->insert(i, functionError);
            }
        } else {
            if (current == selfCntId ) {
                QOwnCardPair ownCard(selfCntId, QContactLocalId(0));
                changeSet.setOldAndNewSelfContactId(ownCard);
            }
        }
    }
    changeSet.emitSignals(this);
    return (*error == QContactManager::NoError);
}

/* relationships */

bool CntSymbianEngine::isRelationshipTypeSupported(const QString &relationshipType, const QString &contactType) const
{
    return m_relationship->isRelationshipTypeSupported(relationshipType, contactType);
}

QList<QContactRelationship> CntSymbianEngine::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const
{
    //retrieve the relationships
    return m_relationship->relationships(relationshipType, participantId, role, error);
}

bool CntSymbianEngine::saveRelationship(QContactRelationship* relationship, QContactManager::Error* error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //save the relationship
    QSet<QContactLocalId> affectedContactIds;
    bool returnValue = m_relationship->saveRelationship(&affectedContactIds, relationship, error);
    changeSet.insertAddedRelationshipsContacts(affectedContactIds.toList());

    //add contacts to the list that shouldn't be emitted
    m_dataBase->appendContactsEmitted(affectedContactIds.toList());

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

bool CntSymbianEngine::saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //save the relationships
    QSet<QContactLocalId> affectedContactIds;
    bool returnValue = m_relationship->saveRelationships(&affectedContactIds, relationships, errorMap, error);
    changeSet.insertAddedRelationshipsContacts(affectedContactIds.toList());

    //add contacts to the list that shouldn't be emitted
    m_dataBase->appendContactsEmitted(affectedContactIds.toList());

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

bool CntSymbianEngine::removeRelationship(const QContactRelationship& relationship, QContactManager::Error* error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //remove the relationship
    QSet<QContactLocalId> affectedContactIds;
    bool returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, error);
    changeSet.insertRemovedRelationshipsContacts(affectedContactIds.toList());

    //add contacts to the list that shouldn't be emitted
    m_dataBase->appendContactsEmitted(affectedContactIds.toList());

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

bool CntSymbianEngine::removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //remove the relationships
    QSet<QContactLocalId> affectedContactIds;
    bool returnValue = m_relationship->removeRelationships(&affectedContactIds, relationships, errorMap, error);
    changeSet.insertRemovedRelationshipsContacts(affectedContactIds.toList());

    //add contacts to the list that shouldn't be emitted
    m_dataBase->appendContactsEmitted(affectedContactIds.toList());

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QMap<QString, QContactDetailDefinition> CntSymbianEngine::detailDefinitions(const QString& contactType, QContactManager::Error* error) const
{
    // TODO: update for SIM contacts later
    if (contactType != QContactType::TypeContact && contactType != QContactType::TypeGroup) {
        *error = QContactManager::InvalidContactTypeError;
        return QMap<QString, QContactDetailDefinition>();
    }

    *error = QContactManager::NoError;

    // First get the default definitions
    QMap<QString, QMap<QString, QContactDetailDefinition> > schemaDefinitions = QContactManagerEngine::schemaDefinitions();

    // And then ask contact transformer to do the modifications required
    QMap<QString, QContactDetailDefinition> schemaForType = schemaDefinitions.value(contactType);
    m_transformContact->detailDefinitions(schemaForType, contactType, error);

    return schemaForType;
}

bool CntSymbianEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    bool returnValue(false);

    // TODO: update for SIM contacts later
    if (contactType != QContactType::TypeContact && contactType != QContactType::TypeGroup)
        return false;

    switch (feature) {
        /*
        TODO:
        How about the others? like:
        Groups,
        ActionPreferences,
        MutableDefinitions,
        Relationships,
        ArbitraryRelationshipTypes,
        RelationshipOrdering,
        DetailOrdering,
        SelfContact,
        Anonymous,
        ChangeLogs
        */
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

bool CntSymbianEngine::isFilterSupported(const QContactFilter& filter) const
{
    return m_contactFilter->filterSupported(filter);
}

/* Synthesise the display label of a contact */
QString CntSymbianEngine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    return m_displayLabel->synthesizedDisplayLabel(contact, error);
}

bool CntSymbianEngine::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error)
{
    if (contactId <= 0) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    TContactItemId id(contactId);
    CContactItem* symContact = 0;
    TRAPD(err,
        symContact = m_dataBase->contactDatabase()->ReadContactL(id);
        m_dataBase->contactDatabase()->SetOwnCardL(*symContact);
        );
    delete symContact;
    CntSymbianTransformError::transformError(err, error);
    return (err==KErrNone);
}

QContactLocalId CntSymbianEngine::selfContactId(QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    QContactLocalId id = 0;

    TContactItemId myCard = m_dataBase->contactDatabase()->OwnCardId();
    if (myCard < 0) {
    *error = QContactManager::DoesNotExistError;
    }
    else {
        id = myCard;
    }
    return id;
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

/*
 * 'async' code borrowed from memory engine - actually does sync operations.
 * This will be replaced by the thread request worker when it is stable.
 */



/*! \reimp */
void CntSymbianEngine::requestDestroyed(QContactAbstractRequest* req)
{
    m_asynchronousOperations.removeOne(req);
}

/*! \reimp */
bool CntSymbianEngine::startRequest(QContactAbstractRequest* req)
{
    if (!m_asynchronousOperations.contains(req))
        m_asynchronousOperations.enqueue(req);
    updateRequestState(req, QContactAbstractRequest::ActiveState);
    QTimer::singleShot(0, this, SLOT(performAsynchronousOperation()));
    return true;
}

/*! \reimp */
bool CntSymbianEngine::cancelRequest(QContactAbstractRequest* req)
{
    updateRequestState(req, QContactAbstractRequest::CanceledState);
    return true;
}

/*! \reimp */
bool CntSymbianEngine::waitForRequestProgress(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(msecs);

    if (!m_asynchronousOperations.removeOne(req))
        return false; // didn't exist.

    // replace at head of queue
    m_asynchronousOperations.insert(0, req);

    // and perform the operation.
    performAsynchronousOperation();

    return true;
}

/*! \reimp */
bool CntSymbianEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    // in our implementation, we always complete any operation we start.
    // so, waitForRequestFinished is equivalent to waitForRequestProgress.
    return waitForRequestProgress(req, msecs);
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void CntSymbianEngine::performAsynchronousOperation()
{
    QContactAbstractRequest *currentRequest;

    // take the first pending request and finish it
    if (m_asynchronousOperations.isEmpty())
        return;
    currentRequest = m_asynchronousOperations.dequeue();

    // check to see if it is cancelling; if so, remove it from the queue and return.
    if (currentRequest->state() == QContactAbstractRequest::CanceledState) {
        return;
    }

    // store up changes, and emit signals once at the end of the (possibly batch) operation.
    QContactChangeSet changeSet;

    // Now perform the active request and emit required signals.
    Q_ASSERT(currentRequest->state() == QContactAbstractRequest::ActiveState);
    switch (currentRequest->type()) {
        case QContactAbstractRequest::ContactFetchRequest:
        {
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(currentRequest);
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();
            QContactFetchHint fh = r->fetchHint();

            QContactManager::Error operationError;
            QList<QContact> requestedContacts = contacts(filter, sorting, fh, &operationError);

            // update the request with the results.
            updateContactFetchRequest(r, requestedContacts, operationError, QContactAbstractRequest::FinishedState); // emit resultsAvailable()
        }
        break;

        case QContactAbstractRequest::ContactLocalIdFetchRequest:
        {
            QContactLocalIdFetchRequest* r = static_cast<QContactLocalIdFetchRequest*>(currentRequest);
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();

            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactLocalId> requestedContactIds = contactIds(filter, sorting, &operationError);

            updateContactLocalIdFetchRequest(r, requestedContactIds, operationError, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::ContactSaveRequest:
        {
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(currentRequest);
            QList<QContact> contacts = r->contacts();

            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;
            saveContacts(&contacts, &errorMap, &operationError);

            updateContactSaveRequest(r, contacts, operationError, errorMap, QContactAbstractRequest::FinishedState); // there will always be results of some form.  emit resultsAvailable().
        }
        break;

        case QContactAbstractRequest::ContactRemoveRequest:
        {
            // this implementation provides scant information to the user
            // the operation either succeeds (all contacts matching the filter were removed)
            // or it fails (one or more contacts matching the filter could not be removed)
            // if a failure occurred, the request error will be set to the most recent
            // error that occurred during the remove operation.
            QContactRemoveRequest* r = static_cast<QContactRemoveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactLocalId> contactsToRemove = r->contactIds();
            QMap<int, QContactManager::Error> errorMap;

            for (int i = 0; i < contactsToRemove.size(); i++) {
                QContactManager::Error tempError;
                removeContact(contactsToRemove.at(i), changeSet, &tempError);

                errorMap.insert(i, tempError);                
                if (tempError != QContactManager::NoError) {
                    operationError = tempError;
                }
            }

            updateContactRemoveRequest(r, operationError, errorMap, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionFetchRequest:
        {
            QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;
            QMap<QString, QContactDetailDefinition> requestedDefinitions;
            QStringList names = r->definitionNames();
            if (names.isEmpty())
                names = detailDefinitions(r->contactType(), &operationError).keys(); // all definitions.

            QContactManager::Error tempError;
            for (int i = 0; i < names.size(); i++) {
                QContactDetailDefinition current = detailDefinition(names.at(i), r->contactType(), &tempError);
                requestedDefinitions.insert(names.at(i), current);

                errorMap.insert(i, tempError);              
                if (tempError != QContactManager::NoError) {
                    operationError = tempError;
                }
            }

            updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionSaveRequest:
        {
            // symbian engine currently does not support mutable definitions.
        }
        break;

        case QContactAbstractRequest::DetailDefinitionRemoveRequest:
        {
            // symbian engine currently does not support mutable definitions.
        }
        break;

        case QContactAbstractRequest::RelationshipFetchRequest:
        {
            QContactRelationshipFetchRequest* r = static_cast<QContactRelationshipFetchRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactManager::Error> operationErrors;
            QList<QContactRelationship> allRelationships = relationships(QString(), QContactId(), QContactRelationship::Either, &operationError);
            QList<QContactRelationship> requestedRelationships;

            // select the requested relationships.
            for (int i = 0; i < allRelationships.size(); i++) {
                QContactRelationship currRel = allRelationships.at(i);
                if (r->first() != QContactId() && r->first() != currRel.first())
                    continue;
                if (r->second() != QContactId() && r->second() != currRel.second())
                    continue;
                if (!r->relationshipType().isEmpty() && r->relationshipType() != currRel.relationshipType())
                    continue;
                requestedRelationships.append(currRel);
            }

            // update the request with the results.
            updateRelationshipFetchRequest(r, requestedRelationships, operationError, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::RelationshipRemoveRequest:
        {
            QContactRelationshipRemoveRequest* r = static_cast<QContactRelationshipRemoveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactRelationship> relationshipsToRemove = r->relationships();
            QMap<int, QContactManager::Error> errorMap;

            for (int i = 0; i < relationshipsToRemove.size(); i++) {
                QContactManager::Error tempError;
                removeRelationship(relationshipsToRemove.at(i), &tempError);

                errorMap.insert(i, tempError);
                if (tempError != QContactManager::NoError) {
                    operationError = tempError;
                }
            }

            updateRelationshipRemoveRequest(r, operationError, errorMap, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::RelationshipSaveRequest:
        {
            QContactRelationshipSaveRequest* r = static_cast<QContactRelationshipSaveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;
            QList<QContactRelationship> requestRelationships = r->relationships();
            QList<QContactRelationship> savedRelationships;

            QContactManager::Error tempError;
            for (int i = 0; i < requestRelationships.size(); i++) {
                QContactRelationship current = requestRelationships.at(i);
                saveRelationship(&current, &tempError);
                savedRelationships.append(current);

                errorMap.insert(i, tempError);
                if (tempError != QContactManager::NoError) {
                    operationError = tempError;
                }
            }

            // update the request with the results.
            updateRelationshipSaveRequest(r, savedRelationships, operationError, errorMap, QContactAbstractRequest::FinishedState);
        }
        break;

        default: // unknown request type.
        break;
    }

    // now emit any signals we have to emit
    changeSet.emitSignals(this);
}

#ifndef PBK_UNIT_TEST
/* Factory lives here in the basement */
QContactManagerEngine* CntSymbianFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    return new CntSymbianEngine(parameters, error);
}

QString CntSymbianFactory::managerName() const
{
    return CNT_SYMBIAN_MANAGER_NAME;
}

Q_EXPORT_PLUGIN2(mobapicontactspluginsymbian, CntSymbianFactory);

#endif  //PBK_UNIT_TEST
