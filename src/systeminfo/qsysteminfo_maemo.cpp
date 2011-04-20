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
#include "qsysteminfocommon_p.h"
#include "qsysteminfo_maemo_p.h"
#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
//#include <QtGui>
#include <QDesktopWidget>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QTimer>
#include <QMapIterator>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#if !defined(Q_WS_MAEMO_6)
#if !defined(SW_KEYPAD_SLIDE)
#define SW_KEYPAD_SLIDE 0x0a
#endif
#endif

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)

#if !defined(QT_NO_DBUS)
#include "linux/gconfitem_p.h" // Temporarily here.
#endif

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>

#endif

#include <QDBusInterface>
#ifdef Q_USE_BME
extern "C" {
#include <errno.h>
#include <time.h>
#include "bme/bmeipc.h"
#include "bme/bmemsg.h"
#include "bme/em_isi.h"
}
#endif

static QString sysinfodValueForKey(const QString& key)
{
    QString value = "";
#if !defined(QT_NO_DBUS)
    QDBusInterface connectionInterface("com.nokia.SystemInfo",
                                       "/com/nokia/SystemInfo",
                                       "com.nokia.SystemInfo",
                                       QDBusConnection::systemBus());

    QDBusReply<QByteArray> reply = connectionInterface.call("GetConfigValue", key);
    if (reply.isValid()) {
        /*
         * sysinfod automatically terminates after some idle time (no D-Bus traffic).
         * Therefore, we cannot use isServiceRegistered() to determine if sysinfod is available.
         *
         * Thus, make a query to sysinfod and if we got back a valid reply, sysinfod
         * is available.
         */
        value = reply.value();
    }
#endif
    return value;
}

#if !defined(QT_NO_DBUS)
QDBusArgument &operator<<(QDBusArgument &argument, const ProfileDataValue &value)
{
  argument.beginStructure();
  argument << value.key << value.val << value.type;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ProfileDataValue &value)
{
  argument.beginStructure();
  argument >> value.key >> value.val >> value.type;
  argument.endStructure();
  return argument;
}
#endif

QTM_BEGIN_NAMESPACE

QSystemInfoPrivate::QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent)
    : QSystemInfoLinuxCommonPrivate(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    QStringList languages;

#if defined(Q_WS_MAEMO_6)
    QDir langDir("/etc/meego-supported-languages");
    languages = langDir.entryList(QStringList() << "??", QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
#else // Q_WS_MAEMO_6
    GConfItem languagesItem("/meegotouch/inputmethods/languages");
    const QStringList locales = languagesItem.value().toStringList();

    foreach (const QString &locale, locales)
        languages << locale.mid(0, 2);

    languages << currentLanguage();
    languages.removeDuplicates();
#endif // Q_WS_MAEMO_6

    return languages;
}

QString QSystemInfoPrivate::currentLanguage() const
{
#if defined(Q_WS_MAEMO_6)
    QString lang;
    if (currentLang.isEmpty()) {
        GConfItem langItem("/meegotouch/i18n/language");
        lang = langItem.value().toString();
        if (lang.count() > 2)
            lang = lang.left(2);
        if (lang.isEmpty())
            lang = QString::fromLocal8Bit(qgetenv("LANG")).left(2);
    } else {
        lang = currentLang;
    }

    return lang;
#else // Q_WS_MAEMO_6
    return QSystemInfoLinuxCommonPrivate::currentLanguage();
#endif // Q_WS_MAEMO_6
}

QString QSystemInfoPrivate::currentCountryCode() const
{
#if defined(Q_WS_MAEMO_6)
    GConfItem langItem("/meegotouch/i18n/region");
    QString langCC = langItem.value().toString().section("_", 1, 1);
    if (langCC.isEmpty()) {
        langCC = QString::fromLocal8Bit(qgetenv("LANG")).section("_", 1, 1);
        langCC = langCC.remove(".UTF-8", Qt::CaseSensitive);
    }
    return langCC;
#else // Q_WS_MAEMO_6
    return QSystemInfoLinuxCommonPrivate::currentCountryCode();
#endif // Q_WS_MAEMO_6
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type, const QString &parameter)
{
    switch(type) {
    case QSystemInfo::Os: {
        QString sysinfodValue = sysinfodValueForKey("/device/sw-release-ver"); //("/device/content-ver");
        if (!sysinfodValue.isEmpty()) {
            sysinfodValue =  sysinfodValue.section("_", 2, 4);
            return sysinfodValue;
        }
    }

    case QSystemInfo::Firmware: {
        QString sysinfodValue = sysinfodValueForKey("/device/sw-release-ver");
        if (!sysinfodValue.isEmpty())
            return sysinfodValue;
    }

    default:
        return QSystemInfoLinuxCommonPrivate::version(type, parameter);
    };
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;

    switch (feature) {
    case QSystemInfo::SimFeature: {
        QSystemDeviceInfoPrivate d;
        featureSupported = (d.simStatus() != QSystemDeviceInfo::SimNotAvailable);
        break;
    }

    case QSystemInfo::LocationFeature: {
#if defined(Q_WS_MAEMO_6)
        GConfItem satellitePositioning("/system/osso/location/settings/satellitePositioning");
        GConfItem networkPositioning("/system/osso/location/settings/networkPositioning");

        bool satellitePositioningAvailable = satellitePositioning.value(false).toBool();
        bool networkPositioningAvailable = networkPositioning.value(false).toBool();

        featureSupported = (satellitePositioningAvailable || networkPositioningAvailable);
#else // Q_WS_MAEMO_6
        GConfItem locationValues("/system/nokia/location");
        const QStringList locationKeys = locationValues.listEntries();
        if (locationKeys.count())
            featureSupported = true;
#endif // Q_WS_MAEMO_6
        break;
    }

    case QSystemInfo::HapticsFeature: {
        QHalInterface iface;
        const QStringList touchSupport(iface.findDeviceByCapability("input.touchpad"));
        if (touchSupport.count())
            featureSupported = true;
        else
            featureSupported = false;
        break;
    }

    default:
        featureSupported = QSystemInfoLinuxCommonPrivate::hasFeatureSupported(feature);
        break;
    };

    return featureSupported;
}

#if defined(Q_WS_MAEMO_6)
QMap<QString, int> QSystemNetworkInfoPrivate::CellularServiceStatus;
#endif // Q_WS_MAEMO_6

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
    : QSystemNetworkInfoLinuxCommonPrivate(parent)
    , currentBluetoothNetworkStatus(QSystemNetworkInfo::UndefinedStatus)
    , currentCellNetworkStatus(-1)
    , currentWlanNetworkStatus(QSystemNetworkInfo::UndefinedStatus)
    , currentNetworkMode(QSystemNetworkInfo::UnknownMode)
    , currentCellSignalStrength(-1)
    , currentEthernetSignalStrength(-1)
    , currentWlanSignalStrength(-1)
    , currentCellId(-1)
    , currentLac(-1)
    , radioAccessTechnology(0)
    , currentCellDataTechnology(QSystemNetworkInfo::UnknownDataTechnology)
    , wlanSignalStrengthTimer(0)
{
#if defined(Q_WS_MAEMO_6)
    if (CellularServiceStatus.isEmpty()) {
        CellularServiceStatus["Unknown"]    = -1;  // Current registration status is unknown.
        CellularServiceStatus["Home"]       = 0;   // Registered with the home network.
        CellularServiceStatus["Roaming"]    = 1;   // Registered with a roaming network.
        CellularServiceStatus["Offline"]    = 3;   // Not registered.
        CellularServiceStatus["Searching"]  = 4;   // Offline, but currently searching for network.
        CellularServiceStatus["NoSim"]      = 6;   // Offline because no SIM is present.
        CellularServiceStatus["PowerOff"]   = 8;   // Offline because the CS is powered off.
        CellularServiceStatus["PowerSave"]  = 9;   // Offline and in power save mode.
        CellularServiceStatus["NoCoverage"] = 10;  // Offline and in power save mode because of poor coverage.
        CellularServiceStatus["Rejected"]   = 11;  // Offline because SIM was rejected by the network.
    }
#endif // Q_WS_MAEMO_6

    setupNetworkInfo();
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
    delete wlanSignalStrengthTimer;
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode: {
        // radioAccessTechnology: 1 = GSM, 2 = WCDMA
        if ((radioAccessTechnology == 1 && mode != QSystemNetworkInfo::GsmMode)
            || (radioAccessTechnology == 2 && mode != QSystemNetworkInfo::WcdmaMode)) {
            return QSystemNetworkInfo::NoNetworkAvailable;
        }

#if defined(Q_WS_MAEMO_6)
        QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.NetworkRegistration").value("RegistrationStatus");
        if (value.isValid())
            currentCellNetworkStatus = CellularServiceStatus.value(value.toString(), -1);
#endif // Q_WS_MAEMO_6

        switch (currentCellNetworkStatus) {
        case  0: return QSystemNetworkInfo::HomeNetwork;        // CS is registered to home network
        case  1: return QSystemNetworkInfo::Roaming;            // CS is registered to some other network than home network
        case  2: return QSystemNetworkInfo::Roaming;            // CS is registered to non-home system in a non-home area
        case  3: return QSystemNetworkInfo::NoNetworkAvailable; // CS is not in service
        case  4: return QSystemNetworkInfo::Searching;          // CS is not in service, but is currently searching for service
        case  5: return QSystemNetworkInfo::NoNetworkAvailable; // CS is not in service and it is not currently searching for service
        case  6: return QSystemNetworkInfo::NoNetworkAvailable; // CS is not in service due to missing SIM or missing subscription
        case  8: return QSystemNetworkInfo::NoNetworkAvailable; // CS is in power off state
        case  9: return QSystemNetworkInfo::NoNetworkAvailable; // CS is in No Service Power Save State (currently not listening to any cell)
        case 10: return QSystemNetworkInfo::NoNetworkAvailable; // CS is in No Service Power Save State (CS is entered to this state
                                                                // because there is no network coverage)
        case 11: return QSystemNetworkInfo::Denied;             // CS is not in service due to missing subscription
        default:
            break;
        };
        break;
    }

    case QSystemNetworkInfo::EthernetMode:
        if (currentEthernetSignalStrength == -1)
            networkSignalStrength(mode);
        if (currentEthernetSignalStrength == 100)
            return QSystemNetworkInfo::Connected;
        else
            return QSystemNetworkInfo::NoNetworkAvailable;

    case QSystemNetworkInfo::WlanMode:
        currentWlanNetworkStatus = QSystemNetworkInfoLinuxCommonPrivate::networkStatus(mode);
        return currentWlanNetworkStatus;

    case QSystemNetworkInfo::BluetoothMode:
        currentBluetoothNetworkStatus = QSystemNetworkInfoLinuxCommonPrivate::networkStatus(mode);
        return currentBluetoothNetworkStatus;

//    case QSystemNetworkInfo::WimaxMode:
//    case QSystemNetworkInfo::LteMode:
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::networkStatus(mode);
    };

    return QSystemNetworkInfo::UndefinedStatus;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode: {
        // radioAccessTechnology: 1 = GSM, 2 = WCDMA
        if ((radioAccessTechnology == 1 && mode != QSystemNetworkInfo::GsmMode)
            || (radioAccessTechnology == 2 && mode != QSystemNetworkInfo::WcdmaMode)) {
            return -1;
        }

#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
        QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.SignalStrength").value("SignalPercent");
        if (value.isValid())
            currentCellSignalStrength = value.toInt();
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

        return currentCellSignalStrength;
    }

    case QSystemNetworkInfo::EthernetMode:
        currentEthernetSignalStrength = QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
        return currentEthernetSignalStrength;

    case QSystemNetworkInfo::WlanMode:
        currentWlanSignalStrength = QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
        return currentWlanSignalStrength;

//    case QSystemNetworkInfo::BluetoothMode:
//    case QSystemNetworkInfo::WimaxMode:
//    case QSystemNetworkInfo::LteMode:
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
    };

    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.NetworkCell").value("CellId");
    if (value.isValid())
        currentCellId = value.toInt();
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

    return currentCellId;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.NetworkCell").value("CellLac");
        if (value.isValid())
            currentLac = value.toInt();
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

    return currentLac;
}

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.NetworkOperator").value("OperatorMCC");
    if (value.isValid())
        currentMCC = value.toString();
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

    return currentMCC;
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.NetworkOperator").value("OperatorMNC");
    if (value.isValid())
        currentMNC = value.toString();
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

    return currentMNC;
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    QSystemDeviceInfoPrivate d;
    QString imsi = d.imsi();
    if (imsi.length() >= 3)
        return imsi.left(3);

    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    QDBusInterface connectionInterface("com.nokia.csd.SIM", "/com/nokia/csd/sim", "com.nokia.csd.SIM.Identity",
                                       QDBusConnection::systemBus(), this);
    QDBusMessage reply = connectionInterface.call(QLatin1String("GetHPLMN"));
    if (reply.errorName().isEmpty()) {
        QList<QVariant> args = reply.arguments();
        // The first attribute should be MCC and the 2nd one MNC
        if (args.size() == 2)
            return args.at(1).toString();
    }
