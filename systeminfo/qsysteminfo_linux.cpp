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
#include <qsysteminfo.h>
#include <qsysteminfo_p.h>

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QtGui>
#include <QDesktopWidget>
#include <QDebug>

#include <QLibraryInfo>

#if !defined(QT_NO_DBUS)
#include <qhalservice_p.h>
#include <QtDBus>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusPendingCallWatcher>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#endif

#include <QLocale>
#include <QTimer>
#include <QDir>
#include <QTextStream>

#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <mntent.h>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>

#endif
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
QT_BEGIN_NAMESPACE

static bool halAvailable()
{
#if !defined(QT_NO_DBUS)
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered("org.freedesktop.Hal");
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
#endif
  //  qDebug() << "Hal is not running";
    return false;
}

bool halIsAvailable;
//////// QSystemInfo
QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
    halIsAvailable = halAvailable();
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{

}

// 2 letter ISO 639-1
QString QSystemInfoPrivate::currentLanguage() const
{
    QString lang = QLocale::system().name().left(2);
    if(lang.isEmpty() || lang == "C") {
        lang = "en";
    }
    return lang;
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    QDir transDir(QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    QStringList langList;

    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << "qt_*.qm" ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach(QString localeName, localeList) {
            QString lang = localeName.mid(3,2);
            if(!langList.contains(lang) && !lang.isEmpty() && !lang.contains("help")) {
                langList <<lang;
            }
        }
        if(langList.count() > 0) {
            return langList;
        }
    }
    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    QString errorStr = "Not Available";
    bool useDate = false;
    if(parameter == "versionDate") {
        useDate = true;
    }
    switch(type) {
    case QSystemInfo::Os :
        {
#if !defined(QT_NO_DBUS)
            QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
            QString str;
            if (iface.isValid()) {
                str = iface.getPropertyString("system.kernel.version");
                if(!str.isEmpty()) {
                    return str;
                }
            }
#endif
            QString versionPath = "/proc/version";
            QFile versionFile(versionPath);
            if(!versionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning()<<"File not opened";
            } else {
                QString  strvalue;
                strvalue = versionFile.readAll().trimmed();
                strvalue = strvalue.split(" ").at(2);
                versionFile.close();
                return strvalue;
            }
        }
        break;
    case QSystemInfo::QtCore :
       return  qVersion();
       break;
    case QSystemInfo::WrtCore :
        {
        }
       break;
    case QSystemInfo::Webkit :
        {
        }
       break;
    case QSystemInfo::ServiceFramework :
        {
           /*
           //#include <QServiceInterfaceDescriptor>
             // -lQtServiceFramework
             QServiceInterfaceDescriptor service;
             return QString::number(service.majorVersion) +"."+
             QString::number(service.minorVersion);
             */
        }
       break;
    case QSystemInfo::WrtExtensions :
        {
        }
       break;
    case QSystemInfo::ServiceProvider :
        {
        }
       break;
    case QSystemInfo::NetscapePlugin :
        {
        }
       break;
    case QSystemInfo::WebApp :
        {
        }
       break;
   case QSystemInfo::Firmware :
       {
#if !defined(QT_NO_DBUS)
           QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
           QString str;
           if (iface.isValid()) {
               if(useDate) {
                   str = iface.getPropertyString("system.firmware.release_date");
                   if(!str.isEmpty()) {
                       return str;
                   }
               } else {
                   str = iface.getPropertyString("system.firmware.version");
                   if(str.isEmpty()) {
                       if(!str.isEmpty()) {
                           return str;
                       }
                   }
               }
           }
#endif
       }
       break;
    };
  return errorStr;
}


//2 letter ISO 3166-1
QString QSystemInfoPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}

#if !defined(QT_NO_DBUS)
bool QSystemInfoPrivate::hasHalDeviceFeature(const QString &param)
{
    QHalInterface halIface;
    QStringList halDevices = halIface.getAllDevices();
    foreach(QString device, halDevices) {
        if(device.contains(param)) {
            return true;
        }
    }
    return false;
}

