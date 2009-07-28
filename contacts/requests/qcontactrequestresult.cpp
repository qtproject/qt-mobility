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


#include "qcontactrequestresult.h"
#include "qcontactrequestresult_p.h"
#include "qcontactrequest.h"
#include "qcontactrequest_p.h"

/*!
 * \class QContactRequestResult
 * \brief The current result of an asynchronous request for contacts
 * This class allows the result of an asynchronous request for contacts to be returned
 * to the request.  It provides an interface to manager engines which allows them
 * to set the result and status of a particular request.
 */

/*! Cosntructs a new contact request result */
QContactRequestResult::QContactRequestResult()
    : d(new QContactRequestResultData)
{
}

/*! Clears any memory in use by this contact request result */
QContactRequestResult::~QContactRequestResult()
{
}

/*! Sets the result contact ids to \a ids */
void QContactRequestResult::setContactIds(const QList<QUniqueId>& ids)
{
    d->m_contactIds = ids;
}

/*! Sets the result contacts to \a contacts */
void QContactRequestResult::setContacts(const QList<QContact>& contacts)
{
    d->m_contacts = contacts;
}

/*! Returns the result list of contact ids */
QList<QUniqueId> QContactRequestResult::contactIds() const
{
    return d->m_contactIds;
}

/*! Returns the result list of contacts */
QList<QContact> QContactRequestResult::contacts() const
{
    return d->m_contacts;
}

/*! Sets the result of the given \a request to be this result and updates the status of the \a request to \a status, before emitting the progress signal */
void QContactRequestResult::updateRequest(QContactRequest* request, QContactAbstractRequest::Status status)
{
    request->d->m_status = status;
    request->d->m_result = this;
    emit request->progress(request, (status == QContactAbstractRequest::Finished || status == QContactAbstractRequest::Cancelled), appendOnly());
}
