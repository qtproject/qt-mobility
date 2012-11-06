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

#ifndef SYMBIAN_LBSTRACKER_H
#define SYMBIAN_LBSTRACKER_H


#include <QObject>
#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"

#include "symbian_lbsfacade_p.h"
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"

#include <lbssatellite.h>


QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class CLbsPositionTrackerBase;
class CTrackingPsy;

class MLbsTrackingCallback
{
public:
    virtual void TrackingLocation(QGeoPositionInfo *aPosition, CLbsPositionTrackerBase *aTracker) = 0;
    virtual void TrackingRequestTimedOut(CLbsPositionTrackerBase *aBase) = 0;
};

//Base class for all tracking requestors
class CLbsPositionTrackerBase : public CBase
{
public:
    QString iTrackerName;//Only for debugging purposes
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

class LbsTrackTimerStateBase;
class LbsTrackTimerIdleState;
class LbsTrackTimerFFState;
class LbsTrackTimerTimedoutState;
class LbsTrackTimerRunningState;

class CLbsTrackTimerBase : public CLbsPositionTrackerBase, public CTimer
        , public MPsyRequestCallback, public MPsyCountChangeCallback
{
public:
    //CLbsPositionTrackerBase
    void StartUpdates();
    void StopUpdates();
    void SetUpdateInterval(TMilliSeconds mSec);

protected:
    CLbsTrackTimerBase(TrackingType aType, CPsyContainer *aContainer,
                       MLbsTrackingCallback *aObserver, TTimeIntervalMicroSeconds32 aTTFFTimeOut);
    void ConstructL();
    ~CLbsTrackTimerBase();

    //MPsyRequestCallback
    void LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy);
    void RequestTimedOut(CSelfManagingPsy *aPsy);
    //CTimer
    void RunL();
    void DoCancel();

    void CancelPsyRequests();

    //MPsyCountChangeCallback
    void PsyUninstalled(TPositionModuleId &aModId);
    void PsyInstalled(TPositionModuleInfo* aModInfo);

    void InitializePSYs(RArray<TPositionModuleInfo*>  aModInfoArray, const TInt64 aDefaultMinInterval);

    RArray<CTrackingPsy*> iTrackingPsys;
    TTimeIntervalMicroSeconds32 iTTFFTimeOut;
    LbsTrackTimerStateBase *iCurrentState;
    LbsTrackTimerStateBase *iIdleState;
    LbsTrackTimerStateBase *iFirstFixState;
    LbsTrackTimerStateBase *iTimedoutState;
    LbsTrackTimerStateBase *iRunningState;
private:
    void SetNextState(LbsTrackTimerStateBase *aNextState);

    friend class LbsTrackTimerStateBase;
    friend class LbsTrackTimerIdleState;
    friend class LbsTrackTimerFFState;
    friend class LbsTrackTimerTimedoutState;
    friend class LbsTrackTimerRunningState;
};

//Satellite based tracking requestor
class CLbsSatellitePositionTracker : public CLbsTrackTimerBase
{
public:
    static CLbsSatellitePositionTracker* NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);
    ~CLbsSatellitePositionTracker();
private:
    CLbsSatellitePositionTracker(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);
    void ConstructL();
};

//Non Satellite based tracking requestor
class CLbsNonSatellitePositionTracker : public CLbsTrackTimerBase
{
public:
    static CLbsNonSatellitePositionTracker* NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);

    ~CLbsNonSatellitePositionTracker();
private:
    CLbsNonSatellitePositionTracker(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver);
    void ConstructL();
};

class CLbsAllPosProxyState;

//Proxy which internally uses the satellite tracker and NW tracker
class CLbsAllPosProxy : public CLbsPositionTrackerBase, public MLbsTrackingCallback
{
public:

    static CLbsAllPosProxy* NewL(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver,
                                 CLbsSatellitePositionTracker *aSatelliteTracker = NULL, CLbsNonSatellitePositionTracker *aNWTracker = NULL);

    CLbsSatellitePositionTracker* ReleaseSatelliteTracker();
    CLbsNonSatellitePositionTracker* ReleaseNWTracker();

    ~CLbsAllPosProxy();

private:

    CLbsAllPosProxy(CPsyContainer *aContainer, MLbsTrackingCallback *aObserver,
                    CLbsSatellitePositionTracker *aSatelliteTracker = NULL, CLbsNonSatellitePositionTracker *aNWTracker = NULL);

    void ConstructL(CLbsSatellitePositionTracker *aSatelliteTracker , CLbsNonSatellitePositionTracker *aNWTracker);

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

    CLbsSatellitePositionTracker *iSatelliteTracker;
    CLbsNonSatellitePositionTracker *iNWTracker;

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


class CTrackingPsy : public CSelfManagingPsy
{
public:
    static CTrackingPsy* NewL(TPositionModuleInfo* aPsy, CPsyContainer *aContainer,
                              MPsyRequestCallback* aRequestCallback);
    virtual ~CTrackingPsy();
    void IssueRequest(TTimeIntervalMicroSeconds updateInterval);

protected:
    CTrackingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback, TPositionInfoBase &aPos);
    void ConstructL();
    void RunL();
};



QTM_END_NAMESPACE
QT_END_HEADER

#endif
