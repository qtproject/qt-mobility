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

#include "qcontact_p.h"
#include "qcontactmanagerenginev3wrapper_p.h"
#include "qcontactlocalidfilter.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactfetchbyidrequest.h"
#include "qcontactobserver.h"
#include <QDebug>

QTM_USE_NAMESPACE

QContactManagerEngineV3Wrapper::QContactManagerEngineV3Wrapper(QContactManagerEngineV2 *wrappee)
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

QContactManagerEngineV3Wrapper::~QContactManagerEngineV3Wrapper()
{
    delete m_engine;
}

QSharedPointer<QContactObserver> QContactManagerEngineV3Wrapper::observeContact(QContactLocalId contactId)
{
    QContactObserver* observer = createContactObserver(this);
    connect(observer, SIGNAL(destroyed(QObject*)), this, SLOT(observerDestroyed(QContactObserver*)));
    m_observerForContact.insert(contactId, observer);
    return QSharedPointer<QContactObserver>(observer);
}

void QContactManagerEngineV3Wrapper::observerDestroyed(QContactObserver* observer)
{
    QContactLocalId key = m_observerForContact.key(observer);
    if (key != 0) {
        m_observerForContact.remove(key, observer);
    }
}

void QContactManagerEngineV3Wrapper::contactsUpdated()
{
    foreach (QContactObserver* observer, m_observerForContact.values()) {
        observer->emitContactChanged();
    }
}

void QContactManagerEngineV3Wrapper::contactsUpdated(const QList<QContactLocalId>& ids)
{
    foreach (QContactLocalId id, ids) {
        QHash<QContactLocalId, QContactObserver*>::iterator it = m_observerForContact.find(id);
        while (it != m_observerForContact.end()) {
            (*it)->emitContactChanged();
            it++;
        }
    }
}

void QContactManagerEngineV3Wrapper::contactsDeleted(const QList<QContactLocalId>& ids)
{
    foreach (QContactLocalId id, ids) {
        QHash<QContactLocalId, QContactObserver*>::iterator it = m_observerForContact.find(id);
        while (it != m_observerForContact.end()) {
            (*it)->emitContactRemoved();
            it++;
        }
    }
}
