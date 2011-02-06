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

#include "qsystemnetworkinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE
        Q_GLOBAL_STATIC(QSystemNetworkInfoPrivate, netInfoPrivate)

#ifdef QT_SIMULATOR
QSystemNetworkInfoPrivate *getSystemNetworkInfoPrivate() { return netInfoPrivate(); }
#endif

/*!
   \class QSystemNetworkInfo
   \ingroup systeminfo

  \inmodule QtSystemInfo

   \brief The QSystemNetworkInfo class provides access to network information.

    \fn QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)

        Constructs a QSystemNetworkInfo object with the given \a parent.
*/


        /*!
            \enum QSystemNetworkInfo::NetworkStatus
            This enum describes the status of the network connection:

            \value UndefinedStatus        There is no network device, or error.
            \value NoNetworkAvailable     There is no network available.
            \value EmergencyOnly          Emergency calls only.
            \value Searching              Searching for or connecting with the network.
            \value Busy                   Network is busy.
            \value Connected              Connected to network.
            \value HomeNetwork            On Home Network.
            \value Denied                 Network access denied.
            \value Roaming                On Roaming network.

          */
        /*!
            \enum QSystemNetworkInfo::NetworkMode
            This enum describes the type of network:

            \value UnknownMode             Unknown network, or network error.
            \value GsmMode                 Global System for Mobile (GSM) network.
            \value CdmaMode                Code division multiple access (CDMA) network.
            \value WcdmaMode               Wideband Code Division Multiple Access (W-CDMA) network.
            \value WlanMode                Wireless Local Area Network (WLAN) network.
            \value EthernetMode            Wired Local Area network.
            \value BluetoothMode           Bluetooth network.
            \value WimaxMode               Wimax network.
          */



        /*!
          \fn void QSystemNetworkInfo::networkStatusChanged(QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)

          This signal is emitted whenever the network status of \a mode changes, specified by \a status.
          */

        /*!
          \fn void QSystemNetworkInfo::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode,int strength)

          This signal is emitted whenever the network \a mode signal strength changes, specified by \a strength.
          */

        /*!
          \fn void QSystemNetworkInfo::currentMobileCountryCodeChanged(const QString &mcc)

          This signal is emitted whenever the Mobile Country Code changes, specified by \a mcc.
        */

        /*!
          \fn void QSystemNetworkInfo::currentMobileNetworkCodeChanged(const QString &mnc)

          This signal is emitted whenever the network Mobile Network Code changes, specified by \a mnc.
        */

        /*!
          \fn void QSystemNetworkInfo::networkNameChanged(QSystemNetworkInfo::NetworkMode mode,const QString & netName)

          This signal is emitted whenever the network \a mode name changes, specified by \a netName.

        */

        /*!
          \fn void QSystemNetworkInfo::networkModeChanged(QSystemNetworkInfo::NetworkMode mode)

          This signal is emitted whenever the network mode changes, specified by \a mode.
        */

/*!
    Constructs a QSystemNetworkInfo with the given \a parent.
*/
QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
   : QObject(parent), d(netInfoPrivate())
{
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");
}

/*!
 Destroys the QSystemNetworkInfo object.
*/
QSystemNetworkInfo::~QSystemNetworkInfo()
{
}

/*!
   Returns the status of the network \a mode.
*/
QSystemNetworkInfo::NetworkStatus QSystemNetworkInfo::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->networkStatus(mode);
}

/*!
   Returns the strength of the network signal, per network \a mode , 0 - 100 linear scaling,
   or -1 in the case of unknown network mode or error.

   In the case of QSystemNetworkInfo::EthMode, it will either be 100 for carrier active, or 0 for when
   there is no carrier or cable connected.
*/
int QSystemNetworkInfo::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    QSystemNetworkInfo::NetworkStatus info = netInfoPrivate()->networkStatus(mode);
    if(info  == QSystemNetworkInfo::UndefinedStatus
            || info == QSystemNetworkInfo::NoNetworkAvailable) {
        return 0;
    }
    return netInfoPrivate()->networkSignalStrength(mode);
}

/*!
 \property QSystemNetworkInfo::cellId
 \brief The devices Cell ID
   Returns the Cell ID of the connected tower or based station, or 0 if not connected.
*/
int QSystemNetworkInfo::cellId()
{
   return netInfoPrivate()->cellId();
}

/*!
 \property QSystemNetworkInfo::locationAreaCode
 \brief The LAC.

   Returns the Location Area Code. In the case of a Desktop computer, 0 is returned.
*/
int QSystemNetworkInfo::locationAreaCode()
{
   return netInfoPrivate()->locationAreaCode();
}

