/*

*/


#include "qsystemsinfo.h"

#include "qsystemsinfo_p.h"

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
    \class QSystemsInfo

    \brief The QSystemsInfo class provides access to various information from the system.

  */
/*!
    \enum QSystemsInfo::Version
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
    \enum QSystemsInfo::CellNetworkStatus
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
    \enum QSystemsInfo::NetworkMode
    This enum describes the type of network:

    \value UnknownMode             No network, or error.
    \value GsmMode                 Global System for Mobile (GSM) network.
    \value CdmaMode                Code division multiple access (CDMA) network.
    \value WcdmaMode               Wideband Code Division Multiple Access (W-CDMA) network.
    \value WlanMode                Wireless Local Area Network (WLAN) network.
    \value EthMode                 Wired Local Area network.

  */
/*!
    \enum QSystemsInfo::Feature
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
    \enum QSystemsInfo::BatteryLevel
    This enum describes the level of the main battery.

    \value BatteryCritical         Battery level is critical 3% or less.
    \value BatteryVeryLow          Battery level is very low, 10% or less.
    \value BatteryLow              Battery level is low 40% or less.
    \value BatteryNormal           Battery level is above 40%.

  */
/*!
    \enum QSystemsInfo::PowerState
    This enum describes the power state:

    \value UnknownPower            Power error.
    \value BatteryPower            On battery power.
    \value WallPower               On wall power.

  */
/*!
    \enum QSystemsInfo::Profile
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
    \enum QSystemsInfo::InputMethods
    This enum describes the input methods for the device or desktop.

    \value KeysOnly                Has only Keys or keyboard
    \value TouchOnly               Has only touchscreen
    \value KeysAndTouch            Has both Keys and touchscreen.
*/

/*!
    \enum QSystemsInfo::VolumeType
    This enum describes the type of drive or volume

    \value Internal                Is internal drive
    \value Removable               Is removable
*/

/*!
    \enum QSystemsInfo::SimStatus
    This enum describes the status is the sim card or cards.

    \value SimNotAvailable         SIM is not available on this device.
    \value SingleAvailable         One SIM card is available on this.
    \value DualAvailable           Two SIM cards are available on this device.
    \value Locked                  Device has SIM lock enabled.
*/


/*!
  \fn void QSystemsInfo::memoryCritical(qint32 percent)

  This signal is emitted when memory fall below 3%.
  \a percent of current charge.
  */

/*!
  \fn void QSystemsInfo::profileChanged(QSystemsInfo::Profile profile)

  This signal is emitted when user changes the current operating profile.
\a profile is the new profile.
 */

/*!
  \fn void QSystemsInfo::networkStatusChanged(QSystemsInfo::NetworkMode netMode, QSystemsInfo::CellNetworkStatus netStatus)

  This signal is emitted when network status has changed.
  \a netMode is the network mode of the changed network.
  \a netStatus  is the new status of the changed network.
 */

/*!
  \fn void QSystemsInfo::batteryLevelChanged(QSystemsInfo::BatteryLevel level)

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */

/*!
  \fn void QSystemsInfo::batteryLevelCritical(qint32 level)

  This signal is emitted when the level of the battery charge as become critical or 3% or less.
  \a level New battery level
 */

/*!
  \fn void QSystemsInfo::powerStateChanged(QSystemsInfo::PowerState state)

  This signal is emitted when the power state has changed, such as when a phone gets plugged qint32o the wall.
  \a state is the new power state.
 */

 /*!
    \fn void QSystemsInfo::diskSpaceCritical(QString &driveVolume, qint32 spaceRemaining)

    This signal is emitted when disk space has reached 3% of capacity.
    \a driveVolume is the volume, drive or disk that has reached critical.
    \a spaceRemaining is the space remaining in kilobytes,

 */
 /*!
   Constructs a QSystemsInfo with the given \a parent.
 */
QSystemsInfo::QSystemsInfo(QObject *parent)
{
    d = new QSystemsInfoPrivate(parent);
}

/*!
    Returns the current language in 2 letter ISO 639-1 format.
 */
QString QSystemsInfo::currentLanguage() const
{
    return d->currentLanguage();
}
/*!
    Returns a QStringList of available languages in 2 letter ISO 639-1 format.
  */
QStringList QSystemsInfo::availableLanguages() const
{
    return d->availableLanguages();
}

/*!
  Returns the version of QSystemsInfo::Version \a type,
 with optional \a parameter as a string.

Note: Version will always be returned in "major.minor.build" format.

  In case a particular version does not use the "build" part, it is set to 0.
If a particular element is not available at all, an error "Not Installed" will be returned by
the API.
*/
QString QSystemsInfo::getVersion(QSystemsInfo::Version type, const QString &parameter)
{
    return d->getVersion(type, parameter);
}

/*!
    Returns the  2 letter ISO 3166-1 country code.
*/
QString QSystemsInfo::countryCode() const
{
    return d->countryCode();
}

/*!
    Returns the status of the cell network.
*/
qint32 QSystemsInfo::getCellNetworkStatus() {
    return d->getCellNetworkStatus();
}

/*!
    Returns the strength of the network signal, per network mode , 0 - 100 linear scaling
*/
qint32 QSystemsInfo::networkSignalStrength()
{
    return d->networkSignalStrength();
}