bool QSystemInfoPrivate::hasHalUsbFeature(qint32 usbClass)
{
    QHalInterface halIface;
    QStringList halDevices = halIface.getAllDevices();
    foreach(QString device, halDevices) {
        QHalDeviceInterface ifaceDevice(device);
        if (ifaceDevice.isValid()) {
            if(ifaceDevice.getPropertyString("info.subsystem") == "usb_device") {
                if(ifaceDevice.getPropertyInt("usb.interface.class") == usbClass) {
                    return true;
                }
            }
        }
    }
    return false;
}
#endif

bool QSystemInfoPrivate::hasSysFeature(const QString &featureStr)
{
    QString sysPath = "/sys/class/";
    QDir sysDir(sysPath);
    QStringList filters;
    filters << "*";
    QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(QString dir, sysList) {
        QDir sysDir2(sysPath + dir);
        if(dir.contains(featureStr)) {
            QStringList sysList2 = sysDir2.entryList( filters ,QDir::Dirs, QDir::Name);
            if(!sysList2.isEmpty()) {
                return true;
            }
        }
    }
    return false;
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
#if !defined(QT_NO_DBUS)
            featureSupported = hasHalDeviceFeature("bluetooth");
            if(featureSupported)
                return featureSupported;
#endif
            featureSupported = hasSysFeature("bluetooth");
        }
        break;
    case QSystemInfo::CameraFeature :
        {
#if !defined(QT_NO_DBUS)
            featureSupported = hasHalUsbFeature(0x06); // image
            if(featureSupported)
                return featureSupported;
#endif
            featureSupported = hasSysFeature("video");
        }
        break;
    case QSystemInfo::FmradioFeature :
        {
            QString sysPath = "/sys/class/video4linux/";
            QDir sysDir(sysPath);
            QStringList filters;
            filters << "*";
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains("radio")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::IrFeature :
        {
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalUsbFeature(0xFE);
        if(featureSupported)
            return featureSupported;
#endif
        featureSupported = hasSysFeature("irda"); //?
    }
        break;
    case QSystemInfo::LedFeature :
        {
            featureSupported = hasSysFeature("led"); //?
        }
        break;
    case QSystemInfo::MemcardFeature :
        {
#if !defined(QT_NO_DBUS)
            QHalInterface iface;
            if (iface.isValid()) {
                QHalInterface halIface;
                QStringList halDevices = halIface.getAllDevices();
                foreach(QString device, halDevices) {
                    QHalDeviceInterface ifaceDevice(device);
                    if (ifaceDevice.isValid()) {
                        if(ifaceDevice.getPropertyString("info.subsystem") == "mmc_host") {
                            return true;
                        }
                        if(ifaceDevice.getPropertyBool("storage.removable")) {
                            return true;
                        }
                    }
                }
            }
#endif
        }
        break;
    case QSystemInfo::UsbFeature :
        {
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalDeviceFeature("usb");
        if(featureSupported)
            return featureSupported;
#endif
            featureSupported = hasSysFeature("usb_host");
        }
        break;
    case QSystemInfo::VibFeature :
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalDeviceFeature("vibrator"); //might not always be true
        if(featureSupported)
            return featureSupported;
#endif
        break;
    case QSystemInfo::WlanFeature :
        {
#if !defined(QT_NO_DBUS)
            QHalInterface iface;
            if (iface.isValid()) {
                QStringList list = iface.findDeviceByCapability("net.80211");
                if(!list.isEmpty()) {
                    featureSupported = true;
                    break;
                }
            }
#endif
            featureSupported = hasSysFeature("80211");
        }
        break;
    case QSystemInfo::SimFeature :
        break;
    case QSystemInfo::LocationFeature :
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalDeviceFeature("gps"); //might not always be true
        if(featureSupported)
            return featureSupported;

#endif
        break;
    case QSystemInfo::VideoOutFeature :
        {
            QString sysPath = "/sys/class/video4linux/";
            QDir sysDir(sysPath);
            QStringList filters;
            filters << "*";
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains("video")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    default:
        featureSupported = false;
        break;
    };
    return featureSupported;
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo wiFi(devFile + "/wireless");
                QFileInfo fi("/proc/net/route");
                if(wiFi.exists() && fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString result = rx.readAll();
                        if(result.contains(dir)) {
                            return QSystemNetworkInfo::Connected;
                        } else {
                            return QSystemNetworkInfo::NoNetworkAvailable;
                        }
                    }
                }
            }
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QString baseSysDir = "/sys/class/net/";
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi("/proc/net/route");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString result = rx.readAll();
                        if(result.contains(dir)) {
                            return QSystemNetworkInfo::Connected;
                        } else {
                            return QSystemNetworkInfo::NoNetworkAvailable;
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    };
    return QSystemNetworkInfo::UndefinedStatus;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/wireless/link");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result.toInt();

                    }
                }
            }
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/carrier");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result.toInt() * 100;

                    }
                }
            }
        }
        break;
    default:
        break;
    };

    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
    return -1;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return -1;
}

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return "No Mobile Network";
}

