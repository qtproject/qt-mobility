/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <e32debug.h>

#include "symbian_lbsfacade_p.h"
#include <lbssatellite.h>

QTM_BEGIN_NAMESPACE

CSelfManagingPsy::CSelfManagingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer,
                                   MPsyRequestCallback* aRequestCallback, TPositionInfoBase &aPos)
        : CActive(EPriorityNormal), iContainer(aContainer), iPositioner(NULL),
        iPsyModInfo(aPsy), iRequestCallback(aRequestCallback), iPosInfo(aPos),
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
}

void CSelfManagingPsy::HandlePositionCallback(TRequestStatus &aStatus)
{
    switch (aStatus.Int()) {
    case KErrNone:
        qDebug() << "CSelfManagingPsy::HandlePositionCallback - Good fix " << iPsyName ;
        iRequestCallback->LocationUpdate(iPosInfo, this);
        break;
    default:
        qDebug() << "CSelfManagingPsy::HandlePositionCallback - ERROR " << iPsyName << " Err=" << iStatus.Int() ;
        iRequestCallback->RequestTimedOut(this);
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
        CSelfManagingPsy::IssueRequest(iUpdInterval, iTimeout);
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

void CSelfManagingPsy::IssueRequest(TTimeIntervalMicroSeconds aUpdInterval, TTimeIntervalMicroSeconds aTimeOut)
{
    qDebug() << "CSelfManagingPsy::IssueRequest " << iPsyName  ;

    if (IsActive())
        Cancel();

    TPositionQuality quality;
    iPsyModInfo->GetPositionQuality(quality);
    if (aUpdInterval < quality.TimeToNextFix())
        aUpdInterval = quality.TimeToNextFix();

    if (aTimeOut < quality.TimeToFirstFix())
        aTimeOut = quality.TimeToFirstFix();

    iUpdInterval = aUpdInterval;
    iTimeout = aTimeOut;

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
void CSelfManagingPsy::RequestPositionAgain()
{
    qDebug() << "CSelfManagingPsy::RequestPositionAgain" << iPsyName  ;
    if (iPsyAvailabilityStatus) {
        iPositioner->NotifyPositionUpdate(iPosInfo, iStatus);
        SetActive();
    }
}
void CSelfManagingPsy::CancelPositionRequest()
{
    qDebug() << "CSelfManagingPsy::CancelPositionRequest" << iPsyName;
    iClientRequestActive = EFalse;
    if (iPositioner)
        iPositioner->CancelRequest(EPositionerNotifyPositionUpdate);
}
TInt CSelfManagingPsy::RunError()
{
    qDebug() << "CSelfManagingPsy::RunError" << iPsyName;
    //IGNORE
    return KErrNone;

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
    iAllPsys.Close();


    for (int j = 0; j < iPsyStatusListeners.Count(); j++)
        delete iPsyStatusListeners[j];
    iPsyStatusListeners.Close();

    delete iPsyModules;
    iPosServer.Close();
}

CPsyContainer::CPsyContainer() : iNWCallback(NULL), iGPSCallback(NULL), iPsyModules(NULL)
{}

void CPsyContainer::ConstructL()
{
    User::LeaveIfError(iPosServer.Connect());
    //Get all modules and store them
    InitAllPsysL();

    iPsyModules = CPosModules::OpenL();
    iPsyModules->SetObserverL(*this);

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
        if (PsyUtils::IsSatellitePsy(modInfo))
            if (iGPSCallback)
                iGPSCallback->PsyUninstalled(modId);
            else /*if(IsNWPsy)*/
                if (iNWCallback)
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

        if (PsyUtils::IsSatellitePsy(modInfo))
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

void CPsyContainer::SetNonSatellitePsyCountChangeCallback(MPsyCountChangeCallback *aNWCallback)
{
    qDebug() << "CPsyContainer::SetNWPSYChangeCallback"  ;
    iNWCallback = aNWCallback;
}

void CPsyContainer::SetSatellitePsyCountChangeCallback(MPsyCountChangeCallback *aGPSCallback)
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
    if (aPosInfoBase.PositionClassType() & EPositionInfoClass  ||
            aPosInfoBase.PositionClassType() & EPositionSatelliteInfoClass) {
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

        if (aPosInfoBase.PositionClassType() & EPositionSatelliteInfoClass) {
            TCourse course;
            TPositionSatelliteInfo *satInfo = static_cast<TPositionSatelliteInfo*>(&aPosInfoBase);
            satInfo->GetCourse(course);
            aQPosInfo.setAttribute(QGeoPositionInfo::Direction, course.Heading());
            aQPosInfo.setAttribute(QGeoPositionInfo::GroundSpeed, course.Speed());
            aQPosInfo.setAttribute(QGeoPositionInfo::VerticalSpeed, course.VerticalSpeed());
        }
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
TBool PsyUtils::IsSatellitePsy(TPositionModuleInfo *aModInfo)
{
    if (!aModInfo)
        return EFalse;

    TPositionModuleInfo::TTechnologyType ttype = aModInfo->TechnologyType();
    TPositionModuleInfo::TCapabilities capabilities = aModInfo->Capabilities();

    if (capabilities  & TPositionModuleInfo::ECapabilitySatellite)
        return ETrue;

    return EFalse;
}

QTM_END_NAMESPACE
