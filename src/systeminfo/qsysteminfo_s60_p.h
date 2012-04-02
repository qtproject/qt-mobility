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
#ifndef QSYSTEMINFO_S60_H
#define QSYSTEMINFO_S60_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QDesktopWidget>

#include "qmobilityglobal.h"
#include "qsysteminfo.h"

#include <f32file.h>
#include "telephonyinfo_s60.h"
#include "chargingstatus_s60.h"
#include "wlaninfo_s60.h"
#include "storagestatus_s60.h"
#include <w32std.h>
#include "batterystatus_s60.h"
#include "networkinfo_s60.h"
#include <ProfileEngineSDKCRKeys.h>
#include <hwrmvibrasdkcrkeys.h>

#ifdef LOCKANDFLIP_SUPPORTED
#include "lockandflipstatus_s60.h"
#endif

#ifdef DISKNOTIFY_SUPPORTED
#include "storagedisknotifier_s60.h"
#endif

#ifdef THERMALSTATUS_SUPPORTED
#include "thermalstatus_s60.h"
#endif

#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
#include "networkoperatornamelistener_s60.h"
#endif

#include <QThread>
QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

const int KMaxBatteryBars = 7; //Max number of battery bars (7 is fixed for all symbian devices now)

//////// QSystemInfo
class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();

    QString currentLanguage() const;
    QStringList availableLanguages() const;
    QString currentCountryCode() const;

    QString version(QSystemInfo::Version type, const QString &parameter = QString());
    bool hasFeatureSupported(QSystemInfo::Feature feature);

Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:
    QString QLocaleToISO639_1(QLocale::Language language) const;
    QString TLanguageToISO639_1(TLanguage language) const;
    QString S60Version() const;
};

//////// QSystemNetworkInfo
class QSystemNetworkInfoPrivate : public QObject, public MTelephonyInfoObserver, public MNetworkInfoObserver, public MWlanInfoObserver
#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
, public MNetworkOperatorNameObserver
#endif
{
    Q_OBJECT

public:
    static QSystemNetworkInfoPrivate* networkinfoPrivateInstance();
    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode); //signal
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();

    QSystemNetworkInfo::CellDataTechnology cellDataTechnology();

Q_SIGNALS:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void currentMobileCountryCodeChanged(const QString &);
    void currentMobileNetworkCodeChanged(const QString &);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode);
    void cellIdChanged(int);//1.2
    void cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology);

protected:  //from MTelephonyInfoObserver
    void batteryLevelChanged(){};

    void countryCodeChanged();
    void networkCodeChanged();
    void networkNameChanged();
    void networkModeChanged();

    void cellNetworkSignalStrengthChanged();
    void cellNetworkStatusChanged();

    void changedCellId(int);
    virtual void changedNetworkStatus();
    virtual void changedNetworkMode();
    virtual void changedCellDataTechnology();

    //from MWLanInfoObserver
    void wlanNetworkNameChanged();
    void wlanNetworkSignalStrengthChanged();
    void wlanNetworkStatusChanged();

#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
    //from MNetworkOperatorNameObserver
    void OperatorNameChanged();
#endif

//public slots:
    //void wlanNetworkNameChanged();
    //void wlanNetworkSignalStrengthChanged();
    //void wlanNetworkStatusChanged();
};

//////// QSystemDisplayInfo
class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    static int displayBrightness(int screen);
    static int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation orientation(int screen);
    float contrast(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
    QSystemDisplayInfo::BacklightState backlightStatus(int screen); //1.2

Q_SIGNALS:
    void orientationChanged(QSystemDisplayInfo::DisplayOrientation newOrientation);

public slots:
    void rotationTimeout();

private:
    bool getSizeandRotation(int screen,TPixelsTwipsAndRotation& sizeAndRotation);

    QSystemDisplayInfo::DisplayOrientation currentOrientation;
    QTimer *rotationTimer;
};

//////// QSystemStorageInfo
class QSystemStorageInfoPrivate : public QObject,
    public MStorageStatusObserver
