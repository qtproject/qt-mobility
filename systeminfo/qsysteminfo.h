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

#include <QObject>
#include <QSize>
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;

class QSystemInfoPrivate;
class QSystemNetworkInfoPrivate;
class QSystemMemoryInfoPrivate;
class QSystemDeviceInfoPrivate;
class QSystemDisplayInfoPrivate;

class QSystemInfo : public QObject
{
    Q_OBJECT

public:

    QSystemInfo(QObject *parent = 0);
     virtual ~QSystemInfo();
//    QSystemInfo::Error error() const;

// general
    QString currentLanguage() const; // 2 letter ISO 639-1
    QStringList availableLanguages() const;	 // 2 letter ISO 639-1

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

    QString countryCode() const; //2 letter ISO 3166-1

// features
    enum Feature {
        UnknownFeature = -1,
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
    QString getDetailOfFeature(QSystemInfo::Feature feature);

private:
    QSystemInfoPrivate *d;
};

////////
class QSystemNetworkInfo : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfo(QObject *parent = 0);
    ~QSystemNetworkInfo();

    enum CellNetworkStatus {
        UndefinedStatus = -1,
        NoNetworkAvailable = 1,
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

    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode);
    qint32 cellId();
    qint32 locationAreaCode();

    qint32 currentMobileCountryCode(); // Mobile Country Code
    qint32 currentMobileNetworkCode(); // Mobile Network Code

    qint32 homeMobileCountryCode();
    qint32 homeMobileNetworkCode();

    bool isLocationEnabled() const;
    bool isWLANAccessible() const;

    QString operatorName();

private:
    QSystemNetworkInfoPrivate *d;
};

////////
class QSystemDisplayInfo : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfo(QObject *parent = 0);
    ~QSystemDisplayInfo();

    qint32 displayBrightness();
    qint32 colorDepth(qint32 screen);
    bool isScreenLockOn();

private:
    QSystemDisplayInfoPrivate *d;
};


////////
class QSystemMemoryInfo : public QObject
{
    Q_OBJECT

public:

    QSystemMemoryInfo(QObject *parent = 0);
    ~QSystemMemoryInfo();

    enum VolumeType {
        NoVolume = 0,
        Internal,
        Removable
	};

    bool hasRamMemoryLevel();
    quint64 freeMemoryLevel() const;
    qlonglong totalDiskSpace(const QString &driveVolume);
    qlonglong availableDiskSpace(const QString &driveVolume);
    QStringList listOfVolumes();
    QSystemMemoryInfo::VolumeType getVolumeType(const QString &driveVolume); //returns enum

Q_SIGNALS:
    void memoryCritical(qint32);
    void diskSpaceCritical(QString &driveVolume, qint32);

private:
    QSystemMemoryInfoPrivate *d;
};

////////
class QSystemDeviceInfo : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfo(QObject *parent = 0);
    virtual ~QSystemDeviceInfo();

    enum BatteryLevel {
        NoBatteryLevel = 0,
        BatteryCritical = 1,
        BatteryVeryLow,
        BatteryLow,
        BatteryNormal
    };

//    Q_DECLARE_FLAGS(BatteryLevels, BatteryLevel)

    enum PowerState {
        UnknownPower = -1,
        BatteryPower = 1,
        WallPower
    };
//    Q_DECLARE_FLAGS(PowerStates, PowerState)


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

    QString imei() const; //International Mobile Equipment Identity
    QString imsi() const; //International Mobile Subscriber Identity
    QString manufacturer() const;
    QString model() const; //external
    QString productName() const; //internal name

// ????
    QSystemDeviceInfo::BatteryLevel batteryLevel() const;
    bool isBatteryCharging();

    enum Profile {
        UnknownProfile = -1,
        SilentProfile = 0,
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

Q_SIGNALS:

//    void profileChanged(QSystemDeviceInfo::Profile);
    void batteryLevelChanged(QSystemDeviceInfo::BatteryLevel);
    void batteryLevelCritical(qint32);
    void powerStateChanged(QSystemDeviceInfo::PowerState);

private:
    QSystemDeviceInfoPrivate *d;
};


////////
class QSystemScreenSaverPrivate;
class QSystemScreenSaver : public QObject
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