#else // Q_WS_MAEMO_6
    QDBusInterface connectionInterface("com.nokia.phone.SIM",
                                       "/com/nokia/phone/SIM",
                                       "Phone.Sim",
                                       QDBusConnection::systemBus(), this);
    if (!connectionInterface.isValid()) {
        qDebug() << "interface not valid";
        return QString();
    }
    QDBusReply<QByteArray> reply = connectionInterface.call(QLatin1String("read_hplmn"));

    // The MNC and MCC are split into Hex numbers in the received byte array.
    // The MNC can be 2 or 3 digits long. If it is 2 digits long, it ends with 0xF.
    // The order of the Hex numbers in the reply is:
    // mcc2 mcc1 mnc3 mcc3 mnc2 mnc1
    QString homeMobileNetworkCode;
    if (reply.isValid()) {
        QString temp = reply.value().toHex();
        QString mnc1 = temp.right(1);
        temp.chop(1);
        QString mnc2 = temp.right(1);
        temp.chop(2);
        QString mnc3 = temp.right(1);
        if (mnc3 != "f")
            homeMobileNetworkCode.prepend(mnc3);
        homeMobileNetworkCode.prepend(mnc2);
        homeMobileNetworkCode.prepend(mnc1);
        return homeMobileNetworkCode;
    }
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

    return QString();
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode: {
        // radioAccessTechnology: 1 = GSM, 2 = WCDMA
        if ((radioAccessTechnology == 1 && mode != QSystemNetworkInfo::GsmMode)
            || (radioAccessTechnology == 2 && mode != QSystemNetworkInfo::WcdmaMode)) {
            break;
        }

#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
        QVariant value = queryCsdProperties("com.nokia.csd.CSNet", "/com/nokia/csd/csnet", "com.nokia.csd.CSNet.NetworkOperator").value("OperatorName");
        if (value.isValid())
            currentOperatorName = value.toString();
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

        return currentOperatorName;
    }

//    case QSystemNetworkInfo::WlanMode:
//    case QSystemNetworkInfo::EthernetMode:
//    case QSystemNetworkInfo::BluetoothMode:
//    case QSystemNetworkInfo::WimaxMode:
//    case QSystemNetworkInfo::LteMode:
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::networkName(mode);
    };

    return QString();
}

#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
QMap<QString, QVariant> QSystemNetworkInfoPrivate::queryCsdProperties(const QString &service, const QString &path, const QString &interface)
{
    QMap<QString, QVariant> properties;

    QDBusMessage message = QDBusMessage::createMethodCall(service, path,
                                                          "org.freedesktop.DBus.Properties", "GetAll");
    message << interface;

    QDBusReply<QMap<QString, QVariant> > reply = QDBusConnection::systemBus().call(message);
    if (reply.isValid())
        properties = reply.value();

    return properties;
}
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

void QSystemNetworkInfoPrivate::setupNetworkInfo()
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    const QString service("com.nokia.csd.CSNet");
    const QString path("/com/nokia/csd/csnet");

    // CSD: network cell
    QMap<QString, QVariant> properties = queryCsdProperties(service, path, "com.nokia.csd.CSNet.NetworkCell");

    QVariant value = properties.value("CellType");
    if (value.isValid()) {
        QString currentCellType = value.toString();
        if (currentCellType == "GSM")
            radioAccessTechnology = 1;
        else if (currentCellType == "WCDMA")
            radioAccessTechnology = 2;
    }
#else // Q_WS_MAEMO_6
    QDBusConnection systemDbusConnection = QDBusConnection::systemBus();
    iWlanStrengthCheckEnabled = 0;

    QDBusInterface connectionInterface("com.nokia.phone.net",
                                       "/com/nokia/phone/net",
                                       "Phone.Net",
                                       systemDbusConnection);
    if (!connectionInterface.isValid()) {
        qDebug() << "setupNetworkInfo(): interface not valid";
        return;
    }

    QDBusMessage reply = connectionInterface.call(QLatin1String("get_registration_status"));
    if (reply.type() == QDBusMessage::ReplyMessage) {
        QList<QVariant> argList = reply.arguments();
        currentCellNetworkStatus = argList.at(STATUS_INDEX).toInt();
        currentLac = argList.at(LAC_INDEX).value<ushort>();
        currentCellId = argList.at(CELLID_INDEX).value<uint>();
        currentMCC.setNum(argList.at(MCC_INDEX).value<uint>());
        currentMNC.setNum(argList.at(MNC_INDEX).value<uint>());
    } else {
        qDebug() << reply.errorMessage();
    }

    if (!systemDbusConnection.connect("com.nokia.phone.net",
                                      "/com/nokia/phone/net",
                                      "Phone.Net",
                                      "registration_status_change",
                                      this, SLOT(registrationStatusChanged(uchar,ushort,uint,uint,uint,uchar,uchar)))) {
        qDebug() << "unable to connect to registration_status_change";
    }

    reply = connectionInterface.call(QLatin1String("get_signal_strength"));
    if (reply.type() == QDBusMessage::ReplyMessage) {
        QList<QVariant> argList = reply.arguments();
        currentCellSignalStrength = argList.at(0).toInt();
    } else {
        qDebug() << reply.errorMessage();
    }

    if (!systemDbusConnection.connect("com.nokia.phone.net",
                                      "/com/nokia/phone/net",
                                      "Phone.Net",
                                      "signal_strength_change",
                                      this, SLOT(cellNetworkSignalStrengthChanged(uchar,uchar)))) {
        qDebug() << "unable to connect to signal_strength_change";
    }

    uchar type = 0;
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(type) << qVariantFromValue(currentMNC.toUInt()) << qVariantFromValue(currentMCC.toUInt());

    reply = connectionInterface.callWithArgumentList(QDBus::Block, QLatin1String("get_operator_name"), argumentList);
    if (reply.type() == QDBusMessage::ReplyMessage) {
        QList<QVariant> argList = reply.arguments();
        currentOperatorName = argList.at(0).toString();
    } else {
        qDebug() << reply.errorMessage();
    }

    if (!systemDbusConnection.connect("com.nokia.phone.net",
                                      "/com/nokia/phone/net",
                                      "Phone.Net",
                                      "operator_name_change",
                                      this, SLOT(operatorNameChanged(uchar,QString,QString,uint,uint)))) {
        qDebug() << "unable to connect to operator_name_change";
    }

    reply = connectionInterface.call(QLatin1String("get_radio_access_technology"));
    if (reply.type() == QDBusMessage::ReplyMessage) {
        QList<QVariant> argList = reply.arguments();
        radioAccessTechnology = argList.at(0).toInt();
    } else {
        qDebug() << reply.errorMessage();
    }

    if (!systemDbusConnection.connect("com.nokia.phone.net",
                                      "/com/nokia/phone/net",
                                      "Phone.Net",
                                      "radio_access_technology_change",
                                      this, SLOT(networkModeChanged(int)))) {
        qDebug() << "unable to connect to radio_access_technology_change";
    }

    // TODO optimize the performance by enabling lazy loading (already done for M6)
    // here I just get the signal strength so we can handle the changes
    currentEthernetSignalStrength = networkSignalStrength(QSystemNetworkInfo::EthernetMode);
    currentBluetoothNetworkStatus = networkStatus(QSystemNetworkInfo::BluetoothMode);
    currentWlanNetworkStatus = networkStatus(QSystemNetworkInfo::WlanMode);
    currentWlanSignalStrength = networkSignalStrength(QSystemNetworkInfo::WlanMode);
    currentNetworkMode = currentMode();
    wlanSignalStrengthTimer = new QTimer(this);
    connect(wlanSignalStrengthTimer, SIGNAL(timeout()), this, SLOT(checkWlanSignalStrength()));

    if (!systemDbusConnection.connect("com.nokia.bme",
                                      "/com/nokia/bme/signal",
                                      "com.nokia.bme.signal",
                                      QLatin1String("charger_connected"),
                                      this, SLOT(updateUsbCableStatus()))) {
        qDebug() << "unable to connect to updateUsbCableStatus (connect)";
    }

    if (!systemDbusConnection.connect("com.nokia.bme",
                                      "/com/nokia/bme/signal",
                                      "com.nokia.bme.signal",
                                      QLatin1String("charger_disconnected"),
                                      this, SLOT(updateUsbCableStatus()))) {
        qDebug() << "unable to connect to updateUsbCableStatus (disconnect)";
    }

    if (!systemDbusConnection.connect("org.freedesktop.Hal",
                                      "/org/freedesktop/Hal/Manager",
                                      "org.freedesktop.Hal.Manager",
                                      QLatin1String("DeviceAdded"),
                                      this, SLOT(updateAttachedDevices(QString)))) {
        qDebug() << "unable to connect to updateAttachedDevices (1)";
    }

    if (!systemDbusConnection.connect("org.freedesktop.Hal",
                                      "/org/freedesktop/Hal/Manager",
                                      "org.freedesktop.Hal.Manager",
                                      QLatin1String("DeviceRemoved"),
                                      this, SLOT(updateAttachedDevices(QString)))) {
        qDebug() << "unable to connect to updateAttachedDevices (2)";
    }

    if (!systemDbusConnection.connect("com.nokia.icd",
                                      "/com/nokia/icd",
                                      "com.nokia.icd",
                                      QLatin1String("status_changed"),
                                      this, SLOT(icdStatusChanged(QString,QString,QString,QString)))) {
        qDebug() << "unable to connect to icdStatusChanged";
    }
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS
}

