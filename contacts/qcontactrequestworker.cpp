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

#include <QDebug>
#include <QTimer>

#include "qcontactrequests.h"
#include "qcontactmanagerengine.h"
#include "qcontactrequestworker_p.h"

/*!
 * \class QContactRequestWorker
 *
 * \brief The QContactRequestWorker class provides a common thread worker queue for QContact asynchronous requests.
 *
 * A QContactRequestWorker consists of a QContactAbstractRequest request queue.
 *
 * An instance of the QContactRequestWorker class is a thread object which is dedicated for processing asynchronous contact requests.
 * All of these requests are instances of derived classes from QContactAbstractRequest. These request instances can be added to, removed/cancelled from the
 * worker thread's internal request queue. Once these requests are processed by the worker thread, the related signals will be emitted
 * and all these requests' waiting clients will also wake up.
 *
 * \sa QContactAbstractRequest
 */

/*! Construct an worker thread. */
QContactRequestWorker::QContactRequestWorker()
    :QThread(), d(new QContactRequestWorkerData)
{
}

/*! Initializes this QContactRequestWorker from \a other */
QContactRequestWorker::QContactRequestWorker(const QContactRequestWorker& other)
    : QThread(), d(other.d)
{
}

/*! Replace the contents of this QContactRequestWorker with \a other */
QContactRequestWorker& QContactRequestWorker::operator=(const QContactRequestWorker& other)
{
    // assign
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QContactRequestWorker, stops the thread and clean up all request elements in the working queue */
QContactRequestWorker::~QContactRequestWorker()
{
    stop();
    quit();
    while (isRunning() && !wait(1)) 
        d->m_newRequestAdded.wakeAll();
}

/*!
 * Stops the worker thread. The worker thread may still run a while until it tries to process next request. 
 * If the worker thread was waiting on an empty queue, wakes it up.
 * \sa run()
 */
void QContactRequestWorker::stop()
{
    QMutexLocker locker(&d->m_mutex);
    d->m_stop = true;
    d->m_newRequestAdded.wakeAll();
}

/*!
 * Worker thread's main working loop. Runs forever until the \l stop() function called.
 * The worker thread initionally waits on the empty request queue, once the request queue is not empty, takes the first element of
 * the queue, then calls related request processing functions based on the request type. Once the request was processed, update the request's
 * status and wakes up any threads which waiting for this request.
 *
 * \sa stop()
 * \sa processContactFetchRequest()
 * \sa processContactIdFetchRequest()
 * \sa processContactSaveRequest()
 * \sa processContactRemoveRequest()
 * \sa processContactGroupFetchRequest()
 * \sa processContactGroupSaveRequest()
 * \sa processContactGroupRemoveRequest()
 * \sa processContactDetailDefinitionFetchRequest()
 * \sa processContactDetailDefinitionSaveRequest()
 * \sa processContactDetailDefinitionRemoveRequest()
 */
void QContactRequestWorker::run()
{
    QContactRequestElement *re;
    
    for(;;) {
        
        d->cleanUpFinishedRequests();
        re = d->takeFirstRequestElement();
        if (d->m_stop)
           break;
        
        Q_ASSERT(re && re->request);
        
        // check to see if it is cancelling; if so, cancel it and perform update.
        if (re->request->status() == QContactAbstractRequest::Cancelling) {
            QList<QContactManager::Error> dummy;
            QContactManagerEngine::updateRequestStatus(re->request, QContactManager::NoError, dummy, QContactAbstractRequest::Cancelled);
        }

        if (re->request->isFinished() || !re->request->manager()) {
            removeRequest(re->request);
            continue;
        }

        // Now perform the active request and emit required signals.
        Q_ASSERT(re->request->status() == QContactAbstractRequest::Active);
        switch (re->request->type()) {
            case QContactAbstractRequest::ContactFetchRequest:
                processContactFetchRequest(static_cast<QContactFetchRequest*>(re->request));
                break;
            case QContactAbstractRequest::ContactIdFetchRequest:
                processContactIdFetchRequest(static_cast<QContactIdFetchRequest*>(re->request));
                break;
            case QContactAbstractRequest::ContactSaveRequest:
                processContactSaveRequest(static_cast<QContactSaveRequest*>(re->request));
                break;
            case QContactAbstractRequest::ContactRemoveRequest:
                processContactRemoveRequest(static_cast<QContactRemoveRequest*>(re->request));
                break;
            case QContactAbstractRequest::GroupFetchRequest:
                processContactGroupFetchRequest(static_cast<QContactGroupFetchRequest*>(re->request));
                break;
            case QContactAbstractRequest::GroupSaveRequest:
                processContactGroupSaveRequest(static_cast<QContactGroupSaveRequest*>(re->request));
                break;
            case QContactAbstractRequest::GroupRemoveRequest:
                processContactGroupRemoveRequest(static_cast<QContactGroupRemoveRequest*>(re->request));
                break;
            case QContactAbstractRequest::DetailDefinitionFetchRequest:
                processContactDetailDefinitionFetchRequest(static_cast<QContactDetailDefinitionFetchRequest*>(re->request));
                break;
            case QContactAbstractRequest::DetailDefinitionSaveRequest:
                processContactDetailDefinitionSaveRequest(static_cast<QContactDetailDefinitionSaveRequest*>(re->request));
                break;
            case QContactAbstractRequest::DetailDefinitionRemoveRequest:
                processContactDetailDefinitionRemoveRequest(static_cast<QContactDetailDefinitionRemoveRequest*>(re->request));
                break;
            default:
                break;
        }//switch
        re->condition.wakeAll();
        removeRequest(re->request);
    }//for
    
    {
        QMutexLocker locker(&d->m_mutex);
        foreach (QContactRequestElement* re, d->m_requestQueue) {
            if (re)
                removeRequest(re->request);
        }
        d->m_requestQueue.clear();
        d->m_requestMap.clear();
        d->cleanUpFinishedRequests(true);
    }
}


/*!
 * Returns true if the given \a req successfully added into the working queue, false if not.
 * \sa removeRequest()
 * \sa cancelRequest()
 */
bool QContactRequestWorker::addRequest(QContactAbstractRequest* req)
{
    if (req) {
        QMutexLocker locker(&d->m_mutex);
        
        if (!d->m_requestMap.contains(req)) {
            QContactRequestElement* re = new QContactRequestElement;
            if (re) {
                re->request = req;
                re->waiting = false;
                d->m_requestQueue.enqueue(re);
                d->m_requestMap.insert(req, re);
                QList<QContactManager::Error> dummy;
                QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Active);
                d->m_newRequestAdded.wakeAll();
                return true;
            }
        }
    }
    return false;
}

