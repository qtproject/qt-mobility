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

#include "telephonyinfo_s60.h"

CTelephonyInfo::CTelephonyInfo(CTelephony &telephony) : CActive(EPriorityStandard),
    m_telephony(telephony)
{
    CActiveScheduler::Add(this);
    m_wait = new CActiveSchedulerWait();
}

CTelephonyInfo::~CTelephonyInfo()
{
    Cancel();
    delete m_wait;
}

void CTelephonyInfo::addObserver(MTelephonyInfoObserver *observer)
{
    m_observers.append(observer);
}

void CTelephonyInfo::removeObserver(MTelephonyInfoObserver *observer)
{
    m_observers.removeOne(observer);
}

void CTelephonyInfo::RunL()
{
    m_wait->AsyncStop();
}

void CTelephonyInfo::makeRequest()
{
    SetActive();
    
    if (!m_wait->IsStarted()) {
        m_wait->Start();
    }
}

CPhoneInfo::CPhoneInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_phoneIdV1Pckg(m_phoneIdV1)
{
    m_telephony.GetPhoneId(iStatus, m_phoneIdV1Pckg);
    makeRequest();

    TBuf<CTelephony::KPhoneSerialNumberSize> imei = m_phoneIdV1.iSerialNumber;
    m_imei = QString::fromUtf16(imei.Ptr(), imei.Length());

    TBuf<CTelephony::KPhoneManufacturerIdSize> manufacturer = m_phoneIdV1.iManufacturer;
    m_manufacturer = QString::fromUtf16(manufacturer.Ptr(), manufacturer.Length());

    TBuf<CTelephony::KPhoneModelIdSize> model = m_phoneIdV1.iModel;
    m_model = QString::fromUtf16(model.Ptr(), model.Length());
}

void CPhoneInfo::DoCancel()
{
   m_telephony.CancelAsync(CTelephony::EGetPhoneIdCancel);
}

QString CPhoneInfo::imei() const
{
    return m_imei;
}

QString CPhoneInfo::manufacturer() const
{
    return m_manufacturer;
}

QString CPhoneInfo::model() const
{
    return m_model;
}

CSubscriberInfo::CSubscriberInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_subscriberIdV1Pckg(m_subscriberIdV1)
{
    m_telephony.GetSubscriberId(iStatus, m_subscriberIdV1Pckg);
    makeRequest();

    TBuf<CTelephony::KIMSISize> imsi = m_subscriberIdV1.iSubscriberId;
    m_imsi = QString::fromUtf16(imsi.Ptr(), imsi.Length());
}

void CSubscriberInfo::DoCancel()
{
    m_telephony.CancelAsync(CTelephony::EGetSubscriberIdCancel);
}

QString CSubscriberInfo::imsi() const
{
    return m_imsi;
}

CBatteryInfo::CBatteryInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_batteryInfoV1Pckg(m_batteryInfoV1)
{
    m_telephony.GetBatteryInfo(iStatus, m_batteryInfoV1Pckg);
    makeRequest();

    m_batteryLevel = m_batteryInfoV1.iChargeLevel;
    m_previousBatteryLevel = m_batteryLevel;

    m_initializing = false;

    startMonitoring();
}

void CBatteryInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_batteryLevel = m_batteryInfoV1.iChargeLevel;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_batteryLevel != m_previousBatteryLevel) {
                observer->batteryLevelChanged();
            }
        }
        m_previousBatteryLevel = m_batteryLevel;
        startMonitoring();
    }
}

void CBatteryInfo::DoCancel()
{
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetBatteryInfoCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::EBatteryInfoChangeCancel);
    }
}

int CBatteryInfo::batteryLevel() const
{
    return m_batteryLevel;
}

void CBatteryInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::EBatteryInfoChange, m_batteryInfoV1Pckg);
    SetActive();
}

CCellNetworkInfo::CCellNetworkInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_networkInfoV1Pckg(m_networkInfoV1)
{
    m_telephony.GetCurrentNetworkInfo(iStatus, m_networkInfoV1Pckg);
    makeRequest();    

    m_cellId = m_networkInfoV1.iCellId;
    m_locationAreaCode = m_networkInfoV1.iLocationAreaCode;

	TBuf<CTelephony::KNetworkIdentitySize> networkId = m_networkInfoV1.iNetworkId;
    m_networkId = QString::fromUtf16(networkId.Ptr(), networkId.Length());
    m_previousNetworkId = m_networkId;

    TBuf<CTelephony::KNetworkCountryCodeSize> countryCode = m_networkInfoV1.iCountryCode;
    m_countryCode = QString::fromUtf16(countryCode.Ptr(), countryCode.Length());
    m_previousCountryCode = m_countryCode;
    TBuf<CTelephony::KNetworkLongNameSize> longName = m_networkInfoV1.iLongName;
    if (longName.Length() > 0) {
        m_networkName = QString::fromUtf16(longName.Ptr(), longName.Length());
    } else {
        TBuf<CTelephony::KNetworkDisplayTagSize> displayTag = m_networkInfoV1.iDisplayTag;
        m_networkName = QString::fromUtf16(displayTag.Ptr(), displayTag.Length());
    }
    m_previousNetworkName = m_networkName;

    m_networkMode = m_networkInfoV1.iMode;
    m_previousNetworkMode = m_networkMode;

    m_initializing = false;
    
    startMonitoring();    
}

void CCellNetworkInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_cellId = m_networkInfoV1.iCellId;
        m_locationAreaCode = m_networkInfoV1.iLocationAreaCode;

        TBuf<CTelephony::KNetworkIdentitySize> networkId = m_networkInfoV1.iNetworkId;
            m_networkId = QString::fromUtf16(networkId.Ptr(), networkId.Length());

        TBuf<CTelephony::KNetworkCountryCodeSize> countryCode = m_networkInfoV1.iCountryCode;
            m_countryCode = QString::fromUtf16(countryCode.Ptr(), countryCode.Length());

        TBuf<CTelephony::KNetworkLongNameSize> longName = m_networkInfoV1.iLongName;
        if (longName.Length() > 0) {
            m_networkName = QString::fromUtf16(longName.Ptr(), longName.Length());
        } else {
            TBuf<CTelephony::KNetworkDisplayTagSize> displayTag = m_networkInfoV1.iDisplayTag;
            m_networkName = QString::fromUtf16(displayTag.Ptr(), displayTag.Length());
        }

        m_networkMode = m_networkInfoV1.iMode;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_networkId != m_previousNetworkId) {
                observer->networkCodeChanged();
            }
            if (m_countryCode != m_previousCountryCode) {
                observer->countryCodeChanged();
            }
            if (m_networkName != m_previousNetworkName) {
                observer->networkNameChanged();
            }
            if (m_networkMode != m_previousNetworkMode) {
                observer->networkModeChanged();
            }
        }
        m_previousNetworkId = m_networkId;
        m_previousCountryCode = m_countryCode;
        m_previousNetworkName = m_networkName;
        m_previousNetworkMode = m_networkMode;
        startMonitoring();
    }
}

void CCellNetworkInfo::DoCancel()
{
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
    }
}

int CCellNetworkInfo::cellId() const
{
    return m_cellId;
}

int CCellNetworkInfo::locationAreaCode() const
{
    return m_locationAreaCode;
}

QString CCellNetworkInfo::countryCode() const
{
    return m_countryCode;
}

QString CCellNetworkInfo::networkCode() const
{
    return m_networkId;
}

QString CCellNetworkInfo::networkName() const
{
    return m_networkName;
}

CTelephony::TNetworkMode CCellNetworkInfo::networkMode() const
{
    return m_networkMode;
}


void CCellNetworkInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::ECurrentNetworkInfoChange, m_networkInfoV1Pckg);
    SetActive();
}

CCellNetworkRegistrationInfo::CCellNetworkRegistrationInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_networkRegistrationV1Pckg(m_networkRegistrationV1)
{
    m_telephony.GetNetworkRegistrationStatus(iStatus, m_networkRegistrationV1Pckg);
    makeRequest();

    m_networkStatus = m_networkRegistrationV1.iRegStatus;
    m_previousNetworkStatus = m_networkStatus;

    m_initializing = false;
    
    startMonitoring();
}

void CCellNetworkRegistrationInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_networkStatus = m_networkRegistrationV1.iRegStatus;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_networkStatus != m_previousNetworkStatus) {
                observer->cellNetworkStatusChanged();
            }
        }
        m_previousNetworkStatus = m_networkStatus;
        startMonitoring();
    }
}

void CCellNetworkRegistrationInfo::DoCancel()
{
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
    }
}

CTelephony::TRegistrationStatus CCellNetworkRegistrationInfo::cellNetworkStatus() const
{
    return m_networkStatus;
}

void CCellNetworkRegistrationInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::ENetworkRegistrationStatusChange, m_networkRegistrationV1Pckg);
    SetActive();
}

CCellSignalStrengthInfo::CCellSignalStrengthInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_signalStrengthV1Pckg(m_signalStrengthV1)
{
    m_telephony.GetSignalStrength(iStatus, m_signalStrengthV1Pckg);
    makeRequest();

    m_cellNetworkSignalStrength = m_signalStrengthV1.iSignalStrength;
    m_previousCellNetworkSignalStrength = m_cellNetworkSignalStrength;
    
    m_signalBar = m_signalStrengthV1.iBar;
    m_previousSignalBar = m_signalBar;

    m_initializing = false;
    
    startMonitoring();    
}

void CCellSignalStrengthInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_cellNetworkSignalStrength = m_signalStrengthV1.iSignalStrength;
        m_signalBar = m_signalStrengthV1.iBar;

        if (m_signalBar != m_previousSignalBar) {
            foreach (MTelephonyInfoObserver *observer, m_observers) {
                observer->cellNetworkSignalStrengthChanged();
            }
        }
        m_previousSignalBar = m_signalBar;
        startMonitoring();
    }
}

void CCellSignalStrengthInfo::DoCancel()
{
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetSignalStrengthCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::ESignalStrengthChangeCancel);
    }
}

int CCellSignalStrengthInfo::cellNetworkSignalStrength() const
{
    //Workaround solution based on the number of signal bars (max. 7)
    return int((TReal(m_signalBar) * 100.0 + 0.5) / 7.0);
}

void CCellSignalStrengthInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::ESignalStrengthChange, m_signalStrengthV1Pckg);
    SetActive();
}