#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
void QSystemNetworkInfoPrivate::connectNotify(const char *signal)
{
    const QString service("com.nokia.csd.CSNet");
    const QString path("/com/nokia/csd/csnet");

    if ((QLatin1String(signal) == SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)))) {
        networkSignalStrength(QSystemNetworkInfo::GsmMode);
        networkSignalStrength(QSystemNetworkInfo::EthernetMode);
        networkSignalStrength(QSystemNetworkInfo::WlanMode);
        wlanSignalStrengthTimer = new QTimer(this);
        wlanSignalStrengthTimer->start(5000);
        connect(wlanSignalStrengthTimer, SIGNAL(timeout()), this, SLOT(checkWlanSignalStrength()));

        if (!QDBusConnection::systemBus().connect(service, path, "com.nokia.csd.CSNet.SignalStrength", "SignalStrengthChanged",
                                                  this, SLOT(slotSignalStrengthChanged(int,int)))) {
            qDebug() << "unable to connect SignalStrengthChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(currentMobileCountryCodeChanged(QString)))
               || (QLatin1String(signal) == SIGNAL(currentMobileNetworkCodeChanged(QString)))) {
        currentMobileCountryCode();

        if (!QDBusConnection::systemBus().connect(service, path, "com.nokia.csd.CSNet.NetworkOperator", "OperatorChanged",
                                                  this, SLOT(slotOperatorChanged(QString,QString)))) {
            qDebug() << "unable to connect (OperatorChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)))) {
        networkName(QSystemNetworkInfo::GsmMode);

        if (!QDBusConnection::systemBus().connect(service, path, "com.nokia.csd.CSNet.NetworkOperator", "OperatorNameChanged",
                                                  this, SLOT(slotOperatorNameChanged(QString)))) {
            qDebug() << "unable to connect OperatorNameChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)))) {
        networkSignalStrength(QSystemNetworkInfo::EthernetMode);
        networkStatus(QSystemNetworkInfo::BluetoothMode);
        networkStatus(QSystemNetworkInfo::GsmMode);
        networkStatus(QSystemNetworkInfo::WlanMode);
        currentMode();

        if (!QDBusConnection::systemBus().connect("com.nokia.icd", "/com/nokia/icd", "com.nokia.icd", QLatin1String("status_changed"),
                                                  this, SLOT(icdStatusChanged(QString,QString,QString,QString)))) {
            qDebug() << "unable to connect to icdStatusChanged";
        }

        if (!QDBusConnection::systemBus().connect("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", QLatin1String("DeviceAdded"),
                                                  this, SLOT(updateAttachedDevices(QString)))) {
            qDebug() << "unable to connect to updateAttachedDevices (1)";
        }

        if (!QDBusConnection::systemBus().connect("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", QLatin1String("DeviceRemoved"),
                                                  this, SLOT(updateAttachedDevices(QString)))) {
            qDebug() << "unable to connect to updateAttachedDevices (2)";
        }

        if (!QDBusConnection::systemBus().connect(service, path, "com.nokia.csd.CSNet.NetworkRegistration", "RegistrationChanged",
                                                  this, SLOT(slotRegistrationChanged(QString)))) {
            qDebug() << "unable to connect RegistrationChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(cellIdChanged(int)))) {
        currentMode();
        cellId();

        if (!QDBusConnection::systemBus().connect(service, path, "com.nokia.csd.CSNet.NetworkCell", "CellChanged",
                                                  this, SLOT(slotCellChanged(QString,int,int)))) {
            qDebug() << "unable to connect CellChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology)))) {
        cellDataTechnology();

        if (!QDBusConnection::systemBus().connect(service, path, "com.nokia.csd.CSNet.RadioAccess", "DataTechnologyChanged",
                                                  this, SLOT(slotCellDataTechnologyChanged(QString)))) {
            qDebug() << "unable to connect DataTechnologyChanged";
        }
    }
}

void QSystemNetworkInfoPrivate::disconnectNotify(const char *signal)
{
    const QString service("com.nokia.csd.CSNet");
    const QString path("/com/nokia/csd/csnet");

    if ((QLatin1String(signal) == SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)))) {
        wlanSignalStrengthTimer->stop();
        if (!QDBusConnection::systemBus().disconnect(service, path, "com.nokia.csd.CSNet.SignalStrength", "SignalStrengthChanged",
                                                     this, SLOT(slotSignalStrengthChanged(int,int)))) {
            qDebug() << "unable to disconnect SignalStrengthChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(currentMobileCountryCodeChanged(QString)))
            || (QLatin1String(signal) == SIGNAL(currentMobileNetworkCodeChanged(QString)))) {
        if (!QDBusConnection::systemBus().disconnect(service, path, "com.nokia.csd.CSNet.NetworkOperator", "OperatorChanged",
                                                     this, SLOT(slotOperatorChanged(QString,QString)))) {
            qDebug() << "unable to disconnect (OperatorChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)))) {
        if (!QDBusConnection::systemBus().disconnect(service, path, "com.nokia.csd.CSNet.NetworkOperator", "OperatorNameChanged",
                                                     this, SLOT(slotOperatorNameChanged(QString)))) {
            qDebug() << "unable to disconnect OperatorNameChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)))) {
        if (!QDBusConnection::systemBus().disconnect(service, path, "com.nokia.csd.CSNet.NetworkRegistration", "RegistrationChanged",
                                                     this, SLOT(slotRegistrationChanged(QString)))) {
            qDebug() << "unable to disconnect RegistrationChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(cellIdChanged(int)))) {
        if (!QDBusConnection::systemBus().disconnect(service, path, "com.nokia.csd.CSNet.NetworkCell", "CellChanged",
                                                     this, SLOT(slotCellChanged(QString,int,int)))) {
            qDebug() << "unable to disconnect CellChanged";
        }
    } else if ((QLatin1String(signal) == SIGNAL(cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology)))) {
        if (!QDBusConnection::systemBus().disconnect(service, path, "com.nokia.csd.CSNet", "ActivityChanged",
                                          this, SLOT(slotCellDataTechnologyChanged(QString)))) {
            qDebug() << "unable to disconnect DataTechnologyChanged";
        }
    }
}

void QSystemNetworkInfoPrivate::slotSignalStrengthChanged(int signalStrength, int /*dbm*/)
{
    currentCellSignalStrength = signalStrength;

    if (radioAccessTechnology == 1)
        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::GsmMode, currentCellSignalStrength);
    else if (radioAccessTechnology == 2)
        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WcdmaMode, currentCellSignalStrength);
}

void QSystemNetworkInfoPrivate::slotOperatorChanged(const QString &mnc, const QString &mcc)
{
    if (currentMCC != mcc) {
        currentMCC = mcc;
        Q_EMIT currentMobileCountryCodeChanged(currentMCC);
    }

    if (currentMNC != mnc) {
        currentMNC = mnc;
        Q_EMIT currentMobileNetworkCodeChanged(currentMNC);
    }
}

void QSystemNetworkInfoPrivate::slotOperatorNameChanged(const QString &name)
{
    currentOperatorName = name;

    if (radioAccessTechnology == 1)
        Q_EMIT networkNameChanged(QSystemNetworkInfo::GsmMode, currentOperatorName);
    else if (radioAccessTechnology == 2)
        Q_EMIT networkNameChanged(QSystemNetworkInfo::WcdmaMode, currentOperatorName);
}

void QSystemNetworkInfoPrivate::slotRegistrationChanged(const QString &status)
{
    int newCellNetworkStatus = CellularServiceStatus.value(status, -1);

    if (currentCellNetworkStatus != newCellNetworkStatus) {
        currentCellNetworkStatus = newCellNetworkStatus;
        if (radioAccessTechnology == 1)
            Q_EMIT networkStatusChanged(QSystemNetworkInfo::GsmMode, networkStatus(QSystemNetworkInfo::GsmMode));
        else if (radioAccessTechnology == 2)
            Q_EMIT networkStatusChanged(QSystemNetworkInfo::WcdmaMode, networkStatus(QSystemNetworkInfo::WcdmaMode));
    }
}

void QSystemNetworkInfoPrivate::slotCellChanged(const QString &type, int id, int lac)
{
    int newRadioAccessTechnology = 0;
    if (type == "GSM")
        newRadioAccessTechnology = 1;
    else if (type == "WCDMA")
        newRadioAccessTechnology = 2;

    if (radioAccessTechnology != newRadioAccessTechnology) {
        radioAccessTechnology = newRadioAccessTechnology;
        checkNetworkMode();
    }

    if (currentCellId != id) {
        currentCellId = id;
        Q_EMIT cellIdChanged(currentCellId);
    }

    if (currentLac != lac) {
        currentLac = lac;
    }
}

void QSystemNetworkInfoPrivate::slotCellDataTechnologyChanged(const QString &tech)
{
    // TODO don't call cellDataTechnology() here
    Q_UNUSED(tech);

    QSystemNetworkInfo::CellDataTechnology oldTech = currentCellDataTechnology;

    if (oldTech != cellDataTechnology())
        Q_EMIT cellDataTechnologyChanged(currentCellDataTechnology);
}
#endif // Q_WS_MAEMO_6
#endif // QT_NO_DBUS

#if defined(Q_WS_MAEMO_5)
void QSystemNetworkInfoPrivate::cellNetworkSignalStrengthChanged(uchar var1, uchar)
{
    currentCellSignalStrength = var1;

    if (radioAccessTechnology == 1)
        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::GsmMode, currentCellSignalStrength);
    else if (radioAccessTechnology == 2)
        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WcdmaMode, currentCellSignalStrength);
}

void QSystemNetworkInfoPrivate::networkModeChanged(int newRadioAccessTechnology)
{
    radioAccessTechnology = newRadioAccessTechnology;
    checkNetworkMode();
}

void QSystemNetworkInfoPrivate::operatorNameChanged(uchar, QString name, QString, uint, uint)
{
    currentOperatorName = name;

    if (radioAccessTechnology == 1)
        Q_EMIT networkNameChanged(QSystemNetworkInfo::GsmMode, currentOperatorName);
    else if (radioAccessTechnology == 2)
        Q_EMIT networkNameChanged(QSystemNetworkInfo::WcdmaMode, currentOperatorName);
}

void QSystemNetworkInfoPrivate::registrationStatusChanged(uchar var1, ushort var2, uint var3, uint var4, uint var5, uchar, uchar)
{
    int newCellNetworkStatus = var1;
    int newLac = var2;
    int newCellId = var3;
    QString newMobileCountryCode;
    QString newMobileNetworkCode;
    newMobileCountryCode.setNum(var5);
    newMobileNetworkCode.setNum(var4);

    if (currentCellNetworkStatus != newCellNetworkStatus) {
        currentCellNetworkStatus = newCellNetworkStatus;
        if (radioAccessTechnology == 1)
            Q_EMIT networkStatusChanged(QSystemNetworkInfo::GsmMode, networkStatus(QSystemNetworkInfo::GsmMode));
        else if (radioAccessTechnology == 2)
            Q_EMIT networkStatusChanged(QSystemNetworkInfo::WcdmaMode, networkStatus(QSystemNetworkInfo::WcdmaMode));
    }

    if (currentLac != newLac)
        currentLac = newLac;

    if (currentCellId != newCellId) {
        currentCellId = newCellId;
        Q_EMIT cellIdChanged(newCellId);
    }

    if (currentMCC != newMobileCountryCode) {
        currentMCC = newMobileCountryCode;
        Q_EMIT currentMobileCountryCodeChanged(currentMCC);
    }

    if (currentMNC != newMobileNetworkCode) {
        currentMNC = newMobileNetworkCode;
        Q_EMIT currentMobileNetworkCodeChanged(currentMNC);
    }
}
#endif // Q_WS_MAEMO_5

void QSystemNetworkInfoPrivate::icdStatusChanged(QString, QString var2, QString, QString)
{
    if (var2 == "WLAN_INFRA") {
        // TODO don't call networkStatus() here
        QSystemNetworkInfo::NetworkStatus oldWlanNetworkStatus = currentWlanNetworkStatus;
        networkStatus(QSystemNetworkInfo::WlanMode);

        if (currentWlanNetworkStatus != oldWlanNetworkStatus) {
            Q_EMIT networkStatusChanged(QSystemNetworkInfo::WlanMode, currentWlanNetworkStatus);
            checkNetworkMode();
        }
    }
}

void QSystemNetworkInfoPrivate::updateUsbCableStatus()
{
    // TODO don't call networkSignalStrength() here
    int oldEthernetSignalStrength = currentEthernetSignalStrength;
    networkSignalStrength(QSystemNetworkInfo::EthernetMode);

    if (currentEthernetSignalStrength != oldEthernetSignalStrength) {
        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::EthernetMode, currentEthernetSignalStrength);
        Q_EMIT networkStatusChanged(QSystemNetworkInfo::EthernetMode, networkStatus(QSystemNetworkInfo::EthernetMode));
        checkNetworkMode();
    }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    if (networkStatus(QSystemNetworkInfo::EthernetMode) == QSystemNetworkInfo::Connected)
        currentNetworkMode = QSystemNetworkInfo::EthernetMode;
    else if (networkStatus(QSystemNetworkInfo::WlanMode) == QSystemNetworkInfo::Connected)
        currentNetworkMode = QSystemNetworkInfo::WlanMode;
    else if (radioAccessTechnology == 1)
        currentNetworkMode = QSystemNetworkInfo::GsmMode;
    else if (radioAccessTechnology == 2)
        currentNetworkMode = QSystemNetworkInfo::WcdmaMode;

    return currentNetworkMode;
}

