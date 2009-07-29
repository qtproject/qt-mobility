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

QContactRequest::QContactRequest()
    : d(new QContactRequestData)
{
}

QContactRequest::~QContactRequest()
{
    // tell the engine that we're being destroyed, then remove ourselves from the
    // result's list of applicable requests (if the engine doesn't do this for us).
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->asynchronousRequestDestroyed(this);
    if (d->m_result)
        d->m_result->removeRequest(this);
    delete d;
}

/*!
 * Limits the selection criteria of the request to contacts whose id is contained in \a ids
 */
void QContactRequest::selectById(const QList<QUniqueId>& contactIds)
{
    d->m_requestIds = contactIds;
    d->m_requestFilter = QContactFilter();
    d->m_requestObjects.clear();
}

/*!
 * Limits the selection criteria of the contact request to contacts contained in \a contacts
 */
void QContactRequest::selectByObject(const QList<QContact>& contacts)
{
    d->m_requestIds.clear();
    d->m_requestFilter = QContactFilter();
    d->m_requestObjects = contacts;
}

/*!
 * Limits the selection criteria of the contact request to contacts which match the given \a filter
 */
void QContactRequest::selectByFilter(const QContactFilter& filter)
{
    d->m_requestIds.clear();
    d->m_requestFilter = filter;
    d->m_requestObjects.clear();
}

/*!
 * Sets the sort order of the result of the next asynchronous operation to \a sortOrder.
 *
 * Calling this function has no effect on asynchronous operations which have already been started.
 */
void QContactRequest::setSortOrder(const QContactSortOrder& order)
{
    d->m_sortorder = order;
}

/*!
 * Returns the sort order which will be used for the next asynchronous operation of \a request
 */
QContactSortOrder QContactRequest::sortOrder() const
{
    return d->m_sortorder;
}


/*!
 * Clears any result restrictions which have been set for the next asynchronous operation of \a request
 */
void QContactRequest::clearRestrictions()
{
    d->m_isRestrictedDefs = false;
    d->m_isRestrictedIds = false;
    d->m_restrictDefinitions.clear();
}

/*!
 * Restricts the result of the next asynchronous operation of \a request to the ids of the contacts which would otherwise be returned
 */
void QContactRequest::restrictToIds()
{
    d->m_isRestrictedIds = true;
    d->m_isRestrictedDefs = false;
    d->m_restrictDefinitions.clear();
}

/*!
 * Restricts the result of the next asynchronous operation of \a request to those details whose definition name is included in the given list of \a detailDefinitionNames.
 */
void QContactRequest::restrictToDetails(const QStringList& detailDefinitionIds)
{
    d->m_isRestrictedIds = false;
    d->m_isRestrictedDefs = true;
    d->m_restrictDefinitions = detailDefinitionIds;
}

/*!
 * Returns true if the state of the given \a request is either QContactAbstractRequest::Finished
 * or QContactAbstractRequest::Cancelled.  Also returns true if the QContactManager supplied to
 * this request has been deleted.  Otherwise, returns false;
 */
bool QContactRequest::isFinished() const
{
    if (!d->m_manager)
        return true;
    return d->m_status == QContactAbstractRequest::Finished || d->m_status == QContactAbstractRequest::Cancelled;
}

/*!
 * Returns the error associated with the most recent operation performed as part of the given asynchronous \a request
 */
QContactManager::Error QContactRequest::error() const
{
    if (!d->m_result)
        return QContactManager::NoError; // no operation started.
    return d->m_error;
}

/*!
 * Returns the current status of the given \a request
 */
QContactAbstractRequest::Status QContactRequest::status() const
{
    if (!d->m_manager)
        return QContactAbstractRequest::Inactive;
    return d->m_status;
}


void QContactRequest::clearSelection()
{
    d->m_requestFilter = QContactFilter();
    d->m_requestIds.clear();
    d->m_requestObjects.clear();
}

QContactRequest::SelectionType QContactRequest::selectionType() const
{
    if (d->m_requestIds.size() > 0)
        return QContactRequest::SelectByIds;
    if (d->m_requestObjects.size() > 0)
        return QContactRequest::SelectByObject;
    if (d->m_requestFilter.type() != QContactFilter::Invalid)
        return QContactRequest::SelectByFilter;
    return QContactRequest::SelectAll;
}

QList<QUniqueId> QContactRequest::idSelection() const
{
    return d->m_requestIds;
}

QList<QContact> QContactRequest::contactSelection() const
{
    return d->m_requestObjects;
}

QContactFilter QContactRequest::filterSelection() const
{
    return d->m_requestFilter;
}

QContactRequest::Restriction QContactRequest::restriction()
{
    if (d->m_isRestrictedDefs)
        return QContactRequest::RestrictToDetails;
    if (d->m_isRestrictedIds)
        return QContactRequest::RestrictToIds;
    return QContactRequest::NoRestriction;
}

QStringList QContactRequest::detailRestrictions() const
{
    return d->m_restrictDefinitions;
}


/* =============== Dynamic functions below (trampoline to the engine) */

bool QContactRequest::waitForFinished(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForFinished(this, msecs);
    return false;
}

bool QContactRequest::waitForProgress(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForProgress(this, msecs);
    return false;
}

void QContactRequest::cancel()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->cancelAsynchronousRequest(this);
}

void QContactRequest::start(QContactManager *manager, QContactAbstractRequest::Operation operation)
{
    if (status() != QContactAbstractRequest::Inactive
        && status() != QContactAbstractRequest::Cancelled
        && status() != QContactAbstractRequest::Finished) {
        return; // unable to start operation; another operation already in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->asynchronousRequestDestroyed(this);

    d->m_manager = manager;
    engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->startAsynchronousRequest(this, operation);
}

QList<QUniqueId> QContactRequest::ids()
{
    if (!d->m_result)
        return QList<QUniqueId>();

    return d->m_result->contactIds();
}

QList<QContact> QContactRequest::contacts()
{
    if (!d->m_result)
        return QList<QContact>();

    return d->m_result->contacts();
}

QList<QContactManager::Error> QContactRequest::errors()
{
    if (!d->m_result)
        return QList<QContactManager::Error>();

    return d->m_result->errors();
}

#include "moc_qcontactrequest.cpp"
