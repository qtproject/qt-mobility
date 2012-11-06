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

#include <QDebug>

//#include <lbs.h>
#include "clbstrackingtequestors_60_p.h"

QTM_BEGIN_NAMESPACE

const TInt64 MIN_GPS_UPD_INTERVAL = 1000000; //1 sec
const TInt64 MIN_NW_UPD_INTERVAL  = 10000000; //10 sec

const TTimeIntervalMicroSeconds32 SATELLITE_TTFF = 60000000;//60 seconds
const TTimeIntervalMicroSeconds32 NW_TTFF =        20000000;//20 seconds

const TInt MAX_TIMER = 2147483647 ;


CLbsPositionTrackerBase::CLbsPositionTrackerBase(TrackingType aType,
        CPsyContainer *aContainer, MLbsTrackingCallback *aObserver) :
        iObserver(aObserver), iTrackType(aType), iContainer(aContainer),
        iLatestQPosInfo(NULL), iLastQPosInfo(NULL)
{
}

CLbsPositionTrackerBase::~CLbsPositionTrackerBase()
{
    delete iLatestQPosInfo;
    delete iLastQPosInfo;
}

void CLbsPositionTrackerBase::SetObserver(MLbsTrackingCallback *aObserver)
{
    iObserver = aObserver;
}
TMilliSeconds CLbsPositionTrackerBase::GetUpdateInterval() const
{
    TMilliSeconds retValue =    iUpdateInterval.Int64() / 1000;
    return retValue;
}

TMilliSeconds CLbsPositionTrackerBase::GetMinimumUpdateInterval() const
{
    TMilliSeconds retValue =    iMinUpdateInterval.Int64() / 1000;
    return retValue;
}

CLbsPositionTrackerBase::TrackingType CLbsPositionTrackerBase::GetTrackingType() const
{
    return iTrackType;
}

QGeoPositionInfo* CLbsPositionTrackerBase::GetLastPosition() const
{
    return iLastQPosInfo;
}

CLbsTrackingTimerBase::CLbsTrackingTimerBase(TrackingType aType,
        CPsyContainer *aContainer, MLbsTrackingCallback *aObserver)
        : CLbsPositionTrackerBase(aType, aContainer, aObserver),
        CActive(CActive::EPriorityStandard), iFailureCount(0),
        iTTFFTimerActive(EFalse), iStartUpdatesActive(EFalse)
{
    qDebug() << "CLbsTrackingTimerBase::CLbsTrackingTimerBase START";
    iTimerInterval.balanceDuration = 0;
    iTimerInterval.itrMaxTimer = 0;
}
void CLbsTrackingTimerBase::ConstructL()
{
    iTimer.CreateLocal();
    CActiveScheduler::Add(this);
}
void CLbsTrackingTimerBase::InitializePSYs(RArray<TPositionModuleInfo*>  aModInfoArray, const TInt64 aDefaultMinInterval)
{
    qDebug() << "CLbsTrackingTimerBase::InitializePSYs START " << iTrackType ;

    TInt insertCount = 0;
    iMinUpdateInterval = aDefaultMinInterval;

    for (int i = 0; i < aModInfoArray.Count(); i++) {

        //The PSYs are self managing, no need for this check
        //if(aModInfoArray[i]->IsAvailable())
        //    {
        TPositionQuality quality;
        aModInfoArray[i]->GetPositionQuality(quality);

        if (0 == i)
            iMinUpdateInterval = quality.TimeToNextFix();
        else if (iMinUpdateInterval > quality.TimeToNextFix())
            iMinUpdateInterval = quality.TimeToNextFix();

        CTrackingPsy *trackingPsy = CTrackingPsy::NewL(aModInfoArray[i], iContainer, this);
        if (trackingPsy) {
            iTrackingPsys.Insert(trackingPsy, insertCount++);
            qDebug() << "CLbsTrackingTimerBase::InitializePSYs inserted " << trackingPsy->iPsyName ;
        }
        //  }

    }
    qDebug() << "CLbsTrackingTimerBase::InitializePSYs END - insertcount = " << insertCount << " type" << iTrackType;
}
CLbsTrackingTimerBase::~CLbsTrackingTimerBase()
{
    qDebug() << "CLbsTrackingTimerBase::~CLbsTrackingTimerBase " << iTrackType;
    Cancel();

    for (int i = 0; i < iTrackingPsys.Count(); i++)
        delete iTrackingPsys[i];

    iTrackingPsys.Close();


    iTimer.Close();
}