/*!
  \property QSystemNetworkInfo::currentMobileCountryCode
  \brief The current MCC.

   Returns the current Mobile Country Code. In the case of a Desktop computer, an empty string is returned.
*/
QString QSystemNetworkInfo::currentMobileCountryCode()
{
   return netInfoPrivate()->currentMobileCountryCode();
}

/*!
 \property QSystemNetworkInfo::currentMobileNetworkCode
 \brief The current MNC.

   Returns the current Mobile Network Code. In the case of a Desktop computer, an empty string is returned.
*/
QString QSystemNetworkInfo::currentMobileNetworkCode()
{
   return netInfoPrivate()->currentMobileNetworkCode();
}

/*!
 \property QSystemNetworkInfo::homeMobileCountryCode
 \brief The home MNC.

   Returns the home Mobile Country Code. In the case of a Desktop computer, an empty string is returned.
*/
QString QSystemNetworkInfo::homeMobileCountryCode()
{
   return netInfoPrivate()->homeMobileCountryCode();
}

/*!
 \property QSystemNetworkInfo::homeMobileNetworkCode
 \brief The home MCC.

   Returns the home Mobile Network Code. In the case of a Desktop computer, an empty string is returned.
   Note: Some platforms don't support retrieving this info. In this case the Network Code is
   returned only when the device is registered on home network.
*/
QString QSystemNetworkInfo::homeMobileNetworkCode()
{
   return netInfoPrivate()->homeMobileNetworkCode();
}

/*!
 Returns the name of the operator for the network \a mode.  For wlan this returns the network's current SSID.
In the case of a Desktop computer, an empty string is returned.
*/
QString QSystemNetworkInfo::networkName(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->networkName(mode);
}

/*!
 Returns the MAC address for the interface servicing the network \a mode.
 */
QString QSystemNetworkInfo::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->macAddress(mode);
}

/*!
 Returns the first found QNetworkInterface for type \a mode, or an invalid QNetworkInterface, if none is found.

 */
QNetworkInterface QSystemNetworkInfo::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
   return netInfoPrivate()->interfaceForMode(mode);
}

/*!
 \property QSystemNetworkInfo::currentMode

    Returns the current active mode. If more than one mode is active, returns the
     default or preferred mode. If no modes are active, returns UnknownMode.

*/
QSystemNetworkInfo::NetworkMode QSystemNetworkInfo::currentMode()
{
   return netInfoPrivate()->currentMode();
}

/*!
   \internal

   This function is called when the client connects to the networkSignalStrengthChanged()
   signal.
*/
void QSystemNetworkInfo::connectNotify(const char *signal)
{
   //check for networkSignalStrengthChanged() signal connect notification
   //This is not required on all platforms
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
                                 networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int))))) {
        netInfoPrivate()->setWlanSignalStrengthCheckEnabled(true);
    }
#endif
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileCountryCodeChanged(QString))))) {
        connect(d,SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SIGNAL(currentMobileCountryCodeChanged(QString)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileNetworkCodeChanged(QString))))) {
        connect(d,SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkModeChanged(QSystemNetworkInfo::NetworkMode))))) {
        connect(d,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))))) {
        connect(d,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))))) {
        connect(d,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus))))) {
        connect(d,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            cellIdChanged(int))))) {
        connect(d,SIGNAL(cellIdChanged(int)),
                this,SIGNAL(cellIdChanged(int)));
    }
}

/*!
   \internal

   This function is called when the client disconnects from the networkSignalStrengthChanged()
   signal.

   \sa connectNotify()
*/
void QSystemNetworkInfo::disconnectNotify(const char *signal)
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
        disconnect(d,SIGNAL(currentMobileCountryCodeChanged(QString)),
                this,SIGNAL(currentMobileCountryCodeChanged(QString)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentMobileNetworkCodeChanged(QString))))) {
        disconnect(d,SIGNAL(currentMobileNetworkCodeChanged(QString)),
                this,SIGNAL(currentMobileNetworkCodeChanged(QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkModeChanged(QSystemNetworkInfo::NetworkMode))))) {
        disconnect(d,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
                this,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))))) {
        disconnect(d,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
                this,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))))) {
        disconnect(d,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
                this,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus))))) {
        disconnect(d,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
                this,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
    }

}


#include "moc_qsystemnetworkinfo.cpp"

QTM_END_NAMESPACE
