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

#include "qsysteminfo_linux_common_p.h"
#include "qsysteminfo.h"
#include <qmobilityglobal.h>
#if !defined(QT_NO_DBUS)
#include <qhalservice_linux_p.h>

typedef enum
{
    NM_DEVICE_STATE_UNKNOWN = 0,
    NM_DEVICE_STATE_UNMANAGED,
    NM_DEVICE_STATE_UNAVAILABLE,
    NM_DEVICE_STATE_DISCONNECTED,
    NM_DEVICE_STATE_PREPARE,
    NM_DEVICE_STATE_CONFIG,
    NM_DEVICE_STATE_NEED_AUTH,
    NM_DEVICE_STATE_IP_CONFIG,
    NM_DEVICE_STATE_ACTIVATED,
    NM_DEVICE_STATE_FAILED
} NMDeviceState;

struct ProfileDataValue {
    QString key;
    QString val;
    QString type;
    };

Q_DECLARE_METATYPE(ProfileDataValue)
Q_DECLARE_METATYPE(QList<ProfileDataValue>)

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
    QSystemNetworkInfo::NetworkMode currentMode();
    void setWlanSignalStrengthCheckEnabled(bool enabled);

protected:
    void setupNetworkInfo();

private Q_SLOTS:
    void bluetoothNetworkStatusCheck();
#if defined(Q_WS_MAEMO_6)
    // Slots only available in Maemo6
    void slotSignalStrengthChanged(int percent, int dbm);
    void slotOperatorChanged(const QString &mnc, const QString &mcc);
    void slotOperatorNameChanged(const QString &name);
    void slotRegistrationChanged(const QString &status);
    void slotCellChanged(const QString &type, int id, int lac);
#endif

#if defined(Q_WS_MAEMO_5)
    // Slots only available in Maemo5
    void cellNetworkSignalStrengthChanged(uchar,uchar);
    void icdStatusChanged(QString,QString,QString,QString);
    void networkModeChanged(int);
    void operatorNameChanged(uchar,QString,QString,uint,uint);
    void registrationStatusChanged(uchar,ushort,uint,uint,uint,uchar,uchar);
#endif
    void usbCableAction();
    void wlanSignalStrengthCheck();

private:
    // The index of wanted argument in the QDBusMessage which is received as a
    // reply to the sent get_registration_status message via interface Phone.Net

    enum {                // In the received QDBusMessage..
        STATUS_INDEX = 0, // the original type of status argument is byte
        LAC_INDEX,        // the original type of lac argument is uint16
        CELLID_INDEX,     // the original type of cellId argument is uint32
        MNC_INDEX,        // the original type of mnc argument is uint32
        MCC_INDEX         // the original type of mcc argument is uint32
    };

    int cellSignalStrength;
    QSystemNetworkInfo::NetworkStatus currentBluetoothNetworkStatus;
    int currentCellId;
    int currentCellNetworkStatus;
    int currentEthernetSignalStrength;
    int currentLac;
    QString currentEthernetState;
    QString currentMCC;
    QString currentMNC;
    QString currentOperatorName;
    int currentWlanSignalStrength;
    int radioAccessTechnology;
    int iWlanStrengthCheckEnabled;
    QTimer *wlanSignalStrengthTimer;

    QMap<QString,int> csStatusMaemo6;
};

class QSystemDisplayInfoPrivate : public QSystemDisplayInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    int displayBrightness(int screen);
};

class QSystemStorageInfoPrivate : public QSystemStorageInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QSystemStorageInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

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

protected:
#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    void setupBluetooth();
    void setupProfile();

private Q_SLOTS:
    void halChanged(int,QVariantList);
    void bluezPropertyChanged(const QString&, QDBusVariant);
    void deviceModeChanged(QString newMode);
    void profileChanged(bool changed, bool active, QString profile, QList<ProfileDataValue> values);

private:
    bool flightMode;
    QString profileName;
    bool silentProfile;
    bool vibratingAlertEnabled;
    int ringingAlertVolume;

    QSystemDeviceInfo::PowerState previousPowerState;
#endif
};


class QSystemScreenSaverPrivate : public QSystemScreenSaverLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();

private Q_SLOTS:
    void wakeUpDisplay();

private:
    QTimer *ssTimer;
#if !defined(QT_NO_DBUS)
    QDBusInterface *mceConnectionInterface;
#endif
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMINFO_MAEMO_P_H*/

// End of file

