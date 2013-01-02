/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <e32debug.h>

#include "clbspsycontainer_s60_p.h"


#include <epos_cposmoduleidlist.h>
#include <epos_cposmoduleupdate.h>
#include "epos_lastknownlocationpskeys.h"

QTM_BEGIN_NAMESPACE

CSelfManagingPsy::CSelfManagingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback)
        : CActive(EPriorityNormal), iContainer(aContainer), iPositioner(NULL),
        iConsequitiveFailureCount(0), iPsyModInfo(aPsy), iRequestCallback(aRequestCallback),
        iClientRequestActive(EFalse), iPsyAvailabilityStatus(EFalse)
{
    TBuf<20> psyName;
    iPsyModInfo->GetModuleName(psyName);
    QString qPsyName((QChar*)psyName.Ptr(), psyName.Length());
    iPsyName = qPsyName;
}
void CSelfManagingPsy::ConstructL()
{
    qDebug() << "CSelfManagingPsy::ConstructL " << iPsyName ;

    TPositionModuleInfo modInfo;
    //If there is any problem with mod ID, then no point in continuing
    User::LeaveIfError(iContainer->PosServer().GetModuleInfoById(iPsyModInfo->ModuleId(), modInfo));

    iPsyAvailabilityStatus = modInfo.IsAvailable();
    iContainer->RegisterPsyStatusListenerL(this, iPsyModInfo->ModuleId());
    InitPositioner();
    CActiveScheduler::Add(this);
}
CSelfManagingPsy::~CSelfManagingPsy()
{
    qDebug() << "CSelfManagingPsy::~CSelfManagingPsy " << iPsyName;
    iContainer->DeRegisterPsyStatusListener(this);
    iPositioner->Close();
    delete iPositioner;
    iPositioner = NULL;
}

void CSelfManagingPsy::Cancel()
{
    qDebug() << "CSelfManagingPsy::Cancel " << iPsyName;
    iClientRequestActive = EFalse;
    CActive::Cancel();
}
TPositionModuleInfo* CSelfManagingPsy::GetModInfo()
{
    return iPsyModInfo;
}
void CSelfManagingPsy::DoCancel()
{
    qDebug() << "CSelfManagingPsy::DoCancel " << iPsyName;

    if (iPositioner)
        iPositioner->CancelRequest(EPositionerNotifyPositionUpdate);

    iClientRequestActive = EFalse;

    qDebug() << "CSelfManagingPsy::DoCancel 2" << iPsyName;
}

void CSelfManagingPsy::HandlePositionCallback(TRequestStatus &aStatus)
{
    switch (aStatus.Int()) {
    case KErrNone:
        qDebug() << "CSelfManagingPsy::HandlePositionCallback - Good fix " << iPsyName ;
        iRequestCallback->LocationUpdate(iPosInfo, this);
        iConsequitiveFailureCount = 0;
        break;
    default:
        qDebug() << "CSelfManagingPsy::HandlePositionCallback - ERROR " << iPsyName << " Err=" << iStatus.Int() ;
        iConsequitiveFailureCount++;
        iRequestCallback->RequestTimedOut(iConsequitiveFailureCount, this);
        break;
    }
}
void CSelfManagingPsy::NotifyPsyStatusChange(TBool aAvailability)
{
    qDebug() << "CSelfManagingPsy::NotifyPsyStatusChange " << iPsyName << " availability=" << aAvailability << " iClientRequestActive=" << iClientRequestActive << " iPositioner " << iPositioner ;
    iPsyAvailabilityStatus = aAvailability;
    if (iPsyAvailabilityStatus && iClientRequestActive) {
        if (!iPositioner) {
            if (!InitPositioner())
                return;
        }
        CSelfManagingPsy::IssueRequest();
    }
}
TBool CSelfManagingPsy::InitPositioner()
{
    qDebug() << "CSelfManagingPsy::InitPositioner " << iPsyName ;

    if (iPositioner) {
        qDebug() << "CSelfManagingPsy::InitPositioner ALREADY INTITIALIZED " << iPsyName ;
        return ETrue;
    }

    TRAPD(ret, iPositioner = new(ELeave) RPositioner());
    if (ret != KErrNone) {
        qDebug() << "CSelfManagingPsy::InitPositioner create positioner failed " << iPsyName ;

        return EFalse;//Nothing can be done
    }

    TInt err = iPositioner->Open(iContainer->PosServer(), iPsyModInfo->ModuleId());
    if (err != KErrNone) {
        delete iPositioner;
        iPositioner = NULL;
        return EFalse; //Nothing can be done
    }
    iPositioner->SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("QTmobility_Location"));
    qDebug() << "CSelfManagingPsy::InitPositioner SUCCESS " << iPsyName ;
    return ETrue;
}

