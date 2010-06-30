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

#include "qmobilityglobal.h"
#include "qsystemgeneralinfo.h"
#include "qsysteminfocommon.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QMetaType>

#include <locale.h>

QTM_BEGIN_NAMESPACE

  /*!
    \class QSystemInfo

    \ingroup systeminfo

    \brief The QSystemInfo class provides access to various general information from the system.

    In the future, parts of the QtSystemInfo API may be moved and renamed into an existing Qt class that provides similiar functionality.

\table
\header
    \o Class
\row
    \o QSystemInfo::currentLanguage
\row
    \o QSystemInfo::availableLanguages
\row
    \o QSystemInfo::currentCountryCode
\row
    \o QSystemDisplayInfo::displayBrightness
\row
    \o QSystemDisplayInfo::colorDepth
\row
    \o QSystemScreenSaver::screenSaverInhibited
\row
    \o QSystemScreenSaver::setScreenSaverInhibit
\endtable

    Platform notes
    Some functionality may or may not be supported on various platforms. Depending on if there
is a reliable way to gather such information.
*/

/*!
    \enum QSystemInfo::Version
    This enum describes the version component.

    \value Os                    Operating system version / platform ID.
    \value QtCore                Qt libs version.
    \value Firmware              Version of (flashable) system as a whole.

  */
/*!
    \enum QSystemInfo::Feature
    This enum describes the features of the device or computer.

    \value BluetoothFeature       Bluetooth feature available.
    \value CameraFeature          Camera feature available.
    \value FmradioFeature         FM Radio feature available.
    \value IrFeature              Infrared feature available.
    \value LedFeature             LED's feature available.
    \value MemcardFeature         Memory card feature available.
    \value UsbFeature             Universal System Bus (USB) feature available.
    \value VibFeature             Vibration feature available.
    \value WlanFeature            Wireless Local Area Network (WLAN) feature available.
    \value SimFeature             Subscriber Identity Module (SIM) available.
    \value LocationFeature        Global Positioning System (GPS) and/or other location feature available.
    \value VideoOutFeature        Video out feature available.
    \value HapticsFeature         Haptics feature available.
  */

  /*!
    \class QSystemNetworkInfo

    \ingroup systeminfo

    \brief The QSystemNetworkInfo class provides access to network information from the system.

  */
/*!
    \enum QSystemNetworkInfo::NetworkStatus
    This enum describes the status of the network connection:

    \value UndefinedStatus        There is no network device, or error.
    \value NoNetworkAvailable     There is no network available.
    \value EmergencyOnly          Emergency calls only.
    \value Searching              Searching for or connecting with the network.
    \value Busy                   Network is busy.
    \value Connected              Connected to newtwork.
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
    \class QSystemDisplayInfo

    \ingroup systeminfo

    \brief The QSystemDisplayInfo class provides access to display information from the system.

  */

  /*!
    \class QSystemStorageInfo

    \ingroup systeminfo

    \brief The QSystemStorageInfo class provides access to memory and disk information from the system.

  */

/*!
    \enum QSystemStorageInfo::DriveType
    This enum describes the type of drive or volume

    \value NoDrive               Drive type undetermined.
    \value InternalDrive         Is internal drive.
    \value RemovableDrive        Is removable.
    \value RemoteDrive           Is a network drive.
    \value CdromDrive            Is a cd rom drive.
*/

/*!
   \enum QSystemDisplayInfo::DisplayOrientation
  This enum describes the current orientation of the display.

     \value Unknown              Orientation could not be determined.
     \value Landscape            Orientation is in landscape.
     \value Portrait             Orientation is in portrait.
     \value InvertedLandscape    Orientation is landscape inverted.
     \value InvertedPortrait     Orientation is portrait inverted.
*/

        /*!
    \class QSystemDeviceInfo

    \ingroup systeminfo

    \brief The QSystemDeviceInfo class provides access to device
information from the system.

  */
/*!
  \fn void QSystemDeviceInfo::batteryLevelChanged(int level)

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */

/*!
  \fn void QSystemDeviceInfo::batteryStatusChanged(QSystemDeviceInfo::BatteryStatus status)

  This signal is emitted when battery status has changed.
  \a status is the new status.
 */

   /*!
  \fn void QSystemDeviceInfo::powerStateChanged(QSystemDeviceInfo::PowerState state)

  This signal is emitted when the power state has changed, such as when a phone gets plugged in to the wall.
  \a state is the new power state.
 */

/*!
  \fn  void QSystemDeviceInfo::currentProfileChanged(QSystemDeviceInfo::Profile profile)

  This signal is emitted whenever the users active profile changes, specified by \a profile.
*/


/*!
    \enum QSystemDeviceInfo::BatteryStatus
    This enum describes the status of the main battery.

    \value NoBatteryLevel          Battery level undetermined.
    \value BatteryCritical         Battery level is critical 3% or less.
    \value BatteryVeryLow          Battery level is very low, 10% or less.
    \value BatteryLow              Battery level is low 40% or less.
    \value BatteryNormal           Battery level is above 40%.

  */
/*!
    \enum QSystemDeviceInfo::PowerState
    This enum describes the power state:

    \value UnknownPower                   Power error.
    \value BatteryPower                   On battery power.
    \value WallPower                      On wall power.
    \value WallPowerChargingBattery       On wall power and charging main battery.

  */
