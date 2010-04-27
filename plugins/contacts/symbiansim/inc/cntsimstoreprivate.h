
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

#ifndef CNTSIMSTOREPRIVATE_H_
#define CNTSIMSTOREPRIVATE_H_

#include <e32base.h>
#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <qcontact.h>

#include "cntsymbiansimengine.h"
#include "cntsimstore.h"

QTM_USE_NAMESPACE

class CntSimStore;
class CntSymbianSimEngine;
class CntSimStoreEventListener;

class CntSimStorePrivate : public CActive
{
public:
    enum State {
        InactiveState,
        ReadState,
        WriteState,
        DeleteState,
        ReadReservedSlotsState
    };
    static CntSimStorePrivate* NewL(CntSymbianSimEngine &engine, CntSimStore &simStore, const QString &storeName);
    ~CntSimStorePrivate();
    
    QString storeName() { return m_storeName; }
    TSimStoreInfo storeInfo() { return m_storeInfo; }

    bool read(int index, int numSlots, QContactManager::Error *error);
    bool write(const QContact &contact, QContactManager::Error *error);
    bool remove(int index, QContactManager::Error *error);
    bool getReservedSlots(QContactManager::Error *error);
    
    TInt lastAsyncError() { return m_asyncError; }
    
private: 
    // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
private:
    CntSimStorePrivate(CntSymbianSimEngine &engine, CntSimStore &simStore, const QString &storeName);
    void ConstructL();
    void convertStoreNameL(TDes &storeName);
    QList<QContact> decodeSimContactsL(TDes8& rawData) const;
    void encodeSimContactL(QContact* contact, TDes8& rawData) const;
    void putTagAndValueL(CPhoneBookBuffer* pbBuffer, TUint8 tag, QString data) const;
    QList<int> decodeReservedSlotsL(TDes8& rawData) const;
    
private:
    State m_state;
    CntSymbianSimEngine &m_engine;
    QString m_managerUri;
    CntSimStore &m_simStore;
    RTelServer m_etelServer;
    RMobilePhone m_etelPhone;
    RMobilePhoneBookStore m_etelStore;
    QString m_storeName;
    bool m_readOnlyAccess;
    TSimStoreInfo m_storeInfo;
    TSimStoreInfoPckg m_storeInfoPckg;
    RBuf8 m_buffer;
    QContact m_convertedContact;
    int m_writeIndex;
    CntSimStoreEventListener* m_listener;
    TInt m_asyncError;
};

#endif // CNTSIMSTOREPRIVATE_H_
