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

#include "qorganizeritemmanager.h"

#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemrequests.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemdetails.h"

#include "qorganizeritemmemorybackend_p.h"

#include <QTimer>
#include <QUuid>
#include <QSharedData>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemMemoryEngine
  \brief The QOrganizerItemMemoryEngine class provides an in-memory implementation
  of a organizeritems backend.
 
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
QMap<QString, QOrganizerItemMemoryEngineData*> QOrganizerItemMemoryEngine::engineDatas;

/*!
 * Factory function for creating a new in-memory backend, based
 * on the given \a parameters.
 *
 * The same engine will be returned for multiple calls with the
 * same value for the "id" parameter, while one of them is in scope.
 */
QOrganizerItemMemoryEngine* QOrganizerItemMemoryEngine::createMemoryEngine(const QMap<QString, QString>& parameters)
{
    bool anonymous = false;
    QString idValue = parameters.value(QLatin1String("id"));
    if (idValue.isNull() || idValue.isEmpty()) {
        // no store given?  new, anonymous store.
        idValue = QUuid::createUuid().toString();
        anonymous = true;
    }

    QOrganizerItemMemoryEngineData* data = engineDatas.value(idValue);
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QOrganizerItemMemoryEngineData();
        data->m_id = idValue;
        data->m_anonymous = anonymous;
        engineDatas.insert(idValue, data);
    }
    return new QOrganizerItemMemoryEngine(data);
}

/*!
 * Constructs a new in-memory backend which shares the given \a data with
 * other shared memory engines.
 */
QOrganizerItemMemoryEngine::QOrganizerItemMemoryEngine(QOrganizerItemMemoryEngineData* data)
    : d(data)
{
    d->m_sharedEngines.append(this);
}

/*! Frees any memory used by this engine */
QOrganizerItemMemoryEngine::~QOrganizerItemMemoryEngine()
{
    d->m_sharedEngines.removeAll(this);
    if (!d->m_refCount.deref()) {
        engineDatas.remove(d->m_id);
        delete d;
    }
}

/*! \reimp */
QString QOrganizerItemMemoryEngine::managerName() const
{
    return QLatin1String("memory");
}

/*! \reimp */
QMap<QString, QString> QOrganizerItemMemoryEngine::managerParameters() const
{
    QMap<QString, QString> params;
    params.insert(QLatin1String("id"), d->m_id);
    return params;
}

/*! \reimp */
QOrganizerItem QOrganizerItemMemoryEngine::item(const QOrganizerItemLocalId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    int index = d->m_organizeritemIds.indexOf(organizeritemId);
    if (index != -1) {
        // found the organizeritem successfully.
        *error = QOrganizerItemManager::NoError;
        return d->m_organizeritems.at(index);
    }

    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerItem();
}

