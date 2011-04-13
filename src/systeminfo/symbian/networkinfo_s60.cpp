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
#include "trace.h"
//This const is added as alternative for EPacketNotifyDynamicCapsChange in pcktcs.h
const TInt KPacketNofityDynamicCapsChange = 40021;


CNetworkBase::CNetworkBase() : CActive(EPriorityNormal),
iConstructed(EFalse), iObserver(NULL),iDynCaps(0)
    {
    TInt err = 0;
    TRACES(qDebug() << "CNetworkBase::CNetworkBase<---");
    CActiveScheduler::Add(this);

    TRAP_IGNORE(
        err = iTelServer.Connect();
        TRACES(qDebug() << "Err val for RTelServer::Connect" << err);
        User::LeaveIfError(err);
    )

    CleanupClosePushL(iTelServer);

    TRAP_IGNORE(
        err = iTelServer.GetPhoneInfo( 0, iPhoneInfo );
        TRACES(qDebug() << "Err val for RTelServer::GetPhoneInfo =" << err);
        User::LeaveIfError(err);
        err = iMobilePhone.Open(iTelServer,iPhoneInfo.iName);
        TRACES(qDebug() << "Err val for RMobilePhone::Open =" << err);
        User::LeaveIfError(err);
        err = iMobilePhone.Initialise();
        TRACES(qDebug() << "Err val for RMobilePhone::Initialise =" << err);
        User::LeaveIfError(err);
    )

#ifdef ETELPACKETSERVICE_SUPPORTED
    TRAP_IGNORE(
        err = iPacketService.Open(iMobilePhone);
        TRACES(qDebug() << "Err val for iPacketService.Open =" << err);
        User::LeaveIfError(err);
        err = iPacketService.GetStatus(iPacketServiceStatus);
        TRACES(qDebug() << "Err val for iPacketService.GetStatus =" << err);
        User::LeaveIfError(err);
    )
#endif
    if (!err)
        {
        iConstructed = ETrue;
        }
    CleanupStack::Pop(&iTelServer);
    TRACES(qDebug() << "CNetworkBase::CNetworkBase--->");
    }

CNetworkBase::~CNetworkBase()
    {
    iMobilePhone.Close();
    iTelServer.Close();
#ifdef ETELPACKETSERVICE_SUPPORTED
    iPacketService.Close();
#endif
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
#ifdef ETELMM_SUPPORTED
    TRACES(qDebug() << "CNetworkMode::CNetworkMode--Start");
    if (iConstructed)
        {
        TInt err = iMobilePhone.GetCurrentMode( iNetworkMode );
        TRACES(qDebug() << "Current Network mode in ctr" <<  iNetworkMode);

        if (!err)
            {
            StartMonitoring();
            }
        else
            iConstructed = EFalse;
        }
#endif
    }
CNetworkMode::~CNetworkMode()
    {
        DoCancel();
    }

void CNetworkMode::DoCancel()
    {
#ifdef ETELMM_SUPPORTED
    TRACES ( qDebug() << "CNetworkMode::DoCancel<--" );
    iMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange);
    TRACES ( qDebug() << "CNetworkMode::DoCancel-->" );
#endif
    }

void CNetworkMode::RunL()
    {
    TRACES(qDebug() << "CNetworkMode::RunL-Networkmode is<--:" << iNetworkMode);
    User::LeaveIfError(iStatus.Int());
    iObserver->ChangedNetworkMode();
    StartMonitoring();
    TRACES(qDebug() << "CNetworkMode::RunL()--->");
    }

void CNetworkMode::StartMonitoring()
    {
#ifdef ETELMM_SUPPORTED
    iMobilePhone.NotifyModeChange ( iStatus,iNetworkMode);
    SetActive();
    TRACES(qDebug() << "CNetworkMode::StartMonitoring--End");
#endif
    }

RMobilePhone::TMobilePhoneNetworkMode CNetworkMode::GetMode() const
    {
    return iNetworkMode;
    }

CNetworkStatus::CNetworkStatus()
   {

#ifdef ETELMM_SUPPORTED
   TRACES (qDebug() << "CNetworkStatus::CNetworkStatus<----");
      if (iConstructed)
        {
        TInt err = iMobilePhone.GetMultimodeCaps( iCapsPhone );
        if (!err)
            {
            TRequestStatus reqStatus;
            iMobilePhone.GetNetworkRegistrationStatus (reqStatus,iNetStatus);
            User::WaitForRequest(reqStatus);
            //CActiveScheduler::Add(this);
            StartMonitoring();
            }
        else
            iConstructed = EFalse;
        }
   TRACES (qDebug() << "CNetworkStatus::CNetworkStatus---->");
#endif
    }
CNetworkStatus::~CNetworkStatus()
    {
     TRACES (qDebug() << "CNetworkStatus::~CNetworkStatus");
      Cancel();
    }

void CNetworkStatus::DoCancel()
    {
#ifdef ETELMM_SUPPORTED
    TRACES (qDebug() << "CNetworkStatus::DoCancel");
    iMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
#endif
    }

void CNetworkStatus::RunL()
    {
    TRACES (qDebug() << "CNetworkStatus::RunL()<---");
    User::LeaveIfError(iStatus.Int());
    iObserver->ChangedNetworkStatus();
    StartMonitoring();
    TRACES (qDebug() << "CNetworkStatus::RunL()--->");
    }

void CNetworkStatus::StartMonitoring()
    {
#ifdef ETELMM_SUPPORTED
    TRACES (qDebug() << "CNetworkStatus::StartMonitoring");
    iMobilePhone.NotifyNetworkRegistrationStatusChange ( iStatus,iNetStatus);
    SetActive();
#endif
    }

