/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontact_p.h"
#include "qcontactmanagerenginev2wrapper_p.h"
#include "qcontactlocalidfilter.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactfetchbyidrequest.h"
#include <QDebug>

QTM_USE_NAMESPACE

QContactManagerEngineV2Wrapper::QContactManagerEngineV2Wrapper(QContactManagerEngine *wrappee)
    : m_engine(wrappee)
{
    Q_ASSERT(wrappee);
    // Connect all the signals
    connect(wrappee, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(wrappee, SIGNAL(contactsAdded(QList<QContactLocalId>)),
            this, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(contactsChanged(QList<QContactLocalId>)),
            this, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(contactsRemoved(QList<QContactLocalId>)),
            this, SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(relationshipsAdded(QList<QContactLocalId>)),
            this, SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)),
            this, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(selfContactIdChanged(QContactLocalId, QContactLocalId)),
            this, SIGNAL(selfContactIdChanged(QContactLocalId, QContactLocalId)));
}

QContactManagerEngineV2Wrapper::~QContactManagerEngineV2Wrapper()
{
    delete m_engine;
}

void QContactManagerEngineV2Wrapper::requestDestroyed(QContactAbstractRequest* req)
{
    RequestController* controller = m_controllerForRequest.value(req);

    if (controller) {
        // If we own it, just delete the controller (and ignore any subrequests' signals from now on)
        delete controller;
        m_controllerForRequest.insert(req, 0);
    } else {
        m_engine->requestDestroyed(req);
    }
}

bool QContactManagerEngineV2Wrapper::startRequest(QContactAbstractRequest* req)
{
    if ((req->type() == QContactAbstractRequest::ContactSaveRequest
            && !static_cast<QContactSaveRequest*>(req)->definitionMask().isEmpty())
        || (req->type() == QContactAbstractRequest::ContactFetchByIdRequest)) {
        RequestController* controller;
        if (req->type() == QContactAbstractRequest::ContactFetchByIdRequest)
            controller = new FetchByIdRequestController(m_engine);
        else
            controller = new PartialSaveRequestController(m_engine, this);
        controller->setRequest(req);
        connect(controller, SIGNAL(stateChanged(QContactAbstractRequest::State)),
                this, SLOT(requestStateChanged(QContactAbstractRequest::State)),
                Qt::QueuedConnection);
        m_controllerForRequest.insert(req, controller);
        if (controller->start()) {
            updateRequestState(req, QContactAbstractRequest::ActiveState);
            return true;
        } else {
            return false;
        }
    }

    // Otherwise, pass it on
    return m_engine->startRequest(req);
}

/* A slot connected to the stateChanged signal of a controller object.  It is signaled by the
   controller to indicate that one of the client's requests has an updated state. */
void QContactManagerEngineV2Wrapper::requestStateChanged(QContactAbstractRequest::State state)
{
    RequestController* controller = qobject_cast<RequestController*>(sender());
    Q_ASSERT(controller);
    QContactAbstractRequest* request = controller->request();

    if (state == QContactAbstractRequest::FinishedState) {
        delete controller;
        if (request) { // It's possible the request was deleted by the sender.
            // Keep the key in m_controllerForRequest but point it to null to indicate a defunct
            // controller
            m_controllerForRequest.insert(request, 0);
        }
    } else {
        updateRequestState(request, state);
    }

}

/* \reimp */
bool QContactManagerEngineV2Wrapper::cancelRequest(QContactAbstractRequest* req)
{
    if (m_controllerForRequest.contains(req)) {
        RequestController* controller = m_controllerForRequest.value(req);
        if (controller) {
            // If we own it and it hasn't already been deleted,
            // just delete the controller (and ignore any subrequests' signals from now on)
            delete controller;
            m_controllerForRequest.insert(req, 0);
        }
        return true;
    } else {
        return m_engine->cancelRequest(req);
    }
}

/* \reimp */
bool QContactManagerEngineV2Wrapper::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    if (m_controllerForRequest.contains(req)) {
        RequestController* controller = m_controllerForRequest.value(req);
        if (controller) {
            if (controller->waitForFinished(msecs)) {
                updateRequestState(req, QContactAbstractRequest::FinishedState);
                return true;
            } else {
                return false;
            }
        } else {
            // A request with a null controller means it has already finished
            return true;
        }
    } else {
        return m_engine->waitForRequestFinished(req, msecs);
    }

}

/* A static helper to twiddle with \a request's privates, setting its engine to \a engine. */
void QContactManagerEngineV2Wrapper::setEngineOfRequest(QContactAbstractRequest* request,
                                                        QContactManagerEngine* engine)
{
    request->d_ptr->m_engine = engine;
}