/*! \reimp */
QList<QOrganizerItemLocalId> QOrganizerItemMemoryEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    /* Special case the fast case */
    if (filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0) {
        return d->m_organizeritemIds;
    } else {
        QList<QOrganizerItem> clist = organizeritems(filter, sortOrders, QOrganizerItemFetchHint(), error);

        /* Extract the ids */
        QList<QOrganizerItemLocalId> ids;
        foreach(const QOrganizerItem& c, clist)
            ids.append(c.localId());

        return ids;
    }
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    Q_UNUSED(error);

    QList<QOrganizerItem> sorted;

    /* First filter out organizeritems - check for default filter first */
    if (filter.type() == QOrganizerItemFilter::DefaultFilter) {
        foreach(const QOrganizerItem&c, d->m_organizeritems) {
            QOrganizerItemManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    } else {
        foreach(const QOrganizerItem&c, d->m_organizeritems) {
            if (QOrganizerItemManagerEngine::testFilter(filter, c))
                QOrganizerItemManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    }

    return sorted;
}

/*! Saves the given organizeritem \a theOrganizerItem, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::saveItem(QOrganizerItem* theOrganizerItem, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // ensure that the organizeritem's details conform to their definitions
    if (!validateOrganizerItem(*theOrganizerItem, error)) {
        return false;
    }

    // check to see if this organizeritem already exists
    int index = d->m_organizeritemIds.indexOf(theOrganizerItem->id().localId());
    if (index != -1) {
        /* We also need to check that there are no modified create only details */
        QOrganizerItem oldOrganizerItem = d->m_organizeritems.at(index);

        if (oldOrganizerItem.type() != theOrganizerItem->type()) {
            *error = QOrganizerItemManager::AlreadyExistsError;
            return false;
        }

        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        QOrganizerItemManagerEngine::setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        // synthesize the display label for the organizeritem.
        setOrganizerItemDisplayLabel(theOrganizerItem, synthesizedDisplayLabel(*theOrganizerItem, error));

        // Looks ok, so continue
        d->m_organizeritems.replace(index, *theOrganizerItem);
        changeSet.insertChangedOrganizerItem(theOrganizerItem->localId());
    } else {
        // id does not exist; if not zero, fail.
        QOrganizerItemId newId;
        newId.setManagerUri(managerUri());
        if (theOrganizerItem->id() != QOrganizerItemId() && theOrganizerItem->id() != newId) {
            // the ID is not empty, and it doesn't identify an existing organizeritem in our database either.
            *error = QOrganizerItemManager::DoesNotExistError;
            return false;
        }

        /* New organizeritem */
        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        // update the organizeritem item - set its ID
        newId.setLocalId(++d->m_nextOrganizerItemId);
        theOrganizerItem->setId(newId);

        // synthesize the display label for the organizeritem.
        setOrganizerItemDisplayLabel(theOrganizerItem, synthesizedDisplayLabel(*theOrganizerItem, error));

        // finally, add the organizeritem to our internal lists and return
        d->m_organizeritems.append(*theOrganizerItem);                   // add organizeritem to list
        d->m_organizeritemIds.append(theOrganizerItem->localId());  // track the organizeritem id.

        changeSet.insertAddedOrganizerItem(theOrganizerItem->localId());
    }

    *error = QOrganizerItemManager::NoError;     // successful.
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveItems(QList<QOrganizerItem>* organizeritems, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if(errorMap) {
        errorMap->clear();
    }

    if (!organizeritems) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    QOrganizerItemChangeSet changeSet;
    QOrganizerItem current;
    QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
    for (int i = 0; i < organizeritems->count(); i++) {
        current = organizeritems->at(i);
        if (!saveOrganizerItem(&current, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        } else {
            (*organizeritems)[i] = current;
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some error occurred
    return (*error == QOrganizerItemManager::NoError);
}

/*! Removes the organizeritem identified by the given \a organizeritemId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems and relationships as required */
bool QOrganizerItemMemoryEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    int index = d->m_organizeritemIds.indexOf(organizeritemId);

    if (index == -1) {
        *error = QOrganizerItemManager::DoesNotExistError;
        return false;
    }

    // remove the organizeritem from any relationships it was in.
    QOrganizerItemId thisOrganizerItem;
    thisOrganizerItem.setManagerUri(managerUri());
    thisOrganizerItem.setLocalId(organizeritemId);
    QList<QOrganizerItemRelationship> allRelationships = relationships(QString(), thisOrganizerItem, QOrganizerItemRelationship::Either, error);
    if (*error != QOrganizerItemManager::NoError && *error != QOrganizerItemManager::DoesNotExistError) {
        *error = QOrganizerItemManager::UnspecifiedError; // failed to clean up relationships
        return false;
    }

    // this is meant to be a transaction, so if any of these fail, we're in BIG TROUBLE.
    // a real backend will use DBMS transactions to ensure database integrity.
    removeRelationships(allRelationships, 0, error);

    // having cleaned up the relationships, remove the organizeritem from the lists.
    d->m_organizeritems.removeAt(index);
    d->m_organizeritemIds.removeAt(index);
    *error = QOrganizerItemManager::NoError;

    // and if it was the self organizeritem, reset the self organizeritem id
    if (organizeritemId == d->m_selfOrganizerItemId) {
        d->m_selfOrganizerItemId = QOrganizerItemLocalId(0);
        changeSet.setOldAndNewSelfOrganizerItemId(QPair<QOrganizerItemLocalId, QOrganizerItemLocalId>(organizeritemId, QOrganizerItemLocalId(0)));
    }

    changeSet.insertRemovedOrganizerItem(organizeritemId);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeItems(const QList<QOrganizerItemLocalId>& organizeritemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if (organizeritemIds.count() == 0) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }
    
    QOrganizerItemChangeSet changeSet;
    QOrganizerItemLocalId current;
    QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
    for (int i = 0; i < organizeritemIds.count(); i++) {
        current = organizeritemIds.at(i);
        if (!removeOrganizerItem(current, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some errors occurred
    return (*error == QOrganizerItemManager::NoError);
}

/*! \reimp */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemMemoryEngine::detailDefinitions(const QString& organizeritemType, QOrganizerItemManager::Error* error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QOrganizerItemManagerEngine::schemaDefinitions();
    }

    *error = QOrganizerItemManager::NoError;
    return d->m_definitions.value(organizeritemType);
}

/*! Saves the given detail definition \a def, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet. TODO.
    Q_UNUSED(changeSet);

    if (!validateDefinition(def, error)) {
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    defsForThisType.insert(def.name(), def);
    d->m_definitions.insert(organizeritemType, defsForThisType);

    *error = QOrganizerItemManager::NoError;
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = saveDetailDefinition(def, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! Removes the detail definition identified by \a definitionId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any organizeritem data, and add the ids of those organizeritems
    // to the change set.  TODO!
    Q_UNUSED(changeSet);

    if (definitionId.isEmpty()) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    bool success = defsForThisType.remove(definitionId);
    d->m_definitions.insert(organizeritemType, defsForThisType);
    if (success)
        *error = QOrganizerItemManager::NoError;
    else
        *error = QOrganizerItemManager::DoesNotExistError;
    return success;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! \reimp */
void QOrganizerItemMemoryEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    if (!req)
        return false;
    updateRequestState(req, QOrganizerItemAbstractRequest::ActiveState);
    performAsynchronousOperation(req);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req); // we can't cancel since we complete immediately
    return false;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
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
void QOrganizerItemMemoryEngine::performAsynchronousOperation(QOrganizerItemAbstractRequest *currentRequest)
{
    // store up changes, and emit signals once at the end of the (possibly batch) operation.
    QOrganizerItemChangeSet changeSet;

    // Now perform the active request and emit required signals.
    Q_ASSERT(currentRequest->state() == QOrganizerItemAbstractRequest::ActiveState);
    switch (currentRequest->type()) {
        case QOrganizerItemAbstractRequest::OrganizerItemFetchRequest:
        {
            QOrganizerItemFetchRequest* r = static_cast<QOrganizerItemFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();
            QOrganizerItemFetchHint fetchHint = r->fetchHint();

            QOrganizerItemManager::Error operationError;
            QList<QOrganizerItem> requestedOrganizerItems = organizeritems(filter, sorting, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedOrganizerItems.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateOrganizerItemFetchRequest(r, requestedOrganizerItems, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::OrganizerItemLocalIdFetchRequest:
        {
            QOrganizerItemLocalIdFetchRequest* r = static_cast<QOrganizerItemLocalIdFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemLocalId> requestedOrganizerItemIds = organizeritemIds(filter, sorting, &operationError);

            if (!requestedOrganizerItemIds.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateOrganizerItemLocalIdFetchRequest(r, requestedOrganizerItemIds, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::OrganizerItemSaveRequest:
        {
            QOrganizerItemSaveRequest* r = static_cast<QOrganizerItemSaveRequest*>(currentRequest);
            QList<QOrganizerItem> organizeritems = r->organizeritems();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            saveOrganizerItems(&organizeritems, &errorMap, &operationError);

            updateOrganizerItemSaveRequest(r, organizeritems, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::OrganizerItemRemoveRequest:
        {
            // this implementation provides scant information to the user
            // the operation either succeeds (all organizeritems matching the filter were removed)
            // or it fails (one or more organizeritems matching the filter could not be removed)
            // if a failure occurred, the request error will be set to the most recent
            // error that occurred during the remove operation.
            QOrganizerItemRemoveRequest* r = static_cast<QOrganizerItemRemoveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemLocalId> organizeritemsToRemove = r->organizeritemIds();
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < organizeritemsToRemove.size(); i++) {
                QOrganizerItemManager::Error tempError;
                removeOrganizerItem(organizeritemsToRemove.at(i), changeSet, &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateOrganizerItemRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:
        {
            QOrganizerItemDetailDefinitionFetchRequest* r = static_cast<QOrganizerItemDetailDefinitionFetchRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QMap<QString, QOrganizerItemDetailDefinition> requestedDefinitions;
            QStringList names = r->definitionNames();
            if (names.isEmpty())
                names = detailDefinitions(r->organizeritemType(), &operationError).keys(); // all definitions.

            QOrganizerItemManager::Error tempError;
            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemDetailDefinition current = detailDefinition(names.at(i), r->organizeritemType(), &tempError);
                requestedDefinitions.insert(names.at(i), current);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || !requestedDefinitions.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
        {
            QOrganizerItemDetailDefinitionSaveRequest* r = static_cast<QOrganizerItemDetailDefinitionSaveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QList<QOrganizerItemDetailDefinition> definitions = r->definitions();
            QList<QOrganizerItemDetailDefinition> savedDefinitions;

            QOrganizerItemManager::Error tempError;
            for (int i = 0; i < definitions.size(); i++) {
                QOrganizerItemDetailDefinition current = definitions.at(i);
                saveDetailDefinition(current, r->organizeritemType(), changeSet, &tempError);
                savedDefinitions.append(current);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // update the request with the results.
            updateDefinitionSaveRequest(r, savedDefinitions, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QOrganizerItemDetailDefinitionRemoveRequest* r = static_cast<QOrganizerItemDetailDefinitionRemoveRequest*>(currentRequest);
            QStringList names = r->definitionNames();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemManager::Error tempError;
                removeDetailDefinition(names.at(i), r->organizeritemType(), changeSet, &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // there are no results, so just update the status with the error.
            updateDefinitionRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::RelationshipFetchRequest:
        {
            QOrganizerItemRelationshipFetchRequest* r = static_cast<QOrganizerItemRelationshipFetchRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemManager::Error> operationErrors;
            QList<QOrganizerItemRelationship> allRelationships = relationships(QString(), QOrganizerItemId(), QOrganizerItemRelationship::Either, &operationError);
            QList<QOrganizerItemRelationship> requestedRelationships;

            // select the requested relationships.
            for (int i = 0; i < allRelationships.size(); i++) {
                QOrganizerItemRelationship currRel = allRelationships.at(i);
                if (r->first() != QOrganizerItemId() && r->first() != currRel.first())
                    continue;
                if (r->second() != QOrganizerItemId() && r->second() != currRel.second())
                    continue;
                if (!r->relationshipType().isEmpty() && r->relationshipType() != currRel.relationshipType())
                    continue;
                requestedRelationships.append(currRel);
            }

            // update the request with the results.
            if (!requestedRelationships.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateRelationshipFetchRequest(r, requestedRelationships, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::RelationshipRemoveRequest:
        {
            QOrganizerItemRelationshipRemoveRequest* r = static_cast<QOrganizerItemRelationshipRemoveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemRelationship> relationshipsToRemove = r->relationships();
            QMap<int, QOrganizerItemManager::Error> errorMap;

            removeRelationships(r->relationships(), &errorMap, &operationError);

            if (!errorMap.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateRelationshipRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::RelationshipSaveRequest:
        {
            QOrganizerItemRelationshipSaveRequest* r = static_cast<QOrganizerItemRelationshipSaveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QList<QOrganizerItemRelationship> requestRelationships = r->relationships();

            saveRelationships(&requestRelationships, &errorMap, &operationError);

            // update the request with the results.
            updateRelationshipSaveRequest(r, requestRelationships, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
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
bool QOrganizerItemMemoryEngine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizeritemType) const
{
    if (!supportedOrganizerItemTypes().contains(organizeritemType))
        return false;

    switch (feature) {
        case QOrganizerItemManager::Groups:
        case QOrganizerItemManager::ActionPreferences:
        case QOrganizerItemManager::Relationships:
        case QOrganizerItemManager::ArbitraryRelationshipTypes:
        case QOrganizerItemManager::MutableDefinitions:
            return true;
        case QOrganizerItemManager::Anonymous:
            return d->m_anonymous;
        case QOrganizerItemManager::SelfOrganizerItem:
            return true;

        default:
            return false;
    }
}

/*!
 * \reimp
 */
bool QOrganizerItemMemoryEngine::isRelationshipTypeSupported(const QString& relationshipType, const QString& organizeritemType) const
{
    // the memory backend supports arbitrary relationship types
    // but some relationship types don't make sense for groups.
    if (organizeritemType == QOrganizerItemType::TypeGroup) {
        if (relationshipType == QOrganizerItemRelationship::HasSpouse || relationshipType == QOrganizerItemRelationship::HasAssistant) {
            return false;
        }
    }

    // all other relationship types for all organizeritem types are supported.
    return true;
}

/*!
 * \reimp
 */
QList<QVariant::Type> QOrganizerItemMemoryEngine::supportedDataTypes() const
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
 * The function returns true if the backend natively supports the given filter \a filter, otherwise false.
 */
bool QOrganizerItemMemoryEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);
    // Until we add hashes for common stuff, fall back to slow code
    return false;
}

#include "moc_qorganizeritemmemorybackend_p.cpp"

QTM_END_NAMESPACE

