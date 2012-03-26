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

#include "telephonyinfo_s60.h"
#ifdef ETELMM_SUPPORTED
#include <etelmm.h>
#include <mmtsy_names.h>
#endif

#ifndef ETELMM_SUPPORTED
// number of nested event loops allowed to avoid stackoverflow
const int KMaxLoops = 25;


CTelephonyInfo::CTelephonyInfo(CTelephony &telephony) : CActive(EPriorityStandard),
    m_telephony(telephony), m_loops()
{
    CActiveScheduler::Add(this);
}

CTelephonyInfo::~CTelephonyInfo()
{
  //Cancel();
  exitWait();
}

void CTelephonyInfo::addObserver(MTelephonyInfoObserver *observer)
{
    m_observers.append(observer);
}

void CTelephonyInfo::removeObserver(MTelephonyInfoObserver *observer)
{
    m_observers.removeOne(observer);
}

void CTelephonyInfo::waitForRequest() const
{
 TRACES (qDebug() << "CTelephonyInfo::waitForRequest<---");

    //Restricing number of blocking loops
    //Each call to api's e.g. IMSI must be blocked until they are initialized
    //But we cannot maintain infinite number of blocked calls as stack is limited
    //So restrict the number of loops after which we return uninitialized values
    if (m_loops.size() <= KMaxLoops) {
      QEventLoop* loop = NULL;
      loop = new QEventLoop();
      if ( loop != NULL ) {
        m_loops.push(loop);
        loop->exec();
        delete loop;
      }
    } else {
      qDebug() << "Blocked calls loop count exceeded";
      }

 TRACES (qDebug() << "CTelephonyInfo::waitForRequest--->");
}

void CTelephonyInfo::exitWait() const
{
 TRACES (qDebug() << "CTelephonyInfo::ExitWait<---");

  while (!m_loops.isEmpty())
      {
        QEventLoop* loop = m_loops.pop();
        loop->quit();
      }

 TRACES (qDebug() << "CTelephonyInfo::ExitWait--->");
}

CPhoneInfo::CPhoneInfo(CTelephony &telephony) : CTelephonyInfo(telephony),m_initializing(true)
    ,m_phoneIdV1Pckg(m_phoneIdV1)
{
 TRACES (qDebug() << "CPhoneInfo::CPhoneInfo<---");
  makeRequest();
 TRACES (qDebug() << "CPhoneInfo::CPhoneInfo--->");
}

void CPhoneInfo::makeRequest()
{
  TRACES (qDebug() << "CPhoneInfo::makeRequest<--");
  if (!IsActive()){
    m_telephony.GetPhoneId(iStatus, m_phoneIdV1Pckg);
    SetActive();
    }
  TRACES (qDebug() << "CPhoneInfo::makeRequest-->");
}

void CPhoneInfo::RunL()
{
 TRACES (qDebug() << "CPhoneInfo::RunL<---");

  if (iStatus == KErrNone) {
    TBuf<CTelephony::KPhoneSerialNumberSize> imei = m_phoneIdV1.iSerialNumber;
    m_imei = QString::fromUtf16(imei.Ptr(), imei.Length());

    TBuf<CTelephony::KPhoneManufacturerIdSize> manufacturer = m_phoneIdV1.iManufacturer;
    m_manufacturer = QString::fromUtf16(manufacturer.Ptr(), manufacturer.Length());

    TBuf<CTelephony::KPhoneModelIdSize> model = m_phoneIdV1.iModel;
    m_model = QString::fromUtf16(model.Ptr(), model.Length());

    m_initializing = false;
  }

  if (iStatus == KErrPermissionDenied) m_initializing =false;
  exitWait();

 TRACES (qDebug() << "CPhoneInfo::RunL--->");
}

CPhoneInfo::~CPhoneInfo()
{
    Cancel();
}

void CPhoneInfo::DoCancel()
{
 TRACES (qDebug() << "CPhoneInfo::DoCancel<---");
   if (m_initializing) {
       m_telephony.CancelAsync(CTelephony::EGetPhoneIdCancel);
      }
 TRACES (qDebug() << "CPhoneInfo::DoCancel--->");
}

QString CPhoneInfo::imei()
{
  TRACES (qDebug() << "CPhoneInfo::imei:" << m_imei);

  if (m_initializing) {
        makeRequest();
        waitForRequest();
      }

  return m_imei;
}

QString CPhoneInfo::manufacturer()
{
  TRACES (qDebug() << "CPhoneInfo::manufacturer:" << m_manufacturer);

   if (m_initializing) {
        makeRequest();
        waitForRequest();
      }

  return m_manufacturer;
}

QString CPhoneInfo::model()
{
  TRACES (qDebug() << "CPhoneInfo::model:" << m_model);

   if (m_initializing) {
        makeRequest();
        waitForRequest();
      }

  return m_model;
}

CSubscriberInfo::CSubscriberInfo(CTelephony &telephony) : CTelephonyInfo(telephony),m_initializing(true)
        ,m_subscriberIdV1Pckg(m_subscriberIdV1)
{
 TRACES (qDebug() << "CSubscriberInfo::CSubscriberInfo<---");
  makeRequest();
 TRACES (qDebug() << "CSubscriberInfo::CSubscriberInfo--->");
}

void CSubscriberInfo::makeRequest()
{
  TRACES (qDebug() << "CSubscriberInfo::MakeRequest<---");
  if (!IsActive()) {
    m_telephony.GetSubscriberId(iStatus, m_subscriberIdV1Pckg);
    SetActive();
  }
  TRACES (qDebug() << "CSubscriberInfo::MakeRequest--->");
}

