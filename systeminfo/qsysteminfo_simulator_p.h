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
#ifndef QSYSTEMINFO_SIMULATOR_P_H
#define QSYSTEMINFO_SIMULATOR_P_H


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
#include <QSize>
#include <QHash>

#include "qsysteminfo.h"
#include "qsysinfoglobal.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;
class QSystemNetworkInfo;
class QTimer;

class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();
// general
    QString currentLanguage() const;
    QStringList availableLanguages() const;
    QString currentCountryCode() const;

    QString version(QSystemInfo::Version, const QString &parameter = QString());

//features
    bool hasFeatureSupported(QSystemInfo::Feature feature);
Q_SIGNALS:
    void currentLanguageChanged(const QString &);
};

class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode(); // Mobile Country Code
    QString currentMobileNetworkCode(); // Mobile Network Code

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

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
    QSystemMemoryInfo::VolumeType volumeType(const QString &driveVolume);
};

class QSystemMemoryInfoState
{
public:
    friend class QSystemMemoryInfoPrivate;

    QSystemMemoryInfoState();
    static QSystemMemoryInfoState* instance();

    struct VolumeInfo
    {
        QSystemMemoryInfo::VolumeType type;
        qint64 totalSpace;
        qint64 availableSpace;
    };

    VolumeInfo *createVolume(const QString &name);
    VolumeInfo *getVolume(const QString &name);
    void removeVolume(const QString &name);

private:
    QHash<QString, VolumeInfo> volumes;
};


class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:
    friend class QSystemDeviceInfoState;

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    ~QSystemDeviceInfoPrivate();

// device

    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model();
    static QString productName();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    int  batteryLevel() const;
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();

    QSystemDeviceInfo::PowerState currentPowerState();
    void setConnection();

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus);

    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);
};

class QSystemDeviceInfoState
{
public:
    friend class QSystemDeviceInfoPrivate;

    QSystemDeviceInfoState();
    static QSystemDeviceInfoState* instance();

    void setImei(QString);
    void setImsi(QString);
    void setManufacturer(QString);
    void setModel(QString);
    void setProductName(QString);

    void setInputMethodType(QSystemDeviceInfo::InputMethodFlags);
    void setSimStatus(QSystemDeviceInfo::SimStatus);
    void setCurrentProfile(QSystemDeviceInfo::Profile);
    void setCurrentPowerState(QSystemDeviceInfo::PowerState);

    void setBatteryLevel(int);
    void setDeviceLocked(bool);

private:
    QString imei;
    QString imsi;
    QString manufacturer;
    QString model;
    QString productName;

    QSystemDeviceInfo::InputMethodFlags inputMethodType;
    QSystemDeviceInfo::SimStatus simStatus;
    QSystemDeviceInfo::Profile currentProfile;
    QSystemDeviceInfo::PowerState currentPowerState;

    int batteryLevel;
    bool deviceLocked;

    void addObserver(QSystemDeviceInfoPrivate *);
    void removeObserver(QSystemDeviceInfoPrivate *);

    QList<QSystemDeviceInfoPrivate *> observers;
};


class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverEnabled();
    bool screenBlankingEnabled();
    bool setScreenSaverEnabled(bool b);
    bool setScreenBlankingEnabled(bool b);
    bool isScreenLockOn();

private:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;

    uint currentPid;
    bool kdeIsRunning;
    bool gnomeIsRunning;
    void whichWMRunning();

};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_SIMULATOR_P_H*/

// End of file

