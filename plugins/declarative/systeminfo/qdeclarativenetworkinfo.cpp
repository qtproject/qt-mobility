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
    \since Mobility 1.1

    This element is part of the \bold{QtMobility.systeminfo 1.1} module.
    It is a convience class to make QML usage easier.

    Note: To use notification signals, you need to set the monitor* properties to true.

    In QtMobility 1.2, the  NetworkInfo Element was changed to work in one network mode at a time. You use the property \bold{mode}
    to set which network mode it is in.

    The following table lists the \bold{incompatible} changes made to the start* functions between QtMobility 1.1 and 1.2,
    where they became Q_PROPERTY:



    \table
    \header
        \o QtMobility 1.1 \o QtMobility 1.2 \o Notes
    \row
        \o slot void startStatusChanged();
        \o void startStatusChanged(bool on);
        \o Became Q_PROPERTY monitorStatusChanges in QtMobility 1.2
    \row
        \o void startSignalStrengthChanged();
        \o void startSignalStrengthChanged(bool)
        \o Became Q_PROPERTY monitorSignalStrengthChanges in QtMobility 1.2
    \row
        \o void startNameChanged()
        \o void startNameChanged(bool on)
        \o Became Q_PROPERTY monitorNameChanges in QtMobility 1.2
    \row
        \o void startModeChanged()
        \o void startModeChanged(bool on)
        \o Became Q_PROPERTY monitorModeChanges in QtMobility 1.2
    \row
        \o void startCurrentMobileCountryCodeChanged()
        \o void startCurrentMobileCountryCodeChanged(bool on)
        \o Became Q_PROPERTY monitorCurrentMobileCountryCodeChanges in QtMobility 1.2
    \row
        \o void startCurrentMobileNetworkCodeChanged()
        \o void startCurrentMobileNetworkCodeChanged(bool on)
        \o Became Q_PROPERTY monitorCurrentMobileNetworkCodeChanges in QtMobility 1.2
    \row
        \o void useMode();
        \o void mode()
        \o Became Q_PROPERTY mode in QtMobility 1.2
    \endtable

    Example new usage:


QtMobility 1.2:
\code
        NetworkInfo {
            id: wlaninfo
            mode: NetworkInfo.WlanMode;


            monitorNameChanges: true;
            monitorSignalStrengthChanges: true
            monitorStatusChanges: true
            monitorModeChanges: true

            onStatusChanged : {
                img = getImage(newStatus)
            }
      }
