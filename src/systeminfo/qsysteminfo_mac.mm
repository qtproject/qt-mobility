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
#include <QHostInfo>

#include <locale.h>

#include <IOBluetooth/IOBluetooth.h>

#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFNotificationCenter.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFLocale.h>
#include <ScreenSaver/ScreenSaverDefaults.h>
#include <dns_sd.h>

#include <QTKit/QTKit.h>

#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/IOTypes.h>

#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>
#include <IOKit/storage/IOBlockStorageDevice.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>

#include <CoreServices/CoreServices.h>

#include <qabstracteventdispatcher.h>

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QEventLoop>

#ifdef MAC_SDK_10_6
#include <CoreLocation/CLLocation.h>
#include <CoreLocation/CLLocationManager.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWGlobals.h>
#else
// 10.5 sdk is broken for this:
#include </Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/CoreServices.framework/Frameworks/OSServices.framework/Headers/Power.h>
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
{ return QString([nsstr UTF8String]);}

inline NSString *qstringToNSString(const QString &qstr)
{ return [reinterpret_cast<const NSString *>(qstringToCFStringRef(qstr)) autorelease]; }

inline QStringList nsarrayToQStringList(void *nsarray)
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    QStringList result;
    NSArray *array = static_cast<NSArray *>(nsarray);
    for (NSUInteger i=0; i<[array count]; ++i)
        result << nsstringToQString([array objectAtIndex:i]);
    [autoreleasepool release];
    return result;
}

bool hasIOServiceMatching(const QString &classstr)
{
    io_iterator_t ioIterator = NULL;
    IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceNameMatching(classstr.toAscii()), &ioIterator);
    if(ioIterator) {
        return true;
    }
    return false;
}



#ifdef MAC_SDK_10_6

@interface QtMNSListener : NSObject
{
    NSNotificationCenter *center;
    CWInterface * currentInterface;
}
- (void)notificationHandler:(NSNotification *)notification;
- (void)remove;
@end

@implementation QtMNSListener
- (id) init
{
   [super init];
    center = [NSNotificationCenter defaultCenter];
    currentInterface = [CWInterface interface];

    [center addObserver:self selector:@selector(notificationHandler:) name:kCWModeDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWSSIDDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWBSSIDDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWCountryCodeDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWLinkDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWPowerDidChangeNotification object:nil];


    return self;
}

-(void)dealloc
{
   [center release];
   [currentInterface release];
   [super dealloc];
}

-(void)remove
{
    [center removeObserver:self];
}

- (void)notificationHandler:(NSNotification *)notification
{
    QTM_NAMESPACE::QSystemNetworkInfoPrivate::instance()->wifiNetworkChanged( nsstringToQString([notification name]), nsstringToQString([[notification object]name]));
}
@end


@interface QtMLangListener : NSObject
{
    NSNotificationCenter *center;
    QString currentLanguage;
}
- (void)languageHandler;//:(NSNotification *)notification;
- (void)remove;
- (void)getCurrentLanguage;
@end



@implementation QtMLangListener
- (id) init
{
    [super init];
    center = [NSNotificationCenter defaultCenter];
    [center addObserver:self selector:@selector(languageHandler:) name:NSCurrentLocaleDidChangeNotification object:nil];
    [self getCurrentLanguage];
    return self;
}

-(void)dealloc
{
    [center release];
    [super dealloc];
}

-(void)remove
{
    [center removeObserver:self];
}

- (void)getCurrentLanguage
{
    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSArray* languages = [defs objectForKey:@"AppleLanguages"];
    NSString* language = [languages objectAtIndex:0];

    QString langString = nsstringToQString(language);
    if(langString != currentLanguage) {
        if(!currentLanguage.isEmpty())
            QTM_NAMESPACE::QSystemInfoPrivate::instance()->languageChanged(currentLanguage);
        currentLanguage = langString;
    }
}

- (void)languageHandler;//:(NSNotification *)notification
{
    [self getCurrentLanguage];
}
@end


