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


#include "symbian_lbsonetime_p.h"
#include "symbian_lbsfacade_p.h"


QTM_BEGIN_NAMESPACE

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
        : CSelfManagingPsy(aPsy, aContainer, aRequestCallback, iPositionInfo)
{}
void CSingleShotPsy::ConstructL()
{

    CSelfManagingPsy::ConstructL();
    qDebug() << "CSingleShotPsy::ConstructL " << iPsyName ;
}

void CSingleShotPsy::IssueRequest(TTimeIntervalMicroSeconds aTimeOut)
{
    qDebug() << "CSingleShotPsy::IssueRequest " << iPsyName ;

    iClientRequestActive = ETrue;
    TTimeIntervalMicroSeconds intr(60000000) ;//60 sec, doesn't matter
    CSelfManagingPsy::IssueRequest(intr, aTimeOut);
}

void CSingleShotPsy::RunL()
{
    qDebug() << "CSingleShotPsy::RunL " << iPsyName << " iStatus=" << iStatus.Int();
    HandlePositionCallback(iStatus);
    CancelPositionRequest();
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
void CLbsSingleShotRequestor::RequestTimedOut(CSelfManagingPsy *aPsy)
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