#ifdef DISKNOTIFY_SUPPORTED
    ,public MStorageSpaceNotifyObserver
#endif
{
    Q_OBJECT

private:
    QSystemStorageInfo::StorageState CheckDiskSpaceThresholdLimit(const QString &);
public:
    static QSystemStorageInfoPrivate* storageinfoPrivateInstance();
    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();
    qlonglong totalDiskSpace(const QString &driveVolume);
    qlonglong availableDiskSpace(const QString &driveVolume);
    static QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

    QString uriForDrive(const QString &driveVolume);//1.2
    QSystemStorageInfo::StorageState getStorageState(const QString &volume);//1.2

protected: // from MStorageStatusObserver
    void storageStatusChanged(bool, const QString &);
#ifdef DISKNOTIFY_SUPPORTED
    // from MStorageSpaceNotifyObserver
    void DiskSpaceChanged(const QString &);
#endif

private:
    RFs iFs;


Q_SIGNALS:
    void logicalDriveChanged(bool, const QString &);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state); //1.2
};

//////// QSystemDeviceInfo


QTM_END_NAMESPACE

#include <MProEngProfileActivationObserver.h>
#include <cenrepnotifyhandler.h>

class MProEngEngine;
class MProEngNotifyHandler;

QTM_BEGIN_NAMESPACE

class QSystemDeviceInfoPrivate : public QObject,
    public MTelephonyInfoObserver,
    public MProEngProfileActivationObserver,
    public MCenRepNotifyHandlerCallback,
    public MChargingStatusObserver
#ifdef LOCKANDFLIP_SUPPORTED
    ,public MKeylockStatusObserver,public MFlipStatusObserver
#endif
#ifdef THERMALSTATUS_SUPPORTED
    ,public MThermalStatusObserver
#endif
{
    Q_OBJECT

public:
    static QSystemDeviceInfoPrivate* deviceinfoPrivateInstance();
    QSystemDeviceInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoPrivate();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    // device
    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    QString productName();

    int batteryLevel() const;
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    bool isDeviceLocked();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::ProfileDetails currentProfileDetails;

    QSystemDeviceInfo::PowerState currentPowerState();
    QSystemDeviceInfo::ThermalState currentThermalState();

    bool currentBluetoothPowerState();

    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2
    bool isWirelessKeyboardConnected(); //1.2
    bool isKeyboardFlippedOpen();//1.2
    void keyboardConnected(bool connect);//1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
    QByteArray uniqueDeviceID(); //1.2
    QSystemDeviceInfo::LockTypeFlags lockStatus(); //1.2

    int messageRingtoneVolume();//1.2
    int voiceRingtoneVolume();//1.2
    bool vibrationActive();//1.2
Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus);
    void bluetoothStateChanged(bool);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void thermalStateChanged(QSystemDeviceInfo::ThermalState);

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlipped(bool open);//1.2
    void deviceLocked(bool isLocked); // 1.2
    void lockStatusChanged(QSystemDeviceInfo::LockTypeFlags); //1.2

protected:
    //From QObject
    void connectNotify(const char *signal);

//From MProEngProfileActivationObserver
    void HandleProfileActivatedL(TInt aProfileId);

    //From MCenRepNotifyHandlerCallback
    void HandleNotifyInt(TUint32 aId, TInt aNewValue);
    //from MTelephonyInfoObserver
    void batteryLevelChanged();

    void countryCodeChanged(){};
    void networkCodeChanged(){};
    void networkNameChanged(){};
    void networkModeChanged(){};
    void changedCellId(int){};

    void cellNetworkSignalStrengthChanged(){};
    void cellNetworkStatusChanged(){};

    //from MChargingStatusObserver
    void chargingStatusChanged();

#ifdef LOCKANDFLIP_SUPPORTED
    //from MKeylockStatusObserver
    void keylockStatusChanged(TInt aLockType);

    //from MFlipStatusObserver
    void flipStatusChanged(TInt aFlipType , TInt aFilpKeyBoard );