/*!
 * Returns true if the given \a req successfully removed from the working queue, false if not.
 * \sa addRequest()
 */
bool QContactRequestWorker::removeRequest(QContactAbstractRequest* req)
{
    if (req) {
        QMutexLocker locker(&d->m_mutex);
        QContactRequestElement * re = d->m_requestMap.value(req);
        if (re) {
            d->m_requestQueue.removeOne(re);
            d->m_requestMap.remove(req);
            d->m_removedRequests.append(re);
            return true;
        }
    }
    return false;
}

/*!
 * Update the request's status to QContactAbstractRequest::Cancelling, returns true if sucessful, false if not.
 * \sa addRequest()
 */
bool QContactRequestWorker::cancelRequest(QContactAbstractRequest* req)
{
    if (req) {
        QMutexLocker locker(&d->m_mutex);
        QList<QContactManager::Error> dummy;
        QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy, QContactAbstractRequest::Cancelling);
        return true;
    }
    return false;
}

/*!
 * Blocks the caller until the request has been completed by the worker thread or worker thread signals that more partial results 
 * are available for the request, or until \a msecs milliseconds has elapsed.
 * If \a msecs is zero, this function will block indefinitely.
 * Returns true if the request was cancelled or completed successfully within the given period, otherwise false. 
 *
 * \sa QContactAbstractRequest::waitForFinished(), QContactAbstractRequest::waitForProgress()
 */
bool QContactRequestWorker::waitRequest(QContactAbstractRequest* req, int msecs)
{
    bool ret = false;
    if (req) {
        QContactRequestElement* re = d->m_requestMap.value(req);
        if (re) {
            QMutexLocker locker(&re->mutex);
            re->waiting = true;
            if (msecs) {
                ret = re->condition.wait(&re->mutex, msecs);
            }
            ret = re->condition.wait(&re->mutex);
            re->waiting = false;
        }
    }
    return ret;
}

/*!
 * Processes the QContactFetchRequest
 * \sa QContactFetchRequest
 */
