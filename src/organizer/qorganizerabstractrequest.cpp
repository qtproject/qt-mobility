/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizerabstractrequest.h"
#include "qorganizerabstractrequest_p.h"
#include "qorganizermanager.h"
#include "qorganizermanager_p.h"
#include "qorganizermanagerengine.h"


QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerAbstractRequest

  \brief The QOrganizerAbstractRequest class provides a mechanism for
  asynchronous requests to be made of a manager if it supports them.

  \inmodule QtOrganizer

  \ingroup organizer-main

  It allows a client to asynchronously request some functionality of a
  particular QOrganizerManager.  Instances of the class will emit signals
  when the state of the request changes, or when more results become
  available.

  Clients should not attempt to create instances of this class directly,
  but should instead use the use-case-specific classes derived from this
  class.

  After creating any sort of request, the client retains ownership and
  must delete the request to avoid leaking memory.  The client may either
  do this directly (if not within a slot connected to a signal emitted
  by the request) or by using the deleteLater() slot to schedule the
  request for deletion when control returns to the event loop.
 */

/*!
  \fn QOrganizerAbstractRequest::stateChanged(QOrganizerAbstractRequest::State newState)
  This signal is emitted when the state of the request is changed.  The new state of
  the request will be contained in \a newState.
 */


/*!
  \fn QOrganizerAbstractRequest::resultsAvailable()
  This signal is emitted when new results are available.  Results can include
  the operation error which may be accessed via error(), or derived-class-specific
  results which are accessible through the derived class API.

  \sa error()
 */

/*!
  \enum QOrganizerAbstractRequest::RequestType
  Enumerates the various possible types of asynchronous requests
  \value InvalidRequest An invalid request
  \value ItemOccurrenceFetchRequest A request to fetch a list of occurrences of an organizer item
  \value ItemFetchRequest A request to fetch a list of organizer items
  \value ItemFetchForExportRequest A request to fetch a list of persisted organizer items and exceptions
  \value ItemLocalIdFetchRequest A request to fetch a list of local organizer item ids
  \value ItemRemoveRequest A request to remove a list of organizer items
  \value ItemSaveRequest A request to save a list of organizer items
  \value DetailDefinitionFetchRequest A request to fetch a collection of detail definitions
  \value DetailDefinitionRemoveRequest A request to remove a list of detail definitions
  \value DetailDefinitionSaveRequest A request to save a list of detail definitions
  \value CollectionFetchRequest A request to fetch a collection.
  \value CollectionRemoveRequest A request to remove a collection.
  \value CollectionSaveRequest A request to save a collection.
 */

/*!
  \enum QOrganizerAbstractRequest::State
  Enumerates the various states that a request may be in at any given time
  \value InactiveState Operation not yet started
  \value ActiveState Operation started, not yet finished
  \value CanceledState Operation is finished due to cancellation
  \value FinishedState Operation successfully completed
 */

/*!
  \fn QOrganizerAbstractRequest::QOrganizerAbstractRequest(QObject* parent)
  Constructs a new, invalid asynchronous request with the specified \a parent
 */

/*!
  \internal
  Constructs a new request from the given request data \a otherd with
  the given parent \a parent
*/
QOrganizerAbstractRequest::QOrganizerAbstractRequest(QOrganizerAbstractRequestPrivate* otherd, QObject* parent)
    : QObject(parent), d_ptr(otherd)
{
}

/*! Cleans up the memory used by this request */
QOrganizerAbstractRequest::~QOrganizerAbstractRequest()
{
    if (d_ptr) {
        QOrganizerManagerEngine *engine = QOrganizerManagerData::engine(d_ptr->m_manager);
        if (engine) {
            engine->requestDestroyed(this);
        }

        delete d_ptr;
    }
}

/*!
  Returns true if the request is in the \c QOrganizerAbstractRequest::InactiveState state; otherwise, returns false

  \sa state()
 */
bool QOrganizerAbstractRequest::isInactive() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QOrganizerAbstractRequest::InactiveState);
}

/*!
  Returns true if the request is in the \c QOrganizerAbstractRequest::ActiveState state; otherwise, returns false

  \sa state()
 */
bool QOrganizerAbstractRequest::isActive() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QOrganizerAbstractRequest::ActiveState);
}

