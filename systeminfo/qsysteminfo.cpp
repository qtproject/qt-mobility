/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsysteminfo.h"

#include "qsysteminfo_p.h"

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

QT_BEGIN_NAMESPACE

  /*!
    \class QSystemInfo

    \brief The QSystemInfo class provides access to various general information from the system.

  */
/*!
    \enum QSystemInfo::Version
    This enum describes the version component.

    \value Os                    Operating system version / platform ID.
    \value QtCore                Qt libs version.
    \value WrtCore               Web runtime version.
    \value Webkit                Version of Webkit.
    \value ServiceFramework      version of Service Framework.
    \value WrtExtensions         version of Web runtime.
    \value ServiceProvider       Version of requested service provider.
    \value NetscapePlugin        Version of Netscap plugin.
    \value WebApp                Web Application identifier.
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
    \value UsbFeature             Univeral System Bus (USB) feature available.
    \value VibFeature             Vibration feature available.
    \value WlanFeature            Wireless Local Area Network (WLAN) feature available.
    \value SimFeature             Subscriber Identity Module (SIM) available.
    \value LocationFeature        Global Positioning System (GPS) and/or other location feature available.
    \value VideoOutFeature        Video out feature available.
    \value HapticsFeature         Haptics feature available.
  */

  /*!
    \class QSystemNetworkInfo

    \brief The QSystemNetworkInfo class provides access to network information from the system.

  */
/*!
    \enum QSystemNetworkInfo::CellNetworkStatus
    This enum describes the status of the network connection:

    \value UndefinedStatus        There is no network device, or error.
    \value NoNetworkAvailable     There is no network available.
    \value EmergencyOnly          Emergency calls only.
    \value Searching              Searching for network.
    \value Busy                   Network is busy.
    \value HomeNetwork            On Home Network.
    \value Denied                 Network access denied.
    \value Roaming                On Roaming network.

  */
/*!
    \enum QSystemNetworkInfo::NetworkMode
    This enum describes the type of network:

    \value UnknownMode             No network, or error.
    \value GsmMode                 Global System for Mobile (GSM) network.
    \value CdmaMode                Code division multiple access (CDMA) network.
    \value WcdmaMode               Wideband Code Division Multiple Access (W-CDMA) network.
    \value WlanMode                Wireless Local Area Network (WLAN) network.
    \value EthMode                 Wired Local Area network.

  */

/*!
    \class QSystemDisplayInfo

    \brief The QSystemDisplayInfo class provides access to display information from the system.

  */

  /*!
    \class QSystemMemoryInfo

    \brief The QSystemMemoryInfo class provides access to memory and disk information from the system.

  */

/*!
    \enum QSystemMemoryInfo::VolumeType
    This enum describes the type of drive or volume

    \value NoVolume               Volume type undetermined.
    \value Internal                   Is internal drive.
    \value Removable              Is removable.
    \value Remote                     Is a network drive.
    \value Cdrom                      Is a cd rom drive.
*/


/*!
    \class QSystemDeviceInfo

    \brief The QSystemDeviceInfo class provides access to device information from the system.

  */
/*!
  \fn void QSystemDeviceInfo::batteryLevelChanged(QSystemDeviceInfo::BatteryLevel level)

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */

/*!
  \fn void QSystemDeviceInfo::powerStateChanged(QSystemDeviceInfo::PowerState state)

  This signal is emitted when the power state has changed, such as when a phone gets plugged qint32o the wall.
  \a state is the new power state.
 */
/*!
    \enum QSystemDeviceInfo::BatteryLevel
    This enum describes the level of the main battery.

    \value NoBatteryLevel          Battery level undetermined.
    \value BatteryCritical         Battery level is critical 3% or less.
    \value BatteryVeryLow          Battery level is very low, 10% or less.
    \value BatteryLow              Battery level is low 40% or less.
    \value BatteryNormal           Battery level is above 40%.

  */
/*!
    \enum QSystemDeviceInfo::PowerState
    This enum describes the power state:

    \value UnknownPower            Power error.
    \value BatteryPower            On battery power.
    \value WallPower               On wall power.

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
    \value SingleAvailable         One SIM card is available on this.
    \value DualAvailable           Two SIM cards are available on this device.
    \value Locked                  Device has SIM lock enabled.
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

    \brief The QSystemScreenSaver class provides access to screen saver and blanking.

  */



 /*!
   Constructs a QSystemInfo with the given \a parent.
 */
QSystemInfo::QSystemInfo(QObject *parent)
{
    d = new QSystemInfoPrivate(parent);
}

/*!
  Destroys the QSystemInfo object
*/
QSystemInfo::~QSystemInfo()
{
    delete d;
}

/*!
    Returns the current language in 2 letter ISO 639-1 format.
 */
QString QSystemInfo::currentLanguage()
{
    QSystemInfoPrivate dp;
    return dp.currentLanguage();
}
/*!
    Returns a QStringList of available languages in 2 letter ISO 639-1 format.
  */
