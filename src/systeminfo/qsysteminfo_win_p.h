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
#ifndef QSYSTEMINFO_WIN_P_H
#define QSYSTEMINFO_WIN_P_H


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
#include <QThread>
#include <QMutex>

#include "qsysteminfo.h"
#include "qmobilityglobal.h"

#include <winsock2.h>
#include <mswsock.h>

#include <QBasicTimer>

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;
class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();
    QString currentLanguage() const;
    QStringList availableLanguages() const;

    QString version(QTM_PREPEND_NAMESPACE(QSystemInfo::Version),  const QString &parameter = QString());

    QString currentCountryCode() const;
    bool hasFeatureSupported(QTM_PREPEND_NAMESPACE(QSystemInfo::Feature) feature);
Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:

    bool hasSysFeature(const QString &featureStr);
    QString currentLanguageStr;
private Q_SLOTS:
     void currentLanguageTimeout();

};

class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkStatus) networkStatus(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkMode) mode);
    qint32 networkSignalStrength(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkMode) mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkMode) mode);
    QString macAddress(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkMode) mode);

    QNetworkInterface interfaceForMode(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkMode) mode);


   void emitNetworkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void emitNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   QSystemNetworkInfo::NetworkMode currentMode();


   static QSystemNetworkInfoPrivate *instance();
protected:
   void timerEvent(QTimerEvent *event);

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);
private Q_SLOTS:
   void networkStrengthTimeout();
   void networkStatusTimeout();
private:
    quint32 wifiStrength;
    quint32 ethStrength;
    HANDLE hWlan;
    int timerMs;
   QBasicTimer netStrengthTimer;
   bool isDefaultMode(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo::NetworkMode) mode);
   void startWifiCallback();
   bool wlanCallbackInitialized;

};

class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();


    int displayBrightness(int screen);
    int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation getOrientation(int screen);
    float contrast(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
};

class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList logicalDrives();
    QTM_PREPEND_NAMESPACE(QSystemStorageInfo::DriveType) typeForDrive(const QString &driveVolume);

private:
    QHash<QString, QString> mountEntriesHash;
    void mountEntries();

};

#if defined(Q_OS_WINCE)

#include "Pm.h"

class QSystemDeviceInfoPrivate;

class QPowerNotificationThread : public QThread
{
    Q_OBJECT

public:
    QPowerNotificationThread(QSystemDeviceInfoPrivate *parent = 0);
    ~QPowerNotificationThread();

protected:
    void run();

private:
    QSystemDeviceInfoPrivate *parent;
    bool done;
    QMutex mutex;
    HANDLE wakeUpEvent;
};
#endif

class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoPrivate();

    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    QString productName();

    QTM_PREPEND_NAMESPACE(QSystemDeviceInfo::InputMethodFlags) inputMethodType();

    int  batteryLevel();

    QTM_PREPEND_NAMESPACE(QSystemDeviceInfo::SimStatus) simStatus();
    bool isDeviceLocked();
    QTM_PREPEND_NAMESPACE(QSystemDeviceInfo::Profile) currentProfile();

    QTM_PREPEND_NAMESPACE(QSystemDeviceInfo::PowerState) currentPowerState();
    void setConnection();
    static QSystemDeviceInfoPrivate *instance() {return self;}

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus );

    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);

private:
    int batteryLevelCache;
    QTM_PREPEND_NAMESPACE(QSystemDeviceInfo::PowerState) currentPowerStateCache;
    QTM_PREPEND_NAMESPACE(QSystemDeviceInfo::BatteryStatus) batteryStatusCache;
#if defined(Q_OS_WINCE)
    QPowerNotificationThread *powerNotificationThread;
#endif
    static QSystemDeviceInfoPrivate *self;
};


class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    bool screenSaverSecureEnabled();
private:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_WIN_P_H*/

// End of file

