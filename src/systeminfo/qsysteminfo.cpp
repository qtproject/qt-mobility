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

#include "qsysteminfo.h"

#ifdef Q_OS_LINUX
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "qsysteminfo_maemo_p.h"
#else
#include "qsysteminfo_linux_p.h"
#endif //Q_WS_MAEMO_5 & Q_WS_MAEMO_6
#endif //Q_OS_LINUX

#ifdef Q_OS_WIN
#include "qsysteminfo_win_p.h"
#endif
#ifdef Q_OS_MAC
#include "qsysteminfo_mac_p.h"
#endif
#ifdef Q_OS_SYMBIAN
#include "qsysteminfo_s60_p.h"
#endif

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>


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

  This signal is emitted when the power state has changed, such as when a phone gets plugged qint32o the wall.
  \a state is the new power state.
 */

/*!
  \fn  void QSystemDeviceInfo::currentProfileChanged(QSystemDeviceInfo::Profile profile)

  This signal is emitted whenever the network profile changes, specified by \a profile.
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

Q_GLOBAL_STATIC(QSystemInfoPrivate, sysinfoPrivate)
Q_GLOBAL_STATIC(QSystemNetworkInfoPrivate, netInfoPrivate)
Q_GLOBAL_STATIC(QSystemDisplayInfoPrivate, displayInfoPrivate)
Q_GLOBAL_STATIC(QSystemStorageInfoPrivate, storageInfoPrivate)
Q_GLOBAL_STATIC(QSystemDeviceInfoPrivate, deviceInfoPrivate)
Q_GLOBAL_STATIC(QSystemScreenSaverPrivate, screenSaverPrivate)

 /*!
\fn QSystemInfo::QSystemInfo(QObject *parent)
   Constructs a QSystemInfo object with the given \a parent.
 */

QSystemInfo::QSystemInfo(QObject *parent)
    : QObject(parent), d(sysinfoPrivate())
{
    connect(d,SIGNAL(currentLanguageChanged(QString)),
            this,SIGNAL(currentLanguageChanged(QString)));
}

/*!
  Destroys the QSystemInfo object
*/
QSystemInfo::~QSystemInfo()
{
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

 /*!
   \fn QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
   Constructs a QSystemNetworkInfo object with the given \a parent.
 */

QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
    : QObject(parent), d(netInfoPrivate())
{
    connect(d,SIGNAL(currentMobileCountryCodeChanged(QString)),
            this,SIGNAL(currentMobileCountryCodeChanged(QString)));

    connect(d,SIGNAL(currentMobileNetworkCodeChanged(QString)),
            this,SIGNAL(currentMobileNetworkCodeChanged(QString)));

    connect(d,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
            this,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));

    connect(d,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
            this,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));

    connect(d,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
            this,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));

    connect(d,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
            this,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));
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
    return netInfoPrivate()->networkSignalStrength(mode);
}

/*!
  \property QSystemNetworkInfo::cellId
  \brief The devices Cell ID
    Returns the Cell ID of the connected tower or based station.
*/
int QSystemNetworkInfo::cellId()
{
    return netInfoPrivate()->cellId();
}

/*!
  \property QSystemNetworkInfo::locationAreaCode
  \brief The LAC.

    Returns the Location Area Code. In the case of none such as a Desktop, an empty string.
*/
int QSystemNetworkInfo::locationAreaCode()
{
    return netInfoPrivate()->locationAreaCode();
}

 /*!
   \property QSystemNetworkInfo::currentMobileCountryCode
   \brief The current MCC.

    Returns the current Mobile Country Code. In the case of none such as a Desktop, an empty string.
/*/
QString QSystemNetworkInfo::currentMobileCountryCode()
{
    return netInfoPrivate()->currentMobileCountryCode();
}

/*!
  \property QSystemNetworkInfo::currentMobileNetworkCode
  \brief The current MNC.

    Returns the current Mobile Network Code. In the case of none such as a Desktop, an empty string.
*/
QString QSystemNetworkInfo::currentMobileNetworkCode()
{
    return netInfoPrivate()->currentMobileNetworkCode();
}

/*!
  \property QSystemNetworkInfo::homeMobileCountryCode
  \brief The home MNC.

    Returns the home Mobile Network Code. In the case of none such as a Desktop, an empty string.
*/
QString QSystemNetworkInfo::homeMobileCountryCode()
{
    return netInfoPrivate()->homeMobileCountryCode();
}