QStringList QSystemInfo::availableLanguages()
{
    QSystemInfoPrivate dp;
    return dp.availableLanguages();
}

/*!
  Returns the version of QSystemInfo::Version \a type,
 with optional \a parameter as a string.

Note: Version will always be returned in "major.minor.build" format.

  In case a particular version does not use the "build" part, it is set to 0.
If a particular element is not available at all, an error "Not Installed" will be returned by
the API.
*/
QString QSystemInfo::version(QSystemInfo::Version type, const QString &parameter)
{
    return d->version(type, parameter);
}

/*!
    Returns the 2 letter ISO 3166-1 for the current country code.
*/
QString QSystemInfo::currentCountryCode()
{
    QSystemInfoPrivate dp;
    return dp.currentCountryCode();
}

/*!
    Returns true if feature \a feature is supported, otherwise false.
*/

bool QSystemInfo::hasFeatureSupported(QSystemInfo::Feature feature)
{
    return d->hasFeatureSupported(feature);
}

 /*!
   Constructs a QSystemNetworkInfo with the given \a parent.
 */
QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
{
    d = new QSystemNetworkInfoPrivate(parent);
}

/*!
  Destroys the QSystemNetworkInfo object.
 */
QSystemNetworkInfo::~QSystemNetworkInfo()
{
    delete d;
}

/*!
    Returns the status of the cell network.
*/
QSystemNetworkInfo::CellNetworkStatus QSystemNetworkInfo::cellNetworkStatus() {
    return d->cellNetworkStatus();
}

/*!
    Returns the strength of the network signal, per network \a mode , 0 - 100 linear scaling,
    or -1 in the case of unknown network mode or error.
*/
int QSystemNetworkInfo::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.networkSignalStrength(mode);
}

/*!
    Returns the Cell ID of the connected tower or based station.
*/
int QSystemNetworkInfo::cellId()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.cellId();
}

/*!
    Returns the Location Area Code.
*/
int QSystemNetworkInfo::locationAreaCode()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.locationAreaCode();
}

 /*!
    Returns the current Mobile Country Code
/*/
QString QSystemNetworkInfo::currentMobileCountryCode()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.currentMobileCountryCode();
}

/*!
    Returns the current Mobile Network Code
*/
QString QSystemNetworkInfo::currentMobileNetworkCode()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.currentMobileNetworkCode();
}

/*!
    Returns the home Mobile Network Code
*/
QString QSystemNetworkInfo::homeMobileCountryCode()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.homeMobileCountryCode();
}

/*!
    Returns the home Mobile Country Code
*/
QString QSystemNetworkInfo::homeMobileNetworkCode()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.homeMobileNetworkCode();
}

/*!
    Returns true if the system has an accessab;e WLan, otherwise false.
*/
bool QSystemNetworkInfo::isWLANAccessible()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.isWLANAccessible();
}

/*!
  Returns the name of the operator.
*/
QString QSystemNetworkInfo::operatorName()
{
    QSystemNetworkInfoPrivate dnp;
    return dnp.operatorName();
}

// display
 /*!
   Constructs a QSystemDisplayInfo.
 */
QSystemDisplayInfo::QSystemDisplayInfo()
{
}

/*!
  Destroys the QSystemDisplayInfo object.
 */
QSystemDisplayInfo::~QSystemDisplayInfo()
{
}

/*!
    Returns the display brightness of \a screen in %, 1 - 100 scale
*/
int QSystemDisplayInfo::displayBrightness(int screen)
{
    QSystemDisplayInfoPrivate dp;
    return dp.displayBrightness(screen);
}

/*!
    Returns the color depth of the screen \a screenNumber, in bits per pixel.
*/
int QSystemDisplayInfo::colorDepth(int screenNumber)
{
    QSystemDisplayInfoPrivate dp;
    return dp.colorDepth(screenNumber);
}

 /*!
   Constructs a QSystemMemoryInfo with the given \a parent.
 */
QSystemMemoryInfo::QSystemMemoryInfo(QObject *parent)
{
    d = new QSystemMemoryInfoPrivate(parent);
  //  connect(d,SIGNAL(newMemoryCardAdded(QString)),this,SIGNAL(newMemoryCard(QString)));
}

/*!
  Destroys the QSystemMemoryInfo object.
*/
QSystemMemoryInfo::~QSystemMemoryInfo()
{
    delete d;
}

/*!
    Returns the amount of total space on the \a volumeDrive,
    in bytes.
*/
qlonglong QSystemMemoryInfo::totalDiskSpace(const QString &volumeDrive)
{
    return d->totalDiskSpace(volumeDrive);
}

/*!
    Returns the amount of available free space on the \a volumeDrive,
in bytes.
*/
qlonglong QSystemMemoryInfo::availableDiskSpace(const QString &volumeDrive)
{
    return d->availableDiskSpace(volumeDrive);
}

/*!
    Returns a QStringList of volumes or partitions.
*/
QStringList QSystemMemoryInfo::listOfVolumes()
{
    QSystemMemoryInfoPrivate dp;
    return dp.listOfVolumes();
}

