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

#include "qcontactdetaildefinitionrequest.h"
#include "qcontactdetaildefinitionrequest_p.h"

#include "qcontactmanager_p.h"

QContactDetailDefinitionRequest::QContactDetailDefinitionRequest(QContactManager* manager)
    : d(new QContactDetailDefinitionRequestData(manager))
{
}

QContactDetailDefinitionRequest::~QContactDetailDefinitionRequest()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->destroyAsynchronousRequest(this);
    delete d;
}

/*!
 * Limits the selection criteria of the request to contacts whose id is contained in \a ids
 */
void QContactDetailDefinitionRequest::selectByName(const QStringList& definitionNames)
{
    d->m_requestNames = definitionNames;
    d->m_requestObjects.clear();
}

/*!
 * Limits the selection criteria of the contact request to detail definitions contained in \a definitions
 */
void QContactDetailDefinitionRequest::selectByObject(const QList<QContactDetailDefinition>& definitions)
{
    d->m_requestNames.clear();
    d->m_requestObjects = definitions;
}

/*!
 * Clears any result restrictions which have been set for the next asynchronous operation of the request
 */
void QContactDetailDefinitionRequest::clearRestrictions()
{
    d->m_isRestrictedNames = false;
}

/*!
 * Restricts the result of the next asynchronous operation of the request to the names of the detail definitions which would otherwise be returned
 */
void QContactDetailDefinitionRequest::restrictToNames()
{
    d->m_isRestrictedNames = true;
}

/*!
 * Returns true if the state of the request is either QContactAbstractRequest::Finished
 * or QContactAbstractRequest::Cancelled.  Also returns true if the QContactManager supplied to
 * this request has been deleted.  Otherwise, returns false;
 */
bool QContactDetailDefinitionRequest::isFinished() const
{
    if (!d->m_manager)
        return true;
    return d->m_status == QContactAbstractRequest::Finished || d->m_status == QContactAbstractRequest::Cancelled;
}

/*!
 * Returns the error associated with the most recent operation performed as part of the asynchronous request
 */
QContactManager::Error QContactDetailDefinitionRequest::error() const
{
    if (!d->m_manager)
        return QContactManager::DoesNotExistError;
    return d->m_error;
}

/*!
 * Returns the current status of the request
 */
QContactAbstractRequest::Status QContactDetailDefinitionRequest::status() const
{
    if (!d->m_manager)
        return QContactAbstractRequest::Finished;
    return d->m_status;
}


void QContactDetailDefinitionRequest::clearSelection()
{
    d->m_requestNames.clear();
    d->m_requestObjects.clear();
}

QContactDetailDefinitionRequest::SelectionType QContactDetailDefinitionRequest::selectionType() const
{
    if (d->m_requestNames.size() > 0)
        return QContactDetailDefinitionRequest::SelectByNames;
    if (d->m_requestObjects.size() > 0)
        return QContactDetailDefinitionRequest::SelectByObject;
    return QContactDetailDefinitionRequest::SelectAll;
}

QStringList QContactDetailDefinitionRequest::nameSelection() const
{
    return d->m_requestNames;
}

QList<QContactDetailDefinition> QContactDetailDefinitionRequest::definitionSelection() const
{
    return d->m_requestObjects;
}

QContactDetailDefinitionRequest::Restriction QContactDetailDefinitionRequest::restriction()
{
    if (d->m_isRestrictedNames)
        return QContactDetailDefinitionRequest::RestrictToNames;
    return QContactDetailDefinitionRequest::NoRestriction;
}

/* =============== Dynamic functions below (trampoline to the engine) */

bool QContactDetailDefinitionRequest::waitForFinished(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForFinished(this, msecs);
    return false;
}

bool QContactDetailDefinitionRequest::waitForProgress(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForProgress(this, msecs);
    return false;
}

void QContactDetailDefinitionRequest::cancel()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->cancelAsynchronousRequest(this);
}

void QContactDetailDefinitionRequest::start(QContactAbstractRequest::Operation operation)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->startAsynchronousRequest(this, operation);
}

QStringList QContactDetailDefinitionRequest::names()
{
    if (!d->m_result)
        return QStringList();

    return d->m_result->definitionNames();
}

QList<QContactDetailDefinition> QContactDetailDefinitionRequest::definitions()
{
    if (!d->m_result)
        return QList<QContactDetailDefinition>();

    return d->m_result->definitions();
}

QList<QContactManager::Error> QContactDetailDefinitionRequest::errors()
{
    if (!d->m_result)
        return QList<QContactManager::Error>();

    return d->m_result->errors();
}

#include "moc_qcontactdetaildefinitionrequest.cpp"