void CLbsTrackingTimerBase::SetUpdateInterval(TMilliSeconds aIntr)
{
    qDebug() << "CLbsTrackingTimerBase::SetUpdateInterval START " << iTrackType;

    TTimeIntervalMicroSeconds intr(aIntr*1000);

    iUpdateInterval = (intr > iMinUpdateInterval) ? intr : iMinUpdateInterval;

    if (iUpdateInterval.Int64() > MAX_TIMER) {
        iTimerInterval.itrMaxTimer = iUpdateInterval.Int64() / MAX_TIMER;
        iTimerInterval.balanceDuration = iUpdateInterval.Int64() % MAX_TIMER;
    } else {
        iTimerInterval.balanceDuration = iUpdateInterval.Int64();
        iTimerInterval.itrMaxTimer = 0;
    }

    qDebug() << "CLbsTrackingTimerBase::SetUpdateInterval MaxItr =  " << iTimerInterval.itrMaxTimer
    << "balanceDuration=" << iTimerInterval.balanceDuration.Int();
}

bool CLbsTrackingTimerBase::StartTimer()
{
    qDebug() << "CLbsTrackingTimerBase::StartTimer  " << iTrackType;
    bool sendClientUpdate = EFalse;

    iTTFFTimerActive = EFalse;


    if (iTimerInterval.itrMaxTimer > 0) {
        iTimer.After(iStatus, MAX_TIMER);
        iTimerInterval.itrMaxTimer--;

        //qDebug() << "CLbsTrackingTimerBase::StartTimer 1 "<<iTrackType;

        if (!IsActive()) {
            // qDebug() << "CLbsTrackingTimerBase::StartTimer 2 "<<iTrackType;
            SetActive();
            //qDebug() << "CLbsTrackingTimerBase::StartTimer 3 "<<iTrackType;
        }
        qDebug() << "CLbsTrackingTimerBase::StartTimer 4 " << iTrackType;
    } else if (iTimerInterval.balanceDuration.Int() > 0) {
        //qDebug() << "CLbsTrackingTimerBase::StartTimer 5 "<<iTrackType;
        iTimer.After(iStatus, iTimerInterval.balanceDuration);
        iTimerInterval.balanceDuration = 0;
        if (!IsActive()) {
            // qDebug() << "CLbsTrackingTimerBase::StartTimer 6 "<<iTrackType;
            SetActive();
            //qDebug() << "CLbsTrackingTimerBase::StartTimer 7 "<<iTrackType;
        }
        //qDebug() << "CLbsTrackingTimerBase::StartTimer 8 "<<iTrackType;
    } else {
        // qDebug() << "CLbsTrackingTimerBase::StartTimer 9 "<<iTrackType;
        //Start from the beginning
        SetUpdateInterval(iUpdateInterval.Int64() / 1000);
        sendClientUpdate = ETrue;
        StartTimer();
    }
    return sendClientUpdate;
}

void CLbsTrackingTimerBase::StartUpdates()
{
    qDebug() << "CLbsTrackingTimerBase::StartUpdates - Start " << iTrackType;

    Cancel();

    iStartUpdatesActive = ETrue;
    iTimer.After(iStatus, iTTFFTimeOut);
    SetActive();

    iTTFFTimerActive = ETrue;

    int count = 0;
    for (int i = 0; i < iTrackingPsys.Count(); i++) {
        iTrackingPsys[i]->IssueRequest(iUpdateInterval);
        count++;
    }
    qDebug() << "CLbsTrackingTimerBase::StartUpdates - END requestCount = " << count ;
}

void CLbsTrackingTimerBase::StopUpdates()
{
    qDebug() << "CLbsTrackingTimerBase::StopUpdates  " << iTrackType;
    Cancel();
    iTTFFTimerActive = EFalse;
    iStartUpdatesActive = EFalse;

    for (int i = 0; i < iTrackingPsys.Count(); i++)
        iTrackingPsys[i]->Cancel();
    qDebug() << "CLbsTrackingTimerBase::StopUpdates  2" << iTrackType;
}