@interface RemoteDeviceRSSIHostControllerDelegate : NSObject
{
}
// See IOBluetoothHostControllerDelegate
- (void)readRSSIForDeviceComplete:(id)controller device:(IOBluetoothDevice*)device info:(BluetoothHCIRSSIInfo*)info error:(IOReturn)error;
@end

@implementation RemoteDeviceRSSIHostControllerDelegate
- (id) init
{
   [super init];
   return self;
}

- (void)readRSSIForDeviceComplete:(id)controller device:(IOBluetoothDevice*)device info:(BluetoothHCIRSSIInfo*)info error:(IOReturn)error
{
    Q_UNUSED(controller);
    Q_UNUSED(device);

    if ((error != kIOReturnSuccess) || (info == NULL)) {
        qWarning() << "ERROR: readRSSIForDeviceComplete return error";

    } else if (info->handle == kBluetoothConnectionHandleNone) {
        qWarning() << "ERROR: readRSSIForDeviceComplete no connection";
    } else {
        NSLog(@"Rssi value: %@", info->RSSIValue);
    }
}
@end
#endif
NSObject* delegate;

QTM_BEGIN_NAMESPACE

QSystemInfoPrivate *QSystemInfoPrivate::self = 0;

QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
    langloopThread = new QLangLoopThread(this);
    langloopThread->start();
    if(!self)
        self = this;
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
    langloopThread->quit();
    langloopThread->wait();
}

QString QSystemInfoPrivate::currentLanguage() const
{
 QString lang = QLocale::system().name().left(2);
    if(lang.isEmpty() || lang == QLatin1String("C")) {
        lang = QLatin1String("en");
    }
    return lang;
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSArray* languages = [defs objectForKey:@"AppleLanguages"];
    QStringList langList = nsarrayToQStringList(languages);

    QStringList returnList;
    for(int i = 0; i < langList.count(); i++) {
     QString language = langList.at(i).left(2);
     if(!returnList.contains(language))
         returnList << language;
    }
    [pool drain];
    return returnList;
}

void QSystemInfoPrivate::languageChanged(const QString &lang)
{
    Q_EMIT currentLanguageChanged(lang);
}


QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Available";
    bool useDate = false;
    if(parameter == QLatin1String("versionDate")) {
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
   case QSystemInfo::Firmware:
       {
           return QSystemDeviceInfoPrivate::model();
       }
       break;
    };
  return errorStr;
}