void QSystemNetworkInfoPrivate::checkNetworkMode()
{
    QSystemNetworkInfo::NetworkMode oldNetworkMode = currentNetworkMode;
    currentMode();

    if (currentNetworkMode != oldNetworkMode)
        Q_EMIT networkModeChanged(currentNetworkMode);
}

void QSystemNetworkInfoPrivate::checkWlanSignalStrength()
{
    int oldWlanSignalStrength = currentWlanSignalStrength;
    networkSignalStrength(QSystemNetworkInfo::WlanMode);

    if (currentWlanSignalStrength != oldWlanSignalStrength)
        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, currentWlanSignalStrength);
}

void QSystemNetworkInfoPrivate::updateAttachedDevices(QString device)
{
    if (device == "/org/freedesktop/Hal/devices/net_1b") {
        QTimer::singleShot(500, this, SLOT(updateUsbCableStatus()));
    } else {
        QSystemNetworkInfo::NetworkStatus oldBluetoothNetworkStatus = currentBluetoothNetworkStatus;
        networkStatus(QSystemNetworkInfo::BluetoothMode);

        if (currentBluetoothNetworkStatus != oldBluetoothNetworkStatus)
            Q_EMIT networkStatusChanged(QSystemNetworkInfo::BluetoothMode, currentBluetoothNetworkStatus);
    }
}

#if defined(Q_WS_MAEMO_5)
void QSystemNetworkInfoPrivate::setWlanSignalStrengthCheckEnabled(bool enabled)
{
    if (enabled) {
        iWlanStrengthCheckEnabled++;
        if (!wlanSignalStrengthTimer->isActive())
            wlanSignalStrengthTimer->start(5000); //5 seconds interval
    } else {
        iWlanStrengthCheckEnabled--;
        if (iWlanStrengthCheckEnabled <= 0) {
            if (wlanSignalStrengthTimer->isActive())
                wlanSignalStrengthTimer->stop();
        }
    }
}
#endif // Q_WS_MAEMO_5

inline QSystemNetworkInfo::CellDataTechnology QSystemNetworkInfoPrivate::csdtToCellDataTechnology(const QString &tech)
{
    QSystemNetworkInfo::CellDataTechnology cdt = QSystemNetworkInfo::UnknownDataTechnology;
    if (tech == "GPRS")
        cdt = QSystemNetworkInfo::GprsDataTechnology;
    else if (tech == "EGPRS")
        cdt = QSystemNetworkInfo::EdgeDataTechnology;
    else if (tech == "UMTS")
        cdt = QSystemNetworkInfo::UmtsDataTechnology;
    else if (tech == "HSPA")
        cdt = QSystemNetworkInfo::HspaDataTechnology;
    return cdt;
}

QSystemNetworkInfo::CellDataTechnology QSystemNetworkInfoPrivate::cellDataTechnology()
{
#if !defined(QT_NO_DBUS)
    const QString service("com.nokia.csd.CSNet");
    const QString path("/com/nokia/csd/csnet");

    QDBusInterface csnetInterface(service, path, "com.nokia.csd.CSNet", QDBusConnection::systemBus());
    QVariant dataTechnology;
    QDBusInterface radioAccessInterface(service, path, "com.nokia.csd.CSNet.RadioAccess", QDBusConnection::systemBus());
    if (csnetInterface.property("Activity").toString() == "PacketData")
        dataTechnology = radioAccessInterface.property("DataTechnology");
    else
        dataTechnology = radioAccessInterface.property("Technology");

    if (dataTechnology.isValid())
        currentCellDataTechnology = csdtToCellDataTechnology(dataTechnology.toString());

    return currentCellDataTechnology;
#endif // QT_NO_DBUS
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent)
    : QSystemDisplayInfoLinuxCommonPrivate(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int screen)
{
    QDesktopWidget wid;
    if (wid.screenCount() - 1 < screen)
        return -1;

    GConfItem currentBrightness("/system/osso/dsm/display/display_brightness");
    GConfItem maxBrightness("/system/osso/dsm/display/max_display_brightness_levels");
    if (maxBrightness.value().toInt()) {
        float retVal = 100 * (currentBrightness.value().toFloat() / maxBrightness.value().toFloat());
        return retVal;
    }

    return -1;
}

