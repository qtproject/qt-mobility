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

#include "qcontactgrouprequest.h"
#include "qcontactgrouprequest_p.h"

#include "qcontactmanager_p.h"

QContactGroupRequest::QContactGroupRequest()
    : d(new QContactGroupRequestData)
{
}

QContactGroupRequest::~QContactGroupRequest()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->asynchronousRequestDestroyed(this);
    delete d;
}

/*!
 * Limits the selection criteria of the request to contacts whose id is contained in \a ids
 */
void QContactGroupRequest::selectById(const QList<QUniqueId>& groupIds)
{
    d->m_requestIds = groupIds;
    d->m_requestObjects.clear();
}

/*!
 * Limits the selection criteria of the contact request to contact groups contained in \a groups
 */
void QContactGroupRequest::selectByObject(const QList<QContactGroup>& groups)
{
    d->m_requestIds.clear();
    d->m_requestObjects = groups;
}

/*!
 * Clears any result restrictions which have been set for the next asynchronous operation of the request
 */
void QContactGroupRequest::clearRestrictions()
{
    d->m_isRestrictedIds = false;
}

/*!
 * Restricts the result of the next asynchronous operation of the request to the ids of the groups which would otherwise be returned
 */
void QContactGroupRequest::restrictToIds()
{
    d->m_isRestrictedIds = true;
}

/*!
 * Returns true if the state of the request is either QContactAbstractRequest::Finished
 * or QContactAbstractRequest::Cancelled.  Also returns true if the QContactManager supplied to
 * this request has been deleted.  Otherwise, returns false;
 */
bool QContactGroupRequest::isFinished() const
{
    if (!d->m_manager)
        return true;
    return d->m_status == QContactAbstractRequest::Finished || d->m_status == QContactAbstractRequest::Cancelled;
}

/*!
 * Returns the error associated with the most recent operation performed as part of the asynchronous request
 */
QContactManager::Error QContactGroupRequest::error() const
{
    if (!d->m_manager)
        return QContactManager::NoError;
    return d->m_error;
}

/*!
 * Returns the current status of the request
 */
QContactAbstractRequest::Status QContactGroupRequest::status() const
{
    if (!d->m_manager)
        return QContactAbstractRequest::Inactive;
    return d->m_status;
}


void QContactGroupRequest::clearSelection()
{
    d->m_requestIds.clear();
    d->m_requestObjects.clear();
}

QContactGroupRequest::SelectionType QContactGroupRequest::selectionType() const
{
    if (d->m_requestIds.size() > 0)
        return QContactGroupRequest::SelectByIds;
    if (d->m_requestObjects.size() > 0)
        return QContactGroupRequest::SelectByObject;
    return QContactGroupRequest::SelectAll;
}

QList<QUniqueId> QContactGroupRequest::idSelection() const
{
    return d->m_requestIds;
}

QList<QContactGroup> QContactGroupRequest::groupSelection() const
{
    return d->m_requestObjects;
}

QContactGroupRequest::Restriction QContactGroupRequest::restriction()
{
    if (d->m_isRestrictedIds)
        return QContactGroupRequest::RestrictToIds;
    return QContactGroupRequest::NoRestriction;
}


/* =============== Dynamic functions below (trampoline to the engine) */

bool QContactGroupRequest::waitForFinished(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForFinished(this, msecs);
    return false;
}

bool QContactGroupRequest::waitForProgress(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForProgress(this, msecs);
    return false;
}

void QContactGroupRequest::cancel()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->cancelAsynchronousRequest(this);
}

void QContactGroupRequest::start(QContactManager *manager, QContactAbstractRequest::Operation operation)
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

QList<QUniqueId> QContactGroupRequest::ids()
{
    if (!d->m_result)
        return QList<QUniqueId>();

    return d->m_result->groupIds();
}

QList<QContactGroup> QContactGroupRequest::groups()
{
    if (!d->m_result)
        return QList<QContactGroup>();

    return d->m_result->groups();
}

QList<QContactManager::Error> QContactGroupRequest::errors()
{
    if (!d->m_result)
        return QList<QContactManager::Error>();

    return d->m_result->errors();
}

#include "moc_qcontactgrouprequest.cpp"
