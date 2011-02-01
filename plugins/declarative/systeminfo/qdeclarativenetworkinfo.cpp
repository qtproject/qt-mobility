/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
***********************************/

#include "qdeclarativenetworkinfo_p.h"

#include "qsystemnetworkinfo.h"
#include <QMetaType>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemNetworkInfo, netInfo)

/*!
    \qmlclass NetworkInfo QDeclarativeNetworkInfo
    \brief The NetworkInfo element allows you to get information and receive notifications from the network.
    \inherits QObject

    \ingroup qml-systeminfo

    This element is part of the \bold{QtMobility.systeminfo 1.1} module.
    It is a convience class to make QML usage easier.

    Note: To use notification signals, you need to set the monitor* properties to true.

    To use this in a mode other than the default mode given by currentMode, set the mode with
    \a useMode.

    \qml
    import Qt 4.7
    import QtMobility.systeminfo 1.0

    Rectangle {

        NetworkInfo {
            id: wlaninfo
            useMode: NetworkInfo.WlanMode;
            monitorNameChanges: true
            monitorSignalStrengthChanges: true
        }
        Text {
            id: signame
            color: "white"
            text:  wlaninfo.networkName +" "+wlaninfo.networkSignalStrength;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;
        }
    }
    \endqml

\sa QSystemNetworkInfo
*/
/*!
    \qmlsignal NetworkInfo::statusChanged()

    This handler is called when network status has changed.
    Note: To receive this notification, you must first call \a startStatusChanged.
*/
/*!
    \qmlsignal NetworkInfo::signalStrengthChanged()

    This handler is called when network signal strength has changed.
    Note: To receive this notification, you must first call \a startSignalStrengthChanged.
*/
/*!
    \qmlsignal NetworkInfo::nameChanged()

    This handler is called when network name has changed.
    Note: To receive this notification, you must first call \a startNameChanged.
*/
/*!
    \qmlsignal NetworkInfo::modeChanged()

    This handler is called when the systems default network mode has changed.
    Note: To receive this notification, you must first call \a startModeChanged.
*/
/*!
    \qmlsignal NetworkInfo::currentMobileCountryCodeChanged()

    This handler is called when network MCC has changed.
    Note: To receive this notification, you must first call \a startCurrentMobileCountryCodeChanged.
*/
/*!
    \qmlsignal NetworkInfo::currentMobileNetworkCodeChanged()

    This handler is called when network MNC has changed.
    Note: To receive this notification, you must first call \a startCurrentMobileNetworkCodeChanged.
*/

QDeclarativeNetworkInfo::QDeclarativeNetworkInfo(QObject *parent) :
    QSystemNetworkInfo(parent)
{
    m_mode = currentMode();
}

QDeclarativeNetworkInfo::~QDeclarativeNetworkInfo()
{
}

/*!
    \qmlproperty enumeration NetworkInfo::useThisMode

    Sets this NetworkInfo to use QSystemNetworkInfo::NetworkModes. Does not set the mode of the underlaying system.

    If not set, the default is whatever defaultMode is.
*/

void QDeclarativeNetworkInfo::useThisMode(QSystemNetworkInfo::NetworkMode mode)
{
    m_mode = mode;
}

/*!
    \qmlproperty string NetworkInfo::networkStatus

    Returns this NetworkInfo's network status.
*/
QString QDeclarativeNetworkInfo::networkStatus()
{
    return statusToString(QSystemNetworkInfo::networkStatus(m_mode));
}

/*!
    \qmlproperty int NetworkInfo::networkSignalStrength

    Returns this NetworkInfo's network signal strength.
*/
int QDeclarativeNetworkInfo::networkSignalStrength()
{
    return QSystemNetworkInfo::networkSignalStrength(m_mode);
}

/*!
    \qmlproperty string NetworkInfo::macAddress

    Returns this NetworkInfo's network interface's MAC address.
*/
QString QDeclarativeNetworkInfo::macAddress()
{
    return QSystemNetworkInfo::macAddress(m_mode);
}

/*!
    \qmlproperty string NetworkInfo::networkName

    Returns this NetworkInfo's network name.
*/
QString QDeclarativeNetworkInfo::networkName()
{
    return QSystemNetworkInfo::networkName(m_mode);
}

