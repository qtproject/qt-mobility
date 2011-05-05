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
#ifndef QSYSTEMINFO_MAEMO_P_H
#define QSYSTEMINFO_MAEMO_P_H

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

#include "linux/qsysteminfo_linux_common_p.h"
#include "qsysteminfo.h"
#include <qmobilityglobal.h>
#if !defined(QT_NO_DBUS)
#include "linux/qhalservice_linux_p.h"

struct ProfileDataValue {
    QString key;
    QString val;
    QString type;
    };

Q_DECLARE_METATYPE(ProfileDataValue)
Q_DECLARE_METATYPE(QList<ProfileDataValue>)

#endif

#ifdef Q_USE_BME
extern "C" {
#include "bme/bmeipc.h"
}
#include <mqueue.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;
class QSystemInfoPrivate : public QSystemInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemInfoPrivate();

    QStringList availableLanguages() const;
    QString version(QSystemInfo::Version,  const QString &parameter = QString());
    virtual QString currentLanguage() const;
    QString currentCountryCode() const;

    bool hasFeatureSupported(QSystemInfo::Feature feature);
};

class QNetworkManagerInterface;
class QNetworkManagerInterfaceDeviceWired;
class QNetworkManagerInterfaceDeviceWireless;
class QNetworkManagerInterfaceAccessPoint;

class QSystemNetworkInfoPrivate : public QSystemNetworkInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    int cellId();
    QSystemNetworkInfo::CellDataTechnology cellDataTechnology();

    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();
    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();
    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);

#if defined(Q_WS_MAEMO_5)
    void setWlanSignalStrengthCheckEnabled(bool enabled);
#endif // Q_WS_MAEMO_5
    int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);

#if defined(Q_WS_MAEMO_6)
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
#endif // Q_WS_MAEMO_6

private Q_SLOTS:
    void updateAttachedDevices(QString device);
    void icdStatusChanged(QString,QString,QString,QString);
    void updateUsbCableStatus();
    void checkWlanSignalStrength();

#if defined(Q_WS_MAEMO_5)
    void cellNetworkSignalStrengthChanged(uchar,uchar);
    void networkModeChanged(int);
    void operatorNameChanged(uchar,QString,QString,uint,uint);
    void registrationStatusChanged(uchar,ushort,uint,uint,uint,uchar,uchar);
#endif // Q_WS_MAEMO_5

#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    void slotSignalStrengthChanged(int percent, int dbm);
    void slotOperatorChanged(const QString &mnc, const QString &mcc);
    void slotOperatorNameChanged(const QString &name);
    void slotRegistrationChanged(const QString &status);
    void slotCellChanged(const QString &type, int id, int lac);
    void slotCellDataTechnologyChanged(const QString &tech);
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

private:
    void setupNetworkInfo();
    void checkNetworkMode();
    QSystemNetworkInfo::CellDataTechnology csdtToCellDataTechnology(const QString &tech);

    QSystemNetworkInfo::NetworkStatus currentBluetoothNetworkStatus;
    int currentCellNetworkStatus;
    QSystemNetworkInfo::NetworkStatus currentWlanNetworkStatus;

    QSystemNetworkInfo::NetworkMode currentNetworkMode;

    int currentCellSignalStrength;
    int currentEthernetSignalStrength;
    int currentWlanSignalStrength;

    int currentCellId;
    int currentLac;
    int radioAccessTechnology;
    QSystemNetworkInfo::CellDataTechnology currentCellDataTechnology;
    QString currentMCC;
    QString currentMNC;
    QString currentOperatorName;

    QTimer *wlanSignalStrengthTimer;

#if defined(Q_WS_MAEMO_5)
    // The index of wanted argument in the QDBusMessage which is received as a
    // reply to the sent get_registration_status message via interface Phone.Net
    enum {                // In the received QDBusMessage..
        STATUS_INDEX = 0, // the original type of status argument is byte
        LAC_INDEX,        // the original type of lac argument is uint16
        CELLID_INDEX,     // the original type of cellId argument is uint32
        MNC_INDEX,        // the original type of mnc argument is uint32
        MCC_INDEX         // the original type of mcc argument is uint32
    };

    int iWlanStrengthCheckEnabled;