QString QSystemInfoPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature:
        {
#ifdef  MAC_SDK_10_6
            IOBluetoothHostController* controller = [IOBluetoothHostController defaultController];
            if (controller != NULL) {
                featureSupported = true;
            }
#endif
        }
        break;
    case QSystemInfo::CameraFeature:
        {

           NSArray * videoDevices;
           videoDevices = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]];
           if([videoDevices count] > 0) {
               featureSupported = true;
           }
        }
        break;
    case QSystemInfo::FmradioFeature:
        break;
    case QSystemInfo::IrFeature:
        {
            if(hasIOServiceMatching("AppleIRController")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::LedFeature:
        {
//kHIDPage_LEDs
        }
        break;
    case QSystemInfo::MemcardFeature:
        {
// IOSCSIPeripheralDeviceType0E
            if(hasIOServiceMatching("IOUSBMassStorageClass")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::UsbFeature:
        {
            if(hasIOServiceMatching("AppleUSBOHCI")) {
                featureSupported = true;
            }
            if(hasIOServiceMatching("AppleUSBEHCI")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::VibFeature:
        {
        }
        break;
    case QSystemInfo::WlanFeature:
        {
            if(!QSystemNetworkInfoPrivate::instance()->interfaceForMode(QSystemNetworkInfo::WlanMode).name().isEmpty()) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::SimFeature:
        {
        }
        break;
    case QSystemInfo::LocationFeature:
        {
#ifdef MAC_SDK_10_6
            CLLocationManager *locationManager = [[CLLocationManager alloc] init];
            if ([locationManager locationServicesEnabled]) {
                featureSupported = true;
            }
            [locationManager release];
#endif
        }
        break;
    case QSystemInfo::VideoOutFeature:
        {
            ComponentDescription description = {'vout', 0, 0, 0L, 1L << 0};
            if( ::CountComponents(&description) > 0) {
                featureSupported = true;
            }
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

QSystemNetworkInfoPrivate *QSystemNetworkInfoPrivate::self = 0;


void networkChangeCallback(SCDynamicStoreRef /*dynamicStore*/, CFArrayRef changedKeys, void */*networkConfigurationManagerPrivate*/)
{
// NSLog(@"changed keys %@", changedKeys);
    QStringList keyList = nsarrayToQStringList((void*)changedKeys);
    if(keyList.contains("State:/Network/Global/DNS")) {
    }
    if(keyList.contains("State:/Network/Global/IPv4")) {
        QTM_NAMESPACE::QSystemNetworkInfoPrivate::instance()->ethernetChanged();
        QTM_NAMESPACE::QSystemNetworkInfoPrivate::instance()->getDefaultInterface();
    }

    return;
}

#ifdef MAC_SDK_10_6
#endif
QLangLoopThread::QLangLoopThread(QObject *parent)
    :QThread(parent)
{
}

QLangLoopThread::~QLangLoopThread()
{
}

void QLangLoopThread::quit()
{
    mutex.lock();
    keepRunning = false;
    mutex.unlock();
}

void QLangLoopThread::run()
{
#ifdef MAC_SDK_10_6
    mutex.lock();
    keepRunning = true;
    mutex.unlock();
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    QtMLangListener *listener;
    listener = [[QtMLangListener alloc] init];

    NSDate *loopUntil = [NSDate dateWithTimeIntervalSinceNow:1.0];
    while (keepRunning &&
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: loopUntil]) {
        loopUntil = [NSDate dateWithTimeIntervalSinceNow:1.0];
    }
    [pool release];
#endif
}


QRunLoopThread::QRunLoopThread(QObject *parent)
    :QThread(parent)
{
}

QRunLoopThread::~QRunLoopThread()
{
}

void QRunLoopThread::quit()
{
    CFRelease(runloopSource);
    mutex.lock();
    keepRunning = false;
    mutex.unlock();
    CFRelease(storeSession);
}

void QRunLoopThread::run()
{
#ifdef MAC_SDK_10_6

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    startNetworkChangeLoop();
    delegate = [[RemoteDeviceRSSIHostControllerDelegate alloc] init];

    mutex.lock();
    keepRunning = true;
    mutex.unlock();

    QtMNSListener *listener;
    listener = [[QtMNSListener alloc] init];

    NSDate *loopUntil = [NSDate dateWithTimeIntervalSinceNow:1.0];
    while (keepRunning &&
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: loopUntil]) {
        loopUntil = [NSDate dateWithTimeIntervalSinceNow:1.0];
    }
    [pool release];
#endif
}

void QRunLoopThread::startNetworkChangeLoop()
{
    storeSession = NULL;

    SCDynamicStoreContext dynStoreContext = { 0, this /*(void *)storeSession*/, NULL, NULL, NULL };
    storeSession = SCDynamicStoreCreate(NULL,
                                 CFSTR("networkChangeCallback"),
                                 networkChangeCallback,
                                 &dynStoreContext);
    if (!storeSession ) {
        qWarning() << "could not open dynamic store: error:" << SCErrorString(SCError());
        return;
    }

    CFMutableArrayRef notificationKeys;
    notificationKeys = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    CFMutableArrayRef patternsArray;
    patternsArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

    CFStringRef storeKey;

    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetLink);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetLink);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetDNS);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetDNS);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetIPv4);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetIPv4);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    if (!SCDynamicStoreSetNotificationKeys(storeSession , notificationKeys, patternsArray)) {
        qWarning() << "register notification error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        CFRelease(notificationKeys);
        CFRelease(patternsArray);
        return;
    }
    CFRelease(notificationKeys);
    CFRelease(patternsArray);

    runloopSource = SCDynamicStoreCreateRunLoopSource(NULL, storeSession , 0);
    if (!runloopSource) {
        qWarning() << "runloop source error:"<< SCErrorString(SCError());
        CFRelease(storeSession);
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    return;
}


QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        : QObject(parent), signalStrengthCache(0)
{
     defaultInterface = "";
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");
#ifdef MAC_SDK_10_6
if([[CWInterface supportedInterfaces] count] > 0 ) {
        hasWifi = true;
    } else {
        hasWifi = false;
    }
#endif
    rssiTimer = new QTimer(this);

    if(!self)
        self = this;
    QTimer::singleShot(200, this, SLOT(primaryInterface()));
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
#ifdef MAC_SDK_10_6
    if(hasWifi) {
        runloopThread->quit();
        runloopThread->wait();
        [delegate release];
    }
#endif
}

void QSystemNetworkInfoPrivate::primaryInterface()
{
    defaultInterface = getDefaultInterface();
}

void QSystemNetworkInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))) {
        connect(rssiTimer, SIGNAL(timeout()), this, SLOT(rssiTimeout()));
        rssiTimer->start(1000);
    }
    if (QLatin1String(signal) == SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString))
        || QLatin1String(signal) == SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus))) {
#ifdef MAC_SDK_10_6
        if(hasWifi) {
            runloopThread = new QRunLoopThread(this);
            runloopThread->start();
        }
#endif
    }
}

void QSystemNetworkInfoPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int))) {
        rssiTimer->stop();
        disconnect(rssiTimer, SIGNAL(timeout()), this, SLOT(rssiTimeout()));
    }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::modeForInterface(QString interfaceName)
{
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    CFArrayRef interfaceArray = SCNetworkInterfaceCopyAll(); //10.4
    CFStringRef iName;
    CFStringRef type;

    for ( long i = 0; i < CFArrayGetCount(interfaceArray); i++) {
        SCNetworkInterfaceRef thisInterface =  (SCNetworkInterfaceRef ) CFArrayGetValueAtIndex(interfaceArray, i);
        type = SCNetworkInterfaceGetInterfaceType(thisInterface);
        iName = SCNetworkInterfaceGetBSDName(thisInterface);
        if( interfaceName == stringFromCFString(iName)) {
            if (type != NULL) {
                if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth)) {
                    mode = QSystemNetworkInfo::BluetoothMode;
                    break;
                } else if (CFEqual(type, kSCNetworkInterfaceTypeEthernet)) {
                    mode = QSystemNetworkInfo::EthernetMode;
                    break;
                } else if (CFEqual(type, kSCNetworkInterfaceTypeIEEE80211)) {
                    mode = QSystemNetworkInfo::WlanMode;
                    break;
                }
            }
        }
    }
    CFRelease(interfaceArray);
    return mode;
}

QString QSystemNetworkInfoPrivate::getDefaultInterface()
{
    SCDynamicStoreRef storeSession2;
    CFStringRef key;
    CFDictionaryRef	globalDict;
    CFStringRef primaryInterface = NULL;
    QString interfaceName;

    storeSession2 = SCDynamicStoreCreate(NULL, CFSTR("getPrimary"), NULL, NULL);
    if (!storeSession2) {
    }
    key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState,kSCEntNetIPv4);
    globalDict = (const __CFDictionary*)SCDynamicStoreCopyValue(storeSession2, key);
    CFRelease(key);
    if (globalDict) {

        primaryInterface = (CFStringRef )CFDictionaryGetValue(globalDict,
                                                                   kSCDynamicStorePropNetPrimaryInterface);
        if (primaryInterface) {
            CFRetain(primaryInterface);
        }
        CFRelease(globalDict);
    }

    CFRelease(storeSession2);
    if (primaryInterface) {
        interfaceName = stringFromCFString(primaryInterface);
        if(interfaceName != defaultInterface) {
            Q_EMIT networkModeChanged(modeForInterface(interfaceName));
             defaultInterface = interfaceName;
        }
    }

    return interfaceName;
}