// General RequestController stuff
/* A single RequestController is associated with a single client QContactAbstractRequest.  It
   manages the sub-requests that need to be sent to the real engine and controls the asynchronous
   flow between start() and one or more handleUpdatedSubRequest() calls that might follow.
   waitForFinished() can also be called on a controller, which synchronously performs the rest of
   the necessary sub-requests.
 */

/* A slot connected the stateChanged signal of a sub request */
void RequestController::handleUpdatedSubRequest(QContactAbstractRequest::State state)
{
    QObject* caller = sender();
    QContactAbstractRequest* subRequest = qobject_cast<QContactAbstractRequest*>(caller);
    if (subRequest) {
        if (state == QContactAbstractRequest::FinishedState) {
            handleFinishedSubRequest(subRequest);
        } else {
            // XXX maybe Canceled should be handled
        }
    }
}

/* This function handles the rest of the program flow in a synchronous way. */
bool RequestController::waitForFinished(int msecs)
{
    // If the current request is active, it must be a ContactFetchRequest.  We just need to
    // wait for it to finish, then finalize the post-processing.
    if (m_currentSubRequest.isNull()) {
        return false;
    }
    while (!isFinished()) {
        if (!m_currentSubRequest->waitForFinished(msecs))
            return false;
        handleFinishedSubRequest(m_currentSubRequest.data());
    }
    return true;
}

// FetchByIdRequestController stuff
/* A FetchByIdRequestController is a RequestController for QContactFetchByIdRequests */
bool FetchByIdRequestController::start()
{
    // Our strategy is to translate it to a ContactFetchRequest.  Later when it finishes, we can
    // fiddle with the results to get it in the right format.
    Q_ASSERT(m_request);
    QContactFetchRequest* qcfr = new QContactFetchRequest;
    QContactLocalIdFilter lif;
    lif.setIds(static_cast<QContactFetchByIdRequest*>(m_request.data())->localIds());
    qcfr->setFilter(lif);
    qcfr->setFetchHint(qcfr->fetchHint());
    // normally, you'd set the manager, but in this case, we only have a bare engine:
    QContactManagerEngineV2Wrapper::setEngineOfRequest(qcfr, m_engine);
    m_currentSubRequest.reset(qcfr);
    connect(qcfr, SIGNAL(stateChanged(QContactAbstractRequest::State)),
            this, SLOT(handleUpdatedSubRequest(QContactAbstractRequest::State)),
            Qt::QueuedConnection);
    return qcfr->start();
}

/* One of our subrequests has finished.  Go to the next step. */
void FetchByIdRequestController::handleFinishedSubRequest(QContactAbstractRequest* subReq)
{
    // It's possibly already finished if this function is called asynchronously and waitForFinished
    // had previously been called
    if (isFinished())
        return;

    // For a FetchByIdRequest, we know that the only subrequest is a QContactFetchRequest.
    // The next step is simply to take the results and reformat it.
    // Take the results:
    QContactFetchRequest* qcfr = qobject_cast<QContactFetchRequest*>(subReq);
    QList<QContact> contacts = qcfr->contacts();
    QContactManager::Error error = qcfr->error();

    // Build an index into the results
    QHash<QContactLocalId, QContact> idMap;
    if (error == QContactManager::NoError) {
        foreach (const QContact& contact, contacts) {
            idMap.insert(contact.localId(), contact);
        }
    }

    // Find the order in which the results should be presented
    QContactFetchByIdRequest* request = static_cast<QContactFetchByIdRequest*>(m_request.data());
    QList<QContactLocalId> ids(request->localIds());

    // Build up the results and errors
    QList<QContact> results;
    QMap<int, QContactManager::Error> errorMap;
    for (int i = 0; i < ids.count(); i++) {
        QContactLocalId id(ids[i]);
        if (!idMap.contains(id)) {
            errorMap.insert(i, QContactManager::DoesNotExistError);
            if (error == QContactManager::NoError)
                error = QContactManager::DoesNotExistError;
        }
        results.append(idMap.value(id));
    }

    // Update the request object
    QContactManagerEngineV2Wrapper::updateContactFetchByIdRequest(
            request,
            results,
            error,
            errorMap,
            QContactAbstractRequest::FinishedState);
    finish();
}


