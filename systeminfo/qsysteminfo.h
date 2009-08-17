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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QSYSTEMINFO_H
#define QSYSTEMINFO_H
#include "qsysinfoglobal.h"

#include <QObject>
#include <QSize>
#include <QPair>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;

class QSystemInfoPrivate;
class QSystemNetworkInfoPrivate;
class QSystemMemoryInfoPrivate;
class QSystemDeviceInfoPrivate;
class QSystemDisplayInfoPrivate;

class Q_SYSINFO_EXPORT QSystemInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(Version)
    Q_ENUMS(Feature)

public:

    QSystemInfo(QObject *parent = 0);
     virtual ~QSystemInfo();

// general
    static QString currentLanguage(); // 2 letter ISO 639-1
    static QStringList availableLanguages();	 // 2 letter ISO 639-1

    enum Version {
        Os = 1,
        QtCore,
        WrtCore,
        Webkit,
        ServiceFramework,
        WrtExtensions,
        ServiceProvider,
        NetscapePlugin,
        WebApp,
        Firmware
    };

    QString getVersion(QSystemInfo::Version type, const QString &parameter = QString());
//    QPair<int,float> getVersion(QSystemInfo::Version type, const QString &parameter = QString());

    static QString currentCountryCode(); //2 letter ISO 3166-1

// features
    enum Feature {
        UnknownFeature = 0,
        BluetoothFeature,
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

private:
    QSystemInfoPrivate *d;
};

////////
class  Q_SYSINFO_EXPORT QSystemNetworkInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(CellNetworkStatus)
    Q_ENUMS(NetworkMode)

public:

    QSystemNetworkInfo(QObject *parent = 0);
    ~QSystemNetworkInfo();

    enum CellNetworkStatus {
        UndefinedStatus = 0,
        NoNetworkAvailable,
        EmergencyOnly,
        Searching,
        Busy,
        HomeNetwork,
        Denied,
        Roaming
    };

	QSystemNetworkInfo::CellNetworkStatus getCellNetworkStatus();

    enum NetworkMode {
        UnknownMode = 0x00000000,
        GsmMode = 0x00000001,
        CdmaMode = 0x00000002,
        WcdmaMode = 0x00000004,
        WlanMode = 0x00000008,
        EthMode = 0x00000010
    };
    Q_DECLARE_FLAGS(NetworkModes, NetworkMode)

    static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);

    static int cellId();
    static int locationAreaCode();

    static QString currentMobileCountryCode();
    static QString currentMobileNetworkCode();

    static QString homeMobileCountryCode();
    static QString homeMobileNetworkCode();

    static bool isWLANAccessible();

    static QString operatorName();

private:
    QSystemNetworkInfoPrivate *d;
};

////////
class  Q_SYSINFO_EXPORT QSystemDisplayInfo
{

public:

    QSystemDisplayInfo();
    ~QSystemDisplayInfo();

    static int displayBrightness(int screen);
    static int colorDepth(int screen);
    static bool isScreenLockOn();

};


////////
class  Q_SYSINFO_EXPORT QSystemMemoryInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(VolumeType)

public:

    QSystemMemoryInfo(QObject *parent = 0);
    ~QSystemMemoryInfo();

    enum VolumeType {
        NoVolume = 0,
        Internal,
        Removable,
        Remote,
        Cdrom
	};

    qlonglong totalDiskSpace(const QString &driveVolume);
    qlonglong availableDiskSpace(const QString &driveVolume);
    static QStringList listOfVolumes();

    QSystemMemoryInfo::VolumeType getVolumeType(const QString &driveVolume); //returns enum

    //bool isDiskSpaceCritical(const QString &driveVolume);

private:
    QSystemMemoryInfoPrivate *d;
};

////////
class  Q_SYSINFO_EXPORT QSystemDeviceInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(BatteryLevel)
    Q_ENUMS(PowerState)
    Q_ENUMS(InputMethod)

public:

    QSystemDeviceInfo(QObject *parent = 0);
    virtual ~QSystemDeviceInfo();

    enum BatteryLevel {
        NoBatteryLevel = 0,
        BatteryCritical,
        BatteryVeryLow,
        BatteryLow,
        BatteryNormal
    };


    enum PowerState {
        UnknownPower = 0,
        BatteryPower,
        WallPower
    };


    enum InputMethod {
        Keys = 0x0000001,
        Keypad = 0x0000002,
        Keyboard = 0x0000004,
        SingleTouch = 0x0000008,
        MultiTouch = 0x0000010,
        Mouse = 0x0000020
    };
    Q_DECLARE_FLAGS(InputMethods, InputMethod)

    QSystemDeviceInfo::InputMethods getInputMethodType();

    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model(); //external
    static QString productName(); //internal name

// ????
    QSystemDeviceInfo::BatteryLevel batteryLevel() const;
    bool isBatteryCharging();

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
		SingleAvailable,
		DualAvailable,
		Locked
	};

    QSystemDeviceInfo::SimStatus getSimStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile getCurrentProfile();

    QSystemDeviceInfo::PowerState currentPowerState();

Q_SIGNALS:
    void batteryLevelChanged(QSystemDeviceInfo::BatteryLevel);
    void powerStateChanged(QSystemDeviceInfo::PowerState);

private:
    QSystemDeviceInfoPrivate *d;


};


////////
class QSystemScreenSaverPrivate;
class  Q_SYSINFO_EXPORT QSystemScreenSaver : public QObject
{
    Q_OBJECT

public:

    QSystemScreenSaver(QObject *parent = 0);
    ~QSystemScreenSaver();

    enum ScreenSaverState {
        UnknownScreenSaverState = 0,
        ScreenSaverDisabled,
        ScreenSaverEnabled
    };

    virtual QSystemScreenSaver::ScreenSaverState screenSaverEnabled();
    virtual QSystemScreenSaver::ScreenSaverState screenBlankingEnabled();

    bool setScreenSaverEnabled(QSystemScreenSaver::ScreenSaverState b);
    bool setScreenBlankingEnabled(QSystemScreenSaver::ScreenSaverState b);

private:
    QSystemScreenSaver::ScreenSaverState screenSaverIsEnabled;
    QSystemScreenSaver::ScreenSaverState screenBlankingIsEnabled;
    QSystemScreenSaverPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_H*/

// End of file

