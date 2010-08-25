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

#include "qsysteminfo.h"
#include "qsysteminfo_simulator_p.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QSettings>
#include <QSysInfo>
#include <QNetworkInterface>
#include <QList>
#include <QSettings>
#include <QDir>
#include <QNetworkInterface>
#include <QString>
#include <QMetaEnum>
#include <QtNetwork/QLocalSocket>

#include <locale.h>

#ifndef QT_BUILD_SYSINFO_LIB
QTM_BEGIN_NAMESPACE
#endif

#ifdef QT_BUILD_SYSINFO_LIB
#include <mobilityconnection_p.h>
#include <private/qsimulatordata_p.h>

QTM_BEGIN_NAMESPACE

// Simulator connection handler
using namespace QtSimulatorPrivate;

namespace Simulator
{
    class SystemInfoConnection : public QObject
    {
        Q_OBJECT
    public:
        SystemInfoConnection(MobilityConnection *mobilityCon);
        virtual ~SystemInfoConnection() {}

        void getInitialData();

    private slots:
        void setSystemInfoData(const QtMobility::QSystemInfoData &);
        void setSystemDeviceInfoData(const QtMobility::QSystemDeviceInfoData &);
        void setSystemStorageInfoData(const QtMobility::QSystemStorageInfoData &);
        void setSystemNetworkInfoData(const QtMobility::QSystemNetworkInfoData &);
        void setSystemDisplayInfoData(const QtMobility::QSystemDisplayInfoData &);
        void initialSystemInfoDataSent();

    private:
        MobilityConnection *mConnection;
        bool mInitialDataReceived;
    };

    #include "qsysteminfo_simulator.moc"

    SystemInfoConnection::SystemInfoConnection(MobilityConnection *mobilityCon)
        : QObject(mobilityCon)
        , mConnection(mobilityCon)
        , mInitialDataReceived(false)
    {
        qt_registerSystemInfoTypes();
        mobilityCon->addMessageHandler(this);
    }


    void SystemInfoConnection::getInitialData()
    {
        RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsSystemInfo");

        // blockingly handle all remote calls until the simulator signals the initial data
        // sending is done
        QByteArray readBuffer;
        QLocalSocket *readSocket = mConnection->receiveSocket();
        while (!mInitialDataReceived) {
            readSocket->waitForReadyRead(100);
            readBuffer += readSocket->readAll();
            forever {
                IncomingRemoteMetacall rpc;
                if (rpc.read(&readBuffer)) {
                    if (rpc.call(readSocket, this))
                        continue;
                    qWarning("Ignoring a call to %s,", rpc.signature().data());
                }
                break;
            }
        }
    }

    void SystemInfoConnection::initialSystemInfoDataSent()
    {
        mInitialDataReceived = true;
    }

    void SystemInfoConnection::setSystemInfoData(const QSystemInfoData &data)
    {
        QSystemInfoPrivate *s = getSystemInfoPrivate();
        s->setAvailableLanguages(data.availableLanguages);
        s->setCurrentCountryCode(data.currentCountryCode);
        s->setCurrentLanguage(data.currentLanguage);

        for (int i = 0; i < data.versions.size(); ++i)
            s->setVersion(static_cast<QSystemInfo::Version>(i), data.versions[i]);

        for (int i = 0; i < data.features.size(); ++i)
            s->setFeature(static_cast<QSystemInfo::Feature>(i), data.features[i]);
    }

    void SystemInfoConnection::setSystemDeviceInfoData(const QSystemDeviceInfoData &data)
    {
        QSystemDeviceInfoPrivate *s = getSystemDeviceInfoPrivate();
        s->setImei(data.imei);
        s->setImsi(data.imsi);
        s->setManufacturer(data.manufacturer);
        s->setModel(data.model);
        s->setProductName(data.productName);

        s->setInputMethodType(data.inputMethodType);
        s->setSimStatus(data.simStatus);
        s->setCurrentProfile(data.currentProfile);
        s->setCurrentPowerState(data.currentPowerState);

        s->setBatteryLevel(data.batteryLevel);
        s->setDeviceLocked(data.deviceLocked);
    }

