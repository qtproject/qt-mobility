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
    languages = langDir.entryList(QStringList() <<"??",QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
#else
    GConfItem languagesItem("/meegotouch/inputmethods/languages");
    const QStringList locales = languagesItem.value().toStringList();

    foreach(const QString &locale, locales) {
        languages << locale.mid(0,2);
    }
    languages << currentLanguage();
    languages.removeDuplicates();
#endif

    return languages;
}

QString QSystemInfoPrivate::currentLanguage() const
{
#if defined(Q_WS_MAEMO_6)
    GConfItem langItem("/meegotouch/i18n/language");
    QString lang = langItem.value().toString();
    if(lang.count() > 2) lang = lang.left(2);
    if (lang.isEmpty()) {
        lang = QString::fromLocal8Bit(qgetenv("LANG")).left(2);
    }
    return lang;
#else
    return QSystemInfoLinuxCommonPrivate::currentLanguage();
#endif
}


QString QSystemInfoPrivate::currentCountryCode() const
{
#if defined(Q_WS_MAEMO_6)
    GConfItem langItem("/meegotouch/i18n/region");
     QString langCC = langItem.value().toString().section("_",1,1);
     if (langCC.isEmpty()) {
         langCC = QString::fromLocal8Bit(qgetenv("LANG")).section("_",1,1);
         langCC = langCC.remove(".UTF-8",Qt::CaseSensitive);
         return langCC;
     }
#endif
    return QSystemInfoLinuxCommonPrivate::currentCountryCode();
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,const QString &parameter)
{
    QString errorStr = "Not Available";

    switch(type) {
    case QSystemInfo::Os :
    {
        QString sysinfodValue = sysinfodValueForKey("/device/sw-release-ver");//("/device/content-ver");
        if (!sysinfodValue.isEmpty()) {
           sysinfodValue =  sysinfodValue.section("_",2,4);
            return sysinfodValue;
        }
    }
        break;
    case QSystemInfo::Firmware :
    {
        QString sysinfodValue = sysinfodValueForKey("/device/sw-release-ver");
        if (!sysinfodValue.isEmpty()) {
            return sysinfodValue;
        }
    }

    default:
        return QSystemInfoLinuxCommonPrivate::version(type, parameter);
        break;
    };
    return errorStr;
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::SimFeature :
        {
            QSystemDeviceInfoPrivate d;
            featureSupported = (d.simStatus() != QSystemDeviceInfo::SimNotAvailable);
        }
        break;
    case QSystemInfo::LocationFeature :
        {
#if defined(Q_WS_MAEMO_6)
            GConfItem satellitePositioning("/system/osso/location/settings/satellitePositioning");
            GConfItem networkPositioning("/system/osso/location/settings/networkPositioning");

            bool satellitePositioningAvailable = satellitePositioning.value(false).toBool();
            bool networkPositioningAvailable   = networkPositioning.value(false).toBool();

            featureSupported = (satellitePositioningAvailable || networkPositioningAvailable);
#else /* Maemo 5 */
            GConfItem locationValues("/system/nokia/location");
            const QStringList locationKeys = locationValues.listEntries();
            if(locationKeys.count()) {
                featureSupported = true;
            }
#endif /* Maemo 5 */
        }
        break;
    case QSystemInfo::HapticsFeature:
        {
           // if(halIsAvailable) {
                QHalInterface iface;
                const QStringList touchSupport =
                        iface.findDeviceByCapability("input.touchpad");
                if(touchSupport.count()) {
                    featureSupported = true;
                } else {
                    featureSupported = false;
                }
            }
      //  }
        break;
    default:
        featureSupported = QSystemInfoLinuxCommonPrivate::hasFeatureSupported(feature);
        break;
    };
    return featureSupported;
}

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
        : QSystemNetworkInfoLinuxCommonPrivate(parent)
{
    csStatusMaemo6["Unknown"]    = -1;  // Current registration status is unknown.
    csStatusMaemo6["Home"]       = 0;   // Registered with the home network.
    csStatusMaemo6["Roaming"]    = 1;   // Registered with a roaming network.
    csStatusMaemo6["Offline"]    = 3;   // Not registered.
    csStatusMaemo6["Searching"]  = 4;   // Offline, but currently searching for network.
    csStatusMaemo6["NoSim"]      = 6;   // Offline because no SIM is present.
    csStatusMaemo6["PowerOff"]   = 8;   // Offline because the CS is powered off.
    csStatusMaemo6["PowerSave"]  = 9;   // Offline and in power save mode.
    csStatusMaemo6["NoCoverage"] = 10;  // Offline and in power save mode because of poor coverage.
    csStatusMaemo6["Rejected"]   = 11;  // Offline because SIM was rejected by the network.

    setupNetworkInfo();
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
    if(wlanSignalStrengthTimer && wlanSignalStrengthTimer->isActive())
        wlanSignalStrengthTimer->stop();
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        {
            // radioAccessTechnology: 1 = GSM, 2 = WCDMA
            if((radioAccessTechnology == 1 && mode != QSystemNetworkInfo::GsmMode) ||
               (radioAccessTechnology == 2 && mode != QSystemNetworkInfo::WcdmaMode)) {
                return QSystemNetworkInfo::NoNetworkAvailable;
            }

            switch(currentCellNetworkStatus) {
                case 0: return QSystemNetworkInfo::HomeNetwork; // CS is registered to home network
                case 1: return QSystemNetworkInfo::Roaming; // CS is registered to some other network than home network
                case 2: return QSystemNetworkInfo::Roaming; // CS is registered to non-home system in a non-home area
                case 3: return QSystemNetworkInfo::NoNetworkAvailable; // CS is not in service
                case 4: return QSystemNetworkInfo::Searching; // CS is not in service, but is currently searching for service
                case 5: return QSystemNetworkInfo::NoNetworkAvailable; // CS is not in service and it is not currently searching for service
                case 6: return QSystemNetworkInfo::NoNetworkAvailable; // CS is not in service due to missing SIM or missing subscription
                case 8: return QSystemNetworkInfo::NoNetworkAvailable; // CS is in power off state
                case 9: return QSystemNetworkInfo::NoNetworkAvailable; // CS is in No Service Power Save State (currently not listening to any cell)
                case 10: return QSystemNetworkInfo::NoNetworkAvailable; // CS is in No Service Power Save State (CS is entered to this state
                                                                        // because there is no network coverage)
                case 11: return QSystemNetworkInfo::Denied; // CS is not in service due to missing subscription
                default:
                    break;
            };
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        if(currentEthernetState == "up") {
            return QSystemNetworkInfo::Connected;
        } else {
            return QSystemNetworkInfo::NoNetworkAvailable;
        }
        break;
    case QSystemNetworkInfo::WlanMode:
    case QSystemNetworkInfo::BluetoothMode:
        {
            return QSystemNetworkInfoLinuxCommonPrivate::networkStatus(mode);
        }
        break;
    default:
        break;
    };
    return QSystemNetworkInfo::UndefinedStatus;
}