QSystemDisplayInfo::BacklightState QSystemDisplayInfoPrivate::backlightStatus(int screen)
{
    Q_UNUSED(screen)

    QSystemDisplayInfo::BacklightState backlightState = QSystemDisplayInfo::BacklightStateUnknown;

#if !defined(QT_NO_DBUS)
    QDBusReply<QString> reply = QDBusConnection::systemBus().call(
                QDBusMessage::createMethodCall("com.nokia.mce", "/com/nokia/mce/request",
                                               "com.nokia.mce.request", "get_display_status"));
    if (reply.isValid()) {
        QString displayStatus = reply.value();
        if (displayStatus == "off")
            backlightState = QSystemDisplayInfo::BacklightStateOff;
        else if (displayStatus == "dimmed")
            backlightState = QSystemDisplayInfo::BacklightStateDimmed;
        else if (displayStatus == "on")
            backlightState = QSystemDisplayInfo::BacklightStateOn;
    }
#endif // QT_NO_DBUS

    return backlightState;
}

QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent)
        : QSystemDeviceInfoLinuxCommonPrivate(parent), gpioFD(-1)
{
    previousPowerState = QSystemDeviceInfo::UnknownPower;

    m_profileName                    = "";
    m_flightMode                     = false;
    m_silentProfile                  = false;
    m_vibratingAlertEnabled          = false;
    m_beepProfile                    = false;
    m_ringingAlertVolume             = 0;
    m_smsAlertVolume                 = 0;

    m_flightModeQueried              = false;
    m_profileNameQueried             = false;
    m_ringingAlertTypeQueried        = false;
    m_vibratingAlertEnabledQueried   = false;
    m_ringingAlertVolumeQueried      = false;
    m_smsAlertVolumeQueried          = false;

    m_profileDataMetaTypesRegistered = false;
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    if (gpioFD == -1) {
        ::close(gpioFD);
        gpioFD = -1;
    }
}

void QSystemDeviceInfoPrivate::registerProfileDataMetaTypes()
{
    if (m_profileDataMetaTypesRegistered) {
        return;
    }

#if !defined(QT_NO_DBUS)
    qDBusRegisterMetaType<ProfileDataValue>();
    qDBusRegisterMetaType<QList<ProfileDataValue> >();
#endif

    m_profileDataMetaTypesRegistered = true;
}

void QSystemDeviceInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags))))) {
        QDBusConnection::systemBus().connect("com.nokia.mce",
                                             "/com/nokia/mce/signal",
                                             "com.nokia.mce.signal",
                                             "tklock_mode_ind",
                                             this, SLOT(touchAndKeyboardStateChanged(const QString&)));
        QDBusConnection::systemBus().connect("com.nokia.devicelock",
                                             "/request",
                                             "com.nokia.devicelock",
                                             "stateChanged",
                                             this, SLOT(deviceStateChanged(int,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile))))) {
        registerProfileDataMetaTypes();

        if (! QDBusConnection::systemBus().connect("com.nokia.mce",
                               "/com/nokia/mce/signal",
                               "com.nokia.mce.signal",
                               "sig_device_mode_ind",
                               this, SLOT(deviceModeChanged(QString)))) {
            qDebug() << "unable to connect to sig_device_mode_ind";
        }
        if (!QDBusConnection::sessionBus().connect("com.nokia.profiled",
                               "/com/nokia/profiled",
                               "com.nokia.profiled",
                               "profile_changed",
                               this, SLOT(profileChanged(bool, bool, QString, QList<ProfileDataValue>)))) {
            qDebug() << "unable to connect to profile_changed";
        }
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(keyboardFlipped(bool))))) {
        if (gpioFD == -1) {
            gpioFD = ::open("/dev/input/gpio-keys", O_RDONLY | O_NONBLOCK);
        }

        if (gpioFD != -1) {
            notifier = new QSocketNotifier(gpioFD, QSocketNotifier::Read);
            connect(notifier, SIGNAL(activated(int)), this, SLOT(socketActivated(int)));
        } else {
            qDebug() << "Could not open gpiokeys";
            notifier = 0;
        }
    }
    QSystemDeviceInfoLinuxCommonPrivate::connectNotify(signal);
}

void QSystemDeviceInfoPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags))))) {
        QDBusConnection::systemBus().disconnect("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal", "tklock_mode_ind",
                                                this, SLOT(touchAndKeyboardStateChanged(const QString&)));
        QDBusConnection::systemBus().disconnect("com.nokia.devicelock", "/request", "com.nokia.devicelock", "stateChanged",
                                                this, SLOT(deviceStateChanged(int,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile))))) {
        QDBusConnection::systemBus().disconnect("com.nokia.mce",
                               "/com/nokia/mce/signal",
                               "com.nokia.mce.signal",
                               "sig_device_mode_ind",
                               this, SLOT(deviceModeChanged(QString)));
        QDBusConnection::sessionBus().disconnect("com.nokia.profiled",
                               "/com/nokia/profiled",
                               "com.nokia.profiled",
                               "profile_changed",
                               this, SLOT(profileChanged(bool, bool, QString, QList<ProfileDataValue>)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(keyboardFlipped(bool))))) {
        if (gpioFD != -1) {
            ::close(gpioFD);
            gpioFD = -1;
        }
    }
    QSystemDeviceInfoLinuxCommonPrivate::disconnectNotify(signal);
}

#if !defined(QT_NO_DBUS)
void QSystemDeviceInfoPrivate::halChanged(int,QVariantList map)
{
    for(int i=0; i < map.count(); i++) {
       if(map.at(i).toString() == "battery.charge_level.percentage") {
            int level = batteryLevel();
            if(currentBatteryLevel != level) {
                currentBatteryLevel = level;
                emit batteryLevelChanged(level);
            }
            QSystemDeviceInfo::BatteryStatus stat = QSystemDeviceInfo::NoBatteryLevel;

            if(level < 4) {
                stat = QSystemDeviceInfo::BatteryCritical;
            } else if(level < 11) {
                stat = QSystemDeviceInfo::BatteryVeryLow;
            } else if(level < 41) {
                stat = QSystemDeviceInfo::BatteryLow;
            } else if(level > 40) {
                stat = QSystemDeviceInfo::BatteryNormal;
            }
            if(currentBatStatus != stat) {
                currentBatStatus = stat;
                Q_EMIT batteryStatusChanged(stat);
            }
        }
        if((map.at(i).toString() == "maemo.charger.connection_status")
        || (map.at(i).toString() == "maemo.rechargeable.charging_status")) {
            QSystemDeviceInfo::PowerState state = currentPowerState();
            if (previousPowerState != state)
                emit powerStateChanged(state);
            previousPowerState = state;
       }
    } //end map
}
#endif

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
#if !defined(QT_NO_DBUS)
    if (flightMode())
        return QSystemDeviceInfo::OfflineProfile;

    if (silentProfile())
        return vibrationActive() ? QSystemDeviceInfo::VibProfile : QSystemDeviceInfo::SilentProfile;

    if (beepProfile())
        return QSystemDeviceInfo::BeepProfile;

    if (voiceRingtoneVolume() > 75)
        return QSystemDeviceInfo::LoudProfile;

    return QSystemDeviceInfo::NormalProfile;
#endif

    return QSystemDeviceInfo::UnknownProfile;
}

QString QSystemDeviceInfoPrivate::imei()
{
#if !defined(QT_NO_DBUS)
    #if defined(Q_WS_MAEMO_6)
        QString dBusService = "com.nokia.csd.Info";
    #else
        /* Maemo 5 */
        QString dBusService = "com.nokia.phone.SIM";
    #endif
    QDBusInterface connectionInterface(dBusService,
                                       "/com/nokia/csd/info",
                                       "com.nokia.csd.Info",
                                        QDBusConnection::systemBus(), this);
    QDBusReply< QString > reply = connectionInterface.call("GetIMEINumber");
    return reply.value();
#endif
    return "";
}

QString QSystemDeviceInfoPrivate::imsi()
{
#if defined(Q_WS_MAEMO_6)
    /* Maemo 6 */
    #if !defined(QT_NO_DBUS)
        QDBusInterface connectionInterface("com.nokia.csd.SIM",
                                           "/com/nokia/csd/sim",
                                           "com.nokia.csd.SIM.Identity",
                                           QDBusConnection::systemBus(), this);
        QDBusReply< QString > reply = connectionInterface.call("GetIMSI");
        return reply.value();
    #endif
    return "";
#else
    /* Maemo 5 */
    return GConfItem("/system/nokia/location/sim_imsi").value().toString();
#endif
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    QSystemDeviceInfo::SimStatus simStatus = QSystemDeviceInfo::SimNotAvailable;
    QString imsi = QSystemDeviceInfoPrivate::imsi();
    if (imsi.length() > 0) {
        simStatus = QSystemDeviceInfo::SingleSimAvailable;
    }
    return simStatus;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
#if !defined(QT_NO_DBUS)
    QDBusConnection systemDbusConnection = QDBusConnection::systemBus();

    QDBusInterface mceConnectionInterface("com.nokia.mce",
                                      "/com/nokia/mce/request",
                                      "com.nokia.mce.request",
                                      systemDbusConnection, this);
    if (mceConnectionInterface.isValid()) {
        QDBusReply<QString> tkLockModeReply = mceConnectionInterface.call("get_tklock_mode");
        return tkLockModeReply.value() == "locked";
    }

    qDebug() << "mce interface not valid";
#endif
    return false;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    if (ifaceDevice.getPropertyString("maemo.charger.connection_status") == "connected") {
                        if (ifaceDevice.getPropertyString("maemo.rechargeable.charging_status") == "full")
                            return QSystemDeviceInfo::WallPower;
                        return QSystemDeviceInfo::WallPowerChargingBattery;
                    }
                    return QSystemDeviceInfo::BatteryPower;
                }
            }
        }
