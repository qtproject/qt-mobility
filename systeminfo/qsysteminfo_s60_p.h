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
#ifndef QSYSTEMINFO_S60_H
#define QSYSTEMINFO_S60_H


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

#include "qsysinfoglobal.h"
#include "qsysteminfo.h"

// For S60
#include <e32base.h>
#include <Etel3rdParty.h>
#include <featureinfo.h>
#include <mproengengine.h> 
#include <proengfactory.h>
#include <mproengprofile.h>
#include <MProEngProfileName.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;
class QSystemNetworkInfo;
class QTimer;
class QSystemInfo;
;
class CDeviceInfo;

class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();

// general

     //TODO: to Qt QLocale
    QString currentLanguage() const; // 2 letter ISO 639-1 //signal
    //TODO: to Qt QLocale
    QStringList availableLanguages() const; // 2 letter ISO 639-1
    //TODO: to Qt QLocale
    QString currentCountryCode() const; //2 letter ISO 3166-1
    
    QString version(QSystemInfo::Version type, const QString &parameter = QString());
    
    bool hasFeatureSupported(QSystemInfo::Feature feature);

Q_SIGNALS:
    void currentLanguageChanged(const QString &);
};

////////
class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    static int cellId();
    static int locationAreaCode();

    static QString currentMobileCountryCode(); // Mobile Country Code
    static QString currentMobileNetworkCode(); // Mobile Network Code

    static QString homeMobileCountryCode();
    static QString homeMobileNetworkCode();

    static QString networkName(); //signal
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);
};

////////
class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

     //TODO: to Qt QDesktopWidget
    static int displayBrightness(int screen);
     //TODO: to Qt QDesktopWidget
    static int colorDepth(int screen);
};

////////
class QSystemMemoryInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemMemoryInfoPrivate(QObject *parent = 0);
    virtual ~QSystemMemoryInfoPrivate();
    
    // memory
    qint64 totalDiskSpace(const QString &driveVolume);
    qint64 availableDiskSpace(const QString &driveVolume);
    QStringList listOfVolumes();
    QSystemMemoryInfo::VolumeType volumeType(const QString &driveVolume); //returns enum
};

////////
class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoPrivate();

    // device
    QString imei() const;
    QString imsi() const;
    QString manufacturer() const;
    QString model() const;
    QString productName() const;
    
    bool isBatteryCharging();
    int batteryLevel() const;

    QSystemDeviceInfo::InputMethodFlags inputMethodType();
    
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();
    
    QSystemDeviceInfo::PowerState currentPowerState();

Q_SIGNALS:
    void batteryLevelChanged(int);
    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void currentPowerStateChanged(QSystemDeviceInfo::PowerState);
    void bluetoothStateChanged(bool);

private:
    CDeviceInfo* iDeviceInfo;
    MProEngEngine* iProfileEngine;
    mutable int iError;
};

////////
class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    //virtual ~QSystemScreenSaverPrivate();

     //TODO: to Qt QDesktopWidget
    bool screenSaverEnabled();
     //TODO: to Qt QDesktopWidget
    bool screenBlankingEnabled();
     //TODO: to Qt QDesktopWidget
    bool setScreenSaverEnabled(bool b);
     //TODO: to Qt QDesktopWidget
    bool setScreenBlankingEnabled(bool b);
     //TODO: to Qt QDesktopWidget
    bool isScreenLockOn();

private:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;
};

class CDeviceInfo : public CActive
{

public:
    static CDeviceInfo* NewL();
    static CDeviceInfo* NewLC();
    ~CDeviceInfo();

    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    bool isBatteryCharging();
    TUint batteryLevel();

protected:
    CDeviceInfo();
    void ConstructL();
    void DoCancel();
    void RunL();

private:
    CTelephony* iTelephony;
    
    CTelephony::TBatteryInfoV1Pckg iBatteryInfoV1Pkg;
    CTelephony::TBatteryInfoV1 iBatteryInfoV1;
    
    CTelephony::TPhoneIdV1 iPhoneIdV1;
    CTelephony::TPhoneIdV1Pckg iPhoneIdV1Pkg;
        
    CTelephony::TSignalStrengthV1Pckg iSignalStrengthV1Pckg;
    CTelephony::TSignalStrengthV1 iSignalStrengthV1;
    
    CTelephony::TSubscriberIdV1Pckg iSubscriberIdV1Pckg;
    CTelephony::TSubscriberIdV1 iSubscriberIdV1;
    
    CTelephony::TNetworkRegistrationV1Pckg iNetworkRegistrationV1Pckg;
    CTelephony::TNetworkRegistrationV1 iNetworkRegistrationV1;
    
    CTelephony::TNetworkInfoV1Pckg iNetworkInfoV1Pckg;
    CTelephony::TNetworkInfoV1 iNetworkInfoV1;
    
    CTelephony::TIndicatorV1Pckg iIndicatorV1Pckg;
    CTelephony::TIndicatorV1 iIndicatorV1;
    
    CActiveSchedulerWait *iWait;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_S60_H*/

// End of file