qint32 QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
    {
            // radioAccessTechnology: 1 = GSM, 2 = WCDMA
            if((radioAccessTechnology == 1 && mode != QSystemNetworkInfo::GsmMode) ||
               (radioAccessTechnology == 2 && mode != QSystemNetworkInfo::WcdmaMode)) {
                return -1;
            }

            return cellSignalStrength;
    }
    case QSystemNetworkInfo::EthernetMode: {
        QString result;
        QString baseSysDir = "/sys/class/net/";
        QString interface = QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode).humanReadableName();
        if (interface == "usb0") {
            QString dir = QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode).name();
            QString devFile = baseSysDir + dir;
            QFileInfo fi(devFile + "/carrier");
            if(fi.exists()) {
                QFile rx(fi.absoluteFilePath());
                if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream stream(&rx);
                    stream >> result;
                    rx.close();
                    return result.toInt() * 100;
                    break;
                }
            }
        }
        return QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
        break;
    }
    case QSystemNetworkInfo::WlanMode:
    case QSystemNetworkInfo::BluetoothMode:
        return QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
        break;
    default:
        break;
    };

    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
    return currentCellId;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return currentLac;
}

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    if (!currentMCC.isEmpty()) {
        return currentMCC;
    }

    // Don't have a cached MCC value, let's query it
    QDBusMessage message = QDBusMessage::createMethodCall("com.nokia.csd.CSNet",
                                                          "/com/nokia/csd/csnet",
                                                          "org.freedesktop.DBus.Properties",
                                                          "Get");
    message << "com.nokia.csd.CSNet.NetworkOperator";
    message << "OperatorMCC";

    QDBusReply<QDBusVariant> mccReply = QDBusConnection::systemBus().call(message);
    if (mccReply.isValid()) {
        QDBusVariant v = mccReply.value();
        return v.variant().toString();
    }

    return currentMCC;
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    if (!currentMNC.isEmpty()) {
        return currentMNC;
    }

    // Don't have a cached MNC value, let's query it
    QDBusMessage message = QDBusMessage::createMethodCall("com.nokia.csd.CSNet",
                                                          "/com/nokia/csd/csnet",
                                                          "org.freedesktop.DBus.Properties",
                                                          "Get");
    message << "com.nokia.csd.CSNet.NetworkOperator";
    message << "OperatorMNC";

    QDBusReply<QDBusVariant> mncReply = QDBusConnection::systemBus().call(message);
    if (mncReply.isValid()) {
        QDBusVariant v = mncReply.value();
        return v.variant().toString();
    }

    return currentMNC;
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    QSystemDeviceInfoPrivate d;
    QString imsi = d.imsi();
    if (imsi.length() >= 3) {
        return imsi.left(3);
    }
    return "";
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
#if !defined(QT_NO_DBUS)
    #if defined(Q_WS_MAEMO_6)
    QDBusInterface connectionInterface("com.nokia.csd.SIM",
                                       "/com/nokia/csd/sim",
                                       "com.nokia.csd.SIM.Identity",
                                       QDBusConnection::systemBus(), this);
    QDBusMessage reply = connectionInterface.call(QLatin1String("GetHPLMN"));
    if (reply.errorName().isEmpty()) {
        QList<QVariant> args = reply.arguments();
        // The first attribute should be MCC and the 2nd one MNC
        if (args.size() == 2) {
            return args.at(1).toString();
        }
    }
    #else
    /* Maemo 5 */
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
        if (mnc3 != "f") {
            homeMobileNetworkCode.prepend(mnc3);
        }
        homeMobileNetworkCode.prepend(mnc2);
        homeMobileNetworkCode.prepend(mnc1);
        return homeMobileNetworkCode;
    }
    #endif