    void SystemInfoConnection::setSystemStorageInfoData(const QSystemStorageInfoData &data)
    {
        QSystemStorageInfoPrivate *s = getSystemStorageInfoPrivate();
        QStringList curDrives = QSystemStorageInfo::logicalDrives();
        foreach (const QString &name, curDrives)
            s->removeDrive(name);
        foreach (const QString &name, data.drives.keys()) {
            s->addDrive(name, data.drives[name].type, data.drives[name].totalSpace, data.drives[name].availableSpace);
        }
    }

    void SystemInfoConnection::setSystemNetworkInfoData(const QSystemNetworkInfoData &data)
    {
        QSystemNetworkInfoPrivate *s = getSystemNetworkInfoPrivate();
        s->setCellId(data.cellId);
        s->setLocationAreaCode(data.locationAreaCode);
        s->setCurrentMobileCountryCode(data.currentMobileCountryCode);
        s->setCurrentMobileNetworkCode(data.currentMobileNetworkCode);
        s->setHomeMobileCountryCode(data.homeMobileCountryCode);
        s->setHomeMobileNetworkCode(data.homeMobileNetworkCode);
        s->setCurrentMode(data.currentMode);

        for (int i = 0; i < data.networkInfo.size(); ++i) {
            QSystemNetworkInfo::NetworkMode m = static_cast<QSystemNetworkInfo::NetworkMode>(i);
            const QSystemNetworkInfoData::NetworkInfo &mdata = data.networkInfo[i];
            s->setNetworkName(m, mdata.name);
            s->setNetworkMacAddress(m, mdata.macAddress);
            s->setNetworkSignalStrength(m, mdata.signalStrength);
            s->setNetworkStatus(m, mdata.status);
        }
    }

    void SystemInfoConnection::setSystemDisplayInfoData(const QSystemDisplayInfoData &data)
    {
        QSystemDisplayInfoPrivate *s = getSystemDisplayInfoPrivate();
        s->setColorDepth(data.colorDepth);
        s->setDisplayBrightness(data.displayBrightness);
    }
} // namespace Simulator
#endif

static void ensureSimulatorConnection()
{
#ifdef QT_BUILD_SYSINFO_LIB
    using namespace Simulator;

    static bool connected = false;
    if (connected)
        return;

    connected = true;
    MobilityConnection *con = MobilityConnection::instance();
    SystemInfoConnection *sysinfoCon = new SystemInfoConnection(con);
    sysinfoCon->getInitialData();
#endif
}

QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
    : QObject(parent)
{
    ensureSimulatorConnection();

    QMetaEnum featureMeta = QSystemInfo::staticMetaObject.enumerator(QSystemInfo::staticMetaObject.indexOfEnumerator("Feature"));
    data.features.fill(false, featureMeta.keyCount());
    QMetaEnum versionMeta = QSystemInfo::staticMetaObject.enumerator(QSystemInfo::staticMetaObject.indexOfEnumerator("Version"));
    data.versions.fill("unknown", versionMeta.keyCount() + 1);
}

void QSystemInfoPrivate::setInitialData()
{
    setCurrentLanguage("en");
    setCurrentCountryCode("EN");
    addAvailableLanguage("en");
    addAvailableLanguage("de");
    setFeature(QSystemInfo::LocationFeature, true);
    setFeature(QSystemInfo::UsbFeature, true);
    setVersion(QSystemInfo::QtCore, "4.6 probably");
    setVersion(QSystemInfo::Firmware, "1.9-alpha-rc7");
}

void QSystemInfoPrivate::setCurrentLanguage(const QString &v)
{
    QString language = v.toLower();
    if (data.currentLanguage.toLower() != language) {
        data.currentLanguage = language;
        emit currentLanguageChanged(language);
    }
}

void QSystemInfoPrivate::setCurrentCountryCode(const QString &v)
{
    QString code = v.toUpper();
    if (data.currentCountryCode.toUpper() != code) {
        data.currentCountryCode = code;
    }
}

void QSystemInfoPrivate::setAvailableLanguages(const QStringList &v)
{
    if (data.availableLanguages != v) {
        data.availableLanguages.clear();
        foreach (QString language, v)
            data.availableLanguages.append(language.toLower());
    }
}

void QSystemInfoPrivate::addAvailableLanguage(const QString &v)
{
    QString language = v.toLower();
    if (!data.availableLanguages.contains(language)) {
        data.availableLanguages.append(language);
    }
}

