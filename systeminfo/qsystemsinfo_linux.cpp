/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qsystemsinfo.h"
#include "qsystemsinfo_p.h"

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

QSystemsInfoPrivate::QSystemsInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

// 2 letter ISO 639-1
QString QSystemsInfoPrivate::currentLanguage() const
{
    return QString(setlocale(LC_ALL,"")).left(2);
}

// 2 letter ISO 639-1
QStringList QSystemsInfoPrivate::availableLanguages() const
{
    return QStringList();
}

// "major.minor.build" format.
QString QSystemsInfoPrivate::getVersion(QSystemsInfo::Version type,  const QString &parameter) const
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Installed";
    switch(type) {
    case QSystemsInfo::Os :
       break;
    case QSystemsInfo::QtCore :
       return  qVersion();
       break;
    case QSystemsInfo::WrtCore :
       break;
    case QSystemsInfo::Webkit :
       break;
    case QSystemsInfo::ServiceFramework :
       break;
    case QSystemsInfo::WrtExtensions :
       break;
    case QSystemsInfo::ServiceProvider :
       break;
    case QSystemsInfo::NetscapePlugin :
       break;
    case QSystemsInfo::WebApp :
       break;
    case QSystemsInfo::Firmware :
       break;
    };
  return errorStr;
}


//2 letter ISO 3166-1
QString QSystemsInfoPrivate::countryCode() const
{
    return QString(setlocale(LC_ALL,"")).mid(3,2);
}

bool QSystemsInfoPrivate::hasFeatureSupported(QSystemsInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemsInfo::BluetoothFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::CameraFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::FmradioFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::IrFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::LedFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::MemcardFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::UsbFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::VibFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::WlanFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::SimFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::LocationFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::VideoOutFeature :
        featureSupported = true;
        break;
    case QSystemsInfo::UnknownFeature :
    default:
        featureSupported = true;
    break;
    };
    return featureSupported;
}

QString QSystemsInfoPrivate::getDetailOfFeature(QSystemsInfo::Feature feature)
{
    Q_UNUSED(feature);
    return QString();
}

////////
QSystemsNetworkInfoPrivate::QSystemsNetworkInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

QSystemsNetworkInfo::CellNetworkStatus QSystemsNetworkInfoPrivate::getCellNetworkStatus()
{
    return QSystemsNetworkInfo::NoNetworkAvailable;
}

qint32 QSystemsNetworkInfoPrivate::networkSignalStrength()
{
    return -1;
}

qint32 QSystemsNetworkInfoPrivate::cellId()
{
    return -1;
}

qint32 QSystemsNetworkInfoPrivate::locationAreaCode()
{
    return -1;
}

// Mobile Country Code
qint32 QSystemsNetworkInfoPrivate::currentMCC()
{
    return -1;
}

// Mobile Network Code
qint32 QSystemsNetworkInfoPrivate::currentMNC()
{
    return -1;
}

qint32 QSystemsNetworkInfoPrivate::homeMCC()
{
    return -1;
}

qint32 QSystemsNetworkInfoPrivate::homeMNC()
{
    return -1;
}

bool QSystemsNetworkInfoPrivate::isLocationEnabled() const
{
    return false;
}

bool QSystemsNetworkInfoPrivate::isWLANAccessible() const
{
    return false;
}


// display
QSystemsDisplayInfoPrivate::QSystemsDisplayInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

qint32 QSystemsDisplayInfoPrivate::displayBrightness()
{
    return -1;
}

void QSystemsDisplayInfoPrivate::setScreenSaverEnabled(bool)
{
}

void QSystemsDisplayInfoPrivate::setScreenBlankingEnabled(bool)
{
}

qint32 QSystemsDisplayInfoPrivate::colorDepth(qint32 screen)
{
    Q_UNUSED(screen);
    return QPixmap::defaultDepth();
}

/////
QSystemsMemoryInfoPrivate::QSystemsMemoryInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

bool  QSystemsMemoryInfoPrivate::hasRamMemoryLevel()
{
    return true;
}

qint64 QSystemsMemoryInfoPrivate::freeMemoryLevel() const
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

qint64 QSystemsMemoryInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    return -1;
}

qint64 QSystemsMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    return -1;
}

QSystemsMemoryInfo::VolumeType QSystemsMemoryInfoPrivate::getVolumeType(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    return QSystemsMemoryInfo::NoVolume;
}

QStringList QSystemsMemoryInfoPrivate::listOfVolumes()
{
    return QStringList();
}

bool QSystemsMemoryInfoPrivate::isCriticalMemory() const
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

bool QSystemsMemoryInfoPrivate::isBatteryCharging()
{
    return false;
}

// device
QSystemsDeviceInfoPrivate::QSystemsDeviceInfoPrivate(QObject *parent)
{
    Q_UNUSED(parent);
}

QString QSystemsDeviceInfoPrivate::imei() const
{
    return QString();
}

QString QSystemsDeviceInfoPrivate::imsi() const
{
    return QString();
}

QString QSystemsDeviceInfoPrivate::manufacturer() const
{
    return QString();
}

QString QSystemsDeviceInfoPrivate::model() const
{
    return QString();
}

QSystemsDeviceInfo::BatteryLevel QSystemsDeviceInfoPrivate::batteryLevel() const
{
    return QSystemsDeviceInfo::NoBatteryLevel;
}

QSystemsDeviceInfo::SimStatus QSystemsDeviceInfoPrivate::getSimStatus()
{
    return QSystemsDeviceInfo::SimNotAvailable;
}

bool QSystemsDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

QT_END_NAMESPACE