void CSelfManagingPsy::IssueRequest()
{
    qDebug() << "CSelfManagingPsy::IssueRequest( " << iPsyName  ;

    if (IsActive())
        Cancel();

    if (!iPositioner) {
        qDebug() << "CSelfManagingPsy::IssueRequest " << iPsyName << " POSITIONER NOT AVBL";
        return;
    }
    if (!iPsyAvailabilityStatus) {
        //PSY is not avbl so no point calling NotifyPositionUpdate
        //If the PSY becomes available, then NotifyPositionUpdate
        //will be called from NotifyPsyStatusChange
        qDebug() << "CSelfManagingPsy::IssueRequest " << iPsyName << " NOT AVBL";
        return;
    }

    TPositionUpdateOptions updateOptions(iUpdInterval, iTimeout);
    iPositioner->SetUpdateOptions(updateOptions);

    iPositioner->NotifyPositionUpdate(iPosInfo, iStatus);
    SetActive();
}

CTrackingPsy* CTrackingPsy::NewL(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback)
{
    CTrackingPsy *trackingPsy = NULL;

    TPositionModuleInfo::TTechnologyType ttype = aPsy->TechnologyType();
    TPositionModuleInfo::TCapabilities capabilities = aPsy->Capabilities();

    if ((ttype  & TPositionModuleInfo::ETechnologyNetwork) &&
            (!(capabilities  & TPositionModuleInfo::ECapabilitySatellite))) {
        trackingPsy = new CNWTrackingPsy(aPsy, aContainer, aRequestCallback);
    } else {
        trackingPsy = new CTrackingPsy(aPsy, aContainer, aRequestCallback);
    }

    TRAPD(err, trackingPsy->ConstructL());

    if (err != KErrNone) {
        qDebug() << "CTrackingPsy::NewL ERROR " << "PSY= " << trackingPsy->iPsyName  ;
        delete trackingPsy;
        return NULL;
    }
    return trackingPsy;
}
CTrackingPsy::~CTrackingPsy()
{
    qDebug() << "CTrackingPsy::~CTrackingPsy " << "PSY= " << iPsyName  ;
}


void CTrackingPsy::IssueRequest(TTimeIntervalMicroSeconds updateInterval)
{
    qDebug() << "CTrackingPsy::IssueRequest - START updateInterval " << updateInterval.Int64() << "PSY= " << iPsyName  ;

    iClientRequestActive = ETrue;

    TPositionQuality quality;
    iPsyModInfo->GetPositionQuality(quality);
    if (updateInterval < quality.TimeToNextFix())
        updateInterval = quality.TimeToNextFix();

    iUpdInterval = updateInterval;

    TTimeIntervalMicroSeconds timeOut(5*quality.TimeToFirstFix().Int64());
    iTimeout = timeOut;

    CSelfManagingPsy::IssueRequest();
}



void CTrackingPsy::RunL()
{
    qDebug() << "CTrackingPsy::RunL " << iPsyName << " iStatus=" << iStatus.Int();
    HandlePositionCallback(iStatus);
    if (iPsyAvailabilityStatus) {
        iPositioner->NotifyPositionUpdate(iPosInfo, iStatus);
        SetActive();
    }
}