/*!
    \enum QSystemDeviceInfo::Profile
    This enum describes the current operating profile of the device or computer.

    \value UnknownProfile          Profile unknown or error.
    \value SilentProfile           Silent profile.
    \value NormalProfile           Normal profile.
    \value LoudProfile             Loud profile.
    \value VibProfile              Vibrate profile.
    \value OfflineProfile          Offline profile.
    \value PowersaveProfile        Powersave profile.
    \value CustomProfile           Custom profile.

  */

/*!
    \enum QSystemDeviceInfo::SimStatus
    This enum describes the status is the sim card or cards.

    \value SimNotAvailable         SIM is not available on this device.
    \value SingleSimAvailable         One SIM card is available on this.
    \value DualSimAvailable           Two SIM cards are available on this device.
    \value SimLocked                  Device has SIM lock enabled.
*/

/*!
    \enum QSystemDeviceInfo::InputMethod
    This enum describes the device method of user input.

    \value Keys               Device has key/buttons.
    \value Keypad             Device has keypad (1,2,3, etc).
    \value Keyboard           Device has qwerty keyboard.
    \value SingleTouch        Device has single touch screen.
    \value MultiTouch         Device has muti touch screen.
    \value Mouse              Device has a mouse.
*/

/*!
    \class QSystemScreenSaver

    \ingroup systeminfo

    \brief The QSystemScreenSaver class provides access to screen saver and blanking.

  */

/*!
  \fn void QSystemInfo::currentLanguageChanged(const QString &lang)

  This signal is emitted whenever the current language changes, specified by \a lang,
  which is in 2 letter, ISO 639-1 specification form.
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
  \fn void QSystemDeviceInfo::bluetoothStateChanged(bool on)

  This signal is emitted whenever bluetooth state changes, specified by \a on.
*/

/*!
  \fn void QSystemDeviceInfo::bluetoothStateChanged(bool on)

  This signal is emitted whenever bluetooth state changes, specified by \a on.
*/

/*!
   \fn void QSystemStorageInfo::storageAdded()

   This signal gets emitted when new storage has been added to the system.
*/

/*!
   \fn void QSystemStorageInfo::storageRemoved()

   This signal gets emitted when new storage has been removed from the system.
*/

Q_GLOBAL_STATIC(QSystemInfoPrivate, sysinfoPrivate)

 /*!
\fn QSystemInfo::QSystemInfo(QObject *parent)
   Constructs a QSystemInfo object with the given \a parent.
 */

QSystemInfo::QSystemInfo(QObject *parent)
    : QObject(parent), d(sysinfoPrivate())
{
    qRegisterMetaType<QSystemInfo::Version>("QSystemInfo::Version");
    qRegisterMetaType<QSystemInfo::Feature>("QSystemInfo::Feature");
}

/*!
  Destroys the QSystemInfo object
*/
QSystemInfo::~QSystemInfo()
{
}

/*!
    \internal

    This function is called when the client connects to signals.

    \sa connectNotify()
*/

void QSystemInfo::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentLanguageChanged(QString))))) {
        connect(d,SIGNAL(currentLanguageChanged(QString)),
                this,SIGNAL(currentLanguageChanged(QString)));
    }
}

/*!
    \internal

    This function is called when the client disconnects from the signals.

    \sa connectNotify()
*/
void QSystemInfo::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentLanguageChanged(QString))))) {
        disconnect(d,SIGNAL(currentLanguageChanged(QString)),
                   this,SIGNAL(currentLanguageChanged(QString)));
    }
}

/*!
  \property QSystemInfo::currentLanguage
  \brief The current Language
    Returns the current language in 2 letter ISO 639-1 format.
 */
QString QSystemInfo::currentLanguage()
{
    return sysinfoPrivate()->currentLanguage();
}
/*!
  \property  QSystemInfo::availableLanguages
  \brief List of available languages.

    Returns a QStringList of available Qt language translations in 2 letter ISO 639-1 format.
    If the Qt translations cannot be found, returns the current system language.
  */
QStringList QSystemInfo::availableLanguages()
{
    return sysinfoPrivate()->availableLanguages();
}

/*!
  Returns the version of QSystemInfo::Version \a type,
  with optional platform dependent \a parameter as a string.

  Version will be returned in "major.minor.build" form.

  In case a particular version does not use the "build" part, it is set to 0.
If a particular element is not available at all, an error "Not Installed" will be returned by
the API.
*/
QString QSystemInfo::version(QSystemInfo::Version type, const QString &parameter)
{
    return sysinfoPrivate()->version(type, parameter);
}

/*!
  \property  QSystemInfo::currentCountryCode
  \brief The current locale country code.

    Returns the 2 letter ISO 3166-1 for the current country code.
*/
QString QSystemInfo::currentCountryCode()
{
    return sysinfoPrivate()->currentCountryCode();
}

/*!
    Returns true if the QSystemInfo::Feature \a feature is supported, otherwise false.
*/

bool QSystemInfo::hasFeatureSupported(QSystemInfo::Feature feature)
{
    return sysinfoPrivate()->hasFeatureSupported(feature);
}


#include "moc_qsystemgeneralinfo.cpp"

QTM_END_NAMESPACE
