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

#ifndef QSYSTEMINFO_LINUX_COMMON_P_H
#define QSYSTEMINFO_LINUX_COMMON_P_H

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

#include "qsysteminfo.h"
#include "qsystemdeviceinfo.h"
#include "qsystemdisplayinfo.h"
#include "qsystemnetworkinfo.h"
#include "qsystemscreensaver.h"
#include "qsystemstorageinfo.h"
#include "qsystembatteryinfo.h"

#if !defined(QT_NO_HAL)
#include "qhalservice_linux_p.h"
#endif // QT_NO_HAL

#if !defined(QT_NO_UPOWER) || !defined(QT_NO_UDISKS)
#include "qdevicekitservice_linux_p.h"
#endif // QT_NO_UPOWER || QT_NO_UDISKS

#if !defined(QT_NO_CONNMAN)
#include "qconnmanservice_linux_p.h"
#include "qofonoservice_linux_p.h"
#endif // QT_NO_CONNMAN

QTM_BEGIN_NAMESPACE

class QSystemInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemInfoLinuxCommonPrivate();

    bool fmTransmitterAvailable();
    virtual bool hasFeatureSupported(QSystemInfo::Feature feature);
    bool hasSysFeature(const QString &featureStr);
    QString currentCountryCode() const;
    virtual QString currentLanguage() const;
    QString version(QSystemInfo::Version, const QString &parameter = QString());

Q_SIGNALS:
    void currentLanguageChanged(const QString &lang);

protected:
    QString currentLang;

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

#if !defined(QT_NO_HAL)
    bool hasHalDeviceFeature(const QString &param);
    bool hasHalUsbFeature(qint32 usbClass);
#endif // QT_NO_HAL

private:
    QTimer *langTimer;

private Q_SLOTS:
    void pollCurrentLanguage();
};

class QSystemNetworkInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemNetworkInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoLinuxCommonPrivate();

    int cellId();
    int locationAreaCode();
    int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();
    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);
    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::CellDataTechnology cellDataTechnology();
    QSystemNetworkInfo::NetworkMode currentMode();
    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);

Q_SIGNALS:
    void cellIdChanged(int cellId); //1.2
    void cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology cellTech); //1.2
    void currentMobileCountryCodeChanged(const QString &mcc);
    void currentMobileNetworkCodeChanged(const QString &mnc);
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &name);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode mode);

private:
    QSystemNetworkInfo::NetworkStatus getBluetoothNetStatus();

#if !defined(QT_NO_CONNMAN)
private:
    QConnmanManagerInterface *connmanManager;
    QOfonoManagerInterface *ofonoManager;
    QStringList knownModems;

    void initConnman();
    void initOfono();
    void initModem(const QString &path);
    QString modeToTechnology(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkStatus stateToStatus(const QString &state);
    QSystemNetworkInfo::NetworkMode typeToMode(const QString &type);
    QSystemNetworkInfo::NetworkMode ofonoTechToMode(const QString &tech);
    QSystemNetworkInfo::NetworkStatus ofonoStatusToStatus(const QString &state);
    QSystemNetworkInfo::NetworkStatus getOfonoStatus(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::CellDataTechnology ofonoTechToCDT(const QString &tech);

private Q_SLOTS:
    void connmanPropertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
    void connmanServicePropertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
    void ofonoPropertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
    void ofonoNetworkPropertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
#endif // QT_NO_CONNMAN
};

class QSystemDisplayInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemDisplayInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoLinuxCommonPrivate();

    int colorDepth(int screen);
    int displayBrightness(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
    float contrast(int screen);
    QSystemDisplayInfo::DisplayOrientation orientation(int screen);
    QSystemDisplayInfo::BacklightState backlightStatus(int screen); //1.2

    static QSystemDisplayInfoLinuxCommonPrivate *instance() { return self; }

#if defined(Q_WS_X11)
    void emitOrientationChanged(int curRotation);
    int xEventBase;
    int xErrorBase;
    int lastRotation;
#endif // Q_WS_X11

Q_SIGNALS:
    void orientationChanged(QSystemDisplayInfo::DisplayOrientation newOrientation);

private:
    static QSystemDisplayInfoLinuxCommonPrivate *self;

    bool isScreenValid(int screen);
};

class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

    qlonglong availableDiskSpace(const QString &driveVolume);
    qlonglong totalDiskSpace(const QString &driveVolume);
    QString uriForDrive(const QString &driveVolume); //1.2
    QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);
    QSystemStorageInfo::StorageState getStorageState(const QString &volume); //1.2

Q_SIGNALS:
    void logicalDriveChanged(bool added, const QString &vol);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state); //1.2

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    int inotifyWatcher;
    int inotifyFileDescriptor;
    QMap<QString, QString> mountedEntries;
    QMap<QString, QSystemStorageInfo::StorageState> storageStates;
    QTimer *storageTimer;

    void updateMountedEntries();
    QString getUuid(const QString &vol);

