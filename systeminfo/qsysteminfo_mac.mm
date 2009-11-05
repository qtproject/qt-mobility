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
#include "qsysteminfo_mac_p.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QSettings>
#include <QSysInfo>
#include <QNetworkInterface>
#include <QList>
#include <QSettings>
#include <QDir>
#include <QNetworkInterface>
#include <QString>

#include <locale.h>

#include <IOBluetooth/IOBluetooth.h>

#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFNumber.h>

#include <IOKit/graphics/IOGraphicsLib.h>

#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/IOTypes.h>

#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>
#include <IOKit/storage/IOBlockStorageDevice.h>

#ifdef MAC_SDK_10_6
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWGlobals.h>
#endif

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#include <math.h>
#include <net/if.h>

#include <net/if_types.h>
#include <net/if_media.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

//
////////
static QString stringFromCFString(CFStringRef value) {
    QString retVal;
    if(CFStringGetLength(value) > 1) {
        CFIndex maxLength = 2 * CFStringGetLength(value) + 1/*zero term*/; // max UTF8
        char *cstring = new char[maxLength];
        if (CFStringGetCString(CFStringRef(value), cstring, maxLength, kCFStringEncodingUTF8)) {
            retVal = QString::fromUtf8(cstring);
        }
        delete cstring;
    }
    return retVal;
}

inline CFStringRef qstringToCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

inline QString nsstringToQString(const NSString *nsstr)
//{ return stringFromCFString(reinterpret_cast<const CFStringRef>(nsstr)); }
{ return QString([nsstr UTF8String]);}

inline NSString *qstringToNSString(const QString &qstr)
{ return [reinterpret_cast<const NSString *>(qstringToCFStringRef(qstr)) autorelease]; }

QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{

}