/*!
    Returns the Cell ID of the connected tower or based station.
*/
qint32 QSystemsInfo::cellId()
{
    return d->cellId();
}

/*!
    Returns the Location Area Code, in the range of 0 to 65,535.
*/
qint32 QSystemsInfo::lac()
{
    return d->lac();
}

 /*!
    Returns the current Mobile Country Code
/*/
qint32 QSystemsInfo::currentMCC()
{
    return d->currentMCC();
}

/*!
    Returns the current Mobile Network Code
*/
qint32 QSystemsInfo::currentMNC()
{
    return d->currentMCC();
}

/*!
    Returns the home Mobile Network Code
*/
qint32 QSystemsInfo::homeMCC()
{
    return d->homeMCC();
}

/*!
    Returns the home Mobile Country Code
*/
qint32 QSystemsInfo::homeMNC()
{
    return d->homeMNC();
}

/*!
    Returns true if the system has location services, otherwise false.
*/
bool QSystemsInfo::isLocationEnabled() const
{
    return d->isLocationEnabled();
}

/*!
    Returns true if the system has an accessab;e WLan, otherwise false.
*/
bool QSystemsInfo::isWLANAccessible() const
{
    return d->isWLANAccessible();
}

/*!
  Returns the name of the operator.
*/
QString QSystemsInfo::operatorName()
{
    return d->operatorName();
}

/*!
    Returns true if feature \a feature is supported, otherwise false.
*/

bool QSystemsInfo::hasFeatureSupported(QSystemsInfo::Feature feature)
{
    return d->hasFeatureSupported(feature);
}

/*!
    Returns further detail of Feature \a feature.
  */
QString QSystemsInfo::getDetailOfFeature(QSystemsInfo::Feature feature)
{
    return d->getDetailOfFeature(feature);
}

// display
/*!
    Returns the display brightness in %, 1 - 100 scale
*/
qint32 QSystemsInfo::displayBrightness()
{
    return d->displayBrightness();
}

/*!
    Returns the color depth of the screen \a screenNumber, in bits per pixel.
*/
qint32 QSystemsInfo::colorDepth(qint32 screenNumber)
{
    return d->colorDepth(screenNumber);
}

/*!
    Sets the screensaver on if \a b is true, otherwise off.
*/
void QSystemsInfo::setScreenSaverEnabled(bool b)
{
    d->setScreenSaverEnabled(b);
}

/*!
    Sets the screen blanking on if \a b is true, otherwise off.
*/
void QSystemsInfo::setScreenBlankingEnabled(bool b)
{
    d->setScreenBlankingEnabled(b);
}

/*!
    Returns whether the systems screen lock is turned on.
*/
bool QSystemsInfo::isScreenLockOn()
{
    return d->isScreenLockOn();
}


/*!
    Returns true, if the system can present RAM mempry levels, otherwise false.
*/
bool QSystemsInfo::hasRamMemoryLevel()
{
    return d->hasRamMemoryLevel();
}

/*!
    Returns the amount of free RAM memory in kilobytes.
*/
quint64 QSystemsInfo::freeMemoryLevel() const
{
    return d->freeMemoryLevel();
}

/*!
    Returns the amount of total space on the \a volumeDrive.
*/
qlonglong QSystemsInfo::totalDiskSpace(const QString &volumeDrive)
{
    return d->totalDiskSpace(volumeDrive);
}

/*!
    Returns the amount of available free space on the \a volumeDrive.
*/
qlonglong QSystemsInfo::availableDiskSpace(const QString &volumeDrive)
{
    return d->availableDiskSpace(volumeDrive);
}

/*!
    Returns a QStringList of volumes or partitions.
*/
QStringList QSystemsInfo::listOfVolumes()
{
    return d->listOfVolumes();
}

/*!
  Returns the type of volume \a driveVolume
*/
int QSystemsInfo::getVolumeType(const QString &driveVolume)
{
    return d->getVolumeType(driveVolume);
}

/*!
    Returns the InputMethodType that the system uses.
*/
qint32 QSystemsInfo::getInputMethodType()
{
 return d->getInputMethodType();
}
// device

/*!
    Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
*/
QString QSystemsInfo::imei() const
{
    return d->imei();
}

/*!
    Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none.
*/
QString QSystemsInfo::imsi() const
{
    return d->imsi();
}

/*!
    Returns the name of the manufacturer.
*/
QString QSystemsInfo::manufacturer() const
{
    return d->manufacturer();
}

/*!
    Returns the model of the device, 0r in the case of desktops, the CPU.
*/
QString QSystemsInfo::model() const
{
    return d->model();
}

/*!
    Returns the battery charge level as percentage 1 - 100 scale
*/
qint32 QSystemsInfo::batteryLevel() const
{
    return d->batteryLevel();
}

/*!
  Returns status of SIM card.
*/
qint32 QSystemsInfo::getSimStatus()
{
    return d->getSimStatus();
}
/*!
  Returns true if the device is locked, otherwise false.
*/
bool QSystemsInfo::isDeviceLocked()
{
    return d->isDeviceLocked();
}


QT_END_NAMESPACE