void CSubscriberInfo::RunL()
{
 TRACES (qDebug() << "CSubscriberInfo::RunL<---");

  if (iStatus == KErrNone) {
    TBuf<CTelephony::KIMSISize> imsi = m_subscriberIdV1.iSubscriberId;
    m_imsi = QString::fromUtf16(imsi.Ptr(), imsi.Length());
    m_initializing = false;
  }

  if (iStatus == KErrPermissionDenied ) m_initializing = false;
  exitWait();

 TRACES (qDebug() << "CSubscriberInfo::RunL--->");
}

CSubscriberInfo::~CSubscriberInfo()
{
    Cancel();
}

void CSubscriberInfo::DoCancel()
{
 TRACES (qDebug() << "CSubscriberInfo::DoCancel<---");
 if (m_initializing) {
     m_telephony.CancelAsync(CTelephony::EGetSubscriberIdCancel);
  }
 TRACES (qDebug() << "CSubscriberInfo::DoCancel--->");
}

QString CSubscriberInfo::imsi()
{
  TRACES (qDebug() << "CSubscriberInfo::imsi:" << m_imsi);
    if (m_initializing) {
        makeRequest();
        waitForRequest();
      }
  return m_imsi;
}

CBatteryInfo::CBatteryInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_batteryInfoV1Pckg(m_batteryInfoV1)
{
 TRACES (qDebug() << "CBatteryInfo::CBatteryInfo<---");

  m_telephony.GetBatteryInfo(iStatus, m_batteryInfoV1Pckg);
  SetActive();

 TRACES (qDebug() << "CBatteryInfo::CBatteryInfo--->");
}

CBatteryInfo::~CBatteryInfo()
{
    Cancel();
}

void CBatteryInfo::RunL()
{
 TRACES (qDebug() << "CBatteryInfo::RunL<---");

    if (iStatus == KErrNone) {
      m_batteryLevel = m_batteryInfoV1.iChargeLevel;

      if (!m_initializing) {
        foreach (MTelephonyInfoObserver *observer, m_observers) {
              if (m_batteryLevel != m_previousBatteryLevel)
                  observer->batteryLevelChanged();
          }
      }
      m_previousBatteryLevel = m_batteryLevel;
    }

    if (m_initializing) {
      m_initializing = false;
      exitWait();
      if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    }

    startMonitoring();

 TRACES (qDebug() << "CBatteryInfo::RunL--->");
}

void CBatteryInfo::DoCancel()
{
 TRACES (qDebug() << "CBatteryInfo::DoCancel<---");
  if (m_initializing) {
      m_telephony.CancelAsync(CTelephony::EGetBatteryInfoCancel);
    }
    else
    {
      m_telephony.CancelAsync(CTelephony::EBatteryInfoChangeCancel);
    }
 TRACES (qDebug() << "CBatteryInfo::DoCancel--->");
}

int CBatteryInfo::batteryLevel() const
{
  TRACES (qDebug() << "CBatteryInfo::batteryLevel:" << m_batteryLevel);

  if (m_initializing)
    waitForRequest();

  return m_batteryLevel;
}

void CBatteryInfo::startMonitoring()
{
 TRACES (qDebug() << "CBatteryInfo::startMonitoring<---");

    if (!IsActive()) {
        m_telephony.NotifyChange(iStatus, CTelephony::EBatteryInfoChange, m_batteryInfoV1Pckg);
        SetActive();
    }

 TRACES (qDebug() << "CBatteryInfo::startMonitoring--->");
}

CCellNetworkInfo::CCellNetworkInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_networkInfoV1Pckg(m_networkInfoV1)
{
 TRACES (qDebug() << "CCellNetworkInfo::CCellNetworkInfo<---");

    m_telephony.GetCurrentNetworkInfo(iStatus, m_networkInfoV1Pckg);
    SetActive();

 TRACES (qDebug() << "CCellNetworkInfo::CCellNetworkInfo--->");
}

CCellNetworkInfo::~CCellNetworkInfo()
{
    Cancel();
}

void CCellNetworkInfo::RunL()
{
 TRACES (qDebug() << "CCellNetworkInfo::RunL<---");

    if (iStatus == KErrNone) {
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

      if (!m_initializing) {
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
              if (m_cellId != m_previouscellId) {
                  observer->changedCellId(m_cellId);
              }
          }
      }

      m_previouscellId = m_cellId;
      m_previousNetworkId = m_networkId;
      m_previousCountryCode = m_countryCode;
      m_previousNetworkName = m_networkName;
      m_previousNetworkMode = m_networkMode;
    }

    if (m_initializing) {
      m_initializing = false;
      exitWait();
      if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    }

  startMonitoring();
 TRACES (qDebug() << "CCellNetworkInfo::RunL--->");
}

void CCellNetworkInfo::DoCancel()
{
 TRACES (qDebug() << "CCellNetworkInfo::DoCancel<---");
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
    }
 TRACES (qDebug() << "CCellNetworkInfo::DoCancel--->");
}

int CCellNetworkInfo::cellId() const
{
  TRACES (qDebug() << "CCellNetworkInfo::cellId:" << m_cellId);

    if (m_initializing)
        waitForRequest();

  return m_cellId;
}

int CCellNetworkInfo::locationAreaCode() const
{
  TRACES (qDebug() << "CCellNetworkInfo::locationAreaCode:" << m_locationAreaCode);

    if (m_initializing)
        waitForRequest();

  return m_locationAreaCode;
}