void CLbsTrackingTimerBase::LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy)
{
    qDebug() << "CLbsTrackingTimerBase::LocationUpdate from PSY " << aPsy->iPsyName << "Type=" << iTrackType ;

    delete iLatestQPosInfo;
    iLatestQPosInfo = new(ELeave) QGeoPositionInfo;
    PsyUtils::TPositionInfo2QGeoPositionInfo(aPosition, *iLatestQPosInfo);

    //IF this is the first location after Start updates then
    //1) Send the location to the client
    //2) Cancel the TTFF timer and start the regular update interval timer
    if (iTTFFTimerActive) {

        qDebug() << "CLbsTrackingTimerBase::LocationUpdate from PSY " << aPsy->iPsyName << "FIRST UPDATE" ;

        //Cancel TTFF timer and start update interval timer
        iTimer.Cancel();
        Cancel();
        StartTimer();
        iObserver->TrackingLocation(iLatestQPosInfo, this);
        delete iLatestQPosInfo;
        iLatestQPosInfo = NULL;
    }
}

void CLbsTrackingTimerBase::RequestTimedOut(TInt aConsecutiveFailureCount, CSelfManagingPsy *aPsy)
{
    qDebug() << "CLbsTrackingTimerBase::RequestTimedOut from PSY " << aPsy->iPsyName << "Type=" << iTrackType ;
    //IGNORE, nothing to do here. It will be handled in RunL
}

void CLbsTrackingTimerBase::RunL()
{
    qDebug() << "CLbsTrackingTimerBase::RunL START " << iTrackType;

    if (iTTFFTimerActive) {
        //If we reache here then position was not calculated with in the TTFF time out
        iObserver->TrackingRequestTimedOut(this);
        //Setting it to 5 to ensure that two timeouts are not sent consequitively
        iFailureCount = 5;
        iTTFFTimerActive = EFalse;
    }

    if (StartTimer()) {
        if (iLatestQPosInfo) {
            qDebug() << "CLbsTrackingTimerBase::RunL SENDING UPDATE " << iTrackType;
            iObserver->TrackingLocation(iLatestQPosInfo, this);
            iLastQPosInfo = iLatestQPosInfo;
            iLatestQPosInfo = NULL;
            iFailureCount = 0;
        } else {
            iFailureCount++;
            qDebug() << "CLbsTrackingTimerBase::RunL FAILED " << iFailureCount << "Type=" << iTrackType;
        }

        if (iFailureCount == 5) //TODO Adjust count differently for GPS and NW
            iObserver->TrackingRequestTimedOut(this);
    }
}

void CLbsTrackingTimerBase::PsyUninstalled(TPositionModuleId &aModId)
{
    for (int i = 0; i < iTrackingPsys.Count(); i++)
        if (iTrackingPsys[i]->GetModInfo()->ModuleId() == aModId) {
            delete iTrackingPsys[i];
            iTrackingPsys.Remove(i);
            break;
        }

}
void CLbsTrackingTimerBase::PsyInstalled(TPositionModuleInfo* aModInfo)
{
    TPositionQuality quality;
    aModInfo->GetPositionQuality(quality);

    if (0 == iTrackingPsys.Count())
        iMinUpdateInterval = quality.TimeToNextFix();
    else if (iMinUpdateInterval > quality.TimeToNextFix())
        iMinUpdateInterval = quality.TimeToNextFix();

    CTrackingPsy *trackingPsy = CTrackingPsy::NewL(aModInfo, iContainer, this);
    if (trackingPsy) {
        iTrackingPsys.Insert(trackingPsy, iTrackingPsys.Count());

        if (iStartUpdatesActive) {
            qDebug() << "CLbsTrackingTimerBase::PsyInstalled inserted & request issued " << trackingPsy->iPsyName ;
            trackingPsy->IssueRequest(iUpdateInterval);
        } else {
            qDebug() << "CLbsTrackingTimerBase::PsyInstalled inserted " << trackingPsy->iPsyName ;
        }
    }
}

void CLbsTrackingTimerBase::DoCancel()
{
    qDebug() << "CLbsTrackingTimerBase::DoCancel " << iTrackType;

    iTimer.Cancel();
    iStartUpdatesActive = EFalse;
    iTTFFTimerActive = EFalse;

    qDebug() << "CLbsTrackingTimerBase::DoCancel 2" << iTrackType;
}