// PartialSaveRequestController stuff
/* A PartialSaveRequestController is a RequestController for QContactSaveRequests with definition mask */
bool PartialSaveRequestController::start()
{
    // The first step is to fetch the existing contacts.
    QList<QContactLocalId> existingContactIds;

    // First, remove the contacts that aren't from this manager
    QList<QContact> contacts(request()->contacts());
    // Try to figure out which of our arguments are new contacts
    for(int i = 0; i < contacts.count(); i++) {
        // See if there's a contactId that's not from this manager
        const QContact c = contacts.at(i);
        if (c.id().managerUri() == m_engine->managerUri()) {
            if (c.localId() != 0) {
                m_existingIdMap.insert(i, existingContactIds.count());
                existingContactIds.append(c.localId());
            } else {
                // Strange. it's just a new contact
            }
        } else if (!c.id().managerUri().isEmpty() || c.localId() != 0) {
            // Hmm, error (wrong manager)
            m_errorMap.insert(i, QContactManager::DoesNotExistError);
        } // else new contact
    }

    // Now do the fetch and wait for the result in handleFinishedSubRequest
    QContactFetchByIdRequest* cfbir = new QContactFetchByIdRequest;
    cfbir->setLocalIds(existingContactIds);
    // normally, you'd set the manager, but in this case, we only have a bare engine:
    QContactManagerEngineV2Wrapper::setEngineOfRequest(cfbir, m_v2wrapper);
    m_currentSubRequest.reset(cfbir);
    connect(cfbir, SIGNAL(stateChanged(QContactAbstractRequest::State)),
            this, SLOT(handleUpdatedSubRequest(QContactAbstractRequest::State)),
            Qt::QueuedConnection);
    return cfbir->start();
}

/* One of our subrequests has finished.  Go to the next step. */
void PartialSaveRequestController::handleFinishedSubRequest(QContactAbstractRequest* subReq)
{
    // It's possibly already finished if this function is called asynchronously and waitForFinished
    // had previously been called
    if (isFinished())
        return;

    if (subReq->type() == QContactAbstractRequest::ContactFetchByIdRequest) {
        QContactFetchByIdRequest* cfbir = qobject_cast<QContactFetchByIdRequest*>(subReq);
        QList<QContact> contactsToSave;
        QMap<int, QContactManager::Error> fetchErrors(cfbir->errorMap());
        QList<QContact> existingContacts(cfbir->contacts());
        QList<QContact> contacts(request()->contacts());
        QSet<QString> mask(request()->definitionMask().toSet());
        for (int i = 0; i < contacts.count(); i++) {
            // See if this is an existing contact or a new one
            const int fetchedIdx = m_existingIdMap.value(i, -1);
            QContact contactToSave;
            if (fetchedIdx >= 0) {
                // See if we had an error
                if (fetchErrors[fetchedIdx] != QContactManager::NoError) {
                    m_errorMap.insert(i, fetchErrors[fetchedIdx]);
                    continue;
                }

                // Existing contact we should have fetched
                contactToSave = existingContacts.at(fetchedIdx);

                QSharedDataPointer<QContactData>& cd = QContactData::contactData(contactToSave);
                cd->removeOnly(mask);
            } else if (m_errorMap.contains(i)) {
                // A bad argument.  Leave it out of the contactsToSave list
                continue;
            } // else new contact

            // Now copy in the details from the arguments
            const QContact& c = contacts.at(i);

            // Perhaps this could do this directly rather than through saveDetail
            // but that would duplicate the checks for display label etc
            foreach (const QString& name, mask) {
                QList<QContactDetail> details = c.details(name);
                foreach(QContactDetail detail, details) {
                    contactToSave.saveDetail(&detail);
                }
            }

            m_savedToOriginalMap.append(i);
            contactsToSave.append(contactToSave);
        }

        // Now do the fetch and wait for the result
        QContactSaveRequest* csr = new QContactSaveRequest;
        csr->setContacts(contactsToSave);
        // normally, you'd set the manager, but in this case, we only have a bare engine:
        QContactManagerEngineV2Wrapper::setEngineOfRequest(csr, m_engine);
        m_currentSubRequest.reset(csr);
        connect(csr, SIGNAL(stateChanged(QContactAbstractRequest::State)),
                this, SLOT(handleUpdatedSubRequest(QContactAbstractRequest::State)),
                Qt::QueuedConnection);
        csr->start(); // TODO what if this fails?
    } else if (subReq->type() == QContactAbstractRequest::ContactSaveRequest) {
        QContactSaveRequest* csr = qobject_cast<QContactSaveRequest*>(subReq);
        QList<QContact> savedContacts(csr->contacts());
        QMap<int, QContactManager::Error> saveErrors(csr->errorMap());
        QList<QContact> contacts(request()->contacts());
        for (int i = 0; i < savedContacts.count(); i++) {
            contacts[m_savedToOriginalMap[i]].setId(savedContacts[i].id());
        }
        // Populate the m_errorMap with the m_errorMap of the attempted save
        QMap<int, QContactManager::Error>::iterator it(saveErrors.begin());
        QContactManager::Error error = QContactManager::NoError;
        while (it != saveErrors.end()) {
            error = it.value();
            m_errorMap.insert(m_savedToOriginalMap[it.key()], it.value());
            it++;
        }

        // Update the request object
        QContactManagerEngine::updateContactSaveRequest(
                request(),
                contacts,
                error,
                m_errorMap,
                QContactAbstractRequest::FinishedState);
        finish();
    } else {
        Q_ASSERT(false);
    }
}

