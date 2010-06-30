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

#include "qcontactabstractrequest.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"

#include <QMutex>
#include <QMutexLocker>

QTM_BEGIN_NAMESPACE
/*!
  \class QContactAbstractRequest

  \brief The QContactAbstractRequest class provides a mechanism for
  asynchronous requests to be made of a manager if it supports them.

  \inmodule QtContacts

  \ingroup contacts-main

  It allows a client to asynchronously request some functionality of a
  particular QContactManager.  Instances of the class will emit signals
  when the state of the request changes, or when more results become
  available.

  Clients should not attempt to create instances of this class directly,
  but should instead use the use-case-specific classes derived from this
  class.

  All such request classes have a similar interface: clients set the
  parameters of the asynchronous call, including which manager the
  request will be made of, and then call the start() slot of the request.
  The manager will then enqueue or begin to process the request, at which
  point the request's state will transition from \c InactiveState to
  \c ActiveState.  After any state transition, the request will emit the
  stateChanged() signal.  The manager may periodically update the request
  with results, at which point the request will emit the resultsAvailable()
  signal.  These results are not guaranteed to have a stable ordering.
  Error information is considered a result, so some requests will emit the
  resultsAvailable() signal even if no results are possible from the request
  (for example, a contact remove request) when the manager updates the request
  with information about any errors which may have occurred.

  Please see the class documentation of each of the use-case-specific
  classes derived from this class for information about how to retrieve
  the results of a request (including error information).  In all cases,
  those functions are synchronous, and will return the cached result set with
  which the manager has updated the request instance if the resultsAvailable()
  signal has been emitted.

  Clients can choose which signals they wish to handle from a request.
  If the client is not interested in interim results, they can choose to
  handle only the stateChanged() signal, and in the slot to which that
  signal is connected, check whether the state has changed to either
  \c FinishedState or \c CanceledState (both of which signify that the
  manager has finished handling the request, and that the request will not
  be updated with any more results).  If the client is not interested in
  any results (including error information), they may choose to delete
  the request after calling \l start(), or simply not connect the
  request's signals to any slots.

  If the request is allocated via operator new, the client must
  delete the request when they are no longer using it in order to avoid
  leaking memory.  That is, the client retains ownership of the request.

  The client may delete a heap-allocated request in various ways:
  by deleting it directly (but not within a slot connected to a signal
  emitted by the request), or by using the deleteLater() slot to schedule
  the request for deletion when control returns to the event loop (from
  within a slot connected to a signal emitted by the request, for example
  \l stateChanged()).

  An active request may be deleted by the client, but the client will not
  receive any notifications about whether the request succeeded or not,
  nor any results of the request.

  Because clients retain ownership of any request object, and may delete
  a request object at any time, manager engine implementors must be careful
  to ensure that they do not assume that a request has not been deleted
  at some point during processing of a request, particularly if the engine
  has a multithreaded implementation.  It is suggested that engine
  implementors read the \l{Qt Contacts Manager Engines} documentation for
  more information on this topic.
 */

/*!
  \fn QContactAbstractRequest::stateChanged(QContactAbstractRequest::State newState)
  This signal is emitted when the state of the request is changed.  The new state of
  the request will be contained in \a newState.
 */


/*!
  \fn QContactAbstractRequest::resultsAvailable()
  This signal is emitted when new results are available.  Results can include
  the operation error which may be accessed via error(), or derived-class-specific
  results which are accessible through the derived class API.

  \sa error()
 */

/*!
  \enum QContactAbstractRequest::RequestType
  Enumerates the various possible types of asynchronous requests
  \value InvalidRequest An invalid request
  \value ContactFetchRequest A request to fetch a list of contacts
  \value ContactLocalIdFetchRequest A request to fetch a list of local contact ids
  \value ContactRemoveRequest A request to remove a list of contacts
  \value ContactSaveRequest A request to save a list of contacts
  \value DetailDefinitionFetchRequest A request to fetch a collection of detail definitions
  \value DetailDefinitionRemoveRequest A request to remove a list of detail definitions
  \value DetailDefinitionSaveRequest A request to save a list of detail definitions
  \value RelationshipFetchRequest A request to fetch relationships between contacts
  \value RelationshipRemoveRequest A request to remove any relationships which match the request criteria
  \value RelationshipSaveRequest A request to save a list of relationships
 */

/*!
  \enum QContactAbstractRequest::State
  Enumerates the various states that a request may be in at any given time
  \value InactiveState Operation not yet started
  \value ActiveState Operation started, not yet finished
  \value CanceledState Operation is finished due to cancellation
  \value FinishedState Operation successfully completed
 */

/*!
  \fn QContactAbstractRequest::QContactAbstractRequest(QObject* parent)
  Constructs a new, invalid asynchronous request with the specified \a parent
 */

