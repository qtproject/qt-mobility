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
#include "qsysteminfo.h"
#include "qsysteminfo_p.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include <locale.h>

QT_BEGIN_NAMESPACE

//////// QSystemInfo
QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

// 2 letter ISO 639-1
QString QSystemInfoPrivate::currentLanguage() const
{
    return QString(setlocale(LC_ALL,"")).left(2);
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
QString QSystemInfoPrivate::getVersion(QSystemInfo::Version type,  const QString &parameter) const
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Installed";
    switch(type) {
    case QSystemInfo::Os :
       break;
    case QSystemInfo::QtCore :
       return  qVersion();
       break;
    case QSystemInfo::WrtCore :
       break;
    case QSystemInfo::Webkit :
       break;
    case QSystemInfo::ServiceFramework :
       break;
    case QSystemInfo::WrtExtensions :
       break;
    case QSystemInfo::ServiceProvider :
       break;
    case QSystemInfo::NetscapePlugin :
       break;
    case QSystemInfo::WebApp :
       break;
    case QSystemInfo::Firmware :
       break;
    };
  return errorStr;
}


//2 letter ISO 3166-1
QString QSystemInfoPrivate::countryCode() const
{
    return QString(setlocale(LC_ALL,"")).mid(3,2);
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        featureSupported = true;
        break;
    case QSystemInfo::CameraFeature :
        featureSupported = true;
        break;
    case QSystemInfo::FmradioFeature :
        featureSupported = true;
        break;
    case QSystemInfo::IrFeature :
        featureSupported = true;
        break;
    case QSystemInfo::LedFeature :
        featureSupported = true;
        break;
    case QSystemInfo::MemcardFeature :
        featureSupported = true;
        break;
    case QSystemInfo::UsbFeature :
        featureSupported = true;
        break;
    case QSystemInfo::VibFeature :
        featureSupported = true;
        break;
    case QSystemInfo::WlanFeature :
        featureSupported = true;
        break;
    case QSystemInfo::SimFeature :
        featureSupported = true;
        break;
    case QSystemInfo::LocationFeature :
        featureSupported = true;
        break;
    case QSystemInfo::VideoOutFeature :
        featureSupported = true;
        break;
    case QSystemInfo::UnknownFeature :
    default:
        featureSupported = true;
    break;
    };
    return featureSupported;
}

QString QSystemInfoPrivate::getDetailOfFeature(QSystemInfo::Feature feature)
{
    Q_UNUSED(feature);
    return "No other features";
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

QSystemNetworkInfo::CellNetworkStatus QSystemNetworkInfoPrivate::getCellNetworkStatus()
{
    return QSystemNetworkInfo::NoNetworkAvailable;
}

qint32 QSystemNetworkInfoPrivate::networkSignalStrength()
{
    return -1;
}

qint32 QSystemNetworkInfoPrivate::cellId()
{
    return -1;
}

qint32 QSystemNetworkInfoPrivate::locationAreaCode()
{
    return -1;
}

// Mobile Country Code
qint32 QSystemNetworkInfoPrivate::currentMCC()
{
    return -1;
}

// Mobile Network Code
qint32 QSystemNetworkInfoPrivate::currentMNC()
{
    return -1;
}

qint32 QSystemNetworkInfoPrivate::homeMCC()
{
    return -1;
}

qint32 QSystemNetworkInfoPrivate::homeMNC()
{
    return -1;
}

bool QSystemNetworkInfoPrivate::isLocationEnabled() const
{
    return false;
}

bool QSystemNetworkInfoPrivate::isWLANAccessible() const
{
    return false;
}

QString QSystemNetworkInfoPrivate::operatorName()
{
    return QString();
}

//////// QSystemDisplayInfo
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

qint32 QSystemDisplayInfoPrivate::displayBrightness()
{
    return -1;
}

void QSystemDisplayInfoPrivate::setScreenSaverEnabled(bool)
{
}

void QSystemDisplayInfoPrivate::setScreenBlankingEnabled(bool)
{
}

qint32 QSystemDisplayInfoPrivate::colorDepth(qint32 screen)
{
    Q_UNUSED(screen);
    return QPixmap::defaultDepth();
}

bool QSystemDisplayInfoPrivate::isScreenLockOn()
{
    return false;
}

//////// QSystemMemoryInfo
QSystemMemoryInfoPrivate::QSystemMemoryInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

bool  QSystemMemoryInfoPrivate::hasRamMemoryLevel()
{
    return true;
}

qint64 QSystemMemoryInfoPrivate::freeMemoryLevel() const
{
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/meminfo";
        return -1;
    }
    QTextStream meminfo(&file);
    QString line;
    line = meminfo.readLine();
    if(line.contains("MemTotal")) {
        QString ls = line.section(' ',-2, -2);
        return ls.toInt();
    }
    return -1;
}