bool QSystemInfoPrivate::removeAvailableLanguage(const QString &v)
{
    QString language = v.toLower();
    int count = data.availableLanguages.removeAll(language);
    if (count == 0)
        return false;

    return true;
}

void QSystemInfoPrivate::removeAllAvailableLanguages()
{
    if (data.availableLanguages.count() > 0) {
        data.availableLanguages.clear();
    }
}

void QSystemInfoPrivate::setFeature(QSystemInfo::Feature f, bool enabled)
{
    if (data.features[f] == enabled)
        return;

    data.features[f] = enabled;
}

void QSystemInfoPrivate::setVersion(QSystemInfo::Version v, const QString &to)
{
    if (data.versions[v] != to) {
        data.versions[v] = to;
    }
}


//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
    : QObject(parent)
{
    ensureSimulatorConnection();

    QSystemNetworkInfoData::NetworkInfo init;
    init.name = "unknown";
    init.macAddress = "ff:ff:ff:ff:ff:ff";
    init.signalStrength = 0;
    init.status = QSystemNetworkInfo::UndefinedStatus;
    QMetaEnum modeMeta = QSystemNetworkInfo::staticMetaObject.enumerator(QSystemNetworkInfo::staticMetaObject.indexOfEnumerator("NetworkMode"));
    data.networkInfo.fill(init, modeMeta.keyCount());
}

void QSystemNetworkInfoPrivate::setInitialData()
{
    setCellId(12345);
    setLocationAreaCode(54321);
    setCurrentMobileCountryCode("242");
    setCurrentMobileNetworkCode("123456789");
    setHomeMobileCountryCode("+47");
    setHomeMobileNetworkCode("987654321");
    setCurrentMode(QSystemNetworkInfo::EthernetMode);
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode m) const
{
    return data.networkInfo[static_cast<int>(m)].name;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode m) const
{
    return data.networkInfo[static_cast<int>(m)].macAddress;
}

qint32 QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode m) const
{
    return data.networkInfo[static_cast<int>(m)].signalStrength;
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode m) const
{
    return data.networkInfo[static_cast<int>(m)].status;
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode m) const
{
    return data.networkInfo[static_cast<int>(m)].interface;
}

void QSystemNetworkInfoPrivate::setCellId(int id)
{
    if (data.cellId != id) {
        data.cellId = id;
    }
}

void QSystemNetworkInfoPrivate::setLocationAreaCode(int code)
{
    if (data.locationAreaCode != code) {
        data.locationAreaCode = code;
    }
}

void QSystemNetworkInfoPrivate::setCurrentMobileCountryCode(const QString &code)
{
    if (data.currentMobileCountryCode != code) {
        data.currentMobileCountryCode = code;
        emit currentMobileCountryCodeChanged(code);
    }
}

void QSystemNetworkInfoPrivate::setCurrentMobileNetworkCode(const QString &code)
{
    if (data.currentMobileNetworkCode != code) {
        data.currentMobileNetworkCode = code;
        emit currentMobileNetworkCodeChanged(code);
    }
}

void QSystemNetworkInfoPrivate::setHomeMobileCountryCode(const QString &code)
{
    if (data.homeMobileCountryCode != code) {
        data.homeMobileCountryCode = code;
    }
}

void QSystemNetworkInfoPrivate::setHomeMobileNetworkCode(const QString &code)
{
    if (data.homeMobileNetworkCode != code) {
        data.homeMobileNetworkCode = code;
    }
}

void QSystemNetworkInfoPrivate::setCurrentMode(QSystemNetworkInfo::NetworkMode m)
{
    if (data.currentMode != m) {
        data.currentMode = m;
    }
}

void QSystemNetworkInfoPrivate::setNetworkName(QSystemNetworkInfo::NetworkMode m, const QString &name)
{
    QSystemNetworkInfoData::NetworkInfo &info = data.networkInfo[static_cast<int>(m)];
    if (info.name != name) {
        info.name = name;
        emit networkNameChanged(m, name);
    }
}

void QSystemNetworkInfoPrivate::setNetworkMacAddress(QSystemNetworkInfo::NetworkMode m, const QString &mac)
{
    QSystemNetworkInfoData::NetworkInfo &info = data.networkInfo[static_cast<int>(m)];
    if (info.macAddress != mac) {
        info.macAddress = mac;
    }
}