// Mobile Network Code
QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return "No Mobile Network";
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return "No Mobile Network";
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return "No Mobile Network";
}

QString QSystemNetworkInfoPrivate::operatorName()
{
    return "No Operator";
}

QString QSystemNetworkInfoPrivate::wlanSsid()
{
    QString essid;
    if(networkStatus(QSystemNetworkInfo::WlanMode) != QSystemNetworkInfo::Connected) {
        return essid;
    }
    QString wlanInterface;
    QString baseSysDir = "/sys/class/net/";
    QDir wDir(baseSysDir);
    QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QString dir, dirs) {
        QString devFile = baseSysDir + dir;
        QFileInfo fi(devFile + "/wireless");
        if(fi.exists()) {
            wlanInterface = dir;
            qWarning() << "interface is" << wlanInterface;
        }
    }
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock > 0) {
        const char* someRandomBuffer[IW_ESSID_MAX_SIZE + 1];
        struct iwreq wifiExchange;
        memset(&wifiExchange, 0, sizeof(wifiExchange));
        memset(someRandomBuffer, 0, sizeof(someRandomBuffer));

        wifiExchange.u.essid.pointer = (caddr_t) someRandomBuffer;
        wifiExchange.u.essid.length = IW_ESSID_MAX_SIZE;
        wifiExchange.u.essid.flags = 0;

        const char* interfaceName = wlanInterface.toLatin1();
        strncpy(wifiExchange.ifr_name, interfaceName, IFNAMSIZ);
        wifiExchange.u.essid.length = IW_ESSID_MAX_SIZE + 1;

        if (ioctl(sock, SIOCGIWESSID, &wifiExchange) == 0) {
            const char *ssid = (const char *)wifiExchange.u.essid.pointer;
            essid = ssid;
        }
    }
    close(sock);
   return essid;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    QFile rx(devFile + "/address");
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result;
                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::EthernetMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/address");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result;
                    }
                }
            }
        }
        break;
    default:
        break;
    };
    return QString();
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    QList<QNetworkInterface> interfaceList;
    interfaceList = QNetworkInterface::allInterfaces();

    QString result;
    QString baseSysDir = "/sys/class/net/";
    QDir eDir(baseSysDir);
    QStringList dirs = eDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QString dir, dirs) {
        QString devFile = baseSysDir + dir;
        QFileInfo devfi(devFile + "/device");
        if(!devfi.exists()) {
            continue;
        }
        switch(mode) {
        case QSystemNetworkInfo::WlanMode:
            {
                qWarning() << devFile;
                QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
        case QSystemNetworkInfo::EthernetMode:
            {
                QFileInfo fi(devFile + "/wireless");
                if(!fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
            default:
            break;
        };
    }
    return QNetworkInterface();
}

//////// QSystemDisplayInfo
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
        : QObject(parent)
{
    halIsAvailable = halAvailable();
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int screen)
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        if (iface.isValid()) {
            QStringList list = iface.findDeviceByCapability("laptop_panel");
            if(!list.isEmpty()) {
                foreach(QString lapDev, list) {
                    QHalDeviceInterface ifaceDevice(lapDev);
                    QHalDeviceLaptopPanelInterface lapIface(lapDev);
                    float numLevels = ifaceDevice.getPropertyInt("laptop_panel.num_levels");
                    float curLevel = lapIface.getBrightness();
                    return curLevel / numLevels * 100;
                }
            }
        }
#endif
    } else {
        QString backlightPath = "/proc/acpi/video/";
        QDir videoDir(backlightPath);
        QStringList filters;
        filters << "*";
        QStringList brightnessList = videoDir.entryList(filters,
                                                        QDir::Dirs
                                                        | QDir::NoDotAndDotDot,
                                                        QDir::Name);
        foreach(QString brightnessFileName, brightnessList) {
            float numLevels = 0.0;
            float curLevel = 0.0;
            QFile curBrightnessFile(backlightPath+brightnessFileName+"/LCD/brightness");
            if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning()<<"File not opened";
            } else {
                QString  strvalue;
                strvalue = curBrightnessFile.readAll().trimmed();
                if(strvalue.contains("levels")) {
                    QStringList list = strvalue.split(" ");
                    numLevels = list.at(2).toFloat();
                }
                if(strvalue.contains("current")) {
                    QStringList list = strvalue.split(": ");
                    curLevel = list.at(list.count()-1).toFloat();
                }
                curBrightnessFile.close();
                return curLevel / numLevels * 100;
            }
        }
    }