/*!
  \property QSystemNetworkInfo::homeMobileNetworkCode
  \brief The home MCC.

    Returns the home Mobile Country Code. In the case of none such as a Desktop, an empty string.
*/
QString QSystemNetworkInfo::homeMobileNetworkCode()
{
    return netInfoPrivate()->homeMobileNetworkCode();
}

/*!
  Returns the name of the operator for the network \a mode.  For wlan this returns the network's current SSID.
In the case of no network such as a desktop, an empty string.
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
  Returns the first found QNetworkInterface for type \a mode.
  */
QNetworkInterface QSystemNetworkInfo::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    return netInfoPrivate()->interfaceForMode(mode);
}
/*!
  Returns the current active mode. If more than one mode is active, returns the
  default or preferred mode. If no modes are active, returns UnknownMode.
  */
QSystemNetworkInfo::NetworkMode QSystemNetworkInfo::currentMode()
{
    return netInfoPrivate()->currentMode();
}

// display
 /*!
   \fn QSystemDisplayInfo::QSystemDisplayInfo(QObject *parent)
   Constructs a QSystemDisplayInfo object with the given \a parent.
 */

QSystemDisplayInfo::QSystemDisplayInfo(QObject *parent)
    : QObject(parent)
{
}

/*!
  Destroys the QSystemDisplayInfo object.
 */
QSystemDisplayInfo::~QSystemDisplayInfo()
{
}

/*!
    Returns the display brightness of \a screen in %, 1 - 100 scale.

    Depending on platform, displayBrightness may not be available due to
    differing hardware, software or driver implementation. In which case this
    will return 0.

*/
int QSystemDisplayInfo::displayBrightness(int screen)
{
    return displayInfoPrivate()->displayBrightness(screen);
}

/*!
    Returns the color depth of the screen \a screenNumber, in bits per pixel.
*/
int QSystemDisplayInfo::colorDepth(int screenNumber)
{
    return displayInfoPrivate()->colorDepth(screenNumber);
}

 /*!
   \fn QSystemStorageInfo::QSystemStorageInfo(QObject *parent)
   Constructs a QSystemStorageInfo object with the given \a parent.
 */

QSystemStorageInfo::QSystemStorageInfo(QObject *parent)
    : QObject(parent)
{
}

/*!
  Destroys the QSystemStorageInfo object.
*/
QSystemStorageInfo::~QSystemStorageInfo()
{
}

/*!
    Returns the amount of total space on the \a volumeDrive,
    in bytes.
*/
qlonglong QSystemStorageInfo::totalDiskSpace(const QString &volumeDrive)
{
    return storageInfoPrivate()->totalDiskSpace(volumeDrive);
}

/*!
    Returns the amount of available free space on the \a volumeDrive,
in bytes.
*/
qlonglong QSystemStorageInfo::availableDiskSpace(const QString &volumeDrive)
{
    return storageInfoPrivate()->availableDiskSpace(volumeDrive);
}

/*!
  \property QSystemStorageInfo::logicalDrives
  \brief The logical drives.

    Returns a QStringList of volumes or partitions.
*/
QStringList QSystemStorageInfo::logicalDrives()
{
    return storageInfoPrivate()->logicalDrives();
}

/*!
  Returns the type of volume \a driveVolume
*/
QSystemStorageInfo::DriveType QSystemStorageInfo::typeForDrive(const QString &driveVolume)
{
    return storageInfoPrivate()->typeForDrive(driveVolume);
}

// device
 /*!
   \fn QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)
   Constructs a QSystemDeviceInfo with the given \a parent.
 */

QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)
    : QObject(parent), d(deviceInfoPrivate())
{
    connect(d,SIGNAL(batteryLevelChanged(int)),
            this,SIGNAL(batteryLevelChanged(int)));

    connect(d,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));

    connect(d,SIGNAL(bluetoothStateChanged(bool)),
            this,SIGNAL(bluetoothStateChanged(bool)));

    connect(d,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            this,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)));

    connect(d,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)));
    }

/*!
  Destroys the QSystemDeviceInfo object.
 */
QSystemDeviceInfo::~QSystemDeviceInfo()
{
}


/*!
  \property QSystemDeviceInfo::inputMethodType
  \brief The supported inputmethods.

    Returns the QSystemDeviceInfo::InputMethodFlags InputMethodType that the system uses.
*/
QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfo::inputMethodType()
{
 return deviceInfoPrivate()->inputMethodType();
}

