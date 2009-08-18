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
#ifndef QSYSTEMINFO_P_H
#define QSYSTEMINFO_P_H

#include <QObject>
#include <QSize>
#include <QHash>

#include "qsysteminfo.h"
#include <qsysinfoglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;

class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();
// general
    QString currentLanguage() const; // 2 letter ISO 639-1
    QStringList availableLanguages() const;	 // 2 letter ISO 639-1

    QString getVersion(QSystemInfo::Version,  const QString &parameter = QString());

    QString currentCountryCode() const; //2 letter ISO 3166-1
//features
    bool hasFeatureSupported(QSystemInfo::Feature feature);
private:
#if !defined(QT_NO_DBUS)
    bool hasHalDeviceFeature(const QString &param);
    bool hasHalUsbFeature(qint32 usbClass);
#endif
    bool hasSysFeature(const QString &featureStr);
};

class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::CellNetworkStatus getCellNetworkStatus();
    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode(); // Mobile Country Code
    QString currentMobileNetworkCode(); // Mobile Network Code

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    bool isWLANAccessible() const;
    QString operatorName();

};

class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();


// display
    int displayBrightness(int screen);
    int colorDepth(int screen);
    bool isScreenLockOn();
};

class QSystemMemoryInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemMemoryInfoPrivate(QObject *parent = 0);
    virtual ~QSystemMemoryInfoPrivate();

    // memory
    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList listOfVolumes();
    QSystemMemoryInfo::VolumeType getVolumeType(const QString &driveVolume); //returns enum

private:
#if defined(Q_OS_LINUX) ||  defined(Q_OS_WIN32)
    QHash<QString, QString> mountEntries;
    void getMountEntries();
#endif
};

class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoPrivate();

// device

    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model();
    static QString productName();

    QSystemDeviceInfo::InputMethods getInputMethodType();

    QSystemDeviceInfo::BatteryLevel batteryLevel() const;

    QSystemDeviceInfo::SimStatus getSimStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile getCurrentProfile();
    bool isBatteryCharging();

    QSystemDeviceInfo::PowerState currentPowerState();


private:

};


class QSystemScreenSaverPrivate : public QSystemScreenSaver
{
    Q_OBJECT

public:

    QSystemScreenSaverPrivate(QObject *parent = 0);

    QSystemScreenSaver::ScreenSaverState screenSaverEnabled();
    QSystemScreenSaver::ScreenSaverState screenBlankingEnabled();
    bool setScreenSaverEnabled(QSystemScreenSaver::ScreenSaverState);
    bool setScreenBlankingEnabled(QSystemScreenSaver::ScreenSaverState);
    
};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_P_H*/

// End of file

