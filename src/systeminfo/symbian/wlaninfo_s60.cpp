/****************************************************************************
**
** Copyright (C) 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QObject>
#include <QTimer>

#include <wlanmgmtclient.h>
#include <wlanmgmtinterface.h>
#include <wlanmgmtcommon.h>
#include "wlaninfo_s60.h"
#include "trace.h"

const int KWlanInfoSignalStrengthMax = 60;
const int KWlanInfoSignalStrengthMin = 100;

CWlanInfo::CWlanInfo() : CActive(EPriorityStandard),
      m_wlanMgmtClient(NULL)
    , m_wlanStatus(false)
    , m_wlanSsid()
    , m_wlanSignalStrength(-1)
{
 TRACES(qDebug() << "CWlanInfo::CWlanInfo<--");
 CActiveScheduler::Add(this);
#ifndef __WINSCW__
    TRAP_IGNORE( m_wlanMgmtClient = CWlanMgmtClient::NewL();)
        if (m_wlanMgmtClient) {
            m_wlanMgmtClient->ActivateNotificationsL(*this);
            StartMonitoring();
       }
#endif
 TRACES(qDebug() << "CWlanInfo::CWlanInfo-->");
}

CWlanInfo::~CWlanInfo()
{
}

void CWlanInfo::addObserver(MWlanInfoObserver *observer)
{
  m_observers.append(observer);
}

void CWlanInfo::removeObserver(MWlanInfoObserver *observer)
{
  m_observers.removeOne(observer);
}

void CWlanInfo::StartMonitoring()
 {
  TRACES(qDebug() << "CWlanInfo::StartMonitoring-->");
  if (IsActive()) {
   TRACES(qDebug() << "CWlanInfo::already active-->");
   return;
  }
  iStatus = KRequestPending;
  SetActive();
  TRACES(qDebug() << "CWlanInfo::StartMonitoring<--");
 }

void CWlanInfo::FreeResources()
 {
 TRACES(qDebug() << "CWlanInfo::FreeResources<--");
    if (m_wlanMgmtClient)
        m_wlanMgmtClient->CancelNotifications();
    delete m_wlanMgmtClient;
 TRACES(qDebug() << "CWlanInfo::FreeResources-->");
 }

QString CWlanInfo::wlanNetworkName() const
{
    return m_wlanSsid;
}

int CWlanInfo::wlanNetworkSignalStrength() const
{
    return m_wlanSignalStrength;
}

bool CWlanInfo::wlanNetworkConnectionStatus() const
{
    return m_wlanStatus;
}

void CWlanInfo::checkWlanInfo()
 {
  TRACES(qDebug() << "CWlanInfo::checkWlanInfo<---");
#ifndef __WINSCW__
    if(!m_wlanMgmtClient)
        return;

    TInt32 signalQuality;
    TInt err = m_wlanMgmtClient->GetConnectionSignalQuality(signalQuality);
    if (err == KErrNone && m_wlanSignalStrength != signalQuality) {
        if (signalQuality <= KWlanInfoSignalStrengthMax && signalQuality >= 0)
            m_wlanSignalStrength = 100;
        else if (signalQuality >= KWlanInfoSignalStrengthMin)
            m_wlanSignalStrength = 0;
        else {
            m_wlanSignalStrength = (KWlanInfoSignalStrengthMin - signalQuality) * 100 /
            (KWlanInfoSignalStrengthMin - KWlanInfoSignalStrengthMax);
        }
        //emit wlanNetworkSignalStrengthChanged();
        foreach (MWlanInfoObserver *observer, m_observers)
         observer->wlanNetworkSignalStrengthChanged();
    }
 TRACES(qDebug() << "CWlanInfo::checkWlanInfo--->");
#endif
}

TInt CWlanInfo::TimeOut(TAny* aAny)
 {
  TRACES(qDebug() << "CWlanInfo::TimeOut<---");
  CWlanInfo* self = static_cast<CWlanInfo*>( aAny );
  self->checkWlanInfo();
  TRACES(qDebug() << "CWlanInfo::TimeOut--->");
  return KErrNone; // Return value ignored by CPeriodic
 }

void CWlanInfo::ConnectionStateChanged(TWlanConnectionMode aNewState)
{
 TRACES(qDebug() << "CWlanInfo::ConnectionStateChanged<---");
    TWlanSsid ssid;
    TInt err;
    if (aNewState == EWlanConnectionModeNotConnected) {
     TRACES(qDebug() << "CWlanInfo::ConnectionStateChanged--Disconnected");
        stopPolling();
    }
    else {
        if (m_wlanStatus == false) {
            TRACES(qDebug() << "CWlanInfo::ConnectionStateChanged--Connected");
            m_wlanStatus = true;
            //emit wlanNetworkStatusChanged();
            foreach (MWlanInfoObserver *observer, m_observers)
              observer->wlanNetworkStatusChanged();

            err = m_wlanMgmtClient->GetConnectionSsid(ssid);
            TRACES(qDebug() << "Error code: GetConnectionSsid():" << err);

            if (err == KErrNone && m_wlanSsid != QString::fromAscii((char*)ssid.Ptr(), ssid.Length())) {
                m_wlanSsid = QString::fromAscii((char*)ssid.Ptr(), ssid.Length());
                //emit wlanNetworkNameChanged();
                foreach (MWlanInfoObserver *observer, m_observers)
                  observer->wlanNetworkNameChanged();
            }

            iStatus = KErrNone;
            CWlanInfo::RunL();
            checkWlanInfo();
        }
    }
 TRACES(qDebug() << "CWlanInfo::ConnectionStateChanged--->");
}

void CWlanInfo::stopPolling()
{
 TRACES(qDebug() << "CWlanInfo::stopPolling<---");
    m_timer->Cancel();
    delete m_timer;
    m_timer = NULL;
    m_wlanStatus = false;
    m_wlanSsid = "";
    m_wlanSignalStrength = 0;

    //emit wlanNetworkStatusChanged();
    //emit wlanNetworkNameChanged();
    //emit wlanNetworkSignalStrengthChanged();
    foreach (MWlanInfoObserver *observer, m_observers) {
           observer->wlanNetworkStatusChanged();
           observer->wlanNetworkNameChanged();
           observer->wlanNetworkSignalStrengthChanged();
     }
    StartMonitoring();
 TRACES(qDebug() << "CWlanInfo::stopPolling--->");
}


void CWlanInfo::RunL()
  {
   TRACES(qDebug() << "CWlanInfo::RunL<---");
   TRACES(qDebug() << "CWlanInfo::RunL: istatus:" << iStatus.Int());
   m_timer = CPeriodic::NewL(CActive::EPriorityIdle);
   m_timer->Start(5000000,5000000,TCallBack(TimeOut, this));
   //StartMonitoring();
   TRACES(qDebug() << "CWlanInfo::RunL--->");
  }

void CWlanInfo::DoCancel()
 {
  TRACES(qDebug() << "CWlanInfo::DoCancel<---");
  TRACES(qDebug() << "CWlanInfo::FreeResources<--");
    if (m_wlanMgmtClient)
        m_wlanMgmtClient->CancelNotifications();
    delete m_wlanMgmtClient;
  TRACES(qDebug() << "CWlanInfo::FreeResources-->");
  TRACES(qDebug() << "CWlanInfo::DoCancel--->");
 }

