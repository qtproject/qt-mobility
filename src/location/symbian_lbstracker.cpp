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


#include "symbian_lbstracker.h"
#include "symbian_lbsfacade.h"

QTM_BEGIN_NAMESPACE

const TInt64 MIN_GPS_UPD_INTERVAL = 1000000; //1 sec
const TInt64 MIN_NW_UPD_INTERVAL  = 10000000; //10 sec

const TInt THIRTY_SECONDS = 30000000; 

const TTimeIntervalMicroSeconds32 SATELLITE_TTFF = 60000000;//60 seconds
const TTimeIntervalMicroSeconds32 NW_TTFF =        20000000;//20 seconds


//Special class for handling NW PSYs
//Gaps b/w requests will be atleast 'x' seconds
class CNWTrackingPsy : public CTrackingPsy
{
private:
CNWTrackingPsy(TPositionModuleInfo* aPsy,CPsyContainer *aContainer,
                MPsyRequestCallback* aRequestCallback): 
                CTrackingPsy(aPsy,aContainer,aRequestCallback,iPositionInfo),
                iRealRequest(ETrue)
    {
    iTimer.CreateLocal();
    }
~CNWTrackingPsy()
    {
    qDebug() << "CNWTrackingPsy::~CNWTrackingPsy" <<iPsyName ; 
    iTimer.Close();
    }
void DoCancel()
{
    qDebug() << "CSelfManagingPsy::DoCancel "<<iPsyName;
    iTimer.Cancel();
    CancelPositionRequest();
    iClientRequestActive = EFalse;
    qDebug() << "CSelfManagingPsy::DoCancel 2"<<iPsyName;
}
void RunL()
        {
        qDebug() << "CNWTrackingPsy::RunL " <<iPsyName <<" iStatus="<<iStatus.Int();
        
        if(iRealRequest)
            {
            qDebug() << "CNWTrackingPsy::RunL SENDING POS and START TIMER" <<iPsyName;
            //A real request has completed, the next should be timer
            iRealRequest = EFalse;
            HandlePositionCallback(iStatus);
            
            TInt64 requestIntr64 = (iUpdInterval.Int64() > iTimeout.Int64()) ? 
                iUpdInterval.Int64() - iTimeout.Int64() : iUpdInterval.Int64() ;
            
            TInt requestIntr = (requestIntr64 < MAX_TIMER) ? requestIntr64 : MAX_TIMER;   
            
            if(requestIntr < THIRTY_SECONDS)
                requestIntr = THIRTY_SECONDS;
            
            //No point in retrying NW based PSYs within 30 seconds
            iTimer.After(iStatus,requestIntr);
            SetActive();
            }
        else
            {
            qDebug() << "CNWTrackingPsy::RunL REAL REQUEST" <<iPsyName;
            iRealRequest = ETrue;
            RequestPositionAgain();
            }
        }

    bool iRealRequest;
    RTimer iTimer;
    TPositionInfo iPositionInfo;
    friend class CTrackingPsy;
};

class CSatelliteTrackingPsy : public CTrackingPsy
{
private:
CSatelliteTrackingPsy(TPositionModuleInfo* aPsy,CPsyContainer *aContainer,
                MPsyRequestCallback* aRequestCallback): 
                CTrackingPsy(aPsy,aContainer,aRequestCallback,iSatelliteInfo)
    {
    }
    TPositionSatelliteInfo iSatelliteInfo;
    friend class CTrackingPsy;
};

CTrackingPsy* CTrackingPsy::NewL(TPositionModuleInfo* aPsy,CPsyContainer *aContainer,MPsyRequestCallback* aRequestCallback)
{
	CTrackingPsy *trackingPsy = NULL;
	
	if(PsyUtils::IsSatellitePsy(aPsy))
	    trackingPsy = new CSatelliteTrackingPsy(aPsy, aContainer, aRequestCallback);
	else
	    trackingPsy = new CNWTrackingPsy(aPsy, aContainer, aRequestCallback);
	
	TRAPD(err,trackingPsy->ConstructL());
	
	if(err != KErrNone)
	{
	    qDebug() << "CTrackingPsy::NewL ERROR " << "PSY= " <<trackingPsy->iPsyName  ;
		delete trackingPsy;	
		return NULL;
	}
	return trackingPsy;
}
CTrackingPsy::~CTrackingPsy()
{
    qDebug() << "CTrackingPsy::~CTrackingPsy " << "PSY= " <<iPsyName  ; 
}

void CTrackingPsy::IssueRequest(TTimeIntervalMicroSeconds updateInterval)
{
	qDebug() << "CTrackingPsy::IssueRequest - START updateInterval " <<updateInterval.Int64() << "PSY= " <<iPsyName  ;		
	
	iClientRequestActive = ETrue;
	TTimeIntervalMicroSeconds timeOut(30000000);//30 seconds
	CSelfManagingPsy::IssueRequest(updateInterval,timeOut);
}

void CTrackingPsy::RunL()
{
    //qDebug() << "CTrackingPsy::RunL " <<iPsyName <<" iStatus="<<iStatus.Int();
    HandlePositionCallback(iStatus);
    RequestPositionAgain();
}	

CTrackingPsy::CTrackingPsy(TPositionModuleInfo* aPsy,CPsyContainer *aContainer,
        MPsyRequestCallback* aRequestCallback,TPositionInfoBase &aPos)
	 : CSelfManagingPsy (aPsy,aContainer,aRequestCallback,aPos)
{
}	

void CTrackingPsy::ConstructL()
{
    qDebug() << "CTrackingPsy::ConstructL " <<iPsyName ; 
    CSelfManagingPsy::ConstructL();	
}	



