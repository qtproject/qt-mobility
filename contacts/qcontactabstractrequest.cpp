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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactabstractrequest.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"

/*!
 * \class QContactAbstractRequest
 * \brief Allows a client to asynchronously request some functionality of a particular QContactManager.
 *
 * This class provides a mechanism for asynchronous requests to be made of a manager if it supports them.
 */

/*!
 * \enum QContactAbstractRequest::Type
 * Enumerates the various possible types of asynchronous requests
 * \value Invalid An invalid request
 * \value ContactFetch A request to fetch a list of contacts
 * \value ContactIdFetch A request to fetch a list of contact ids
 * \value ContactRemove A request to remove a list of contacts
 * \value ContactSave A request to save a list of contacts
 * \value DetailDefinitionFetch A request to fetch a collection of detail definitions
 * \value DetailDefinitionRemove A request to remove a list of detail definitions
 * \value DetailDefinitionSave A request to save a list of detail definitions
 * \value GroupFetch A request to fetch a list of groups
 * \value GroupRemove A request to remove a list of groups
 * \value GroupSave A request to save a list of groups
 */

/*!
 * \enum QContactAbstractRequest::Status
 * Enumerates the various states that a request may be in at any given time
 * \value Inactive Operation not yet started
 * \value Active Operation started, not yet finished
 * \value Cancelling Operation started then cancelled, not yet finished
 * \value Cancelled Operation is finished due to cancellation
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
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        engine->requestDestroyed(this);
}

/*!
 * Returns true if the request is pending, processing or cancelling; otherwise, returns false.
 *
 * \sa status()
 */
bool QContactAbstractRequest::isActive() const
{
    return (d_ptr->m_status == QContactAbstractRequest::Active
            || d_ptr->m_status == QContactAbstractRequest::Cancelling);
}

/*!
 * Returns true if the request is finished or cancelled; otherwise, returns false.
 *
 * \sa status()
 */
bool QContactAbstractRequest::isFinished() const
{
    return (d_ptr->m_status == QContactAbstractRequest::Finished
            || d_ptr->m_status == QContactAbstractRequest::Cancelled);
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
QContactAbstractRequest::Type QContactAbstractRequest::type() const
{
    return d_ptr->type();
}

/*!
 * Returns the current status of the request.
 *
 * \sa isFinished(), isActive()
 */
QContactAbstractRequest::Status QContactAbstractRequest::status() const
{
    return d_ptr->m_status;
}

/*! Returns a pointer to the manager which this request performs operations on */
QContactManager* QContactAbstractRequest::manager() const
{
    return d_ptr->m_manager;
}

/*! Sets the manager which this request performs operations on to \a manager */
void QContactAbstractRequest::setManager(QContactManager* manager)
{
    d_ptr->m_manager = manager;
}

/*! Attempts to start the request.  Returns false if the request is not in the \c QContactAbstractRequest::Inactive, \c QContactAbstractRequest::Finished or \c QContactAbstractRequest::Cancelled states,
    or if the request was unable to be performed by the manager engine; otherwise returns true. */
bool QContactAbstractRequest::start()
{
    if (status() != QContactAbstractRequest::Inactive
        && status() != QContactAbstractRequest::Cancelled
        && status() != QContactAbstractRequest::Finished) {
        return false; // unable to start operation; another operation already in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        return engine->startRequest(this);

    return false;
}

/*! Attempts to cancel the request.  Returns false if the request is not in the \c QContactAbstractRequest::Active state,
    or if the request is unable to be cancelled by the manager engine; otherwise returns true. */
bool QContactAbstractRequest::cancel()
{
    if (status() != QContactAbstractRequest::Active) {
        return false; // unable to cancel operation; operation not in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        return engine->cancelRequest(this);

    return false;
}

/*! Blocks until the request has been completed by the manager engine, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero, this function will block indefinitely.
    Returns true if the request was cancelled or completed successfully within the given period, otherwise false. */
bool QContactAbstractRequest::waitForFinished(int msecs)
{
    if (!isActive()) {
        return false; // unable to wait for operation; not in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        return engine->waitForRequestFinished(this, msecs);
    return false;
}

/*! Blocks until the manager engine signals that more partial results are available for the request, or until \a msecs milliseconds has elapsed.
    If \a msecs is zero, this function will block indefinitely.
    Returns true if the request was cancelled or completed successfully within the given period, otherwise false. */
bool QContactAbstractRequest::waitForProgress(int msecs)
{
    if (!isActive()) {
        return false; // unable to wait for operation; not in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        return engine->waitForRequestProgress(this, msecs);
    return false;
}