void QSystemNetworkInfoPrivate::rssiTimeout()
{
    networkSignalStrength(QSystemNetworkInfo::WlanMode);
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

void QSystemNetworkInfoPrivate::ethernetChanged()
{
    QSystemNetworkInfo::NetworkStatus status = QSystemNetworkInfo::NoNetworkAvailable;
    int carrier = 0;

    if(isInterfaceActive(interfaceForMode(QSystemNetworkInfo::EthernetMode).name().toLocal8Bit())) {
        status = QSystemNetworkInfo::Connected;
        carrier = 100;
    }
    Q_EMIT networkStatusChanged(QSystemNetworkInfo::EthernetMode,status);
    Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::EthernetMode,carrier);
    Q_EMIT networkNameChanged(QSystemNetworkInfo::EthernetMode, networkName(QSystemNetworkInfo::EthernetMode));
    Q_EMIT networkModeChanged(modeForInterface(getDefaultInterface()));
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
            if(hasWifi) {
                NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
                CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceForMode(mode).name())];

                if([wifiInterface power]) {
                    if(!rssiTimer->isActive())
                        rssiTimer->start(1000);
                }  else {
                    if(rssiTimer->isActive())
                        rssiTimer->stop();
                }

                switch([[wifiInterface interfaceState]intValue]) {
                case  kCWInterfaceStateInactive:
                    status = QSystemNetworkInfo::NoNetworkAvailable;
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
            }