CLbsPositionTrackerBase::CLbsPositionTrackerBase(TrackingType aType,
        CPsyContainer *aContainer,MLbsTrackingCallback *aObserver) :
        iObserver(aObserver),iTrackType(aType),iContainer(aContainer),
        iLatestQPosInfo(NULL),iLastQPosInfo(NULL)
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
    TMilliSeconds retValue =    iUpdateInterval.Int64()/1000;
    return retValue;    
    }

TMilliSeconds CLbsPositionTrackerBase::GetMinimumUpdateInterval() const
    {
    TMilliSeconds retValue =    iMinUpdateInterval.Int64()/1000;    
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
CLbsSatellitePositionTracker* CLbsSatellitePositionTracker::NewL(CPsyContainer *aContainer,MLbsTrackingCallback *aObserver)
    {
    qDebug() << "CLbsSatellitePositionTracker::NewL "; 
    CLbsSatellitePositionTracker *tracker = new (ELeave)CLbsSatellitePositionTracker(aContainer,aObserver);
    tracker->ConstructL();
    return tracker;
    }

CLbsSatellitePositionTracker::~CLbsSatellitePositionTracker()
    {
    qDebug() << "CLbsSatellitePositionTracker::~CLbsSatellitePositionTracker "<<iTrackerName; 
        
    }
    
CLbsSatellitePositionTracker::CLbsSatellitePositionTracker(CPsyContainer *aContainer,MLbsTrackingCallback *aObserver)
: CLbsTrackTimerBase (GPSOnly,aContainer,aObserver,SATELLITE_TTFF)
    {
    qDebug() << "CLbsSatellitePositionTracker::CLbsSatellitePositionTracker "<<iTrackerName; 
    QString qTrackerName("SatelliteTracker");
    iTrackerName = qTrackerName;
    }

void CLbsSatellitePositionTracker::ConstructL()
    {
    qDebug() << "CLbsSatellitePositionTracker::ConstructL - START "<<iTrackerName  ;
    
    CLbsTrackTimerBase::ConstructL();
    
    //Initialize PSYs
    RArray<TPositionModuleInfo*>  modInfoArray;
    iContainer->GetAvailablePsys(EFalse,modInfoArray);
    qDebug() << "CLbsSatellitePositionTracker::ConstructL - START 2"<<iTrackerName  ;
    InitializePSYs(modInfoArray,MIN_GPS_UPD_INTERVAL);
    qDebug() << "CLbsSatellitePositionTracker::ConstructL - START 3"<<iTrackerName  ;
    modInfoArray.Close();
    
    SetUpdateInterval(iMinUpdateInterval.Int64()/1000);
    qDebug() << "CLbsSatellitePositionTracker::ConstructL - START 4"<<iTrackerName  ;
    //Register for callbacks
    iContainer->SetSatellitePsyCountChangeCallback(this);
    qDebug() << "CLbsSatellitePositionTracker::ConstructL - End "<<iTrackerName  ;
    }


CLbsNonSatellitePositionTracker* CLbsNonSatellitePositionTracker::NewL(CPsyContainer *aContainer,MLbsTrackingCallback *aObserver)
    {
    qDebug() << "CLbsNonSatellitePositionTracker::NewL "; 
    CLbsNonSatellitePositionTracker *tracker = new (ELeave)CLbsNonSatellitePositionTracker(aContainer,aObserver);
    tracker->ConstructL();
    return tracker;
    }

CLbsNonSatellitePositionTracker::~CLbsNonSatellitePositionTracker()
    {
    qDebug() << "CLbsNonSatellitePositionTracker::~CLbsNonSatellitePositionTracker "<<iTrackerName; 
        
    }
    
CLbsNonSatellitePositionTracker::CLbsNonSatellitePositionTracker(CPsyContainer *aContainer,MLbsTrackingCallback *aObserver)
: CLbsTrackTimerBase (NWOnly,aContainer,aObserver,NW_TTFF)
    {
    qDebug() << "CLbsNonSatellitePositionTracker::CLbsNonSatellitePositionTracker "<<iTrackerName; 
    QString qTrackerName("NonSatelliteTracker");
    iTrackerName = qTrackerName;
    }

void CLbsNonSatellitePositionTracker::ConstructL()
    {
    qDebug() << "CLbsNonSatellitePositionTracker::ConstructL - START "<<iTrackerName  ;
    
    CLbsTrackTimerBase::ConstructL();
    
    //Initialize PSYs
    RArray<TPositionModuleInfo*>  modInfoArray;
    iContainer->GetAvailablePsys(ETrue,modInfoArray);
    
    InitializePSYs(modInfoArray,MIN_NW_UPD_INTERVAL);
    
    modInfoArray.Close();
    
    SetUpdateInterval(iMinUpdateInterval.Int64()/1000);

    //Register for callbacks
    iContainer->SetNonSatellitePsyCountChangeCallback(this);
    qDebug() << "CLbsNonSatellitePositionTracker::ConstructL - End "  ;
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
    
     CLbsAllPosProxyState(CLbsAllPosProxy *aTracker):iTracker(aTracker){}
            
      
            
      CLbsAllPosProxy *iTracker;
};


class CLbsAllPosIdle : public CLbsAllPosProxyState
    {
public:    
    static CLbsAllPosIdle* NewL(CLbsAllPosProxy *aTracker)
        {
        CLbsAllPosIdle *idle = new (ELeave) CLbsAllPosIdle(aTracker);
        return idle;
        }
protected:
    CLbsAllPosIdle(CLbsAllPosProxy *aTracker) : CLbsAllPosProxyState(aTracker){}
    
    void StartUpdates()
        {
        qDebug() << "CLbsAllPosIdle::StartUpdates";
        iTracker->SetNextState(iTracker->iInitialHybrid);
        }
    void StopUpdates()
        {
        qDebug() << "CLbsAllPosIdle::StopUpdates";
        //Nothing to do
        
        }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosIdle::SatelliteFixReceived ERROR";
        //should never happen
        }
    void SatelliteFixTimedOut()
        {
        qDebug() << "CLbsAllPosIdle::SatelliteFixTimedOut ERROR";
        //should never happen
        }
    void NWFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosIdle::NWFixReceived ERROR";
        //should never happen
        }
    void NWFixTimedOut()
        {
        qDebug() << "CLbsAllPosIdle::NWFixTimedOut ERROR";
        //should never happen
        }
    void EnterState()
        {
        qDebug() << "CLbsAllPosIdle::EnterState";
        iTracker->iSatelliteTracker->StopUpdates();
        iTracker->iNWTracker->StopUpdates();
        }


    };

