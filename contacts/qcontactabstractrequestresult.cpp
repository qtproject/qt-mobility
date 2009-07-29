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

#include "qcontactabstractrequestresult.h"
#include "qcontactrequests.h"

#include "qcontactrequest_p.h"
#include "qcontactdetaildefinitionrequest_p.h"
#include "qcontactgrouprequest_p.h"

#include "qcontactabstractrequestresult_p.h"
#include "qcontactrequestresult_p.h"
#include "qcontactdetaildefinitionrequestresult_p.h"
#include "qcontactgrouprequestresult_p.h"

/*!
 * \class QContactAbstractRequestResult
 * \brief The result of an asynchronous request
 * This class should be extended for each particular request type implemented.
 */

/*! Constructs a new request result */
QContactAbstractRequestResult::QContactAbstractRequestResult()
    : d(new QContactAbstractRequestResultData)
{
}

/*! Clears the memory used by this result */
QContactAbstractRequestResult::~QContactAbstractRequestResult()
{
    // clean up - the requests which use us as their result must be notified of our destruction.
    QList<QContactAbstractRequest*> requests = d->m_requests.toList();
    for (int i = 0; i < requests.size(); i++) {
        removeRequest(requests.at(i));
    }
}

/*! Sets the list of errors associated with the result of the request to \a errors */
void QContactAbstractRequestResult::setErrors(const QList<QContactManager::Error>& errors)
{
    d->m_errors = errors;
}

/*! Sets the error associated with the operation of the request itself to \a error */
void QContactAbstractRequestResult::setError(const QContactManager::Error& error)
{
    d->m_error = error;
}

/*! Returns the list of errors associated with the result of the request */
QList<QContactManager::Error> QContactAbstractRequestResult::errors() const
{
    return d->m_errors;
}

/*! Returns the error associated with the operation of the request */
QContactManager::Error QContactAbstractRequestResult::error() const
{
    return d->m_error;
}

/*! Sets the status of the order of the results to \a appendOnly.  If \a appendOnly is true, the order of the results
 *  since the last update remains unchanged (i.e., new results have merely been appended). */
void QContactAbstractRequestResult::setAppendOnly(bool appendOnly)
{
    d->m_appendOnly = appendOnly;
}

/*! Retrieve the status of the order of the results.  Returns true if the order of the results has remained unchanged
 *  since the last udpate (i.e., new results have merely been appended to the old set). */
bool QContactAbstractRequestResult::appendOnly() const
{
    return d->m_appendOnly;
}

/*! Returns a list of pointers to requests for which this result is applicable */
QList<QContactAbstractRequest*> QContactAbstractRequestResult::requests() const
{
    return d->m_requests.toList();
}

/*! Adds the given request to the list of requests for which this result is applicable, and sets the result pointer of the
    given request to point to this result */
void QContactAbstractRequestResult::addRequest(QContactAbstractRequest* request)
{
    if (d->m_requests.contains(request))
        return;

    switch (request->type()) {
        case QContactAbstractRequest::Contact:
        {
            QContactRequest* cr = static_cast<QContactRequest*>(request);
            if (cr->d->m_result != 0)
                cr->d->m_result->removeRequest(request);
            cr->d->m_result = static_cast<QContactRequestResult*>(this);
        }
        break;

        case QContactAbstractRequest::DetailDefinition:
        {
            QContactDetailDefinitionRequest* dr = static_cast<QContactDetailDefinitionRequest*>(request);
            if (dr->d->m_result != 0)
                dr->d->m_result->removeRequest(request);
            dr->d->m_result = static_cast<QContactDetailDefinitionRequestResult*>(this);
        }
        break;

        case QContactAbstractRequest::Group:
        {
            QContactGroupRequest* gr = static_cast<QContactGroupRequest*>(request);
            if (gr->d->m_result != 0)
                gr->d->m_result->removeRequest(request);
            gr->d->m_result = static_cast<QContactGroupRequestResult*>(this);
        }
        break;

        default:
        return; // unknown request type.
    }

    // insert the request into our list of applicable requests.
    d->m_requests.insert(request);
}

/*! Removes the given request from the list of requests for which this result is applicable, and sets the result pointer of
    the given request to null. */
bool QContactAbstractRequestResult::removeRequest(QContactAbstractRequest* request)
{
    bool retn = true;
    switch (request->type()) {
        case QContactAbstractRequest::Contact:
        {
            QContactRequest* cr = static_cast<QContactRequest*>(request);
            cr->d->m_result = 0;
        }
        break;

        case QContactAbstractRequest::DetailDefinition:
        {
            QContactDetailDefinitionRequest* dr = static_cast<QContactDetailDefinitionRequest*>(request);
            dr->d->m_result = 0;
        }
        break;

        case QContactAbstractRequest::Group:
        {
            QContactGroupRequest* gr = static_cast<QContactGroupRequest*>(request);
            gr->d->m_result = 0;
        }
        break;

        default:
        {
            retn = false;
        }
        break;
    }

    bool remv = d->m_requests.remove(request);
    return (remv && retn);
}

/*! If \a request is one of the requests for which this result is applicable, this function updates the status of the \a request
    to \a status, before emitting the progress signal; otherwise has no effect. */
void QContactAbstractRequestResult::updateRequest(QContactAbstractRequest* request, QContactAbstractRequest::Status status)
{
    if (!d->m_requests.contains(request))
        return;

    switch (request->type()) {
        case QContactAbstractRequest::Contact:
        {
            QContactRequest* cr = static_cast<QContactRequest*>(request);
            cr->d->m_status = status;
            emit cr->progress(cr, (status == QContactAbstractRequest::Finished || status == QContactAbstractRequest::Cancelled), appendOnly());
        }
        break;

        case QContactAbstractRequest::DetailDefinition:
        {
            QContactDetailDefinitionRequest* dr = static_cast<QContactDetailDefinitionRequest*>(request);
            dr->d->m_status = status;
            emit dr->progress(dr, (status == QContactAbstractRequest::Finished || status == QContactAbstractRequest::Cancelled), appendOnly());
        }
        break;

        case QContactAbstractRequest::Group:
        {
            QContactGroupRequest* gr = static_cast<QContactGroupRequest*>(request);
            gr->d->m_status = status;
            emit gr->progress(gr, (status == QContactAbstractRequest::Finished || status == QContactAbstractRequest::Cancelled), appendOnly());
        }
        break;

        default:
        break;
    }
}