\endcode

    To use this in a mode other than the default mode given by currentMode, set the mode with
    \a mode.

    \qml
    import Qt 4.7
    import QtMobility.systeminfo 1.1

    Rectangle {

        NetworkInfo {
            id: wlaninfo
            mode: NetworkInfo.WlanMode;
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
    \qmlsignal NetworkInfo::statusChanged(string networkStatus)
    \since Mobility 1.1

    This handler is called when network status has changed.
    Note: To receive this notification, you must first call \a startStatusChanged.
*/
/*!
    \qmlsignal NetworkInfo::signalStrengthChanged(int signalStrength)
    \since Mobility 1.1

    This handler is called when network signal strength has changed.
    Note: To receive this notification, you must first call \a startSignalStrengthChanged.
*/
/*!
    \qmlsignal NetworkInfo::nameChanged(string networkName)
    \since Mobility 1.1

    This handler is called when network name has changed.
    Note: To receive this notification, you must first call \a startNameChanged.
*/
/*!
    \qmlsignal NetworkInfo::modeChanged()
    \since Mobility 1.1

    This handler is called when the systems default network mode has changed.
    Note: To receive this notification, you must first call \a startModeChanged.
*/
/*!
    \qmlsignal NetworkInfo::currentMobileCountryCodeChanged(string mcc)
    \since Mobility 1.1

    This handler is called when network MCC has changed.
    Note: To receive this notification, you must first call \a startCurrentMobileCountryCodeChanged.
*/
/*!
    \qmlsignal NetworkInfo::currentMobileNetworkCodeChanged(string mnc)
    \since Mobility 1.1

    This handler is called when network MNC has changed.
    Note: To receive this notification, you must first call \a startCurrentMobileNetworkCodeChanged.
*/
/*!
    \qmlsignal NetworkInfo::cellIdChanges(int cellId)
    \since Mobility 1.1

    This handler is called when cell id has changed.
    Note: To receive this notification, you must first call \a startCellIdChanged.
*/
/*!
    \qmlsignal NetworkInfo::cellDataTechnologyChanges(QSystemNetworkInfo::CellDataTechnology cellTech)
    \since Mobility 1.1

    This handler is called when cell data technology has changed.
    Note: To receive this notification, you must first call \a startCellDataChanged.
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
    \qmlproperty enumeration NetworkInfo::mode
    \since Mobility 1.1

    Sets this NetworkInfo to use QSystemNetworkInfo::NetworkModes. Does not set the mode of the underlaying system.

    If not set, the default is whatever defaultMode is.
*/

void QDeclarativeNetworkInfo::useMode(QSystemNetworkInfo::NetworkMode mode)
{
    m_mode = mode;
}

/*!
    \qmlproperty string NetworkInfo::networkStatus
    \since Mobility 1.1

    Returns this NetworkInfo's network status.

    The following table shows the mapping between the NetworkInfo NetworkStatus property and the QSystemNetworkInfo::NetworkStatus enum.

\table
    \header
        \o NetworkInfo.networkStatus \o QSystemNetworkInfo::NetworkStatus
        \row
            \o "Undefined"
            \o QSystemNetworkInfo::UndefinedStatus
        \row
            \o "No Network Available"
            \o QSystemNetworkInfo::NoNetworkAvailable
        \row
            \o "Emergency Only"
            \o  QSystemNetworkInfo::EmergencyOnly
        \row
            \o "Searching"
            \o QSystemNetworkInfo::Searching
        \row
            \o "Busy"
            \o QSystemNetworkInfo::Busy
        \row
            \o "Connected"
            \o QSystemNetworkInfo::Connected
        \row
            \o "Home Network"
            \o QSystemNetworkInfo::HomeNetwork
        \row
            \o "Denied"
            \o QSystemNetworkInfo::Denied
        \row
            \o "Roaming"
            \o QSystemNetworkInfo::Roaming
 \endtable
*/
QString QDeclarativeNetworkInfo::networkStatus()
{
    return statusToString(QSystemNetworkInfo::networkStatus(m_mode));
}

/*!
    \qmlproperty int NetworkInfo::networkSignalStrength
    \since Mobility 1.1

    Returns this NetworkInfo's network signal strength.
*/
int QDeclarativeNetworkInfo::networkSignalStrength()
{
    return QSystemNetworkInfo::networkSignalStrength(m_mode);
}

/*!
    \qmlproperty string NetworkInfo::macAddress
    \since Mobility 1.1

    Returns this NetworkInfo's network interface's MAC address.
*/
QString QDeclarativeNetworkInfo::macAddress()
{
    return QSystemNetworkInfo::macAddress(m_mode);
}

/*!
    \qmlproperty string NetworkInfo::networkName
    \since Mobility 1.1

    Returns this NetworkInfo's network name.
*/
QString QDeclarativeNetworkInfo::networkName()
{
    return QSystemNetworkInfo::networkName(m_mode);
}

/*!
    \qmlmethod QNetworkInterface NetworkInfo::interfaceForMode()
    \since Mobility 1.1
    Returns the QNetworkInterface for this NetworkInfo.

*/
QNetworkInterface QDeclarativeNetworkInfo::interfaceForMode()
{
    return QSystemNetworkInfo::interfaceForMode(m_mode);
}

/*!
   This function starts the startStatusChanged notification
   \since Mobility 1.1

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
    Use the statusChanged signal.
    \since Mobility 1.2
    */
bool QDeclarativeNetworkInfo::monitorStatusChanges()
{
    return monitoringStatusChanges;
}


/*!
   This function starts the signalStrengthChanged notification.
   \since Mobility 1.1

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
    Use the signalStrengthChanged signal.
    \since Mobility 1.2
    */
bool QDeclarativeNetworkInfo::monitorSignalStrengthChanges()
{
    return monitoringSignalStrengthChanges;
}


/*!
   This function starts the nameChanged notification.
   \since Mobility 1.1

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
    Use the nameChanged signal.
    \since Mobility 1.2
    */
bool QDeclarativeNetworkInfo::monitorNameChanges()
{
    return monitoringNameChanges;
}


/*!
   This function starts the modeChanged notification.
   \since Mobility 1.1

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
    Use the modeChanged signal.
    \since Mobility 1.2
    */
bool QDeclarativeNetworkInfo::monitorModeChanges()
{
    return monitoringModeChanges;
}


/*!

   This function starts the currentMobileCountryCodeChanged notification.
   \since Mobility 1.1

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
    \qmlproperty bool NetworkInfo::monitorCurrentMobileCountryCodeChanges
    Use the currentMobileCountryCodeChanged signal.
    \since Mobility 1.2
    */
bool QDeclarativeNetworkInfo::monitorCurrentMobileCountryCodeChanges()
{
    return monitoringCurrentMobileCountryCodeChanges;
}


/*!
   This function starts the currentMobileNetworkCodeChanged notification.
   \since Mobility 1.1

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
    Use the currentMobileNetworkCodeChanged signal.
    \since Mobility 1.2
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
    \internal
*/

QSystemNetworkInfo::NetworkMode QDeclarativeNetworkInfo::mode()
{
    return m_mode;
}


/*!
    \qmlproperty bool NetworkInfo::monitorCellDataChanges
    Use the cellDataTechnologyChanges signal.
    \since Mobility 1.2

   \sa QSystemNetworkInfo::cellDataTechnologyChanged
  */
bool QDeclarativeNetworkInfo::monitorCellDataChanges()
{
    return monitoringCellDataTechnologyChanges;
}

/*!
   This function starts the cellDataTechnologyChanges notification
   \since Mobility 1.2

*/
void QDeclarativeNetworkInfo::startCellDataChanged(bool on)
{
    monitoringCellDataTechnologyChanges = on;
    if (on) {
        connect(netInfo(),SIGNAL(cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology)),
                this,SIGNAL(cellDataTechnologyChanges(QSystemNetworkInfo::CellDataTechnology)),Qt::UniqueConnection);
    } else {
        disconnect(netInfo(),SIGNAL(cellDataTechnologyChanged(QSystemNetworkInfo::CellDataTechnology)),
                   this,SIGNAL(cellDataTechnologyChanges(QSystemNetworkInfo::CellDataTechnology)));
    }
}

/*!
    \qmlproperty bool NetworkInfo::monitorCellIdChanges
    Use the cellIdChanges signal.
    \since Mobility 1.2
    */
bool QDeclarativeNetworkInfo::monitorCellIdChanges()
{
    return monitoringCellIdChanges;
}

/*!
   This function starts the cellIdChanges notification
   \since Mobility 1.2

   \sa QSystemNetworkInfo::cellIdChanged

*/
void QDeclarativeNetworkInfo::startCellIdChanged(bool on)
{
    monitoringCellIdChanges = on;
    if (on) {
        connect(netInfo(),SIGNAL(cellIdChanged(int)),
                this,SIGNAL(cellIdChanges(int)),Qt::UniqueConnection);
    } else {
        disconnect(netInfo(),SIGNAL(cellIdChanged(int)),
                   this,SIGNAL(cellIdChanges(int)));
    }
}

