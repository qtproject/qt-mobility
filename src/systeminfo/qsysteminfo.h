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
#ifndef QSYSTEMINFO_H
#define QSYSTEMINFO_H
#include "qmobilityglobal.h"

#include <QObject>
#include <QSize>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QNetworkInterface>
#include <QFileSystemWatcher>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemInfoPrivate;
class QSystemNetworkInfoPrivate;
class QSystemStorageInfoPrivate;
class QSystemDeviceInfoPrivate;
class QSystemDisplayInfoPrivate;


class Q_SYSINFO_EXPORT QSystemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages)
    Q_PROPERTY(QString currentCountryCode READ currentCountryCode)
    Q_ENUMS(Version)
    Q_ENUMS(Feature)

public:

    QSystemInfo(QObject *parent = 0);
     virtual ~QSystemInfo();

    QString currentLanguage(); // 2 letter ISO 639-1 //signal
    QStringList availableLanguages(); // 2 letter ISO 639-1
    QString currentCountryCode(); //2 letter ISO 3166-1
    enum Version {
        Os = 1,
        QtCore,
        Firmware
    };

    QString version(QSystemInfo::Version type, const QString &parameter = QString());

    enum Feature {
        BluetoothFeature=0,
        CameraFeature,
        FmradioFeature,
        IrFeature,
        LedFeature,
        MemcardFeature,
        UsbFeature,
        VibFeature,
        WlanFeature,
        SimFeature,
        LocationFeature,
        VideoOutFeature,
        HapticsFeature
	};

    bool hasFeatureSupported(QSystemInfo::Feature feature);

Q_SIGNALS:
    void currentLanguageChanged(const QString &);
private:
    QSystemInfoPrivate *d;
};

class  Q_SYSINFO_EXPORT QSystemNetworkInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkStatus)
    Q_ENUMS(NetworkMode)
    Q_PROPERTY(int cellId READ cellId)
    Q_PROPERTY(int locationAreaCode READ locationAreaCode)
    Q_PROPERTY(QString currentMobileCountryCode READ currentMobileCountryCode NOTIFY currentMobileCountryCodeChanged)
    Q_PROPERTY(QString currentMobileNetworkCode READ currentMobileNetworkCode NOTIFY currentMobileNetworkCodeChanged)
    Q_PROPERTY(QString homeMobileCountryCode READ homeMobileCountryCode CONSTANT)
    Q_PROPERTY(QString homeMobileNetworkCode READ homeMobileNetworkCode CONSTANT)
    Q_PROPERTY(QSystemNetworkInfo::NetworkMode currentMode READ currentMode)


public:

    QSystemNetworkInfo(QObject *parent = 0);
    ~QSystemNetworkInfo();

    enum NetworkStatus {
        UndefinedStatus = 0,
        NoNetworkAvailable,
        EmergencyOnly,
        Searching,
        Busy,
        Connected,
        HomeNetwork,
        Denied,
        Roaming
    };

    enum NetworkMode {
        UnknownMode=0,
        GsmMode,
        CdmaMode,
        WcdmaMode,
        WlanMode,
        EthernetMode,
        BluetoothMode,
        WimaxMode
    };
    Q_DECLARE_FLAGS(NetworkModes, NetworkMode)

    Q_INVOKABLE QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();

    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();
    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();
    Q_INVOKABLE static QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);


Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

protected:
    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

private:
       QSystemNetworkInfoPrivate *d;
};


class  Q_SYSINFO_EXPORT QSystemDisplayInfo : public QObject
{
Q_OBJECT
    Q_ENUMS(DisplayOrientation)

public:

    QSystemDisplayInfo(QObject *parent = 0);
    ~QSystemDisplayInfo();

    enum DisplayOrientation {
        Unknown = 0,
        Landscape,
        Portrait,
        InvertedLandscape,
        InvertedPortrait
    };

    static int displayBrightness(int screen);
    static int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation getOrientation(int screen);
    float contrast(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
};


class  Q_SYSINFO_EXPORT QSystemStorageInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList logicalDrives READ logicalDrives)
    Q_ENUMS(DriveType)

public:

    QSystemStorageInfo(QObject *parent = 0);
    ~QSystemStorageInfo();

    enum DriveType {
        NoDrive = 0,
        InternalDrive,
        RemovableDrive,
        RemoteDrive,
        CdromDrive
	};

    Q_INVOKABLE qlonglong totalDiskSpace(const QString &driveVolume);
    Q_INVOKABLE qlonglong availableDiskSpace(const QString &driveVolume);
    static QStringList logicalDrives();

    Q_INVOKABLE QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

Q_SIGNALS:
    void storageAdded(/*const QString &driveVolume*/);
    void storageRemoved(/*const QString &driveVolume*/);

private:
       QSystemStorageInfoPrivate *d;
};

class  Q_SYSINFO_EXPORT QSystemDeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Profile currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(PowerState currentPowerState READ currentPowerState NOTIFY powerStateChanged)
    Q_PROPERTY(SimStatus simStatus READ simStatus CONSTANT)
    Q_PROPERTY(BatteryStatus batteryStatus READ batteryStatus NOTIFY batteryStatusChanged)
    Q_PROPERTY(InputMethodFlags inputMethodType READ inputMethodType)

    Q_PROPERTY(QString imei READ imei CONSTANT)
    Q_PROPERTY(QString imsi READ imsi CONSTANT)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString productName READ productName CONSTANT)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(bool isDeviceLocked READ isDeviceLocked)


    Q_ENUMS(BatteryStatus)
    Q_ENUMS(PowerState)
    Q_FLAGS(InputMethod InputMethodFlags)
    Q_ENUMS(SimStatus)
    Q_ENUMS(Profile)

public:

    QSystemDeviceInfo(QObject *parent = 0);
    virtual ~QSystemDeviceInfo();

    enum BatteryStatus {
        NoBatteryLevel = 0,
        BatteryCritical,
        BatteryVeryLow,
        BatteryLow,
        BatteryNormal
    };


    enum PowerState {
        UnknownPower = 0,
        BatteryPower,
        WallPower,
        WallPowerChargingBattery
    };


    enum InputMethod {
        Keys = 0x0000001,
        Keypad = 0x0000002,
        Keyboard = 0x0000004,
        SingleTouch = 0x0000008,
        MultiTouch = 0x0000010,
        Mouse = 0x0000020
    };

    Q_DECLARE_FLAGS(InputMethodFlags, InputMethod)

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    QString productName();
    int batteryLevel() const;
   QSystemDeviceInfo::BatteryStatus batteryStatus();

    enum Profile {
        UnknownProfile = 0,
        SilentProfile,
        NormalProfile,
        LoudProfile,
        VibProfile,
        OfflineProfile,
        PowersaveProfile,
        CustomProfile
    };

    enum SimStatus {
        SimNotAvailable = 0,
        SingleSimAvailable,
        DualSimAvailable,
        SimLocked
	};

    bool isDeviceLocked();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::PowerState currentPowerState();

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus );
    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);

private:
    QSystemDeviceInfoPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSystemDeviceInfo::InputMethodFlags )

class QSystemScreenSaverPrivate;
class  Q_SYSINFO_EXPORT QSystemScreenSaver : public QObject
{
    Q_OBJECT
    Q_ENUMS(ScreenSaverState)
    Q_PROPERTY(bool screenSaverInhibited READ screenSaverInhibited)

public:

    QSystemScreenSaver(QObject *parent = 0);
    ~QSystemScreenSaver();

    bool screenSaverInhibited();
    Q_INVOKABLE bool setScreenSaverInhibit();

private:
    bool screenSaverIsInhibited;
    QSystemScreenSaverPrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_H*/

// End of file

