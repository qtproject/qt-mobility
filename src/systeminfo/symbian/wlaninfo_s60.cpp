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

#include <QObject>
#include <QTimer>

#include <wlanmgmtclient.h>
#include <wlanmgmtinterface.h>
#include <wlanmgmtcommon.h>
#include "wlaninfo_s60.h"

const int KWlanInfoSignalStrengthMax = 60;
const int KWlanInfoSignalStrengthMin = 100;

CWlanInfo::CWlanInfo(QObject *parent) : QObject(parent)
    , m_wlanStatus(false)
    , m_wlanSsid()
    , m_wlanSignalStrength(-1)
{
    TRAP_IGNORE(
        m_wlanMgmtClient = CWlanMgmtClient::NewL();
        m_wlanMgmtClient->ActivateNotificationsL(*this);
    )

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(checkWlanInfo()));
    m_timer->setInterval(1000);
    m_timer->start();
}

CWlanInfo::~CWlanInfo()
{
    m_wlanMgmtClient->CancelNotifications();
    delete m_wlanMgmtClient;
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
    TWlanConnectionMode connectionMode;
    TInt err = m_wlanMgmtClient->GetConnectionMode(connectionMode);
    if (err == KErrNone && connectionMode != EWlanConnectionModeNotConnected) {
        m_wlanStatus = true;
        emit wlanNetworkStatusChanged();
    } else {
        stopPolling();
        return;
    }

    TWlanSsid ssid;
    err = m_wlanMgmtClient->GetConnectionSsid(ssid);
    ssid.ZeroTerminate();
    if (err == KErrNone && m_wlanSsid != (char*)ssid.Ptr()) {
        m_wlanSsid = (char*)ssid.Ptr();
        emit wlanNetworkNameChanged();
    }

    TInt32 signalQuality;
    err = m_wlanMgmtClient->GetConnectionSignalQuality(signalQuality);

    if (err == KErrNone && m_wlanSignalStrength != signalQuality) {
        if (signalQuality <= KWlanInfoSignalStrengthMax && signalQuality >= 0)
            m_wlanSignalStrength = 100;
        else if (signalQuality >= KWlanInfoSignalStrengthMin)
            m_wlanSignalStrength = 0;
        else {
            m_wlanSignalStrength = (KWlanInfoSignalStrengthMin - signalQuality) * 100 /
            (KWlanInfoSignalStrengthMin - KWlanInfoSignalStrengthMax);
        }
        emit wlanNetworkSignalStrengthChanged();
    }
}

void CWlanInfo::ConnectionStateChanged(TWlanConnectionMode aNewState)
{
    if (aNewState == EWlanConnectionModeNotConnected)
        stopPolling();
    else {
        m_timer->start();
        checkWlanInfo();
    }
}

void CWlanInfo::stopPolling()
{
    m_timer->stop();
    m_wlanStatus = false;
    m_wlanSsid = "";
    m_wlanSignalStrength = 0;

    emit wlanNetworkStatusChanged();
    emit wlanNetworkNameChanged();
    emit wlanNetworkSignalStrengthChanged();
}