#if 0
    QString backlightPath = "/sys/devices/virtual/backlight/";
    QDir videoDir(backlightPath);
    QStringList filters;
    filters << "*";
    QStringList brightnessList = videoDir.entryList(filters,
                                                     QDir::Dirs
                                                     | QDir::NoDotAndDotDot,
                                                     QDir::Name);
    foreach(QString brightnessFileName, brightnessList) {
        float numLevels = 0.0;
        float curLevel = 0.0;
        QFile curBrightnessFile(backlightPath+brightnessFileName+"/brightness");
        if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = curBrightnessFile.readLine().trimmed();
            curBrightnessFile.close();
            curLevel = strvalue.toFloat();

            QFile maxBrightnessFile(backlightPath+brightnessFileName+"/max_brightness");
            if(!maxBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning()<<"File not opened";
            } else {
                QString strvalue;
                strvalue = maxBrightnessFile.readLine().trimmed();
                maxBrightnessFile.close();
                numLevels = strvalue.toFloat();
            }
            return curLevel / numLevels * 100;
        }
    }
#endif
    return -1;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
#ifdef Q_WS_X11
    QDesktopWidget wid;
    return wid.screen(screen)->x11Info().depth();
#else
        return QPixmap::defaultDepth();
#endif
}


//////// QSystemMemoryInfo
QSystemMemoryInfoPrivate::QSystemMemoryInfoPrivate(QObject *parent)
        : QObject(parent)
{
    halIsAvailable = halAvailable();
}


QSystemMemoryInfoPrivate::~QSystemMemoryInfoPrivate()
{
}

qint64 QSystemMemoryInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    mountEntries();
    struct statfs fs;
    if(statfs(mountEntriesHash[driveVolume].toLatin1(), &fs ) ==0 ) {
        long blockSize = fs.f_bsize;
        long availBlocks = fs.f_bavail;
        return (double)availBlocks * blockSize;
    }
    return 0;
}

qint64 QSystemMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    mountEntries();
    mountEntriesHash[driveVolume];
    struct statfs fs;
    if(statfs(mountEntriesHash[driveVolume].toLatin1(), &fs ) == 0 ) {
        long blockSize = fs.f_bsize;
        long totalBlocks = fs.f_blocks;
        return (double)totalBlocks * blockSize;
    }
    return 0;
}

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::volumeType(const QString &driveVolume)
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QStringList mountedVol;
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("volume");
        if(!list.isEmpty()) {
            foreach(QString vol, list) {
                QHalDeviceInterface ifaceDevice(vol);
                if(driveVolume == ifaceDevice.getPropertyString("block.device")) {
                    QHalDeviceInterface ifaceDeviceParent(ifaceDevice.getPropertyString("info.parent"), this);
                    if(ifaceDeviceParent.getPropertyBool("storage.removable")) {
                        return QSystemMemoryInfo::Removable;
                    } else {
                        return QSystemMemoryInfo::Internal;
                    }
                }
            }
        }
