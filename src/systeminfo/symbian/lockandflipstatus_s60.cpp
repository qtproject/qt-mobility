/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "lockandflipstatus_s60.h"
#include <e32debug.h>
#define KFlipStateUnknown -1

CKeylockStatus::CKeylockStatus() : CActive(EPriorityStandard),
    m_currentStatus(EKeyguardNotActive)
{
    CActiveScheduler::Add(this);
    TInt err = m_keylockProperty.Attach(KPSUidAvkonDomain, KAknKeyguardStatus);
    if (err == KErrNone) {
        TInt status = EKeyguardNotActive;
        m_keylockProperty.Get(status);
        m_currentStatus = status;
        startMonitoring();
    }
}

CKeylockStatus::~CKeylockStatus()
{
    Cancel();
    m_keylockProperty.Close();
}

void CKeylockStatus::addObserver(MKeylockStatusObserver *observer)
{
    m_observers.append(observer);
}

void CKeylockStatus::removeObserver(MKeylockStatusObserver *observer)
{
    m_observers.removeOne(observer);
}

void CKeylockStatus::DoCancel()
{
    m_keylockProperty.Cancel();
}

void CKeylockStatus::RunL()
{
    TInt status = EKeyguardNotActive;
    m_keylockProperty.Get(status);
    m_currentStatus = status;
    foreach (MKeylockStatusObserver *observer, m_observers)
        observer->keylockStatusChanged(m_currentStatus);

    startMonitoring();
}

void CKeylockStatus::startMonitoring()
{
 if (!IsActive())
    {
    m_keylockProperty.Subscribe(iStatus);
    SetActive();
    }
}

int CKeylockStatus::getLockStatus()
{
return m_currentStatus;
}

CFlipStatus::CFlipStatus() : CActive(EPriorityStandard),
    m_flipStatus(EPSHWRMGripStatusUninitialized),m_filpKeyBoard(-1),m_flipenabled(false),m_gripenabled(false)
{
    CActiveScheduler::Add(this);
    TInt status = KFlipStateUnknown;

    //In symbian3 devices KHWRMGripStatus is used for getting flip open/closed state
    RProperty::Get(KPSUidHWRM,  KHWRMGripStatus, status);
    if ( status != EPSHWRMGripStatusUninitialized ) {
        m_flipStatus = status;
        m_gripenabled = true;
        m_FlipProperty.Attach(KPSUidHWRM,  KHWRMGripStatus);
        m_filpKeyBoard = 1;
     }

    // In 5.0 devices KHWRMFlipStatus is used for getting flip open/closed state
    RProperty::Get(KPSUidHWRM,  KHWRMFlipStatus, status);
    if ( status != EPSHWRMFlipStatusUninitialized ) {
        m_flipStatus = status;
        m_flipenabled = true;
        m_FlipProperty.Attach(KPSUidHWRM,  KHWRMFlipStatus);
        m_filpKeyBoard = 1;
     }

    if ( m_flipenabled || m_gripenabled ) {
     startMonitoring();
    }

}

//void CFlipStatus::ConstructL()
//{
//    TInt flipKbType = 0;
//#ifdef SYMBIAN_3_PLATFORM
//    TRAP_IGNORE(
//                CRepository* repository = CRepository::NewLC( KCRUidAvkon ) ;
//                User::LeaveIfError(repository->Get( KAknKeyboardSlideOpen, flipKbType));
//                CleanupStack::PopAndDestroy(repository);
//            )
//#endif
//    if (flipKbType)
//        m_filpKeyBoard = 1;
//    else
//        m_filpKeyBoard = 0;
//}

CFlipStatus::~CFlipStatus()
{
    Cancel();
    m_FlipProperty.Close();
}

void CFlipStatus::addObserver(MFlipStatusObserver *observer)
{
    m_observers.append(observer);
}

void CFlipStatus::removeObserver(MFlipStatusObserver *observer)
{
    m_observers.removeOne(observer);
}

void CFlipStatus::DoCancel()
{
    m_FlipProperty.Cancel();
}

void CFlipStatus::RunL()
{
   TInt status = KFlipStateUnknown;
   if ( m_gripenabled ) {
    RProperty::Get(KPSUidHWRM,  KHWRMGripStatus, status);
   }

   if ( m_flipenabled ) {
    RProperty::Get(KPSUidHWRM,  KHWRMFlipStatus, status);
   }
   m_flipStatus = status;

   foreach (MFlipStatusObserver *observer, m_observers)
       observer->flipStatusChanged(m_flipStatus , m_filpKeyBoard);

   startMonitoring();
}

void CFlipStatus::startMonitoring()
{
  if (!IsActive()) {
    m_FlipProperty.Subscribe(iStatus);
    SetActive();
    }
}

bool CFlipStatus::getFlipStatus()
{
   //In 5.0 it is EPSHWRMFlipOpen and symbian3 EPSHWRMGripOpen
   return (m_flipStatus  == EPSHWRMGripOpen || m_flipStatus == EPSHWRMFlipOpen);
}

bool CFlipStatus::IsFlipSupported()
{
   //TBD: Need to find a way how to find the flipkeyboard presence
   return (m_filpKeyBoard == 1);
}