#endif
    return "";
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString netname = "";

    switch(mode) {

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode: {
        // radioAccessTechnology: 1 = GSM, 2 = WCDMA
        if((radioAccessTechnology == 1 && mode != QSystemNetworkInfo::GsmMode) ||
           (radioAccessTechnology == 2 && mode != QSystemNetworkInfo::WcdmaMode)) {
            break;
        }

        return currentOperatorName;
        break;
    }
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::networkName(mode);
        break;
    };
    return netname;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode:
    case QSystemNetworkInfo::WimaxMode:
        break;
    case QSystemNetworkInfo::EthernetMode: {
        QString address;
        QString baseSysDir = "/sys/class/net/";
        QString interface = QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode).humanReadableName();
        if (interface == "usb0") {
            QString dir = QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode).name();
            QString devFile = baseSysDir + dir;
            QFileInfo fi(devFile + "/address");
            if(fi.exists()) {
                QFile rx(fi.absoluteFilePath());
                if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream stream(&rx);
                    stream >> address;
                    rx.close();
                    return address;
                    break;
                }
            }
        }
        return QSystemNetworkInfoLinuxCommonPrivate::macAddress(mode);
        break;
    }
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::macAddress(mode);
        break;
    };
    return QString();
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_DBUS)
    switch(mode) {
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode:
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode);
        break;
    };
#endif
    return QNetworkInterface();
}

QMap<QString,QVariant> QSystemNetworkInfoPrivate::queryCsdProperties(const QString& service, const QString& servicePath, const QString& interface)
{
    QMap<QString,QVariant> props;
#if !defined(QT_NO_DBUS)
    const QString dBusProps = "org.freedesktop.DBus.Properties";

    QDBusMessage message = QDBusMessage::createMethodCall(service,
                                                          servicePath,
                                                          dBusProps,
                                                          "GetAll");
    message << interface;
    QDBusReply< QMap<QString, QVariant> > csdReply = QDBusConnection::systemBus().call(message);

    if (csdReply.isValid()) {
        props = csdReply.value();
    }
#endif
    return props;
}

void QSystemNetworkInfoPrivate::setupNetworkInfo()
{
    currentCellNetworkStatus = -1;
    currentBluetoothNetworkStatus = networkStatus(QSystemNetworkInfo::BluetoothMode);
    currentWlanNetworkStatus = networkStatus(QSystemNetworkInfo::WlanMode);
    currentEthernetState = "down";
    currentEthernetSignalStrength = networkSignalStrength(QSystemNetworkInfo::EthernetMode);
    currentWlanSignalStrength = networkSignalStrength(QSystemNetworkInfo::WlanMode);
    currentLac = -1;
    currentCellId = -1;
    currentMCC = "";
    currentMNC = "";
    cellSignalStrength = 0;
    currentOperatorName = "";
    radioAccessTechnology = 0;
    iWlanStrengthCheckEnabled = 0;
    wlanSignalStrengthTimer = new QTimer(this);

    connect(wlanSignalStrengthTimer, SIGNAL(timeout()), this, SLOT(wlanSignalStrengthCheck()));

    QString devFile = "/sys/class/net/usb0/operstate";
    QFileInfo fi(devFile);
    if (fi.exists()) {
        QFile rx(fi.absoluteFilePath());
        if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&rx);
            stream >> currentEthernetState;
            rx.close();
        }
    }

    currentNetworkMode = currentMode();

