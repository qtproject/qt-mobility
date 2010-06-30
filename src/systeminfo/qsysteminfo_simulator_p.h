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
#include <QStringList>

#include <qmobilityglobal.h>
#include "qsysteminfo.h"
#include "qsysteminfodata_simulator_p.h"


QT_BEGIN_HEADER

class QTimer;

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;

class QSystemInfoPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemInfoPrivate(QObject *parent = 0);

    QString currentLanguage() const { return data.currentLanguage; }
    QString currentCountryCode() const { return data.currentCountryCode; }
    QStringList availableLanguages() const { return data.availableLanguages; }
    bool hasFeature(QSystemInfo::Feature f) const { return data.features[f]; }
    bool hasFeatureSupported(QSystemInfo::Feature f) const { return data.features[f]; }
    QString version(QSystemInfo::Version v, const QString &param = "") const { Q_UNUSED(param); return data.versions[v]; }

    void setCurrentLanguage(const QString &v);
    void setCurrentCountryCode(const QString &v);
    void setAvailableLanguages(const QStringList &v);
    void addAvailableLanguage(const QString &v);
    bool removeAvailableLanguage(const QString &v);
    void removeAllAvailableLanguages();
    void setFeature(QSystemInfo::Feature f, bool enabled);
    void setVersion(QSystemInfo::Version v, const QString &to);

    void setInitialData();

signals:
    void currentLanguageChanged(const QString &) const;

private:
    QSystemInfoData data;
};
QSystemInfoPrivate *getSystemInfoPrivate();

class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemNetworkInfoPrivate(QObject *parent = 0);

    int cellId() const { return data.cellId; }
    int locationAreaCode() const { return data.locationAreaCode; }
    QString currentMobileCountryCode() const { return data.currentMobileCountryCode; }
    QString currentMobileNetworkCode() const { return data.currentMobileNetworkCode; }
    QString homeMobileCountryCode() const { return data.homeMobileCountryCode; }
    QString homeMobileNetworkCode() const { return data.homeMobileNetworkCode; }
    QSystemNetworkInfo::NetworkMode currentMode() const { return data.currentMode; }

    QString networkName(QSystemNetworkInfo::NetworkMode m) const;
    QString macAddress(QSystemNetworkInfo::NetworkMode m) const;
    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode m) const;
    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode m) const;
    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode) const;

    void setCellId(int id);
    void setLocationAreaCode(int code);
    void setCurrentMobileCountryCode(const QString &code);
    void setCurrentMobileNetworkCode(const QString &code);
    void setHomeMobileCountryCode(const QString &code);
    void setHomeMobileNetworkCode(const QString &code);
    void setCurrentMode(QSystemNetworkInfo::NetworkMode m);

    void setNetworkName(QSystemNetworkInfo::NetworkMode m, const QString &name);
    void setNetworkMacAddress(QSystemNetworkInfo::NetworkMode m, const QString &mac);
    void setNetworkSignalStrength(QSystemNetworkInfo::NetworkMode m, qint32 strength);
    void setNetworkStatus(QSystemNetworkInfo::NetworkMode m, QSystemNetworkInfo::NetworkStatus status);

    void setInitialData();

signals:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus) const;
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int) const;
    void currentMobileCountryCodeChanged(const QString &) const;
    void currentMobileNetworkCodeChanged(const QString &) const;
    void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &) const;
    void networkModeChanged(QSystemNetworkInfo::NetworkMode) const;

private:
    QSystemNetworkInfoData data;
};
QSystemNetworkInfoPrivate *getSystemNetworkInfoPrivate();

class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemDisplayInfoPrivate(QObject *parent = 0);

    int displayBrightness(int screen = 0) const { Q_UNUSED(screen); return data.displayBrightness; }
    int colorDepth(int screen = 0) const { Q_UNUSED(screen); return data.colorDepth; }

    QSystemDisplayInfo::DisplayOrientation getOrientation(int screen) const { Q_ASSERT(false); }
    float contrast(int screen) const { Q_ASSERT(false); }
    int getDPIWidth(int screen) const { Q_ASSERT(false); }
    int getDPIHeight(int screen) const { Q_ASSERT(false); }
    int physicalHeight(int screen) const { Q_ASSERT(false); }
    int physicalWidth(int screen) const { Q_ASSERT(false); }

    void setDisplayBrightness(int brightness);
    void setColorDepth(int depth);

    void setInitialData();

private:
    QSystemDisplayInfoData data;
};
QSystemDisplayInfoPrivate *getSystemDisplayInfoPrivate();

class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemDeviceInfoPrivate(QObject *parent = 0);

    QSystemDeviceInfo::Profile currentProfile() const { return data.currentProfile; }
    QSystemDeviceInfo::PowerState currentPowerState() const { return data.currentPowerState; }
    QSystemDeviceInfo::SimStatus simStatus() const { return data.simStatus; }
    QSystemDeviceInfo::InputMethodFlags inputMethodType() const { return data.inputMethodType; }

    QString imei() const { return data.imei; }
    QString imsi() const { return data.imsi; }
    QString manufacturer() const { return data.manufacturer; }
    QString model() const { return data.model; }
    QString productName() const { return data.productName; }

    int batteryLevel() const { return data.batteryLevel; }
    bool isDeviceLocked() const { return data.deviceLocked; }

    bool currentBluetoothPowerState() const { Q_ASSERT(false); }

    QSystemDeviceInfo::BatteryStatus batteryStatus() const;

    void setCurrentProfile(QSystemDeviceInfo::Profile v);
    void setCurrentPowerState(QSystemDeviceInfo::PowerState v);
    void setSimStatus(QSystemDeviceInfo::SimStatus v);
    void setInputMethodType(QSystemDeviceInfo::InputMethodFlags v);

    void setImei(const QString &v);
    void setImsi(const QString &v);
    void setManufacturer(const QString &v);
    void setModel(const QString &v);
    void setProductName(const QString &v);

    void setBatteryLevel(int v);
    void setDeviceLocked(bool v);

    void setInitialData();

signals:
    void batteryLevelChanged(int) const;
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus) const;
    void powerStateChanged(QSystemDeviceInfo::PowerState) const;
    void currentProfileChanged(QSystemDeviceInfo::Profile) const;
    void bluetoothStateChanged(bool) const;

private:
    QSystemDeviceInfoData data;
};
QSystemDeviceInfoPrivate *getSystemDeviceInfoPrivate();

class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemStorageInfoPrivate(QObject *parent = 0);

    QStringList logicalDrives() const;
    QSystemStorageInfo::DriveType typeForDrive(const QString &name) const;
    qint64 totalDiskSpace(const QString &name) const;
    qint64 availableDiskSpace(const QString &name) const;

    bool addDrive(const QString &name);
    bool addDrive(const QString &name, QSystemStorageInfo::DriveType type,
                  qint64 totalSpace, qint64 availableSpace);
    bool removeDrive(const QString &name);
    bool setName(const QString &oldname, const QString &newname);
    bool setType(const QString &name, QSystemStorageInfo::DriveType type);
    bool setTotalSpace(const QString &name, qint64 space);
    bool setAvailableSpace(const QString &name, qint64 space);

    void setInitialData();

signals:
    void logicalDrivesChanged(bool added);

private:
    QSystemStorageInfoData data;
};
QSystemStorageInfoPrivate *getSystemStorageInfoPrivate();

class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    explicit QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    bool isScreenLockOn();

private:
    bool didInhibit;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif /*QSYSTEMINFO_SIMULATOR_P_H*/