// 2 letter ISO 639-1
QString QSystemInfoPrivate::currentLanguage() const
{
 QString lang = QLocale::system().name().left(2);
    if(lang.isEmpty() || lang == "C") {
        lang = "en";
    }
    return lang;
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{

    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Available";
    bool useDate = false;
    if(parameter == "versionDate") {
        useDate = true;
    }
    switch(type) {
    case QSystemInfo::Os:
        {
            return nsstringToQString([[NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"] objectForKey:@"ProductVersion"]);
        }
        break;
    case QSystemInfo::QtCore:
       return  qVersion();
       break;
    case QSystemInfo::WrtCore:
        {
        }
       break;
    case QSystemInfo::Webkit:
        {
        }
       break;
    case QSystemInfo::ServiceFramework:
        {
        }
       break;
    case QSystemInfo::WrtExtensions:
        {
        }
       break;
    case QSystemInfo::ServiceProvider:
        {
        }
       break;
    case QSystemInfo::NetscapePlugin:
        {
        }
       break;
    case QSystemInfo::WebApp:
        {
        }
       break;
   case QSystemInfo::Firmware:
       {
       }
       break;
    };
  return errorStr;
}


//2 letter ISO 3166-1
QString QSystemInfoPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
#ifdef  MAC_SDK_10_6
            //IOBluetoothHostController
            //addressAsString
            IOBluetoothHostController* controller = [IOBluetoothHostController defaultController];
            if (controller != NULL) {
                featureSupported = true;
            }
#endif
        }
        break;
    case QSystemInfo::CameraFeature :
        {
//ICCameraDevice
            //ICCameraDeviceCanTakePicture
        }
        break;
    case QSystemInfo::FmradioFeature :
        {

        }
        break;
    case QSystemInfo::IrFeature :
        {
//kSCNetworkInterfaceTypeIrDA;
        }
        break;
    case QSystemInfo::LedFeature :
        {
//kHIDPage_LEDs
        }
        break;
    case QSystemInfo::MemcardFeature :
        {

        }
        break;
    case QSystemInfo::UsbFeature :
        {
               CFDictionaryRef 	match = 0;

               match = IOServiceMatching(kIOUSBDeviceClassName);
               if(match != NULL) {
                   featureSupported = true;
               }
        }
        break;
    case QSystemInfo::VibFeature :
        {

        }
        break;
    case QSystemInfo::WlanFeature :
        {

        }
        break;
    case QSystemInfo::SimFeature :
        {

        }
        break;
    case QSystemInfo::LocationFeature :
        {

        }
        break;
    case QSystemInfo::VideoOutFeature :
        {

        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    default:
        featureSupported = false;
        break;
    };
    return featureSupported;
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

bool QSystemNetworkInfoPrivate::isInterfaceActive(const char* netInterface)
{
    struct ifmediareq ifm;

    memset(&ifm, 0, sizeof(struct ifmediareq));
    strncpy(ifm.ifm_name, netInterface, IFNAMSIZ);
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    ioctl(s, SIOCGIFMEDIA, &ifm);
    if (ifm.ifm_status & IFM_ACTIVE) {
        return true;
    }
    return false;
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    QSystemNetworkInfo::NetworkStatus status = QSystemNetworkInfo::NoNetworkAvailable;
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
#ifdef MAC_SDK_10_6
            NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
            CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceForMode(mode).name())];
            [wifiInterface disassociate];
            switch([[wifiInterface interfaceState]intValue]) {
            case  kCWInterfaceStateInactive:
                break;
            case kCWInterfaceStateScanning:
            case kCWInterfaceStateAuthenticating:
            case kCWInterfaceStateAssociating:
                status = QSystemNetworkInfo::Searching;
                break;
            case kCWInterfaceStateRunning:
                status = QSystemNetworkInfo::Connected;
                break;
            };
            [autoreleasepool release];
#else
            if(isInterfaceActive(interfaceForMode(mode).name().toLatin1())) {
                status = QSystemNetworkInfo::Connected;
            }
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            if(networkSignalStrength(mode) == 100) {
                return QSystemNetworkInfo::Connected;
            } else {
                return QSystemNetworkInfo::NoNetworkAvailable;
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {

        }
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
        default:
        break;
    };
    return status;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
             int percent = 0;
#ifdef MAC_SDK_10_6
            CWInterface *wifiInterface = [CWInterface interface/*interfaceWithName: qstringToNSString(interfaceString)*/];
            int rssiSignal = [[wifiInterface rssi] intValue];
            
            int maxSignal = -60;
            int disSignal = -80;
            percent = 95 - 80*(maxSignal - rssiSignal)/(maxSignal - disSignal);
//            qWarning() << __FUNCTION__ << [[wifiInterface rssi] intValue] << percent;
#endif

            return percent;
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            int percent = (isInterfaceActive(interfaceForMode(mode).name().toLatin1())) ? 100 : 0;
            return percent;
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        // link quality for which device?
        // [controller readRSSIForDevice: blah?];
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        break;
    };
    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
    return -1;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return -1;
}

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return "No Network";
}

// Mobile Network Code
QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return "No Network";
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return "No Network";
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return "No Network";
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
#ifdef MAC_SDK_10_6
            qWarning() <<__FUNCTION__ << interfaceForMode(mode).name();
            QString name = interfaceForMode(mode).name();
            CWInterface *wifiInterface = [CWInterface interfaceWithName:qstringToNSString(name)];
           NSString *ssidStr = [wifiInterface ssid];
qWarning() << nsstringToQString(ssidStr);
            return nsstringToQString([wifiInterface ssid]);
#else
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        break;
    };
    return "";
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    QString mac;
#ifdef  MAC_SDK_10_6
    if(mode == QSystemNetworkInfo::BluetoothMode) {
        NSString *addy;
        IOBluetoothHostController* controller = [IOBluetoothHostController defaultController];
        if (controller != NULL) {
            addy = [controller addressAsString];
            mac = [addy UTF8String];
            mac.replace("-",":");
        }
        return mac;
    }