private Q_SLOTS:
    void deviceChanged();
    void inotifyActivated();
    void updateStorageStates();

#if !defined(QT_NO_UDISKS)
private:
    QUDisksInterface *udisksIface;

private Q_SLOTS:
    void udisksDeviceChanged(const QDBusObjectPath &);
#endif // QT_NO_UDISKS
};

class QSystemDeviceInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemDeviceInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoLinuxCommonPrivate();

    bool currentBluetoothPowerState();
    bool isWirelessKeyboardConnected(); //1.2
    int batteryLevel() const;
    QByteArray uniqueDeviceID(); //1.2
    QString imei();
    QString imsi();
    QString manufacturer();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::InputMethodFlags inputMethodType();
    QSystemDeviceInfo::PowerState currentPowerState();
    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2

Q_SIGNALS:
    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus status);
    void bluetoothStateChanged(bool state);
    void currentProfileChanged(QSystemDeviceInfo::Profile profile);
    void deviceLocked(bool isLocked); // 1.2
    void keyboardFlipped(bool open); //1.2
    void lockStatusChanged(QSystemDeviceInfo::LockTypeFlags); //1.2
    void powerStateChanged(QSystemDeviceInfo::PowerState state);
    void thermalStateChanged(QSystemDeviceInfo::ThermalState state);
    void wirelessKeyboardConnected(bool connected); //1.2

protected:
    bool btPowered;

#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_HAL)
    QHalDeviceInterface *halIfaceDevice;
#endif // QT_NO_HAL
    bool hasWirelessKeyboardConnected;
    bool connectedBtPower;
    bool connectedWirelessKeyboard;
    void connectBtPowered(const QString &str);
    void connectBtKeyboard(const QString &str);

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private Q_SLOTS:
#if !defined(QT_NO_HAL)
    virtual void halChanged(int,QVariantList);
#endif // QT_NO_HAL
    void bluezPropertyChanged(const QString&, QDBusVariant);
    virtual void upowerChanged();
    virtual void upowerDeviceChanged();
#endif // QT_NO_DBUS

private:
    void initBatteryStatus();

    QSystemDeviceInfo::BatteryStatus currentBatStatus;
    int currentBatLevel;
    QSystemDeviceInfo::PowerState curPowerState;
    bool currentBatStatusInitialized;
    bool currentBatLevelInitialized;
};

class QSystemBatteryInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    explicit QSystemBatteryInfoLinuxCommonPrivate(QObject *parent = 0);
    ~QSystemBatteryInfoLinuxCommonPrivate();

    int currentFlow() const;
    int maxBars() const;
    int nominalCapacity() const;
    int remainingCapacity() const;
    int remainingCapacityBars() const;
    int remainingCapacityPercent() const;
    int remainingChargingTime() const;
    int voltage() const;
    QSystemBatteryInfo::BatteryStatus batteryStatus() const;
    QSystemBatteryInfo::ChargerType chargerType() const;
    QSystemBatteryInfo::ChargingState chargingState() const;
    QSystemBatteryInfo::EnergyUnit energyMeasurementUnit() const;

Q_SIGNALS:
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);
    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);
    void currentFlowChanged(int flow);
    void nominalCapacityChanged(int capacity);
    void remainingCapacityPercentChanged(int capacity);
    void remainingCapacityChanged(int capacity);
    void remainingCapacityBarsChanged(int capacity);
    void remainingChargingTimeChanged(int time);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_HAL)
    QHalDeviceInterface *halIfaceDevice;
#endif // QT_NO_HAL
    QSystemBatteryInfo::ChargerType currentChargerType();

private Q_SLOTS:
    void setConnection();
#if !defined(QT_NO_HAL)
    virtual void halChanged(int,QVariantList);
#endif // QT_NO_HAL
    void getBatteryStats();
    void timeout();
#if !defined(QT_NO_UPOWER)
    void uPowerPropertyChanged(const QString &, const QVariant &);
#endif // QT_NO_UPOWER
#endif

protected:
    QSystemBatteryInfo::ChargerType curChargeType;

private:
    bool batteryIsPresent;
    QSystemBatteryInfo::BatteryStatus currentBatStatus;
    QSystemBatteryInfo::ChargingState curChargeState;
    int currentBatLevelPercent;
    int currentVoltage;
    int dischargeRate;
    int capacity;
    int timeToFull;
    int remainingEnergy;
    int  batteryLevel() const;

#if !defined(QT_NO_UPOWER)
    QUPowerDeviceInterface *battery;
#endif // QT_NO_UPOWER
};

QTM_END_NAMESPACE

#endif // QSYSTEMINFO_LINUX_COMMON_P_H