#endif
    } else {

    }
    return QSystemMemoryInfo::Internal;
}

QStringList QSystemMemoryInfoPrivate::listOfVolumes()
{
    mountEntries();
    return mountEntriesHash.keys();
}

void QSystemMemoryInfoPrivate::mountEntries()
{
    mountEntriesHash.clear();
    FILE *mntfp = setmntent( "/proc/mounts", "r" );
    mntent *me = getmntent(mntfp);
    while(me != NULL) {
        struct statfs fs;
        if(statfs(me->mnt_dir, &fs ) ==0 ) {
            long blockSize = fs.f_bsize;
            long totalBlocks = fs.f_blocks;
            double total = (double)totalBlocks * blockSize;
            if(total > 0 && !mountEntriesHash.keys().contains(me->mnt_dir)) {
//                qWarning() << me->mnt_type;
                mountEntriesHash[me->mnt_fsname] = me->mnt_dir;
            }
        }

        me = getmntent(mntfp);
    }
    endmntent(mntfp);
}


//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
    halIsAvailable = halAvailable();
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods = 0;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface2;
        if (iface2.isValid()) {
            QStringList capList;
            capList << "input.keyboard" << "input.keys" << "input.keypad" << "input.mouse" << "input.tablet";
            for(int i = 0; i < capList.count(); i++) {
                QStringList list = iface2.findDeviceByCapability(capList.at(i));
                if(!list.isEmpty()) {
                    switch(i) {
                    case 0:
                        methods = (methods | QSystemDeviceInfo::Keyboard);
                        break;
                    case 1:
                        methods = (methods | QSystemDeviceInfo::Keys);
                        break;
                    case 2:
                        methods = (methods | QSystemDeviceInfo::Keypad);
                        break;
                    case 3:
                        methods = (methods | QSystemDeviceInfo::Mouse);
                        break;
                    case 4:
                        methods = (methods | QSystemDeviceInfo::SingleTouch);
                        break;
                    };
                }
            }
            if(methods != 0)
                return methods;
        }
#endif
    }
    QString inputsPath = "/sys/class/input/";
    QDir inputDir(inputsPath);
    QStringList filters;
    filters << "event*";
    QStringList inputList = inputDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(QString inputFileName, inputList) {
        QFile file(inputsPath+inputFileName+"/device/name");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = file.readLine();
            file.close();
            if(strvalue.contains("keyboard")) {
                if( (methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                    methods = (methods | QSystemDeviceInfo::Keyboard);
                }
            } else if(strvalue.contains("Mouse")) {
                if( (methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
                    methods = (methods | QSystemDeviceInfo::Mouse);
                }
            } else if(strvalue.contains("Button")) {
                if( (methods & QSystemDeviceInfo::Keys) != QSystemDeviceInfo::Keys) {
                    methods = (methods | QSystemDeviceInfo::Keys);
                }
            } else if(strvalue.contains("TouchScreen")) {
                if( (methods & QSystemDeviceInfo::SingleTouch) != QSystemDeviceInfo::SingleTouch) {
                    methods = (methods | QSystemDeviceInfo::SingleTouch);
                }
            }
        }
    }
    return methods;
}