#endif
    mac = interfaceForMode(mode).hardwareAddress();
    return mac;
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    QNetworkInterface netInterface;

    CFArrayRef interfaceArray = SCNetworkInterfaceCopyAll(); //10.4
    CFStringRef iName;
    CFStringRef type;

    for ( long i = 0; i < CFArrayGetCount(interfaceArray); i++) {
        SCNetworkInterfaceRef thisInterface =  (SCNetworkInterfaceRef ) CFArrayGetValueAtIndex(interfaceArray, i);
        type = SCNetworkInterfaceGetInterfaceType(thisInterface);
        iName = SCNetworkInterfaceGetBSDName(thisInterface);
        //        qWarning() << stringFromCFString(type) <<stringFromCFString( iName);
        if (type != NULL) {
            if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth) && mode == QSystemNetworkInfo::BluetoothMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            } else if (CFEqual(type, kSCNetworkInterfaceTypeEthernet) && mode == QSystemNetworkInfo::EthernetMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            } else if (CFEqual(type, kSCNetworkInterfaceTypeIEEE80211) && mode == QSystemNetworkInfo::WlanMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            }
        }
    }
    CFRelease(interfaceArray);
    return netInterface;
}

//////// QSystemDisplayInfo
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int screen)
{
    int macScreens = 4;
    CGDisplayErr dErr;
    io_service_t service;
    CFStringRef key = CFSTR(kIODisplayBrightnessKey);

    float brightness = HUGE_VALF;
    int displayBrightness = -1;
    CGDirectDisplayID screensArray[macScreens]; //support 4 screens
    CGDisplayCount numberScreens;
    CGGetActiveDisplayList(macScreens, screensArray, &numberScreens);
    if(numberScreens >= (uint)screen) {
        service = CGDisplayIOServicePort(screensArray[screen]);
        dErr = IODisplayGetFloatParameter(service, kNilOptions, key, &brightness);
        if (dErr == kIOReturnSuccess) {
            displayBrightness = (int)(brightness * 100);
        }
    }
    return displayBrightness;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    int macScreens = 4;
    CGDirectDisplayID screensArray[macScreens]; //support 4 screens
    CGDisplayCount numberScreens;
    long bitsPerPixel = 0;
    CGGetActiveDisplayList(macScreens, screensArray, &numberScreens);
    if(numberScreens >= (uint)screen) {
        bitsPerPixel = CGDisplayBitsPerPixel (screensArray[screen]);
    }
    return (int)bitsPerPixel;
}

//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
        : QObject(parent)
{
}


QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

bool QSystemStorageInfoPrivate::updateVolumesMap()
{
    struct statfs64 *buf = NULL;
    unsigned i, count = 0;

    count = getmntinfo64(&buf, 0);
    for (i=0; i<count; i++) {
        char *volName = buf[i].f_mntonname;
        mountEntriesHash.insert(buf[i].f_mntfromname,volName);
    }
    return true;
}


qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    struct statfs64 *buf = NULL;
    unsigned i, count = 0;
    qint64 totalFreeBytes=0;

    count = getmntinfo64(&buf, 0);
    for (i=0; i<count; i++) {
        char *volName = buf[i].f_mntonname;
        if(driveVolume == QString(volName)) {
            totalFreeBytes = (buf[i].f_bavail * (buf[i].f_bsize/512));
        }
    }

    return  totalFreeBytes;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    struct statfs64 *buf = NULL;
    unsigned i, count = 0;
    qint64 totalBytes=0;

    count = getmntinfo64(&buf, 0);
    for (i=0; i<count; i++) {
        char *volName = buf[i].f_mntonname;
        if(driveVolume == QString(volName)) {
            totalBytes = (buf[i].f_blocks * (buf[i].f_bsize/512));
            return totalBytes;
        }
    }

    return totalBytes;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    updateVolumesMap();
    OSStatus osstatusResult = noErr;
    ItemCount volumeIndex;

    for (volumeIndex = 1; osstatusResult == noErr || osstatusResult != nsvErr; volumeIndex++) {
        FSVolumeRefNum actualVolume;
        HFSUniStr255 volumeName;
        FSVolumeInfo	volumeInfo;

        bzero((void *) &volumeInfo, sizeof(volumeInfo));

        osstatusResult = FSGetVolumeInfo(kFSInvalidVolumeRefNum, volumeIndex, &actualVolume, kFSVolInfoFSInfo,
                                         &volumeInfo, &volumeName, NULL);

        if (osstatusResult == noErr) {
            GetVolParmsInfoBuffer volumeParmeters;
            osstatusResult = FSGetVolumeParms(actualVolume, &volumeParmeters, sizeof(volumeParmeters));

            QString devId = QString((char *)volumeParmeters.vMDeviceID);
            devId = devId.prepend("/dev/");
            if(mountEntriesHash.value(devId) == driveVolume) {
                if (volumeParmeters.vMServerAdr == 0) { //local drive
                    io_service_t ioService;
                    ioService = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                            IOBSDNameMatching(kIOMasterPortDefault, 0,
                                                                              (char *)volumeParmeters.vMDeviceID));

                    if (IOObjectConformsTo(ioService, kIOMediaClass)) {
                        CFTypeRef wholeMedia;

                        wholeMedia = IORegistryEntryCreateCFProperty(ioService,
                                                                     CFSTR(kIOMediaContentKey),
                                                                     kCFAllocatorDefault,
                                                                     0);

                        if((volumeParmeters.vMExtendedAttributes & (1L << bIsRemovable))) {
                            CFRelease(wholeMedia);
                            return QSystemStorageInfo::RemovableDrive;
                        } else {
                            return QSystemStorageInfo::InternalDrive;
                        }
                    }
                } else {
                    return QSystemStorageInfo::RemoteDrive;
                }
            }
        }
    }
    return QSystemStorageInfo::NoDrive;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    QStringList drivesList;

    struct statfs64 *buf = NULL;
    unsigned i, count = 0;

    count = getmntinfo64(&buf, 0);
    for (i=0; i<count; i++) {
        char *volName = buf[i].f_mntonname;
//        qWarning() << "Volume:"
//                << volName
//                << buf[i].f_bfree
//                << buf[i].f_bavail
//                << buf[i].f_fstypename
//                << buf[i].f_type;
        if(buf[i].f_type != 19
           && buf[i].f_type != 20) {
            drivesList << volName;
        }
    }
    return drivesList;
}

QSystemDeviceInfoPrivate *QSystemDeviceInfoPrivate::self = 0;

void powerInfoChanged(void* runLoopInfo)
{
    Q_UNUSED(runLoopInfo)
    QSystemDeviceInfoPrivate::instance()->batteryLevel();
    QSystemDeviceInfoPrivate::instance()->currentPowerState();
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
    batteryLevelCache = 0;
    currentPowerStateCache = QSystemDeviceInfo::UnknownPower;
    batteryStatusCache = QSystemDeviceInfo::NoBatteryLevel;
    CFRunLoopSourceRef runLoopSource = (CFRunLoopSourceRef)IOPSNotificationCreateRunLoopSource(powerInfoChanged, this);
    if (runLoopSource) {
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
        CFRelease(runLoopSource);
    }
    if(!self)
        self = this;
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;
    return methods;
}


QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    QSystemDeviceInfo::PowerState state = QSystemDeviceInfo::UnknownPower;

    CFDictionaryRef powerSourceDict = NULL;
    CFStringRef powerStateString;

    CFTypeRef powerSourcesInfoBlob = IOPSCopyPowerSourcesInfo();
    CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourcesInfoBlob);
    int i;
    for (i=0; i<CFArrayGetCount(powerSourcesList); i++) {
        powerSourceDict = IOPSGetPowerSourceDescription(powerSourcesInfoBlob, /* [powerSourcesList objectAtIndex:0]*/ CFArrayGetValueAtIndex(powerSourcesList, i));
        if(!powerSourceDict) {
            state = QSystemDeviceInfo::UnknownPower;
            break;
        }

        powerStateString = (CFStringRef)CFDictionaryGetValue(powerSourceDict, CFSTR(kIOPSPowerSourceStateKey));
    //    qWarning() << stringFromCFString(powerStateString);
        if(CFStringCompare(powerStateString,CFSTR(kIOPSBatteryPowerValue),0)==kCFCompareEqualTo) {
            //has battery
            state = QSystemDeviceInfo::BatteryPower;
        } else {

            NSDictionary *powerSourceInfo = nil;
            powerSourceInfo = [NSDictionary dictionaryWithDictionary:(NSDictionary*)powerSourceDict];
            bool charging = (bool)[[powerSourceInfo valueForKey:[NSString stringWithCString:kIOPSIsChargingKey]] boolValue];
            if (charging ) {
                state = QSystemDeviceInfo::WallPowerChargingBattery;
            } else {
                state = QSystemDeviceInfo::WallPower;
            }
        }
    }

    CFRelease(powerSourcesInfoBlob);
    CFRelease(powerSourcesList);

    if( currentPowerStateCache != state) {
        currentPowerStateCache = state;
        emit powerStateChanged(state);
    }
    return state;
}

