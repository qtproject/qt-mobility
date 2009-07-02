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
#ifndef QSYSTEMSINFO_P_H
#define QSYSTEMSINFO_P_H

#include <QObject>
#include <QSize>

#include "qsystemsinfo.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;

class QSystemsInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemsInfoPrivate(QObject *parent = 0);

// general
    QString currentLanguage() const; // 2 letter ISO 639-1
    QStringList availableLanguages() const;	 // 2 letter ISO 639-1

    QString getVersion(QSystemsInfo::Version,  const QString &parameter) const;

    QString countryCode() const; //2 letter ISO 3166-1
//features
    bool hasFeatureSupported(QSystemsInfo::Feature feature);
    QString getDetailOfFeature(QSystemsInfo::Feature feature);
};

class QSystemsNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemsNetworkInfoPrivate(QObject *parent = 0);

    QSystemsNetworkInfo::CellNetworkStatus getCellNetworkStatus();
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

    void networkStatusChanged(QSystemsNetworkInfo::NetworkMode, QSystemsNetworkInfo::CellNetworkStatus);
};

class QSystemsDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemsDisplayInfoPrivate(QObject *parent = 0);



// display
    qint32 displayBrightness();
    qint32 colorDepth(qint32 screen);
    void setScreenSaverEnabled(bool b);
    void setScreenBlankingEnabled(bool b);
    bool isScreenLockOn();
};

class QSystemsMemoryInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemsMemoryInfoPrivate(QObject *parent = 0);

// memory
    bool hasRamMemoryLevel();
    qint64 freeMemoryLevel() const;
    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList listOfVolumes();
    QSystemsMemoryInfo::VolumeType getVolumeType(const QString &driveVolume); //returns enum

Q_SIGNALS:
    void memoryCritical(qint32);
    void diskSpaceCritical(QString &driveVolume, qint32);
 private:
#if defined(Q_OS_LINUX) ||  defined(Q_OS_WIN32)
    virtual bool isCriticalMemory() const;
    virtual bool isBatteryCharging();
    virtual bool isDiskSpaceCritical(const QString &driveVolume);
#endif
};

class QSystemsDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemsDeviceInfoPrivate(QObject *parent = 0);

// device

    QString imei() const;
    QString imsi() const;
    QString manufacturer() const;
    QString model() const;
    
    QSystemsDeviceInfo::InputMethods getInputMethodType();

    QSystemsDeviceInfo::BatteryLevel batteryLevel() const;

    QSystemsDeviceInfo::SimStatus getSimStatus();
    bool isDeviceLocked();
Q_SIGNALS:

    void profileChanged(QSystemsDeviceInfo::Profile);
    void batteryLevelChanged(QSystemsDeviceInfo::BatteryLevel);
    void batteryLevelCritical(qint32);
    void powerStateChanged(QSystemsDeviceInfo::PowerState);
};
//    QSystemsInfo::Error error() const;


QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_P_H*/

// End of file