#endif

#ifdef THERMALSTATUS_SUPPORTED
    //from MThermalStatusObserver
    void NotiftythermalStateChanged(TUint8 aThermalStatus);
#endif
private:
    QSystemDeviceInfo::Profile s60ProfileIdToProfile(TInt profileId) const;

//data
    MProEngEngine *m_profileEngine;
    MProEngNotifyHandler* m_proEngNotifyHandler;

    CRepository *m_bluetoothRepository;
    CCenRepNotifyHandler *m_bluetoothNotifyHandler;

    bool hasWirelessKeyboardConnected;

    QSystemDeviceInfo::BatteryStatus m_previousBatteryStatus;
};

//////// QSystemScreenSaver
class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    void setScreenSaverInhibited(bool on);

private Q_SLOTS:
    void resetInactivityTime();

private:    //data
    bool m_screenSaverInhibited;
    QTimer *timer;
};

//////// DeviceInfo (singleton)
class DeviceInfo
{
public:
    static DeviceInfo *instance()
      {
      TRACES( qDebug() << "DeviceInfo::instance()"
              << ": QThread id = " << QThread::currentThread()
              << "Symbian thread id = " << RThread().Id().Id());
      if (!Dll::Tls())
        {
           Dll::SetTls(new DeviceInfo);
        TRACES( qDebug() << "DeviceInfo::instance() CREATED!"
               << ": QThread id = " << QThread::currentThread()
               << "Symbian thread id = " << RThread().Id().Id());
        }
      return static_cast<DeviceInfo *>(Dll::Tls());
    }

    void incrementRefCount()
        {
            m_refcount = m_refcount+1;
            TRACES( qDebug() << "DeviceInfo RefCount:" << m_refcount);
        }

    int getRefCount()
        {
            TRACES( qDebug() << "Get RefCount:" << m_refcount);
            return m_refcount;
        }

    void decrementRefCount()
        {
            m_refcount = m_refcount-1;
            TRACES( qDebug() << "Decrement RefCount:" << m_refcount);
        }

  void initMobilePhonehandleL()
    {
  #ifdef ETELMM_SUPPORTED
        if (m_rmobilePhoneInitialised)
          return;

        TInt err = KErrNone;
        err = m_etelServer.Connect();
        if ( err != KErrNone ) {
          TRACES (qDebug() << "DeviceInfo:: InitMobilePhonehandle err code RTelServer::Connect" << err);
          m_etelServer.Close();
          User::Leave(err);
        }

        //Eumerate legal phone
        // Get number of phones
        TInt phones(0);
        err = m_etelServer.EnumeratePhones(phones);
        if ( err != KErrNone) {
            m_etelServer.Close();
            TRACES (qDebug() << "DeviceInfo:: EnumeratePhones err code" << err);
            User::Leave(err);
        }

        // Get phone info of first legal phone.
        TInt legalPhoneIndex = KErrNotFound;
        for (TInt i=0; i<phones && legalPhoneIndex == KErrNotFound; i++)
        {
          if (m_etelServer.GetPhoneInfo(i, m_etelphoneInfo) == KErrNone)
            {
            if (m_etelphoneInfo.iNetworkType == RTelServer::ENetworkTypeMobileDigital)
              {
                legalPhoneIndex = i;
              }
            }
        }
        if ( legalPhoneIndex == KErrNotFound) {
            m_etelServer.Close();
            TRACES (qDebug() << "DeviceInfo:: legalPhoneIndex err code" << legalPhoneIndex);
            User::Leave(legalPhoneIndex);
        }

        err = m_rmobilePhone.Open(m_etelServer,m_etelphoneInfo.iName);
        if (err != KErrNone) {
          TRACES (qDebug() << "DeviceInfo:: InitMobilePhonehandle err code RMobilePhone::Open =" << err);
            m_rmobilePhone.Close();
            m_etelServer.Close();
          User::Leave(err);
        }

        err = m_rmobilePhone.Initialise();
        if (err != KErrNone) {
            TRACES (qDebug() << "DeviceInfo:: InitMobilePhonehandle err val for RMobilePhone::Initialise =" << err);
            m_rmobilePhone.Close();
            m_etelServer.Close();
            User::Leave(err);
        }

        TRACES (qDebug() << "InitMobilePhonehandleL- successful");
        m_rmobilePhoneInitialised = true;
  #endif
    }