#else
            if(isInterfaceActive(interfaceForMode(mode).name().toLatin1())) {
                status = QSystemNetworkInfo::Connected;
            }
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            if(isInterfaceActive(interfaceForMode(mode).name().toLatin1())) {
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
            int signalQuality = 0;
            if(hasWifi) {
#ifdef MAC_SDK_10_6
                NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
                QString name = interfaceForMode(mode).name();
                CWInterface *wifiInterface = [CWInterface interfaceWithName:qstringToNSString(name)];

                if([wifiInterface power]) {
                    if(!rssiTimer->isActive())
                        rssiTimer->start(1000);
                }  else {
                    if(rssiTimer->isActive())
                        rssiTimer->stop();
                }

                int rssiSignal = [[wifiInterface rssi] intValue];

                if(rssiSignal !=0 ) {
                    int maxRssi = -40;
                    int minRssi = [[wifiInterface noise] intValue];
                    signalQuality = ( 100 * (maxRssi - minRssi) * (maxRssi - minRssi) - (maxRssi - rssiSignal) *
                                      (15 * (maxRssi - minRssi) + 62 * (maxRssi - rssiSignal)) ) /
                                    ((maxRssi - minRssi) * (maxRssi - minRssi));

                } else {
                    signalQuality = 0;
                }

                if(signalStrengthCache != signalQuality) {
                    if(signalStrengthCache == 0) {
                        networkStatus(QSystemNetworkInfo::WlanMode);
                    }
                    signalStrengthCache = signalQuality;
                    Q_EMIT networkSignalStrengthChanged(mode, signalQuality);
                }
                [autoreleasepool release];
#endif
            }
            return signalQuality;
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            int percent = (isInterfaceActive(interfaceForMode(mode).name().toLatin1())) ? 100 : 0;
            return percent;
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        {
#ifdef  MAC_SDK_10_6
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            IOBluetoothHostController* controller = IOBluetoothHostController.defaultController;
            if (controller != NULL) {

                NSArray *devices = [IOBluetoothDevice recentDevices:0];
                for ( IOBluetoothDevice *btDevice in devices ) {
                    if([btDevice isConnected]) {
                        qWarning() <<"IOBluetoothDevice connected"<< nsstringToQString([btDevice getName]);
//                        delegate = [[RemoteDeviceRSSIHostControllerDelegate alloc] init];
                        [delegate retain];
                        [controller setDelegate:delegate];
                        IOReturn rc = [controller readRSSIForDevice:btDevice];
                        if (rc != noErr) {
                            qWarning() << "ERROR: call readRSSIForDevice failed";
                        }
//[delegate release];
                    }
                }
//              [devices release];
  //  [controller release];
            }
            [pool release];
#endif
        }
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

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
#if defined(MAC_SDK_10_6)
    if(hasWifi) {
        CWInterface *primary = [CWInterface interface ];
        if([primary power]) {
            return  nsstringToQString( [primary countryCode]);
        }
    }
#endif
    return "";
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return "";
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    if(networkStatus(mode) == QSystemNetworkInfo::NoNetworkAvailable) {
        return "";
    }
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
            QString name = interfaceForMode(mode).name();
#ifdef MAC_SDK_10_6
            if(hasWifi) {
                CWInterface *wifiInterface = [CWInterface interfaceWithName:qstringToNSString(name)];
                return nsstringToQString([wifiInterface ssid]);
            }
#else
            SCDynamicStoreRef theDynamicStore;
            theDynamicStore = SCDynamicStoreCreate(nil, CFSTR("FindCurrentInterfaceAndIP"), nil, nil);

            NSMutableString *interfaceName = [NSMutableString string];
            NSString *airportPath = [NSString stringWithFormat:@"State:/Network/Interface/%@/AirPort", qstringToNSString(name)];

            CFDictionaryRef airportPlist = (const __CFDictionary*)SCDynamicStoreCopyValue(theDynamicStore, (CFStringRef)airportPath);

            CFRelease(theDynamicStore);

            return nsstringToQString([(NSDictionary *)airportPlist valueForKey:@"SSID_STR"]);
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            if(isInterfaceActive(interfaceForMode(mode).name().toLocal8Bit())) {
                return QHostInfo::localDomainName();
            }
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
    return interfaceForMode(mode).hardwareAddress();
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

        if (type != NULL) {
            if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth) && mode == QSystemNetworkInfo::BluetoothMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                // workaround for null MAC from SCNetworkInterfaceGetHardwareAddressString and bogus BSD name here
#ifdef  MAC_SDK_10_6
                IOBluetoothHostController* controller = IOBluetoothHostController.defaultController;
                QString macbtMac = nsstringToQString([controller addressAsString]).replace("-",":").toUpper();
                if(!macbtMac.isEmpty()) {
                    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
                    foreach(const QNetworkInterface thisNetInterface, interfaces) {
                        if( thisNetInterface.hardwareAddress() == macbtMac) {
                            netInterface = thisNetInterface;
                            break;
                        }
                    }
                }
#endif
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

void QSystemNetworkInfoPrivate::wifiNetworkChanged(const QString &notification, const QString interfaceName)
{
    getDefaultInterface();

    if(notification == QLatin1String("SSID_CHANGED_NOTIFICATION")) {
        Q_EMIT networkNameChanged(QSystemNetworkInfo::WlanMode, networkName(QSystemNetworkInfo::WlanMode));
    }

    if(notification == QLatin1String("BSSID_CHANGED_NOTIFICATION")) {
        QSystemNetworkInfo::NetworkStatus status =  networkStatus(QSystemNetworkInfo::WlanMode);
        Q_EMIT networkStatusChanged( QSystemNetworkInfo::WlanMode, status);
    }
    if(notification == QLatin1String("POWER_CHANGED_NOTIFICATION")) {
#ifdef MAC_SDK_10_6
        CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceName)];
        if([wifiInterface power]) {
            if(!rssiTimer->isActive()) {
                rssiTimer->start(1000);

            }
        }  else {
            if(rssiTimer->isActive()) {
                rssiTimer->stop();
            }
            Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, 0);
        }