CTrackingPsy::CTrackingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback)
        : CSelfManagingPsy(aPsy, aContainer, aRequestCallback)
{
}

void CTrackingPsy::ConstructL()
{
    qDebug() << "CTrackingPsy::ConstructL " << iPsyName ;
    CSelfManagingPsy::ConstructL();
}

CNWTrackingPsy::CNWTrackingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer,
                               MPsyRequestCallback* aRequestCallback):
        CTrackingPsy(aPsy, aContainer, aRequestCallback), iRealRequest(ETrue)
{
    iTimer.CreateLocal();
}
void CNWTrackingPsy::RunL()
{
    qDebug() << "CNWTrackingPsy::RunL " << iPsyName << " iStatus=" << iStatus.Int();

    if (iRealRequest) {
        qDebug() << "CNWTrackingPsy::RunL SENDING POS and START TIMER" << iPsyName;
        //A real request has completed, the next should be timer
        iRealRequest = EFalse;
        HandlePositionCallback(iStatus);
        iTimer.After(iStatus, 10000000);//10 secs
        SetActive();
    } else {
        iRealRequest = ETrue;
        if (iPsyAvailabilityStatus && iPositioner) {
            qDebug() << "CNWTrackingPsy::RunL NEW REQUEST" << iPsyName;
            iPositioner->NotifyPositionUpdate(iPosInfo, iStatus);
            SetActive();
        } else {
            qDebug() << "CNWTrackingPsy::RunL 2" << iPsyName;
        }
    }

}


CNWTrackingPsy::~CNWTrackingPsy()
{
    qDebug() << "CNWTrackingPsy::~CNWTrackingPsy" << iPsyName ;
    iTimer.Close();
}
void CNWTrackingPsy::DoCancel()
{
    qDebug() << "CSelfManagingPsy::DoCancel " << iPsyName;

    iTimer.Cancel();

    if (iPositioner)
        iPositioner->CancelRequest(EPositionerNotifyPositionUpdate);

    iClientRequestActive = EFalse;

    qDebug() << "CSelfManagingPsy::DoCancel 2" << iPsyName;
}


CSingleShotPsy* CSingleShotPsy::NewL(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback)
{
    CSingleShotPsy *Psy = new CSingleShotPsy(aPsy, aContainer, aRequestCallback);
    TRAPD(err, Psy->ConstructL());

    if (err != KErrNone) {
        delete Psy;
        return NULL;
    }
    return Psy;
}

CSingleShotPsy::CSingleShotPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback)
        : CSelfManagingPsy(aPsy, aContainer, aRequestCallback)
{}
void CSingleShotPsy::ConstructL()
{

    CSelfManagingPsy::ConstructL();
    qDebug() << "CSingleShotPsy::ConstructL " << iPsyName ;
}

void CSingleShotPsy::IssueRequest(TTimeIntervalMicroSeconds aTimeOut)
{
    qDebug() << "CSingleShotPsy::IssueRequest - START " << iPsyName ;

    iClientRequestActive = ETrue;

    iTimeout = aTimeOut;
    TTimeIntervalMicroSeconds intr(10000000) ;//10 sec, doesn't matter
    iUpdInterval = intr;

    CSelfManagingPsy::IssueRequest();
}

void CSingleShotPsy::RunL()
{
    qDebug() << "CSingleShotPsy::RunL " << iPsyName << " iStatus=" << iStatus.Int();
    HandlePositionCallback(iStatus);
    iPositioner->CancelRequest(EPositionerNotifyPositionUpdate);
}

CPsyContainer* CPsyContainer::NewL()
{
    CPsyContainer *container = new(ELeave) CPsyContainer;
    TRAPD(err, container->ConstructL());
    if (err == KErrNone)
        return container;
    else {
        delete container;
        return NULL;
    }
}

