/****************************************************************************
**
** Copyright (C) 2010-2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TELEPHONY_S60_H
#define TELEPHONY_S60_H

#include <e32base.h>
#include <etel3rdparty.h>
#include <QString>
#include <QList>
#include <QStack>
#include <QEventLoop>
#include <qDebug>
#include "trace.h"
#ifdef ETELMM_SUPPORTED
#include <etelmm.h>
#endif

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
    virtual void changedCellId(int) = 0;
};

#ifndef ETELMM_SUPPORTED
class CTelephonyInfo : public CActive
{
public:
    CTelephonyInfo(CTelephony &telephony);
    ~CTelephonyInfo();

    void addObserver(MTelephonyInfoObserver *observer);
    void removeObserver(MTelephonyInfoObserver *observer);

protected:
    void waitForRequest() const;
    void exitWait() const;

protected:
    CTelephony &m_telephony;
    QList<MTelephonyInfoObserver *> m_observers;
    mutable QStack<QEventLoop*> m_loops;
};

class CPhoneInfo : public CTelephonyInfo
{
public:
    CPhoneInfo(CTelephony &telephony);
    ~CPhoneInfo();

protected:
    void RunL();
    void DoCancel();

public:
    QString imei();
    QString manufacturer();
    QString model();

private:
    void makeRequest();
    bool m_initializing;
    mutable CTelephony::TPhoneIdV1 m_phoneIdV1;
    mutable CTelephony::TPhoneIdV1Pckg m_phoneIdV1Pckg;

    QString m_imei;
    QString m_manufacturer;
    QString m_model;
};

class CSubscriberInfo : public CTelephonyInfo
{
public:
    CSubscriberInfo(CTelephony &telephony);
    ~CSubscriberInfo();

protected:
    void RunL();
    void DoCancel();

public:
    QString imsi();

private:
    void makeRequest();
    bool m_initializing;
    mutable CTelephony::TSubscriberIdV1 m_subscriberIdV1;
    mutable CTelephony::TSubscriberIdV1Pckg m_subscriberIdV1Pckg;

    QString m_imsi;
};

class CBatteryInfo : public CTelephonyInfo
{
public:
    CBatteryInfo(CTelephony &telephony);
    ~CBatteryInfo();
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
    ~CCellNetworkInfo();
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
    QString homeNetworkCode();
    CTelephony::TNetworkMode networkMode() const;

private:
    bool m_initializing;

    CTelephony::TNetworkInfoV1Pckg m_networkInfoV1Pckg;
    CTelephony::TNetworkInfoV1 m_networkInfoV1;

    int m_cellId;
    int m_previouscellId;
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
    ~CCellNetworkRegistrationInfo();
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
    ~CCellSignalStrengthInfo();
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
#else //ETELMM_SUPPORTED
class CPhoneInfo : public CBase
{
public:
    CPhoneInfo(RMobilePhone &aMobilePhone);
    ~CPhoneInfo();
    void initialise();

public:
    QString imei();
    QString manufacturer();
    QString model();

private:
    RMobilePhone &m_rmobilePhone;
    bool m_phoneInfoinitialised;
    QString m_imei;
    QString m_manufacturer;
    QString m_model;
};

class CSubscriberInfo : public CBase
{
public:
    CSubscriberInfo(RMobilePhone &aMobilePhone);
    ~CSubscriberInfo();
    void initialise();

public:
    QString imsi();

private:
    RMobilePhone &m_rmobilePhone;
    bool m_subscriberInfoinitialised;
    QString m_imsi;
};


class CEtelInfo : public CActive
{
public:
    CEtelInfo(RMobilePhone &aMobilePhone);
    ~CEtelInfo();
    void addObserver(MTelephonyInfoObserver *observer);
    void removeObserver(MTelephonyInfoObserver *observer);

protected:
    RMobilePhone &m_rmobilePhone;
    QList<MTelephonyInfoObserver *> m_observers;
    bool m_initialised;
};


class CBatteryInfo : public CEtelInfo
{
public:
    CBatteryInfo(RMobilePhone &aMobilePhone);
    ~CBatteryInfo();
    void initialise();
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    int batteryLevel() const;

private:
    int m_batteryLevel;
    RMobilePhone::TMobilePhoneBatteryInfoV1 m_batteryinfo;
};


class CCellSignalStrengthInfo : public CEtelInfo
{
public:
    CCellSignalStrengthInfo(RMobilePhone &aMobilePhone);
    ~CCellSignalStrengthInfo();
    void initialise();
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    int cellNetworkSignalStrength() const;

private:
    TInt32 m_cellNetworkSignalStrength;
    TInt8 m_signalBar;
    TInt32 m_prevcellNetworkSignalStrength;
    TInt8 m_prevsignalBar;
};

class CCellNetworkRegistrationInfo : public CEtelInfo
{
public:
    CCellNetworkRegistrationInfo(RMobilePhone &aMobilePhone);
    ~CCellNetworkRegistrationInfo();
    void initialise();
    void startMonitoring();

protected:
    void RunL();
    void DoCancel();

public:
    RMobilePhone::TMobilePhoneRegistrationStatus cellNetworkStatus() const;

private:
    RMobilePhone::TMobilePhoneRegistrationStatus m_networkStatus;
    RMobilePhone::TMobilePhoneRegistrationStatus m_previousNetworkStatus;
};

class CCellNetworkInfo : public CEtelInfo
{
public:
    CCellNetworkInfo(RMobilePhone &aMobilePhone);
    ~CCellNetworkInfo();
    void initialise();
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
    QString homeNetworkCode();
    CTelephony::TNetworkMode networkMode() const;/*RMobilePhone::TMobilePhoneNetworkMode <=> CTelePhony::TNetworkMode*/

private:
    /** Network info of mobile phone. */
    RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
    /** Location of mobile phone. */
    RMobilePhone::TMobilePhoneLocationAreaV1 iLocation;
    /** Packaged network info object. */
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iNetworkInfoPckg;

    int m_cellId;
    int m_previouscellId;
    int m_locationAreaCode;

    QString m_networkId;
    QString m_previousNetworkId;

    QString m_countryCode;
    QString m_previousCountryCode;

    QString m_networkName;
    QString m_previousNetworkName;

    RMobilePhone::TMobilePhoneNetworkMode m_networkMode;
    RMobilePhone::TMobilePhoneNetworkMode m_previousNetworkMode;
};
#endif //End ETELMM_SUPPORTED

#endif //TELEPHONY_S60_H