/*!
  \property QSystemDeviceInfo::imei
  \brief The IMEI.

    Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imei()
{
    return deviceInfoPrivate()->imei();
}

/*!
  \property QSystemDeviceInfo::imsi
  \brief The IMSI.

    Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imsi()
{
    return deviceInfoPrivate()->imsi();
}

/*!
  \property QSystemDeviceInfo::manufacturer
  \brief The manufacture's name.

    Returns the name of the manufacturer of this device. In the case of desktops, the name of the vendor
    of the motherboard.
*/
QString QSystemDeviceInfo::manufacturer()
{
    return deviceInfoPrivate()->manufacturer();
}

/*!
  \property QSystemDeviceInfo::model
  \brief The model name.

    Returns the model information of the device. In the case of desktops where no
    model information is present, the CPU architect, such as i686, and machine type, such as Server,
    Desktop or Laptop.
*/
QString QSystemDeviceInfo::model()
{
    return deviceInfoPrivate()->model();
}

/*!
  \property QSystemDeviceInfo::productName
  \brief The product name.

    Returns the product name of the device. In the case where no product information is available,

*/
QString QSystemDeviceInfo::productName()
{
    return deviceInfoPrivate()->productName();
}
/*!
  \property  QSystemDeviceInfo::batteryLevel
  \brief The battery level.

    Returns the battery charge level as percentage 1 - 100 scale.
*/
int QSystemDeviceInfo::batteryLevel() const
{
    return deviceInfoPrivate()->batteryLevel();
}

  /*!
    \property QSystemDeviceInfo::batteryStatus
    \brief The battery status.

    Returns the battery charge status.
*/
QSystemDeviceInfo::BatteryStatus QSystemDeviceInfo::batteryStatus()
{
   int level = batteryLevel();
    if(level < 4) {
        return QSystemDeviceInfo::BatteryCritical;
    }   else if(level < 11) {
        return QSystemDeviceInfo::BatteryVeryLow;
    }  else if(level < 41) {
        return QSystemDeviceInfo::BatteryLow;
    }   else if(level > 40) {
        return QSystemDeviceInfo::BatteryNormal;
    }

    return QSystemDeviceInfo::NoBatteryLevel;
}

/*!
  \property QSystemDeviceInfo::simStatus
  \brief the status of the sim card.
  Returns the QSystemDeviceInfo::simStatus status of SIM card.
*/
QSystemDeviceInfo::SimStatus QSystemDeviceInfo::simStatus()
{
    return deviceInfoPrivate()->simStatus();
}
/*!
  \property QSystemDeviceInfo::isDeviceLocked
  \brief Device lock.

  Returns true if the device is locked, otherwise false.
*/
bool QSystemDeviceInfo::isDeviceLocked()
{
    return deviceInfoPrivate()->isDeviceLocked();
}

/*!
  \property QSystemDeviceInfo::currentProfile
  \brief the device profile
  Gets the current QSystemDeviceInfo::currentProfile device profile.
*/
QSystemDeviceInfo::Profile QSystemDeviceInfo::currentProfile()
{
    return deviceInfoPrivate()->currentProfile();
}

/*!
  \property QSystemDeviceInfo::currentPowerState
  \brief the power state.

  Gets the current QSystemDeviceInfo::currentPowerState state.
*/
QSystemDeviceInfo::PowerState QSystemDeviceInfo::currentPowerState()
{
    return deviceInfoPrivate()->currentPowerState();
}


/////
 /*!
   Constructs a QSystemScreenSaver object with the given \a parent.

   On platforms where there is no one default screensaver mechanism, such as Linux, this class
   may not be available.
 */

QSystemScreenSaver::QSystemScreenSaver(QObject *parent)
    : QObject(parent), d(screenSaverPrivate())
{
    screenSaverIsInhibited = screenSaverInhibited();
}

/*!
  Destroys the QSystemScreenSaver object.
 */
QSystemScreenSaver::~QSystemScreenSaver()
{
    qWarning() << Q_FUNC_INFO;
    delete d;
}

/*!
    Temporarily inhibits the screensaver.

    Will be reverted upon destruction of the QSystemScreenSaver object.
    Returns true on success, otherwise false.

    On platforms that support it, if screensaver is secure by policy, the policy will be honored
    and this will fail.
*/
bool QSystemScreenSaver::setScreenSaverInhibit()
{
    return d->setScreenSaverInhibit();
}

/*!
  \property QSystemScreenSaver::screenSaverInhibited
  \brief Screensaver inhibited.

   Returns true if the screensaver is inhibited, otherwise false.
*/
bool QSystemScreenSaver::screenSaverInhibited()
{
    return d->screenSaverInhibited();
}


#include "moc_qsysteminfo.cpp"

QTM_END_NAMESPACE
