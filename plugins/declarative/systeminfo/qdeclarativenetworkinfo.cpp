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

#include <qsystemnetworkinfo.h>
#include <QMetaType>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemNetworkInfo, netInfo)

QDeclarativeNetworkInfo::QDeclarativeNetworkInfo(QObject *parent) :
    QSystemNetworkInfo(parent)
{
    m_mode = currentMode();
}

QDeclarativeNetworkInfo::~QDeclarativeNetworkInfo()
{
}

void QDeclarativeNetworkInfo::setMode(QSystemNetworkInfo::NetworkMode mode)
{
    m_mode = mode;
}

QString QDeclarativeNetworkInfo::networkStatus()
{
    return statusToString(QSystemNetworkInfo::networkStatus(m_mode));
}

int QDeclarativeNetworkInfo::networkSignalStrength()
{
    return QSystemNetworkInfo::networkSignalStrength(m_mode);
}

QString QDeclarativeNetworkInfo::macAddress()
{
    return QSystemNetworkInfo::macAddress(m_mode);
}

QString QDeclarativeNetworkInfo::networkName()
{
    return QSystemNetworkInfo::networkName(m_mode);
}

QNetworkInterface QDeclarativeNetworkInfo::interfaceForMode()
{
    return QSystemNetworkInfo::interfaceForMode(m_mode);
}


/*!
   \internal

   This function is called when the client connects to the networkSignalStrengthChanged()
   signal.
*/
void QDeclarativeNetworkInfo::connectNotify(const char *signal)
{
//   //check for networkSignalStrengthChanged() signal connect notification
//   //This is not required on all platforms
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
                                 networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int))))) {
        netInfoPrivate()->setWlanSignalStrengthCheckEnabled(true);
    }
#endif
    qDebug() << __FUNCTION__;
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileCountryCodeChanged(QString))))) {
        connect(netInfo(),SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SLOT(currentMobileCountryCodeChanged(QString)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileNetworkCodeChanged(QString))))) {
        connect(netInfo(),SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SLOT(currentMobileNetworkCodeChanged(QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            modeChanged())))) {
        connect(netInfo(),SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SLOT(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            nameChanged(QSystemNetworkInfo::NetworkMode,QString))))) {
        connect(netInfo(),SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            signalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))))) {
        connect(netInfo(),SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            statusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus))))) {
        connect(netInfo(),SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }
}

/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::disconnectNotify(const char *signal)
{
   //check for networkSignalStrengthChanged() signal disconnect notification
   //This is not required on all platforms
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
                                 networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int))))) {
        netInfoPrivate()->setWlanSignalStrengthCheckEnabled(false);
    }
#endif
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileCountryCodeChanged(QString))))) {
        disconnect(netInfo(),SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SLOT(currentMobileCountryCodeChanged(QString)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileNetworkCodeChanged(QString))))) {
        disconnect(netInfo(),SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkModeChanged(QSystemNetworkInfo::NetworkMode))))) {
        disconnect(netInfo(),SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))))) {
        disconnect(netInfo(),SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))))) {
        disconnect(netInfo(),SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus))))) {
        disconnect(netInfo(),SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }

}

/*!

   This function is needed to start statusChanged signals

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::startStatusChanged()
{
    connect(netInfo(),SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
            this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
}

/*!

   This function is needed to start signalStrengthChanged signals

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::startSignalStrengthChanged()
{
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int))))) {
        netInfoPrivate()->setWlanSignalStrengthCheckEnabled(true);
    }
#endif
    connect(netInfo(),SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
            this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
}

/*!

   This function is needed to start nameChanged signals

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::startNameChanged()
{
    connect(netInfo(),SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
            this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
}

/*!

   This function is needed to start modeChanged signals

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::startModeChanged()
{
    connect(netInfo(),SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
            this,SLOT(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
}

/*!

   This function is needed to start startCurrentMobileCountryCodeChanged signals

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::startCurrentMobileCountryCodeChanged()
{
    connect(netInfo(),SIGNAL(currentMobileCountryCodeChanged(QString)),
            this,SIGNAL(currentMobileCountryCodeChanged(QString)));
}

/*!

   This function is needed to start startCurrentMobileNetworkCodeChanged signals

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::startCurrentMobileNetworkCodeChanged()
{
    connect(netInfo(),SIGNAL(currentMobileNetworkCodeChanged(QString)),
            this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
}

/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::networkStatusChanged(QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)
{
    if(mode == m_mode) {
        Q_EMIT statusChanged(statusToString(status));
    }
}

QString QDeclarativeNetworkInfo::statusToString(QSystemNetworkInfo::NetworkStatus status)
{
    switch(status) {
    case QSystemNetworkInfo::UndefinedStatus:
        return "Undefined";
        break;
    case QSystemNetworkInfo::NoNetworkAvailable:
        return "No Network Available";
        break;
    case QSystemNetworkInfo::EmergencyOnly:
        return "Emergency Only";
        break;
    case QSystemNetworkInfo::Searching:
        return "Searching";
        break;
    case QSystemNetworkInfo::Busy:
        return "Busy";
        break;
    case QSystemNetworkInfo::Connected:
        return "Connected";
        break;
    case QSystemNetworkInfo::HomeNetwork:
        return "Home Network";
        break;
    case QSystemNetworkInfo::Denied:
        return "Denied";
        break;
    case QSystemNetworkInfo::Roaming:
        return "Roaming";
        break;
    }
    return QString();
}

QString QDeclarativeNetworkInfo::modeToString(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::UnknownMode:
        return "Unknown";
        break;
    case QSystemNetworkInfo::GsmMode:
        return "Gsm";
        break;
    case QSystemNetworkInfo::CdmaMode:
        return "Cdma";
        break;
    case QSystemNetworkInfo::WcdmaMode:
        return "Wcdma";
        break;
    case QSystemNetworkInfo::WlanMode:
        return "Wlan";
        break;
    case QSystemNetworkInfo::EthernetMode:
        return "Ethernet";
        break;
    case QSystemNetworkInfo::BluetoothMode:
        return "Bluetooth";
        break;
    case QSystemNetworkInfo::WimaxMode:
        return "Wimax";
        break;
    };
    return QString();
}

/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength)
{
    if(mode == m_mode) {
        Q_EMIT signalStrengthChanged(strength);
    }
}


/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::networkNameChanged(QSystemNetworkInfo::NetworkMode mode,const QString &name)
{
    if(mode == m_mode) {
        Q_EMIT nameChanged(name);
    }
}

/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QDeclarativeNetworkInfo::networkModeChanged(QSystemNetworkInfo::NetworkMode mode)
{
    if(mode == m_mode) {
        Q_EMIT modeChanged();
    }
}

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

