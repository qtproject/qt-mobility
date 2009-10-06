/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
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

/*
CIndicatorInfo::CIndicatorInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_batteryInfoV1Pckg(m_batteryInfoV1)
{
}

void CIndicatorInfo::DoCancel()
{
    m_telephony.CancelAsync(CTelephony::EGetIndicatorCancel);
}

bool CIndicatorInfo::isBatteryCharging() const
{
    m_telephony.GetIndicator(iStatus,iIndicatorV1Pckg);
    makeRequest();

    if (iIndicatorV1.iIndicator & CTelephony::KIndChargerConnected) {
        chargeStatus = true;
    }
    return chargeStatus;
}
*/

CBatteryInfo::CBatteryInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_batteryInfoV1Pckg(m_batteryInfoV1), m_initializing(true)
{
    m_telephony.GetBatteryInfo(iStatus, m_batteryInfoV1Pckg);
    makeRequest();

    m_batteryLevel = m_batteryInfoV1.iChargeLevel;
    m_previousBatteryLevel = m_batteryLevel;

    m_batteryStatus = m_batteryInfoV1.iStatus;
    m_previousBatteryStatus = m_batteryStatus;
    
    m_initializing = false;

    startMonitoring();
}

void CBatteryInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_batteryLevel = m_batteryInfoV1.iChargeLevel;
        m_batteryStatus = m_batteryInfoV1.iStatus;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_batteryStatus != m_previousBatteryStatus) {
                m_previousBatteryStatus = m_batteryStatus;
                observer->batteryStatusChanged();
            }
            if (m_batteryLevel != m_previousBatteryLevel) {
                m_previousBatteryLevel = m_batteryLevel;
                observer->batteryLevelChanged();
            }
        }
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
    return m_previousBatteryLevel;
}

CTelephony::TBatteryStatus CBatteryInfo::batteryStatus() const
{
    return m_previousBatteryStatus;
}

void CBatteryInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::EBatteryInfoChange, m_batteryInfoV1Pckg);
    SetActive();
}

CCellNetworkInfo::CCellNetworkInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_networkInfoV1Pckg(m_networkInfoV1), m_initializing(true)
{
    m_telephony.GetCurrentNetworkInfo(iStatus, m_networkInfoV1Pckg);
    makeRequest();    

    m_cellId = m_networkInfoV1.iCellId;

	m_locationAreaCode = m_networkInfoV1.iLocationAreaCode;

	TBuf<CTelephony::KNetworkIdentitySize> homeMobileNetworkCode = m_networkInfoV1.iNetworkId;
    m_homeMobileNetworkCode = QString::fromUtf16(homeMobileNetworkCode.Ptr(), homeMobileNetworkCode.Length());

    TBuf<CTelephony::KNetworkCountryCodeSize> homeMobileCountryCode = m_networkInfoV1.iCountryCode;
    m_homeMobileCountryCode = QString::fromUtf16(homeMobileCountryCode.Ptr(), homeMobileCountryCode.Length());

    TBuf<CTelephony::KNetworkLongNameSize> networkName = m_networkInfoV1.iLongName;
    m_networkName = QString::fromUtf16(networkName.Ptr(), networkName.Length());
    
    m_initializing = false;
}

//TODO
/*
void CCellNetworkInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_homeMobileNetworkCode = m_networkInfoV1.iNetworkId;
        m_homeMobileCountryCode = m_networkInfoV1.iCountryCode;
        m_networkName = m_networkInfoV1.iLongName;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_homeMobileNetworkCode != m_previousHomeMobileNetworkCode) {
                m_previousHomeMobileNetworkCode = m_homeMobileNetworkCode;
                observer->currentMobileNetworkCodeChanged(&mnc);
            }
            if (m_homeMobileCountryCode != m_previousHomeMobileCountryCode) {
                m_previousHomeMobileCountryCode = m_homeMobileCountryCode;
                observer->currentMobileCountryCodeChanged(&mcc);
            }
            if (m_networkName != m_previousNetworkName) {
                m_previousNetworkName = m_networkName;
                observer->networkNameChanged(mode, &netName);
            }
        }
        startMonitoring();
    }
}
*/

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

QString CCellNetworkInfo::homeMobileNetworkCode() const
{
    return m_homeMobileNetworkCode;
}

QString CCellNetworkInfo::homeMobileCountryCode() const
{
    return m_homeMobileCountryCode;
}

QString CCellNetworkInfo::networkName() const
{
    return m_networkName;
}

//TODO
/*
void CCellNetworkInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::ESignalStrengthChange, m_signalStrengthV1Pckg);
    SetActive();
}
*/

CCellNetworkRegistrationInfo::CCellNetworkRegistrationInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_networkRegistrationV1Pckg(m_networkRegistrationV1), m_initializing(true)
{
    m_telephony.GetNetworkRegistrationStatus(iStatus, m_networkRegistrationV1Pckg);
    makeRequest();

    m_networkStatus = m_networkRegistrationV1.iRegStatus;
    m_initializing = false;
}

//TODO
/*
void CCellNetworkRegistrationInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_networkStatus = m_networkRegistrationV1.iRegStatus;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_networkStatus != m_previousNetworkStatus) {
                m_previousNetworkStatus = m_networkStatus;
                observer->networkStatusChanged(mode, status);
            }
        }
        startMonitoring();
    }
}
*/

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

//TODO
/*
void CCellNetworkRegistrationInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::ENetworkRegistrationStatusChange, m_networkRegistrationV1Pckg);
    SetActive();
}
*/

CCellSignalStrengthInfo::CCellSignalStrengthInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_signalStrengthV1Pckg(m_signalStrengthV1), m_initializing(true)
{
    m_telephony.GetSignalStrength(iStatus, m_signalStrengthV1Pckg);
    makeRequest();

    m_cellNetworkSignalStrength = m_signalStrengthV1.iSignalStrength;
    
    m_initializing = false;
}

//TODO
/*
void CCellSignalStrengthInfo::RunL()
{
    if (m_initializing) {
        CTelephonyInfo::RunL();
    } else {
        m_cellNetworkSignalStrength = m_signalStrengthV1.iSignalStrength;

        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_cellNetworkSignalStrength != m_previousCellNetworkSignalStrength) {
                m_previousCellNetworkSignalStrength = m_cellNetworkSignalStrength;
                observer->batteryStatusChanged(mode, strength);
            }
        }
        startMonitoring();

    }
}
*/

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
    return m_cellNetworkSignalStrength;
}

//TODO
/*
void CCellSignalStrengthInfo::startMonitoring()
{
    m_telephony.NotifyChange(iStatus, CTelephony::ESignalStrengthChange, m_signalStrengthV1Pckg);
    SetActive();
}
*/
