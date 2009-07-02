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
#ifndef QSYSTEMSINFO_H
#define QSYSTEMSINFO_H

#include <QObject>
#include <QSize>
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;

class QSystemsInfoPrivate;
class QSystemsNetworkInfoPrivate;
class QSystemsMemoryInfoPrivate;
class QSystemsDeviceInfoPrivate;
class QSystemsDisplayInfoPrivate;

class QSystemsInfo : public QObject
{
    Q_OBJECT

public:

    QSystemsInfo(QObject *parent = 0);

//    QSystemsInfo::Error error() const;

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

    QString getVersion(QSystemsInfo::Version type, const QString &parameter = QString());

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
	
    bool hasFeatureSupported(QSystemsInfo::Feature feature);
    QString getDetailOfFeature(QSystemsInfo::Feature feature);

private:
    QSystemsInfoPrivate *d;
};

class QSystemsNetworkInfo : public QObject
{
    Q_OBJECT

public:

    QSystemsNetworkInfo(QObject *parent = 0);

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

	QSystemsNetworkInfo::CellNetworkStatus getCellNetworkStatus();

    enum NetworkMode {
        UnknownMode = 0x00000000,
        GsmMode = 0x00000001,
        CdmaMode = 0x00000002,
        WcdmaMode = 0x00000004,
        WlanMode = 0x00000008,
        EthMode = 0x00000010
    };

    qint32 networkSignalStrength();
    qint32 cellId();
    qint32 locationAreaCode();

    qint32 currentMCC(); // Mobile Country Code
    qint32 currentMNC(); // Mobile Network Code

    qint32 homeMCC();
    qint32 homeMNC();

    bool isLocationEnabled() const;
    bool isWLANAccessible() const;

    QString operatorName();

Q_SIGNALS:

    void networkStatusChanged(QSystemsNetworkInfo::NetworkMode netmode, QSystemsNetworkInfo::CellNetworkStatus netStatus);

private:
    QSystemsNetworkInfoPrivate *d;
};

class QSystemsDisplayInfo : public QObject
{
    Q_OBJECT

public:

    QSystemsDisplayInfo(QObject *parent = 0);
    qint32 displayBrightness();
    qint32 colorDepth(qint32 screen);
    void setScreenSaverEnabled(bool);
    void setScreenBlankingEnabled(bool);
    bool isScreenLockOn();

private:
    QSystemsDisplayInfoPrivate *d;
};


class QSystemsMemoryInfo : public QObject
{
    Q_OBJECT

public:

    QSystemsMemoryInfo(QObject *parent = 0);

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
    QSystemsMemoryInfo::VolumeType getVolumeType(const QString &driveVolume); //returns enum

Q_SIGNALS:
    void memoryCritical(qint32);
    void diskSpaceCritical(QString &driveVolume, qint32);

private:
    QSystemsMemoryInfoPrivate *d;
};

class QSystemsDeviceInfo : public QObject
{
    Q_OBJECT

public:

    QSystemsDeviceInfo(QObject *parent = 0);

    enum BatteryLevel {
        NoBatteryLevel = 0,
        BatteryCritical = 1,
        BatteryVeryLow,
        BatteryLow,
        BatteryNormal
    };

    enum PowerState {
        UnknownPower = -1,
        BatteryPower = 1,
        WallPower
    };


    enum InputMethods {
        KeysOnly,
        TouchOnly,
        KeysAndTouch
    };

    QSystemsDeviceInfo::InputMethods getInputMethodType();

    QString imei() const;
    QString imsi() const;
    QString manufacturer() const;
    QString model() const;

    QSystemsDeviceInfo::BatteryLevel batteryLevel() const;

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

    QSystemsDeviceInfo::SimStatus getSimStatus();
    bool isDeviceLocked();

Q_SIGNALS:

    void profileChanged(QSystemsDeviceInfo::Profile);
    void batteryLevelChanged(QSystemsDeviceInfo::BatteryLevel);
    void batteryLevelCritical(qint32);
    void powerStateChanged(QSystemsDeviceInfo::PowerState);

private:
    QSystemsDeviceInfoPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_H*/

// End of file