/*!
  Returns true if the request is in the \c QOrganizerAbstractRequest::FinishedState; otherwise, returns false

  \sa state()
 */
bool QOrganizerAbstractRequest::isFinished() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QOrganizerAbstractRequest::FinishedState);
}

/*!
  Returns true if the request is in the \c QOrganizerAbstractRequest::CanceledState; otherwise, returns false

  \sa state()
 */
bool QOrganizerAbstractRequest::isCanceled() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return (d_ptr->m_state == QOrganizerAbstractRequest::CanceledState);
}

/*! Returns the overall error of the most recent asynchronous operation */
QOrganizerManager::Error QOrganizerAbstractRequest::error() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->m_error;
}

/*!
  Returns the type of this asynchronous request
 */
QOrganizerAbstractRequest::RequestType QOrganizerAbstractRequest::type() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->type();
}

/*!
  Returns the current state of the request.
 */
QOrganizerAbstractRequest::State QOrganizerAbstractRequest::state() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->m_state;
}

/*! Returns a pointer to the manager of which this request instance requests operations */
QOrganizerManager* QOrganizerAbstractRequest::manager() const
{
    QMutexLocker ml(&d_ptr->m_mutex);
    return d_ptr->m_manager;
}

/*! Sets the manager of which this request instance requests operations to \a manager */
void QOrganizerAbstractRequest::setManager(QOrganizerManager* manager)
{
    QMutexLocker ml(&d_ptr->m_mutex);
    // In theory we might have been active and the manager didn't cancel/finish us
    if (d_ptr->m_state == QOrganizerAbstractRequest::ActiveState && d_ptr->m_manager)
        return;
    d_ptr->m_manager = manager;
    d_ptr->m_engine = QOrganizerManagerData::engine(d_ptr->m_manager);
}

/*! Attempts to start the request.  Returns false if the request is not in the \c QOrganizerAbstractRequest::Inactive, \c QOrganizerAbstractRequest::Finished or \c QOrganizerAbstractRequest::Cancelled states,
    or if the request was unable to be performed by the manager engine; otherwise returns true. */
bool QOrganizerAbstractRequest::start()
{
    QMutexLocker ml(&d_ptr->m_mutex);
    if (d_ptr->m_engine && (d_ptr->m_state == QOrganizerAbstractRequest::CanceledState
                   || d_ptr->m_state == QOrganizerAbstractRequest::FinishedState
                   || d_ptr->m_state == QOrganizerAbstractRequest::InactiveState)) {
        ml.unlock();
        return d_ptr->m_engine->startRequest(this);
    }

    return false; // unable to start operation; another operation already in progress or no engine.
}

/*! Attempts to cancel the request.  Returns false if the request is not in the \c QOrganizerAbstractRequest::Active state,
    or if the request is unable to be cancelled by the manager engine; otherwise returns true. */
bool QOrganizerAbstractRequest::cancel()
{
    QMutexLocker ml(&d_ptr->m_mutex);
    if (d_ptr->m_engine && d_ptr->m_state == QOrganizerAbstractRequest::ActiveState) {
        ml.unlock();
        return d_ptr->m_engine->cancelRequest(this);
    }

    return false; // unable to cancel operation; not in progress or no engine.
}

/*! Blocks until the request has been completed by the manager engine, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero or negative, this function will block until the request is complete, regardless of how long it takes.
    Returns true if the request was cancelled or completed successfully within the given period, otherwise false.
    Some backends are unable to support this operation safely, and will return false immediately.
 */
bool QOrganizerAbstractRequest::waitForFinished(int msecs)
{
    QMutexLocker ml(&d_ptr->m_mutex);
    if (d_ptr->m_engine) {
        switch (d_ptr->m_state) {
        case QOrganizerAbstractRequest::ActiveState:
            ml.unlock();
            return d_ptr->m_engine->waitForRequestFinished(this, msecs);
        case QOrganizerAbstractRequest::CanceledState:
        case QOrganizerAbstractRequest::FinishedState:
            return true;
        default:
            return false;
        }
    }

    return false; // unable to wait for operation; not in progress or no engine
}

#include "moc_qorganizerabstractrequest.cpp"

QTM_END_NAMESPACE