/*!
  Returns the type of volume \a driveVolume
*/
QSystemMemoryInfo::VolumeType QSystemMemoryInfo::volumeType(const QString &driveVolume)
{
    return d->volumeType(driveVolume);
}

// device
 /*!
   Constructs a QSystemDeviceInfo with the given \a parent.
 */
QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)
{
    d = new QSystemDeviceInfoPrivate(parent);
}

/*!
  Destroys the QSystemDeviceInfo object.
 */
QSystemDeviceInfo::~QSystemDeviceInfo()
{
    delete d;
}

/*!
    Returns the InputMethodType that the system uses.
*/
QSystemDeviceInfo::InputMethods QSystemDeviceInfo::inputMethodType()
{
 return d->inputMethodType();
}
/*!
    Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imei()
{
    QSystemDeviceInfoPrivate dip;
    return dip.imei();
}

/*!
    Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imsi()
{
    QSystemDeviceInfoPrivate dip;
    return dip.imsi();
}

/*!
    Returns the name of the manufacturer.
*/
QString QSystemDeviceInfo::manufacturer()
{
    QSystemDeviceInfoPrivate dip;
    return dip.manufacturer();
}

/*!
    Returns the model of the device, 0r in the case of desktops, the CPU.
*/
QString QSystemDeviceInfo::model()
{
    QSystemDeviceInfoPrivate dip;
    return dip.model();
}

/*!
    Returns the product name of the device.
*/
QString QSystemDeviceInfo::productName()
{
    QSystemDeviceInfoPrivate dip;
    return dip.productName();
}
/*!
    Returns the battery charge level as percentage 1 - 100 scale
*/
int QSystemDeviceInfo::batteryLevel() const
{
    return d->batteryLevel();
}

/*!
    Returns true if the battery is charging, otherwise false;
*/
bool QSystemDeviceInfo::isBatteryCharging()
{
    return d->isBatteryCharging();
}

/*!
  \property QSystemDeviceInfo::simStatus
  \brief the status of the sim card.
  Returns status of SIM card.
*/
QSystemDeviceInfo::SimStatus QSystemDeviceInfo::simStatus()
{
    return d->simStatus();
}
/*!
  Returns true if the device is locked, otherwise false.
*/
bool QSystemDeviceInfo::isDeviceLocked()
{
    return d->isDeviceLocked();
}

/*!
  \property QSystemDeviceInfo::currentProfile
  \brief the device profile
  Gets the current device profile QSystemDeviceInfo::Profile
*/
QSystemDeviceInfo::Profile QSystemDeviceInfo::currentProfile()
{
    return d->currentProfile();
}

/*!
  \property QSystemDeviceInfo::currentPowerState
  \brief the power state.

  Gets the current device power state
*/
QSystemDeviceInfo::PowerState QSystemDeviceInfo::currentPowerState()
{
return d->currentPowerState();
}


/////
 /*!
   Constructs a QSystemScreenSaver with the given \a parent.
 */
QSystemScreenSaver::QSystemScreenSaver(QObject *parent)
{
    d = new QSystemScreenSaverPrivate(parent);
    screenSaverIsEnabled = screenSaverEnabled();
    screenBlankingIsEnabled = screenBlankingEnabled();
}

/*!
  Desctoys the QSyatemScreenSaver object.
 */
QSystemScreenSaver::~QSystemScreenSaver()
{
    if(screenSaverIsEnabled != screenSaverEnabled())
        setScreenSaverEnabled(screenSaverIsEnabled);
    if(screenBlankingIsEnabled != screenBlankingEnabled())
        setScreenBlankingEnabled(screenBlankingIsEnabled);
    delete d;
}

/*!
    Temporarily sets the screensaver on to  \a b.
Will be reverted upon destruction of the QSystemScreenSaver object.
Returns true on success, otherwise false.

On Windows platform, if screensaver is secure by policy, the policy will be honored
and this will fail.
*/
bool QSystemScreenSaver::setScreenSaverEnabled(bool b)
{
    return d->setScreenSaverEnabled(b);
}

/*!
    Temporarily sets the screen blanking on to  \a b
Will be reverted upon destruction of the QSystemScreenSaver object.
Returns true on success, otherwise false.

On Windows platform, if screensaver is secure by policy, the policy will be honored
and this will fail.
*/
bool QSystemScreenSaver::setScreenBlankingEnabled(bool b)
{
    return d->setScreenBlankingEnabled(b);
}

/*!
   Returns true if the screensaver is enabled, otherwise false.
*/
bool QSystemScreenSaver::screenSaverEnabled()
{
    return d->screenSaverEnabled();
}

/*!
   Returns true if screen blanking is enabled, other wise false.
*/
bool QSystemScreenSaver::screenBlankingEnabled()
{
    return d->screenBlankingEnabled();
}

/*!
    Returns whether the systems screen lock is turned on.
*/
bool QSystemScreenSaver::isScreenLockOn()
{
    QSystemScreenSaverPrivate dp;
    return dp.isScreenLockOn();
}

QT_END_NAMESPACE
