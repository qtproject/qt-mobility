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

#include "qmlbackendmonitorao_s60_p.h"
#include "qgeoareamonitor_s60_p.h"
#include "qmlbackendmonitorinfo_s60_p.h"

#include <lbtsessiontrigger.h>
#include <lbtgeocircle.h>
#include <lbttriggerconditionarea.h>
#include <lbttriggerentry.h>

QTM_BEGIN_NAMESPACE


//static member of the QMLBackendMonitorAO object holding the address of the object
QMLBackendMonitorAO* QMLBackendMonitorAO::iBackendMonitorAO = NULL;

//static member of the QMLBackendMonitorAO object maintaining the reference count
TInt QMLBackendMonitorAO::refCount = 0;

//request for notification of the trigger fired events
void QMLBackendMonitorAO::NotifyFiredEvent()
{
    if (!IsActive()) {
        iLbt.NotifyTriggerFired(iTriggerInfo, iStatus);
        SetActive();
    }
}

//static function called prior to the destruction of the
//singleton QMLBackendMonitorAO object
void QMLBackendMonitorAO::DeleteAO(QGeoAreaMonitorS60* aParent)
{
    //decrement the reference count
    refCount--;
    if (refCount == 0) {
        delete iBackendMonitorAO;
        iBackendMonitorAO = NULL;
    }
}

QMLBackendMonitorAO::~QMLBackendMonitorAO()
{
    Cancel();
    delete iTriggerMonitorInfo; //deletes the CBackendMonitorInfo object holding the linked list
    iLbt.Close();   //closes the subsession
}


void  QMLBackendMonitorAO::DoCancel()
{
    if (IsActive()) { //if request is still active,cancel the CancelNotifyTriggerFired request
        iLbt.CancelNotifyTriggerFired();
    }
}

void QMLBackendMonitorAO::RunL()
{
    CMonitorTriggerInfo *triggerInfo = NULL;
    switch (iStatus.Int()) {
        case KErrNone :
            //retrieve the triggerInfo corresponding to iTriggerInfo.iTriggerId
            triggerInfo = iTriggerMonitorInfo->getMonitorTriggerInfo(
                              iTriggerInfo.iTriggerId);
            if (triggerInfo) {
                //callback called only if generated for the current AO - Trigger ID
                (triggerInfo->iParent)->handleTriggerEvent(iTriggerInfo.iFiredPositionInfo ,
                        triggerInfo->iType);
            }
            break;
        default :
            break;
    }

    //request for any trigger fired event, for any triggers owned by the client
    NotifyFiredEvent();
}

QMLBackendMonitorAO* QMLBackendMonitorAO::NewL(RLbtServer &aLbt)
{
    //increment the reference count
    refCount++;
    if (!iBackendMonitorAO) {
        iBackendMonitorAO = QMLBackendMonitorAO::NewLC(aLbt);
        CleanupStack::Pop();
    }
    return iBackendMonitorAO;
}

QMLBackendMonitorAO* QMLBackendMonitorAO::NewLC(RLbtServer &aLbtServer)
{
    QMLBackendMonitorAO *self = new(ELeave) QMLBackendMonitorAO;
    CleanupStack::PushL(self);
    self->ConstructL(aLbtServer);
    if (!self->isValid()) {
        delete self;
        self = NULL;
    }
    return self;
}

void QMLBackendMonitorAO::ConstructL(RLbtServer &aLbtServ)
{
    if (iLbt.Open(aLbtServ) == KErrNone) { //opens the subsession
        subsessionCreated = TRUE;
        iTriggerMonitorInfo = CBackendMonitorInfo::NewL();
    }
}

QMLBackendMonitorAO::QMLBackendMonitorAO()
        : CActive(EPriorityStandard), // Standard priority
        subsessionCreated(FALSE)
{
    CActiveScheduler::Add(this);
}

QTM_END_NAMESPACE

