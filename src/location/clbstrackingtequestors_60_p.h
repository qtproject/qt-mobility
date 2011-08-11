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

#ifndef __CLBS_TRACKINGREQUESTORS_h
#define __CLBS_TRACKINGREQUESTORS_h

#include <lbs.h>
#include "clbspsycontainer_s60_p.h"

#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

#include "QGeoPositionInfo.h"
#include "qgeopositioninfosource.h"

#include <QObject>
#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class CPsyContainer;
class CTrackingPsy;
class CSingleShotPsy;

class QGeoPositionInfo;



typedef TInt TMilliSeconds;

class CLbsTrackingTimerBase;
class CLbsPositionTrackerBase;
class MLbsTrackingCallback
{
public:
    //virtual void TrackingIsDown() = 0;
    //virtual void TrackingIsAvbl() = 0;
    virtual void TrackingLocation(QGeoPositionInfo *aPosition, CLbsPositionTrackerBase *aTracker) = 0;
    virtual void TrackingRequestTimedOut(CLbsPositionTrackerBase *aBase) = 0;
};

//Base class for all tracking requestors
class CLbsPositionTrackerBase : public CBase
{
public:
    enum TrackingType {
        AllPsys = 1,
        GPSOnly,
        NWOnly
    };

    virtual ~CLbsPositionTrackerBase();

    virtual void StartUpdates() = 0;
    virtual void StopUpdates() = 0;

    virtual void SetUpdateInterval(TMilliSeconds mSec) = 0;
    void SetObserver(MLbsTrackingCallback *aObserver);

    TMilliSeconds GetUpdateInterval() const;
    TMilliSeconds GetMinimumUpdateInterval() const;
    CLbsPositionTrackerBase::TrackingType GetTrackingType() const;
    QGeoPositionInfo* GetLastPosition() const;

protected:
    CLbsPositionTrackerBase(TrackingType aType, CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);


    TrackingType iTrackType;
    MLbsTrackingCallback *iObserver;
    CPsyContainer *iContainer;
    QGeoPositionInfo  *iLatestQPosInfo;
    QGeoPositionInfo  *iLastQPosInfo;
    TTimeIntervalMicroSeconds  iUpdateInterval;
    TTimeIntervalMicroSeconds iMinUpdateInterval;

};


//Base class for timer based tracking requestor
class CLbsTrackingTimerBase : public CLbsPositionTrackerBase, public CActive
        , public MPsyRequestCallback, public MPsyCountChangeCallback
{
public:
    ~CLbsTrackingTimerBase();
    void StartUpdates();
    void StopUpdates();
    void SetUpdateInterval(TMilliSeconds mSec);
protected:
    CLbsTrackingTimerBase(TrackingType aType, CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);

    void ConstructL();

    void InitializePSYs(RArray<TPositionModuleInfo*>  aModInfoArray, const TInt64 aDefaultMinInterval);

    //MPsyRequestCallback
    void LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy);
    void RequestTimedOut(TInt aConsecutiveFailureCount, CSelfManagingPsy *aPsy);

    //MPsyCountChangeCallback
    void PsyUninstalled(TPositionModuleId &aModId);
    void PsyInstalled(TPositionModuleInfo* aModInfo);

    //CTimer
    void RunL();
    void DoCancel();

    //Returns ETrue if the updateInterval has expired
    bool StartTimer();


    TTimeIntervalMicroSeconds32 iTTFFTimeOut;
    TBool iTTFFTimerActive;
    RArray<CTrackingPsy*> iTrackingPsys;

    struct TimerInterval {
        TInt itrMaxTimer;
        TTimeIntervalMicroSeconds32 balanceDuration;
    };
    TimerInterval iTimerInterval;
    TInt iFailureCount;
    RTimer iTimer;

    TBool iStartUpdatesActive;

};

//Satellite based tracking requestor
class CLbsSatelliteTracker : public CLbsTrackingTimerBase
{
public:
    static CLbsSatelliteTracker* NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);
    ~CLbsSatelliteTracker();
private:
    CLbsSatelliteTracker(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);
    void ConstructL();


};