#endif
    }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    return modeForInterface(getDefaultInterface());
}


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
            devId = devId.prepend(QLatin1String("/dev/"));
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
    QSystemDeviceInfo::InputMethodFlags methods = 0;

    if(hasIOServiceMatching("AppleUSBTCButtons")) {
        methods = (methods | QSystemDeviceInfo::Keys);
    }
    if(hasIOServiceMatching("AppleUSBTCKeyboard")) {
        methods = (methods | QSystemDeviceInfo::Keyboard);
    }
    if(hasIOServiceMatching("AppleUSBMultitouchDriver")) {
        methods = (methods | QSystemDeviceInfo::MultiTouch);
    }
    if(hasIOServiceMatching("IOHIDPointing")) {
        methods = (methods | QSystemDeviceInfo::Mouse);
    }
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
        if(CFStringCompare(powerStateString,CFSTR(kIOPSBatteryPowerValue),0)==kCFCompareEqualTo) {
            //has battery
            state = QSystemDeviceInfo::BatteryPower;
        } else {

            NSDictionary *powerSourceInfo = nil;
            powerSourceInfo = [NSDictionary dictionaryWithDictionary:(NSDictionary*)powerSourceDict];
            bool charging = (bool)[[powerSourceInfo valueForKey:[NSString stringWithUTF8String:kIOPSIsChargingKey]] boolValue];
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
        Q_EMIT powerStateChanged(state);
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
          model = QLatin1String(modelBuffer);
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
    }

    CFRelease(powerSourcesInfoBlob);
    CFRelease(powerSourcesList);

    if(batteryLevelCache != level) {
        batteryLevelCache = level;
        Q_EMIT batteryLevelChanged(level);
    }

    if(batteryLevelCache < 4 && batteryStatusCache != QSystemDeviceInfo::BatteryCritical) {
        batteryStatusCache = QSystemDeviceInfo::BatteryCritical;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    } else if((batteryLevelCache > 3 && batteryLevelCache < 11) && batteryStatusCache != QSystemDeviceInfo::BatteryVeryLow) {
        batteryStatusCache = QSystemDeviceInfo::BatteryVeryLow;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    } else if((batteryLevelCache > 10 && batteryLevelCache < 41) && batteryStatusCache != QSystemDeviceInfo::BatteryLow) {
        batteryStatusCache = QSystemDeviceInfo::BatteryLow;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    } else if(batteryLevelCache > 40 && batteryStatusCache != QSystemDeviceInfo::BatteryNormal) {
        batteryStatusCache = QSystemDeviceInfo::BatteryNormal;
        Q_EMIT batteryStatusChanged(batteryStatusCache);
    }

    return (int)level;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    // find out if auto login is being used.
    QSettings loginSettings("/Library/Preferences/com.apple.loginwindow.plist", QSettings::NativeFormat);
    QString autologinname = loginSettings.value("autoLoginUser").toString();

// find out if locked screensaver is used.
    int passWordProtected = 0;
#if defined(QT_ARCH_X86_64)  && !defined(MAC_SDK_10_6)
    ScreenSaverDefaults *ssDefaults;
    ssDefaults = [ScreenSaverDefaults defaultsForModuleWithName:@"com.apple.screensaver"];
    passWordProtected = [ssDefaults integerForKey:@"askForPassword"];
#endif

    if(autologinname.isEmpty() || passWordProtected == 1) {
        return true;
    } else {
        return false;
    }
}

QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent), isInhibited(0)
{
    ssTimer = new QTimer(this);
    connect(ssTimer, SIGNAL(timeout()), this, SLOT(activityTimeout()));
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
    if(ssTimer->isActive())
        ssTimer->stop();
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    activityTimeout();
    ssTimer->start(1000 * 60);
    if(ssTimer->isActive()) {
        isInhibited = true;
        return isInhibited;
    }

    return false;
}


bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return isInhibited;
}

void QSystemScreenSaverPrivate::activityTimeout()
{
    UpdateSystemActivity(OverallAct);
}

#include "moc_qsysteminfo_mac_p.cpp"

QTM_END_NAMESPACE
