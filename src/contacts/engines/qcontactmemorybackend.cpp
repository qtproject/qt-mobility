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

#include "qcontactmanager.h"

#include "qcontactdetaildefinition.h"
#include "qcontactmanagerengine.h"
#include "qcontactabstractrequest.h"
#include "qcontactrequests.h"
#include "qcontactchangeset.h"
#include "qcontactdetails.h"

#include "qcontactmemorybackend_p.h"

#include <QTimer>
#include <QUuid>
#include <QSharedData>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QContactMemoryEngine
  \brief The QContactMemoryEngine class provides an in-memory implementation
  of a contacts backend.
 
  It may be used as a reference implementation, or when persistent storage is not required.
 
  During construction, it will load the in-memory data associated with the memory store
  identified by the "id" parameter from the given parameters if it exists, or a new,
  anonymous store if it does not.
 
  Data stored in this engine is only available in the current process.
 
  This engine supports sharing, so an internal reference count is increased
  whenever a manager uses this backend, and is decreased when the manager
  no longer requires this engine.
 */

/* static data for manager class */
QMap<QString, QContactMemoryEngineData*> QContactMemoryEngine::engineDatas;

/*!
 * Factory function for creating a new in-memory backend, based
 * on the given \a parameters.
 *
 * The same engine will be returned for multiple calls with the
 * same value for the "id" parameter, while one of them is in scope.
 */
QContactMemoryEngine* QContactMemoryEngine::createMemoryEngine(const QMap<QString, QString>& parameters)
{
    bool anonymous = false;
    QString idValue = parameters.value(QLatin1String("id"));
    if (idValue.isNull() || idValue.isEmpty()) {
        // no store given?  new, anonymous store.
        idValue = QUuid::createUuid().toString();
        anonymous = true;
    }

    QContactMemoryEngineData* data = engineDatas.value(idValue);
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QContactMemoryEngineData();
        data->m_id = idValue;
        data->m_anonymous = anonymous;
        engineDatas.insert(idValue, data);
    }
    return new QContactMemoryEngine(data);
}

/*!
 * Constructs a new in-memory backend.
 *
 * Loads the in-memory data associated with the memory store identified by the "id" parameter
 * from the given \a parameters if it exists, or a new, anonymous store if it does not.
 */
QContactMemoryEngine::QContactMemoryEngine(QContactMemoryEngineData* data)
    : d(data)
{
    d->m_sharedEngines.append(this);
}

/*! \reimp */
QContactMemoryEngine::~QContactMemoryEngine()
{
    d->m_sharedEngines.removeAll(this);
    if (!d->m_refCount.deref()) {
        engineDatas.remove(d->m_id);
        delete d;
    }
}

/*! \reimp */
QString QContactMemoryEngine::managerName() const
{
    return QLatin1String("memory");
}

/*! \reimp */
QMap<QString, QString> QContactMemoryEngine::managerParameters() const
{
    QMap<QString, QString> params;
    params.insert(QLatin1String("id"), d->m_id);
    return params;
}

/*! \reimp */
bool QContactMemoryEngine::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error)
{
    if (contactId == QContactLocalId(0) || d->m_contactIds.contains(contactId)) {
        *error = QContactManager::NoError;
        QContactLocalId oldId = d->m_selfContactId;
        d->m_selfContactId = contactId;

        QContactChangeSet changeSet;
        changeSet.setOldAndNewSelfContactId(QPair<QContactLocalId, QContactLocalId>(oldId, contactId));
        d->emitSharedSignals(&changeSet);
        return true;
    }

    *error = QContactManager::DoesNotExistError;
    return false;
}

/*! \reimp */
QContactLocalId QContactMemoryEngine::selfContactId(QContactManager::Error* error) const
{
    *error = QContactManager::DoesNotExistError;
    if (d->m_selfContactId != QContactLocalId(0))
        *error = QContactManager::NoError;
    return d->m_selfContactId;
}

