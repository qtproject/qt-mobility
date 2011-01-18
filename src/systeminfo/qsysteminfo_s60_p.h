/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmobilityglobal.h"
#include "qsysteminfo.h"

#include <f32file.h>
#include "telephonyinfo_s60.h"
#include "chargingstatus_s60.h"
#include "wlaninfo_s60.h"
#include "storagestatus_s60.h"
#include "lockandflipstatus_s60.h"
#include <w32std.h>
#include "storagedisknotifier_s60.h"
#include "batterystatus_s60.h"
#include <ProfileEngineSDKCRKeys.h>
#include <hwrmvibrasdkcrkeys.h>

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
class QSystemNetworkInfoPrivate : public QObject, public MTelephonyInfoObserver
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    static int cellId();
    static int locationAreaCode();

    static QString currentMobileCountryCode();
    static QString currentMobileNetworkCode();

    static QString homeMobileCountryCode();
    static QString homeMobileNetworkCode();

    static QString networkName(QSystemNetworkInfo::NetworkMode mode); //signal
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();

Q_SIGNALS:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void currentMobileCountryCodeChanged(const QString &);
    void currentMobileNetworkCodeChanged(const QString &);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode);
    void cellIdChanged(int);//1.2

protected:  //from MTelephonyInfoObserver
    void batteryLevelChanged(){};

    void countryCodeChanged();
    void networkCodeChanged();
    void networkNameChanged();
    void networkModeChanged();

    void cellNetworkSignalStrengthChanged();
    void cellNetworkStatusChanged();

    void changedCellId(int);

public slots:
    void wlanNetworkNameChanged();
    void wlanNetworkSignalStrengthChanged();
    void wlanNetworkStatusChanged();
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

private:
    bool getSizeandRotation(int screen,TPixelsTwipsAndRotation& sizeAndRotation);
};

//////// QSystemStorageInfo
class QSystemStorageInfoPrivate : public QObject,
    public MStorageStatusObserver,
    public MStorageSpaceNotifyObserver
{
    Q_OBJECT

private:
    QSystemStorageInfo::StorageState CheckDiskSpaceThresholdLimit(const QString &);
public:
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
    // from MStorageSpaceNotifyObserver
    void DiskSpaceChanged(const QString &);

private:
    RFs iFs;


Q_SIGNALS:
    void logicalDriveChanged(bool, const QString &);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state); //1.2
};

//////// QSystemDeviceInfo


class DeviceInfo;
QTM_END_NAMESPACE

#include <mproengprofileactivationobserver.h>
#include <cenrepnotifyhandler.h>

class MProEngEngine;
class MProEngNotifyHandler;

QTM_BEGIN_NAMESPACE

class QSystemDeviceInfoPrivate : public QObject,
    public MTelephonyInfoObserver,
    public MProEngProfileActivationObserver,
    public MCenRepNotifyHandlerCallback,
    public MChargingStatusObserver,
    public MKeylockStatusObserver,
    public MFlipStatusObserver
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoPrivate();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    // device
    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model();
    static QString productName();

    int batteryLevel() const;
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    bool isDeviceLocked();
    static QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();

    QSystemDeviceInfo::PowerState currentPowerState();

    bool currentBluetoothPowerState();

    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2
    bool isWirelessKeyboardConnected(); //1.2
    bool isKeyboardFlippedOpen();//1.2
    void keyboardConnected(bool connect);//1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
    QUuid uniqueDeviceID(); //1.2
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

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlipped(bool open);//1.2
    void deviceLocked(bool isLocked); // 1.2
    void lockStatusChanged(QSystemDeviceInfo::LockType); //1.2

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

    //from MKeylockStatusObserver
    void keylockStatusChanged(TInt aLockType);

    //from MFlipStatusObserver
    void flipStatusChanged(TInt aFlipType , TInt aFilpKeyBoard );


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

private Q_SLOTS:
    void resetInactivityTime();

private:    //data
    bool m_screenSaverInhibited;
};

//////// DeviceInfo (singleton)
class DeviceInfo
{
public:
    static DeviceInfo *instance()
    {
        if (!m_instance)
        {
            m_instance = new DeviceInfo;
        }
        return m_instance;
    }

    CPhoneInfo *phoneInfo()
    {
        if (!m_phoneInfo) {
            m_phoneInfo = new CPhoneInfo(*m_telephony);
        }
        return m_phoneInfo;
    }

    CSubscriberInfo *subscriberInfo()
    {
        if (!m_subscriberInfo) {
            m_subscriberInfo = new CSubscriberInfo(*m_telephony);
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
        if (!m_batteryInfo) {
            m_batteryInfo = new CBatteryInfo(*m_telephony);
        }
        return m_batteryInfo;
    }

    CCellNetworkInfo *cellNetworkInfo()
    {
        if (!m_cellNetworkInfo) {
            m_cellNetworkInfo = new CCellNetworkInfo(*m_telephony);
        }
        return m_cellNetworkInfo;
    }

    CCellNetworkRegistrationInfo *cellNetworkRegistrationInfo()
    {
        if (!m_cellNetworkRegistrationInfo) {
            m_cellNetworkRegistrationInfo = new CCellNetworkRegistrationInfo(*m_telephony);
        }
        return m_cellNetworkRegistrationInfo;
    }

    CCellSignalStrengthInfo *cellSignalStrenghtInfo()
    {
        if (!m_cellSignalStrengthInfo) {
            m_cellSignalStrengthInfo = new CCellSignalStrengthInfo(*m_telephony);
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

    CStorageDiskNotifier *storagedisknotifier()
    {
        if (!m_storagedisknotifier) {
            TRAP_IGNORE(m_storagedisknotifier = CStorageDiskNotifier::NewL();)
        }
        return m_storagedisknotifier;
    }

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
            TRAP_IGNORE(m_flipStatus->ConstructL();
            )
        }
        return m_flipStatus;
    }

    CBatteryCommonInfo *batteryCommonInfo ()
    {
        if (!m_batteryCommonInfo) {
            m_batteryCommonInfo = new CBatteryCommonInfo();
        }
        return m_batteryCommonInfo;
    }
private:
    DeviceInfo() : m_phoneInfo(NULL), m_subscriberInfo(NULL), m_chargingStatus(NULL),
        m_batteryInfo(NULL), m_cellNetworkInfo(NULL), m_cellNetworkRegistrationInfo(NULL),
        m_cellSignalStrengthInfo(NULL), m_wlanInfo(NULL), m_mmcStorageStatus(NULL), m_keylockStatus(NULL),m_flipStatus(NULL),m_storagedisknotifier(NULL), m_batteryCommonInfo(NULL)
    {
        m_telephony = CTelephony::NewL();
    };

    ~DeviceInfo()
    {
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
        delete m_keylockStatus;
        delete m_flipStatus;
        delete m_storagedisknotifier;
        delete m_batteryCommonInfo;
    }

    DeviceInfo(const DeviceInfo &);

    static DeviceInfo *m_instance;

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
    CKeylockStatus *m_keylockStatus;
    CFlipStatus *m_flipStatus;
    CStorageDiskNotifier* m_storagedisknotifier;
    CBatteryCommonInfo* m_batteryCommonInfo;
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
    qint32 startCurrentMeasurement(int rate);

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