CLbsSatelliteTracker* CLbsSatelliteTracker::NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver)
{
    qDebug() << "CLbsSatelliteTracker::NewL ";
    CLbsSatelliteTracker *tracker = new(ELeave)CLbsSatelliteTracker(aContainer, aObserver);
    tracker->ConstructL();
    return tracker;
}

CLbsSatelliteTracker::~CLbsSatelliteTracker()
{
    qDebug() << "CLbsSatelliteTracker::~CLbsSatelliteTracker " << iTrackType;

}

CLbsSatelliteTracker::CLbsSatelliteTracker(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver)
        : CLbsTrackingTimerBase(GPSOnly, aContainer, aObserver)
{
    qDebug() << "CLbsSatelliteTracker::CLbsSatelliteTracker " << iTrackType;
    iTTFFTimeOut = SATELLITE_TTFF;
}

void CLbsSatelliteTracker::ConstructL()
{
    qDebug() << "CLbsSatelliteTracker::ConstructL - START " << iTrackType  ;

    CLbsTrackingTimerBase::ConstructL();

    //Initialize PSYs
    RArray<TPositionModuleInfo*>  modInfoArray;
    iContainer->GetAvailablePsys(EFalse, modInfoArray);

    CLbsTrackingTimerBase::InitializePSYs(modInfoArray, MIN_GPS_UPD_INTERVAL);

    modInfoArray.Close();

    SetUpdateInterval(iMinUpdateInterval.Int64() / 1000);

    //Register for callbacks
    iContainer->SetGPSPSYChangeCallback(this);
    qDebug() << "CLbsSatelliteTracker::ConstructL - End " << iTrackType  ;
}






CLbsNetworkTracker* CLbsNetworkTracker::NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver)
{
    qDebug() << "CLbsNetworkTracker::NewL ";
    CLbsNetworkTracker *tracker = new(ELeave)CLbsNetworkTracker(aContainer, aObserver);
    tracker->ConstructL();
    return tracker;
}

CLbsNetworkTracker::~CLbsNetworkTracker()
{
    qDebug() << "CLbsNetworkTracker::~CLbsSNetworkTracker " << iTrackType;

}

CLbsNetworkTracker::CLbsNetworkTracker(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver)
        : CLbsTrackingTimerBase(NWOnly, aContainer, aObserver)
{
    qDebug() << "CLbsNetworkTracker::CLbsSNetworkTracker " << iTrackType;
    iTTFFTimeOut = NW_TTFF;
}

void CLbsNetworkTracker::ConstructL()
{
    qDebug() << "CLbsNetworkTracker::ConstructL - START " << iTrackType  ;

    CLbsTrackingTimerBase::ConstructL();

    //Initialize PSYs
    RArray<TPositionModuleInfo*>  modInfoArray;
    iContainer->GetAvailablePsys(ETrue, modInfoArray);


    CLbsTrackingTimerBase::InitializePSYs(modInfoArray, MIN_NW_UPD_INTERVAL);

    modInfoArray.Close();

    SetUpdateInterval(iMinUpdateInterval.Int64() / 1000);

    //Register for callbacks
    iContainer->SetNWPSYChangeCallback(this);
    qDebug() << "CLbsNetworkTracker::ConstructL - End "  ;
}


class CLbsAllPosProxyState //base class
{

public:
    virtual void StartUpdates() = 0;
    virtual void StopUpdates() = 0;
    virtual void SatelliteFixReceived(QGeoPositionInfo *aPosition) = 0;
    virtual void SatelliteFixTimedOut() = 0;
    virtual void NWFixReceived(QGeoPositionInfo *aPosition) = 0;
    virtual void NWFixTimedOut() = 0;

    virtual void EnterState() = 0;

protected:

    CLbsAllPosProxyState(CLbsAllPosProxy *aTracker): iTracker(aTracker) {}



    CLbsAllPosProxy *iTracker;
};


class CLbsAllPosIdle : public CLbsAllPosProxyState
{
public:
    static CLbsAllPosIdle* NewL(CLbsAllPosProxy *aTracker) {
        CLbsAllPosIdle *idle = new(ELeave) CLbsAllPosIdle(aTracker);
        return idle;
    }
protected:
    CLbsAllPosIdle(CLbsAllPosProxy *aTracker) : CLbsAllPosProxyState(aTracker) {}

