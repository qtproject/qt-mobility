/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "networkinfo_s60.h"

CNetworkBase::CNetworkBase() : CActive(EPriorityStandard),
iConstructed(EFalse), iObserver(NULL)
    {
    TInt err = 0;
    TRAP_IGNORE(
        err = iTelServer.Connect();
        RDebug::Print(_L("Err val for RTelServer::Connect = %d"),err);
        User::LeaveIfError(err);
        CleanupClosePushL(iTelServer);
        err = iTelServer.GetPhoneInfo( 0, iPhoneInfo );
        RDebug::Print(_L("Err val for RTelServer::GetPhoneInfo = %d"),err);
        User::LeaveIfError(err);
        err = iMobilePhone.Open(iTelServer,iPhoneInfo.iName);
        RDebug::Print(_L("Err val for RMobilePhone::Open = %d"),err);
        User::LeaveIfError(err);
        err = iMobilePhone.Initialise();
        RDebug::Print(_L("Err val for RMobilePhone::Initialise = %d"),err);
        User::LeaveIfError(err);
        CleanupStack::Pop(&iTelServer);
    )
    if (!err)
        {
        iConstructed = ETrue;
        }
    }

CNetworkBase::~CNetworkBase()
    {
    iMobilePhone.Close();
    iTelServer.Close();
    }

void CNetworkBase::AddObserver(MNetworkObserver *aObserver)
    {
    iObserver = aObserver;
    }

void CNetworkBase::RemoveObserver()
    {
    iObserver = NULL;
    }

CNetworkMode::CNetworkMode()
    {
    if (iConstructed)
        {
        TInt err = iMobilePhone.GetCurrentMode( iNetworkMode );

        if (!err)
            {
            CActiveScheduler::Add(this);
            StartMonitoring();
            }
        else
            iConstructed = EFalse;
        }
    }
CNetworkMode::~CNetworkMode()
    {
        DoCancel();
    }

void CNetworkMode::DoCancel()
    {
    iMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange);
    }

void CNetworkMode::RunL()
    {
    RDebug::Print(_L("CNetworkMode::RunL Called"));
    iObserver->ChangedNetworkMode();
    StartMonitoring();
    }

void CNetworkMode::StartMonitoring()
    {
    iMobilePhone.NotifyModeChange ( iStatus,iNetworkMode);
    SetActive();
    }

RMobilePhone::TMobilePhoneNetworkMode CNetworkMode::GetMode() const
    {
    return iNetworkMode;
    }

CNetworkStatus :: CNetworkStatus()
    {
    if (iConstructed)
        {
        TInt err = iMobilePhone.GetMultimodeCaps( iCapsPhone );
        if (!err)
            {
            TRequestStatus reqStatus;
            iMobilePhone.GetNetworkRegistrationStatus (reqStatus,iNetStatus);
            User::WaitForRequest(reqStatus);
            CActiveScheduler::Add(this);
            StartMonitoring();
            }
        else
            iConstructed = EFalse;
        }
    }
CNetworkStatus::~CNetworkStatus()
    {
        DoCancel();
    }

void CNetworkStatus::DoCancel()
    {
    iMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
    }

void CNetworkStatus::RunL()
    {

    RDebug::Print(_L("CNetworkStatus::RunL Called"));
    iObserver->ChangedNetworkStatus();
    StartMonitoring();
    }

void CNetworkStatus::StartMonitoring()
    {
    iMobilePhone.NotifyNetworkRegistrationStatusChange ( iStatus,iNetStatus);
    SetActive();
    }

TUint32 CNetworkStatus::GetCapability () const
    {
    return iCapsPhone;
    }
RMobilePhone::TMobilePhoneRegistrationStatus CNetworkStatus::GetStatus () const
    {
    return iNetStatus;
    }

CNetworkInfo::CNetworkInfo()
    {
    iNetStat.Add(this);
    iNetMode.Add(this);
    }

RMobilePhone::TMobilePhoneNetworkMode CNetworkInfo::GetMode() const
    {
    return iNetMode.GetMode();
    }
TUint32 CNetworkInfo::GetCapability () const
    {
    return iNetStat.GetCapability();
    }

RMobilePhone::TMobilePhoneRegistrationStatus CNetworkInfo::GetStatus () const
    {
    return iNetStat.GetStatus();
    }

void CNetworkInfo::addObserver(MNetworkInfoObserver *aObserver)
{
    iObservers.append(aObserver);
}

void CNetworkInfo::removeObserver(MNetworkInfoObserver *aObserver)
{
    iObservers.removeOne(aObserver);
}

void CNetworkInfo::ChangedNetworkMode()
    {
    foreach (MNetworkInfoObserver *observer, iObservers)
        observer->changedNetworkMode();
    }
void CNetworkInfo::ChangedNetworkStatus()
    {
    foreach (MNetworkInfoObserver *observer, iObservers)
        observer->changedNetworkStatus();
    }
