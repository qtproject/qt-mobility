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
#include <QThread>
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
    currentInterface = [CWInterface interfaceWithName:nil];
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

QNSListener *listener = 0;

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

QScanThread::QScanThread(QObject *parent)
    :QThread(parent), interfaceName(nil)
{
}

QScanThread::~QScanThread()
{
}

void QScanThread::quit()
{
    wait();
}

void QScanThread::run()
{
    getUserProfiles();
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    QStringList found;
    mutex.lock();
    CWInterface *currentInterface;
    if(interfaceName.isEmpty()) {
        currentInterface = [CWInterface interfaceWithName:nil];
        interfaceName = nsstringToQString([currentInterface name]);
    } else {
        currentInterface = [CWInterface interfaceWithName:qstringToNSString(interfaceName)];
    }
    mutex.unlock();

    if([currentInterface power]) {
        NSError *err = nil;
        NSDictionary *parametersDict =  [NSDictionary dictionaryWithObjectsAndKeys:
                                         [NSNumber numberWithBool:YES], kCWScanKeyMerge,
                                         [NSNumber numberWithInteger:100], kCWScanKeyRestTime, nil];

        NSArray* apArray = [currentInterface scanForNetworksWithParameters:parametersDict error:&err];
        CWNetwork *apNetwork;

        if (!err) {
            for(uint row=0; row < [apArray count]; row++ ) {
                apNetwork = [apArray objectAtIndex:row];

                const QString networkSsid = nsstringToQString([apNetwork ssid]);
                found.append(networkSsid);

                QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;

                bool known = isKnownSsid(networkSsid);
                if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                    if( networkSsid == nsstringToQString( [currentInterface ssid])) {
                        state = QNetworkConfiguration::Active;
                    }
                }
                if(state == QNetworkConfiguration::Undefined) {
                    if(known) {
                        state = QNetworkConfiguration::Discovered;
                    } else {
                        state = QNetworkConfiguration::Undefined;
                    }
                }

                QNetworkConfiguration::Purpose purpose = QNetworkConfiguration::UnknownPurpose;
                if([[apNetwork securityMode] intValue] == kCWSecurityModeOpen) {
                    purpose = QNetworkConfiguration::PublicPurpose;
                } else {
                    purpose = QNetworkConfiguration::PrivatePurpose;
                }

                found.append(foundNetwork(networkSsid, networkSsid, state, interfaceName, purpose));

            } //end row
        }//end error
    }// endwifi power

// add known configurations that are not around.
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();

        QString networkName = i.key();
        const QString id = networkName;

        if(!found.contains(id)) {
            QString networkSsid = getSsidFromNetworkName(networkName);
            const QString ssidId = QString::number(qHash(QLatin1String("corewlan:") + networkSsid));
            QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;
            QString interfaceName;
            QMapIterator<QString, QString> ij(i.value());
            while (ij.hasNext()) {
                ij.next();
                interfaceName = ij.value();
            }

            if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                if( networkSsid == nsstringToQString([currentInterface ssid])) {
                    state = QNetworkConfiguration::Active;
                }
            }
            if(state == QNetworkConfiguration::Undefined) {
                if( userProfiles.contains(networkName)
                    && found.contains(ssidId)) {
                    state = QNetworkConfiguration::Discovered;
                }
            }

            if(state == QNetworkConfiguration::Undefined) {
                state = QNetworkConfiguration::Defined;
            }

            found.append(foundNetwork(id, networkName, state, interfaceName, QNetworkConfiguration::UnknownPurpose));
        }
    }
    emit networksChanged();
    [pool release];
}

QStringList QScanThread::foundNetwork(const QString &id, const QString &name, const QNetworkConfiguration::StateFlags state, const QString &interfaceName, const QNetworkConfiguration::Purpose purpose)
{
    QStringList found;
    QMutexLocker locker(&mutex);
    QNetworkConfigurationPrivate *ptr = new QNetworkConfigurationPrivate;

    ptr->name = name;
    ptr->isValid = true;
    ptr->id = id;
    ptr->state = state;
    ptr->type = QNetworkConfiguration::InternetAccessPoint;
    ptr->bearer = QLatin1String("WLAN");
    ptr->purpose = purpose;
    ptr->internet = true;
    ptr->serviceInterface = QNetworkInterface::interfaceFromName(interfaceName);

    fetchedConfigurations.append( ptr);
    configurationInterface[name] =  interfaceName;

    locker.unlock();
    locker.relock();
    found.append(id);
    return found;
}

QList<QNetworkConfigurationPrivate *> QScanThread::getConfigurations()
{
    QMutexLocker locker(&mutex);

    QList<QNetworkConfigurationPrivate *> foundConfigurations;

    for (int i = 0; i < fetchedConfigurations.count(); ++i) {
        QNetworkConfigurationPrivate *config = new QNetworkConfigurationPrivate;
        config->name = fetchedConfigurations.at(i)->name;
        config->isValid = fetchedConfigurations.at(i)->isValid;
        config->id = fetchedConfigurations.at(i)->id;
        config->state = fetchedConfigurations.at(i)->state;

        config->type = fetchedConfigurations.at(i)->type;
        config->roamingSupported = fetchedConfigurations.at(i)->roamingSupported;
        config->purpose = fetchedConfigurations.at(i)->purpose;
        config->internet = fetchedConfigurations.at(i)->internet;
        foundConfigurations.append(config);
    }

    return foundConfigurations;
}

void QScanThread::getUserProfiles()
{
    QMutexLocker locker(&mutex);
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
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
        NSEnumerator *ssidEnumerator = [thisSsidarray objectEnumerator];
        NSString *ssidkey;
        while ((ssidkey = [ssidEnumerator nextObject])) {
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
        NSDictionary* eapDict = [[NSDictionary alloc] initWithContentsOfFile:qstringToNSString(userProfilePath)];
        NSString *profileStr= @"Profiles";
        NSString *nameStr = @"UserDefinedName";
        NSString *networkSsidStr = @"Wireless Network";

        id profileKey;
        NSEnumerator *dictEnumerator = [eapDict objectEnumerator];
        while ((profileKey = [dictEnumerator nextObject])) {

            if ([profileStr isEqualToString:profileKey]) {
                NSDictionary *itemDict = [eapDict objectForKey:profileKey];
                id itemKey;
                NSEnumerator *dictEnumerator = [thisSsidarray objectEnumerator];
                while ((itemKey = [dictEnumerator nextObject])) {

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
                [itemDict release];
            }
        }
    }

    [pool release];
}

QString QScanThread::getSsidFromNetworkName(const QString &name)
{
    QMutexLocker locker(&mutex);
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             const QString networkNameHash = QString::number(qHash(QLatin1String("corewlan:") +i.key()));
             if(name == i.key() || name == networkNameHash) {
                 return ij.key();
             }
        }
    }
    return QString();
}

QString QScanThread::getNetworkNameFromSsid(const QString &ssid)
{
    QMutexLocker locker(&mutex);
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
    }
    return QString();
}

bool QScanThread::isKnownSsid(const QString &ssid)
{
    QMutexLocker locker(&mutex);

    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        if(map.keys().contains(ssid)) {
            return true;
        }
    }
    return false;
}


QCoreWlanEngine::QCoreWlanEngine(QObject *parent)
:   QNetworkSessionEngine(parent)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    getAllScInterfaces();
    scanThread = new QScanThread(this);
    connect(scanThread, SIGNAL(networksChanged()),
            this, SIGNAL(configurationsChanged()));

    QTimer::singleShot(0,this,SLOT(init()));
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

void QCoreWlanEngine::init()
{
#ifdef MAC_SDK_10_6
    if([[CWInterface supportedInterfaces] count] > 0 && !listener) {
        listener = [[QNSListener alloc] init];
        hasWifi = true;
    } else {
        hasWifi = false;
    }
#endif
    storeSession = NULL;
    scanThread->start();

    startNetworkChangeLoop();
}


QList<QNetworkConfigurationPrivate *> QCoreWlanEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = true;
    foundConfigurations.clear();

    QNetworkConfigurationPrivate* cpPriv = 0;
    QMutexLocker locker(&mutex);
     QList<QNetworkConfigurationPrivate *> fetchedConfigurations = scanThread->getConfigurations();
locker.unlock();

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

         foundConfigurations.append(config);
         delete cpPriv;
     }


     return foundConfigurations;
}

QString QCoreWlanEngine::getInterfaceFromId(const QString &id)
{
    return scanThread->configurationInterface.value(id);
}

bool QCoreWlanEngine::hasIdentifier(const QString &id)
{
    return scanThread->configurationInterface.contains(id);
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

            QString wantedSsid = 0;
            bool using8021X = false;

            if(scanThread->getNetworkNameFromSsid(id) != id) {
                NSArray *array = [CW8021XProfile allUser8021XProfiles];
                for (NSUInteger i=0; i<[array count]; ++i) {

                    if(id == nsstringToQString([[array objectAtIndex:i] userDefinedName])
                        || id == nsstringToQString([[array objectAtIndex:i] ssid]) ) {
                        QString thisName = scanThread->getSsidFromNetworkName(id);
                        if(thisName.isEmpty()) {
                            wantedSsid = id;
                        } else {
                            wantedSsid = thisName;
                        }
                        [params setValue: [array objectAtIndex:i] forKey:kCWAssocKey8021XProfile];
                        using8021X = true;
                        break;
                    }
                }
            }

            if(!using8021X) {
                QString wantedNetwork;
                QMapIterator<QString, QMap<QString,QString> > i(scanThread->userProfiles);
                while (i.hasNext()) {
                    i.next();
                    wantedNetwork = i.key();
                    if(id == wantedNetwork) {
                        wantedSsid = scanThread->getSsidFromNetworkName(wantedNetwork);
                        break;
                    }
                }
            }

            NSDictionary *parametersDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                       [NSNumber numberWithBool:YES], kCWScanKeyMerge,
                                       [NSNumber numberWithInt:kCWScanTypeFast], kCWScanKeyScanType,
                                       [NSNumber numberWithInteger:100], kCWScanKeyRestTime,
                                       qstringToNSString(wantedSsid), kCWScanKeySSID,
                                       nil];

            NSArray *scanArray = [wifiInterface scanForNetworksWithParameters:parametersDict error:&err];
            if(!err) {
                for(uint row=0; row < [scanArray count]; row++ ) {
                    CWNetwork *apNetwork = [scanArray objectAtIndex:row];
                    if(wantedSsid == nsstringToQString([apNetwork ssid])) {

                        if(!using8021X) {
                            SecKeychainAttribute attributes[3];

                            NSString *account = [apNetwork ssid];
                            NSString *keyKind = @"AirPort network password";
                            NSString *keyName = account;

                            attributes[0].tag = kSecAccountItemAttr;
                            attributes[0].data = (void *)[account UTF8String];
                            attributes[0].length = [account length];

                            attributes[1].tag = kSecDescriptionItemAttr;
                            attributes[1].data = (void *)[keyKind UTF8String];
                            attributes[1].length = [keyKind length];

                            attributes[2].tag = kSecLabelItemAttr;
                            attributes[2].data = (void *)[keyName UTF8String];
                            attributes[2].length = [keyName length];

                            SecKeychainAttributeList attributeList = {3,attributes};

                            SecKeychainSearchRef searchRef;
                            OSErr result = SecKeychainSearchCreateFromAttributes(NULL, kSecGenericPasswordItemClass, &attributeList, &searchRef);
Q_UNUSED(result);
                            NSString *password = @"";
                            SecKeychainItemRef searchItem;
                            OSStatus resultStatus;
                            resultStatus = SecKeychainSearchCopyNext(searchRef, &searchItem);

                            if (resultStatus == errSecSuccess) {
                                UInt32 realPasswordLength;
                                SecKeychainAttribute attributesW[8];
                                attributesW[0].tag = kSecAccountItemAttr;
                                SecKeychainAttributeList listW = {1,attributesW};
                                char *realPassword;
                                OSStatus status = SecKeychainItemCopyContent(searchItem, NULL, &listW, &realPasswordLength,(void **)&realPassword);

                                if (status == noErr) {
                                    if (realPassword != NULL) {

                                        QByteArray pBuf;
                                        pBuf.resize(realPasswordLength);
                                        pBuf.prepend(realPassword);
                                        pBuf.insert(realPasswordLength,'\0');

                                        password = [NSString stringWithUTF8String:pBuf];
                                    }
                                }

                                CFRelease(searchItem);
                                SecKeychainItemFreeContent(&listW, realPassword);
                            } else {
                                qDebug() << "SecKeychainSearchCopyNext error" << cfstringRefToQstring(SecCopyErrorMessageString(resultStatus, NULL));
                            }
                            [params setValue: password forKey: kCWAssocKeyPassphrase];
                        } // end using8021X

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
            qDebug() << "wifi power off";
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
    scanThread->getUserProfiles();
    scanThread->start();
}

QCoreWlanEngine *QCoreWlanEngine::instance()
{
    return coreWlanEngine();
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
        qDebug() << "could not open dynamic store: error:" << SCErrorString(SCError());
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
        qDebug() << "register notification error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        CFRelease(notificationKeys);
        CFRelease(patternsArray);
        return;
    }
    CFRelease(notificationKeys);
    CFRelease(patternsArray);

    runloopSource = SCDynamicStoreCreateRunLoopSource(NULL, storeSession , 0);
    if (!runloopSource) {
        qDebug() << "runloop source error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    return;
}
#include "moc_qcorewlanengine_mac_p.cpp"

QTM_END_NAMESPACE
