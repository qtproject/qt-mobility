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

#ifndef SYMBIAN_LBSONETIME_H
#define SYMBIAN_LBSONETIME_H


#include <QObject>
#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"

#include "symbian_lbsfacade_p.h"
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"


QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class CSingleShotPsy;

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
    ~CLbsSingleShotRequestor();
    void Cancel();

    void RequestLocation(TMilliSeconds &aMsec);
    SSType GetSingleShotType() const;

private:
    CLbsSingleShotRequestor(CPsyContainer *aContainer, MLbsSingleShotCallback *aObserver, SSType aSSType);
    void ConstructL();

    void RunL();
    void DoCancel();

    bool StartTimer();

    //MPsyRequestCallback
    void LocationUpdate(TPositionInfoBase &aPosition, CSelfManagingPsy *aPsy);
    void RequestTimedOut(CSelfManagingPsy *aPsy);

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

class CSingleShotPsy : public CSelfManagingPsy
{
public:
    static CSingleShotPsy* NewL(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback);
    void RunL();

    void IssueRequest(TTimeIntervalMicroSeconds aTimeOut);
protected:
    CSingleShotPsy(TPositionModuleInfo* aPsy, CPsyContainer *aContainer, MPsyRequestCallback* aRequestCallback);
    void ConstructL();

    TPositionInfo iPositionInfo;
};


QTM_END_NAMESPACE
QT_END_HEADER

#endif
