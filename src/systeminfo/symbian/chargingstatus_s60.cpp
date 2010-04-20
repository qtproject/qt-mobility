/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "chargingstatus_s60.h"

CChargingStatus::CChargingStatus() : CActive(EPriorityStandard),
    m_currentStatus(EChargingStatusError)
{
    CActiveScheduler::Add(this);
    TInt err = m_chargingProperty.Attach(KPSUidHWRMPowerState, KHWRMChargingStatus, EOwnerThread);
    if (err == KErrNone) {
        int status = EChargingStatusError;
        m_chargingProperty.Get(status);
        m_currentStatus = (EPSHWRMChargingStatus)status;
        startMonitoring();
    }
}

CChargingStatus::~CChargingStatus()
{
    Cancel();
    m_chargingProperty.Close();
}

EPSHWRMChargingStatus CChargingStatus::chargingStatus() const
{
    return m_currentStatus;
}

void CChargingStatus::addObserver(MChargingStatusObserver *observer)
{
    m_observers.append(observer);
}

void CChargingStatus::removeObserver(MChargingStatusObserver *observer)
{
    m_observers.removeOne(observer);
}

void CChargingStatus::DoCancel()
{
    m_chargingProperty.Cancel();
}

void CChargingStatus::RunL()
{
    int status = EChargingStatusError;
    m_chargingProperty.Get(status);
    m_currentStatus = (EPSHWRMChargingStatus)status;
    
    foreach (MChargingStatusObserver *observer, m_observers)
        observer->chargingStatusChanged();
    
    startMonitoring();
}

void CChargingStatus::startMonitoring()
{
    m_chargingProperty.Subscribe(iStatus);
    SetActive();
}