QString CCellNetworkInfo::countryCode() const
{
  TRACES (qDebug() << "CCellNetworkInfo::countryCode:" << m_countryCode);

    if (m_initializing)
        waitForRequest();

  return m_countryCode;
}

QString CCellNetworkInfo::networkCode() const
{
  TRACES (qDebug() << "CCellNetworkInfo::networkCode:" << m_networkId);

    if (m_initializing)
        waitForRequest();

  return m_networkId;
}

QString CCellNetworkInfo::networkName() const
{
  TRACES (qDebug() << "CCellNetworkInfo::networkName<---");

    if (m_initializing)
        waitForRequest();

  return m_networkName;
}

QString CCellNetworkInfo::homeNetworkCode()
    {
      TRACES (qDebug() << "CCellNetworkInfo::homeNetworkCode<---");
#ifdef ETELMM_SUPPORTED
        RTelServer telServer;
        RMobilePhone mobilePhone;
        TInt error = telServer.Connect();
        if ( error != KErrNone )
            {
            return QString();
            }
        error = telServer.LoadPhoneModule( KMmTsyModuleName );
        if ( error != KErrNone )
            {
            telServer.Close();
            return QString();
            }
        RTelServer::TPhoneInfo phoneInfo;
        const TInt KPhoneIndex = 0;
        error = telServer.GetPhoneInfo( KPhoneIndex, phoneInfo );
        if ( error != KErrNone )
            {
            telServer.Close();
            return QString();
            }
        error = mobilePhone.Open( telServer, phoneInfo.iName );
        if ( error != KErrNone )
            {
            telServer.Close();
            return QString();
            }
          TRequestStatus networkStatus;
          RMobilePhone::TMobilePhoneNetworkInfoV1 infov1;
          RMobilePhone::TMobilePhoneNetworkInfoV1Pckg statusPkg(infov1);
          mobilePhone.GetHomeNetwork(networkStatus, statusPkg);
          User::WaitForRequest(networkStatus);
          mobilePhone.Close();
          telServer.Close();
          if (networkStatus == KErrNone)
              {
              QString homeNetworkCode= QString::fromUtf16(infov1.iNetworkId.Ptr(), infov1.iNetworkId.Length());
              return homeNetworkCode;
              }
           else
               return QString();
#else
     return QString();
#endif
    }

CTelephony::TNetworkMode CCellNetworkInfo::networkMode() const
{
  TRACES (qDebug() << "CCellNetworkInfo::networkMode:" << m_networkMode);

    if (m_initializing)
        waitForRequest();

  return m_networkMode;
}

void CCellNetworkInfo::startMonitoring()
{
 TRACES (qDebug() << "CCellNetworkInfo::startMonitoring<---");

 if (!IsActive()) {
    m_telephony.NotifyChange(iStatus, CTelephony::ECurrentNetworkInfoChange, m_networkInfoV1Pckg);
    SetActive();
  }

 TRACES (qDebug() << "CCellNetworkInfo::startMonitoring--->");
}

CCellNetworkRegistrationInfo::CCellNetworkRegistrationInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_networkRegistrationV1Pckg(m_networkRegistrationV1)
{
 TRACES (qDebug() << "CCellNetworkRegistrationInfo::CCellNetworkRegistrationInfo--->");

    m_telephony.GetNetworkRegistrationStatus(iStatus, m_networkRegistrationV1Pckg);
    SetActive();

 TRACES (qDebug() << "CCellNetworkRegistrationInfo::CCellNetworkRegistrationInfo<---");
}

CCellNetworkRegistrationInfo::~CCellNetworkRegistrationInfo()
{
    Cancel();
}

void CCellNetworkRegistrationInfo::RunL()
{
 TRACES (qDebug() << "CCellNetworkRegistrationInfo::RunL<---");
    if (iStatus == KErrNone) {
    m_networkStatus = m_networkRegistrationV1.iRegStatus;
      if (!m_initializing) {
        foreach (MTelephonyInfoObserver *observer, m_observers) {
            if (m_networkStatus != m_previousNetworkStatus) {
                observer->cellNetworkStatusChanged();
            }
        }
      }
    m_previousNetworkStatus = m_networkStatus;
    }

    if (m_initializing) {
      m_initializing = false;
      exitWait();
      if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    }

    startMonitoring();
 TRACES (qDebug() << "CCellNetworkRegistrationInfo::RunL--->");
}

void CCellNetworkRegistrationInfo::DoCancel()
{
 TRACES (qDebug() << "CCellNetworkRegistrationInfo::DoCancel<---");
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
    }
 TRACES (qDebug() << "CCellNetworkRegistrationInfo::DoCancel--->");
}

CTelephony::TRegistrationStatus CCellNetworkRegistrationInfo::cellNetworkStatus() const
{
  TRACES (qDebug() << "CCellNetworkRegistrationInfo::cellNetworkStatus:" << m_networkStatus);

    if (m_initializing)
        waitForRequest();

  return m_networkStatus;
}

void CCellNetworkRegistrationInfo::startMonitoring()
{
 TRACES (qDebug() << "CCellNetworkRegistrationInfo::startMonitoring<---");

 if (!IsActive()) {
    m_telephony.NotifyChange(iStatus, CTelephony::ENetworkRegistrationStatusChange, m_networkRegistrationV1Pckg);
    SetActive();
  }

TRACES (qDebug() << "CCellNetworkRegistrationInfo::startMonitoring--->");
}