/*! \reimp */
QContact QContactMemoryEngine::contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    int index = d->m_contactIds.indexOf(contactId);
    if (index != -1) {
        // found the contact successfully.
        *error = QContactManager::NoError;
        return d->m_contacts.at(index);
    }

    *error = QContactManager::DoesNotExistError;
    return QContact();
}

QList<QContactLocalId> QContactMemoryEngine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    /* Special case the fast case */
    if (filter.type() == QContactFilter::DefaultFilter && sortOrders.count() == 0) {
        return d->m_contactIds;
    } else {
        QList<QContact> clist = contacts(filter, sortOrders, QContactFetchHint(), error);

        /* Extract the ids */
        QList<QContactLocalId> ids;
        foreach(const QContact& c, clist)
            ids.append(c.localId());

        return ids;
    }
}

QList<QContact> QContactMemoryEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    Q_UNUSED(error);

    QList<QContact> sorted;

    /* First filter out contacts - check for default filter first */
    if (filter.type() == QContactFilter::DefaultFilter) {
        foreach(const QContact&c, d->m_contacts) {
            QContactManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    } else {
        foreach(const QContact&c, d->m_contacts) {
            if (QContactManagerEngine::testFilter(filter, c))
                QContactManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    }

    return sorted;
}


bool QContactMemoryEngine::saveContact(QContact* theContact, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    // ensure that the contact's details conform to their definitions
    if (!validateContact(*theContact, error)) {
        return false;
    }

    // check to see if this contact already exists
    int index = d->m_contactIds.indexOf(theContact->id().localId());
    if (index != -1) {
        /* We also need to check that there are no modified create only details */
        QContact oldContact = d->m_contacts.at(index);

        if (oldContact.type() != theContact->type()) {
            *error = QContactManager::AlreadyExistsError;
            return false;
        }

        QContactTimestamp ts = theContact->detail(QContactTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        QContactManagerEngine::setDetailAccessConstraints(&ts, QContactDetail::ReadOnly | QContactDetail::Irremovable);
        theContact->saveDetail(&ts);

        // synthesize the display label for the contact.
        setContactDisplayLabel(theContact, synthesizedDisplayLabel(*theContact, error));

        // Looks ok, so continue
        d->m_contacts.replace(index, *theContact);
        changeSet.insertChangedContact(theContact->localId());
    } else {
        // id does not exist; if not zero, fail.
        QContactId newId;
        newId.setManagerUri(managerUri());
        if (theContact->id() != QContactId() && theContact->id() != newId) {
            // the ID is not empty, and it doesn't identify an existing contact in our database either.
            *error = QContactManager::DoesNotExistError;
            return false;
        }

        /* New contact */
        QContactTimestamp ts = theContact->detail(QContactTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        setDetailAccessConstraints(&ts, QContactDetail::ReadOnly | QContactDetail::Irremovable);
        theContact->saveDetail(&ts);

        // update the contact item - set its ID
        newId.setLocalId(++d->m_nextContactId);
        theContact->setId(newId);

        // synthesize the display label for the contact.
        setContactDisplayLabel(theContact, synthesizedDisplayLabel(*theContact, error));

        // finally, add the contact to our internal lists and return
        d->m_contacts.append(*theContact);                   // add contact to list
        d->m_contactIds.append(theContact->localId());  // track the contact id.

        changeSet.insertAddedContact(theContact->localId());
    }

    *error = QContactManager::NoError;     // successful.
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    if(errorMap) {
        errorMap->clear();
    }

    if (!contacts) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    QContactChangeSet changeSet;
    QContact current;
    QContactManager::Error operationError = QContactManager::NoError;
    for (int i = 0; i < contacts->count(); i++) {
        current = contacts->at(i);
        if (!saveContact(&current, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        } else {
            (*contacts)[i] = current;
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some error occurred
    return (*error == QContactManager::NoError);
}

bool QContactMemoryEngine::removeContact(const QContactLocalId& contactId, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    int index = d->m_contactIds.indexOf(contactId);

    if (index == -1) {
        *error = QContactManager::DoesNotExistError;
        return false;
    }

    // remove the contact from any relationships it was in.
    QContactId thisContact;
    thisContact.setManagerUri(managerUri());
    thisContact.setLocalId(contactId);
    QList<QContactRelationship> allRelationships = relationships(QString(), thisContact, QContactRelationship::Either, error);
    if (*error != QContactManager::NoError && *error != QContactManager::DoesNotExistError) {
        *error = QContactManager::UnspecifiedError; // failed to clean up relationships
        return false;
    }

    // this is meant to be a transaction, so if any of these fail, we're in BIG TROUBLE.
    // a real backend will use DBMS transactions to ensure database integrity.
    removeRelationships(allRelationships, 0, error);

    // having cleaned up the relationships, remove the contact from the lists.
    d->m_contacts.removeAt(index);
    d->m_contactIds.removeAt(index);
    *error = QContactManager::NoError;

    // and if it was the self contact, reset the self contact id
    if (contactId == d->m_selfContactId) {
        d->m_selfContactId = QContactLocalId(0);
        changeSet.setOldAndNewSelfContactId(QPair<QContactLocalId, QContactLocalId>(contactId, QContactLocalId(0)));
    }

    changeSet.insertRemovedContact(contactId);
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    if (contactIds.count() == 0) {
        *error = QContactManager::BadArgumentError;
        return false;
    }
    
    QContactChangeSet changeSet;
    QContactLocalId current;
    QContactManager::Error operationError = QContactManager::NoError;
    for (int i = 0; i < contactIds.count(); i++) {
        current = contactIds.at(i);
        if (!removeContact(current, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some errors occurred
    return (*error == QContactManager::NoError);
}

/*! \reimp */
QList<QContactRelationship> QContactMemoryEngine::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const
{
    QContactId defaultId;
    QList<QContactRelationship> retn;
    for (int i = 0; i < d->m_relationships.size(); i++) {
        QContactRelationship curr = d->m_relationships.at(i);

        // check that the relationship type matches
        if (curr.relationshipType() != relationshipType && !relationshipType.isEmpty())
            continue;

        // if the participantId argument is default constructed, then the relationship matches.
        if (participantId == defaultId) {
            retn.append(curr);
            continue;
        }

        // otherwise, check that the participant exists and plays the required role in the relationship.
        if (role == QContactRelationship::First && curr.first() == participantId) {
            retn.append(curr);
        } else if (role == QContactRelationship::Second && curr.second() == participantId) {
            retn.append(curr);
        } else if (role == QContactRelationship::Either && (curr.first() == participantId || curr.second() == participantId)) {
            retn.append(curr);
        }
    }

    *error = QContactManager::NoError;
    if (retn.isEmpty())
        *error = QContactManager::DoesNotExistError;
    return retn;
}

bool QContactMemoryEngine::saveRelationship(QContactRelationship* relationship, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    // Attempt to validate the relationship.
    // first, check that the source contact exists and is in this manager.
    QString myUri = managerUri();
    int firstContactIndex = d->m_contactIds.indexOf(relationship->first().localId());
    if ((!relationship->first().managerUri().isEmpty() && relationship->first().managerUri() != myUri)
            ||firstContactIndex == -1) {
        *error = QContactManager::InvalidRelationshipError;
        return false;
    }

    // second, check that the second contact exists (if it's local); we cannot check other managers' contacts.
    QContactId dest = relationship->second();
    int secondContactIndex = d->m_contactIds.indexOf(dest.localId());

    if (dest.managerUri().isEmpty() || dest.managerUri() == myUri) {
        // this entry in the destination list is supposedly stored in this manager.
        // check that it exists, and that it isn't the source contact (circular)
        if (secondContactIndex == -1 || dest.localId() == relationship->first().localId()) {
            *error = QContactManager::InvalidRelationshipError;
            return false;
        }
    }

    // the relationship is valid.  We need to update the manager URIs in the second contact if it is empty to our URI.
    if (dest.managerUri().isEmpty()) {
        // need to update the URI
        dest.setManagerUri(myUri);
        relationship->setSecond(dest);
    }

    // check to see if the relationship already exists in the database.  If so, replace.
    // We do this because we don't want duplicates in our lists / maps of relationships.
    *error = QContactManager::NoError;
    QList<QContactRelationship> allRelationships = d->m_relationships;
    for (int i = 0; i < allRelationships.size(); i++) {
        QContactRelationship curr = allRelationships.at(i);
        if (curr == *relationship) {
            return true;
            // TODO: set error to AlreadyExistsError and return false?
        }
    }

    // no matching relationship; must be new.  append it to lists in our map of relationships where required.
    QList<QContactRelationship> firstRelationships = d->m_orderedRelationships.value(relationship->first().localId());
    QList<QContactRelationship> secondRelationships = d->m_orderedRelationships.value(relationship->second().localId());
    firstRelationships.append(*relationship);
    secondRelationships.append(*relationship);
    d->m_orderedRelationships.insert(relationship->first().localId(), firstRelationships);
    d->m_orderedRelationships.insert(relationship->second().localId(), secondRelationships);
    changeSet.insertAddedRelationshipsContact(relationship->first().localId());
    changeSet.insertAddedRelationshipsContact(relationship->second().localId());

    // update the contacts involved
    QContactManagerEngine::setContactRelationships(&d->m_contacts[firstContactIndex], firstRelationships);
    QContactManagerEngine::setContactRelationships(&d->m_contacts[secondContactIndex], secondRelationships);

    // finally, insert into our list of all relationships, and return.
    d->m_relationships.append(*relationship);
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    QContactManager::Error functionError;
    QContactChangeSet changeSet;

    for (int i = 0; i < relationships->size(); i++) {
        QContactRelationship curr = relationships->at(i);
        saveRelationship(&curr, changeSet, &functionError);
        if (functionError != QContactManager::NoError && errorMap)
            errorMap->insert(i, functionError);

        // and replace the current relationship with the updated version.
        relationships->replace(i, curr);

        // also, update the total error if it did not succeed.
        if (functionError != QContactManager::NoError)
            *error = functionError;
    }

    d->emitSharedSignals(&changeSet);
    return (*error == QContactManager::NoError);
}

bool QContactMemoryEngine::removeRelationship(const QContactRelationship& relationship, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    // attempt to remove it from our list of relationships.
    if (!d->m_relationships.removeOne(relationship)) {
        *error = QContactManager::DoesNotExistError;
        return false;
    }

    // if that worked, then we need to remove it from the two locations in our map, also.
    QList<QContactRelationship> firstRelationships = d->m_orderedRelationships.value(relationship.first().localId());
    QList<QContactRelationship> secondRelationships = d->m_orderedRelationships.value(relationship.second().localId());
    firstRelationships.removeOne(relationship);
    secondRelationships.removeOne(relationship);
    d->m_orderedRelationships.insert(relationship.first().localId(), firstRelationships);
    d->m_orderedRelationships.insert(relationship.second().localId(), secondRelationships);

    // Update the contacts as well
    int firstContactIndex = d->m_contactIds.indexOf(relationship.first().localId());
    int secondContactIndex = relationship.second().managerUri() == managerUri() ? d->m_contactIds.indexOf(relationship.second().localId()) : -1;
    if (firstContactIndex != -1)
        QContactMemoryEngine::setContactRelationships(&d->m_contacts[firstContactIndex], firstRelationships);
    if (secondContactIndex != -1)
        QContactMemoryEngine::setContactRelationships(&d->m_contacts[secondContactIndex], secondRelationships);

    // set our changes, and return.
    changeSet.insertRemovedRelationshipsContact(relationship.first().localId());
    changeSet.insertRemovedRelationshipsContact(relationship.second().localId());
    *error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    QContactManager::Error functionError;
    QContactChangeSet cs;
    for (int i = 0; i < relationships.size(); i++) {
        removeRelationship(relationships.at(i), cs, &functionError);

        // update the total error if it did not succeed.
        if (functionError != QContactManager::NoError) {
            if (errorMap)
                errorMap->insert(i, functionError);
            *error = functionError;
        }
    }

    d->emitSharedSignals(&cs);
    return (*error == QContactManager::NoError);
}

/*! \reimp */
QMap<QString, QContactDetailDefinition> QContactMemoryEngine::detailDefinitions(const QString& contactType, QContactManager::Error* error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QContactManagerEngine::schemaDefinitions();
    }

    *error = QContactManager::NoError;
    return d->m_definitions.value(contactType);
}

bool QContactMemoryEngine::saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet. TODO.
    Q_UNUSED(changeSet);

    if (!validateDefinition(def, error)) {
        return false;
    }

    detailDefinitions(contactType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QContactDetailDefinition> defsForThisType = d->m_definitions.value(contactType);
    defsForThisType.insert(def.name(), def);
    d->m_definitions.insert(contactType, defsForThisType);

    *error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error)
{
    QContactChangeSet changeSet;
    bool retn = saveDetailDefinition(def, contactType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

bool QContactMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any contact data, and add the ids of those contacts
    // to the change set.  TODO!
    Q_UNUSED(changeSet);

    if (definitionId.isEmpty()) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    detailDefinitions(contactType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QContactDetailDefinition> defsForThisType = d->m_definitions.value(contactType);
    bool success = defsForThisType.remove(definitionId);
    d->m_definitions.insert(contactType, defsForThisType);
    if (success)
        *error = QContactManager::NoError;
    else
        *error = QContactManager::DoesNotExistError;
    return success;
}

/*! \reimp */
bool QContactMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error)
{
    QContactChangeSet changeSet;
    bool retn = removeDetailDefinition(definitionId, contactType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! \reimp */
void QContactMemoryEngine::requestDestroyed(QContactAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*! \reimp */
bool QContactMemoryEngine::startRequest(QContactAbstractRequest* req)
{
    if (!req)
        return false;
    updateRequestState(req, QContactAbstractRequest::ActiveState);
    performAsynchronousOperation(req);
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::cancelRequest(QContactAbstractRequest* req)
{
    Q_UNUSED(req); // we can't cancel since we complete immediately
    return false;
}

/*! \reimp */
bool QContactMemoryEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    // in our implementation, we always complete any operation we start.
    Q_UNUSED(msecs);
    Q_UNUSED(req);

    return true;
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void QContactMemoryEngine::performAsynchronousOperation(QContactAbstractRequest *currentRequest)
{
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
            QContactFetchHint fetchHint = r->fetchHint();

            QContactManager::Error operationError;
            QList<QContact> requestedContacts = contacts(filter, sorting, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedContacts.isEmpty() || operationError != QContactManager::NoError)
                updateContactFetchRequest(r, requestedContacts, operationError, QContactAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::ContactLocalIdFetchRequest:
        {
            QContactLocalIdFetchRequest* r = static_cast<QContactLocalIdFetchRequest*>(currentRequest);
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();

            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactLocalId> requestedContactIds = contactIds(filter, sorting, &operationError);

            if (!requestedContactIds.isEmpty() || operationError != QContactManager::NoError)
                updateContactLocalIdFetchRequest(r, requestedContactIds, operationError, QContactAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::ContactSaveRequest:
        {
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(currentRequest);
            QList<QContact> contacts = r->contacts();

            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;
            saveContacts(&contacts, &errorMap, &operationError);

            updateContactSaveRequest(r, contacts, operationError, errorMap, QContactAbstractRequest::FinishedState);
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

                if (tempError != QContactManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QContactManager::NoError)
                updateContactRemoveRequest(r, operationError, errorMap, QContactAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QContactAbstractRequest::FinishedState);
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

                if (tempError != QContactManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || !requestedDefinitions.isEmpty() || operationError != QContactManager::NoError)
                updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QContactAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionSaveRequest:
        {
            QContactDetailDefinitionSaveRequest* r = static_cast<QContactDetailDefinitionSaveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;
            QList<QContactDetailDefinition> definitions = r->definitions();
            QList<QContactDetailDefinition> savedDefinitions;

            QContactManager::Error tempError;
            for (int i = 0; i < definitions.size(); i++) {
                QContactDetailDefinition current = definitions.at(i);
                saveDetailDefinition(current, r->contactType(), changeSet, &tempError);
                savedDefinitions.append(current);

                if (tempError != QContactManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // update the request with the results.
            updateDefinitionSaveRequest(r, savedDefinitions, operationError, errorMap, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QContactDetailDefinitionRemoveRequest* r = static_cast<QContactDetailDefinitionRemoveRequest*>(currentRequest);
            QStringList names = r->definitionNames();

            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;

            for (int i = 0; i < names.size(); i++) {
                QContactManager::Error tempError;
                removeDetailDefinition(names.at(i), r->contactType(), changeSet, &tempError);

                if (tempError != QContactManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // there are no results, so just update the status with the error.
            updateDefinitionRemoveRequest(r, operationError, errorMap, QContactAbstractRequest::FinishedState);
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
            if (!requestedRelationships.isEmpty() || operationError != QContactManager::NoError)
                updateRelationshipFetchRequest(r, requestedRelationships, operationError, QContactAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::RelationshipRemoveRequest:
        {
            QContactRelationshipRemoveRequest* r = static_cast<QContactRelationshipRemoveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QList<QContactRelationship> relationshipsToRemove = r->relationships();
            QMap<int, QContactManager::Error> errorMap;

            removeRelationships(r->relationships(), &errorMap, &operationError);

            if (!errorMap.isEmpty() || operationError != QContactManager::NoError)
                updateRelationshipRemoveRequest(r, operationError, errorMap, QContactAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QContactAbstractRequest::FinishedState);
        }
        break;

        case QContactAbstractRequest::RelationshipSaveRequest:
        {
            QContactRelationshipSaveRequest* r = static_cast<QContactRelationshipSaveRequest*>(currentRequest);
            QContactManager::Error operationError = QContactManager::NoError;
            QMap<int, QContactManager::Error> errorMap;
            QList<QContactRelationship> requestRelationships = r->relationships();

            saveRelationships(&requestRelationships, &errorMap, &operationError);

            // update the request with the results.
            updateRelationshipSaveRequest(r, requestRelationships, operationError, errorMap, QContactAbstractRequest::FinishedState);
        }
        break;

        default: // unknown request type.
        break;
    }

    // now emit any signals we have to emit
    d->emitSharedSignals(&changeSet);
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    if (!supportedContactTypes().contains(contactType))
        return false;

    switch (feature) {
        case QContactManager::Groups:
        case QContactManager::ActionPreferences:
        case QContactManager::Relationships:
        case QContactManager::ArbitraryRelationshipTypes:
        case QContactManager::RelationshipOrdering:
        case QContactManager::MutableDefinitions:
            return true;
        case QContactManager::Anonymous:
            return d->m_anonymous;
        case QContactManager::SelfContact:
            return true;

        default:
            return false;
    }
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::isRelationshipTypeSupported(const QString& relationshipType, const QString& contactType) const
{
    // the memory backend supports arbitrary relationship types
    // but some relationship types don't make sense for groups.
    if (contactType == QContactType::TypeGroup) {
        if (relationshipType == QContactRelationship::HasSpouse || relationshipType == QContactRelationship::HasAssistant) {
            return false;
        }
    }

    // all other relationship types for all contact types are supported.
    return true;
}

/*!
 * \reimp
 */
QList<QVariant::Type> QContactMemoryEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);
    st.append(QVariant::Time);
    st.append(QVariant::Bool);
    st.append(QVariant::Char);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::LongLong);
    st.append(QVariant::ULongLong);
    st.append(QVariant::Double);

    return st;
}

/*!
 * This function is deprecated.  Use QContactManagerEngine::isFilterSupported() instead!
 * The function returns true if the backend natively supports the given filter \a filter, otherwise false.
 */
bool QContactMemoryEngine::isFilterSupported(const QContactFilter& filter) const
{
    Q_UNUSED(filter);
    // Until we add hashes for common stuff, fall back to slow code
    return false;
}

#include "moc_qcontactmemorybackend_p.cpp"

QTM_END_NAMESPACE