/*!
  \internal
  Constructs a new request from the given request data \a otherd with
  the given parent \a parent
*/
QContactAbstractRequest::QContactAbstractRequest(QContactAbstractRequestPrivate* otherd, QObject* parent)
    : QObject(parent), d_ptr(otherd)
{
}

/*! Cleans up the memory used by this request */
QContactAbstractRequest::~QContactAbstractRequest()
{
     delete d_ptr;
}

/*!
  Returns true if the request is in the \c QContactAbstractRequest::InactiveState state; otherwise, returns false

  \sa state()
 */
bool QContactAbstractRequest::isInactive() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QContactAbstractRequest::InactiveState);
}

/*!
  Returns true if the request is in the \c QContactAbstractRequest::ActiveState state; otherwise, returns false

  \sa state()
 */
bool QContactAbstractRequest::isActive() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QContactAbstractRequest::ActiveState);
}

/*!
  Returns true if the request is in the \c QContactAbstractRequest::FinishedState; otherwise, returns false

  \sa state()
 */
bool QContactAbstractRequest::isFinished() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QContactAbstractRequest::FinishedState);
}

/*!
  Returns true if the request is in the \c QContactAbstractRequest::CanceledState; otherwise, returns false

  \sa state()
 */
bool QContactAbstractRequest::isCanceled() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QContactAbstractRequest::CanceledState);
}

/*! Returns the overall error of the most recent asynchronous operation */
QContactManager::Error QContactAbstractRequest::error() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->m_error;
}

/*!
  \deprecated
  Returns the list of errors which occurred during the most recent asynchronous operation.  Each individual error in the list corresponds to a result in the result list.
  This function is deprecated and will be removed after the transition period has elapsed.  Use errorMap() instead.
 */
QList<QContactManager::Error> QContactAbstractRequest::errors() const
{
    return QList<QContactManager::Error>();
}

/*!
  Returns the type of this asynchronous request
 */
QContactAbstractRequest::RequestType QContactAbstractRequest::type() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->type();
}

/*!
  Returns the current state of the request.
 */
QContactAbstractRequest::State QContactAbstractRequest::state() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->m_state;
}

/*! Returns a pointer to the manager of which this request instance requests operations */
QContactManager* QContactAbstractRequest::manager() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->m_manager;
}

/*!
    Sets the manager of which this request instance requests operations to \a manager

    If the request is currently active, this function will return without updating the \a manager object.
*/
void QContactAbstractRequest::setManager(QContactManager* manager)
{
    QMutexLocker ml(&d_ptr->m_mutex);
    // In theory we might have been active and the manager didn't cancel/finish us
    if (d_ptr->m_state == QContactAbstractRequest::ActiveState && d_ptr->m_manager)
        return;
    d_ptr->m_manager = manager;
}

/*! Attempts to start the request.  Returns false if the request is not in the \c QContactAbstractRequest::Inactive, \c QContactAbstractRequest::Finished or \c QContactAbstractRequest::Cancelled states,
    or if the request was unable to be performed by the manager engine; otherwise returns true. */
bool QContactAbstractRequest::start()
{
    QMutexLocker ml(&d_ptr->m_mutex);
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine && (d_ptr->m_state == QContactAbstractRequest::CanceledState
                   || d_ptr->m_state == QContactAbstractRequest::FinishedState
                   || d_ptr->m_state == QContactAbstractRequest::InactiveState)) {
        ml.unlock();
        return engine->startRequest(this);
    }

    return false; // unable to start operation; another operation already in progress or no engine.
}

/*! Attempts to cancel the request.  Returns false if the request is not in the \c QContactAbstractRequest::Active state,
    or if the request is unable to be cancelled by the manager engine; otherwise returns true. */
bool QContactAbstractRequest::cancel()
{
    QMutexLocker ml(&d_ptr->m_mutex);
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine && d_ptr->m_state == QContactAbstractRequest::ActiveState) {
        ml.unlock();
        return engine->cancelRequest(this);
    }

    return false; // unable to cancel operation; not in progress or no engine.
}

/*! Blocks until the request has been completed by the manager engine, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero, this function will block indefinitely.
    Returns true if the request was cancelled or completed successfully within the given period, otherwise false.
    Some backends are unable to support this operation safely, and will return false immediately.
 */
bool QContactAbstractRequest::waitForFinished(int msecs)
{
    QMutexLocker ml(&d_ptr->m_mutex);
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine) {
        switch (d_ptr->m_state) {
        case QContactAbstractRequest::ActiveState:
            ml.unlock();
            return engine->waitForRequestFinished(this, msecs);
        case QContactAbstractRequest::CanceledState:
        case QContactAbstractRequest::FinishedState:
            return true;
        default:
            return false;
        }
    }

    return false; // unable to wait for operation; not in progress or no engine.
}

#include "moc_qcontactabstractrequest.cpp"

QTM_END_NAMESPACE