CCellSignalStrengthInfo::CCellSignalStrengthInfo(CTelephony &telephony) : CTelephonyInfo(telephony),
    m_initializing(true), m_signalStrengthV1Pckg(m_signalStrengthV1)
{
 TRACES (qDebug() << "CCellSignalStrengthInfo::CCellSignalStrengthInfo<---");

    m_telephony.GetSignalStrength(iStatus, m_signalStrengthV1Pckg);
    SetActive();

 TRACES (qDebug() << "CCellSignalStrengthInfo::CCellSignalStrengthInfo--->");
}

CCellSignalStrengthInfo::~CCellSignalStrengthInfo()
{
    Cancel();
}

void CCellSignalStrengthInfo::RunL()
{
 TRACES (qDebug() << "CCellSignalStrengthInfo::RunL<---");

    if (iStatus == KErrNone) {
      m_cellNetworkSignalStrength = m_signalStrengthV1.iSignalStrength;
      m_signalBar = m_signalStrengthV1.iBar;

      if (!m_initializing) {
          if (m_signalBar != m_previousSignalBar) {
              foreach (MTelephonyInfoObserver *observer, m_observers) {
                  observer->cellNetworkSignalStrengthChanged();
              }
          }
      }

      m_previousCellNetworkSignalStrength = m_cellNetworkSignalStrength;
      m_previousSignalBar = m_signalBar;
    }

    if (m_initializing) {
      m_initializing = false;
      exitWait();
      if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    }

    startMonitoring();

 TRACES (qDebug() << "CCellSignalStrengthInfo::RunL--->");
}

void CCellSignalStrengthInfo::DoCancel()
{
 TRACES (qDebug() << "CCellSignalStrengthInfo::DoCancel<---");
    if (m_initializing) {
        m_telephony.CancelAsync(CTelephony::EGetSignalStrengthCancel);
    } else {
        m_telephony.CancelAsync(CTelephony::ESignalStrengthChangeCancel);
    }
 TRACES (qDebug() << "CCellSignalStrengthInfo::DoCancel--->");
}

int CCellSignalStrengthInfo::cellNetworkSignalStrength() const
{
  TRACES (qDebug() << "CCellSignalStrengthInfo::cellNetworkSignalStrength<---");
    if (m_initializing)
        waitForRequest();
    //Workaround solution based on the number of signal bars (max. 7)
    return int((TReal(m_signalBar) * 100.0 + 0.5) / 7.0);
}

void CCellSignalStrengthInfo::startMonitoring()
{
 TRACES (qDebug() << "CCellSignalStrengthInfo::startMonitoring<---");
 if (!IsActive()) {
    m_telephony.NotifyChange(iStatus, CTelephony::ESignalStrengthChange, m_signalStrengthV1Pckg);
    SetActive();
  }
 TRACES (qDebug() << "CCellSignalStrengthInfo::startMonitoring--->");
}
#else //ETELMM_SUPPORTED
CPhoneInfo::CPhoneInfo(RMobilePhone &aMobilePhone):
      m_rmobilePhone(aMobilePhone),m_phoneInfoinitialised(false)
{
  TRACES (qDebug() << "CPhoneInfov2::Constructor<--");
  TRACES (qDebug() << "CPhoneInfov2::Constructor-->");
}

CPhoneInfo::~CPhoneInfo()
{
 TRACES (qDebug() << "CPhoneInfov2::Destructor<--");
 TRACES (qDebug() << "CPhoneInfov2::Destructor-->");
}

void CPhoneInfo::initialise()
{
  TRACES (qDebug() << "CPhoneInfov2::Initialise<--");

  if (m_phoneInfoinitialised)
    {
      TRACES(qDebug() << "CPhoneInfov2 already initialised, return");
      return;
    }

  //RMobilePhone implementation
  //check for identity capabilities
  TUint32 identityCaps;
  TInt capabilityErr = m_rmobilePhone.GetIdentityCaps(identityCaps);
  if (capabilityErr != KErrNone)
    {
    TRACES ( qDebug() << "CPhoneInfo::initialise-capability error:" << capabilityErr);
    return;
    }

  //Initialize imei,imsi, manufacturer
  TRequestStatus phoneInforeqstatus;
  RMobilePhone::TMobilePhoneIdentityV1 mobilePhoneIdentity;
  m_rmobilePhone.GetPhoneId(phoneInforeqstatus, mobilePhoneIdentity);
  User::WaitForRequest(phoneInforeqstatus);
  if ( phoneInforeqstatus == KErrNone ) {
    m_model = QString::fromUtf16(mobilePhoneIdentity.iModel.Ptr(), mobilePhoneIdentity.iModel.Length());
    m_manufacturer = QString::fromUtf16(mobilePhoneIdentity.iManufacturer.Ptr(), mobilePhoneIdentity.iManufacturer.Length());
    m_imei = QString::fromUtf16(mobilePhoneIdentity.iSerialNumber.Ptr(), mobilePhoneIdentity.iSerialNumber.Length());
    TRACES ( qDebug() << "CPhoneInfov2::Model:" << m_model);
    TRACES ( qDebug() << "CPhoneInfov2::Manufacturer:" << m_manufacturer);
    TRACES ( qDebug() << "CPhoneInfov2::Imei:" << m_imei);
    m_phoneInfoinitialised = true;
    }

  TRACES (qDebug() << "CPhoneInfov2::Initialise-->");
}

QString CPhoneInfo::imei()
{
  TRACES (qDebug() << "query CPhoneInfov2::IMEI" << m_imei);
  initialise();
  return m_imei;
}

QString CPhoneInfo::manufacturer()
{
  TRACES (qDebug() << "query CPhoneInfov2::manufacturer" << m_manufacturer);
  initialise();
  return m_manufacturer;
}