    void StartUpdates() {
        qDebug() << "CLbsAllPosIdle::StartUpdates";
        iTracker->SetNextState(iTracker->iInitialHybrid);
    }
    void StopUpdates() {
        qDebug() << "CLbsAllPosIdle::StopUpdates";
        //Nothing to do

    }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosIdle::SatelliteFixReceived ERROR";
        //should never happen
    }
    void SatelliteFixTimedOut() {
        qDebug() << "CLbsAllPosIdle::SatelliteFixTimedOut ERROR";
        //should never happen
    }
    void NWFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosIdle::NWFixReceived ERROR";
        //should never happen
    }
    void NWFixTimedOut() {
        qDebug() << "CLbsAllPosIdle::NWFixTimedOut ERROR";
        //should never happen
    }
    void EnterState() {
        qDebug() << "CLbsAllPosIdle::EnterState";
        iTracker->iSatelliteTracker->StopUpdates();
        iTracker->iNWTracker->StopUpdates();
    }


};

class CLbsAllPosInitialHybrid : public CLbsAllPosProxyState
{
public:
    static CLbsAllPosInitialHybrid* NewL(CLbsAllPosProxy *aTracker) {
        CLbsAllPosInitialHybrid *state = new(ELeave) CLbsAllPosInitialHybrid(aTracker);
        return state;
    }
protected:
    CLbsAllPosInitialHybrid(CLbsAllPosProxy *aTracker) :
            CLbsAllPosProxyState(aTracker), iSatelliteTrackerTimedOut(EFalse),
            iNWTrackerTimedOut(EFalse), iTimeOutSent(EFalse) {}

    void StartUpdates() {
        qDebug() << "CLbsAllPosInitialHybrid::StartUpdates";
        //Nothing to do. The trackers are alredy started
    }
    void StopUpdates() {
        qDebug() << "CLbsAllPosInitialHybrid::StopUpdates";
        iTracker->SetNextState(iTracker->iIdle);
    }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosInitialHybrid::SatelliteFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iTracker->SetNextState(iTracker->iSatelliteOnly);
    }
    void SatelliteFixTimedOut() {
        qDebug() << "CLbsAllPosInitialHybrid::SatelliteFixTimedOut";
        iSatelliteTrackerTimedOut = ETrue;

        if (!iTimeOutSent)
            if (iSatelliteTrackerTimedOut && iNWTrackerTimedOut) {
                qDebug() << "CLbsAllPosInitialHybrid::SatelliteFixTimedOut NotifyTimeOutToObserver";
                iTracker->NotifyTimeOutToObserver();
                iTimeOutSent = ETrue;
            }
    }
    void NWFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosInitialHybrid::NWFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iTracker->SetNextState(iTracker->iSatelliteOnly);
    }
    void NWFixTimedOut() {
        qDebug() << "CLbsAllPosInitialHybrid::NWFixTimedOut";

        if (!iTimeOutSent)
            if (iSatelliteTrackerTimedOut && iNWTrackerTimedOut) {
                qDebug() << "CLbsAllPosInitialHybrid::NWFixTimedOut NotifyTimeOutToObserver";
                iTracker->NotifyTimeOutToObserver();
                iTimeOutSent = ETrue;
            }
    }
    void EnterState() {
        qDebug() << "CLbsAllPosInitialHybrid::EnterState";
        iTracker->iSatelliteTracker->StartUpdates();
        iTracker->iNWTracker->StartUpdates();
    }

private:
    TBool iSatelliteTrackerTimedOut;
    TBool iNWTrackerTimedOut;
    TBool iTimeOutSent;

};
class CLbsAllPosSatelliteOnly : public CLbsAllPosProxyState
{
public:
    static CLbsAllPosSatelliteOnly* NewL(CLbsAllPosProxy *aTracker) {
        CLbsAllPosSatelliteOnly *state = new(ELeave) CLbsAllPosSatelliteOnly(aTracker);
        return state;
    }
protected:
    CLbsAllPosSatelliteOnly(CLbsAllPosProxy *aTracker): CLbsAllPosProxyState(aTracker) {}

