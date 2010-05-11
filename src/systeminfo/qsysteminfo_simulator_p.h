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

    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage)
    Q_PROPERTY(QString currentCountryCode READ currentCountryCode WRITE setCurrentCountryCode)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages WRITE setAvailableLanguages)

    QString currentLanguage() const { return data.currentLanguage; }
    QString currentCountryCode() const { return data.currentCountryCode; }
    QStringList availableLanguages() const { return data.availableLanguages; }
    Q_INVOKABLE bool hasFeature(QSystemInfo::Feature f) const { return data.features[f]; }
    Q_INVOKABLE bool hasFeatureSupported(QSystemInfo::Feature f) const { return data.features[f]; }
    Q_INVOKABLE QString version(QSystemInfo::Version v, const QString &param = "") const { Q_UNUSED(param); return data.versions[v]; }

public slots:
    void setCurrentLanguage(const QString &v);
    void setCurrentCountryCode(const QString &v);
    void setAvailableLanguages(const QStringList &v);
    void addAvailableLanguage(const QString &v);
    bool removeAvailableLanguage(const QString &v);
    void removeAllAvailableLanguages();
    void setFeature(QSystemInfo::Feature f, bool enabled);
    void setVersion(QSystemInfo::Version v, const QString &to);

public slots:
    void publish() const;
    void setInitialData();

signals:
    void genericSystemInfoChanged(const QSystemInfoData &) const;

    void currentLanguageChanged(const QString &) const;
    void currentCountryCodeChanged(const QString &) const;
    void availableLanguagesChanged(const QStringList &) const;
    void featureChanged(QSystemInfo::Feature, bool) const;
    void versionChanged(QSystemInfo::Version, const QString &) const;

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

    Q_PROPERTY(int cellId READ cellId WRITE setCellId)
    Q_PROPERTY(int locationAreaCode READ locationAreaCode WRITE setLocationAreaCode)
    Q_PROPERTY(QString currentMobileCountryCode READ currentMobileCountryCode WRITE setCurrentMobileCountryCode)
    Q_PROPERTY(QString currentMobileNetworkCode READ currentMobileNetworkCode WRITE setCurrentMobileNetworkCode)
    Q_PROPERTY(QString homeMobileCountryCode READ homeMobileCountryCode WRITE setHomeMobileCountryCode)
    Q_PROPERTY(QString homeMobileNetworkCode READ homeMobileNetworkCode WRITE setHomeMobileNetworkCode)
    Q_PROPERTY(QSystemNetworkInfo::NetworkMode currentMode READ currentMode WRITE setCurrentMode)

    Q_INVOKABLE QString networkName(QSystemNetworkInfo::NetworkMode m) const;
    Q_INVOKABLE QString macAddress(QSystemNetworkInfo::NetworkMode m) const;
    Q_INVOKABLE qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode m) const;
    Q_INVOKABLE QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode m) const;
    Q_INVOKABLE QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode) const;

public slots:
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

public slots:
    void publish() const;
    void setInitialData();

signals:
    void systemNetworkInfoChanged(const QSystemNetworkInfoData &) const;

    void cellIdChanged(int) const;
    void locationAreaCodeChanged(int) const;
    void currentMobileCountryCodeChanged(const QString &) const;
    void currentMobileNetworkCodeChanged(const QString &) const;
    void homeMobileCountryCodeChanged(const QString &) const;
    void homeMobileNetworkCodeChanged(const QString &) const;
    void currentModeChanged(QSystemNetworkInfo::NetworkMode) const;

    void networkModeChanged(QSystemNetworkInfo::NetworkMode) const;
    void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &) const;
    void networkMacAddressChanged(QSystemNetworkInfo::NetworkMode, const QString &) const;
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int) const;
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus) const;

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

    Q_PROPERTY(int displayBrightness READ displayBrightness WRITE setDisplayBrightness)
    Q_PROPERTY(int colorDepth READ colorDepth WRITE setColorDepth)

