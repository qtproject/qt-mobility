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
** contact Nokia at http://www.qtsoftware.com/contact.
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

    \value UnknownFeature         Unknown feature or error.
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
  \fn void QSystemNetworkInfo::networkStatusChanged(QSystemNetworkInfo::NetworkMode netMode, QSystemNetworkInfo::CellNetworkStatus netStatus)

  This signal is emitted when network status has changed.
  \a netMode is the network mode of the changed network.
  \a netStatus  is the new status of the changed network.
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

    \value NoVolume                Volume type undetermined.
    \value Internal                Is internal drive
    \value Removable               Is removable
*/

/*!
  \fn void QSystemMemoryInfo::memoryCritical(qint32 percent)

  This signal is emitted when memory fall below 3%.
  \a percent of current charge.
  */


 /*!
    \fn void QSystemMemoryInfo::diskSpaceCritical(QString &driveVolume, qint32 spaceRemaining)

    This signal is emitted when disk space has reached 3% of capacity.
    \a driveVolume is the volume, drive or disk that has reached critical.
    \a spaceRemaining is the space remaining in kilobytes,

 */

/*!
    \class QSystemDeviceInfo

    \brief The QSystemDeviceInfo class provides access to device information from the system.

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
    \enum QSystemDeviceInfo::InputMethods
    This enum describes the input methods for the device or desktop.

    \value KeysOnly                Has only Keys or keyboard
    \value TouchOnly               Has only touchscreen
    \value KeysAndTouch            Has both Keys and touchscreen.
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
  \fn void QSystemDeviceInfo::profileChanged(QSystemDeviceInfo::Profile profile)

  This signal is emitted when user changes the current operating profile.
\a profile is the new profile.
 */


/*!
  \fn void QSystemDeviceInfo::batteryLevelChanged(QSystemDeviceInfo::BatteryLevel level)

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */

/*!
  \fn void QSystemDeviceInfo::batteryLevelCritical(qint32 level)

  This signal is emitted when the level of the battery charge as become critical or 3% or less.
  \a level New battery level
 */

/*!
  \fn void QSystemDeviceInfo::powerStateChanged(QSystemDeviceInfo::PowerState state)

  This signal is emitted when the power state has changed, such as when a phone gets plugged qint32o the wall.
  \a state is the new power state.
 */

 /*!
   Constructs a QSystemInfo with the given \a parent.
 */
QSystemInfo::QSystemInfo(QObject *parent)
{
    d = new QSystemInfoPrivate(parent);
}

/*!
    Returns the current language in 2 letter ISO 639-1 format.
 */
QString QSystemInfo::currentLanguage() const
{
    return d->currentLanguage();
}
/*!
    Returns a QStringList of available languages in 2 letter ISO 639-1 format.
  */
QStringList QSystemInfo::availableLanguages() const
{
    return d->availableLanguages();
}

/*!
  Returns the version of QSystemInfo::Version \a type,
 with optional \a parameter as a string.

Note: Version will always be returned in "major.minor.build" format.

  In case a particular version does not use the "build" part, it is set to 0.
If a particular element is not available at all, an error "Not Installed" will be returned by
the API.
*/
QString QSystemInfo::getVersion(QSystemInfo::Version type, const QString &parameter)
{
    return d->getVersion(type, parameter);
}

/*!
    Returns the  2 letter ISO 3166-1 country code.
*/
QString QSystemInfo::countryCode() const
{
    return d->countryCode();
}

/*!
    Returns true if feature \a feature is supported, otherwise false.
*/

bool QSystemInfo::hasFeatureSupported(QSystemInfo::Feature feature)
{
    return d->hasFeatureSupported(feature);
}

/*!
    Returns further detail of Feature \a feature.
  */
QString QSystemInfo::getDetailOfFeature(QSystemInfo::Feature feature)
{
    return d->getDetailOfFeature(feature);
}

 /*!
   Constructs a QSystemNetworkInfo with the given \a parent.
 */
QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent)
{
    d = new QSystemNetworkInfoPrivate(parent);
}

/*!
    Returns the status of the cell network.
*/
QSystemNetworkInfo::CellNetworkStatus QSystemNetworkInfo::getCellNetworkStatus() {
    return d->getCellNetworkStatus();
}

/*!
    Returns the strength of the network signal, per network mode , 0 - 100 linear scaling
*/
qint32 QSystemNetworkInfo::networkSignalStrength()
{
    return d->networkSignalStrength();
}

/*!
    Returns the Cell ID of the connected tower or based station.
*/
qint32 QSystemNetworkInfo::cellId()
{
    return d->cellId();
}

/*!
    Returns the Location Area Code, in the range of 0 to 65,535.
*/
qint32 QSystemNetworkInfo::locationAreaCode()
{
    return d->locationAreaCode();
}

 /*!
    Returns the current Mobile Country Code
/*/
qint32 QSystemNetworkInfo::currentMCC()
{
    return d->currentMCC();
}