  void initRmobilePhone()
    {
  #ifdef ETELMM_SUPPORTED
        TRAPD ( err,initMobilePhonehandleL());
        if ( err != KErrNone) m_rmobilePhoneInitialised = false;
  #endif
    }

    bool etelmmSupported()
      {
      #ifdef ETELMM_SUPPORTED
            return true;
      #else
            return false;
      #endif
      }

    CPhoneInfo *phoneInfo()
    {
      if (etelmmSupported() && !m_rmobilePhoneInitialised) initRmobilePhone();
        if (!m_phoneInfo) {
#ifdef ETELMM_SUPPORTED
        m_phoneInfo = new CPhoneInfo(m_rmobilePhone);
#else
        m_phoneInfo = new CPhoneInfo(*m_telephony);
#endif
        }
        return m_phoneInfo;
    }

    CSubscriberInfo *subscriberInfo()
    {
      if (etelmmSupported() && !m_rmobilePhoneInitialised) initRmobilePhone();
        if (!m_subscriberInfo) {
#ifdef ETELMM_SUPPORTED
         m_subscriberInfo = new CSubscriberInfo(m_rmobilePhone);
#else
         m_subscriberInfo = new CSubscriberInfo(*m_telephony);
#endif
        }
        return m_subscriberInfo;
    }

    CChargingStatus *chargingStatus()
    {
        if (!m_chargingStatus) {
            m_chargingStatus = new CChargingStatus;
        }
        return m_chargingStatus;
    }

    CBatteryInfo *batteryInfo()
    {
      if (etelmmSupported() && !m_rmobilePhoneInitialised) initRmobilePhone();
        if (!m_batteryInfo) {
#ifdef ETELMM_SUPPORTED
         m_batteryInfo = new CBatteryInfo(m_rmobilePhone);
#else
         m_batteryInfo = new CBatteryInfo(*m_telephony);
#endif
        }
        return m_batteryInfo;
    }

    CCellNetworkInfo *cellNetworkInfo()
    {
      if (etelmmSupported() && !m_rmobilePhoneInitialised) initRmobilePhone();
        if (!m_cellNetworkInfo) {
#ifdef ETELMM_SUPPORTED
         m_cellNetworkInfo = new CCellNetworkInfo(m_rmobilePhone);
#else
         m_cellNetworkInfo = new CCellNetworkInfo(*m_telephony);
#endif
        }
        return m_cellNetworkInfo;
    }

    CCellNetworkRegistrationInfo *cellNetworkRegistrationInfo()
    {
      if (etelmmSupported() && !m_rmobilePhoneInitialised) initRmobilePhone();
        if (!m_cellNetworkRegistrationInfo) {
#ifdef ETELMM_SUPPORTED
        m_cellNetworkRegistrationInfo = new CCellNetworkRegistrationInfo(m_rmobilePhone);
#else
        m_cellNetworkRegistrationInfo = new CCellNetworkRegistrationInfo(*m_telephony);
#endif
        }
        return m_cellNetworkRegistrationInfo;
    }

    CCellSignalStrengthInfo *cellSignalStrenghtInfo()
    {
      if (etelmmSupported() && !m_rmobilePhoneInitialised) initRmobilePhone();
        if (!m_cellSignalStrengthInfo) {
#ifdef ETELMM_SUPPORTED
        m_cellSignalStrengthInfo = new CCellSignalStrengthInfo(m_rmobilePhone);
#else
        m_cellSignalStrengthInfo = new CCellSignalStrengthInfo(*m_telephony);
#endif
        }
        return m_cellSignalStrengthInfo;
    }