CPsyContainer::~CPsyContainer()
{
    for (int i = 0; i < iAllPsys.Count(); i++)
        delete iAllPsys[i];

    for (int j = 0; j < iPsyStatusListeners.Count(); j++)
        delete iPsyStatusListeners[j];

    iAllPsys.Close();
    iPsyStatusListeners.Close();
    iPosServer.Close();

}

CPsyContainer::CPsyContainer() : iNWCallback(NULL), iGPSCallback(NULL)
{

}
void CPsyContainer::ConstructL()
{
    User::LeaveIfError(iPosServer.Connect());
    //Get all modules and store them
    InitAllPsysL();

    iPsyModules = CPosModules::OpenL();
    iPsyModules->SetObserverL(*this);
    //iPnsListener = CLbsLastKnownPositioner::NewL(this);//TODO
}

void CPsyContainer::RegisterPsyStatusListenerL(MPsyStatusChangeCallback *aCallBack, TPositionModuleId aModId)
{
    TPsyStatusListener *ptr = new(ELeave) TPsyStatusListener();
    ptr->iCallBack = aCallBack;
    ptr->iModId = aModId;
    iPsyStatusListeners.Insert(ptr, iPsyStatusListeners.Count());
}

void CPsyContainer::DeRegisterPsyStatusListener(MPsyStatusChangeCallback *aCallBack)
{
    for (int i = 0; i < iPsyStatusListeners.Count(); i++) {
        if (iPsyStatusListeners[i]->iCallBack == aCallBack) {
            delete iPsyStatusListeners[i];
            iPsyStatusListeners.Remove(i);
            break;
        }
    }
}
TBool CPsyContainer::IsNWPsy(TPositionModuleInfo *aModInfo) const
{
    if (!aModInfo)
        return EFalse;

    TPositionModuleInfo::TTechnologyType ttype = aModInfo->TechnologyType();
    TPositionModuleInfo::TCapabilities capabilities = aModInfo->Capabilities();
    if ((ttype  & TPositionModuleInfo::ETechnologyNetwork) &&
            (!(capabilities  & TPositionModuleInfo::ECapabilitySatellite))) {
        //NW PSY
        return ETrue;
    }
    return EFalse;
}
TBool CPsyContainer::IsSatellitePsy(TPositionModuleInfo *aModInfo) const
{
    if (!aModInfo)
        return EFalse;

    TPositionModuleInfo::TTechnologyType ttype = aModInfo->TechnologyType();
    TPositionModuleInfo::TCapabilities capabilities = aModInfo->Capabilities();

    if (capabilities  & TPositionModuleInfo::ECapabilitySatellite)
        return ETrue;

    return EFalse;
}

void CPsyContainer::HandleSettingsChangeL(TPosModulesEvent aEvent)
{
    TPositionModuleId modId = aEvent.iModuleId;

    if (aEvent.iType & EPosModulesEventModuleRemoved) {
        //1) Update the module info list
        TPositionModuleInfo *modInfo = NULL;
        for (int i = 0; i < iAllPsys.Count(); i++) {
            if (modId == iAllPsys[i]->ModuleId());
            {
                modInfo = iAllPsys[i];
                iAllPsys.Remove(i);
                break;
            }
        }
        //2) Notify listeners
        if (IsSatellitePsy(modInfo))
            iGPSCallback->PsyUninstalled(modId);
        else /*if(IsNWPsy)*/
            iNWCallback->PsyUninstalled(modId);

        delete modInfo;
        return;
    }

    if (aEvent.iType &  EPosModulesEventModuleInstalled) {
        TPositionModuleInfo *modInfo = new TPositionModuleInfo ;
        if (!modInfo)
            return;//nothing can be done


        //If there is any problem with mod ID, then no point in continuing
        if (KErrNone != iPosServer.GetModuleInfoById(modId, *modInfo)) {
            delete modInfo;
            return;
        }
        iAllPsys.Insert(modInfo, iAllPsys.Count());

        if (IsSatellitePsy(modInfo))
            iGPSCallback->PsyInstalled(modInfo);
        else /*if(IsNWPsy)*/
            iNWCallback->PsyInstalled(modInfo);
        return;
    }

    if (aEvent.iType & EPosModulesEventAvailabilityChanged) {

        TPositionModuleInfo modInfo;
        if (KErrNone == iPosServer.GetModuleInfoById(modId, modInfo)) {
            for (int i = 0; i < iPsyStatusListeners.Count(); i++) {
                if (iPsyStatusListeners[i]->iModId == modId)
                    iPsyStatusListeners[i]->iCallBack->NotifyPsyStatusChange(modInfo.IsAvailable());
                //There can be multiple listeners (tracking, single shot) so do not break here
            }

            for (int j = 0; j < iAllPsys.Count(); j++) {
                if (modId == iAllPsys[j]->ModuleId());
                {
                    iAllPsys[j]->SetIsAvailable(modInfo.IsAvailable());
                    break;
                }
            }
        }
    }

}