QString CPhoneInfo::model()
{
  TRACES (qDebug() << "query CPhoneInfov2::model" << m_model);
  initialise();
  return m_model;
}

CSubscriberInfo::CSubscriberInfo(RMobilePhone &aMobilePhone):
          m_rmobilePhone(aMobilePhone),m_subscriberInfoinitialised(false)
{
  TRACES (qDebug() << "CSubscriberInfov2::Constructor<--");
  TRACES (qDebug() << "CSubscriberInfov2::Constructor-->");
}

CSubscriberInfo::~CSubscriberInfo()
{
 TRACES (qDebug() << "CSubscriberInfov2::Destructor<--");
 TRACES (qDebug() << "CSubscriberInfo2::Destructor-->");
}

void CSubscriberInfo::initialise()
{
  TRACES (qDebug() << "CSubscriberInfov2::Initialise<--");

  if (m_subscriberInfoinitialised)
    {
      TRACES(qDebug() << "CSubscriberInfov2 already initialised, return");
      return;
    }

  //RMobilePhone implementation
  //check for identity capabilities
  TUint32 identityCaps;
  TInt capabilityErr = m_rmobilePhone.GetIdentityCaps(identityCaps);
  if (capabilityErr != KErrNone)
    {
    TRACES ( qDebug() << "CSubscriberInfo::initialise-capability error:" << capabilityErr);
    return;
    }

  //Initialize imei,imsi, manufacturer
  TRequestStatus  subscriberInforeqstatus;
  RMobilePhone::TMobilePhoneSubscriberId subscriberId;
  m_rmobilePhone.GetSubscriberId(subscriberInforeqstatus, subscriberId);
  User::WaitForRequest(subscriberInforeqstatus);
  if ( subscriberInforeqstatus == KErrNone ) {
    m_imsi = QString::fromUtf16(subscriberId.Ptr(), subscriberId.Length());
    TRACES ( qDebug() << "CSubscriberInfov2::Initialise.Imsi:" << m_imsi);
    m_subscriberInfoinitialised = true;
    }

  TRACES (qDebug() << "CSubscriberInfov2::Initialise-->");
}

QString CSubscriberInfo::imsi()
{
  TRACES (qDebug() << "query CSubscriberInfov2::IMSI" << m_imsi);
  initialise();
  return m_imsi;
}

CEtelInfo::CEtelInfo(RMobilePhone &aMobilePhone):CActive(EPriorityStandard),
                m_rmobilePhone(aMobilePhone),m_initialised(false)
{
  CActiveScheduler::Add(this);
}

CEtelInfo::~CEtelInfo()
{
}

void CEtelInfo::addObserver(MTelephonyInfoObserver *observer)
{
    m_observers.append(observer);
}

void CEtelInfo::removeObserver(MTelephonyInfoObserver *observer)
{
    m_observers.removeOne(observer);
}

CBatteryInfo::CBatteryInfo(RMobilePhone &aMobilePhone)
    : CEtelInfo(aMobilePhone),m_batteryLevel(0)
{
  TRACES ( qDebug() << "CBatteryInfov2::constructor<--" );
  initialise();
  TRACES ( qDebug() << "CBatteryInfov2::constructor-->" );
}

void CBatteryInfo::initialise()
{
  TRACES (qDebug() << "CBatteryInfov2::initialise<--");
  if (m_initialised)
    {
      TRACES (qDebug() << "CBatteryInfov2 already initialised,return");
      return;
    }
  //Check BatteryCaps
  TUint32 batteryCaps;
  TInt batteryCapserror = m_rmobilePhone.GetBatteryCaps(batteryCaps);
  if ( batteryCapserror != KErrNone) TRACES ( qDebug() << "CBatteryInfo::initialise-capability error:" << batteryCapserror);

  if ( batteryCapserror == KErrNone && (batteryCaps & RMobilePhone::KCapsGetBatteryInfo) ) {
      TRequestStatus batterystatus;
      m_rmobilePhone.GetBatteryInfo(batterystatus, m_batteryinfo);
      User::WaitForRequest(batterystatus);
      if (batterystatus == KErrNone){
          m_batteryLevel = m_batteryinfo.iChargeLevel;
          TRACES(qDebug() << "CBatteryInfov2:Initial chargelevel:" << m_batteryLevel);
          m_initialised = true;
        }
    }

  if ( batteryCapserror == KErrPermissionDenied)
    {
      m_initialised = true;
      return;
    }

  if (batteryCaps & RMobilePhone::KCapsNotifyBatteryInfoChange) startMonitoring();

  TRACES (qDebug() << "CBatteryInfov2::initialise-->");
}

CBatteryInfo::~CBatteryInfo()
{
    Cancel();
}

void CBatteryInfo::DoCancel()
{
    TRACES ( qDebug() << "CBatteryInfov2::DoCancel<--" );
    if (IsActive())
      m_rmobilePhone.CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange);
    TRACES ( qDebug() << "CBatteryInfov2::DoCancel-->" );
}

void CBatteryInfo::RunL()
{
    TRACES(qDebug() << "CBatteryInfov2::RunL()<---");
    if (iStatus == KErrNone)
      {
        if ( m_batteryLevel != m_batteryinfo.iChargeLevel ) {
            foreach (MTelephonyInfoObserver *observer, m_observers) {
                  TRACES (qDebug() << "Notifying batteryLevelChanged");
                  observer->batteryLevelChanged();
                }
          }
        m_batteryLevel = m_batteryinfo.iChargeLevel;
      }

    if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    startMonitoring();
    TRACES(qDebug() << "CBatteryInfov2::RunL()--->");
}