QString QSystemDeviceInfoPrivate::imei()
{
//    if(this->getSimStatus() == QSystemDeviceInfo::SimNotAvailable)
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::imsi()
{
//    if(getSimStatus() == QSystemDeviceInfo::SimNotAvailable)
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString manu;
        if (iface.isValid()) {
            manu = iface.getPropertyString("system.firmware.vendor");
            if(manu.isEmpty()) {
                manu = iface.getPropertyString("system.hardware.vendor");
                if(!manu.isEmpty()) {
                    return manu;
                }
            }
        }
#endif
    }
    QFile vendorId("/sys/devices/virtual/dmi/id/board_vendor");
    if (vendorId.open(QIODevice::ReadOnly)) {
        QTextStream cpuinfo(&vendorId);
        return cpuinfo.readLine().trimmed();
    } else {
        //        qWarning() << "Could not open /sys/devices/virtual/dmi/id/board_vendor";
        QFile file("/proc/cpuinfo");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open /proc/cpuinfo";
        } else {
            QTextStream cpuinfo(&file);
            QString line = cpuinfo.readLine();
            while (!line.isNull()) {
                line = cpuinfo.readLine();
                if(line.contains("vendor_id")) {
                    return line.split(": ").at(1).trimmed();
                }
            }
        }
    }
    return QString();
}

QString QSystemDeviceInfoPrivate::model()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString model;
        if (iface.isValid()) {
            model = iface.getPropertyString("system.kernel.machine");
            if(!model.isEmpty())
                model += " ";
            model += iface.getPropertyString("system.chassis.type");
            if(!model.isEmpty())
                return model;
        }
#endif
    }
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/cpuinfo";
    } else {
        QTextStream cpuinfo(&file);
        QString line = cpuinfo.readLine();
        while (!line.isNull()) {
            line = cpuinfo.readLine();
            if(line.contains("model name")) {
                return line.split(": ").at(1).trimmed();
            }
        }
    }
    return QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString productName;
        if (iface.isValid()) {
            productName = iface.getPropertyString("info.product");
            if(productName.isEmpty()) {
                productName = iface.getPropertyString("system.product");
                if(!productName.isEmpty())
                    return productName;
            } else {
                return productName;
            }
        }
#endif
    }
    QDir dir("/etc");
    if(dir.exists()) {
        QStringList langList;
        QFileInfoList localeList = dir.entryInfoList(QStringList() << "*release",
                                                     QDir::Files | QDir::NoDotAndDotDot,
                                                     QDir::Name);
        foreach(QFileInfo fileInfo, localeList) {
            QString filepath = fileInfo.filePath();
            QFile file(filepath);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream prodinfo(&file);
                QString line = prodinfo.readLine();
                while (!line.isNull()) {
                    if(filepath.contains("lsb.release")) {
                        if(line.contains("DISTRIB_DESCRIPTION")) {
                            return line.split("=").at(1).trimmed();
                        }
                    } else {
                        return line;
                    }
                    line = prodinfo.readLine();
                }
            }
        } //end foreach
    }

    QFile file("/etc/issue");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/cpuinfo";
    } else {
        QTextStream prodinfo(&file);
        QString line = prodinfo.readLine();
        while (!line.isNull()) {
            line = prodinfo.readLine();
            if(!line.isEmpty()) {
                QStringList lineList = line.split(" ");
                for(int i = 0; i < lineList.count(); i++) {
                    if(lineList.at(i).toFloat()) {
                        return lineList.at(i-1) + " "+ lineList.at(i);
                    }
                }
            }
        }
    }
    return QString();
}

bool QSystemDeviceInfoPrivate::isBatteryCharging()
{
    bool isCharging = false;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    isCharging = ifaceDevice.getPropertyBool("battery.rechargeable.is_charging");
                }
            }
        }
#endif
    }
    QFile statefile("/proc/acpi/battery/BAT0/state");
    if (!statefile.open(QIODevice::ReadOnly)) {
      //  qWarning() << "Could not open /proc/acpi/battery/BAT0/state";
    } else {
        QTextStream batstate(&statefile);
        QString line = batstate.readLine();
        while (!line.isNull()) {
            if(line.contains("charging state")) {
                if(line.split(" ").at(1).trimmed() == "charging") {
                    isCharging = true;
                    break;
                }
            }
            line = batstate.readLine();
        }
        statefile.close();
    }
    return isCharging;
}