#endif
    return QSystemDeviceInfo::UnknownPower;
}

QSystemDeviceInfo::ThermalState QSystemDeviceInfoPrivate::currentThermalState()
{
#if !defined(QT_NO_DBUS)
    QString dBusService = "com.nokia.thermalmanager";
    QDBusReply<QString> thermalStateReply = QDBusConnection::systemBus().call
            (QDBusMessage::createMethodCall("com.nokia.thermalmanager",
                                            "/com/nokia/thermalmanager",
                                            "com.nokia.thermalmanager",
                                            "get_thermal_state"));
    if (thermalStateReply.isValid()){
        if (thermalStateReply.value() == "normal"){
            return QSystemDeviceInfo::NormalThermal;
        }
        if (thermalStateReply.value() == "warning"){
            return QSystemDeviceInfo::WarningThermal;
        }
        if (thermalStateReply.value() == "alert"){
            return QSystemDeviceInfo::AlertThermal;
        }
        if (thermalStateReply.value() == "unknown"){
            return QSystemDeviceInfo::UnknownThermal;
        } else {
            return QSystemDeviceInfo::ErrorThermal;
        }
    }
#endif
     return QSystemDeviceInfo::UnknownThermal;
}

#if !defined(QT_NO_DBUS)
 void QSystemDeviceInfoPrivate::setupBluetooth()
 {
     QDBusInterface *connectionInterface;
     connectionInterface = new QDBusInterface("org.bluez",
                                              "/",
                                              "org.bluez.Manager",
                                              QDBusConnection::systemBus(), this);
     if (connectionInterface->isValid()) {

         QDBusReply<  QDBusObjectPath > reply = connectionInterface->call("DefaultAdapter");
         if (reply.isValid()) {
             QDBusInterface *adapterInterface;
             adapterInterface = new QDBusInterface("org.bluez",
                                                   reply.value().path(),
                                                   "org.bluez.Adapter",
                                                   QDBusConnection::systemBus(), this);
             if (adapterInterface->isValid()) {
                 if (!QDBusConnection::systemBus().connect("org.bluez",
                                           reply.value().path(),
                                            "org.bluez.Adapter",
                                            "PropertyChanged",
                                            this,SLOT(bluezPropertyChanged(QString, QDBusVariant)))) {
                     qDebug() << "bluez could not connect signal";
                 }
             }
         }
     }
 }
#endif

#if !defined(QT_NO_DBUS)
void QSystemDeviceInfoPrivate::bluezPropertyChanged(const QString &name, QDBusVariant value)
{
    if (name == "Powered")
        emit bluetoothStateChanged(value.variant().toBool());
}
#endif

#if !defined(QT_NO_DBUS)

void QSystemDeviceInfoPrivate::queryRingingAlertType()
{
    if (m_ringingAlertTypeQueried) {
        return;
    }

    QDBusMessage ringingAlertTypeMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                      "com.nokia.profiled", "get_value");
    ringingAlertTypeMsg << profileName();
    ringingAlertTypeMsg << "ringing.alert.type";

    QDBusReply<QString> ringingAlertTypeReply = QDBusConnection::sessionBus().call(ringingAlertTypeMsg);

    if (ringingAlertTypeReply.isValid()) {
        m_silentProfile = QString::compare(ringingAlertTypeReply.value(), "silent", Qt::CaseInsensitive) == 0;
        m_beepProfile = QString::compare(ringingAlertTypeReply.value(), "beep", Qt::CaseInsensitive) == 0;
        m_ringingAlertTypeQueried = true;
    }
}

bool QSystemDeviceInfoPrivate::flightMode()
{
   if (m_flightModeQueried) {
       return m_flightMode;
   }

   QDBusReply<quint32> radioStatesReply = QDBusConnection::systemBus().call(
                QDBusMessage::createMethodCall("com.nokia.mce",  "/com/nokia/mce/request",
                                               "com.nokia.mce.request", "get_radio_states"));
    if (radioStatesReply.isValid()) {
        quint32 radioStateFlags = radioStatesReply.value();
#define MCE_RADIO_STATE_WLAN            (1 << 2)
#define MCE_RADIO_STATE_BLUETOOTH       (1 << 3)

        m_flightMode = !(radioStateFlags & ~(MCE_RADIO_STATE_WLAN | MCE_RADIO_STATE_BLUETOOTH));
        m_flightModeQueried = true;
    }
    return m_flightMode;
}

QString QSystemDeviceInfoPrivate::profileName()
{
    if (m_profileNameQueried) {
        return m_profileName;
    }

    QDBusReply<QString> profileNameReply = QDBusConnection::sessionBus().call(
                QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                               "com.nokia.profiled", "get_profile"));

    if (profileNameReply.isValid()) {
        m_profileName = profileNameReply.value();
        m_profileNameQueried = true;
    }
    return m_profileName;
}

bool QSystemDeviceInfoPrivate::silentProfile()
{
    if (!m_ringingAlertTypeQueried) {
        queryRingingAlertType();
    }
    return m_silentProfile;
}

bool QSystemDeviceInfoPrivate::beepProfile()
{
    if (!m_ringingAlertTypeQueried) {
        queryRingingAlertType();
    }
    return m_beepProfile;
}

void QSystemDeviceInfoPrivate::deviceModeChanged(QString newMode)
{
    bool previousFlightMode = m_flightMode;
    m_flightMode = newMode == "flight";
    m_flightModeQueried = true;
    if (previousFlightMode != m_flightMode)
        emit currentProfileChanged(currentProfile());
}

void QSystemDeviceInfoPrivate::profileChanged(bool changed, bool active, QString profile, QList<ProfileDataValue> values)
{
    if (active) {
        m_profileName = profile;
        m_profileNameQueried = true;

        foreach (const ProfileDataValue &value, values) {
            if (value.key == "ringing.alert.type") {
                m_silentProfile = QString::compare(value.val, "silent", Qt::CaseInsensitive) == 0;
                m_beepProfile = QString::compare(value.val, "beep", Qt::CaseInsensitive) == 0;
                m_ringingAlertTypeQueried = true;
            } else if (value.key == "vibrating.alert.enabled") {
                m_vibratingAlertEnabled = QString::compare(value.val, "On", Qt::CaseInsensitive) == 0;
                m_vibratingAlertEnabledQueried = true;
            } else if (value.key == "ringing.alert.volume") {
                m_ringingAlertVolume = value.val.toInt();
                m_ringingAlertVolumeQueried = true;
            } else if (value.key == "sms.alert.volume") {
                m_smsAlertVolume = value.val.toInt();
                m_smsAlertVolumeQueried = true;
            }
        }
        if (changed)
            emit currentProfileChanged(currentProfile());
    }
}

QString QSystemDeviceInfoPrivate::model()
{
#if !defined(QT_NO_DBUS)
    QString product = sysinfodValueForKey("/component/product");
    if (!product.isEmpty()) {
        return product;
    }
#endif
    return QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
#if !defined(QT_NO_DBUS)
    QString productName = sysinfodValueForKey("/component/product-name");
    if (!productName.isEmpty()) {
        return productName;
    }
#endif
    return QString();
}

#endif

bool QSystemDeviceInfoPrivate::isKeyboardFlippedOpen()
{
    bool keyboardFlippedOpen = false;
    unsigned long bits[NBITS(KEY_MAX)] = {0}; /* switch state bits */
    int eventFd = ::open("/dev/input/gpio-keys", O_RDONLY | O_NONBLOCK);

    if ((eventFd != -1) && (ioctl(eventFd, EVIOCGSW(KEY_MAX), bits) != -1)) {
            keyboardFlippedOpen = (0 == test_bit(SW_KEYPAD_SLIDE, bits));
    }
    if (eventFd != -1) {
        ::close(eventFd);
    }
    return keyboardFlippedOpen;
}

void QSystemDeviceInfoPrivate::socketActivated(int fd)
{
    int result = 0;
     do {
        struct input_event inputEvent;
        result = read(fd, &inputEvent, sizeof(inputEvent));
        if (result == sizeof(inputEvent)) {
            if(inputEvent.type > 0 && inputEvent.code == SW_KEYPAD_SLIDE) {
                Q_EMIT keyboardFlipped(!inputEvent.value);
            }
        }
    } while (result > 0);
}


bool QSystemDeviceInfoPrivate::keypadLightOn(QSystemDeviceInfo::KeypadType type)
{
    bool lightOn = false;

    if (type != QSystemDeviceInfo::PrimaryKeypad) {
        return lightOn;
    }

#if !defined(QT_NO_DBUS)
    QDBusReply<bool> reply = QDBusConnection::systemBus().call(
                                 QDBusMessage::createMethodCall("com.nokia.mce", "/com/nokia/mce/request",
                                                                "com.nokia.mce.request", "get_key_backlight_state"));
    if (reply.isValid()) {
        lightOn = reply.value();
    }
#endif
    return lightOn;
}

int QSystemDeviceInfoPrivate::messageRingtoneVolume()
{
    if (m_smsAlertVolumeQueried) {
        return m_smsAlertVolume;
    }

    QDBusMessage smsAlertVolumeMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                    "com.nokia.profiled", "get_value");
    smsAlertVolumeMsg << profileName();
    smsAlertVolumeMsg << "sms.alert.volume";

    QDBusReply<QString> smsAlertVolumeReply = QDBusConnection::sessionBus().call(smsAlertVolumeMsg);
    if (smsAlertVolumeReply.isValid()) {
        m_smsAlertVolume = smsAlertVolumeReply.value().toInt();
        m_smsAlertVolumeQueried = true;
    }
    return m_smsAlertVolume;
}

