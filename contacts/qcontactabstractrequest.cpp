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

QContactAbstractRequest::QContactAbstractRequest(QContactAbstractRequestPrivate* otherd)
    : d_ptr(otherd)
{
}

QContactAbstractRequest::~QContactAbstractRequest()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        engine->requestDestroyed(this);
}

bool QContactAbstractRequest::isActive() const
{
    return (d_ptr->m_status == QContactAbstractRequest::Active
            || d_ptr->m_status == QContactAbstractRequest::Cancelling);
}

bool QContactAbstractRequest::isFinished() const
{
    return (d_ptr->m_status == QContactAbstractRequest::Finished
            || d_ptr->m_status == QContactAbstractRequest::Cancelled);
}

QContactManager::Error QContactAbstractRequest::error() const
{
    return d_ptr->m_error;
}

QContactAbstractRequest::Status QContactAbstractRequest::status() const
{
    return d_ptr->m_status;
}

QContactManager* QContactAbstractRequest::manager() const
{
    return d_ptr->m_manager;
}

void QContactAbstractRequest::setManager(QContactManager* manager)
{
    d_ptr->m_manager = manager;
}

bool QContactAbstractRequest::start()
{
    if (status() != QContactAbstractRequest::Inactive
        && status() != QContactAbstractRequest::Cancelled
        && status() != QContactAbstractRequest::Finished) {
        return false; // unable to start operation; another operation already in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        engine->startRequest(this);

    return true;
}

bool QContactAbstractRequest::cancel()
{
    if (status() != QContactAbstractRequest::Active) {
        return false; // unable to cancel operation; operation not in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        engine->cancelRequest(this);

    return true;
}

bool QContactAbstractRequest::waitForFinished(int msecs)
{
    if (status() != QContactAbstractRequest::Active) {
        return false; // unable to wait for operation; not in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        return engine->waitForRequestFinished(this, msecs);
    return false;
}

bool QContactAbstractRequest::waitForProgress(int msecs)
{
    if (status() != QContactAbstractRequest::Active) {
        return false; // unable to wait for operation; not in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d_ptr->m_manager);
    if (engine)
        return engine->waitForRequestProgress(this, msecs);
    return false;
}