#if !defined(QT_NO_DBUS)
    QDBusConnection systemDbusConnection = QDBusConnection::systemBus();

    #if defined(Q_WS_MAEMO_6)
        const QString service = "com.nokia.csd.CSNet";
        const QString servicePath = "/com/nokia/csd/csnet";

        /* CSD: network cell */
        QMap<QString,QVariant> csdNetworkCell = queryCsdProperties(service, servicePath, "com.nokia.csd.CSNet.NetworkCell");

        QVariant cellLac = csdNetworkCell.value("CellLac", -1);
        currentLac = cellLac.isValid() ? cellLac.value<int>() : -1;

        QVariant cellId = csdNetworkCell.value("CellId", -1);
        currentCellId =  cellId.isValid() ? cellId.value<int>() : -1;

        QVariant cellType = csdNetworkCell.value("CellType", "");
        QString currentCellType = cellType.isValid() ? cellType.value<QString>() : "";

        if (currentCellType == "GSM")
            radioAccessTechnology = 1;
        else if (currentCellType == "WCDMA")
            radioAccessTechnology = 2;

        /* CSD: network operator */
        QMap<QString,QVariant> csdNetworkOperator = queryCsdProperties(service, servicePath, "com.nokia.csd.CSNet.NetworkOperator");

        QVariant mcc = csdNetworkOperator.value("OperatorMCC", "");
        currentMCC = mcc.isValid() ? mcc.value<QString>() : "";

        QVariant mnc = csdNetworkOperator.value("OperatorMNC", "");
        currentMNC = mnc.isValid() ? mnc.value<QString>() : "";

        QVariant operatorName = csdNetworkOperator.value("OperatorName", "");
        currentOperatorName = operatorName.isValid() ? operatorName.value<QString>() : "";

        /* CSD: signal strength */
        QMap<QString,QVariant> csdSignalStrength = queryCsdProperties(service, servicePath, "com.nokia.csd.CSNet.SignalStrength");

        QVariant signalStrength = csdSignalStrength.value("SignalPercent", -1);
        cellSignalStrength = signalStrength.isValid() ? signalStrength.value<int>() : -1;

        /* CSD: network registration */
        QMap<QString,QVariant> csdNetworkRegistration = queryCsdProperties(service, servicePath, "com.nokia.csd.CSNet.NetworkRegistration");

        QVariant registrationStatus = csdNetworkRegistration.value("RegistrationStatus", "");
        QString status = registrationStatus.isValid() ? registrationStatus.value<QString>() : "";

        currentCellNetworkStatus = csStatusMaemo6.value(status, -1);

        /* CSD: data technology */
        currentCellDataTechnology = cellDataTechnology();

    #else
    /* Maemo 5 */
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
        cellSignalStrength = argList.at(0).toInt();
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
    if(!systemDbusConnection.connect("com.nokia.bme",
                              "/com/nokia/bme/signal",
                              "com.nokia.bme.signal",
                              QLatin1String("charger_connected"),
                              this, SLOT(usbCableAction())) ) {
        qDebug() << "unable to connect to usbCableAction (connect)";
    }
    if(!systemDbusConnection.connect("com.nokia.bme",
                              "/com/nokia/bme/signal",
                              "com.nokia.bme.signal",
                              QLatin1String("charger_disconnected"),
                              this, SLOT(usbCableAction())) ) {
        qDebug() << "unable to connect to usbCableAction (disconnect)";
    }
    #endif /* Maemo 5 */

    if(!systemDbusConnection.connect("com.nokia.icd",
                              "/com/nokia/icd",
                              "com.nokia.icd",
                              QLatin1String("status_changed"),
                              this, SLOT(icdStatusChanged(QString,QString,QString,QString))) ) {
        qDebug() << "unable to connect to icdStatusChanged";
    }
    if(!systemDbusConnection.connect("org.freedesktop.Hal",
                              "/org/freedesktop/Hal/Manager",
                              "org.freedesktop.Hal.Manager",
                              QLatin1String("DeviceAdded"),
                              this, SLOT(bluetoothNetworkStatusCheck(QString))) ) {
        qDebug() << "unable to connect to bluetoothNetworkStatusCheck (1)";
    }
    if(!systemDbusConnection.connect("org.freedesktop.Hal",
                              "/org/freedesktop/Hal/Manager",
                              "org.freedesktop.Hal.Manager",
                              QLatin1String("DeviceRemoved"),
                              this, SLOT(bluetoothNetworkStatusCheck(QString))) ) {
        qDebug() << "unable to connect to bluetoothNetworkStatusCheck (2)";
    }
#endif
}

void QSystemNetworkInfoPrivate::connectNotify(const char *signal)
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    const QString service = "com.nokia.csd.CSNet";
    const QString servicePath = "/com/nokia/csd/csnet";

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)))))) {
        if (!QDBusConnection::systemBus().connect(service, servicePath, "com.nokia.csd.CSNet.SignalStrength", "SignalStrengthChanged",
                                                  this, SLOT(slotSignalStrengthChanged(int, int)))) {
            qDebug() << "unable to connect SignalStrengthChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentMobileCountryCodeChanged(const QString &)))))
            || (QLatin1String(signal)
                == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentMobileNetworkCodeChanged(const QString &))))) ) {
        if (!QDBusConnection::systemBus().connect(service, servicePath, "com.nokia.csd.CSNet.NetworkOperator", "OperatorChanged",
                                                  this, SLOT(slotOperatorChanged(const QString&,const QString&)))) {
            qDebug() << "unable to connect (OperatorChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &)))))) {
        if (!QDBusConnection::systemBus().connect(service, servicePath, "com.nokia.csd.CSNet.NetworkOperator", "OperatorNameChanged",
                                                  this, SLOT(slotOperatorNameChanged(const QString&)))) {
            qDebug() << "unable to connect OperatorNameChanged";
        }
    }


    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)))))) {
        if (!QDBusConnection::systemBus().connect(service, servicePath, "com.nokia.csd.CSNet.NetworkRegistration", "RegistrationChanged",
                                                  this, SLOT(slotRegistrationChanged(const QString&)))) {
            qDebug() << "unable to connect RegistrationChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(cellIdChanged(int)))))) {
        if (!QDBusConnection::systemBus().connect(service, servicePath, "com.nokia.csd.CSNet.NetworkCell", "CellChanged",
                                          this, SLOT(slotCellChanged(const QString&,int,int)))) {
            qDebug() << "unable to connect CellChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology)))))) {
        if (!QDBusConnection::systemBus().connect(service, servicePath, "com.nokia.csd.CSNet.RadioAccess", "DataTechnologyChanged",
                                          this, SLOT(slotCellDataTechnologyChanged(const QString&)))) {
            qDebug() << "unable to connect DataTechnologyChanged";
        }
    }

#else //maemo5

#endif
#endif
}

void QSystemNetworkInfoPrivate::disconnectNotify(const char *signal)
{
#if !defined(QT_NO_DBUS)
#if defined(Q_WS_MAEMO_6)
    const QString service = "com.nokia.csd.CSNet";
    const QString servicePath = "/com/nokia/csd/csnet";
    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)))))) {
        if (!QDBusConnection::systemBus().disconnect(service, servicePath, "com.nokia.csd.CSNet.SignalStrength", "SignalStrengthChanged",
                                                  this, SLOT(slotSignalStrengthChanged(int, int)))) {
            qDebug() << "unable to disconnect SignalStrengthChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentMobileCountryCodeChanged(const QString &)))))
            || (QLatin1String(signal)
                == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(currentMobileNetworkCodeChanged(const QString &))))) ) {
        if (!QDBusConnection::systemBus().disconnect(service, servicePath, "com.nokia.csd.CSNet.NetworkOperator", "OperatorChanged",
                                                  this, SLOT(slotOperatorChanged(const QString&,const QString&)))) {
            qDebug() << "unable to disconnect (OperatorChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &)))))) {
        if (!QDBusConnection::systemBus().disconnect(service, servicePath, "com.nokia.csd.CSNet.NetworkOperator", "OperatorNameChanged",
                                                  this, SLOT(slotOperatorNameChanged(const QString&)))) {
            qDebug() << "unable to disconnect OperatorNameChanged";
        }
    }


    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)))))) {
        if (!QDBusConnection::systemBus().disconnect(service, servicePath, "com.nokia.csd.CSNet.NetworkRegistration", "RegistrationChanged",
                                                  this, SLOT(slotRegistrationChanged(const QString&)))) {
            qDebug() << "unable to disconnect RegistrationChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(cellIdChanged(int)))))) {
        if (!QDBusConnection::systemBus().disconnect(service, servicePath, "com.nokia.csd.CSNet.NetworkCell", "CellChanged",
                                          this, SLOT(slotCellChanged(const QString&,int,int)))) {
            qDebug() << "unable to disconnect CellChanged";
        }
    }

    if ((QLatin1String(signal)
         == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology)))))) {
        if (!QDBusConnection::systemBus().disconnect(service, servicePath, "com.nokia.csd.CSNet", "ActivityChanged",
                                          this, SLOT(slotCellDataTechnologyChanged(const QString&)))) {
            qDebug() << "unable to disconnect DataTechnologyChanged";
        }
    }