//Network based tracking requestor
class CLbsNetworkTracker : public CLbsTrackingTimerBase
{
public:
    static CLbsNetworkTracker* NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);

    ~CLbsNetworkTracker();
private:
    CLbsNetworkTracker(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);
    void ConstructL();


};

class CLbsAllPosProxyState;
class CLbsAllPosProxy;
class CLbsAllPosIdle;

//Proxy which internally uses the satellite tracker and NW tracker
class CLbsAllPosProxy : public CLbsPositionTrackerBase, public MLbsTrackingCallback
{
public:
    static CLbsAllPosProxy* NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver,
                                 CLbsSatelliteTracker *aSatelliteTracker = NULL, CLbsNetworkTracker *aNWTracker = NULL);

    CLbsSatelliteTracker* ReleaseSatelliteTracker();
    CLbsNetworkTracker* ReleaseNWTracker();
    ~CLbsAllPosProxy();

private:

    CLbsAllPosProxy(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver,
                    CLbsSatelliteTracker *aSatelliteTracker = NULL, CLbsNetworkTracker *aNWTracker = NULL);

    void ConstructL(CLbsSatelliteTracker *aSatelliteTracker , CLbsNetworkTracker *aNWTracker);

    //Should be propagated to the other Satellite and NW trackers
    void SetUpdateInterval(TMilliSeconds mSec);
    void StartUpdates();
    void StopUpdates();

    //Used for listening to updates from satellite tracker and NW Tracker
    void TrackingLocation(QGeoPositionInfo *aPosition, CLbsPositionTrackerBase *aTracker);
    void TrackingRequestTimedOut(CLbsPositionTrackerBase *aBase);

    void SetNextState(CLbsAllPosProxyState *nextState);

    void NotifyFixToObserver(QGeoPositionInfo *aPosition);
    void NotifyTimeOutToObserver();


    CLbsSatelliteTracker *iSatelliteTracker;
    CLbsNetworkTracker *iNWTracker;

    CLbsAllPosProxyState *iCurrentState;

    //states
    CLbsAllPosProxyState *iIdle;
    CLbsAllPosProxyState *iInitialHybrid;
    CLbsAllPosProxyState *iSatelliteOnly;
    CLbsAllPosProxyState *iRunningHybrid;

    friend class CLbsAllPosIdle;
    friend class CLbsAllPosInitialHybrid;
    friend class CLbsAllPosSatelliteOnly;
    friend class CLbsAllPosRunningHybrid;
};

class CLbsSingleShotRequestor;

class MLbsSingleShotCallback
{
public:
    virtual void SSLocation(QGeoPositionInfo *aPosition, CLbsSingleShotRequestor *aRequestor) = 0;
    virtual void SSRequestTimedOut(CLbsSingleShotRequestor *aRequestor) = 0;
};

class CLbsSingleShotRequestor : public CTimer , public MPsyRequestCallback
{
public:

    enum SSType {
        SS_AllPsys = 1,
        SS_GPSOnly,
        SS_NWOnly
    };

    static CLbsSingleShotRequestor* NewL(CPsyContainer *aContainer, MLbsSingleShotCallback *aObserver, SSType aSSType);

    void RequestLocation(TMilliSeconds &aMsec);
    SSType GetSingleShotType() const;

    ~CLbsSingleShotRequestor();

    void Cancel();

private:
    CLbsSingleShotRequestor(CPsyContainer *aContainer, MLbsSingleShotCallback *aObserver, SSType aSSType);
    void ConstructL();



    void RunL();
    void DoCancel();

    bool StartTimer();

    //MPsyRequestCallback
    void LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy);
    void RequestTimedOut(TInt aConsecutiveFailureCount, CSelfManagingPsy *aPsy);

    CPsyContainer *iContainer;
    MLbsSingleShotCallback *iObserver;
    RArray<CSingleShotPsy*> iSSPsys;
    SSType iSSType;
    QGeoPositionInfo  *iLatestQPosInfo;

    struct TimerInterval {
        TInt itrMaxTimer;
        TTimeIntervalMicroSeconds32 balanceDuration;
    };

    TimerInterval iTimerInterval;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif

