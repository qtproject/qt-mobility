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

#include "qcontactrequest.h"
#include "qcontactrequest_p.h"

#include "qcontactmanager_p.h"

QContactRequest::QContactRequest(QContactManager* manager)
{
    d->m_manager = manager;
    QContactManagerData::engine(d->m_manager)->createAsynchronousRequest(this);
}

QContactRequest::~QContactRequest()
{
    QContactManagerData::engine(d->m_manager)->destroyAsynchronousRequest(this);
}

bool QContactRequest::isFinished() const
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestIsFinished(this);
}

QContactManager::Error QContactRequest::error() const
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestError(this);
}

QContactRequest::Status QContactRequest::status() const
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestStatus(this);
}

bool QContactRequest::waitForFinished(int msecs)
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestWaitForFinished(this, msecs);
}

bool QContactRequest::waitForProgress(int msecs)
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestWaitForProgress(this, msecs);
}

void QContactRequest::cancel()
{
    QContactManagerData::engine(d->m_manager)->cancelAsynchronousRequest(this);
}

void QContactRequest::start(QContactAbstractRequest::Operation operation)
{
    QContactManagerData::engine(d->m_manager)->startAsynchronousRequest(this, operation);
}

void QContactRequest::selectById(const QList<QUniqueId>& contactIds)
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestSelectById(this, contactIds);
}

void QContactRequest::selectByObject(const QList<QContact>& contacts)
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestSelectByObject(this, contacts);
}

void QContactRequest::selectByFilter(const QContactFilter& filter)
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestSelectByFilter(this, filter);
}

void QContactRequest::setSortOrder(const QContactSortOrder& order)
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestSetSortOrder(this, order);
}

QContactSortOrder QContactRequest::sortOrder() const
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestSortOrder(this);
}

void QContactRequest::clearRestrictions()
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestClearRestrictions(this);
}

void QContactRequest::restrictToIds()
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestRestrictToIds(this);
}

void QContactRequest::restrictToDetails(QStringList detailDefinitionIds)
{
    QContactManagerData::engine(d->m_manager)->asynchronousRequestRestrictToDetails(this, detailDefinitionIds);
}

QList<QUniqueId> QContactRequest::ids()
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestIds(this);
}

QList<QContact> QContactRequest::contacts()
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestContacts(this);
}

QList<QContactManager::Error> QContactRequest::errors()
{
    return QContactManagerData::engine(d->m_manager)->asynchronousRequestErrors(this);
}