#else //maemo5

#endif
#endif
}


#if defined(Q_WS_MAEMO_6)
// Slots only available in Maemo6

void QSystemNetworkInfoPrivate::slotSignalStrengthChanged(int percent, int /*dbm*/)
{
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    cellSignalStrength = percent;

    if (radioAccessTechnology == 1)
        mode = QSystemNetworkInfo::GsmMode;
    if (radioAccessTechnology == 2)
        mode = QSystemNetworkInfo::WcdmaMode;

    if (mode != QSystemNetworkInfo::UnknownMode)
        emit networkSignalStrengthChanged(mode, cellSignalStrength);
}

void QSystemNetworkInfoPrivate::slotOperatorChanged(const QString &mnc, const QString &mcc)
{
    if (currentMCC != mcc) {
        currentMCC = mcc;
        emit currentMobileCountryCodeChanged(currentMCC);
    }
    if (currentMNC != mnc) {
        currentMNC = mnc;
        emit currentMobileNetworkCodeChanged(currentMNC);
    }
}

void QSystemNetworkInfoPrivate::slotOperatorNameChanged(const QString &name)
{
    currentOperatorName = name;
    if (radioAccessTechnology == 1)
        emit networkNameChanged(QSystemNetworkInfo::GsmMode, currentOperatorName);
    if (radioAccessTechnology == 2)
        emit networkNameChanged(QSystemNetworkInfo::WcdmaMode, currentOperatorName);
}

void QSystemNetworkInfoPrivate::slotRegistrationChanged(const QString &status)
{
    int newCellNetworkStatus = csStatusMaemo6.value(status, -1);

    if (currentCellNetworkStatus != newCellNetworkStatus) {
        currentCellNetworkStatus = newCellNetworkStatus;
        if (radioAccessTechnology == 1)
            emit networkStatusChanged(QSystemNetworkInfo::GsmMode,
                                      networkStatus(QSystemNetworkInfo::GsmMode));
        if (radioAccessTechnology == 2)
            emit networkStatusChanged(QSystemNetworkInfo::WcdmaMode,
                                      networkStatus(QSystemNetworkInfo::WcdmaMode));
    }
}

void QSystemNetworkInfoPrivate::slotCellChanged(const QString &type, int id, int lac)
{
    int newRadioAccessTechnology = 0;
    if (type == "GSM") {
        newRadioAccessTechnology = 1;
    } else if (type == "WCDMA") {
        newRadioAccessTechnology = 2;
    }

    if (newRadioAccessTechnology != radioAccessTechnology) {
        radioAccessTechnology = newRadioAccessTechnology;
        networkModeChangeCheck();
    }
    if (currentCellId != id) {
        currentCellId = id;
        emit cellIdChanged(currentCellId);
    }
    if (currentLac != lac) {
        currentLac = lac;
    }
}

void QSystemNetworkInfoPrivate::slotCellDataTechnologyChanged(const QString &tech)
{
    QSystemNetworkInfo::CellDataTechnology oldcdt = currentCellDataTechnology;

    if (oldcdt != cellDataTechnology()) {
        emit cellDataTechnologyChanged(currentCellDataTechnology);
    }
}

#endif /* Maemo 6 */