int QSystemDeviceInfoPrivate::voiceRingtoneVolume()
{
    if (m_ringingAlertVolumeQueried) {
        return m_ringingAlertVolume;
    }

    QDBusMessage ringingAlertVolumeMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                        "com.nokia.profiled", "get_value");
    ringingAlertVolumeMsg << profileName();
    ringingAlertVolumeMsg << "ringing.alert.volume";

    QDBusReply<QString> ringingAlertVolumeReply = QDBusConnection::sessionBus().call(ringingAlertVolumeMsg);
    if (ringingAlertVolumeReply.isValid()) {
        m_ringingAlertVolume = ringingAlertVolumeReply.value().toInt();
        m_ringingAlertVolumeQueried = true;
    }
    return m_ringingAlertVolume;
}

bool QSystemDeviceInfoPrivate::vibrationActive()
{
    if (m_vibratingAlertEnabledQueried) {
        return m_vibratingAlertEnabled;
    }

    QDBusMessage vibratingAlertMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                    "com.nokia.profiled", "get_value");
    vibratingAlertMsg << profileName();
    vibratingAlertMsg << "vibrating.alert.enabled";

    QDBusReply<QString> vibratingAlertEnabledReply = QDBusConnection::sessionBus().call(vibratingAlertMsg);
    if (vibratingAlertEnabledReply.isValid()) {
        m_vibratingAlertEnabled = QString::compare(vibratingAlertEnabledReply.value(), "On", Qt::CaseInsensitive) == 0;
        m_vibratingAlertEnabledQueried = true;
    }
    return m_vibratingAlertEnabled;
}

QSystemDeviceInfo::LockTypeFlags QSystemDeviceInfoPrivate::lockStatus()
{
    QSystemDeviceInfo::LockTypeFlags lockFlags; /* no active locks */
#if !defined(QT_NO_DBUS)
    /* Check the PIN lock status from devicelock */
    QDBusMessage lockStateCall = QDBusMessage::createMethodCall("com.nokia.devicelock", "/request",
                                                                "com.nokia.devicelock", "getState");
    /* getState argument 0: LockType_t, where 1 = the device lock */
    lockStateCall << QVariant::fromValue(1);

    QDBusReply<int> deviceLockReply = QDBusConnection::systemBus().call(lockStateCall);
    if (deviceLockReply.isValid()) {
        int lockState = deviceLockReply.value();
        if (lockState != 0) {
            /* 0 == unlocked, if we get any other state back, we are locked */
            lockFlags |= QSystemDeviceInfo::PinLocked;
        }
    }

    /* Check the touch screen/keypad lock status from MCE */
    QDBusReply<QString> mceReply = QDBusConnection::systemBus().call(
                                       QDBusMessage::createMethodCall("com.nokia.mce", "/com/nokia/mce/request",
                                                                      "com.nokia.mce.request", "get_tklock_mode"));
    if (mceReply.isValid()) {
        QString tkLockMode = mceReply.value();
        if (tkLockMode != "unlocked" && tkLockMode != "silent-unlocked") {
            lockFlags |= QSystemDeviceInfo::TouchAndKeyboardLocked;
             currentLockType = lockFlags;
        }
    }
#endif

    return lockFlags;
}

QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfoPrivate::keyboardTypes()
{
    QSystemDeviceInfo::KeyboardTypeFlags keyboardFlags;
    keyboardFlags = QSystemDeviceInfoLinuxCommonPrivate::keyboardTypes();
    keyboardFlags = (keyboardFlags | QSystemDeviceInfo::SoftwareKeyboard);
    if(model() == "RX-51") //fixme detect flip keyboard
        keyboardFlags = (keyboardFlags | QSystemDeviceInfo::FlipKeyboard);
    return keyboardFlags;
}

void QSystemDeviceInfoPrivate::deviceStateChanged(int device, int state)
{
    QSystemDeviceInfo::LockTypeFlags lockFlags;
    if (device == 1 && state != 0) {
        lockFlags |= QSystemDeviceInfo::PinLocked;
        currentLockType |= lockFlags;
        emit lockStatusChanged(lockFlags);
    } else {
        if (currentLockType & QSystemDeviceInfo::PinLocked) {
            currentLockType &= ~QSystemDeviceInfo::PinLocked;
        }
        lockFlags |= QSystemDeviceInfo::UnknownLock;
        currentLockType |= lockFlags;
        emit lockStatusChanged(lockFlags);
    }
}

void QSystemDeviceInfoPrivate::touchAndKeyboardStateChanged(const QString& state)
{
    QSystemDeviceInfo::LockTypeFlags lockFlags;
    if (state != "unlocked" && state != "silent-unlocked") {
        lockFlags |= QSystemDeviceInfo::TouchAndKeyboardLocked;
        currentLockType |= lockFlags;
        emit lockStatusChanged(lockFlags);
    } else {
        if (currentLockType & QSystemDeviceInfo::TouchAndKeyboardLocked) {
            currentLockType &= ~QSystemDeviceInfo::TouchAndKeyboardLocked;
        }
        lockFlags |= QSystemDeviceInfo::UnknownLock;
        currentLockType |= lockFlags;
        emit lockStatusChanged(lockFlags);
    }
}

QByteArray QSystemDeviceInfoPrivate::uniqueDeviceID()
{
#if defined(Q_WS_MAEMO_6)
    // create one from imei and mac addersses of bt and wlan interfaces
    QSystemNetworkInfo netinfo;
    QString wlanmac = netinfo.macAddress(QSystemNetworkInfo::WlanMode);
    QString btmac = netinfo.macAddress(QSystemNetworkInfo::BluetoothMode);

    QByteArray bytes = imei().toLocal8Bit();
    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData(bytes);
    hash.addData(wlanmac.toLocal8Bit());
    hash.addData(btmac.toLocal8Bit());

    return hash.result().toHex();
#endif
}

QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
    : QObject(parent)
    , isInhibited(false)
    , ssTimer(0)
{
#if !defined(QT_NO_DBUS)
    mceConnectionInterface = new QDBusInterface("com.nokia.mce",
                                                "/com/nokia/mce/request",
                                                "com.nokia.mce.request",
                                                QDBusConnection::systemBus(), this);
#endif // QT_NO_DBUS
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
    setScreenSaverInhibited(false);
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    wakeUpDisplay();

    if (!ssTimer)
        ssTimer = new QTimer(this);

    if (!ssTimer->isActive()) {
        connect(ssTimer, SIGNAL(timeout()), this, SLOT(wakeUpDisplay()));
        // Set a wake up interval of 30 seconds.
        // The reason for this is to avoid the situation where
        // a crashed/hung application keeps the display on.
        ssTimer->start(30000);
        isInhibited = true;
    } else {
        isInhibited = false;
    }

    return screenSaverInhibited();
}

void QSystemScreenSaverPrivate::wakeUpDisplay()
{
#if !defined(QT_NO_DBUS)
    if (mceConnectionInterface->isValid()) {
        QDBusMessage msg = mceConnectionInterface->call("req_tklock_mode_change", "unlocked");
        qDebug() << msg.errorName() << msg.errorMessage();
        msg = mceConnectionInterface->call("req_display_blanking_pause");
        qDebug() << msg.errorName() << msg.errorMessage();
    }
#endif // QT_NO_DBUS
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    bool displayOn = false;
    GConfItem screenBlankItem("/system/osso/dsm/display/inhibit_blank_mode");
    /* 0 - no inhibit
       1 - inhibit dim with charger
       2 - inhibit blank with charger (display still dims)
       3 - inhibit dim (always)
       4 - inhibit blank (always; display still dims)
    */
    int blankingItem = screenBlankItem.value().toInt();

    bool isBlankingInhibited = false;
    QSystemDeviceInfo devInfo(this);
    QSystemDeviceInfo::PowerState batState = devInfo.currentPowerState();

    if (((batState == QSystemDeviceInfo::WallPower || batState == QSystemDeviceInfo::WallPowerChargingBattery) && blankingItem == 2)
        || blankingItem == 4) {
        isBlankingInhibited = true;
    }

#if !defined(QT_NO_DBUS)
    if (mceConnectionInterface->isValid()) {
        // The most educated guess for the screen saver being inhibited is to determine
        // whether the display is on. That is because the QSystemScreenSaver cannot
        // prevent other processes from blanking the screen (like, if
        // MCE decides to blank the screen for some reason).
        // but that means it reports to be inhibited when display is on. meaning
        // effectly always inhibited by default. so we try a bit harder
        QDBusReply<QString> reply = mceConnectionInterface->call("get_display_status");
        displayOn = ("on" == reply.value());
    }
#endif // QT_NO_DBUS

    return ((displayOn && isBlankingInhibited) || (displayOn && isInhibited));
}

void QSystemScreenSaverPrivate::setScreenSaverInhibited(bool on)
{
    if (on) {
        setScreenSaverInhibit();
    } else {
        if (ssTimer && ssTimer->isActive()) {
            ssTimer->stop();
            isInhibited = false;
        }
    }
}


#ifdef Q_USE_BME
////////////
// from QmSystem remove if/when QmSystem can be a dependency.

template <typename T>
class EmHandle
{
public:
    EmHandle() { }

    virtual ~EmHandle() { close(); }

    bool open()
    {
        T *self = static_cast<T*>(this);
        if (!self->is_opened()) {
            self->open_();
        }

        if (!self->is_opened()) {
            qCritical() << "EM: error opening: " << strerror(errno);
        }
        return self->is_opened();
    }

    void close()
    {
        T *self = static_cast<T*>(this);
        if (self->is_opened()) {
            self->close_();
        }
    }

};

#define BMEIPC_MAX_TRIES 2

class EmIpc : public EmHandle<EmIpc>
{
    friend class EmHandle<EmIpc>;
public:

    EmIpc() : EmHandle<EmIpc>(), sd_(-1), restart_count_(0) {}

