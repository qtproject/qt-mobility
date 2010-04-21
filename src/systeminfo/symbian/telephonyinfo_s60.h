/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <e32base.h>
#include <etel3rdparty.h>
#include <QString>
#include <QList>

class CActiveSchedulerWait;

class MTelephonyInfoObserver
{
public:
    virtual void batteryLevelChanged() = 0;

    virtual void countryCodeChanged() = 0;
    virtual void networkCodeChanged() = 0;
    virtual void networkNameChanged() = 0;
    virtual void networkModeChanged() = 0;

    virtual void cellNetworkSignalStrengthChanged() = 0;
    virtual void cellNetworkStatusChanged() = 0;
};

class CTelephonyInfo : public CActive
{
public:
    CTelephonyInfo(CTelephony &telephony);
    ~CTelephonyInfo();

    void addObserver(MTelephonyInfoObserver *observer);
    void removeObserver(MTelephonyInfoObserver *observer);

protected:  //from CActive
    void RunL();

protected:
    void makeRequest();

protected:
    CTelephony &m_telephony;
    QList<MTelephonyInfoObserver *> m_observers;
    CActiveSchedulerWait *m_wait;
};

class CPhoneInfo : public CTelephonyInfo
{
public:
    CPhoneInfo(CTelephony &telephony);

protected:
    void DoCancel();

public:
    QString imei() const;
    QString manufacturer() const;
    QString model() const;

private:
    CTelephony::TPhoneIdV1 m_phoneIdV1;
    CTelephony::TPhoneIdV1Pckg m_phoneIdV1Pckg;

    QString m_imei;
    QString m_manufacturer;
    QString m_model;
};

class CSubscriberInfo : public CTelephonyInfo
{
public:
    CSubscriberInfo(CTelephony &telephony);

protected:
    void DoCancel();

public:
    QString imsi() const;

private:
    CTelephony::TSubscriberIdV1 m_subscriberIdV1;
    CTelephony::TSubscriberIdV1Pckg m_subscriberIdV1Pckg;

    QString m_imsi;
};

class CBatteryInfo : public CTelephonyInfo
{
public:
    CBatteryInfo(CTelephony &telephony);
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    int batteryLevel() const;

private:
    bool m_initializing;

    CTelephony::TBatteryInfoV1 m_batteryInfoV1;
    CTelephony::TBatteryInfoV1Pckg m_batteryInfoV1Pckg;
    
    int m_batteryLevel;
    int m_previousBatteryLevel;
};

class CCellNetworkInfo : public CTelephonyInfo
{
public:
    CCellNetworkInfo(CTelephony &telephony);
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    int cellId() const;
	int locationAreaCode() const;

    QString countryCode() const;
    QString networkCode() const;
    QString networkName() const;
    CTelephony::TNetworkMode networkMode() const;

private:
    bool m_initializing;

    CTelephony::TNetworkInfoV1Pckg m_networkInfoV1Pckg;
    CTelephony::TNetworkInfoV1 m_networkInfoV1;

    int m_cellId;
    int m_locationAreaCode;

    QString m_networkId;
    QString m_previousNetworkId;

    QString m_countryCode;
    QString m_previousCountryCode;
    
    QString m_networkName;
    QString m_previousNetworkName;
    
    CTelephony::TNetworkMode m_networkMode;
    CTelephony::TNetworkMode m_previousNetworkMode;

};

class CCellNetworkRegistrationInfo : public CTelephonyInfo
{
public:
    CCellNetworkRegistrationInfo(CTelephony &telephony);
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    CTelephony::TRegistrationStatus cellNetworkStatus() const;

private:
    bool m_initializing;

    CTelephony::TNetworkRegistrationV1Pckg m_networkRegistrationV1Pckg;
    CTelephony::TNetworkRegistrationV1 m_networkRegistrationV1;

    CTelephony::TRegistrationStatus m_networkStatus;
    CTelephony::TRegistrationStatus m_previousNetworkStatus;
};

class CCellSignalStrengthInfo : public CTelephonyInfo
{
public:
    CCellSignalStrengthInfo(CTelephony &telephony);
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    int cellNetworkSignalStrength() const;

private:
    bool m_initializing;

    CTelephony::TSignalStrengthV1Pckg m_signalStrengthV1Pckg;
    CTelephony::TSignalStrengthV1 m_signalStrengthV1;

    int m_cellNetworkSignalStrength;
    int m_previousCellNetworkSignalStrength;

    int m_signalBar;
    int m_previousSignalBar;
};

#endif //DEVICEINFO_H