#endif // Q_WS_MAEMO_5

#if defined(Q_WS_MAEMO_6)
    QMap<QString, QVariant> queryCsdProperties(const QString &service, const QString &servicePath, const QString &interface);

    static QMap<QString, int> CellularServiceStatus;
#endif // Q_WS_MAEMO_6
};

class QSystemDisplayInfoPrivate : public QSystemDisplayInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    int displayBrightness(int screen);
    QSystemDisplayInfo::BacklightState backlightStatus(int screen);
};

class QSystemDeviceInfoPrivate : public QSystemDeviceInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent = 0);
    ~QSystemDeviceInfoPrivate();

    QString imei();
    QString imsi();
    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::PowerState currentPowerState();
    QSystemDeviceInfo::ThermalState currentThermalState();
    QString model();
    QString productName();
    bool isKeyboardFlippedOpen();//1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type);//1.2

    int messageRingtoneVolume();//1.2
    int voiceRingtoneVolume();//1.2
    bool vibrationActive();//1.2

    QSystemDeviceInfo::LockTypeFlags lockStatus();//1.2
    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2
    QByteArray uniqueDeviceID(); //1.2


Q_SIGNALS:
    void keyboardFlipped(bool open);

protected:

#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    void setupBluetooth();

private Q_SLOTS:
    void halChanged(int,QVariantList);
    void bluezPropertyChanged(const QString&, QDBusVariant);
    void deviceModeChanged(QString newMode);
    void profileChanged(bool changed, bool active, QString profile, QList<ProfileDataValue> values);
    void deviceStateChanged(int device, int state);
    void touchAndKeyboardStateChanged(const QString& state);

    void socketActivated(int);
private:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    void registerProfileDataMetaTypes();

    // profile data
    bool m_flightMode;
    QString m_profileName;
    bool m_silentProfile;
    bool m_vibratingAlertEnabled;
    bool m_beepProfile;
    int m_ringingAlertVolume;
    int m_smsAlertVolume;

    // profile data lazy-loading status
    bool m_flightModeQueried;
    bool m_profileNameQueried;
    bool m_ringingAlertTypeQueried;
    bool m_vibratingAlertEnabledQueried;
    bool m_ringingAlertVolumeQueried;
    bool m_smsAlertVolumeQueried;
    bool m_profileDataMetaTypesRegistered;

    // profile data accessors
    void queryRingingAlertType();
    bool flightMode();
    QString profileName();
    bool silentProfile();
    bool beepProfile();

    QSystemDeviceInfo::BatteryStatus currentBatStatus;

    QSystemDeviceInfo::PowerState previousPowerState;
    QSystemDeviceInfo::LockTypeFlags  currentLockType;
#endif
     QSocketNotifier *notifier;
     int gpioFD;
     int currentBatteryLevel;
};


class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    void setScreenSaverInhibited(bool on);

private Q_SLOTS:
    void wakeUpDisplay();

private:
    bool isInhibited;
    QTimer *ssTimer;
#if !defined(QT_NO_DBUS)
    QDBusInterface *mceConnectionInterface;
#endif
};

#ifdef Q_USE_BME
class EmIpc;
class EmEvents;
class EmCurrentMeasurement;
#endif

class QSystemBatteryInfoPrivate : public QSystemBatteryInfoLinuxCommonPrivate
{
    Q_OBJECT
public:
    QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent = 0);
    ~QSystemBatteryInfoPrivate();

    int currentFlow() const;

private Q_SLOTS:
#if !defined(QT_NO_DBUS)
    void halChangedMaemo(int,QVariantList);
#endif
#ifdef Q_USE_BME
    void onMeasurement(int socket);
#endif

private:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

#ifdef Q_USE_BME
    mutable bmestat_t bmeStat;
    mutable bool isDataActual;
    mutable QDateTime cacheExpire;

    mutable int coloumbCounterOffset;
    mutable int prevColoumbCounterRestartCount;

    void startMeasurements();
    void stopMeasurements();

    QScopedPointer<EmIpc> emIpc;
    QScopedPointer<EmEvents> emEvents;
    QScopedPointer<EmCurrentMeasurement> emCurrentMeasurements;
#endif
};


QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMINFO_MAEMO_P_H*/

// End of file

