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

#ifndef QSYSTEMINFOSTATE_SIMULATOR_P_H
#define QSYSTEMINFOSTATE_SIMULATOR_P_H


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


#include "qsysteminfo.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMetaType>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

void qt_registerSystemInfoTypes();

struct QSystemInfoData
{
    QString currentLanguage;
    QStringList availableLanguages;
    QString currentCountryCode;

    QVector<QString> versions;
    QVector<bool> features;
};

struct QSystemNetworkInfoData
{
    struct NetworkInfo
    {
        QSystemNetworkInfo::NetworkStatus status;
        QString name;
        QString macAddress;
        qint32 signalStrength;
        QNetworkInterface interface;
    };

    QVector<NetworkInfo> networkInfo;

    int cellId;
    int locationAreaCode;

    QString currentMobileCountryCode;
    QString currentMobileNetworkCode;
    QString homeMobileCountryCode;
    QString homeMobileNetworkCode;
    QSystemNetworkInfo::NetworkMode currentMode;
};

struct QSystemDisplayInfoData
{
    int displayBrightness;
    int colorDepth;
};

struct QSystemStorageInfoData
{
    struct DriveInfo
    {
        QSystemStorageInfo::DriveType type;
        qint64 totalSpace;
        qint64 availableSpace;
    };

    QHash<QString, DriveInfo> drives;
};

struct QSystemDeviceInfoData
{
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
};

struct QSystemScreenSaverData
{
    unsigned screenSaverInhibited;
};

QTM_END_NAMESPACE

// Can't use QTM_NAMESPACE here, since it won't be expanded.
Q_DECLARE_METATYPE(QtMobility::QSystemInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemNetworkInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemDisplayInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemStorageInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemDeviceInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemScreenSaverData)

QT_END_HEADER

#endif // QSYSTEMINFOSTATE_SIMULATOR_P_H
