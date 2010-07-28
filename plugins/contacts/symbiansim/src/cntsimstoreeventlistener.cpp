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

#include "cntsimstoreeventlistener.h"
#include "cntsymbiansimengine.h"
#include <qcontactchangeset.h>
#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <QDebug>

QTM_USE_NAMESPACE

CntSimStoreEventListener::CntSimStoreEventListener(CntSymbianSimEngine &engine, RMobilePhoneBookStore& store)
    :CActive(CActive::EPriorityUserInput),
     m_engine(engine),
     m_store(store)
{
    // NOTE:
    // The AO priority must be higher than CntSimStorePrivate's priority.
    // Otherwise we might not receive all store events. This is because a new
    // store request might be issued before the listener has had the chance to
    // issue a new notify request.
    CActiveScheduler::Add(this);
}

CntSimStoreEventListener::~CntSimStoreEventListener()
{
    Cancel();
}

void CntSimStoreEventListener::start()
{
    if (IsActive())
        return;
    
    m_store.NotifyStoreEvent(iStatus, m_event, m_index);
    SetActive();
}

void CntSimStoreEventListener::DoCancel()
{
    if (IsActive())
        m_store.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
}

void CntSimStoreEventListener::RunL()
{
    if (iStatus.Int()) {
        qWarning() << "Failed to listen store events!" << iStatus.Int();
        start(); // We cannot do anything about the error. Just try again.
        return;
    }
    
    // The store may report several events at the same time.
    
    QContactChangeSet changeSet;

    if (m_event & RMobilePhoneStore::KStoreFull) { 
        //qDebug() << "SIM store event: full";
    }
    if (m_event & RMobilePhoneStore::KStoreHasSpace) { 
        //qDebug() << "SIM store event: has space";
    }
    if (m_event & RMobilePhoneStore::KStoreEmpty ) { 
        //qDebug() << "SIM store event: empty" << m_index;
        changeSet.insertRemovedContact(m_index);
    }
    if (m_event & RMobilePhoneStore::KStoreEntryAdded) {
        //qDebug() << "SIM store event: added" << m_index;
        changeSet.insertAddedContact(m_index);
    }
    if (m_event & RMobilePhoneStore::KStoreEntryDeleted) {
        //qDebug() << "SIM store event: deleted" << m_index;
        changeSet.insertRemovedContact(m_index);
    }
    if (m_event & RMobilePhoneStore::KStoreEntryChanged) {
        //qDebug() << "SIM store event: changed" << m_index;
        changeSet.insertChangedContact(m_index);
    }
    if (m_event & RMobilePhoneStore::KStoreDoRefresh) {
        //qDebug() << "SIM store event: do refresh";
        changeSet.setDataChanged(true);
    }
    
    changeSet.emitSignals(&m_engine);
    
    start();
}