void CPsyContainer::InitAllPsysL()
{
    TUint totalModules = 0;

    iPosServer.GetNumModules(totalModules);
    TInt insertCount = 0 ;
    for (TInt i = 0 ; i < totalModules ; i++) {
        TPositionModuleInfo *modInfo = new(ELeave) TPositionModuleInfo;

        if (KErrNone == iPosServer.GetModuleInfoByIndex(i, *modInfo))
            iAllPsys.Insert(modInfo, insertCount++);
        else
            delete modInfo;
    }
    qDebug() << "CPsyContainer::InitAllPsysL - End Inserted " << insertCount << " psys" ;
}



void CPsyContainer::GetAvailablePsys(TBool aNWTye, RArray<TPositionModuleInfo*> &aPsys)
{
    TUint insertCount = 0;

    for (int i = 0; i < iAllPsys.Count(); i++) {
        TPositionModuleInfo::TTechnologyType ttype = iAllPsys[i]->TechnologyType();
        TPositionModuleInfo::TCapabilities capabilities = iAllPsys[i]->Capabilities();

        if (aNWTye) {

            if ((ttype  & TPositionModuleInfo::ETechnologyNetwork) &&
                    (!(capabilities  & TPositionModuleInfo::ECapabilitySatellite))) {
                aPsys.Insert(iAllPsys[i], insertCount++);
                //break;//TODO: Hack for ignoring SUPL PSY
            }
        } else {
            if (capabilities  & TPositionModuleInfo::ECapabilitySatellite) {
                aPsys.Insert(iAllPsys[i], insertCount++);
            }
        }
    }
    qDebug() << "CPsyContainer::GetAvailablePsys - End insertCount=" << insertCount << " aNWTye=" << aNWTye ;
}

void CPsyContainer::SetNWPSYChangeCallback(MPsyCountChangeCallback *aNWCallback)
{
    qDebug() << "CPsyContainer::SetNWPSYChangeCallback"  ;
    iNWCallback = aNWCallback;
}

void CPsyContainer::SetGPSPSYChangeCallback(MPsyCountChangeCallback *aGPSCallback)
{
    qDebug() << "CPsyContainer::SetGPSPSYChangeCallback"  ;
    iGPSCallback = aGPSCallback;
}

RPositionServer& CPsyContainer::PosServer()
{
    return iPosServer;
}