void CBatteryInfo::startMonitoring()
{
  TRACES(qDebug() << "CBatteryInfov2::StartMonitoring--start");
    if (!IsActive()) {
        m_rmobilePhone.NotifyBatteryInfoChange(iStatus, m_batteryinfo);
        SetActive();
    }
    TRACES(qDebug() << "CBatteryInfov2::StartMonitoring--End");
}

int CBatteryInfo::batteryLevel() const
{
  TRACES (qDebug() << "CBatteryInfov2::batteryLevel:" << m_batteryLevel);
  return m_batteryLevel;
}

//Signal Strength
CCellSignalStrengthInfo::CCellSignalStrengthInfo(RMobilePhone &aMobilePhone)
    :CEtelInfo(aMobilePhone)
{
  TRACES(qDebug() << "CCellSignalStrengthInfov2::CCellSignalStrengthInfov2<---");
  initialise();
  TRACES(qDebug() << "CCellSignalStrengthInfov2::CCellSignalStrengthInfov2--->");
}

void CCellSignalStrengthInfo::initialise()
{
  TRACES (qDebug() << "CCellSignalStrengthInfov2::initialise<--");
  if (m_initialised)
    {
      TRACES (qDebug() << "CCellSignalStrengthInfov2 already initialised,return");
      return;
    }
  //Check Signal related Caps
  TUint32 signalCaps;
  TInt signalCapserror = m_rmobilePhone.GetSignalCaps(signalCaps);
  if ( signalCapserror != KErrNone) TRACES ( qDebug() << "CCellSignalStrengthInfo::initialise-capability error:" << signalCapserror);

  if (signalCapserror == KErrNone)
    {
      TRequestStatus signalstatus;
      if (signalCaps & RMobilePhone::KCapsGetSignalStrength)
        {
          m_rmobilePhone.GetSignalStrength(signalstatus, m_cellNetworkSignalStrength, m_signalBar);
          User::WaitForRequest(signalstatus);
          if ( signalstatus == KErrNone ) {
            //m_prevcellNetworkSignalStrength = m_cellNetworkSignalStrength;
            m_prevsignalBar = m_signalBar;
            m_initialised = true;
          }
        }
    }

  if ( signalCapserror == KErrPermissionDenied)
    {
      m_initialised = true;
      return;
    }

  if (signalCaps & RMobilePhone::KCapsNotifySignalStrengthChange) startMonitoring();
  TRACES (qDebug() << "CCellSignalStrengthInfov2::initialise-->");
}

CCellSignalStrengthInfo::~CCellSignalStrengthInfo()
{
    Cancel();
}

void CCellSignalStrengthInfo::DoCancel()
{
    TRACES ( qDebug() << "CCellSignalStrengthInfov2::DoCancel<--" );
    if (IsActive())
      m_rmobilePhone.CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
    TRACES ( qDebug() << "CCellSignalStrengthInfov2::DoCancel-->" );
}

void CCellSignalStrengthInfo::RunL()
{
    TRACES(qDebug() << "CCellSignalStrengthInfov2::RunL()<---");
    if (iStatus == KErrNone)
      {
        if ( m_signalBar != m_prevsignalBar ) {
            foreach (MTelephonyInfoObserver *observer, m_observers) {
                  TRACES (qDebug() << "Notifying cellNetworkSignalStrengthChanged");
                  observer->cellNetworkSignalStrengthChanged();
                }
        }
        m_prevsignalBar = m_signalBar;
      }

    if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    startMonitoring();
    TRACES(qDebug() << "CCellSignalStrengthInfov2::RunL()--->");
}

void CCellSignalStrengthInfo::startMonitoring()
{
  TRACES(qDebug() << "CCellSignalStrengthInfov2::StartMonitoring--start");
    if (!IsActive()) {
        m_rmobilePhone.NotifySignalStrengthChange(iStatus, m_cellNetworkSignalStrength, m_signalBar);
        SetActive();
    }
    TRACES(qDebug() << "CCellSignalStrengthInfov2::StartMonitoring--End");
}

int CCellSignalStrengthInfo::cellNetworkSignalStrength() const
{
  TRACES(qDebug() << "CCellsignalstrengthv2::cellNetworkSignalStrength");
  return int((TReal(m_signalBar) * 100.0 + 0.5) / 7.0);
}

CCellNetworkRegistrationInfo::CCellNetworkRegistrationInfo(RMobilePhone &aMobilePhone):CEtelInfo(aMobilePhone)
{
  TRACES(qDebug() << "CCellNetworkRegistrationInfov2::CCellNetworkRegistrationInfov2<---");
  initialise();
  TRACES(qDebug() << "CCellNetworkRegistrationInfov2::CCellNetworkRegistrationInfov2--->");
}

CCellNetworkRegistrationInfo::~CCellNetworkRegistrationInfo()
{
    Cancel();
}

void CCellNetworkRegistrationInfo::initialise()
{
  TRACES (qDebug() << "CCellNetworkRegistrationInfov2::initialise<--");
  if (m_initialised)
    {
      TRACES (qDebug() << "CCellNetworkRegistrationInfov2 already initialised,return");
      return;
    }

  TUint32 capsPhone;
  TInt capserror = m_rmobilePhone.GetMultimodeCaps(capsPhone);
  if ( capserror != KErrNone) TRACES ( qDebug() << "CCellNetworkRegistrationInfo::initialise-capability error:" << capserror);
    if (!capserror) {
        TRequestStatus reqStatus;
        m_rmobilePhone.GetNetworkRegistrationStatus(reqStatus, m_networkStatus);
        User::WaitForRequest(reqStatus);
        m_previousNetworkStatus = m_networkStatus;
        m_initialised = true;
    }

 if ( capserror == KErrPermissionDenied)
    {
      m_initialised = true;
      return;
    }

  startMonitoring();
 TRACES (qDebug() << "CCellNetworkRegistrationInfov2::initialise-->");
}