    void StartUpdates() {
        qDebug() << "CLbsAllPosSatelliteOnly::StartUpdates";
        //Nothing to do. Satellite tracker is already running
    }
    void StopUpdates() {
        qDebug() << "CLbsAllPosSatelliteOnly::StopUpdates";
        iTracker->SetNextState(iTracker->iIdle);
    }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosSatelliteOnly::SatelliteFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
    }
    void SatelliteFixTimedOut() {
        qDebug() << "CLbsAllPosSatelliteOnly::SatelliteFixTimedOut";
        iTracker->NotifyTimeOutToObserver();
        iTracker->SetNextState(iTracker->iRunningHybrid);
    }
    void NWFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosSatelliteOnly::NWFixReceived ERROR";
        //can not happen
    }
    void NWFixTimedOut() {
        qDebug() << "CLbsAllPosSatelliteOnly::NWFixTimedOut ERROR";
        //Can not happen
    }
    void EnterState() {
        qDebug() << "CLbsAllPosSatelliteOnly::EnterState";
        iTracker->iNWTracker->StopUpdates();
        qDebug() << "CLbsAllPosSatelliteOnly::EnterState 2";
        //SatelliteTracker is already started so no need to start it here.
    }

};

class CLbsAllPosRunningHybrid : public CLbsAllPosProxyState
{
public:
    static CLbsAllPosRunningHybrid* NewL(CLbsAllPosProxy *aTracker) {
        CLbsAllPosRunningHybrid *state = new(ELeave) CLbsAllPosRunningHybrid(aTracker);
        return state;
    }
protected:
    CLbsAllPosRunningHybrid(CLbsAllPosProxy *aTracker) : CLbsAllPosProxyState(aTracker) {}

    void StartUpdates() {
        qDebug() << "CLbsAllPosRunningHybrid::StartUpdates";
        //Nothing to do. Both trackers are already running
    }
    void StopUpdates() {
        qDebug() << "CLbsAllPosRunningHybrid::StopUpdates";
        iTracker->SetNextState(iTracker->iIdle);
    }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosRunningHybrid::SatelliteFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iLastNotificationIsTimeOut = EFalse;
        iTracker->SetNextState(iTracker->iSatelliteOnly);
    }
    void SatelliteFixTimedOut() {
        qDebug() << "CLbsAllPosRunningHybrid::SatelliteFixTimedOut";
        //Nothing to do
    }
    void NWFixReceived(QGeoPositionInfo *aPosition) {
        qDebug() << "CLbsAllPosRunningHybrid::NWFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iLastNotificationIsTimeOut = EFalse;
    }
    void NWFixTimedOut() {
        qDebug() << "CLbsAllPosRunningHybrid::NWFixTimedOut";

        if (!iLastNotificationIsTimeOut) {
            iTracker->NotifyTimeOutToObserver();
            iLastNotificationIsTimeOut = ETrue;
        }
    }
    void EnterState() {
        qDebug() << "CLbsAllPosRunningHybrid::EnterState";
        iTracker->iNWTracker->StartUpdates();
        //SatelliteTracker is already started so no need to start it again.

        iLastNotificationIsTimeOut = ETrue;
    }
private:
    TBool iLastNotificationIsTimeOut;
};

CLbsAllPosProxy* CLbsAllPosProxy::NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver,
                                       CLbsSatelliteTracker *aSatelliteTracker, CLbsNetworkTracker *aNWTracker)
{
    CLbsAllPosProxy *tracker = new(ELeave) CLbsAllPosProxy(aContainer, aObserver, aSatelliteTracker, aNWTracker);
    TRAPD(err, tracker->ConstructL(aSatelliteTracker, aNWTracker));

    if (err == KErrNone)
        return tracker;

    delete tracker;
    return NULL;
}

CLbsAllPosProxy::~CLbsAllPosProxy()
{
    qDebug() << "CLbsAllPosProxy::~CLbsAllPosProxy";
    delete iSatelliteTracker;
    delete iNWTracker;
    delete iIdle;
    delete iInitialHybrid;
    delete iSatelliteOnly;
    delete iRunningHybrid;
}

CLbsAllPosProxy::CLbsAllPosProxy(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver,
                                 CLbsSatelliteTracker *aSatelliteTracker, CLbsNetworkTracker *aNWTracker)
        : CLbsPositionTrackerBase(AllPsys, aContainer, aObserver),
        iSatelliteTracker(aSatelliteTracker), iNWTracker(aNWTracker)
{

}