class CLbsAllPosInitialHybrid : public CLbsAllPosProxyState
    {
public:    
    static CLbsAllPosInitialHybrid* NewL(CLbsAllPosProxy *aTracker)
        {
        CLbsAllPosInitialHybrid *state = new (ELeave) CLbsAllPosInitialHybrid(aTracker);
        return state;
        }
protected:
    CLbsAllPosInitialHybrid(CLbsAllPosProxy *aTracker) : 
    CLbsAllPosProxyState(aTracker),iSatelliteTrackerTimedOut(EFalse),
    iNWTrackerTimedOut(EFalse),iTimeOutSent(EFalse)
        {}
    
    void StartUpdates()
        {
        qDebug() << "CLbsAllPosInitialHybrid::StartUpdates";
        //Nothing to do. The trackers are alredy started
        }
    void StopUpdates()
        {
        qDebug() << "CLbsAllPosInitialHybrid::StopUpdates";
        iTracker->SetNextState(iTracker->iIdle);
        }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosInitialHybrid::SatelliteFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iTracker->SetNextState(iTracker->iSatelliteOnly);
        }
    void SatelliteFixTimedOut()
        {
        qDebug() << "CLbsAllPosInitialHybrid::SatelliteFixTimedOut";
        iSatelliteTrackerTimedOut = ETrue;
        
        if(!iTimeOutSent)
        if(iSatelliteTrackerTimedOut && iNWTrackerTimedOut)
            {
            qDebug() << "CLbsAllPosInitialHybrid::SatelliteFixTimedOut NotifyTimeOutToObserver";
            iTracker->NotifyTimeOutToObserver();
            iTimeOutSent = ETrue;
            }
        }
    void NWFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosInitialHybrid::NWFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iTracker->SetNextState(iTracker->iSatelliteOnly);
        }
    void NWFixTimedOut()
        {
        qDebug() << "CLbsAllPosInitialHybrid::NWFixTimedOut";
        
        if(!iTimeOutSent)
        if(iSatelliteTrackerTimedOut && iNWTrackerTimedOut)
             {
             qDebug() << "CLbsAllPosInitialHybrid::NWFixTimedOut NotifyTimeOutToObserver";
             iTracker->NotifyTimeOutToObserver();
             iTimeOutSent = ETrue;
             }
        }
    void EnterState()
        {
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
    static CLbsAllPosSatelliteOnly* NewL(CLbsAllPosProxy *aTracker)
        {
        CLbsAllPosSatelliteOnly *state = new (ELeave) CLbsAllPosSatelliteOnly(aTracker);
        return state;
        }
protected:
    CLbsAllPosSatelliteOnly(CLbsAllPosProxy *aTracker):CLbsAllPosProxyState(aTracker){}
    
    void StartUpdates()
        {
        qDebug() << "CLbsAllPosSatelliteOnly::StartUpdates";
        //Nothing to do. Satellite tracker is already running
        }
    void StopUpdates()
        {
        qDebug() << "CLbsAllPosSatelliteOnly::StopUpdates";
        iTracker->SetNextState(iTracker->iIdle);
        }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosSatelliteOnly::SatelliteFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        }
    void SatelliteFixTimedOut()
        {
        qDebug() << "CLbsAllPosSatelliteOnly::SatelliteFixTimedOut";
        iTracker->NotifyTimeOutToObserver();
        iTracker->SetNextState(iTracker->iRunningHybrid);
        }
    void NWFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosSatelliteOnly::NWFixReceived ERROR";
        //can not happen
        }
    void NWFixTimedOut()
        {
        qDebug() << "CLbsAllPosSatelliteOnly::NWFixTimedOut ERROR";
        //Can not happen
        }
    void EnterState()
        {
        qDebug() << "CLbsAllPosSatelliteOnly::EnterState";
        iTracker->iNWTracker->StopUpdates();
        qDebug() << "CLbsAllPosSatelliteOnly::EnterState 2";
        //SatelliteTracker is already started so no need to start it here.
        }
    
    };