void CCellNetworkRegistrationInfo::RunL()
{
    TRACES (qDebug() << "CCellNetworkRegistrationInfov2::RunL()<---");
    if (iStatus == KErrNone)
      {
        if ( m_networkStatus != m_previousNetworkStatus) {
          foreach (MTelephonyInfoObserver *observer, m_observers) {
                  TRACES (qDebug() << "Notifying cellnetworkstatusChanged");
                  observer->cellNetworkStatusChanged();
                }
         }
        m_previousNetworkStatus = m_networkStatus;
      }

    if (iStatus == KErrPermissionDenied) {
        return; //No monitoring required
      }
    startMonitoring();
    TRACES (qDebug() << "CCellNetworkRegistrationInfov2::RunL()--->");
}

void CCellNetworkRegistrationInfo::startMonitoring()
{
    TRACES (qDebug() << "CCellNetworkRegistrationInfov2::StartMonitoring<---");
    if (!IsActive()) {
        m_rmobilePhone.NotifyNetworkRegistrationStatusChange(iStatus,m_networkStatus);
        SetActive();
    }
   TRACES (qDebug() << "CCellNetworkRegistrationInfov2::StartMonitoring--->");
}

void CCellNetworkRegistrationInfo::DoCancel()
{
    TRACES (qDebug() << "CCellNetworkRegistrationInfov2::DoCancel");
    m_rmobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
}

RMobilePhone::TMobilePhoneRegistrationStatus CCellNetworkRegistrationInfo::cellNetworkStatus() const
{
  TRACES (qDebug() << "CCellNetworkRegistrationInfov2::cellNetworkStatus:" << m_networkStatus);
  return m_networkStatus;
}

CCellNetworkInfo::CCellNetworkInfo(RMobilePhone &aMobilePhone)
    :CEtelInfo(aMobilePhone),iNetworkInfoPckg(iNetworkInfo)
{
  TRACES(qDebug() << "CCellNetworkInfov2::CCellNetworkInfov2<---");
  initialise();
  TRACES(qDebug() << "CCellNetworkInfov2::CCellNetworkInfov2--->");
}

void CCellNetworkInfo::initialise()
{
  TRACES (qDebug() << "CCellNetworkInfov2::initialise<--");

  if (m_initialised)
    {
      TRACES (qDebug() << "CCellNetworkInfov2 already initialised,return");
      return;
    }

    // Check Signal related Caps
    TUint32 networkCaps;
    TInt capserror = m_rmobilePhone.GetNetworkCaps(networkCaps);
    if ( capserror != KErrNone) TRACES ( qDebug() << "CCellNetworkInfo::initialise-capability error:" << capserror);
    if ( capserror == KErrNone && (networkCaps & RMobilePhone::KCapsGetCurrentNetwork))
      {
        //For cellid & location area code, networkid, countrycode
        TRequestStatus locationstatus;
        m_rmobilePhone.GetCurrentNetwork(locationstatus, iNetworkInfoPckg, iLocation);
        User::WaitForRequest(locationstatus);
        if ( locationstatus == KErrNone )
          {
            //cellid
            m_cellId = iLocation.iCellId;
            m_previouscellId = m_cellId;

            //location area code
            m_locationAreaCode = iLocation.iLocationAreaCode;

            //network id
            m_networkId = QString::fromUtf16(iNetworkInfo.iNetworkId.Ptr(), iNetworkInfo.iNetworkId.Length());
            m_previousNetworkId = m_networkId;

            //country code
            m_countryCode = QString::fromUtf16(iNetworkInfo.iCountryCode.Ptr(), iNetworkInfo.iCountryCode.Length());
            m_previousCountryCode = m_countryCode;

            //networkname
            RMobilePhone::TMobilePhoneNetworkLongName longName = iNetworkInfo.iLongName;
            if (longName.Length() > 0 ) {
              m_networkName = QString::fromUtf16(longName.Ptr(), longName.Length());
            } else {
              RMobilePhone::TMobilePhoneNetworkDisplayTag displayTag = iNetworkInfo.iDisplayTag;
              m_networkName = QString::fromUtf16(displayTag.Ptr(), displayTag.Length());
            }
            m_previousNetworkName = m_networkName;
            //network mode
            m_networkMode = iNetworkInfo.iMode;
            m_previousNetworkMode = m_networkMode;
            m_initialised = true;
          }
      }

  if (capserror == KErrPermissionDenied) {
    m_initialised = true;
    return;
  }

  if (networkCaps & RMobilePhone::KCapsNotifyCurrentNetwork) startMonitoring();

  TRACES (qDebug() << "CCellNetworkInfov2::initialise-->");
}

CCellNetworkInfo::~CCellNetworkInfo()
{
    Cancel();
}

int CCellNetworkInfo::cellId() const
{
  TRACES (qDebug() << "CCellNetworkInfov2::cellId():" << m_cellId);
  return m_cellId;
}

int CCellNetworkInfo::locationAreaCode() const
{
  TRACES (qDebug() << "CCellNetworkInfov2::cellId():" << m_locationAreaCode);
  return m_locationAreaCode;
}