void CLbsAllPosProxy::ConstructL(CLbsSatelliteTracker *aSatelliteTracker , CLbsNetworkTracker *aNWTracker)
{

    if (aSatelliteTracker == NULL)
        iSatelliteTracker = CLbsSatelliteTracker::NewL(iContainer, this);
    else {
        iSatelliteTracker = aSatelliteTracker;
        iSatelliteTracker->SetObserver(this);
    }

    if (aNWTracker == NULL)
        iNWTracker = CLbsNetworkTracker::NewL(iContainer, this);
    else {
        iNWTracker = aNWTracker;
        iNWTracker->SetObserver(this);
    }

    TMilliSeconds minInterval = (iSatelliteTracker->GetMinimumUpdateInterval()  < iNWTracker->GetMinimumUpdateInterval()) ?
                                iSatelliteTracker->GetMinimumUpdateInterval() : iNWTracker->GetMinimumUpdateInterval();

    iMinUpdateInterval = minInterval * 1000;
    SetUpdateInterval(minInterval);

    //Initialize states
    iIdle = CLbsAllPosIdle::NewL(this);
    iInitialHybrid = CLbsAllPosInitialHybrid::NewL(this);
    iSatelliteOnly = CLbsAllPosSatelliteOnly::NewL(this);
    iRunningHybrid = CLbsAllPosRunningHybrid::NewL(this);

    iCurrentState = iIdle;
}

void CLbsAllPosProxy::SetUpdateInterval(TMilliSeconds mSec)
{
    iSatelliteTracker->SetUpdateInterval(mSec);
    iNWTracker->SetUpdateInterval(mSec);
}

void CLbsAllPosProxy::SetNextState(CLbsAllPosProxyState *nextState)
{
    iCurrentState = nextState;
    iCurrentState->EnterState();
}

void CLbsAllPosProxy::StartUpdates()
{
    iCurrentState->StartUpdates();
}
void CLbsAllPosProxy::StopUpdates()
{
    iCurrentState->StopUpdates();
}
void CLbsAllPosProxy::TrackingLocation(QGeoPositionInfo *aPosition, CLbsPositionTrackerBase *aTracker)
{
    if (GPSOnly == aTracker->GetTrackingType())
        iCurrentState->SatelliteFixReceived(aPosition);
    else
        iCurrentState->NWFixReceived(aPosition);
}
void CLbsAllPosProxy::TrackingRequestTimedOut(CLbsPositionTrackerBase *aTracker)
{
    if (GPSOnly == aTracker->GetTrackingType())
        iCurrentState->SatelliteFixTimedOut();
    else
        iCurrentState->NWFixTimedOut();
}
void CLbsAllPosProxy::NotifyFixToObserver(QGeoPositionInfo *aPosition)
{
    iLatestQPosInfo = aPosition;
    iObserver->TrackingLocation(iLatestQPosInfo, this);
    iLastQPosInfo = iLatestQPosInfo;
}
void CLbsAllPosProxy::NotifyTimeOutToObserver()
{
    iObserver->TrackingRequestTimedOut(this);
}

CLbsSatelliteTracker* CLbsAllPosProxy::ReleaseSatelliteTracker()
{
    CLbsSatelliteTracker *tracker = iSatelliteTracker;
    iSatelliteTracker = NULL;
    return tracker;
}

CLbsNetworkTracker* CLbsAllPosProxy::ReleaseNWTracker()
{
    CLbsNetworkTracker *tracker = iNWTracker;
    iNWTracker = NULL;
    return tracker;
}

CLbsSingleShotRequestor* CLbsSingleShotRequestor::NewL(CPsyContainer *aContainer,
        MLbsSingleShotCallback *aObserver, SSType aSSType)
{
    qDebug() << "CLbsSingleShotRequestor::NewL " << aSSType;
    CLbsSingleShotRequestor *requestor = new(ELeave) CLbsSingleShotRequestor(aContainer, aObserver, aSSType);
    TRAPD(err, requestor->ConstructL());
    if (err == KErrNone)
        return requestor;

    delete requestor;
    return NULL;
}
CLbsSingleShotRequestor::CLbsSingleShotRequestor(CPsyContainer *aContainer, MLbsSingleShotCallback *aObserver,
        SSType aSSType): CTimer(CActive::EPriorityStandard),
        iContainer(aContainer), iObserver(aObserver), iSSType(aSSType), iLatestQPosInfo(NULL)
{
}

