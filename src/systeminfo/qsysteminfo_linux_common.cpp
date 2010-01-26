/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qsysteminfo_linux_common_p.h"

QTM_BEGIN_NAMESPACE

QSystemInfoLinuxCommonPrivate::QSystemInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
    langCached = currentLanguage();
    startLanguagePolling();
}

QSystemInfoLinuxCommonPrivate::~QSystemInfoLinuxCommonPrivate()
{
}

void QSystemInfoLinuxCommonPrivate::startLanguagePolling()
{
    QString checkLang = QString::fromLocal8Bit(qgetenv("LANG"));
    if(langCached.isEmpty()) {
        currentLanguage();
    }
    checkLang = checkLang.left(2);
    if(checkLang != langCached) {
        emit currentLanguageChanged(checkLang);
        langCached = checkLang;
    }
    langTimer = new QTimer(this);
    QTimer::singleShot(1000, this, SLOT(startLanguagePolling()));
}

QString QSystemInfoLinuxCommonPrivate::currentLanguage() const
{
    QString lang;
    if(langCached.isEmpty()) {
        lang  = QLocale::system().name().left(2);
        if(lang.isEmpty() || lang == "C") {
            lang = "en";
        }
    } else {
        lang = langCached;
    }
    return lang;
}

// "major.minor.build" format.
QString QSystemInfoLinuxCommonPrivate::version(QSystemInfo::Version type,
                                               const QString &parameter)
{
    QString errorStr = "Not Available";

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
                qWarning() << "File not opened";
            } else {
                QString  strvalue;
                strvalue = versionFile.readAll().trimmed();
                strvalue = strvalue.split(" ").at(2);
                versionFile.close();
                return strvalue;
            }
            break;
        }
        case QSystemInfo::QtCore :
            return qVersion();
            break;
        default:
            break;
    };
    return errorStr;
}

//2 letter ISO 3166-1
QString QSystemInfoLinuxCommonPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}

bool QSystemInfoLinuxCommonPrivate::hasSysFeature(const QString &featureStr)
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
QSystemNetworkInfoLinuxCommonPrivate::QSystemNetworkInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemNetworkInfoLinuxCommonPrivate::~QSystemNetworkInfoLinuxCommonPrivate()
{
}

QSystemDisplayInfoLinuxCommonPrivate::QSystemDisplayInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemDisplayInfoLinuxCommonPrivate::~QSystemDisplayInfoLinuxCommonPrivate()
{
}

QSystemStorageInfoLinuxCommonPrivate::QSystemStorageInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemStorageInfoLinuxCommonPrivate::~QSystemStorageInfoLinuxCommonPrivate()
{
}

QSystemDeviceInfoLinuxCommonPrivate::QSystemDeviceInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemDeviceInfoLinuxCommonPrivate::~QSystemDeviceInfoLinuxCommonPrivate()
{
}

QSystemScreenSaverLinuxCommonPrivate::QSystemScreenSaverLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemScreenSaverLinuxCommonPrivate::~QSystemScreenSaverLinuxCommonPrivate()
{
}


#include "moc_qsysteminfo_linux_common_p.cpp"

QTM_END_NAMESPACE