QString CCellNetworkInfo::countryCode() const
{
  TRACES (qDebug() << "CCellNetworkInfov2::countryCode():" << m_countryCode);
  return m_countryCode;
}

QString CCellNetworkInfo::networkCode() const
{
  TRACES (qDebug() << "CCellNetworkInfov2::networkCode():" << m_networkId);
  return m_networkId;
}

QString CCellNetworkInfo::networkName() const
{
  TRACES (qDebug() << "CCellNetworkInfov2::networkName():" << m_networkName);
  return m_networkName;
}

QString CCellNetworkInfo::homeNetworkCode()
{
    TRequestStatus homenetworkstatus;
    RMobilePhone::TMobilePhoneNetworkInfoV1 infov1;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg statusPkg(infov1);
    m_rmobilePhone.GetHomeNetwork(homenetworkstatus, statusPkg);
    User::WaitForRequest(homenetworkstatus);
    if (homenetworkstatus.Int() == KErrNone)
              {
              QString homeNetworkCode= QString::fromUtf16(infov1.iNetworkId.Ptr(), infov1.iNetworkId.Length());
              return homeNetworkCode;
              }
           else
               return QString();
}

CTelephony::TNetworkMode CCellNetworkInfo::networkMode() const
{
  TRACES (qDebug() << "CCellNetworkInfov2::networkMode():" << m_networkMode);
  switch (m_networkMode){
    case RMobilePhone::ENetworkModeUnregistered:
      return CTelephony::ENetworkModeUnregistered;
    case RMobilePhone::ENetworkModeGsm:
      return CTelephony::ENetworkModeGsm;
    case RMobilePhone::ENetworkModeAmps:
      return CTelephony::ENetworkModeAmps;
    case RMobilePhone::ENetworkModeCdma95:
      return CTelephony::ENetworkModeCdma95;
    case RMobilePhone::ENetworkModeCdma2000:
      return CTelephony::ENetworkModeCdma2000;
    case RMobilePhone::ENetworkModeWcdma:
      return CTelephony::ENetworkModeWcdma;
    /*case RMobilePhone::ENetworkModeTdcdma:
      return CTelephony::ENetworkModeTdcdma;*/
    default:
      return CTelephony::ENetworkModeUnknown;
  }
}

void CCellNetworkInfo::DoCancel()
{
    TRACES ( qDebug() << "CCellNetworkInfov2::DoCancel<--" );
    if (IsActive())
      m_rmobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
    TRACES ( qDebug() << "CCellNetworkInfov2::DoCancel-->" );
}

void CCellNetworkInfo::RunL()
{
    TRACES(qDebug() << "CCellNetworkInfov2::RunL()<---");
    if (iStatus == KErrNone)
      {
          //cellid
          m_cellId = iLocation.iCellId;

          //location area code
          m_locationAreaCode = iLocation.iLocationAreaCode;

          //network id
          m_networkId = QString::fromUtf16(iNetworkInfo.iNetworkId.Ptr(), iNetworkInfo.iNetworkId.Length());

          //country code
          m_countryCode = QString::fromUtf16(iNetworkInfo.iCountryCode.Ptr(), iNetworkInfo.iCountryCode.Length());

          //networkname
          RMobilePhone::TMobilePhoneNetworkLongName longName = iNetworkInfo.iLongName;
          if (longName.Length() > 0 ) {
            m_networkName = QString::fromUtf16(longName.Ptr(), longName.Length());
          } else {
            RMobilePhone::TMobilePhoneNetworkDisplayTag displayTag = iNetworkInfo.iDisplayTag;
            m_networkName = QString::fromUtf16(displayTag.Ptr(), displayTag.Length());
          }

          //network mode
          m_networkMode = iNetworkInfo.iMode;

          foreach (MTelephonyInfoObserver *observer, m_observers)
            {
              if (m_networkId != m_previousNetworkId) {
                  TRACES (qDebug() << "Notifying networkcodechanged");
                  observer->networkCodeChanged();
              }
              if (m_countryCode != m_previousCountryCode) {
                  TRACES (qDebug() << "Notifying countryCodeChanged");
                  observer->countryCodeChanged();
              }
              if (m_networkName != m_previousNetworkName) {
                  TRACES (qDebug() << "Notifying networkNameChanged");
                  observer->networkNameChanged();
              }
              if (m_networkMode != m_previousNetworkMode) {
                  TRACES (qDebug() << "Notifying networkModeChanged");
                  observer->networkModeChanged();
              }
              if (m_cellId != m_previouscellId) {
                  TRACES (qDebug() << "Notifying cellId changes");
                  observer->changedCellId(m_cellId);
              }
            }
        m_previouscellId = m_cellId;
        m_previousNetworkId = m_networkId;
        m_previousCountryCode = m_countryCode;
        m_previousNetworkName = m_networkName;
        m_previousNetworkMode = m_networkMode;
      }

    if (iStatus != KErrPermissionDenied) {
        // NotifyCurrentNetworkChange requires ReadDeviceData so don't retry if app does not
        // have required capabilities
        startMonitoring();
    }
    TRACES(qDebug() << "CCellNetworkInfov2::RunL()--->");
}

void CCellNetworkInfo::startMonitoring()
{
  TRACES(qDebug() << "CCellNetworkInfov2::StartMonitoring--start");
    if (!IsActive()) {
        m_rmobilePhone.NotifyCurrentNetworkChange(iStatus, iNetworkInfoPckg, iLocation);
        SetActive();
    }
    TRACES(qDebug() << "CCellNetworkInfov2::StartMonitoring--End");
}

#endif