inline QSystemNetworkInfo::CellDataTechnology QSystemNetworkInfoPrivate::csdtToCellDataTechnology(const QString &tech)
{
    QSystemNetworkInfo::CellDataTechnology cdt = QSystemNetworkInfo::UnknownDataTechnology;
    if (tech == "GPRS") {
        cdt = QSystemNetworkInfo::GprsDataTechnology;
    } else if (tech == "EGPRS") {
        cdt = QSystemNetworkInfo::EdgeDataTechnology;
    } else if (tech == "UMTS") {
        cdt = QSystemNetworkInfo::UmtsDataTechnology;
    } else if (tech == "HSPA") {
        cdt = QSystemNetworkInfo::HspaDataTechnology;
    }
    return cdt;
}


#if defined(Q_WS_MAEMO_5)
// Slots only available in Maemo5

void QSystemNetworkInfoPrivate::cellNetworkSignalStrengthChanged(uchar var1, uchar)
{
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    cellSignalStrength = var1;

    if (radioAccessTechnology == 1)
        mode = QSystemNetworkInfo::GsmMode;
    if (radioAccessTechnology == 2)
        mode = QSystemNetworkInfo::WcdmaMode;

    if (mode != QSystemNetworkInfo::UnknownMode)
        emit networkSignalStrengthChanged(mode, cellSignalStrength);
}

void QSystemNetworkInfoPrivate::networkModeChanged(int newRadioAccessTechnology)
{
    radioAccessTechnology = newRadioAccessTechnology;
    networkModeChangeCheck();
}

void QSystemNetworkInfoPrivate::operatorNameChanged(uchar, QString name, QString, uint, uint)
{
    currentOperatorName = name;
    if (radioAccessTechnology == 1)
        emit networkNameChanged(QSystemNetworkInfo::GsmMode, currentOperatorName);
    if (radioAccessTechnology == 2)
        emit networkNameChanged(QSystemNetworkInfo::WcdmaMode, currentOperatorName);
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
            emit networkStatusChanged(QSystemNetworkInfo::GsmMode,
                                      networkStatus(QSystemNetworkInfo::GsmMode));
        if (radioAccessTechnology == 2)
            emit networkStatusChanged(QSystemNetworkInfo::WcdmaMode,
                                      networkStatus(QSystemNetworkInfo::WcdmaMode));
    }
    if (currentLac != newLac) {
        currentLac = newLac;
    }
    if (currentCellId != newCellId) {
        currentCellId = newCellId;
        emit cellIdChanged(newCellId);
    }
    if (currentMCC != newMobileCountryCode) {
        currentMCC = newMobileCountryCode;
        emit currentMobileCountryCodeChanged(currentMCC);
    }
    if (currentMNC != newMobileNetworkCode) {
        currentMNC = newMobileNetworkCode;
        emit currentMobileNetworkCodeChanged(currentMNC);
    }
}

#endif /* Maemo 5 */

void QSystemNetworkInfoPrivate::icdStatusChanged(QString, QString var2, QString, QString)
{
    if (var2 == "WLAN_INFRA") {
        QSystemNetworkInfo::NetworkStatus newWlanNetworkStatus =
                networkStatus(QSystemNetworkInfo::WlanMode);

        if (currentWlanNetworkStatus != newWlanNetworkStatus) {
            currentWlanNetworkStatus = newWlanNetworkStatus;
            emit networkStatusChanged(QSystemNetworkInfo::WlanMode, currentWlanNetworkStatus);
            networkModeChangeCheck();
        }
    }
}

void QSystemNetworkInfoPrivate::usbCableAction()
{
    if (currentEthernetSignalStrength != networkSignalStrength(QSystemNetworkInfo::EthernetMode)) {
        currentEthernetSignalStrength = networkSignalStrength(QSystemNetworkInfo::EthernetMode);
        emit networkSignalStrengthChanged(QSystemNetworkInfo::EthernetMode,
                                  currentEthernetSignalStrength);
    }
    QString newEthernetState;
    QString devFile = "/sys/class/net/usb0/operstate";
    QFileInfo fi(devFile);
    if (fi.exists()) {
        QFile rx(fi.absoluteFilePath());
        if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&rx);
            stream >> newEthernetState;
            rx.close();
        }
    }

    if (currentEthernetState != newEthernetState) {
        currentEthernetState = newEthernetState;
        emit networkStatusChanged(QSystemNetworkInfo::EthernetMode,
                                  networkStatus(QSystemNetworkInfo::EthernetMode));
    }

    networkModeChangeCheck();
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    if(networkStatus(QSystemNetworkInfo::EthernetMode) == QSystemNetworkInfo::Connected) {
        return QSystemNetworkInfo::EthernetMode;
    }
    if(networkStatus(QSystemNetworkInfo::WlanMode) == QSystemNetworkInfo::Connected) {
        return QSystemNetworkInfo::WlanMode;
    }

    if (radioAccessTechnology == 1)
        return QSystemNetworkInfo::GsmMode;
    if (radioAccessTechnology == 2)
        return QSystemNetworkInfo::WcdmaMode;

    return QSystemNetworkInfo::UnknownMode;
}

void QSystemNetworkInfoPrivate::networkModeChangeCheck()
{
    QSystemNetworkInfo::NetworkMode newNetworkMode = currentMode();
    if (currentNetworkMode != newNetworkMode)
    {
        currentNetworkMode = newNetworkMode;
        emit networkModeChanged(currentNetworkMode);
    }
}

