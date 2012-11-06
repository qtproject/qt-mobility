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

#ifndef __CLBS_PSYCONTAINER_H
#define __CLBS_PSYCONTAINER_H

#include <e32std.h>
#include <e32base.h>
#include <lbs.h>
#include <lbscommon.h>
#include <e32property.h>


#include <QObject>
#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"
#include <epos_mposmodulesobserver.h>

#include <epos_cposmodules.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class CSelfManagingPsy;
class CPsyContainer;


class MPsyStatusChangeCallback
{
public:
    virtual void NotifyPsyStatusChange(TBool aAvailability) = 0;
};


class MPsyRequestCallback
{
public:
    virtual void LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy) = 0;
    virtual void RequestTimedOut(TInt aConsecutiveFailureCount, CSelfManagingPsy *aPsy) = 0;

};


class CSelfManagingPsy : public CActive, public MPsyStatusChangeCallback
{
public:
    QString iPsyName;//Only for debugging purposes

    TPositionModuleInfo* GetModInfo();

    void Cancel();
protected:
    CSelfManagingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer,
                     MPsyRequestCallback* aRequestCallback);
    virtual ~CSelfManagingPsy();

    void ConstructL();
    void DoCancel();

    void IssueRequest();

    TBool InitPositioner();

    //MPsyStatusChangeCallback
    void NotifyPsyStatusChange(TBool aAvailability);

    void HandlePositionCallback(TRequestStatus &aStatus);

    TPositionModuleInfo* iPsyModInfo;
    CPsyContainer *iContainer;
    RPositioner *iPositioner;
    MPsyRequestCallback* iRequestCallback;
    TPositionInfo iPosInfo;
    TInt iConsequitiveFailureCount;

    TBool iClientRequestActive;
    TBool iPsyAvailabilityStatus;

    TTimeIntervalMicroSeconds iUpdInterval;
    TTimeIntervalMicroSeconds iTimeout;
};

class CTrackingPsy : public CSelfManagingPsy
{
public:
    static CTrackingPsy* NewL(TPositionModuleInfo* aPsy, CPsyContainer *aContainer,
                              MPsyRequestCallback* aRequestCallback);

    void IssueRequest(TTimeIntervalMicroSeconds updateInterval);
    void RunL();

    virtual ~CTrackingPsy();

protected:
    CTrackingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback);
    void ConstructL();
};

//Special class for handling NW PSYs
//Gaps b/w requests will be atleast 'x' seconds
class CNWTrackingPsy : public CTrackingPsy
{
private:
    CNWTrackingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback);
    ~CNWTrackingPsy();

    void DoCancel();
    void RunL();


    bool iRealRequest;

    RTimer iTimer;
    friend class CTrackingPsy;
};

class CSingleShotPsy : public CSelfManagingPsy
{
public:
    static CSingleShotPsy* NewL(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback);
    void RunL();

    void IssueRequest(TTimeIntervalMicroSeconds aTimeOut);
protected:
    CSingleShotPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback);
    void ConstructL();

};


class MPsyCountChangeCallback
{
public:
    virtual void PsyUninstalled(TPositionModuleId &aModId) = 0;
    virtual void PsyInstalled(TPositionModuleInfo* aModInfo) = 0;
};

class MMWPsyChangeCallback
{
    virtual void AvailableModulesChanged() = 0;
};

class MGpsPsyChangeCallback
{
    virtual void AvailableModulesChanged() = 0;
};

class CPsyContainer : public CBase, public MPosModulesObserver
{
public:
    static CPsyContainer* NewL();

    ~CPsyContainer();

    void GetAvailablePsys(TBool aNWPsys, RArray<TPositionModuleInfo*> &aPsys);


    void SetNWPSYChangeCallback(MPsyCountChangeCallback *aNWCallback);
    void SetGPSPSYChangeCallback(MPsyCountChangeCallback *aGPSCallback);

    void RegisterPsyStatusListenerL(MPsyStatusChangeCallback *aCallBack, TPositionModuleId aModId);
    void DeRegisterPsyStatusListener(MPsyStatusChangeCallback *aCallBack);

    RPositionServer& PosServer();

    QGeoPositionInfo lastKnownPosition(bool aFromSatellitePositioningMethodsOnly = false) ;

private:
    CPsyContainer();
    void ConstructL();


    void InitAllPsysL();

    // from MPosModulesObserver
    virtual void HandleSettingsChangeL(TPosModulesEvent aEvent);

    //MLbsLastKnownPosObserver
    void NotifyLastKnownPosUpdate(TPositionInfo& aPositionInfo);

    TBool IsSatellitePsy(TPositionModuleInfo *aModInfo) const;
    TBool IsNWPsy(TPositionModuleInfo *aModInfo) const;

private:
    RPositionServer iPosServer;

    MPsyCountChangeCallback *iNWCallback;
    MPsyCountChangeCallback *iGPSCallback;

    RArray<TPositionModuleInfo*> iAllPsys;
    TPositionModuleStatusEvent iModuleStatusEvent;

    struct TPsyStatusListener {
        MPsyStatusChangeCallback *iCallBack;
        TPositionModuleId iModId;
    };

    RArray<TPsyStatusListener*> iPsyStatusListeners;
    CPosModules* iPsyModules;


};

class PsyUtils
{
public:
    static void TPositionInfo2QGeoPositionInfo(TPositionInfoBase &aPosInfo, QGeoPositionInfo& aQPosInfo);

};

QTM_END_NAMESPACE
QT_END_HEADER
#endif


