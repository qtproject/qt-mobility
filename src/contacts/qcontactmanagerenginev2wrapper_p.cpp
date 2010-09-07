/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactmanagerenginev2wrapper_p.h"

QTM_USE_NAMESPACE

QContactManagerEngineV2Wrapper::QContactManagerEngineV2Wrapper(QContactManagerEngine *wrappee)
    : m_engine(wrappee)
{
    Q_ASSERT(wrappee);
    // Connect all the signals
    connect(wrappee, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(wrappee, SIGNAL(contactsAdded(QList<QContactLocalId>)),
            this, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(contactsChanged(QList<QContactLocalId>)),
            this, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(contactsRemoved(QList<QContactLocalId>)),
            this, SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(relationshipsAdded(QList<QContactLocalId>)),
            this, SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)),
            this, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    connect(wrappee, SIGNAL(selfContactIdChanged(QContactLocalId, QContactLocalId)),
            this, SIGNAL(selfContactIdChanged(QContactLocalId, QContactLocalId)));
}

QContactManagerEngineV2Wrapper::~QContactManagerEngineV2Wrapper()
{
    delete m_engine;
}

void QContactManagerEngineV2Wrapper::requestDestroyed(QContactAbstractRequest* req)
{
    // TODO - if it's a partial save request, handle it, otherwise pass it on
    return m_engine->requestDestroyed(req);
}

bool QContactManagerEngineV2Wrapper::startRequest(QContactAbstractRequest* req)
{
    if (req && req->type() == QContactAbstractRequest::ContactSaveRequest
        && !static_cast<QContactSaveRequest*>(req)->definitionMask().isEmpty()) {
        // This is a partial save
        // TODO do something
    }

    // Otherwise, pass it on
    return m_engine->startRequest(req);
}

bool QContactManagerEngineV2Wrapper::cancelRequest(QContactAbstractRequest* req)
{
    // TODO - see if we know about this request
    return m_engine->cancelRequest(req);
}

bool QContactManagerEngineV2Wrapper::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    // TODO - see if we know about this request
    return m_engine->waitForRequestFinished(req, msecs);
}

