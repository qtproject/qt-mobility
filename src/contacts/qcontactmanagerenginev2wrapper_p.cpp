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

#include "qcontactmanagerenginev2wrapper_p.h"
#include "qcontactlocalidfilter.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactfetchbyidrequest.h"

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
    // TODO - if it's a partial save request, handle it, otherwise pass it on
    return m_engine->requestDestroyed(req);
}

bool QContactManagerEngineV2Wrapper::startRequest(QContactAbstractRequest* req)
{
    if (!req)
        return false;
    if (req->type() == QContactAbstractRequest::ContactSaveRequest
        && !static_cast<QContactSaveRequest*>(req)->definitionMask().isEmpty()) {
        // This is a partial save
        // TODO do something
    } else if (req->type() == QContactAbstractRequest::ContactFetchByIdRequest) {
        RequestController* controller = new FetchByIdRequestController(m_engine);
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
    Q_ASSERT(request);

    if (state == QContactAbstractRequest::FinishedState) {
        delete controller;
        m_controllerForRequest.remove(request);
    } else {
        updateRequestState(request, state);
    }

}

/* \reimp */
bool QContactManagerEngineV2Wrapper::cancelRequest(QContactAbstractRequest* req)
{
    // TODO - see if we know about this request
    return m_engine->cancelRequest(req);
}

/* \reimp */
bool QContactManagerEngineV2Wrapper::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    if (!m_controllerForRequest.contains(req))
        return m_engine->waitForRequestFinished(req, msecs);

    if (m_controllerForRequest.value(req)->waitForFinished(msecs)) {
        updateRequestState(req, QContactAbstractRequest::FinishedState);
        return true;
    } else {
        return false;
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
        emit stateChanged(state);
    }
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
    m_currentSubRequest = qcfr;
    connect(qcfr, SIGNAL(stateChanged(QContactAbstractRequest::State)),
            this, SLOT(handleUpdatedSubRequest(QContactAbstractRequest::State)),
            Qt::QueuedConnection);
    return qcfr->start();
}

/* This function handles the rest of the program flow in a synchronous way. */
bool FetchByIdRequestController::waitForFinished(int msecs)
{
    // If the current request is active, it must be a ContactFetchRequest.  We just need to
    // wait for it to finish, then finalize the post-processing.
    if (!m_currentSubRequest)
        return false;
    if (!m_currentSubRequest->waitForFinished(msecs))
        return false;

    handleFinishedSubRequest(m_currentSubRequest);
    return true;
}

/* One of our subrequests has finished.  Go to the next step. */
void FetchByIdRequestController::handleFinishedSubRequest(QContactAbstractRequest* subReq)
{
    // It's possibly already finished if this function is called asynchronously and waitForFinished
    // had previously been called
    if (m_finished)
        return;
    m_finished = true;

    // For a FetchByIdRequest, we know that the only subrequest is a QContactFetchRequest.
    // The next step is simply to take the results and reformat it.
    // Take the results:
    QContactFetchRequest* qcfr = static_cast<QContactFetchRequest*>(subReq);
    QList<QContact> contacts = qcfr->contacts();
    QContactManager::Error error = qcfr->error();

    delete subReq; // No need for this any more

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
            errorMap,
            error,
            QContactAbstractRequest::FinishedState);
}
