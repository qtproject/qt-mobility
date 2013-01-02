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

#ifndef WLANINFO_S60_H
#define WLANINFO_S60_H

#include <QObject>
#include <QTimer>

#include <wlanmgmtclient.h>
#include <wlanmgmtinterface.h>
#include <wlanmgmtcommon.h>

class MWlanInfoObserver
{
public:
    virtual void wlanNetworkNameChanged() = 0;
    virtual void wlanNetworkSignalStrengthChanged() = 0;
    virtual void wlanNetworkStatusChanged() = 0;
};

class CWlanInfo : public CActive, public MWlanMgmtNotifications
{
public:
    CWlanInfo();
    ~CWlanInfo();

    QString wlanNetworkName() const;
    int wlanNetworkSignalStrength() const;
    bool wlanNetworkConnectionStatus() const;
    void addObserver(MWlanInfoObserver *observer);
    void removeObserver(MWlanInfoObserver *observer);

private:
    void stopPolling();

protected: // MWlanMgmtNotifications
    void RunL();
    void DoCancel();
    void StartMonitoring();
    void ConnectionStateChanged(TWlanConnectionMode aNewState);
    void BssidChanged(TWlanBssid& aNewBSSID) {};
    void BssLost() {};
    void BssRegained() {};
    void NewNetworksDetected() {};
    void OldNetworksLost() {};
    void TransmitPowerChanged(TUint) {};
    void RssChanged(TWlanRssClass , TUint strength) {};
    static TInt TimeOut(TAny*);
    void FreeResources();

private:
    void initWlanInfo();
    void checkWlanInfo();

    void updateSignalStrength(TInt32 signalQuality);
    QString ssidToString(TWlanSsid ssid);

private:
    CWlanMgmtClient *m_wlanMgmtClient;
    bool m_wlanStatus;
    QString m_wlanSsid;
    int m_wlanSignalStrength;
    CPeriodic *m_timer;
    QList<MWlanInfoObserver *> m_observers;
};

#endif //WLANINFO_S60_H