/*!
    \qmlmethod QNetworkInterface NetworkInfo::interfaceForMode()
   Returns the QNetworkInterface for this NetworkInfo.

*/
QNetworkInterface QDeclarativeNetworkInfo::interfaceForMode()
{
    return QSystemNetworkInfo::interfaceForMode(m_mode);
}

/*!
   This function starts the startBatteryStatusChanged notification

*/
void QDeclarativeNetworkInfo::startStatusChanged(bool on)
{
    monitoringStatusChanges = on;
    if(on) {
        connect(netInfo(),SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),Qt::UniqueConnection);
    } else {
        disconnect(netInfo(),SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitorStatusChanges
    \brief Use the statusChanged signal.
  */
bool QDeclarativeNetworkInfo::monitorStatusChanges()
{
    return monitoringStatusChanges;
}


/*!
   This function starts the signalStrengthChanged notification.

*/
void QDeclarativeNetworkInfo::startSignalStrengthChanged(bool on)
{
    monitoringSignalStrengthChanges = on;
    if(monitoringSignalStrengthChanges) {
        connect(netInfo(),SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),Qt::UniqueConnection);
    } else {
        disconnect(netInfo(),SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                   this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitorSignalStrengthChanges
    \brief Use the monitorSignalStrengthChanges signal.
  */
bool QDeclarativeNetworkInfo::monitorSignalStrengthChanges()
{
    return monitoringSignalStrengthChanges;
}


/*!
   This function starts the statusChanged notification.

*/
void QDeclarativeNetworkInfo::startNameChanged(bool on)
{
    monitoringNameChanges = on;
    if(on) {
    connect(netInfo(),SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
            this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),Qt::UniqueConnection);
} else {
        disconnect(netInfo(),SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitorNameChanges
    \brief Use the monitorNameChanges signal.
  */
bool QDeclarativeNetworkInfo::monitorNameChanges()
{
    return monitoringNameChanges;
}


/*!
   This function starts the modeChanged notification.

*/
void QDeclarativeNetworkInfo::startModeChanged(bool on)
{
    monitoringModeChanges = on;
    if(on) {
    connect(netInfo(),SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
            this,SLOT(networkModeChanged(QSystemNetworkInfo::NetworkMode)),Qt::UniqueConnection);
} else {
        disconnect(netInfo(),SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SLOT(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitorModeChanges
    \brief Use the monitorModeChanges signal.
  */
bool QDeclarativeNetworkInfo::monitorModeChanges()
{
    return monitoringModeChanges;
}


/*!

   This function starts the currentMobileCountryCodeChanged notification.

*/
void QDeclarativeNetworkInfo::startCurrentMobileCountryCodeChanged(bool on)
{
    monitoringCurrentMobileCountryCodeChanges = on;
    if(on) {
        connect(netInfo(),SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SIGNAL(currentMobileCountryCodeChanged(QString)),Qt::UniqueConnection);
    } else {
        disconnect(netInfo(),SIGNAL(currentMobileCountryCodeChanged(QString)),
                   this,SIGNAL(currentMobileCountryCodeChanged(QString)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitoringCurrentMobileCountryCodeChanges
    \brief Use the monitoringCurrentMobileCountryCodeChanges signal.
  */
bool QDeclarativeNetworkInfo::monitorCurrentMobileCountryCodeChanges()
{
    return monitoringCurrentMobileCountryCodeChanges;
}


/*!
   This function starts the currentMobileNetworkCodeChanged notification.

*/
void QDeclarativeNetworkInfo::startCurrentMobileNetworkCodeChanged(bool on)
{
    monitoringCurrentMobileNetworkCodeChanges = on;
    if (on) {
        connect(netInfo(),SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SIGNAL(currentMobileNetworkCodeChanged(QString)),Qt::UniqueConnection);
    } else {
        disconnect(netInfo(),SIGNAL(currentMobileNetworkCodeChanged(QString)),
                   this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitorCurrentMobileNetworkCodeChanges
    \brief Use the monitorCurrentMobileNetworkCodeChanges signal.
  */
bool QDeclarativeNetworkInfo::monitorCurrentMobileNetworkCodeChanges()
{
    return monitoringCurrentMobileNetworkCodeChanges;
}

/*!
   \internal

   This function is called when the client connects from the networkSignalStrengthChanged()
   notification.

*/
void QDeclarativeNetworkInfo::networkStatusChanged(QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)
{
    if(mode == m_mode) {
        Q_EMIT statusChanged(statusToString(status));
    }
}

/*!
   \internal
    Converts QSystemNetworkStatus to user string

*/
QString QDeclarativeNetworkInfo::statusToString(QSystemNetworkInfo::NetworkStatus status)
{
    switch(status) {
    case QSystemNetworkInfo::UndefinedStatus:
        return QLatin1String("Undefined");
    case QSystemNetworkInfo::NoNetworkAvailable:
        return QLatin1String("No Network Available");
    case QSystemNetworkInfo::EmergencyOnly:
        return QLatin1String("Emergency Only");
    case QSystemNetworkInfo::Searching:
        return QLatin1String("Searching");
    case QSystemNetworkInfo::Busy:
        return QLatin1String("Busy");
    case QSystemNetworkInfo::Connected:
        return QLatin1String("Connected");
    case QSystemNetworkInfo::HomeNetwork:
        return QLatin1String("Home Network");
    case QSystemNetworkInfo::Denied:
        return QLatin1String("Denied");
    case QSystemNetworkInfo::Roaming:
        return QLatin1String("Roaming");
    }
    return QString();
}

/*!
   \internal
    Converts QSYstemNetworkMode to user string

*/
QString QDeclarativeNetworkInfo::modeToString(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::UnknownMode:
        return QLatin1String("Unknown");
    case QSystemNetworkInfo::GsmMode:
        return QLatin1String("Gsm");
    case QSystemNetworkInfo::CdmaMode:
        return QLatin1String("Cdma");
    case QSystemNetworkInfo::WcdmaMode:
        return QLatin1String("Wcdma");
    case QSystemNetworkInfo::WlanMode:
        return QLatin1String("Wlan");
    case QSystemNetworkInfo::EthernetMode:
        return QLatin1String("Ethernet");
    case QSystemNetworkInfo::BluetoothMode:
        return QLatin1String("Bluetooth");
    case QSystemNetworkInfo::WimaxMode:
        return QLatin1String("Wimax");
    case QSystemNetworkInfo::GprsMode:
        return QLatin1String("Gprs");
    case QSystemNetworkInfo::EdgeMode:
        return QLatin1String("Edge");
    case QSystemNetworkInfo::HspaMode:
        return QLatin1String("Hspa");
    case QSystemNetworkInfo::LteMode:
        return QLatin1String("Lte");
    };
    return QString();
}

/*!
   \internal

   This function is called when the client connects from the networkSignalStrengthChanged()
   signal.

*/
void QDeclarativeNetworkInfo::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength)
{
    if(mode == m_mode) {
        Q_EMIT signalStrengthChanged(strength);
    }
}


/*!
   \internal

   This function is called when the client connects from the networkSignalStrengthChanged()
   signal.

*/
void QDeclarativeNetworkInfo::networkNameChanged(QSystemNetworkInfo::NetworkMode mode,const QString &name)
{
    if(mode == m_mode) {
        Q_EMIT nameChanged(name);
    }
}

/*!
   \internal

   This function is called when the client connects from the networkSignalStrengthChanged()
   signal.

*/
void QDeclarativeNetworkInfo::networkModeChanged(QSystemNetworkInfo::NetworkMode mode)
{
    if(mode == m_mode) {
        Q_EMIT modeChanged();
    }
}

/*!
   \internal

   This function returns a list of available QSystemNetworkInfo::NetworkMode.
*/
QList<QSystemNetworkInfo::NetworkMode> QDeclarativeNetworkInfo::availableModes()
{
    QList<QSystemNetworkInfo::NetworkMode> list;

    for(int i = 0; i < 7; i++) {
        if(QSystemNetworkInfo::networkStatus(static_cast<QSystemNetworkInfo::NetworkMode> (i)) != QSystemNetworkInfo::UndefinedStatus) {
            list.append(static_cast<QSystemNetworkInfo::NetworkMode> (i));
        }
    }
    return list;
}

/*!
    \qmlmethod NetworkMode NetworkInfo::useMode()
    This function returns the mode set by useMode(QSystemNetworkInfo::NetworkMode mode);

*/
QSystemNetworkInfo::NetworkMode QDeclarativeNetworkInfo::useMode()
{
    return m_mode;
}