QGeoPositionInfo CPsyContainer::lastKnownPosition(bool aFromSatellitePositioningMethodsOnly)
{
    QGeoPositionInfo posUpdate;
    TPosition pos;
    TInt error = KErrNone;
    RPositioner lastKnownpositioner;
    TRequestStatus status;

    error = lastKnownpositioner.Open(PosServer());
    //request for lastknown position update and wait for the request to complete
    if (error == KErrNone) {
        TPositionInfo posInfo;
        lastKnownpositioner.SetRequestor(CRequestor::ERequestorService ,
                                         CRequestor::EFormatApplication, _L("QTmobility_Location"));
        lastKnownpositioner.GetLastKnownPosition(posInfo, status);

        //Sub-optimal implementation inherited from the previous implementation
        //Since this is a 'once in a while' usecase, this should be ok
        //An optimal implementation will be much more complex and will need changes
        //in the symbian locaiton stack
        User::WaitForRequest(status);

        error = status.Int();
        lastKnownpositioner.Close();

        if ((error == KErrNone) || (error == KPositionPartialUpdate)) {
            TPositionModuleInfo modInfo;
            iPosServer.GetModuleInfoById(posInfo.ModuleId(), modInfo);

            if (!aFromSatellitePositioningMethodsOnly ||
                    (aFromSatellitePositioningMethodsOnly && (modInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite))) {
                PsyUtils::TPositionInfo2QGeoPositionInfo(posInfo, posUpdate);
            }
        }
    }
    return posUpdate;
}

void PsyUtils::TPositionInfo2QGeoPositionInfo(TPositionInfoBase &aPosInfoBase, QGeoPositionInfo& aQPosInfo)
{
    if (aPosInfoBase.PositionClassType() & EPositionInfoClass) {
        TPositionInfo *posInfo = static_cast<TPositionInfo*>(&aPosInfoBase);
        TPosition pos;
        QGeoCoordinate  coord;

        posInfo->GetPosition(pos);
        coord.setLatitude(pos.Latitude());
        coord.setLongitude(pos.Longitude());
        coord.setAltitude(pos.Altitude());

        //store the QGeoCoordinate values
        aQPosInfo.setCoordinate(coord);

        TDateTime datetime = pos.Time().DateTime();
        QDateTime dt(QDate(datetime.Year() , datetime.Month() + 1, datetime.Day() + 1),
                     QTime(datetime.Hour() , datetime.Minute(), datetime.Second(),
                           datetime.MicroSecond() / 1000),
                     Qt::UTC);

        //store the time stamp
        aQPosInfo.setTimestamp(dt);

        //store the horizontal accuracy
        aQPosInfo.setAttribute(QGeoPositionInfo::HorizontalAccuracy, pos.HorizontalAccuracy());

        //store the vertical accuracy
        aQPosInfo.setAttribute(QGeoPositionInfo::VerticalAccuracy, pos.VerticalAccuracy());
    }

    if (aPosInfoBase.PositionClassType() & EPositionGenericInfoClass) {
        HPositionGenericInfo *genInfo = static_cast<HPositionGenericInfo*>(&aPosInfoBase);
        float val;
        //check for the horizontal speed
        if (genInfo->IsFieldAvailable(EPositionFieldHorizontalSpeed)) {
            genInfo->GetValue(EPositionFieldHorizontalSpeed, val);
            aQPosInfo.setAttribute(QGeoPositionInfo::GroundSpeed, val);
        }
        //check for the vertcal speed
        if (genInfo->IsFieldAvailable(EPositionFieldVerticalSpeed)) {
            genInfo->GetValue(EPositionFieldVerticalSpeed, val);
            aQPosInfo.setAttribute(QGeoPositionInfo::VerticalSpeed, val);
        }

        //check for the magnetic variation
        if (genInfo->IsFieldAvailable(EPositionFieldMagneticCourseError)) {
            genInfo->GetValue(EPositionFieldMagneticCourseError, val);
            aQPosInfo.setAttribute(QGeoPositionInfo::MagneticVariation, val);
        }

        //check for the heading
        if (genInfo->IsFieldAvailable(EPositionFieldHeading)) {
            genInfo->GetValue(EPositionFieldHeading, val);
            aQPosInfo.setAttribute(QGeoPositionInfo::Direction, val);
        }
    }
}

QTM_END_NAMESPACE