int QSystemDeviceInfoPrivate::batteryLevel() const
{
    float levelWhenFull = 0.0;
    float level = 0.0;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(!ifaceDevice.getPropertyBool("battery.present") ){
                        return QSystemDeviceInfo::NoBatteryLevel;
                    } else {
                        levelWhenFull = ifaceDevice.getPropertyInt("battery.charge_level.last_full");
                        level = ifaceDevice.getPropertyInt("battery.charge_level.current");
                    }
                }
            }
        }
#endif
    }
    QFile infofile("/proc/acpi/battery/BAT0/info");
    if (!infofile.open(QIODevice::ReadOnly)) {
     //   qWarning() << "Could not open /proc/acpi/battery/BAT0/info";
        return QSystemDeviceInfo::NoBatteryLevel;
    } else {
        QTextStream batinfo(&infofile);
        QString line = batinfo.readLine();
        while (!line.isNull()) {
            if(line.contains("design capacity")) {
                levelWhenFull = line.split(" ").at(1).trimmed().toFloat();
                infofile.close();
                break;
            }
            line = batinfo.readLine();
        }
        infofile.close();
    }

    QFile statefile("/proc/acpi/battery/BAT0/state");
    if (!statefile.open(QIODevice::ReadOnly)) {
   //     qWarning() << "Could not open /proc/acpi/battery/BAT0/state";
        return QSystemDeviceInfo::NoBatteryLevel;
    } else {
        QTextStream batstate(&statefile);
        QString line = batstate.readLine();
        while (!line.isNull()) {
            if(line.contains("remaining capacity")) {
                level = line.split(" ").at(1).trimmed().toFloat();
                statefile.close();
                break;
            }
            line = batstate.readLine();
        }
    }
    if(level != 0 && levelWhenFull != 0) {
        level = level / levelWhenFull * 100;
        return level;
    }

    return 0;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

 QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
 {
#if !defined(QT_NO_DBUS)
     qWarning() << Q_FUNC_INFO;
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("ac_adapter");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                qWarning() <<"AC" << dev;
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(ifaceDevice.getPropertyBool("ac_adapter.present")) {
                        return QSystemDeviceInfo::WallPower;
                    }
                }
            }
        }

        list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                qWarning() <<"battery"<< dev;
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(ifaceDevice.getPropertyBool("battery.rechargable.is_discharging") ){
                        return QSystemDeviceInfo::BatteryPower;
                    } else {
                        return QSystemDeviceInfo::WallPower;
                    }
                }
            }
        }
#endif

        return QSystemDeviceInfo::UnknownPower;
 }


//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
{

}

bool QSystemScreenSaverPrivate::setScreenSaverEnabled(bool state)
{
    Q_UNUSED(state);
    QDesktopWidget wid;
#ifdef Q_WS_X11
    qWarning() << wid.screenGeometry(0) << wid.window()->isActiveWindow();
    Display *dip = QX11Info::display();
    //   XActivateScreenSaver(dip);
    qWarning() << wid.screenGeometry(0) << wid.window()->isActiveWindow();
    int timeout;
    int interval;
    int preferBlank;
    int allowExp;
    XGetScreenSaver(dip, &timeout, &interval, &preferBlank, &allowExp);
    qWarning() << "XScreensaver" << timeout << interval << preferBlank << allowExp;
#endif
    /* ~/.kde4/share/config/kscreensaverrc
    [ScreenSaver]
    Enabled=
    Lock=
    Saver=kblank.desktop
*/
    //    wid.x11Info();
    return false;
}

bool QSystemScreenSaverPrivate::setScreenBlankingEnabled(bool state)
{
    Q_UNUSED(state);
    return false;
}

bool QSystemScreenSaverPrivate::screenSaverEnabled()
{
    return false;
}

bool QSystemScreenSaverPrivate::screenBlankingEnabled()
{
    return false;
}

bool QSystemScreenSaverPrivate::isScreenLockOn()
{
    return false;
}


QT_END_NAMESPACE
