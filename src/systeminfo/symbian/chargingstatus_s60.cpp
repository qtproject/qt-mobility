/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
  if (!IsActive()) {
    m_chargingProperty.Subscribe(iStatus);
    SetActive();
  }
}