void QSystemNetworkInfoPrivate::wlanSignalStrengthCheck()
{
    int strength = 0;
    strength =  networkSignalStrength(QSystemNetworkInfo::WlanMode);
    if (currentWlanSignalStrength != strength) {
        currentWlanSignalStrength = strength;
        emit networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, currentWlanSignalStrength);
    }
}

void QSystemNetworkInfoPrivate::bluetoothNetworkStatusCheck(QString device)
{
    if (device == "/org/freedesktop/Hal/devices/net_1b") {
        QTimer::singleShot(500, this, SLOT(usbCableAction()));
    } else {
        if (currentBluetoothNetworkStatus != networkStatus(QSystemNetworkInfo::BluetoothMode)) {
            currentBluetoothNetworkStatus = networkStatus(QSystemNetworkInfo::BluetoothMode);
            emit networkStatusChanged(QSystemNetworkInfo::BluetoothMode, currentBluetoothNetworkStatus);
        }
    }
}


void QSystemNetworkInfoPrivate::setWlanSignalStrengthCheckEnabled(bool enabled)
{
    if (enabled) {
        iWlanStrengthCheckEnabled++;
        if (!wlanSignalStrengthTimer->isActive())
            wlanSignalStrengthTimer->start(5000); //5 seconds interval
    } else {
        iWlanStrengthCheckEnabled--;
        if (iWlanStrengthCheckEnabled <= 0) {
            if(wlanSignalStrengthTimer->isActive())
                wlanSignalStrengthTimer->stop();
        }
    }
}

QSystemNetworkInfo::CellDataTechnology QSystemNetworkInfoPrivate::cellDataTechnology()
{
    const QString service = "com.nokia.csd.CSNet";
    const QString servicePath = "/com/nokia/csd/csnet";
    QDBusInterface ifc5(service, servicePath, "com.nokia.csd.CSNet.RadioAccess", QDBusConnection::systemBus());

    QDBusInterface ifc51(service, servicePath, "com.nokia.csd.CSNet", QDBusConnection::systemBus());
    QString activity = ifc51.property("Activity").toString();
    QVariant dataTechnology;

    if ( activity == "PacketData") {
        dataTechnology = ifc5.property("DataTechnology");
    } else {
        dataTechnology = ifc5.property("Technology");
    }

    QString dt = dataTechnology.isValid() ? dataTechnology.value<QString>() : "";
    currentCellDataTechnology = csdtToCellDataTechnology(dt);

    return currentCellDataTechnology;
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
    if(wid.screenCount() - 1 < screen) {
        return -1;
    }
    GConfItem currentBrightness("/system/osso/dsm/display/display_brightness");
    GConfItem maxBrightness("/system/osso/dsm/display/max_display_brightness_levels");
    if(maxBrightness.value().toInt()) {
        float retVal = 100 * (currentBrightness.value().toFloat() /
                              maxBrightness.value().toFloat());
        return retVal;
    }

    return -1;
}

float QSystemDisplayInfoPrivate::contrast(int screen)
{
    Q_UNUSED(screen);

    return 0.0;
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
        if (displayStatus == "off") {
            backlightState = QSystemDisplayInfo::BacklightStateOff;
        } else if (displayStatus == "dimmed") {
            backlightState = QSystemDisplayInfo::BacklightStateDimmed;
        } else if (displayStatus == "on") {
            backlightState = QSystemDisplayInfo::BacklightStateOn;
        }
    }
#endif
    return backlightState;
}

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QSystemStorageInfoLinuxCommonPrivate *parent)
        : QSystemStorageInfoLinuxCommonPrivate(parent)
{
}

QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent)
        : QSystemDeviceInfoLinuxCommonPrivate(parent), flightMode(0),gpioFD(-1)
{

 #if !defined(QT_NO_DBUS)
    previousPowerState = QSystemDeviceInfo::UnknownPower;
    setupProfile();
#endif
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    if (gpioFD == -1) {
        ::close(gpioFD);
        gpioFD = -1;
    }
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
    if (flightMode)
        return QSystemDeviceInfo::OfflineProfile;

    if (silentProfile)
        return vibratingAlertEnabled ? QSystemDeviceInfo::VibProfile : QSystemDeviceInfo::SilentProfile;

    if (beepProfile)
        return QSystemDeviceInfo::BeepProfile;

    if (ringingAlertVolume > 75)
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
 void QSystemDeviceInfoPrivate::bluezPropertyChanged(const QString&, QDBusVariant v)
 {
     //qDebug() << str << v.variant().toBool();
     emit bluetoothStateChanged(v.variant().toBool());
 }
#endif

#if !defined(QT_NO_DBUS)

void QSystemDeviceInfoPrivate::setupProfile()
{
    QDBusReply<quint32> radioStatesReply = QDBusConnection::systemBus().call(
                QDBusMessage::createMethodCall("com.nokia.mce",  "/com/nokia/mce/request",
                                               "com.nokia.mce.request", "get_radio_states"));
    if (radioStatesReply.isValid()) {
        quint32 radioStateFlags = radioStatesReply.value();
#define MCE_RADIO_STATE_WLAN            (1 << 2)
#define MCE_RADIO_STATE_BLUETOOTH       (1 << 3)

        flightMode = !(radioStateFlags & ~(MCE_RADIO_STATE_WLAN | MCE_RADIO_STATE_BLUETOOTH));
    }

    QDBusReply<QString> profileNameReply = QDBusConnection::sessionBus().call(
                QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                               "com.nokia.profiled", "get_profile"));

    if (profileNameReply.isValid()) {
        profileName = profileNameReply.value();
    }

    QDBusMessage ringingAlertTypeMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                      "com.nokia.profiled", "get_value");
    ringingAlertTypeMsg << profileName;
    ringingAlertTypeMsg << "ringing.alert.type";

    QDBusReply<QString> ringingAlertTypeReply = QDBusConnection::sessionBus().call(ringingAlertTypeMsg);

    if (ringingAlertTypeReply.isValid()) {
        silentProfile = QString::compare(ringingAlertTypeReply.value(), "silent", Qt::CaseInsensitive) == 0;
        beepProfile = QString::compare(ringingAlertTypeReply.value(), "beep", Qt::CaseInsensitive) == 0;
    }

    QDBusMessage vibratingAlertMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                    "com.nokia.profiled", "get_value");
    vibratingAlertMsg << profileName;
    vibratingAlertMsg << "vibrating.alert.enabled";

    QDBusReply<QString> vibratingAlertEnabledReply = QDBusConnection::sessionBus().call(vibratingAlertMsg);
    if (vibratingAlertEnabledReply.isValid()) {
        vibratingAlertEnabled = QString::compare(vibratingAlertEnabledReply.value(), "On", Qt::CaseInsensitive) == 0;
    }

    QDBusMessage ringingAlertVolumeMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                        "com.nokia.profiled", "get_value");
    ringingAlertVolumeMsg << profileName;
    ringingAlertVolumeMsg << "ringing.alert.volume";

    QDBusReply<QString> ringingAlertVolumeReply = QDBusConnection::sessionBus().call(ringingAlertVolumeMsg);
    if (ringingAlertVolumeReply.isValid()) {
        ringingAlertVolume = ringingAlertVolumeReply.value().toInt();
    }

    QDBusMessage smsAlertVolumeMsg = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled",
                                                                    "com.nokia.profiled", "get_value");
    smsAlertVolumeMsg << profileName;
    smsAlertVolumeMsg << "sms.alert.volume";

    QDBusReply<QString> smsAlertVolumeReply = QDBusConnection::sessionBus().call(smsAlertVolumeMsg);
    if (smsAlertVolumeReply.isValid()) {
        smsAlertVolume = smsAlertVolumeReply.value().toInt();
    }

    qDBusRegisterMetaType<ProfileDataValue>();
    qDBusRegisterMetaType<QList<ProfileDataValue> >();
}

void QSystemDeviceInfoPrivate::deviceModeChanged(QString newMode)
{
    bool previousFlightMode = flightMode;
    flightMode = newMode == "flight";
    if (previousFlightMode != flightMode)
        emit currentProfileChanged(currentProfile());
}

void QSystemDeviceInfoPrivate::profileChanged(bool changed, bool active, QString profile, QList<ProfileDataValue> values)
{
    if (active) {
        profileName = profile;
        foreach (const ProfileDataValue &value, values) {
            if (value.key == "ringing.alert.type") {
                silentProfile = QString::compare(value.val, "silent", Qt::CaseInsensitive) == 0;
                beepProfile = QString::compare(value.val, "beep", Qt::CaseInsensitive) == 0;
            }
            else if (value.key == "vibrating.alert.enabled")
                vibratingAlertEnabled = QString::compare(value.val, "On", Qt::CaseInsensitive) == 0;
            else if (value.key == "ringing.alert.volume")
                ringingAlertVolume = value.val.toInt();
            else if (value.key == "sms.alert.volume")
                smsAlertVolume = value.val.toInt();
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
    return smsAlertVolume;
}

int QSystemDeviceInfoPrivate::voiceRingtoneVolume()
{
    return ringingAlertVolume;
}

bool QSystemDeviceInfoPrivate::vibrationActive()
{
    return vibratingAlertEnabled;
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
    qDebug() << Q_FUNC_INFO << hash.result().toHex();

    return hash.result().toHex();
#endif
}

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QSystemScreenSaverLinuxCommonPrivate(parent),isInhibited(0)
{
    ssTimer = new QTimer(this);
#if !defined(QT_NO_DBUS)
    mceConnectionInterface = new QDBusInterface("com.nokia.mce",
                                                "/com/nokia/mce/request",
                                                "com.nokia.mce.request",
                                                QDBusConnection::systemBus());
#endif
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
    setScreenSaverInhibited(false);

#if !defined(QT_NO_DBUS)
    delete mceConnectionInterface, mceConnectionInterface = 0;
#endif
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    wakeUpDisplay();
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
#endif
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

    if( ((batState == QSystemDeviceInfo::WallPower || batState == QSystemDeviceInfo::WallPowerChargingBattery)
       && blankingItem == 2) || blankingItem == 4) {
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
#endif
    return ((displayOn && isBlankingInhibited) || (displayOn && isInhibited));
}

void QSystemScreenSaverPrivate::setScreenSaverInhibited(bool on)
{
    if (on) {
        setScreenSaverInhibit();
    } else {
        if (ssTimer->isActive()) {
            ssTimer->stop();
            isInhibited = false;
        }
    }
}

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent)
    : QSystemBatteryInfoLinuxCommonPrivate(parent)
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

#include "moc_qsysteminfo_maemo_p.cpp"

QTM_END_NAMESPACE