void QSystemNetworkInfoPrivate::setNetworkSignalStrength(QSystemNetworkInfo::NetworkMode m, qint32 strength)
{
    QSystemNetworkInfoData::NetworkInfo &info = data.networkInfo[static_cast<int>(m)];
    if (info.signalStrength != strength) {
        info.signalStrength = strength;
        emit networkSignalStrengthChanged(m, strength);
    }
}

void QSystemNetworkInfoPrivate::setNetworkStatus(QSystemNetworkInfo::NetworkMode m, QSystemNetworkInfo::NetworkStatus status)
{
    QSystemNetworkInfoData::NetworkInfo &info = data.networkInfo[static_cast<int>(m)];
    if (info.status != status) {
        info.status = status;
        emit networkStatusChanged(m, status);
    }
}


//////// QSystemDisplayInfo
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
    : QObject(parent)
{
    ensureSimulatorConnection();
}

void QSystemDisplayInfoPrivate::setInitialData()
{
    setDisplayBrightness(100);
    setColorDepth(32);
}

void QSystemDisplayInfoPrivate::setColorDepth(int depth)
{
    if (data.colorDepth != depth) {
        data.colorDepth = depth;
    }
}

void QSystemDisplayInfoPrivate::setDisplayBrightness(int brightness)
{
    if (data.displayBrightness != brightness) {
        data.displayBrightness = brightness;
    }
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
    : QObject(parent)
{
    ensureSimulatorConnection();
}

void QSystemDeviceInfoPrivate::setInitialData()
{
    setCurrentProfile(QSystemDeviceInfo::NormalProfile);
    setCurrentPowerState(QSystemDeviceInfo::WallPower);
    setSimStatus(QSystemDeviceInfo::SimNotAvailable);
    setInputMethodType(
           static_cast<QSystemDeviceInfo::InputMethod>(static_cast<int>(
                    QSystemDeviceInfo::Keyboard | QSystemDeviceInfo::Mouse)));
    setImei("12-345678-901234-5");
    setImsi("12345679012345");
    setManufacturer("simulator manufacturer");
    setModel("simulator model");
    setProductName("simulator product name");
    setBatteryLevel(84);
    setDeviceLocked(false);
}

QSystemDeviceInfo::BatteryStatus QSystemDeviceInfoPrivate::batteryStatus() const
{
    int level = batteryLevel();
     if(level < 4) {
         return QSystemDeviceInfo::BatteryCritical;
     }   else if(level < 11) {
         return QSystemDeviceInfo::BatteryVeryLow;
     }  else if(level < 41) {
         return QSystemDeviceInfo::BatteryLow;
     }   else if(level > 40) {
         return QSystemDeviceInfo::BatteryNormal;
     }

     return QSystemDeviceInfo::NoBatteryLevel;
}

void QSystemDeviceInfoPrivate::setCurrentProfile(QSystemDeviceInfo::Profile v)
{
    if (data.currentProfile != v) {
        data.currentProfile = v;
        emit currentProfileChanged(v);
    }
}

void QSystemDeviceInfoPrivate::setCurrentPowerState(QSystemDeviceInfo::PowerState v)
{
    if (data.currentPowerState != v) {
        data.currentPowerState = v;
        emit powerStateChanged(v);
    }
}

void QSystemDeviceInfoPrivate::setSimStatus(QSystemDeviceInfo::SimStatus v)
{
    if (data.simStatus != v) {
        data.simStatus = v;
    }
}

void QSystemDeviceInfoPrivate::setInputMethodType(QSystemDeviceInfo::InputMethodFlags v)
{
    if (data.inputMethodType != v) {
        data.inputMethodType= v;
    }
}

void QSystemDeviceInfoPrivate::setImei(const QString &v)
{
    if (data.imei != v) {
        data.imei = v;
    }
}

void QSystemDeviceInfoPrivate::setImsi(const QString &v)
{
    if (data.imsi != v) {
        data.imsi = v;
    }
}

void QSystemDeviceInfoPrivate::setManufacturer(const QString &v)
{
    if (data.manufacturer != v) {
        data.manufacturer = v;
    }
}

void QSystemDeviceInfoPrivate::setModel(const QString &v)
{
    if (data.model != v) {
        data.model = v;
    }
}

void QSystemDeviceInfoPrivate::setProductName(const QString &v)
{
    if (data.productName != v) {
        data.productName = v;
    }
}

void QSystemDeviceInfoPrivate::setBatteryLevel(int v)
{
    if (data.batteryLevel != v) {
        QSystemDeviceInfo::BatteryStatus oldstatus = batteryStatus();
        data.batteryLevel = v;
        emit batteryLevelChanged(v);

        QSystemDeviceInfo::BatteryStatus newstatus = batteryStatus();
        if (newstatus != oldstatus)
            emit batteryStatusChanged(newstatus);
    }
}

void QSystemDeviceInfoPrivate::setDeviceLocked(bool v)
{
    if (data.deviceLocked != v) {
        data.deviceLocked = v;
    }
}

//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
    : QObject(parent)
{
    ensureSimulatorConnection();
}

void QSystemStorageInfoPrivate::setInitialData()
{
    addDrive("internal drive", QSystemStorageInfo::InternalDrive, 256*1024*qint64(1024), 32*1024*qint64(1024));
    addDrive("removable drive", QSystemStorageInfo::RemovableDrive, 4*1024*1024*qint64(1024), 3*1024*1024*qint64(1024));
}

QStringList QSystemStorageInfoPrivate::logicalDrives() const
{
    return data.drives.keys();
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &name) const
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::const_iterator it = data.drives.find(name);
    if (it != data.drives.end())
        return it.value().type;
    return QSystemStorageInfo::NoDrive;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &name) const
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::const_iterator it = data.drives.find(name);
    if (it != data.drives.end())
        return it.value().totalSpace;
    return -1;
}

qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &name) const
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::const_iterator it = data.drives.find(name);
    if (it != data.drives.end())
        return it.value().availableSpace;
    return -1;
}

bool QSystemStorageInfoPrivate::addDrive(const QString &name)
{
    return addDrive(name, QSystemStorageInfo::NoDrive, 0, 0);
}

bool QSystemStorageInfoPrivate::addDrive(const QString &name, QSystemStorageInfo::DriveType type, qint64 totalSpace, qint64 availableSpace)
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::const_iterator it = data.drives.find(name);
    if (it != data.drives.end())
        return false;

    QSystemStorageInfoData::DriveInfo d;
    d.type = static_cast<QSystemStorageInfo::DriveType>(type);
    d.totalSpace = totalSpace;
    d.availableSpace = availableSpace;
    data.drives[name] = d;

    emit logicalDrivesChanged(true);
    return true;
}

bool QSystemStorageInfoPrivate::removeDrive(const QString &name)
{
    if (data.drives.remove(name) > 0) {
        emit logicalDrivesChanged(false);
        return true;
    }
    return false;
}

bool QSystemStorageInfoPrivate::setName(const QString &oldname, const QString &newname)
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::const_iterator oldit = data.drives.find(oldname);
    QHash<QString, QSystemStorageInfoData::DriveInfo>::const_iterator newit = data.drives.find(newname);
    if (oldit == data.drives.end() || newit != data.drives.end())
        return false;

    QSystemStorageInfoData::DriveInfo copy(oldit.value());
    data.drives.remove(oldname);
    data.drives[newname] = copy;

    emit logicalDrivesChanged(false);
    return true;
}

bool QSystemStorageInfoPrivate::setType(const QString &name, QSystemStorageInfo::DriveType type)
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::iterator it = data.drives.find(name);
    if (it == data.drives.end())
        return false;

    it.value().type = static_cast<QSystemStorageInfo::DriveType>(type);
    emit logicalDrivesChanged(false);
    return true;
}

bool QSystemStorageInfoPrivate::setTotalSpace(const QString &name, qint64 space)
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::iterator it = data.drives.find(name);
    if (it == data.drives.end())
        return false;

    it.value().totalSpace = space;
    emit logicalDrivesChanged(false);
    return true;
}

bool QSystemStorageInfoPrivate::setAvailableSpace(const QString &name, qint64 space)
{
    QHash<QString, QSystemStorageInfoData::DriveInfo>::iterator it = data.drives.find(name);
    if (it == data.drives.end())
        return false;

    it.value().availableSpace = space;
    emit logicalDrivesChanged(false);
    return true;
}

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
        , didInhibit(false)
{
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return true;
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    return true;
}

bool QSystemScreenSaverPrivate::isScreenLockOn()
{
    return true;
}

#include "moc_qsysteminfo_simulator_p.cpp"

QTM_END_NAMESPACE
