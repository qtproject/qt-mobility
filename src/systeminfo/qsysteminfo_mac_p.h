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
#ifndef QSYSTEMINFO_MAC_P_H
#define QSYSTEMINFO_MAC_P_H


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
#include <qmobilityglobal.h>

#include <QTimer>
#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
 #include <QEventLoop>

#include <SystemConfiguration/SystemConfiguration.h>
//#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;

class QLangLoopThread;

class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();
    QString currentLanguage() const;
    QStringList availableLanguages() const;

    QString version(QSystemInfo::Version,  const QString &parameter = QString());

    QString currentCountryCode() const;
    bool hasFeatureSupported(QSystemInfo::Feature feature);
    void languageChanged(const QString &);
    static QSystemInfoPrivate *instance() {return self;}

Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:
    QTimer *langTimer;
    QString langCached;
    QLangLoopThread * langloopThread;
    static QSystemInfoPrivate *self;

private Q_SLOTS:
 protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

};

class QRunLoopThread;
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

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    static QSystemNetworkInfoPrivate *instance() {return self;}
    void wifiNetworkChanged(const QString &notification, const QString interfaceName);
    QString getDefaultInterface();
    QSystemNetworkInfo::NetworkMode currentMode();
    void ethernetChanged();


Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

public Q_SLOTS:
   void primaryInterface();

private:
    QTimer *rssiTimer;
    int signalStrengthCache;
    static QSystemNetworkInfoPrivate *self;
    QRunLoopThread * runloopThread;
    QString defaultInterface;
    QSystemNetworkInfo::NetworkMode modeForInterface(QString interfaceName);

private Q_SLOTS:
    void rssiTimeout();
protected:
    void startNetworkChangeLoop();
    bool isInterfaceActive(const char* netInterface);

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    bool hasWifi;

};

class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    int displayBrightness(int screen);
    int colorDepth(int screen);
};

class QDASessionThread;
class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

public slots:
    void storageChanged( bool added);
Q_SIGNALS:
    void storageAdded();
    void storageRemoved();

private:
    QHash<QString, QString> mountEntriesHash;
    bool updateVolumesMap();
    void mountEntries();
    bool sessionThread();

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

    QDASessionThread *daSessionThread;
};

class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    ~QSystemDeviceInfoPrivate();

    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model();
    static QString productName();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    int  batteryLevel();

    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();

    QSystemDeviceInfo::PowerState currentPowerState();
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
    QSystemDeviceInfo::PowerState currentPowerStateCache;
    QSystemDeviceInfo::BatteryStatus batteryStatusCache;
    static QSystemDeviceInfoPrivate *self;
};


class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();

private:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;
    bool isInhibited;
    QTimer *ssTimer;

private Q_SLOTS:
    void activityTimeout();

};

class QRunLoopThread : public QThread
{
    Q_OBJECT

public:
    QRunLoopThread(QObject *parent = 0);
    ~QRunLoopThread();
    bool keepRunning;
    void quit();

protected:
    void run();

private:
    void startNetworkChangeLoop();
    QMutex mutex;
    SCDynamicStoreRef storeSession;// = NULL;
    CFRunLoopSourceRef runloopSource;

private Q_SLOTS:
};

class QLangLoopThread : public QThread
{
    Q_OBJECT

public:
    QLangLoopThread(QObject *parent = 0);
    ~QLangLoopThread();
    bool keepRunning;
    void quit();

protected:
    void run();

private:
    QMutex mutex;

private Q_SLOTS:
};

class QDASessionThread : public QThread
{
    Q_OBJECT

public:
    QDASessionThread(QObject *parent = 0);
    ~QDASessionThread();
    bool keepRunning;
    void quit();
    DASessionRef session;
Q_SIGNALS:
    void storageAdded();
    void storageRemoved();

protected:
    void run();

private:
    QMutex mutex;

private Q_SLOTS:
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_MAC_P_H*/

// End of file