QString QSystemDeviceInfoPrivate::imei()
{
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::imsi()
{
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    return QString("Apple"); //pretty sure we can hardcode this one
}

QString QSystemDeviceInfoPrivate::model()
{
    char modelBuffer[256];
    QString model;
      size_t sz = sizeof(modelBuffer);
      if (0 == sysctlbyname("hw.model", modelBuffer, &sz, NULL, 0)) {
          model = modelBuffer;
      }
    return  model;
}

QString QSystemDeviceInfoPrivate::productName()
{
    return nsstringToQString([[NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"] objectForKey:@"ProductName"]);
}

int QSystemDeviceInfoPrivate::batteryLevel()
{
    float level = 0;
    CFDictionaryRef powerSourceDict = NULL;
    CFTypeRef powerSourcesInfoBlob = IOPSCopyPowerSourcesInfo();
    CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourcesInfoBlob);
    int i;
    for (i=0; i<CFArrayGetCount(powerSourcesList); i++) {
        powerSourceDict = IOPSGetPowerSourceDescription(powerSourcesInfoBlob, /* [powerSourcesList objectAtIndex:0]*/ CFArrayGetValueAtIndex(powerSourcesList, i));
        if(!powerSourceDict) {
            break;
        }

        float currentLevel = 0;
        float maxLevel = 0;
        const void *powerStateValue;

        powerStateValue = CFDictionaryGetValue(powerSourceDict, CFSTR(kIOPSCurrentCapacityKey));
        CFNumberGetValue((CFNumberRef)powerStateValue, kCFNumberSInt32Type, &currentLevel);

        powerStateValue = CFDictionaryGetValue(powerSourceDict, CFSTR(kIOPSMaxCapacityKey));
        CFNumberGetValue((CFNumberRef)powerStateValue, kCFNumberSInt32Type, &maxLevel);
        level = (currentLevel/maxLevel) * 100;
      //  qWarning() << __FUNCTION__<<level;
    }

    CFRelease(powerSourcesInfoBlob);
    CFRelease(powerSourcesList);    

    if(batteryLevelCache != level) {
        batteryLevelCache = level;
        emit batteryLevelChanged(level);
    }

    if(batteryLevelCache < 4 && batteryStatusCache != QSystemDeviceInfo::BatteryCritical) {
        batteryStatusCache = QSystemDeviceInfo::BatteryCritical;
        emit batteryStatusChanged(batteryStatusCache);
    } else if((batteryLevelCache > 3 && batteryLevelCache < 11) && batteryStatusCache != QSystemDeviceInfo::BatteryVeryLow) {
        batteryStatusCache = QSystemDeviceInfo::BatteryVeryLow;
        emit batteryStatusChanged(batteryStatusCache);
    } else if((batteryLevelCache > 10 && batteryLevelCache < 41) && batteryStatusCache != QSystemDeviceInfo::BatteryLow) {
        batteryStatusCache = QSystemDeviceInfo::BatteryLow;
        emit batteryStatusChanged(batteryStatusCache);
    } else if(batteryLevelCache > 40 && batteryStatusCache != QSystemDeviceInfo::BatteryNormal) {
        batteryStatusCache = QSystemDeviceInfo::BatteryNormal;
        emit batteryStatusChanged(batteryStatusCache);
    }

    return (int)level;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    return false;
}


bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return false;
}


bool QSystemScreenSaverPrivate::isScreenLockOn()
{
    return false;
}

QT_END_NAMESPACE