/*!
    Returns the current Mobile Network Code
*/
qint32 QSystemNetworkInfo::currentMNC()
{
    return d->currentMCC();
}

/*!
    Returns the home Mobile Network Code
*/
qint32 QSystemNetworkInfo::homeMCC()
{
    return d->homeMCC();
}

/*!
    Returns the home Mobile Country Code
*/
qint32 QSystemNetworkInfo::homeMNC()
{
    return d->homeMNC();
}

/*!
    Returns true if the system has location services, otherwise false.
*/
bool QSystemNetworkInfo::isLocationEnabled() const
{
    return d->isLocationEnabled();
}

/*!
    Returns true if the system has an accessab;e WLan, otherwise false.
*/
bool QSystemNetworkInfo::isWLANAccessible() const
{
    return d->isWLANAccessible();
}

/*!
  Returns the name of the operator.
*/
QString QSystemNetworkInfo::operatorName()
{
    return d->operatorName();
}

// display
 /*!
   Constructs a QSystemDisplayInfo with the given \a parent.
 */
QSystemDisplayInfo::QSystemDisplayInfo(QObject *parent)
{
    d = new QSystemDisplayInfoPrivate(parent);
}
/*!
    Returns the display brightness in %, 1 - 100 scale
*/
qint32 QSystemDisplayInfo::displayBrightness()
{
    return d->displayBrightness();
}

/*!
    Returns the color depth of the screen \a screenNumber, in bits per pixel.
*/
qint32 QSystemDisplayInfo::colorDepth(qint32 screenNumber)
{
    return d->colorDepth(screenNumber);
}

/*!
    Sets the screensaver on if \a b is true, otherwise off.
*/
void QSystemDisplayInfo::setScreenSaverEnabled(bool b)
{
    d->setScreenSaverEnabled(b);
}

/*!
    Sets the screen blanking on if \a b is true, otherwise off.
*/
void QSystemDisplayInfo::setScreenBlankingEnabled(bool b)
{
    d->setScreenBlankingEnabled(b);
}

/*!
    Returns whether the systems screen lock is turned on.
*/
bool QSystemDisplayInfo::isScreenLockOn()
{
    return d->isScreenLockOn();
}


 /*!
   Constructs a QSystemMemoryInfo with the given \a parent.
 */
QSystemMemoryInfo::QSystemMemoryInfo(QObject *parent)
{
    d = new QSystemMemoryInfoPrivate(parent);
}

/*!
    Returns true, if the system can present RAM mempry levels, otherwise false.
*/
bool QSystemMemoryInfo::hasRamMemoryLevel()
{
    return d->hasRamMemoryLevel();
}

/*!
    Returns the amount of free RAM memory in kilobytes.
*/
quint64 QSystemMemoryInfo::freeMemoryLevel() const
{
    return d->freeMemoryLevel();
}

/*!
    Returns the amount of total space on the \a volumeDrive.
*/
qlonglong QSystemMemoryInfo::totalDiskSpace(const QString &volumeDrive)
{
    return d->totalDiskSpace(volumeDrive);
}

/*!
    Returns the amount of available free space on the \a volumeDrive.
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
    return d->listOfVolumes();
}

/*!
  Returns the type of volume \a driveVolume
*/
QSystemMemoryInfo::VolumeType QSystemMemoryInfo::getVolumeType(const QString &driveVolume)
{
    return d->getVolumeType(driveVolume);
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
    Returns the InputMethodType that the system uses.
*/
QSystemDeviceInfo::InputMethods QSystemDeviceInfo::getInputMethodType()
{
 return d->getInputMethodType();
}
/*!
    Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imei() const
{
    return d->imei();
}

/*!
    Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imsi() const
{
    return d->imsi();
}

/*!
    Returns the name of the manufacturer.
*/
QString QSystemDeviceInfo::manufacturer() const
{
    return d->manufacturer();
}

/*!
    Returns the model of the device, 0r in the case of desktops, the CPU.
*/
QString QSystemDeviceInfo::model() const
{
    return d->model();
}

/*!
    Returns the battery charge level as percentage 1 - 100 scale
*/
QSystemDeviceInfo::BatteryLevel QSystemDeviceInfo::batteryLevel() const
{
    return d->batteryLevel();
}

/*!
  Returns status of SIM card.
*/
QSystemDeviceInfo::SimStatus QSystemDeviceInfo::getSimStatus()
{
    return d->getSimStatus();
}
/*!
  Returns true if the device is locked, otherwise false.
*/
bool QSystemDeviceInfo::isDeviceLocked()
{
    return d->isDeviceLocked();
}


QT_END_NAMESPACE
