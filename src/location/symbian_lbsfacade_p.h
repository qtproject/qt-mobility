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

#ifndef SYMBIAN_LBSFACADE_H
#define SYMBIAN_LBSFACADE_H

#include <lbs.h>
#include <lbscommon.h>
#include <EPos_MPosModulesObserver.h>
#include <EPos_CPosModules.h>

#include <QObject>
#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

//Forward Declarationa
class CSelfManagingPsy;
class CPsyContainer;

const TInt MAX_TIMER = 2147483647 ;
typedef TInt TMilliSeconds;

class MPsyCountChangeCallback
{
public:
    virtual void PsyUninstalled(TPositionModuleId &aModId) = 0;
    virtual void PsyInstalled(TPositionModuleInfo* aModInfo) = 0;
};

class MPsyStatusChangeCallback
{
public:
    virtual void NotifyPsyStatusChange(TBool aAvailability) = 0;
};

class CPsyContainer : public CBase, public MPosModulesObserver
{
public:
    static CPsyContainer* NewL();
    ~CPsyContainer();

    void GetAvailablePsys(TBool aNWPsys, RArray<TPositionModuleInfo*> &aPsys);
    void SetNonSatellitePsyCountChangeCallback(MPsyCountChangeCallback *aNWCallback);
    void SetSatellitePsyCountChangeCallback(MPsyCountChangeCallback *aGPSCallback);
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

class MPsyRequestCallback
{
public:
    virtual void LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy) = 0;
    virtual void RequestTimedOut(CSelfManagingPsy *aPsy) = 0;
};

//Abstract class. Derived concrete classes must implement RunL
class CSelfManagingPsy : public CActive, public MPsyStatusChangeCallback
{
public:
    QString iPsyName;//Only for debugging purposes
    TPositionModuleInfo* GetModInfo();
    void Cancel();
protected:
    CSelfManagingPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer,
                     MPsyRequestCallback* aRequestCallback, TPositionInfoBase &aPos);
    virtual ~CSelfManagingPsy();
    void ConstructL();
    void DoCancel();
    void IssueRequest(TTimeIntervalMicroSeconds aUpdInterval, TTimeIntervalMicroSeconds aTimeOut);
    void RequestPositionAgain();//Should be called only after calling IssueRequest
    void CancelPositionRequest();

    //Can be called from the derived class RunL to notify observer.
    void HandlePositionCallback(TRequestStatus &aStatus);

    TBool iClientRequestActive;
    TTimeIntervalMicroSeconds iUpdInterval;
    TTimeIntervalMicroSeconds iTimeout;
private:
    TBool InitPositioner();
    //MPsyStatusChangeCallback
    void NotifyPsyStatusChange(TBool aAvailability);
    TInt RunError();

    //Derviced class should use the appropriate position info class
    TPositionInfoBase &iPosInfo;
    RPositioner *iPositioner;
    CPsyContainer *iContainer;
    MPsyRequestCallback* iRequestCallback;
    TBool iPsyAvailabilityStatus;
    TPositionModuleInfo* iPsyModInfo;
};

class PsyUtils
{
public:
    static void TPositionInfo2QGeoPositionInfo(TPositionInfoBase &aPosInfo, QGeoPositionInfo& aQPosInfo);
    static TBool IsSatellitePsy(TPositionModuleInfo *aModInfo);
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif
