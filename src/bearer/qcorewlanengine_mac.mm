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

#include "qcorewlanengine_mac_p.h"
#include "qnetworkconfiguration_p.h"

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#include <QDir>
#if defined(MAC_SDK_10_6) //not much functionality without this
#include <CoreWLAN/CoreWLAN.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CW8021XProfile.h>

#endif

#include <Foundation/NSEnumerator.h>
#include <Foundation/NSKeyValueObserving.h>
#include <Foundation/NSAutoreleasePool.h>

#include <SystemConfiguration/SCNetworkConfiguration.h>
QMap <QString, QString> networkInterfaces;

#ifdef MAC_SDK_10_6
@interface QNSListener : NSObject
{
    NSNotificationCenter *center;
    CWInterface * currentInterface;
}
- (void)notificationHandler;//:(NSNotification *)notification;
- (void)remove;
@end

@implementation QNSListener
- (id) init
{
    [super init];
    center = [NSNotificationCenter defaultCenter];
    currentInterface = [CWInterface interface];
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

- (void)notificationHandler;//:(NSNotification *)notification
{
    QTM_NAMESPACE::QCoreWlanEngine::instance()->requestUpdate();
}
@end
#endif

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QCoreWlanEngine, coreWlanEngine)

inline QString cfstringRefToQstring(CFStringRef cfStringRef) {
    QString retVal;
    CFIndex maxLength = 2 * CFStringGetLength(cfStringRef) + 1/*zero term*/; // max UTF8
    char *cstring = new char[maxLength];
    if (CFStringGetCString(CFStringRef(cfStringRef), cstring, maxLength, kCFStringEncodingUTF8)) {
        retVal = QString::fromUtf8(cstring);
    }
    delete[] cstring;
    return retVal;
}

inline CFStringRef qstringToCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

inline NSString *qstringToNSString(const QString &qstr)
{ return [reinterpret_cast<const NSString *>(qstringToCFStringRef(qstr)) autorelease]; }

inline QString nsstringToQString(const NSString *nsstr)
{ return cfstringRefToQstring(reinterpret_cast<const CFStringRef>(nsstr)); }

inline QStringList nsarrayToQStringList(void *nsarray)
{
    QStringList result;
    NSArray *array = static_cast<NSArray *>(nsarray);
    for (NSUInteger i=0; i<[array count]; ++i)
        result << nsstringToQString([array objectAtIndex:i]);
    return result;
}

static QString qGetInterfaceType(const QString &interfaceString)
{
    return networkInterfaces.value(interfaceString, QLatin1String("Unknown"));
}

void networkChangeCallback(SCDynamicStoreRef/* store*/, CFArrayRef changedKeys, void *info)
{
    for ( long i = 0; i < CFArrayGetCount(changedKeys); i++) {

        CFStringRef changed = (CFStringRef)CFArrayGetValueAtIndex(changedKeys, i);
        if( cfstringRefToQstring(changed).contains("/Network/Global/IPv4")) {
            QCoreWlanEngine* wlanEngine = static_cast<QCoreWlanEngine*>(info);
            wlanEngine->requestUpdate();
        }
    }
    return;
}

QCoreWlanEngine::QCoreWlanEngine(QObject *parent)
:   QNetworkSessionEngine(parent)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    getAllScInterfaces();
    startNetworkChangeLoop();
#ifdef MAC_SDK_10_6
    if([[CWInterface supportedInterfaces] count] > 0 ) {
        QNSListener *listener;
        listener = [[QNSListener alloc] init];
        hasWifi = true;
    } else {
        hasWifi = false;
    }
#endif
    getUserConfigurations();
    requestUpdate();
    [pool release];
}

QCoreWlanEngine::~QCoreWlanEngine()
{
    QNetworkConfigurationPrivate* cpPriv = 0;
    foundConfigurations.clear();
    while(!foundConfigurations.isEmpty()) {
        cpPriv = foundConfigurations.takeFirst();
        delete cpPriv;
    }
}

QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = true;
    foundConfigurations.clear();

    uint identifier;
    QMapIterator<QString, QString> i(networkInterfaces);
    QNetworkConfigurationPrivate* cpPriv = 0;
    while (i.hasNext()) {
        i.next();
        if (i.value() == "WLAN") {
            QList<QNetworkConfigurationPrivate *> fetchedConfigurations = scanForSsids(i.key());
            for (int i = 0; i < fetchedConfigurations.count(); ++i) {

                QNetworkConfigurationPrivate *config = new QNetworkConfigurationPrivate();
                cpPriv = fetchedConfigurations.at(i);
                config->name = cpPriv->name;
                config->isValid = cpPriv->isValid;
                config->id = cpPriv->id;

                config->state = cpPriv->state;
                config->type = cpPriv->type;
                config->roamingSupported = cpPriv->roamingSupported;
                config->purpose = cpPriv->purpose;
                config->internet = cpPriv->internet;
                config->serviceInterface = cpPriv->serviceInterface;
                config->bearer = cpPriv->bearer;

                identifier = config->name.toUInt();
                configurationInterface[identifier] =  config->serviceInterface.name();
                foundConfigurations.append(config);
                delete cpPriv;
            }
        }

        QNetworkInterface interface = QNetworkInterface::interfaceFromName(i.key());
        QNetworkConfigurationPrivate *cpPriv = new QNetworkConfigurationPrivate();
        const QString humanReadableName = interface.humanReadableName();
        cpPriv->name = humanReadableName.isEmpty() ? interface.name() : humanReadableName;
        cpPriv->isValid = true;

        if (interface.index())
            identifier = interface.index();
        else
            identifier = qHash(interface.hardwareAddress());

        cpPriv->id = QString::number(identifier);
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        cpPriv->state = QNetworkConfiguration::Undefined;

        if (interface.flags() & QNetworkInterface::IsRunning) {
            cpPriv->state = QNetworkConfiguration::Defined;
            cpPriv->internet = true;
        }
        if ( !interface.addressEntries().isEmpty())  {
            cpPriv->state |= QNetworkConfiguration::Active;
            cpPriv->internet = true;
        }
        configurationInterface[identifier] = interface.name();
        cpPriv->bearer = interface.name().isEmpty()? QLatin1String("Unknown") : qGetInterfaceType(interface.name());
        foundConfigurations.append(cpPriv);
    }

    pollTimer.start();
    return foundConfigurations;
}

QString QCoreWlanEngine::getInterfaceFromId(const QString &id)
{
    return configurationInterface.value(id.toUInt());
}

bool QCoreWlanEngine::hasIdentifier(const QString &id)
{
    return configurationInterface.contains(id.toUInt());
}

void QCoreWlanEngine::connectToId(const QString &id)
{
    QString interfaceString = getInterfaceFromId(id);

    if(networkInterfaces.value(interfaceString) == "WLAN") {
#if defined(MAC_SDK_10_6)
        NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
        CWInterface *wifiInterface = [CWInterface interfaceWithName: qstringToNSString(interfaceString)];

        if([wifiInterface power]) {
            NSError *err = nil;
            NSMutableDictionary *params = [NSMutableDictionary dictionaryWithCapacity:0];

            NSString *wantedSsid = 0;
            bool okToProceed = true;

            if(getNetworkNameFromSsid(id) != id) {
                NSArray *array = [CW8021XProfile allUser8021XProfiles];
                for (NSUInteger i=0; i<[array count]; ++i) {
                    if(id == nsstringToQString([[array objectAtIndex:i] userDefinedName])
                        || id == nsstringToQString([[array objectAtIndex:i] ssid]) ) {
                        QString thisName = getSsidFromNetworkName(id);
                        if(thisName.isEmpty()) {
                            wantedSsid = qstringToNSString(id);
                        } else {
                            wantedSsid = qstringToNSString(thisName);
                        }
                        okToProceed = false;
                        [params setValue: [array objectAtIndex:i] forKey:kCWAssocKey8021XProfile];
                        break;
                    }
                }
            }

            if(okToProceed) {
                NSUInteger index = 0;

                CWConfiguration *userConfig = [ wifiInterface configuration];
                NSSet *remNets = [userConfig rememberedNetworks];
                NSEnumerator *enumerator = [remNets objectEnumerator];
                CWWirelessProfile *wProfile;

                while ((wProfile = [enumerator nextObject])) {
                    if(id == nsstringToQString([wProfile ssid])) {

                        wantedSsid = [wProfile ssid];
                        [params setValue: [wProfile passphrase] forKey: kCWAssocKeyPassphrase];
                        break;
                    }
                    index++;
                }
            }

            NSDictionary *parametersDict = nil;
            NSArray *apArray = [NSMutableArray arrayWithArray:[wifiInterface scanForNetworksWithParameters:parametersDict error:&err]];

            if(!err) {
                for(uint row=0; row < [apArray count]; row++ ) {
                    CWNetwork *apNetwork = [apArray objectAtIndex:row];
                    if([[apNetwork ssid] compare:wantedSsid] == NSOrderedSame) {
                        bool result = [wifiInterface associateToNetwork: apNetwork parameters:[NSDictionary dictionaryWithDictionary:params] error:&err];

                        if(!result) {
                            emit connectionError(id, ConnectError);
                        } else {
                            [autoreleasepool release];
                            return;
                        }
                    }
                }
            } else {
                qDebug() <<"ERROR"<< nsstringToQString([err localizedDescription ]);
            }

            emit connectionError(id, InterfaceLookupError);
            [autoreleasepool release];

        } else {
            // not wifi
        }
#endif
    }
    emit connectionError(id, OperationNotSupported);
}

void QCoreWlanEngine::disconnectFromId(const QString &id)
{
    QString interfaceString = getInterfaceFromId(id);
    if(networkInterfaces.value(getInterfaceFromId(id)) == "WLAN") { //wifi only for now
#if defined(MAC_SDK_10_6)
        NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
        CWInterface *wifiInterface = [CWInterface interfaceWithName:  qstringToNSString(interfaceString)];
        [wifiInterface disassociate];
        if([[wifiInterface interfaceState]intValue] != kCWInterfaceStateInactive) {
            emit connectionError(id, DisconnectionError);
        }
       [autoreleasepool release];
       return;
#endif
    } else {

    }
    emit connectionError(id, OperationNotSupported);
}

void QCoreWlanEngine::requestUpdate()
{
    getAllScInterfaces();
    getUserConfigurations();
    emit configurationsChanged();
}

QCoreWlanEngine *QCoreWlanEngine::instance()
{
    return coreWlanEngine();
}

QString QCoreWlanEngine::getSsidFromNetworkName(const QString &name)
{
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             if(name == i.key()) {
                 return ij.key();
             }
        }
    }
    return QString();
}

QString QCoreWlanEngine::getNetworkNameFromSsid(const QString &ssid)
{
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             if(ij.key() == ssid) {
                 return i.key();
             }

         }
            return map.key(ssid);
    }
    return QString();
}

QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::scanForSsids(const QString &interfaceName)
{
    QList<QNetworkConfigurationPrivate *> foundConfigs;
    if(!hasWifi) {
        return foundConfigs;
    }
#if defined(MAC_SDK_10_6)
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    CWInterface *currentInterface = [CWInterface interfaceWithName:qstringToNSString(interfaceName)];
    QStringList addedConfigs;

    if([currentInterface power]) {
        NSError *err = nil;
        NSDictionary *parametersDict = nil;
        NSArray* apArray = [currentInterface scanForNetworksWithParameters:parametersDict error:&err];

        CWNetwork *apNetwork;
        if(!err) {
            for(uint row=0; row < [apArray count]; row++ ) {
                apNetwork = [apArray objectAtIndex:row];

                QString networkSsid = nsstringToQString([apNetwork ssid]);

                QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
                cpPriv->name = networkSsid;
                cpPriv->isValid = true;
                cpPriv->id = networkSsid;
                cpPriv->internet = true;
                cpPriv->bearer = QLatin1String("WLAN");
                cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
                cpPriv->serviceInterface = QNetworkInterface::interfaceFromName(interfaceName);
                bool known = isKnownSsid(networkSsid);
                if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                    if( cpPriv->name == nsstringToQString( [currentInterface ssid])) {
                        cpPriv->state |=  QNetworkConfiguration::Active;
                    }
                }

                if(!cpPriv->state) {
                    if(known) {
                        cpPriv->state =  QNetworkConfiguration::Discovered;
                    } else {
                        cpPriv->state = QNetworkConfiguration::Undefined;
                    }
                }
                if([[apNetwork securityMode ] intValue]== kCWSecurityModeOpen)
                    cpPriv->purpose = QNetworkConfiguration::PublicPurpose;
                else
                    cpPriv->purpose = QNetworkConfiguration::PrivatePurpose;

                foundConfigs.append(cpPriv);
                addedConfigs << networkSsid;

            } //end scanned row
        }
    } //end power


    // add known configurations that are not around.
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QString networkName = i.key();

        if(!addedConfigs.contains(networkName)) {
            QString interfaceName;
            QMapIterator<QString, QString> ij(i.value());
             while (ij.hasNext()) {
                 ij.next();
                 interfaceName = ij.value();
             }

            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            cpPriv->name = networkName;
            cpPriv->isValid = true;
            cpPriv->id = networkName;
            cpPriv->internet = true;
            cpPriv->bearer = QLatin1String("WLAN");
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            cpPriv->serviceInterface = QNetworkInterface::interfaceFromName(interfaceName);
            QString ssid = getSsidFromNetworkName(networkName);
            if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                if( ssid == nsstringToQString( [currentInterface ssid])) {
                    cpPriv->state |=  QNetworkConfiguration::Active;
                }
            }

            if( addedConfigs.contains(ssid)) {
                cpPriv->state |=  QNetworkConfiguration::Discovered;
            }

            if(!cpPriv->state) {
                cpPriv->state =  QNetworkConfiguration::Defined;
            }

            foundConfigs.append(cpPriv);
        }
    }

    [autoreleasepool drain];
#else
    Q_UNUSED(interfaceName);
#endif
    return foundConfigs;
}

bool QCoreWlanEngine::isWifiReady(const QString &wifiDeviceName)
{
#if defined(MAC_SDK_10_6)
    if([[CWInterface supportedInterfaces] count] > 0 ) {
        CWInterface *defaultInterface = [CWInterface interfaceWithName: qstringToNSString(wifiDeviceName)];
        if([defaultInterface power])
            return true;
    }
#else
    Q_UNUSED(wifiDeviceName);
#endif
    return false;
}

bool QCoreWlanEngine::isKnownSsid( const QString &ssid)
{
#if defined(MAC_SDK_10_6)
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        if(map.keys().contains(ssid)) {
            return true;
        }
    }
#else
    Q_UNUSED(ssid);
#endif
    return false;
}

bool QCoreWlanEngine::getAllScInterfaces()
{
    networkInterfaces.clear();
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];

    CFArrayRef interfaces = SCNetworkInterfaceCopyAll();
    if (interfaces != NULL) {
        CFIndex interfaceCount;
        CFIndex interfaceIndex;
        interfaceCount = CFArrayGetCount(interfaces);
        for (interfaceIndex = 0; interfaceIndex < interfaceCount; interfaceIndex++) {
            NSAutoreleasePool *looppool = [[NSAutoreleasePool alloc] init];

            CFStringRef bsdName;
            CFTypeRef thisInterface = CFArrayGetValueAtIndex(interfaces, interfaceIndex);
            bsdName = SCNetworkInterfaceGetBSDName((SCNetworkInterfaceRef)thisInterface);
            QString interfaceName = cfstringRefToQstring(bsdName);
            QString typeStr;
            CFStringRef type = SCNetworkInterfaceGetInterfaceType((SCNetworkInterfaceRef)thisInterface);
            if ( CFEqual(type, kSCNetworkInterfaceTypeIEEE80211)) {
                typeStr = "WLAN";
//            } else if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth)) {
//                typeStr = "Bluetooth";
            } else if(CFEqual(type, kSCNetworkInterfaceTypeEthernet)) {
                typeStr = "Ethernet";
            } else if(CFEqual(type, kSCNetworkInterfaceTypeFireWire)) {
                typeStr = "Ethernet"; //ok a bit fudged
            }
            if(!networkInterfaces.contains(interfaceName) && !typeStr.isEmpty()) {
                networkInterfaces.insert(interfaceName,typeStr);
            }
            [looppool release];
        }
    }
    CFRelease(interfaces);

    [autoreleasepool drain];
    return true;
}

void QCoreWlanEngine::startNetworkChangeLoop()
{
    storeSession = NULL;

    SCDynamicStoreContext dynStoreContext = { 0, this/*(void *)storeSession*/, NULL, NULL, NULL };
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
        CFRelease(storeSession );
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    return;
}

void QCoreWlanEngine::getUserConfigurations()
{
#ifdef MAC_SDK_10_6
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    userProfiles.clear();

    NSArray *wifiInterfaces = [CWInterface supportedInterfaces];
    for(uint row=0; row < [wifiInterfaces count]; row++ ) {

        CWInterface *wifiInterface = [CWInterface interfaceWithName: [wifiInterfaces objectAtIndex:row]];
        NSString *nsInterfaceName = [wifiInterface name];
// add user configured system networks
        SCDynamicStoreRef dynRef = SCDynamicStoreCreate(kCFAllocatorSystemDefault, (CFStringRef)@"Qt corewlan", nil, nil);
        NSDictionary * airportPlist = (NSDictionary *)SCDynamicStoreCopyValue(dynRef, (CFStringRef)[NSString stringWithFormat:@"Setup:/Network/Interface/%@/AirPort", nsInterfaceName]);
        CFRelease(dynRef);

        NSDictionary *prefNetDict = [airportPlist objectForKey:@"PreferredNetworks"];

        NSArray *thisSsidarray = [prefNetDict valueForKey:@"SSID_STR"];
        for(NSString *ssidkey in thisSsidarray) {
            QString thisSsid = nsstringToQString(ssidkey);
            if(!userProfiles.contains(thisSsid)) {
                QMap <QString,QString> map;
                map.insert(thisSsid, nsstringToQString(nsInterfaceName));
                userProfiles.insert(thisSsid, map);
            }
        }
        CFRelease(airportPlist);

        // 802.1X user profiles
        QString userProfilePath = QDir::homePath() + "/Library/Preferences/com.apple.eap.profiles.plist";
        NSDictionary* eapDict = [[NSMutableDictionary alloc] initWithContentsOfFile:qstringToNSString(userProfilePath)];
        NSString *profileStr= @"Profiles";
        NSString *nameStr = @"UserDefinedName";
        NSString *networkSsidStr = @"Wireless Network";
        for (id profileKey in eapDict) {
            if ([profileStr isEqualToString:profileKey]) {
                NSDictionary *itemDict = [eapDict objectForKey:profileKey];
                for (id itemKey in itemDict) {

                    NSInteger dictSize = [itemKey count];
                    id objects[dictSize];
                    id keys[dictSize];

                    [itemKey getObjects:objects andKeys:keys];
                    QString networkName;
                    QString ssid;
                    for(int i = 0; i < dictSize; i++) {
                        if([nameStr isEqualToString:keys[i]]) {
                            networkName = nsstringToQString(objects[i]);
                        }
                        if([networkSsidStr isEqualToString:keys[i]]) {
                            ssid = nsstringToQString(objects[i]);
                        }
                        if(!userProfiles.contains(networkName)
                            && !ssid.isEmpty()) {
                            QMap<QString,QString> map;
                            map.insert(ssid, nsstringToQString(nsInterfaceName));
                            userProfiles.insert(networkName, map);
                        }
                    }
                }
            }
        }
    }
    [autoreleasepool release];
#endif

}

#include "moc_qcorewlanengine_mac_p.cpp"

QTM_END_NAMESPACE