    CWlanInfo *wlanInfo()
    {
        if (!m_wlanInfo) {
            m_wlanInfo = new CWlanInfo;
        }
        return m_wlanInfo;
    }

    CMMCStorageStatus *mmcStorageStatus()
    {
        if (!m_mmcStorageStatus) {
            m_mmcStorageStatus = new CMMCStorageStatus;
        }
        return m_mmcStorageStatus;
    }

#ifdef DISKNOTIFY_SUPPORTED
    CStorageDiskNotifier *storagedisknotifier()
    {
        if (!m_storagedisknotifier) {
            TRAP_IGNORE(m_storagedisknotifier = CStorageDiskNotifier::NewL();)
        }
        return m_storagedisknotifier;
    }
#endif

#ifdef LOCKANDFLIP_SUPPORTED
    CKeylockStatus *keylockStatus()
    {
        if (!m_keylockStatus) {
            m_keylockStatus = new CKeylockStatus;
        }
        return m_keylockStatus;
    }

    CFlipStatus *flipStatus()
    {
        if (!m_flipStatus) {
            m_flipStatus = new CFlipStatus;
        }
        return m_flipStatus;
    }
#endif

#ifdef THERMALSTATUS_SUPPORTED
    CThermalStatus *thermalStatus()
        {
           if (!m_thermalStatus)
            {
             m_thermalStatus = new CThermalStatus;
            }
            return m_thermalStatus;
        }
#endif

#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
     CNetworkOperatorNameListener *networkInfoListener()
        {
         if (!m_networkinfolistener)
          {
           m_networkinfolistener = CNetworkOperatorNameListener::NewL();
          }
          return m_networkinfolistener;
        }
#endif

    CBatteryCommonInfo *batteryCommonInfo ()
    {
        if (!m_batteryCommonInfo) {
            m_batteryCommonInfo = new CBatteryCommonInfo();
        }
        return m_batteryCommonInfo;
    }

    CNetworkInfo* networkInfo ()
    {
        if (!m_networkInfo)
            m_networkInfo = new CNetworkInfo();
        if (!m_networkInfo->IsInitialised())
            m_networkInfo->Initialise();
        return m_networkInfo;
    }

    DeviceInfo() : m_phoneInfo(NULL), m_subscriberInfo(NULL), m_chargingStatus(NULL),
        m_batteryInfo(NULL), m_cellNetworkInfo(NULL), m_cellNetworkRegistrationInfo(NULL),
        m_cellSignalStrengthInfo(NULL), m_wlanInfo(NULL), m_mmcStorageStatus(NULL), m_batteryCommonInfo(NULL), m_networkInfo(NULL)
#ifdef LOCKANDFLIP_SUPPORTED
        ,m_keylockStatus(NULL),m_flipStatus(NULL)
#endif
#ifdef DISKNOTIFY_SUPPORTED
        ,m_storagedisknotifier(NULL)
#endif
#ifdef THERMALSTATUS_SUPPORTED
        ,m_thermalStatus(NULL)
#endif
#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
        ,m_networkinfolistener(NULL)
#endif
    ,m_rmobilePhoneInitialised(false)
    ,m_refcount(0)
    {
        TRACES(qDebug() << "DeviceInfo():Constructor");
        m_telephony = CTelephony::NewL();
    };