void QContactRequestWorker::processContactFetchRequest(QContactFetchRequest* req)
{
    if (req->manager()) {   
        QContactFilter filter = req->filter();
        QList<QContactSortOrder> sorting = req->sorting();
        QStringList defs = req->definitionRestrictions();

        QContactManager::Error operationError;
        QList<QContactManager::Error> operationErrors;
        QList<QContact> requestedContacts;

        QList<QUniqueId> requestedContactIds = req->manager()->contacts(filter, sorting);
        operationError = req->manager()->error();

        QContactManager::Error tempError;
        for (int i = 0; i < requestedContactIds.size(); i++) {
            QContact current = req->manager()->contact(requestedContactIds.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);

            // check for single error; update total operation error if required
            if (tempError != QContactManager::NoError)
                operationError = tempError;

            // apply the required detail definition restrictions
            if (!defs.isEmpty()) {
                QList<QContactDetail> allDetails = current.details();
                for (int j = 0; j < allDetails.size(); j++) {
                    QContactDetail d = allDetails.at(j);
                    if (!defs.contains(d.definitionName())) {
                        // this detail is not required.
                        current.removeDetail(&d);
                    }
                }
            }

            // add the contact to the result list.
            requestedContacts.append(current);
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, requestedContacts, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactIdFetchRequest
 * \sa QContactIdFetchRequest
 */
void QContactRequestWorker::processContactIdFetchRequest(QContactIdFetchRequest* req)
{
    if (req->manager()) {
        QContactFilter filter = req->filter();
        QList<QContactSortOrder> sorting = req->sorting();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QUniqueId> requestedContactIds = req->manager()->contacts(filter, sorting);
        operationError = req->manager()->error();

        QContactManagerEngine::updateRequest(req, requestedContactIds, operationError, QList<QContactManager::Error>(), QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactSaveRequest
 * \sa QContactSaveRequest
 */
void QContactRequestWorker::processContactSaveRequest(QContactSaveRequest* req)
{
    if (req->manager()) {
        QList<QContact> contacts = req->contacts();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors = req->manager()->saveContacts(&contacts);
        operationError = req->manager()->error();

        for (int i = 0; i < operationErrors.size(); i++) {
            if (operationErrors.at(i) != QContactManager::NoError) {
                operationError = operationErrors.at(i);
                break;
            }
        }

        QContactManagerEngine::updateRequest(req, contacts, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactRemoveRequest
 * \sa QContactRemoveRequest
 */
void QContactRequestWorker::processContactRemoveRequest(QContactRemoveRequest* req )
{
    if (req->manager()) {
        // this implementation provides scant information to the user
        // the operation either succeeds (all contacts matching the filter were removed)
        // or it fails (one or more contacts matching the filter could not be removed)
        // if a failure occurred, the request error will be set to the most recent
        // error that occurred during the remove operation.
        QContactFilter filter = req->filter();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QUniqueId> contactsToRemove = req->manager()->contacts(filter, QList<QContactSortOrder>());
        operationError = req->manager()->error();
    
        for (int i = 0; i < contactsToRemove.size(); i++) {
            QContactManager::Error tempError;
            
            req->manager()->removeContact(contactsToRemove.at(i));
            tempError = req->manager()->error();

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // there are no results, so just update the status with the error.
        QList<QContactManager::Error> dummy;
        QContactManagerEngine::updateRequestStatus(req, operationError, dummy, QContactAbstractRequest::Finished);
    }
}
/*!
 * Processes the QContactGroupFetchRequest
 * \sa QContactGroupFetchRequest
 */
void QContactRequestWorker::processContactGroupFetchRequest(QContactGroupFetchRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QList<QContactGroup> requestedGroups;
        QList<QUniqueId> requestedGroupIds = req->ids();
        if (requestedGroupIds.isEmpty()) {
            requestedGroupIds = req->manager()->groups(); // all groups.
            operationError = req->manager()->error();
        }

        QContactManager::Error tempError;
        for (int i = 0; i < requestedGroupIds.size(); i++) {
            QContactGroup current = req->manager()->group(requestedGroupIds.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);

            // add the group to the result list.
            requestedGroups.append(current);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, requestedGroups, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}
/*!
 * Processes the QContactGroupSaveRequest
 * \sa QContactGroupSaveRequest
 */
void QContactRequestWorker::processContactGroupSaveRequest(QContactGroupSaveRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QList<QContactGroup> groups = req->groups();
        QList<QContactGroup> savedGroups;

        for (int i = 0; i < groups.size(); i++) {
            QContactManager::Error tempError;
            QContactGroup current = groups.at(i);
            req->manager()->saveGroup(&current);
            tempError = req->manager()->error();
            savedGroups.append(current);
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        QContactManagerEngine::updateRequest(req, savedGroups, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactGroupRemoveRequest
 * \sa QContactGroupRemoveRequest
 */
void QContactRequestWorker::processContactGroupRemoveRequest(QContactGroupRemoveRequest* req)
{
    if (req->manager()) {
        QList<QUniqueId> ids = req->ids();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;

        for (int i = 0; i < ids.size(); i++) {
            QContactManager::Error tempError;
            req->manager()->removeGroup(ids.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // there are no results, so just emit our changes and update the status with any errors.
        QContactManagerEngine::updateRequestStatus(req, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}

/*!
 * Processes the QContactDetailDefinitionFetchRequest
 * \sa QContactDetailDefinitionFetchRequest
 */
void QContactRequestWorker::processContactDetailDefinitionFetchRequest(QContactDetailDefinitionFetchRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QMap<QString, QContactDetailDefinition> requestedDefinitions;
        QStringList names = req->names();
        if (names.isEmpty()) {
            names = req->manager()->detailDefinitions().keys(); // all definitions.
            operationError = req->manager()->error();
        }

        QContactManager::Error tempError;
        for (int i = 0; i < names.size(); i++) {
            QContactDetailDefinition current = req->manager()->detailDefinition(names.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);
            requestedDefinitions.insert(names.at(i), current);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, requestedDefinitions, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}
/*!
 * Processes the QContactDetailDefinitionSaveRequest
 * \sa QContactDetailDefinitionSaveRequest
 */
void QContactRequestWorker::processContactDetailDefinitionSaveRequest(QContactDetailDefinitionSaveRequest* req)
{
    if (req->manager()) {
        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        QList<QContactDetailDefinition> definitions = req->definitions();
        QList<QContactDetailDefinition> savedDefinitions;

        QContactManager::Error tempError;
        for (int i = 0; i < definitions.size(); i++) {
            QContactDetailDefinition current = definitions.at(i);
            req->manager()->saveDetailDefinition(current);
            tempError = req->manager()->error();
            savedDefinitions.append(current);
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // update the request with the results.
        QContactManagerEngine::updateRequest(req, savedDefinitions, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}
/*!
 * Processes the QContactDetailDefinitionRemoveRequest
 * \sa QContactDetailDefinitionRemoveRequest
 */
void QContactRequestWorker::processContactDetailDefinitionRemoveRequest(QContactDetailDefinitionRemoveRequest* req)
{
    if (req->manager()) {
        QStringList names = req->names();

        QContactManager::Error operationError = QContactManager::NoError;
        QList<QContactManager::Error> operationErrors;
        
        for (int i = 0; i < names.size(); i++) {
            QContactManager::Error tempError;
            req->manager()->removeDetailDefinition(names.at(i));
            tempError = req->manager()->error();
            operationErrors.append(tempError);

            if (tempError != QContactManager::NoError)
                operationError = tempError;
        }

        // there are no results, so just update the status with the error.
        QContactManagerEngine::updateRequestStatus(req, operationError, operationErrors, QContactAbstractRequest::Finished);
    }
}


QContactRequestElement* QContactRequestWorkerData::takeFirstRequestElement()
{
    QMutexLocker locker(&m_mutex);

    // take the first pending request and finish it
    if (m_requestQueue.isEmpty())
        m_newRequestAdded.wait(&m_mutex);
    if (!m_requestQueue.isEmpty())
        return m_requestQueue.head();
    return 0;
}
void QContactRequestWorkerData::cleanUpFinishedRequests(bool waitForAll)
{
    QList<QContactRequestElement*> deleteAll;
    QMutex mtx;
    QWaitCondition wc;
    for (;;) {
        foreach (QContactRequestElement* re, m_removedRequests) {
            if (re->waiting) {
                re->condition.wakeAll();
            } else {
                deleteAll.append(re);
                m_removedRequests.removeOne(re);
            }
        }
        
        foreach (QContactRequestElement* re, deleteAll) {
            if (re)
                delete re;
        }
        deleteAll.clear();
        if (!waitForAll || m_removedRequests.isEmpty()) {
            break;
        } else {
            QMutexLocker locker(&mtx);
            wc.wait(&mtx, 10);
        }
    }
}