TUint32 CNetworkStatus::GetCapability () const
    {
    return iCapsPhone;
    }

#ifdef ETELMM_SUPPORTED
RMobilePhone::TMobilePhoneRegistrationStatus CNetworkStatus::GetStatus () const
    {
    return iNetStatus;
    }
#endif

CPacketDataStatus::CPacketDataStatus()
   {
#ifdef ETELPACKETSERVICE_SUPPORTED
    TRACES (qDebug() << "CPacketDataStatus::CPacketDataStatus");
    if (iPacketServiceStatus == RPacketService::EStatusActive)
      {
        iPacketService.GetDynamicCaps(iDynCaps); //Initialize dynamic caps
      }
    StartMonitoring();
#endif
    }

CPacketDataStatus::~CPacketDataStatus()
    {
     TRACES (qDebug() << "CPacketDataStatus::~CPacketDataStatus");
     Cancel();
    }

TBool CPacketDataStatus::IsDynamicCapsSupported()
  {
#ifdef ETELPACKETSERVICE_SUPPORTED
   TInt err = iPacketService.GetStatus(iPacketServiceStatus);
   if (iPacketServiceStatus == RPacketService::EStatusActive)
    {
     if (iPacketService.GetDynamicCaps(iDynCaps) == KErrNone) //other than KErrNotSupported
      {
       return ETrue;
      }
    }
#endif
   return EFalse;
  }

TUint CPacketDataStatus::DynamicCaps()
 {
  TRACES (qDebug() << "CPacketDataStatus::DynamicCaps value:" << iDynCaps);
  return iDynCaps;
 }

void CPacketDataStatus::DoCancel()
 {
#ifdef ETELPACKETSERVICE_SUPPORTED
  TRACES (qDebug() << "CPacketDataStatus::DoCancel<---");
  iPacketService.CancelAsyncRequest(KPacketNofityDynamicCapsChange);
  TRACES (qDebug() << "CPacketDataStatus::DoCancel--->");
#endif
 }

void CPacketDataStatus::RunL()
 {
#ifdef ETELPACKETSERVICE_SUPPORTED
 TRACES (qDebug() << "CPacketDataStatus::RunL()<---");
 TRACES (qDebug() << "iStatus code:" << iStatus.Int());
 User::LeaveIfError(iStatus.Int());
 iObserver->ChangedCellDataTechnology();
 StartMonitoring();
 TRACES(qDebug() << "CPacketDataStatus::RunL()--->");
#endif
 }

void CPacketDataStatus::StartMonitoring()
 {
#ifdef ETELPACKETSERVICE_SUPPORTED
 TRACES (qDebug() << "CPacketDataStatus::StartMonitoring<---");
 if (!IsActive())
  {
    iPacketService.NotifyDynamicCapsChange(iStatus,iDynCaps);
    SetActive();
  }
 TRACES (qDebug() << "CPacketDataStatus::StartMonitoring--->");
#endif
 }

CNetworkInfo::CNetworkInfo():iCellDataTechnology(KDefaultBearer)
    {
     //Add observers
    iNetStat.Add(this);
    iNetMode.Add(this);
    iPacketDataStatus.Add(this);
    }

RMobilePhone::TMobilePhoneNetworkMode CNetworkInfo::GetMode() const
    {
    return iNetMode.GetMode();
    }
TUint32 CNetworkInfo::GetCapability () const
    {
    return iNetStat.GetCapability();
    }

#ifdef ETELMM_SUPPORTED
RMobilePhone::TMobilePhoneRegistrationStatus CNetworkInfo::GetStatus () const
    {
    return iNetStat.GetStatus();
    }
#endif
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

void CNetworkInfo::ChangedCellDataTechnology()
 {
  TRACES (qDebug() << "CNetworkInfo::ChangedCellDataTechnology");
  TUint celldatatech = CellDataTechnology();
  if ( celldatatech != iCellDataTechnology )
   {
    TRACES (qDebug() << "Notify observers");
     foreach (MNetworkInfoObserver *observer, iObservers)
        observer->changedCellDataTechnology();
   }
   iCellDataTechnology = celldatatech;
 }

 TUint CNetworkInfo::CellDataTechnology()
  {
#ifdef ETELPACKETSERVICE_SUPPORTED
   TRACES (qDebug() << "CNetworkInfo::CellDataTechnology<---");
   TUint dynamicCaps = 0;
   if (iPacketDataStatus.IsDynamicCapsSupported())
    {
     dynamicCaps = iPacketDataStatus.DynamicCaps();
    }

   RMobilePhone::TMobilePhoneNetworkMode networkMode = iNetMode.GetMode();

   if ((dynamicCaps & RPacketService::KCapsHSUPA) || (dynamicCaps & RPacketService::KCapsHSDPA))
    {
     TRACES ( qDebug() << "KHsdpaBearer" );
     return KHsdpaBearer;
    }
   else
    if ( dynamicCaps & RPacketService::KCapsEGPRS )
       {
        TRACES ( qDebug() << "KEGprsBearer" );
        return KEGprsBearer;
       }
   else
       {
        if (networkMode == RMobilePhone::ENetworkModeGsm)
          {
           TRACES ( qDebug() << "KGprsBearer" );
           return KGprsBearer;
          }
        else if (networkMode == RMobilePhone::ENetworkModeWcdma)
          {
           TRACES ( qDebug() << "KUmtsBearer" );
           return KUmtsBearer;
          }
        else
          {
           TRACES ( qDebug() << "KDefaultBearer" );
           return KDefaultBearer;
          }
       }
#else
     return KDefaultBearer;
#endif
  }