    ~DeviceInfo()
    {
        TRACES(qDebug() << "DeviceInfo():Destructor++");
        delete m_cellSignalStrengthInfo;
        delete m_cellNetworkRegistrationInfo;
        delete m_cellNetworkInfo;
        delete m_batteryInfo;
        delete m_chargingStatus;
        delete m_subscriberInfo;
        delete m_phoneInfo;
        delete m_telephony;
        delete m_wlanInfo;
        delete m_mmcStorageStatus;
        delete m_batteryCommonInfo;
        delete m_networkInfo;
#ifdef LOCKANDFLIP_SUPPORTED
        delete m_keylockStatus;
        delete m_flipStatus;
#endif
#ifdef DISKNOTIFY_SUPPORTED
        delete m_storagedisknotifier;
#endif

#ifdef THERMALSTATUS_SUPPORTED
        delete m_thermalStatus;
#endif

#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
        delete m_networkinfolistener;
#endif

#ifdef ETELMM_SUPPORTED
        if ( m_rmobilePhoneInitialised) {
            TRACES ( qDebug() << "Closing RMobilePhone handles++");
            m_rmobilePhone.Close();
            m_etelServer.Close();
            TRACES ( qDebug() << "Closing RMobilePhone handles--");
            }
#endif
    TRACES(qDebug() << "DeviceInfo():Destructor--");
    }

private:
    DeviceInfo(const DeviceInfo &);

    CTelephony *m_telephony;
    CPhoneInfo *m_phoneInfo;
    CSubscriberInfo *m_subscriberInfo;
    CChargingStatus *m_chargingStatus;
    CBatteryInfo *m_batteryInfo;
    CCellNetworkInfo *m_cellNetworkInfo;
    CCellNetworkRegistrationInfo *m_cellNetworkRegistrationInfo;
    CCellSignalStrengthInfo *m_cellSignalStrengthInfo;
    CWlanInfo* m_wlanInfo;
    CMMCStorageStatus* m_mmcStorageStatus;
    CBatteryCommonInfo* m_batteryCommonInfo;
    CNetworkInfo* m_networkInfo;
#ifdef LOCKANDFLIP_SUPPORTED
    CKeylockStatus *m_keylockStatus;
    CFlipStatus *m_flipStatus;
#endif
#ifdef DISKNOTIFY_SUPPORTED
    CStorageDiskNotifier* m_storagedisknotifier;
#endif

#ifdef THERMALSTATUS_SUPPORTED
    CThermalStatus* m_thermalStatus;
#endif

#ifdef NETWORKHANDLER_SYMBIAN_SUPPORTED
   CNetworkOperatorNameListener* m_networkinfolistener;
#endif

#ifdef ETELMM_SUPPORTED
    RTelServer m_etelServer;
    RMobilePhone m_rmobilePhone;
    RTelServer::TPhoneInfo m_etelphoneInfo;
#endif
  bool m_rmobilePhoneInitialised;
  int m_refcount;
};

class QSystemBatteryInfoPrivate : public QObject, public MBatteryInfoObserver, public MBatteryHWRMObserver
{
    Q_OBJECT
public:
    QSystemBatteryInfoPrivate(QObject *parent = 0);
    ~QSystemBatteryInfoPrivate();

    QSystemBatteryInfo::ChargerType chargerType() const;
    QSystemBatteryInfo::ChargingState chargingState() const;

    int nominalCapacity() const;
    int remainingCapacityPercent() const;
    int remainingCapacity() const;

    int voltage() const;
    int remainingChargingTime() const;
    int currentFlow() const;
    int remainingCapacityBars() const;
    int maxBars() const;
    QSystemBatteryInfo::BatteryStatus batteryStatus() const;
    QSystemBatteryInfo::EnergyUnit energyMeasurementUnit() const;

Q_SIGNALS:
    void batteryLevelChanged(qint32 level);
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);


    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void nominalCapacityChanged(int);
    void remainingCapacityPercentChanged(int);
    void remainingCapacitymAhChanged(int);
    void batteryCurrentFlowChanged(int);
    void currentFlowChanged(int);
    void cumulativeCurrentFlowChanged(int);
    void remainingCapacityBarsChanged(int);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    void changedBatteryStatus() ;
    void changedChargingState() ;
    void changedChargerType() ;
    void changedRemainingCapacityBars();
    void changedCurrentFlow(int);

private :
    CBatteryHWRM *m_batteryHWRM;
    QSystemBatteryInfo::ChargerType m_charger, m_previousChagrger;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_S60_H*/

// End of file