    bool query(const void *msg1, int len1, void *msg2 = NULL, int len2 = -1)
    {
        if (!is_opened()) {
            qCritical() << "EM: not open";
            return false;
        }

        int tries = 0;
        while (true) {
            tries++;
            if (::bmeipc_query(sd_, msg1, len1, msg2, len2) >= 0)
                return true;
            if (errno == EIO)
                restart_count_++;
            if (tries >= BMEIPC_MAX_TRIES)  {
                qCritical() << "EM: Query failed: " << strerror(errno);
                return false;
            }
            qWarning() << "EM:" << strerror(errno) << "(trying to reopen)";
            close();
            if (!open())
                return false;
        }
    }

    bool is_opened() { return sd_ >= 0; }
    int restart_count() { return restart_count_; }

private:

    inline void open_()
    {
        sd_ = ::bmeipc_open();
    }

    inline void close_()
    {
        ::bmeipc_close(sd_);
        sd_ = -1;
    }


    int sd_;
    int restart_count_;
};


class EmEvents : public EmHandle<EmEvents>
{
    friend class EmHandle<EmEvents>;

public:
    EmEvents(int mask = -1)
        : EmHandle<EmEvents>(),
          sd_(-1),
          mask_(mask)
    { }

    int read()
    {
        if (!is_opened()) {
            return BMEVENT_ERROR;
        }

        int res = ::bmeipc_eread(sd_);
        if (res == BMEVENT_ERROR) {
            qDebug() << "bmeipc_eread returned error" << strerror(errno);
        }
        return res;
    }

    inline bool is_opened() { return sd_ >= 0; }

    QSocketNotifier const* notifier() const { return notifier_.data(); }

private:

    inline void open_()
    {
        sd_ = ::bmeipc_eopen(mask_);
        if (is_opened())
            notifier_.reset(new QSocketNotifier(sd_, QSocketNotifier::Read));
    }

    inline void close_()
    {
        notifier_.reset(0);
        ::bmeipc_eclose(sd_);
        sd_ = -1;
    }

    int sd_;
    int mask_;
    QScopedPointer<QSocketNotifier> notifier_;
};


class EmCurrentMeasurement : public EmHandle<EmCurrentMeasurement>
{
    friend class EmHandle<EmCurrentMeasurement>;

public:

    EmCurrentMeasurement(unsigned int period)
        : EmHandle<EmCurrentMeasurement>(),
          period_(period),
          mq_(-1),
          em_ipc_(new EmIpc())
    { }

    inline bool is_opened() { return mq_ >= 0; }

    bool measure(int &current)
    {
        current = 0;

        if (!is_opened())
            return false;

        int n;
        bmeipc_meas_t msg;
        n = mq_receive(mq_, (char *)&msg, sizeof(msg), 0);

        if (0 > n) {
            qDebug() << "failed to receive message: "
                     << strerror(errno);
        } else if (n != sizeof(msg)) {
            qDebug() << "bad message size: need "
                     << sizeof (msg) << ", got " << n;
        } else if (MEASUREMENTS_ERROR == msg.state) {
            qDebug() << " error message received";
        } else if (MEASUREMENTS_OFF == msg.state) {
            qDebug() << "measurements are off";
        } else {
            current = msg.bat_current;
            return true;
        }
        return false;
    }

    QSocketNotifier const* notifier() const { return notifier_.data(); }

private:

    inline bool request_measurements_(unsigned int period)
    {
        struct emsg_measurement_req req;
        struct emsg_measurement_req_elem req_elem;

        if (!em_ipc_->open()) {
            qCritical() << "failed to contact bme server: "
                        << strerror(errno);
            return false;
        }

        /* Send message header */
        req.type = EM_MEASUREMENT_REQ;
        req.subtype = 0;
        req.measurement_action = EM_MEASUREMENT_ACTION_START;
        req.channel_count = 1;

        if (!em_ipc_->query(&req, sizeof(req))) {
            qCritical() << "failed to request measurments: "
                        << strerror(errno);
            return false;
        }

        req_elem.type = EM_MEASUREMENT_TYPE_CURRENT;
        req_elem.period = period;

        if (!em_ipc_->query(&req_elem, sizeof(req_elem))) {
            qCritical() << "failed to request data: " << strerror(errno);
            return false;
        }

        return true;

    }

    void open_()
    {
        if (!request_measurements_(period_))
            return;

        mq_ = mq_open(BMEIPC_MQNAME, O_RDONLY);
        if (!is_opened())
            return;

        notifier_.reset(new QSocketNotifier(mq_, QSocketNotifier::Read));
    }

    void close_()
    {
        struct emsg_measurement_req req;

        req.type = EM_MEASUREMENT_REQ;
        req.subtype = 0;
        req.measurement_action = EM_MEASUREMENT_ACTION_STOP;
        req.channel_count = 0;

        if (!em_ipc_->query(&req, sizeof(req))) {
            qCritical() << "failed to stop measurements"
                        << strerror(errno);
        }

        em_ipc_->close();

        notifier_.reset(0);
        ::mq_close(mq_);
        mq_ = -1;
    }

    int period_;
    mqd_t mq_;
    QScopedPointer<EmIpc> em_ipc_;
    QScopedPointer<QSocketNotifier> notifier_;
};
#endif

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent)
    : QSystemBatteryInfoLinuxCommonPrivate(parent)
#ifdef Q_USE_BME
      ,emIpc(new EmIpc()),
      emEvents(new EmEvents())
#endif
{
#if !defined(QT_NO_DBUS)
    QHalInterface iface;
    QStringList list = iface.findDeviceByCapability("battery");
    if (!list.isEmpty()) {
        foreach (const QString &dev, list) {
            halIfaceDevice = new QHalDeviceInterface(dev);
            if (halIfaceDevice->isValid()) {
                if (halIfaceDevice->setConnections()) {
                    if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                 this,SLOT(halChangedMaemo(int,QVariantList)))) {
                        qDebug() << "connection malfunction";
                    }
                }
                return;
            }
        }
    }
#endif
}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{
}

#if !defined(QT_NO_DBUS)
void QSystemBatteryInfoPrivate::halChangedMaemo(int count,QVariantList map)
{
    QHalInterface iface;
    QStringList list = iface.findDeviceByCapability("battery");
    QHalDeviceInterface ifaceDevice(list.at(0)); //default battery
    if (ifaceDevice.isValid()) {
        for(int i=0; i < count; i++) {
            QString mapS = map.at(i).toString();
            qDebug() << __FUNCTION__ << mapS;
            QSystemBatteryInfo::ChargerType chargerType = QSystemBatteryInfo::UnknownCharger;
             if (  mapS == "maemo.charger.connection_status" || mapS == "maemo.charger.type") {
                const QString chargeType = ifaceDevice.getPropertyString("maemo.charger.type");
                if(chargeType == "host 500 mA") {
                    chargerType = QSystemBatteryInfo::USB_500mACharger;
                }
                if(chargeType == "host 100 mA") {
                    chargerType = QSystemBatteryInfo::USB_100mACharger;
                }
                chargerType = QSystemBatteryInfoLinuxCommonPrivate::currentChargerType();
                if (chargerType == QSystemBatteryInfo::UnknownCharger) {
                    chargerType = QSystemBatteryInfo::WallCharger;
                }

                if(chargerType != curChargeType) {
                    curChargeType = chargerType;
                    Q_EMIT chargerTypeChanged(curChargeType);
                }
            }
         }
    }
}
#endif

void QSystemBatteryInfoPrivate::connectNotify(const char *signal)
{
#ifdef Q_USE_BME
    if (QLatin1String(signal) ==
            QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentFlowChanged(int))))) {
        startMeasurements();
    }
#endif
}

void QSystemBatteryInfoPrivate::disconnectNotify(const char *signal)
{
#ifdef Q_USE_BME
    if (QLatin1String(signal) ==
            QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentFlowChanged(int))))) {
        stopMeasurements();
    }
#endif
}

#ifdef Q_USE_BME
void QSystemBatteryInfoPrivate::startMeasurements()
{
    emCurrentMeasurements.reset(new EmCurrentMeasurement(2));//every 1 second
    if (emCurrentMeasurements->open()) {
        connect(emCurrentMeasurements->notifier(),SIGNAL(activated(int)),this,SLOT(onMeasurement(int)));
    }
}


void QSystemBatteryInfoPrivate::stopMeasurements()
{
    disconnect(emCurrentMeasurements->notifier(),SIGNAL(activated(int)),this,SLOT(onMeasurement(int)));
    emCurrentMeasurements->close();
    if (emCurrentMeasurements->is_opened()) {
        qDebug() << "disconnect failed";
    } else {
        emCurrentMeasurements.reset(0);
    }
}

void QSystemBatteryInfoPrivate::onMeasurement(int)
{
    bool rc;
    int current;

    if (emCurrentMeasurements.isNull()) {
        qWarning() << "onMeasurement: null";
        return;
    }

    rc = emCurrentMeasurements->measure(current);
    Q_EMIT currentFlowChanged(current);
}
#endif

int QSystemBatteryInfoPrivate::currentFlow() const
{
#ifdef Q_USE_BME
    QDateTime now(QDateTime::currentDateTime());

    if (!isDataActual || now >= cacheExpire) {
        if (!emIpc->open()) {
            return 0;
        }

        int prevCc = bmeStat[COULOMB_COUNTER];

        bmeipc_msg_t request;
        request.type = BME_SYSMSG_GETSTAT;
        request.subtype = 0;
        if (!emIpc->query(&request, sizeof(request), &bmeStat, sizeof(bmeStat))) {
            return 0;
        }

        isDataActual = true;
        cacheExpire = now.addSecs(5);

        if (prevColoumbCounterRestartCount != emIpc->restart_count()) {
            coloumbCounterOffset += (prevCc - bmeStat[COULOMB_COUNTER]);
            qDebug() << "CC reset, prev_cc:" << prevCc
                     << "new_cc" << bmeStat[COULOMB_COUNTER]
                     << "new offset:" << coloumbCounterOffset;
            prevColoumbCounterRestartCount = emIpc->restart_count();
        }
    }

    return bmeStat[BATTERY_CURRENT];
#endif
    return 0;
}



#include "moc_qsysteminfo_maemo_p.cpp"

QTM_END_NAMESPACE