class CLbsAllPosRunningHybrid : public CLbsAllPosProxyState
    {
public:    
    static CLbsAllPosRunningHybrid* NewL(CLbsAllPosProxy *aTracker)
        {
        CLbsAllPosRunningHybrid *state = new (ELeave) CLbsAllPosRunningHybrid(aTracker);
        return state;
        }
protected:
    CLbsAllPosRunningHybrid(CLbsAllPosProxy *aTracker) : CLbsAllPosProxyState(aTracker){}
    
    void StartUpdates()
        {
        qDebug() << "CLbsAllPosRunningHybrid::StartUpdates";
        //Nothing to do. Both trackers are already running
        }
    void StopUpdates()
        {
        qDebug() << "CLbsAllPosRunningHybrid::StopUpdates";
        iTracker->SetNextState(iTracker->iIdle);
        }
    void SatelliteFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosRunningHybrid::SatelliteFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iLastNotificationIsTimeOut = EFalse;
        iTracker->SetNextState(iTracker->iSatelliteOnly);
        }
    void SatelliteFixTimedOut()
        {
        qDebug() << "CLbsAllPosRunningHybrid::SatelliteFixTimedOut";
        //Nothing to do
        }
    void NWFixReceived(QGeoPositionInfo *aPosition)
        {
        qDebug() << "CLbsAllPosRunningHybrid::NWFixReceived";
        iTracker->NotifyFixToObserver(aPosition);
        iLastNotificationIsTimeOut = EFalse;
        }
    void NWFixTimedOut()
        {
        qDebug() << "CLbsAllPosRunningHybrid::NWFixTimedOut";
        
        if(!iLastNotificationIsTimeOut)
            {
             iTracker->NotifyTimeOutToObserver();
             iLastNotificationIsTimeOut = ETrue;
            }
        }
    void EnterState()
        {
        qDebug() << "CLbsAllPosRunningHybrid::EnterState";
        iTracker->iNWTracker->StartUpdates();
        //SatelliteTracker is already started so no need to start it again.
        
        iLastNotificationIsTimeOut = ETrue;
        }
private:
    TBool iLastNotificationIsTimeOut;
    };