void CLbsSingleShotRequestor::ConstructL()
{
    CTimer::ConstructL();
    CActiveScheduler::Add(this);

    //Initialize PSYs
    RArray<TPositionModuleInfo*>  modInfoArray;
    switch (iSSType) {
    case SS_GPSOnly:
        iContainer->GetAvailablePsys(EFalse, modInfoArray);
        break;
    case SS_NWOnly:
        iContainer->GetAvailablePsys(ETrue, modInfoArray);
        break;

    case SS_AllPsys:
        iContainer->GetAvailablePsys(EFalse, modInfoArray);
        iContainer->GetAvailablePsys(ETrue, modInfoArray);
        break;
    }

    TInt insertCount = 0;
    for (int i = 0; i < modInfoArray.Count(); i++) {
        if (modInfoArray[i]->IsAvailable()) {
            CSingleShotPsy *psy = CSingleShotPsy::NewL(modInfoArray[i], iContainer, this);
            if (psy) {
                iSSPsys.Insert(psy, insertCount++);
                qDebug() << "CLbsSingleShotRequestor::InitializePSYs inserted " << psy->iPsyName ;
                break;//TODO: Hack for the SUPL issue
            }
        }
    }
    qDebug() << "CLbsSingleShotRequestor::ConstructL " << "insertCount=" << insertCount;
}

CLbsSingleShotRequestor::~CLbsSingleShotRequestor()
{
    Cancel();

    for (int i = 0; i < iSSPsys.Count(); i++)
        delete iSSPsys[i];

    iSSPsys.Close();
}

void CLbsSingleShotRequestor::Cancel()
{
    for (int i = 0; i < iSSPsys.Count(); i++)
        iSSPsys[i]->Cancel();
    CTimer::Cancel();
}

void CLbsSingleShotRequestor::DoCancel()
{



}
void CLbsSingleShotRequestor::RunL()
{
    qDebug() << "CLbsSingleShotRequestor::RunL " << iSSType;

    if (StartTimer()) {
        if (iLatestQPosInfo)
            iObserver->SSLocation(iLatestQPosInfo, this);
        else
            iObserver->SSRequestTimedOut(this);

        //Cancel all outstanding PSY requests
        Cancel();
    }
}

void CLbsSingleShotRequestor::LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy)
{
    qDebug() << "CLbsSingleShotRequestor::LocationUpdate from PSY " << aPsy->iPsyName << "Type=" << iSSType ;
    delete iLatestQPosInfo;
    iLatestQPosInfo = new(ELeave) QGeoPositionInfo;
    PsyUtils::TPositionInfo2QGeoPositionInfo(aPosition, *iLatestQPosInfo);
}
void CLbsSingleShotRequestor::RequestTimedOut(TInt aConsecutiveFailureCount, CSelfManagingPsy *aPsy)
{
    qDebug() << "CLbsSingleShotRequestor::RequestTimedOut from PSY " << aPsy->iPsyName << "Type=" << iSSType ;
    //Nothing to do, handled in RunL
}

void CLbsSingleShotRequestor::RequestLocation(TMilliSeconds &aMsec)
{
    Cancel();

    TTimeIntervalMicroSeconds intr(aMsec*1000);

    if (intr.Int64() > MAX_TIMER) {
        iTimerInterval.itrMaxTimer = intr.Int64() / MAX_TIMER;
        iTimerInterval.balanceDuration = intr.Int64() % MAX_TIMER;
    } else {
        iTimerInterval.balanceDuration = intr.Int64();
        iTimerInterval.itrMaxTimer = 0;
    }

    for (int i = 0; i < iSSPsys.Count(); i++) {
        iSSPsys[i]->IssueRequest(aMsec);

    }
    StartTimer();
}

bool CLbsSingleShotRequestor::StartTimer()
{

    bool sendClientUpdate = EFalse;

    if (iTimerInterval.itrMaxTimer > 0) {
        CTimer::After(MAX_TIMER);
        iTimerInterval.itrMaxTimer--;

    } else if (iTimerInterval.balanceDuration.Int() > 0) {
        CTimer::After(iTimerInterval.balanceDuration);
        iTimerInterval.balanceDuration = 0;
    } else {
        sendClientUpdate = ETrue;
    }
    return sendClientUpdate;
}


QTM_END_NAMESPACE
