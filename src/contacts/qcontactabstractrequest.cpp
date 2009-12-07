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

#include "qcontactabstractrequest.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"


QTM_BEGIN_NAMESPACE
/*!
  \class QContactAbstractRequest

  \brief The QContactAbstractRequest class provides a mechanism for
  asynchronous requests to be made of a manager if it supports them.

  It allows a client to asynchronously request some functionality of a
  particular QContactManager.
 */

/*!
 * \enum QContactAbstractRequest::RequestType
 * Enumerates the various possible types of asynchronous requests
 * \value InvalidRequest An invalid request
 * \value ContactFetchRequest A request to fetch a list of contacts
 * \value ContactLocalIdFetchRequest A request to fetch a list of local contact ids
 * \value ContactRemoveRequest A request to remove a list of contacts
 * \value ContactSaveRequest A request to save a list of contacts
 * \value DetailDefinitionFetchRequest A request to fetch a collection of detail definitions
 * \value DetailDefinitionRemoveRequest A request to remove a list of detail definitions
 * \value DetailDefinitionSaveRequest A request to save a list of detail definitions
 * \value RelationshipFetchRequest A request to fetch relationships between contacts
 * \value RelationshipRemoveRequest A request to remove any relationships which match the request criteria
 * \value RelationshipSaveRequest A request to save a list of relationships
 */

/*!
 * \enum QContactAbstractRequest::State
 * Enumerates the various states that a request may be in at any given time
 * \value Inactive Operation not yet started
 * \value Active Operation started, not yet finished
 * \value Canceling Operation started then cancelled, not yet finished
 * \value Canceled Operation is finished due to cancellation
 * \value Finished Operation successfully completed
 */

/*!
 * \fn QContactAbstractRequest::QContactAbstractRequest()
 * Constructs a new, invalid asynchronous request
 */

/*! Constructs a new request from the given request data \a otherd */
QContactAbstractRequest::QContactAbstractRequest(QContactAbstractRequestPrivate* otherd)
    : d_ptr(otherd)
{
}

/*! Cleans up the memory used by this request */
QContactAbstractRequest::~QContactAbstractRequest()
{
    if (d_ptr) {
        QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
        if (engine) {
            engine->requestDestroyed(this);
        }

        delete d_ptr;
    }
}

/*!
 * \deprecated
 *
 * Returns true if the request is pending, processing or cancelling; otherwise, returns false.
 * This function is deprecated and will be removed in week 1.
 *
 * \sa state()
 */
bool QContactAbstractRequest::isActive() const
{
    qWarning("This function is deprecated and will be removed in week 1!");
    return (d_ptr->m_state == QContactAbstractRequest::Active
            || d_ptr->m_state == QContactAbstractRequest::Canceling);
}

/*!
 * \deprecated
 *
 * Returns true if the request is finished or cancelled; otherwise, returns false.
 * This function is deprecated and will be removed in week 1.
 *
 * \sa state()
 */
bool QContactAbstractRequest::isFinished() const
{
    qWarning("This function is deprecated and will be removed in week 1!");
    return (d_ptr->m_state == QContactAbstractRequest::Finished
            || d_ptr->m_state == QContactAbstractRequest::Canceled);
}

/*! Returns the overall error of the most recent asynchronous operation */
QContactManager::Error QContactAbstractRequest::error() const
{
    return d_ptr->m_error;
}

/*! Returns the list of errors which occurred during the most recent asynchronous operation.  Each individual error in the list corresponds to a result in the result list. */
QList<QContactManager::Error> QContactAbstractRequest::errors() const
{
    return d_ptr->m_errors;
}

/*!
 * Returns the type of this asynchronous request
 */
QContactAbstractRequest::RequestType QContactAbstractRequest::type() const
{
    return d_ptr->type();
}

/*!
 * Returns the current state of the request.
 */
QContactAbstractRequest::State QContactAbstractRequest::state() const
{
    return d_ptr->m_state;
}

/*! Returns a pointer to the manager of which this request instance requests operations */
QContactManager* QContactAbstractRequest::manager() const
{
    return d_ptr->m_manager;
}

/*! Sets the manager of which this request instance requests operations to \a manager */
void QContactAbstractRequest::setManager(QContactManager* manager)
{
    d_ptr->m_manager = manager;
}

/*! Attempts to start the request.  Returns false if the request is not in the \c QContactAbstractRequest::Inactive, \c QContactAbstractRequest::Finished or \c QContactAbstractRequest::Cancelled states,
    or if the request was unable to be performed by the manager engine; otherwise returns true. */
bool QContactAbstractRequest::start()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine && !isActive()) {
        return engine->startRequest(this);
    }

    return false; // unable to start operation; another operation already in progress or no engine.
}

/*! Attempts to cancel the request.  Returns false if the request is not in the \c QContactAbstractRequest::Active state,
    or if the request is unable to be cancelled by the manager engine; otherwise returns true. */
bool QContactAbstractRequest::cancel()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine && state() == QContactAbstractRequest::Active) {
        return engine->cancelRequest(this);
    }

    return false; // unable to cancel operation; not in progress or no engine.
}

/*! Blocks until the request has been completed by the manager engine, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero, this function will block indefinitely.
    Returns true if the request was cancelled or completed successfully within the given period, otherwise false. */
bool QContactAbstractRequest::waitForFinished(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine && isActive()) {
        return engine->waitForRequestFinished(this, msecs);
    }

    return false; // unable to wait for operation; not in progress or no engine.
}

/*! Blocks until the manager engine signals that more partial results are available for the request, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero, this function will block indefinitely.
    Returns true if the request was cancelled or more partial results were made available within the given period, otherwise false. */
bool QContactAbstractRequest::waitForProgress(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine && isActive()) {
        return engine->waitForRequestProgress(this, msecs);
    }

    return false; // unable to wait for operation; not in progress or no engine.
}

#include "moc_qcontactabstractrequest.cpp"

QTM_END_NAMESPACE