CLbsAllPosProxy* CLbsAllPosProxy::NewL(CPsyContainer *aContainer,MLbsTrackingCallback *aObserver,
        CLbsSatellitePositionTracker *aSatelliteTracker, CLbsNonSatellitePositionTracker *aNWTracker)
    {
    CLbsAllPosProxy *tracker = new (ELeave) CLbsAllPosProxy(aContainer,aObserver,aSatelliteTracker,aNWTracker);
    TRAPD(err,tracker->ConstructL(aSatelliteTracker,aNWTracker));
    
    if(err == KErrNone)
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

CLbsAllPosProxy::CLbsAllPosProxy(CPsyContainer *aContainer,MLbsTrackingCallback *aObserver,
        CLbsSatellitePositionTracker *aSatelliteTracker, CLbsNonSatellitePositionTracker *aNWTracker)
                : CLbsPositionTrackerBase (AllPsys,aContainer,aObserver),
                  iSatelliteTracker(aSatelliteTracker),iNWTracker(aNWTracker)
    {
    QString qTrackerName("AllPosProxy");
    iTrackerName = qTrackerName;
    
    }
void CLbsAllPosProxy::ConstructL(CLbsSatellitePositionTracker *aSatelliteTracker , CLbsNonSatellitePositionTracker *aNWTracker)
    {
    
    if(aSatelliteTracker == NULL)
        iSatelliteTracker = CLbsSatellitePositionTracker::NewL(iContainer,this);
    else
        {
        iSatelliteTracker = aSatelliteTracker;
        iSatelliteTracker->SetObserver(this);
        }
    
     if(aNWTracker == NULL)
         iNWTracker = CLbsNonSatellitePositionTracker::NewL(iContainer,this);
     else
         {
         iNWTracker = aNWTracker;
         iNWTracker->SetObserver(this);
         }

     TMilliSeconds minInterval =  (iSatelliteTracker->GetMinimumUpdateInterval()  < iNWTracker->GetMinimumUpdateInterval())?
         iSatelliteTracker->GetMinimumUpdateInterval() : iNWTracker->GetMinimumUpdateInterval();
     
     iMinUpdateInterval = minInterval*1000;
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
void CLbsAllPosProxy::TrackingLocation(QGeoPositionInfo *aPosition,CLbsPositionTrackerBase *aTracker )
    {
    if(GPSOnly == aTracker->GetTrackingType())
        iCurrentState->SatelliteFixReceived(aPosition);
    else
        iCurrentState->NWFixReceived(aPosition);
    }
void CLbsAllPosProxy::TrackingRequestTimedOut(CLbsPositionTrackerBase *aTracker)
    {
    if(GPSOnly == aTracker->GetTrackingType())
        iCurrentState->SatelliteFixTimedOut();
    else
        iCurrentState->NWFixTimedOut();
    }
void CLbsAllPosProxy::NotifyFixToObserver(QGeoPositionInfo *aPosition)
    {
    iLatestQPosInfo = aPosition;
    iObserver->TrackingLocation(iLatestQPosInfo,this);
    iLastQPosInfo = iLatestQPosInfo;
    }
void CLbsAllPosProxy::NotifyTimeOutToObserver()
    {
    iObserver->TrackingRequestTimedOut(this);
    }

CLbsSatellitePositionTracker* CLbsAllPosProxy::ReleaseSatelliteTracker()
    {
    CLbsSatellitePositionTracker *tracker = iSatelliteTracker;
    iSatelliteTracker = NULL;
    return tracker;
    }
CLbsNonSatellitePositionTracker* CLbsAllPosProxy::ReleaseNWTracker()
    {
    CLbsNonSatellitePositionTracker *tracker = iNWTracker;
    iNWTracker = NULL;
    return tracker;
    }

class LbsTrackTimerStateBase
    {
public:
    virtual void StartUpdates() = 0;
    virtual void StopUpdates() = 0;
    virtual void FixReceived() = 0;
    virtual void FixTimedOut() = 0;
    virtual void TimerExpired() = 0;
    virtual void EnterState() = 0;
    virtual void SetUpdateInterval(TTimeIntervalMicroSeconds aIntr) = 0;
    
    //interrupts
   
    virtual void PsyInstalled(CTrackingPsy *trackingPsy) = 0; 
    virtual void PsyUninstalled()
    {
        qDebug() << "LbsTrackTimerStateBase::PsyUninstalled"<<iTrackTimer->iTrackerName;
        TBool oneOrMorePsysActive = EFalse;
        for(int i = 0; i < iTrackTimer->iTrackingPsys.Count(); i++)
            {
            if(iTrackTimer->iTrackingPsys[i]->IsActive())
                {
                oneOrMorePsysActive = ETrue;
                break;
                 }
            }
        if(!oneOrMorePsysActive)
            {
            qDebug() << "LbsTrackTimerStateBase::StartUpdates 4<<iTrackTimer->iTrackerName";
            iTrackTimer->iObserver->TrackingRequestTimedOut(iTrackTimer);
            //There were no PSYs, wait for one of them to be enabled or installed :)
            iTrackTimer->SetNextState(iTrackTimer->iTimedoutState);        
            }
        //else do nothing
    
    }
    
    virtual ~LbsTrackTimerStateBase() {}
protected:
    LbsTrackTimerStateBase(CLbsTrackTimerBase *aTrackTimer) : iTrackTimer(aTrackTimer) {}
    CLbsTrackTimerBase *iTrackTimer;
    };

class LbsTrackTimerIdleState : public LbsTrackTimerStateBase
    {
public:
    LbsTrackTimerIdleState(CLbsTrackTimerBase *aTrackTimer) : LbsTrackTimerStateBase(aTrackTimer) {}
protected:
void TimerExpired()
    {
    qDebug() << "LbsTrackTimerIdleState::TimerExpired"<<iTrackTimer->iTrackerName;
    }
void StartUpdates()
    {
    qDebug() << "LbsTrackTimerIdleState::StartUpdates"<<iTrackTimer->iTrackerName;
    iTrackTimer->Cancel();     
    qDebug() << "LbsTrackTimerIdleState::StartUpdates 0"<<iTrackTimer->iTrackerName;
    iTrackTimer->After(iTrackTimer->iTTFFTimeOut);
    qDebug() << "LbsTrackTimerIdleState::StartUpdates 1";    
    TBool oneOrMorePsysActive = EFalse;
    for(int i = 0; i < iTrackTimer->iTrackingPsys.Count(); i++)
         {
         iTrackTimer->iTrackingPsys[i]->IssueRequest(iTrackTimer->iUpdateInterval);
         if(iTrackTimer->iTrackingPsys[i]->IsActive())
             oneOrMorePsysActive = ETrue;
         }
    qDebug() << "LbsTrackTimerIdleState::StartUpdates 2<<iTrackTimer->iTrackerName";
    if(oneOrMorePsysActive)
        {
        qDebug() << "LbsTrackTimerIdleState::StartUpdates 3<<iTrackTimer->iTrackerName";
        iTrackTimer->SetNextState(iTrackTimer->iFirstFixState);
        }
    else
        {
        qDebug() << "LbsTrackTimerIdleState::StartUpdates 4<<iTrackTimer->iTrackerName";
        iTrackTimer->iObserver->TrackingRequestTimedOut(iTrackTimer);
        //There were no PSYs, wait for one of them to be enabled or installed :)
        iTrackTimer->SetNextState(iTrackTimer->iTimedoutState);
        }
    }
void StopUpdates()
    {
    qDebug() << "LbsTrackTimerIdleState::StopUpdates"<<iTrackTimer->iTrackerName;
    //Nothing to do. Nothing is started
    }
void FixReceived()
    {
    qDebug() << "LbsTrackTimerIdleState::FixReceived STATE MACHINE ERROR"<<iTrackTimer->iTrackerName;
    }
void FixTimedOut()
    {
    qDebug() << "LbsTrackTimerIdleState::FixTimedOut STATE MACHINE ERROR"<<iTrackTimer->iTrackerName;
    }
void EnterState()
    {
    qDebug() << "LbsTrackTimerIdleState::EnterState"<<iTrackTimer->iTrackerName;
    //Nothing to do
    }
void SetUpdateInterval(TTimeIntervalMicroSeconds aIntr)
    {
    qDebug() << "LbsTrackTimerIdleState::SetUpdateInterval"<<iTrackTimer->iTrackerName;
    //Nothing to do
    }
void PsyUninstalled()
    {
    qDebug() << "LbsTrackTimerIdleState::PsyUninstalled"<<iTrackTimer->iTrackerName;
    //Nothing to do
    }
void PsyInstalled(CTrackingPsy *trackingPsy)
    {
    qDebug() << "LbsTrackTimerIdleState::PsyInstalled"<<iTrackTimer->iTrackerName<<trackingPsy->iPsyName;
    //Nothing to do
    }
    };
class LbsTrackTimerFFState : public LbsTrackTimerStateBase
    {
public:
    LbsTrackTimerFFState(CLbsTrackTimerBase *aTrackTimer) : LbsTrackTimerStateBase(aTrackTimer) {}
protected:    
void TimerExpired()
    {
    qDebug() << "LbsTrackTimerFFState::TimerExpired"<<iTrackTimer->iTrackerName;
    iTrackTimer->iObserver->TrackingRequestTimedOut(iTrackTimer);
    iTrackTimer->SetNextState(iTrackTimer->iTimedoutState);
    }
void StartUpdates()
    {
    qDebug() << "LbsTrackTimerFFState::TimerExpired"<<iTrackTimer->iTrackerName;
    //Nothing to do already started
    }
void StopUpdates()
    {
    qDebug() << "LbsTrackTimerFFState::StopUpdates"<<iTrackTimer->iTrackerName;
    
    if(iTrackTimer->IsActive())
        iTrackTimer->Cancel();
    else
        iTrackTimer->CancelPsyRequests();
    
    iTrackTimer->SetNextState(iTrackTimer->iIdleState);
    }
void FixReceived()
    {
    qDebug() << "LbsTrackTimerFFState::FixReceived"<<iTrackTimer->iTrackerName;
    iTrackTimer->iObserver->TrackingLocation(iTrackTimer->iLatestQPosInfo,iTrackTimer);
    delete iTrackTimer->iLastQPosInfo;
    iTrackTimer->iLastQPosInfo = iTrackTimer->iLatestQPosInfo;
    iTrackTimer->iLatestQPosInfo = NULL;
    iTrackTimer->SetNextState(iTrackTimer->iRunningState);
    }
void FixTimedOut()
    {
    qDebug() << "LbsTrackTimerFFState::FixTimedOut"<<iTrackTimer->iTrackerName;
    //Nothing to do. Can be called multiple times from different PSYs 
    //It will be handled in TimerExpired()
    }
void EnterState()
    {
    qDebug() << "LbsTrackTimerFFState::EnterState"<<iTrackTimer->iTrackerName;
    //Nothing to do, Already started
    }
void SetUpdateInterval(TTimeIntervalMicroSeconds aIntr)
    {
    qDebug() << "LbsTrackTimerFFState::SetUpdateInterval"<<iTrackTimer->iTrackerName;
    for(int i = 0; i < iTrackTimer->iTrackingPsys.Count(); i++)
        iTrackTimer->iTrackingPsys[i]->IssueRequest(iTrackTimer->iUpdateInterval);
        //IssueRequest will internally cancel and call NotifyPositionUpdate
    }
void PsyUninstalled()
    {
    qDebug() << "LbsTrackTimerFFState::PsyUninstalled"<<iTrackTimer->iTrackerName;
    LbsTrackTimerStateBase::PsyUninstalled();
    }
void PsyInstalled(CTrackingPsy *trackingPsy)
    {
    qDebug() << "LbsTrackTimerFFState::PsyInstalled"<<iTrackTimer->iTrackerName<<trackingPsy->iPsyName;
    trackingPsy->IssueRequest(iTrackTimer->iUpdateInterval);
    }

    };
class LbsTrackTimerTimedoutState : public LbsTrackTimerStateBase
    {
public:
    LbsTrackTimerTimedoutState(CLbsTrackTimerBase *aTrackTimer) : LbsTrackTimerStateBase(aTrackTimer) {}
protected:    
void TimerExpired()
    {
    qDebug() << "LbsTrackTimerTimedoutState::TimerExpired STATE MACHINE ERROR"<<iTrackTimer->iTrackerName;
    }
void StartUpdates()
    {
    qDebug() << "LbsTrackTimerTimedoutState::StartUpdates"<<iTrackTimer->iTrackerName;
    //Nothing to do. Already started
    }
void StopUpdates()
    {
    qDebug() << "LbsTrackTimerTimedoutState::StopUpdates"<<iTrackTimer->iTrackerName;
    if(iTrackTimer->IsActive())
        iTrackTimer->Cancel();
    else
        iTrackTimer->CancelPsyRequests();
    iTrackTimer->SetNextState(iTrackTimer->iIdleState);
    }

void FixReceived()
    {
    qDebug() << "LbsTrackTimerTimedoutState::FixReceived"<<iTrackTimer->iTrackerName;
    iTrackTimer->iObserver->TrackingLocation(iTrackTimer->iLatestQPosInfo,iTrackTimer);
    delete iTrackTimer->iLastQPosInfo;
    iTrackTimer->iLastQPosInfo = iTrackTimer->iLatestQPosInfo;
    iTrackTimer->iLatestQPosInfo = NULL;
    iTrackTimer->SetNextState(iTrackTimer->iRunningState);     
    }
void FixTimedOut()
    {
    qDebug() << "LbsTrackTimerTimedoutState::FixTimedOut"<<iTrackTimer->iTrackerName;
    //Nothing to do timeout already sent
    //Keep waiting for fix received
    }
void EnterState()
    {
    qDebug() << "LbsTrackTimerTimedoutState::EnterState"<<iTrackTimer->iTrackerName;
    //Nothing to do
    }
void SetUpdateInterval(TTimeIntervalMicroSeconds aIntr)
    {
    qDebug() << "LbsTrackTimerTimedoutState::SetUpdateInterval"<<iTrackTimer->iTrackerName;
    for(int i = 0; i < iTrackTimer->iTrackingPsys.Count(); i++)
         iTrackTimer->iTrackingPsys[i]->IssueRequest(iTrackTimer->iUpdateInterval);
         //IssueRequest will internally cancel and call NotifyPositionUpdate    
    }
void PsyUninstalled()
    {
    qDebug() << "LbsTrackTimerTimedoutState::PsyUninstalled"<<iTrackTimer->iTrackerName;
    //Nothing to do
    }
void PsyInstalled(CTrackingPsy *trackingPsy)
    {
    qDebug() << "LbsTrackTimerTimedoutState::PsyInstalled"<<iTrackTimer->iTrackerName<<trackingPsy->iPsyName;
    trackingPsy->IssueRequest(iTrackTimer->iUpdateInterval);
    }
    };
class LbsTrackTimerRunningState : public LbsTrackTimerStateBase
    {
public:
    LbsTrackTimerRunningState(CLbsTrackTimerBase *aTrackTimer) : 
    LbsTrackTimerStateBase(aTrackTimer),iFailureCount(0)
        {}

private:    
void TimerExpired()
    {
    qDebug() << "LbsTrackTimerRunningState::TimerExpired"<<iTrackTimer->iTrackerName;
    
    if(HasUpdateIntervalTimerCompleted())
        {
        if(iTrackTimer->iLatestQPosInfo)
            {
            qDebug() << "LbsTrackTimerRunningState::TimerExpired SENDING UPDATE "<<iTrackTimer->iTrackerName; 
            iTrackTimer->iObserver->TrackingLocation(iTrackTimer->iLatestQPosInfo,iTrackTimer);
            delete iTrackTimer->iLastQPosInfo;
            iTrackTimer->iLastQPosInfo = iTrackTimer->iLatestQPosInfo;
            iTrackTimer->iLatestQPosInfo = NULL;
            iFailureCount = 0;
            iTimerInterval = iTimerIntervalCache;
            StartTimer();
            }
        else
            {
            iFailureCount++;
            qDebug() << "LbsTrackTimerRunningState::TimerExpired FAILED " << iFailureCount << "Type=" <<iTrackTimer->iTrackerName;
            if(iFailureCount == 6) //Adjust count differently for GPS and NW?
                {
                iTrackTimer->iObserver->TrackingRequestTimedOut(iTrackTimer);
                iTrackTimer->SetNextState(iTrackTimer->iTimedoutState);
                }
            else
                {
                iTimerInterval = iTimerIntervalCache;
                StartTimer();
                }
            }
        }
    else
        StartTimer();
    }
void StartUpdates()
    {
    qDebug() << "LbsTrackTimerRunningState::StartUpdates"<<iTrackTimer->iTrackerName;
    //Already running, nothing to do
    }
void StopUpdates()
    {
    qDebug() << "LbsTrackTimerRunningState::StopUpdates"<<iTrackTimer->iTrackerName;
    if(iTrackTimer->IsActive())
        iTrackTimer->Cancel();
    else
        iTrackTimer->CancelPsyRequests();
    iTrackTimer->SetNextState(iTrackTimer->iIdleState);
    }
void FixReceived()
    {
    qDebug() << "LbsTrackTimerRunningState::FixReceived"<<iTrackTimer->iTrackerName;
    //Nothing to do. Handled in TimerExpired()
    }
void FixTimedOut()
    {
    qDebug() << "LbsTrackTimerRunningState::FixTimedOut"<<iTrackTimer->iTrackerName;
    //Nothing to do. Handled in TimerExpired()
    }
void EnterState()
    {
    qDebug() << "LbsTrackTimerRunningState::EnterState"<<iTrackTimer->iTrackerName;
    iFailureCount = 0;
    if(iTrackTimer->iUpdateInterval.Int64() > MAX_TIMER )
        {
        iTimerInterval.itrMaxTimer = iTrackTimer->iUpdateInterval.Int64()/MAX_TIMER;
        iTimerInterval.balanceDuration = iTrackTimer->iUpdateInterval.Int64()%MAX_TIMER;
        }
    else
        {
        iTimerInterval.itrMaxTimer = 0;
        iTimerInterval.balanceDuration = iTrackTimer->iUpdateInterval.Int64();
        }
    iTimerIntervalCache = iTimerInterval;
    if(StartTimer())
        {
        //This check should never pass
        qDebug() << "LbsTrackTimerRunningState::EnterState STATE MACHINE ERROR"<<iTrackTimer->iTrackerName;
        }
    }
void SetUpdateInterval(TTimeIntervalMicroSeconds aIntr)
    {
    qDebug() << "LbsTrackTimerRunningState::SetUpdateInterval"<<iTrackTimer->iTrackerName;
    for(int i = 0; i < iTrackTimer->iTrackingPsys.Count(); i++)
        iTrackTimer->iTrackingPsys[i]->IssueRequest(iTrackTimer->iUpdateInterval);
        //IssueRequest will internally cancel and call NotifyPositionUpdate    
    }
void PsyUninstalled()
    {
    qDebug() << "LbsTrackTimerRunningState::PsyUninstalled"<<iTrackTimer->iTrackerName;
    LbsTrackTimerStateBase::PsyUninstalled();
    }
void PsyInstalled(CTrackingPsy *trackingPsy)
    {
    qDebug() << "LbsTrackTimerRunningState::PsyInstalled"<<iTrackTimer->iTrackerName<<trackingPsy->iPsyName;
    trackingPsy->IssueRequest(iTrackTimer->iUpdateInterval);
    }

//Returns ETrue if the updateInterval has expired
bool StartTimer()
    {
    qDebug() << "LbsTrackTimerRunningState::StartTimer  "<<iTrackTimer->iTrackerName;     
    bool sendClientUpdate = EFalse;
    
    if(iTimerInterval.itrMaxTimer > 0)
        {
        iTrackTimer->After(MAX_TIMER);
        iTimerInterval.itrMaxTimer--;
        
        if(!iTrackTimer->IsActive())
            iTrackTimer->SetActive();
        }
    else if(iTimerInterval.balanceDuration.Int() > 0)
        {
        iTrackTimer->After(iTimerInterval.balanceDuration);
        iTimerInterval.balanceDuration = 0;
        }
    else
        {
        iTimerInterval = iTimerIntervalCache;
        sendClientUpdate = ETrue;
        StartTimer();//Ignore return value
        }
    return sendClientUpdate;
    }

TBool HasUpdateIntervalTimerCompleted()
    {
    if(0 == iTimerInterval.balanceDuration.Int() &&
       0 == iTimerInterval.itrMaxTimer )
        return ETrue;
    return EFalse;
    }

//data
struct TimerInterval
    {
    TInt itrMaxTimer;
    TTimeIntervalMicroSeconds32 balanceDuration;
    };
TimerInterval iTimerInterval;
TimerInterval iTimerIntervalCache;
TInt iFailureCount;

    };

CLbsTrackTimerBase::CLbsTrackTimerBase(TrackingType aType,CPsyContainer *aContainer,
                    MLbsTrackingCallback *aObserver,TTimeIntervalMicroSeconds32 aTTFFTimeOut)
                    : CLbsPositionTrackerBase(aType,aContainer,aObserver), CTimer(CActive::EPriorityStandard),
                      iTTFFTimeOut(1000000),iCurrentState(NULL),iIdleState(NULL),iFirstFixState(NULL),
                      iTimedoutState(NULL),iRunningState(NULL)
    {
    }
void CLbsTrackTimerBase::ConstructL()
    {
    qDebug() << "CLbsTrackTimerBase::ConstructL START  "<<iTrackerName;         
    iIdleState = new (ELeave) LbsTrackTimerIdleState(this);
    iFirstFixState = new (ELeave) LbsTrackTimerFFState(this);
    iTimedoutState = new (ELeave) LbsTrackTimerTimedoutState(this);
    iRunningState = new (ELeave) LbsTrackTimerRunningState(this);
    iCurrentState = iIdleState;
    CTimer::ConstructL();
    CActiveScheduler::Add(this);  
    qDebug() << "CLbsTrackTimerBase::ConstructL END  "<<iTrackerName;
    }

CLbsTrackTimerBase::~CLbsTrackTimerBase()
    {
    Cancel();
    delete iIdleState;
    delete iFirstFixState;
    delete iTimedoutState;
    delete iRunningState;
    }

void CLbsTrackTimerBase::DoCancel()
    {
    qDebug() << "CLbsTrackTimerBase::DoCancel"<<iTrackerName;
    CancelPsyRequests();
    }

void CLbsTrackTimerBase::CancelPsyRequests()
    {
    qDebug() << "CLbsTrackTimerBase::CancelPsyRequests  "<<iTrackerName;
    for(int i = 0; i < iTrackingPsys.Count(); i++)
            iTrackingPsys[i]->Cancel();
    }

void CLbsTrackTimerBase::StartUpdates()
    {
    iCurrentState->StartUpdates();
    }
void CLbsTrackTimerBase::StopUpdates()
    {
    iCurrentState->StopUpdates();
    }
void CLbsTrackTimerBase::SetUpdateInterval(TMilliSeconds aIntr)
    {
    qDebug() << "CLbsTrackTimerBase::SetUpdateInterval"<<iTrackerName;
    TTimeIntervalMicroSeconds intr(aIntr*1000); 
    iUpdateInterval = (intr > iMinUpdateInterval) ? intr: iMinUpdateInterval;

    iCurrentState->SetUpdateInterval(iUpdateInterval);

    }

void CLbsTrackTimerBase::LocationUpdate(TPositionInfoBase &aPosition,CSelfManagingPsy *aPsy)
    {
    delete iLatestQPosInfo;
    iLatestQPosInfo = new QGeoPositionInfo;
    PsyUtils::TPositionInfo2QGeoPositionInfo(aPosition, *iLatestQPosInfo);
    iCurrentState->FixReceived();
    }
void CLbsTrackTimerBase::RequestTimedOut(CSelfManagingPsy *aPsy)
    {
    iCurrentState->FixTimedOut();
    }
void CLbsTrackTimerBase::RunL()
    {
    iCurrentState->TimerExpired();
    }
void CLbsTrackTimerBase::SetNextState(LbsTrackTimerStateBase *aNextState)
    {
    iCurrentState = aNextState;
    iCurrentState->EnterState();
    }

void CLbsTrackTimerBase::InitializePSYs(RArray<TPositionModuleInfo*>  aModInfoArray,const TInt64 aDefaultMinInterval)
    {
    qDebug() << "CLbsTrackTimerBase::InitializePSYs START "<<iTrackerName ;
    
    TInt insertCount = 0;
    iMinUpdateInterval = aDefaultMinInterval;
    
    for(int i = 0; i < aModInfoArray.Count(); i++)
        {
        TPositionQuality quality;   
        aModInfoArray[i]->GetPositionQuality(quality);
                
        if(0 == i)
            iMinUpdateInterval = quality.TimeToNextFix();
        else if(iMinUpdateInterval > quality.TimeToNextFix() )
            iMinUpdateInterval = quality.TimeToNextFix();
                        
        CTrackingPsy *trackingPsy = CTrackingPsy::NewL(aModInfoArray[i],iContainer,this);   
        if(trackingPsy)
            {
            iTrackingPsys.Insert(trackingPsy,insertCount++);
            qDebug() << "CLbsTrackTimerBase::InitializePSYs inserted " << trackingPsy->iPsyName ;
            }
        
        }
    iUpdateInterval = iMinUpdateInterval;
    qDebug() << "CLbsTrackTimerBase::InitializePSYs END - insertcount = " << insertCount <<" type" <<iTrackerName;    
    }

void CLbsTrackTimerBase::PsyUninstalled(TPositionModuleId &aModId)
    {
    for(int i = 0; i < iTrackingPsys.Count(); i++)
        if(iTrackingPsys[i]->GetModInfo()->ModuleId() == aModId)
            {
            delete iTrackingPsys[i];
            iTrackingPsys.Remove(i);
            iCurrentState->PsyUninstalled();
            break;
            }
    }
void CLbsTrackTimerBase::PsyInstalled(TPositionModuleInfo* aModInfo)
    {

    TPositionQuality quality;   
    aModInfo->GetPositionQuality(quality);
                    
    if(0 == iTrackingPsys.Count())
        iMinUpdateInterval = quality.TimeToNextFix();
    else if(iMinUpdateInterval > quality.TimeToNextFix() )
         iMinUpdateInterval = quality.TimeToNextFix();
                            
    CTrackingPsy *trackingPsy = CTrackingPsy::NewL(aModInfo,iContainer,this);   
    if(trackingPsy)
        {
        iTrackingPsys.Insert(trackingPsy,iTrackingPsys.Count());
        iCurrentState->PsyInstalled(trackingPsy);
        }
    }

QTM_END_NAMESPACE
