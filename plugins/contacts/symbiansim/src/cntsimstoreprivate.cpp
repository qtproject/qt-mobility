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

#include "cntsimstoreprivate.h"
#include "cntsymbiansimtransformerror.h"
#include <qcontactsynctarget.h>
#include "cntsimstore.h"

const int KOneSimContactBufferSize = 512;
Q_DEFINE_LATIN1_LITERAL(KSimSyncTarget, "SIM");

CntSimStorePrivate::CntSimStorePrivate(CntSymbianSimEngine &engine, CntSimStore &simStore)
    :CActive(CActive::EPriorityStandard),
     m_state(InactiveState),
     m_engine(engine),
     m_simStore(simStore),
     m_etelInfoPckg(m_etelStoreInfo)
{
    CActiveScheduler::Add(this);
}

CntSimStorePrivate::~CntSimStorePrivate()
{
    m_buffer.Close();
    Cancel();
}

QContactManager::Error CntSimStorePrivate::getInfo()
{
    if (IsActive())
        return QContactManager::LockedError;
    
    // start get info request
    mobilePhoneBookStore().GetInfo(iStatus, (TDes8&)m_etelInfoPckg);
    SetActive();
    m_state = GetInfoState;
    
    return QContactManager::NoError;
}

QContactManager::Error CntSimStorePrivate::read(int index, int numSlots)
{
    if (IsActive())
        return QContactManager::LockedError;
    
    // start reading
    m_buffer.Zero();
    m_buffer.ReAlloc(KOneSimContactBufferSize*numSlots);
    mobilePhoneBookStore().Read(iStatus, index, numSlots, m_buffer);
    SetActive();
    m_state = ReadState;
    
    return QContactManager::NoError;
}

QContactManager::Error CntSimStorePrivate::write(const QContact &contact)
{
    if (IsActive())
        return QContactManager::LockedError;
    
    // get index
    m_writeIndex = KErrNotFound;
    if (contact.id().managerUri() == m_engine.managerUri() &&
        contact.localId() > 0) {
        m_writeIndex = contact.localId();  
    }
    
    // encode
    m_buffer.Zero();
    m_buffer.ReAlloc(KOneSimContactBufferSize);
    TRAPD(err, m_convertedContact = m_engine.encodeSimContactL(&contact, m_buffer));
    if (err != KErrNone) {
        QContactManager::Error qtError;
        CntSymbianSimTransformError::transformError(err, qtError);
        return qtError;
    }

    // start writing
    mobilePhoneBookStore().Write(iStatus, m_buffer, m_writeIndex);
    SetActive();
    m_state = WriteState;
    
    return QContactManager::NoError;
}

QContactManager::Error CntSimStorePrivate::remove(int index)
{
    if (IsActive())
        return QContactManager::LockedError;
    
    // NOTE:
    // If index points to an empty slot and running in hardware the 
    // delete operation will not return any error.
    
    mobilePhoneBookStore().Delete(iStatus, index);
    SetActive();
    m_state = DeleteState;
    
    return QContactManager::NoError;
}

void CntSimStorePrivate::DoCancel()
{
    if (m_state == GetInfoState)
        mobilePhoneBookStore().CancelAsyncRequest(EMobilePhoneStoreGetInfo);
    if (m_state == ReadState)
        mobilePhoneBookStore().CancelAsyncRequest(EMobilePhoneStoreRead);
    if (m_state == WriteState)
        mobilePhoneBookStore().CancelAsyncRequest(EMobilePhoneStoreWrite);
    if (m_state == DeleteState)
        mobilePhoneBookStore().CancelAsyncRequest(EMobilePhoneStoreDelete);
    
    m_state = InactiveState;
}

void CntSimStorePrivate::RunL()
{
    User::LeaveIfError(iStatus.Int());
    
    // NOTE: emit might be synchronous so do not touch member variables after it
    
    switch (m_state)
    {
        case GetInfoState:
        {
            emit m_simStore.getInfoComplete(m_etelStoreInfo, QContactManager::NoError);
        }
        break;
        
        case ReadState:
        {
            QList<QContact> contacts = m_engine.decodeSimContactsL(m_buffer);
            m_buffer.Zero();
            emit m_simStore.readComplete(contacts, QContactManager::NoError);
        }
        break;
        
        case WriteState:
        {
            //save id, if new contact
            QContactId contactId;
            contactId.setLocalId(m_writeIndex);
            contactId.setManagerUri(m_engine.managerUri());
            m_convertedContact.setId(contactId);  
            
            // set sync target
            QContactSyncTarget syncTarget;
            syncTarget.setSyncTarget(KSimSyncTarget);
            m_convertedContact.saveDetail(&syncTarget);
            
            emit m_simStore.writeComplete(m_convertedContact, QContactManager::NoError);
        }
        break;
        
        case DeleteState:
        {
            emit m_simStore.removeComplete(QContactManager::NoError);
        }
        break;
        
        default:
        {
            User::Leave(KErrUnknown);
        }
        break;
    }
    m_state = InactiveState;
}

TInt CntSimStorePrivate::RunError(TInt aError)
{
    QContactManager::Error qtError = QContactManager::NoError;
    CntSymbianSimTransformError::transformError(aError, qtError);
        
    if (m_state == GetInfoState) {
        RMobilePhoneBookStore::TMobilePhoneBookInfoV5 emptyInfo;
        emit m_simStore.getInfoComplete(emptyInfo, qtError);
    } else if (m_state == ReadState) {
        QList<QContact> emptyList;
        emit m_simStore.readComplete(emptyList, qtError);            
    } else if (m_state == WriteState) {
        emit m_simStore.writeComplete(m_convertedContact, qtError);  
    } else if (m_state == DeleteState) {
        emit m_simStore.removeComplete(qtError);
    }
    
    m_state = InactiveState;
    
    return KErrNone;
}
