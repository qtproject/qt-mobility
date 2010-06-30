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

#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemabstractrequest_p.h"
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanager_p.h"
#include "qorganizeritemmanagerengine.h"


QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemAbstractRequest

  \brief The QOrganizerItemAbstractRequest class provides a mechanism for
  asynchronous requests to be made of a manager if it supports them.

  \ingroup organizeritems-main

  It allows a client to asynchronously request some functionality of a
  particular QOrganizerItemManager.  Instances of the class will emit signals
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
  \fn QOrganizerItemAbstractRequest::stateChanged(QOrganizerItemAbstractRequest::State newState)
  This signal is emitted when the state of the request is changed.  The new state of
  the request will be contained in \a newState.
 */


/*!
  \fn QOrganizerItemAbstractRequest::resultsAvailable()
  This signal is emitted when new results are available.  Results can include
  the operation error which may be accessed via error(), or derived-class-specific
  results which are accessible through the derived class API.

  \sa error()
 */

/*!
  \enum QOrganizerItemAbstractRequest::RequestType
  Enumerates the various possible types of asynchronous requests
  \value InvalidRequest An invalid request
  \value OrganizerItemInstanceFetchRequest A request to fetch a list of instances of organizeritems
  \value OrganizerItemFetchRequest A request to fetch a list of organizeritems
  \value OrganizerItemLocalIdFetchRequest A request to fetch a list of local organizeritem ids
  \value OrganizerItemRemoveRequest A request to remove a list of organizeritems
  \value OrganizerItemSaveRequest A request to save a list of organizeritems
  \value DetailDefinitionFetchRequest A request to fetch a collection of detail definitions
  \value DetailDefinitionRemoveRequest A request to remove a list of detail definitions
  \value DetailDefinitionSaveRequest A request to save a list of detail definitions
 */

/*!
  \enum QOrganizerItemAbstractRequest::State
  Enumerates the various states that a request may be in at any given time
  \value InactiveState Operation not yet started
  \value ActiveState Operation started, not yet finished
  \value CanceledState Operation is finished due to cancellation
  \value FinishedState Operation successfully completed
 */

/*!
  \fn QOrganizerItemAbstractRequest::QOrganizerItemAbstractRequest(QObject* parent)
  Constructs a new, invalid asynchronous request with the specified \a parent
 */

/*!
  \internal
  Constructs a new request from the given request data \a otherd with
  the given parent \a parent
*/
QOrganizerItemAbstractRequest::QOrganizerItemAbstractRequest(QOrganizerItemAbstractRequestPrivate* otherd, QObject* parent)
    : QObject(parent), d_ptr(otherd)
{
}

/*! Cleans up the memory used by this request */
QOrganizerItemAbstractRequest::~QOrganizerItemAbstractRequest()
{
    if (d_ptr) {
        QOrganizerItemManagerEngine *engine = QOrganizerItemManagerData::engine(d_ptr->m_manager);
        if (engine) {
            engine->requestDestroyed(this);
        }

        delete d_ptr;
    }
}

/*!
  Returns true if the request is in the \c QOrganizerItemAbstractRequest::InactiveState state; otherwise, returns false

  \sa state()
 */
bool QOrganizerItemAbstractRequest::isInactive() const
{
    return (d_ptr->m_state == QOrganizerItemAbstractRequest::InactiveState);
}

/*!
  Returns true if the request is in the \c QOrganizerItemAbstractRequest::ActiveState state; otherwise, returns false

  \sa state()
 */
bool QOrganizerItemAbstractRequest::isActive() const
{
    return (d_ptr->m_state == QOrganizerItemAbstractRequest::ActiveState);
}

/*!
  Returns true if the request is in the \c QOrganizerItemAbstractRequest::FinishedState; otherwise, returns false

  \sa state()
 */
bool QOrganizerItemAbstractRequest::isFinished() const
{
    return (d_ptr->m_state == QOrganizerItemAbstractRequest::FinishedState);
}

/*!
  Returns true if the request is in the \c QOrganizerItemAbstractRequest::CanceledState; otherwise, returns false

  \sa state()
 */
bool QOrganizerItemAbstractRequest::isCanceled() const
{
    return (d_ptr->m_state == QOrganizerItemAbstractRequest::CanceledState);
}

/*! Returns the overall error of the most recent asynchronous operation */
QOrganizerItemManager::Error QOrganizerItemAbstractRequest::error() const
{
    return d_ptr->m_error;
}

/*!
  Returns the type of this asynchronous request
 */
QOrganizerItemAbstractRequest::RequestType QOrganizerItemAbstractRequest::type() const
{
    return d_ptr->type();
}

/*!
  Returns the current state of the request.
 */
QOrganizerItemAbstractRequest::State QOrganizerItemAbstractRequest::state() const
{
    return d_ptr->m_state;
}

/*! Returns a pointer to the manager of which this request instance requests operations */
QOrganizerItemManager* QOrganizerItemAbstractRequest::manager() const
{
    return d_ptr->m_manager;
}

/*! Sets the manager of which this request instance requests operations to \a manager */
void QOrganizerItemAbstractRequest::setManager(QOrganizerItemManager* manager)
{
    d_ptr->m_manager = manager;
}

/*! Attempts to start the request.  Returns false if the request is not in the \c QOrganizerItemAbstractRequest::Inactive, \c QOrganizerItemAbstractRequest::Finished or \c QOrganizerItemAbstractRequest::Cancelled states,
    or if the request was unable to be performed by the manager engine; otherwise returns true. */
bool QOrganizerItemAbstractRequest::start()
{
    QOrganizerItemManagerEngine *engine = QOrganizerItemManagerData::engine(d_ptr->m_manager);
    if (engine && (d_ptr->m_state == QOrganizerItemAbstractRequest::CanceledState
                   || d_ptr->m_state == QOrganizerItemAbstractRequest::FinishedState
                   || d_ptr->m_state == QOrganizerItemAbstractRequest::InactiveState)) {
        return engine->startRequest(this);
    }

    return false; // unable to start operation; another operation already in progress or no engine.
}

/*! Attempts to cancel the request.  Returns false if the request is not in the \c QOrganizerItemAbstractRequest::Active state,
    or if the request is unable to be cancelled by the manager engine; otherwise returns true. */
bool QOrganizerItemAbstractRequest::cancel()
{
    QOrganizerItemManagerEngine *engine = QOrganizerItemManagerData::engine(d_ptr->m_manager);
    if (engine && state() == QOrganizerItemAbstractRequest::ActiveState) {
        return engine->cancelRequest(this);
    }

    return false; // unable to cancel operation; not in progress or no engine.
}

/*! Blocks until the request has been completed by the manager engine, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero, this function will block indefinitely.
    Returns true if the request was cancelled or completed successfully within the given period, otherwise false.
    Some backends are unable to support this operation safely, and will return false immediately.
 */
bool QOrganizerItemAbstractRequest::waitForFinished(int msecs)
{
    QOrganizerItemManagerEngine *engine = QOrganizerItemManagerData::engine(d_ptr->m_manager);
    if (engine) {
        switch (d_ptr->m_state) {
        case QOrganizerItemAbstractRequest::ActiveState:
            return engine->waitForRequestFinished(this, msecs);
        case QOrganizerItemAbstractRequest::CanceledState:
        case QOrganizerItemAbstractRequest::FinishedState:
            return true;
        default:
            return false;
        }
    }

    return false; // unable to wait for operation; not in progress or no engine.
}

#include "moc_qorganizeritemabstractrequest.cpp"

QTM_END_NAMESPACE