public slots:
    void setDisplayBrightness(int brightness);
    void setColorDepth(int depth);

public slots:
    void publish() const;
    void setInitialData();

signals:
    void systemDisplayInfoChanged(const QSystemDisplayInfoData &) const;

    void displayBrightnessChanged(int) const;
    void colorDepthChanged(int) const;

private:
    QSystemDisplayInfoData data;
};
QSystemDisplayInfoPrivate *getSystemDisplayInfoPrivate();

class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemDeviceInfoPrivate(QObject *parent = 0);

    Q_PROPERTY(QSystemDeviceInfo::Profile currentProfile READ currentProfile WRITE setCurrentProfile)
    Q_PROPERTY(QSystemDeviceInfo::PowerState currentPowerState READ currentPowerState WRITE setCurrentPowerState)
    Q_PROPERTY(QSystemDeviceInfo::SimStatus simStatus READ simStatus WRITE setSimStatus)
    Q_PROPERTY(QSystemDeviceInfo::InputMethodFlags inputMethodType READ inputMethodType WRITE setInputMethodType)

    Q_PROPERTY(QString imei READ imei WRITE setImei)
    Q_PROPERTY(QString imsi READ imsi WRITE setImsi)
    Q_PROPERTY(QString manufacturer READ manufacturer WRITE setManufacturer)
    Q_PROPERTY(QString model READ model WRITE setModel)
    Q_PROPERTY(QString productName READ productName WRITE setProductName)

    Q_PROPERTY(int batteryLevel READ batteryLevel WRITE setBatteryLevel)
    Q_PROPERTY(bool deviceLocked READ isDeviceLocked WRITE setDeviceLocked)

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

    Q_INVOKABLE QSystemDeviceInfo::BatteryStatus batteryStatus() const;

public slots:
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

public slots:
    void publish() const;
    void setInitialData();

signals:
    void systemDeviceInfoChanged(const QSystemDeviceInfoData &) const;

    void currentProfileChanged(QSystemDeviceInfo::Profile) const;
    void powerStateChanged(QSystemDeviceInfo::PowerState) const;
    void simStatusChanged(QSystemDeviceInfo::SimStatus) const;
    void inputMethodTypeChanged(QSystemDeviceInfo::InputMethodFlags) const;

    void imeiChanged(const QString &) const;
    void imsiChanged(const QString &) const;
    void manufacturerChanged(const QString &) const;
    void modelChanged(const QString &) const;
    void productNameChanged(const QString &) const;

    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus) const;
    void batteryLevelChanged(int) const;
    void deviceLockChanged(bool) const;

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

    Q_INVOKABLE QStringList logicalDrives() const;
    Q_INVOKABLE QSystemStorageInfo::DriveType typeForDrive(const QString &name) const;
    Q_INVOKABLE qint64 totalDiskSpace(const QString &name) const;
    Q_INVOKABLE qint64 availableDiskSpace(const QString &name) const;

public slots:
    bool addDrive(const QString &name);
    bool addDrive(const QString &name, QSystemStorageInfo::DriveType type,
                  qint64 totalSpace, qint64 availableSpace);
    bool removeDrive(const QString &name);
    bool setName(const QString &oldname, const QString &newname);
    bool setType(const QString &name, QSystemStorageInfo::DriveType type);
    bool setTotalSpace(const QString &name, qint64 space);
    bool setAvailableSpace(const QString &name, qint64 space);

public slots:
    void publish() const;
    void setInitialData();

signals:
    void systemStorageInfoChanged(const QSystemStorageInfoData &) const;

    void driveAdded(const QString &name, QSystemStorageInfo::DriveType type,
                    qint64 totalSpace, qint64 availableSpace) const;
    void driveRemoved(const QString &name) const;
    void nameChanged(const QString &oldname, const QString &newname) const;
    void typeChanged(const QString &name, QSystemStorageInfo::DriveType type) const;
    void totalSpaceChanged(const QString &name, qint64 space) const;
    void availableSpaceChanged(const QString &name, qint64 space) const;

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