qint64 QSystemMemoryInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    return -1;
}

qint64 QSystemMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    return -1;
}

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::getVolumeType(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    return QSystemMemoryInfo::NoVolume;
}

QStringList QSystemMemoryInfoPrivate::listOfVolumes()
{
    return QStringList();
}

bool QSystemMemoryInfoPrivate::isCriticalMemory() const
{
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/meminfo";
        return false;
    }

    QTextStream meminfo(&file);
    QString line;
    quint64 total = 0;
    quint64 memfree = 0;
    quint64 buffers = 0;
    quint64 cached = 0;

    bool ok = false;
    static QRegExp kernel24HeaderLine("\\s+total:\\s+used:\\s+free:\\s+shared:\\s+buffers:\\s+cached:");

    line = meminfo.readLine();
    if(kernel24HeaderLine.indexIn(line) > -1) {
        static QRegExp kernel24DataLine("Mem:\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
        line = meminfo.readLine();
        if(kernel24DataLine.indexIn(line) > -1) {
            total = kernel24DataLine.cap(1).toULongLong(&ok);
            memfree = kernel24DataLine.cap(3).toULongLong();
            buffers = kernel24DataLine.cap(5).toULongLong();
            cached = kernel24DataLine.cap(6).toULongLong();
        }
    }  else {
        static QRegExp kernel26MemTotalLine("MemTotal:\\s+(\\d+)\\s+kB");
        static QRegExp kernel26MemFreeLine("MemFree:\\s+(\\d+)\\s+kB");
        static QRegExp kernel26BuffersLine("Buffers:\\s+(\\d+)\\s+kB");
        static QRegExp kernel26CachedLine("Cached:\\s+(\\d+)\\s+kB");
        while (!meminfo.atEnd()) {
            if (kernel26MemTotalLine.indexIn(line) > -1)
                total = kernel26MemTotalLine.cap(1).toULongLong(&ok);
            else if (kernel26MemFreeLine.indexIn(line) > -1)
                memfree = kernel26MemFreeLine.cap(1).toULongLong();
            else if (kernel26BuffersLine.indexIn(line) > -1)
                buffers = kernel26BuffersLine.cap(1).toULongLong();
            else if (kernel26CachedLine.indexIn(line) > -1) {
                cached = kernel26CachedLine.cap(1).toULongLong();
                break; //last entry to read
            }
            line = meminfo.readLine();
        }
    }

    if (!ok)
        qWarning() << "Meminfo cannot monitor available memory";
    quint64 percentAvailable = total ? (100 * ( buffers + cached + memfree) / total) : 0;
if(percentAvailable < 4) //3% is critical
    return true;
else
    return false;
}

//bool  QSystemMemoryInfoPrivate::isDiskSpaceCritical(const QString &driveVolume)
// {
//    Q_UNUSED(driveVolume);
//    return false;
// }

 bool QSystemMemoryInfoPrivate::isBatteryCharging()
{
    return false;
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

QSystemDeviceInfo::InputMethods QSystemDeviceInfoPrivate::getInputMethodType()
{
 return QSystemDeviceInfo::KeysAndTouch;
}

QString QSystemDeviceInfoPrivate::imei() const
{
    return QString();
}

QString QSystemDeviceInfoPrivate::imsi() const
{
    return QString();
}

QString QSystemDeviceInfoPrivate::manufacturer() const
{
    return QString();
}

QString QSystemDeviceInfoPrivate::model() const
{
    return QString();
}

QSystemDeviceInfo::BatteryLevel QSystemDeviceInfoPrivate::batteryLevel() const
{
    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::getSimStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

QT_END_NAMESPACE
