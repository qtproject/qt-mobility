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
#include "qsysteminfo_s60_p.h"

#include <QStringList>

#include <SysUtil.h>
#include <e32const.h> 
#include <etel3rdparty.h>
#include <f32file.h>
#include <FeatDiscovery.h>
#include <e32property.h>

#include <centralrepository.h>  
#include <CenRepNotifyHandler.h>
#include <btserversdkcrkeys.h>

#include <HWRMPowerStateSDKPSKeys.h>

//////// QSystemInfo
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
    //return lang;
    return QString();
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    QString errorStr = "Not Available";
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

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    return QSystemNetworkInfo::UndefinedStatus;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    return 1;
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

QString QSystemNetworkInfoPrivate::networkName()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    return QString();
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    return QNetworkInterface();
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
    return -1;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    return -1;
}

//////// QSystemMemoryInfo
QSystemMemoryInfoPrivate::QSystemMemoryInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemMemoryInfoPrivate::~QSystemMemoryInfoPrivate()
{
}

qint64 QSystemMemoryInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    return 0;
}

qint64 QSystemMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    return 0;
}

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::volumeType(const QString &driveVolume)
{
    return QSystemMemoryInfo::NoVolume;
}

QStringList QSystemMemoryInfoPrivate::listOfVolumes()
{
    QStringList drivesList;
    return drivesList << QString("C");
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
    TRAP(iError,
        iDeviceInfo = CDeviceInfo::NewL();
    )    
    TRAP(iError,
        iProfileEngine = ProEngFactory::NewEngineL();
    )
    TRAP(iError,
        if (!iBluetoothMonitor) {
            iBluetoothMonitor = CBluetoothMonitor::NewL(*this);
        }
    )
    TRAP(iError,
        if (!iProfileMonitor) {
            iProfileMonitor = CProfileMonitor::NewL(*this);
        }
    )
    TRAP(iError,
        iBatteryStatusObserver = 
            CBatteryStateObserver::NewL(*this, KPSUidHWRMPowerState, KHWRMBatteryStatus);
        iBatteryLevelObserver = 
            CBatteryStateObserver::NewL(*this, KPSUidHWRMPowerState, KHWRMBatteryLevel);
        iChargingStatusObserver = 
            CBatteryStateObserver::NewL(*this, KPSUidHWRMPowerState, KHWRMChargingStatus);
    )
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    if(iBatteryStatusObserver) {
        delete iBatteryStatusObserver;
    }
    if(iBatteryLevelObserver) {
        delete iBatteryLevelObserver;
    }
    if(iChargingStatusObserver) {
        delete iChargingStatusObserver;
    }
    if(iProfileEngine) {
        iProfileEngine->Release();
    }
    if(iBluetoothMonitor) {
        delete iBluetoothMonitor;
    }
    if(iProfileMonitor) {
        delete iProfileMonitor;
    }
    if(iProfileEngine) {
        delete iDeviceInfo;
    }
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    QSystemDeviceInfo::Profile currentProfile = QSystemDeviceInfo::UnknownProfile;
    if(iProfileEngine) {
        MProEngProfile* activeProfile = NULL;
        TRAP(iError, activeProfile = iProfileEngine->ActiveProfileL();)
        
        if (iError == KErrNone) {
            currentProfile = static_cast<QSystemDeviceInfo::Profile>(activeProfile->ProfileName().Id());
            activeProfile->Release();
        }
    }
    return currentProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;
    return methods;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    return iDeviceInfo->currentPowerState();
}

QString QSystemDeviceInfoPrivate::imei()
{
    CDeviceInfo* deviceInfo = NULL;
    TRAPD(error,
        deviceInfo = CDeviceInfo::NewL();
    )
    if (deviceInfo->imei().length() > 0) {
        return deviceInfo->imei();
    }
    else {
        return QString();
    }
}

QString QSystemDeviceInfoPrivate::imsi()
{
    CDeviceInfo* deviceInfo = NULL;
    TRAPD(error,
        deviceInfo = CDeviceInfo::NewL();
    )
    if (deviceInfo->imei().length() > 0) {
        return deviceInfo->imei();
    }
    else {
        return QString();
    }
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    CDeviceInfo* deviceInfo = NULL;
    TRAPD(error,
        deviceInfo = CDeviceInfo::NewL();
    )
    return deviceInfo->manufacturer();
}

QString QSystemDeviceInfoPrivate::model()
{
    CDeviceInfo* deviceInfo = NULL;
    TRAPD(error,
        deviceInfo = CDeviceInfo::NewL();
    )
    return deviceInfo->model();
}

QString QSystemDeviceInfoPrivate::productName()
{
    QString productname;
    TBuf<KSysUtilVersionTextLength> versionBuf;
    if (SysUtil::GetSWVersion(versionBuf) == KErrNone) {
		productname = QString::fromUtf16(versionBuf.Ptr(), versionBuf.Length());
    }
    return productname.split("\n").at(2);
}

bool QSystemDeviceInfoPrivate::isBatteryCharging()
{
    return iDeviceInfo->isBatteryCharging();
}

int QSystemDeviceInfoPrivate::batteryLevel() const
{
    return iDeviceInfo->batteryLevel();
}

QSystemDeviceInfo::BatteryStatus QSystemDeviceInfoPrivate::batteryStatus()
{
    int batteryLevel = iDeviceInfo->batteryLevel();
    if(batteryLevel < 4) {
        return QSystemDeviceInfo::BatteryCritical;
    }   else if(batteryLevel < 11) {
        return QSystemDeviceInfo::BatteryVeryLow;
    }  else if(batteryLevel < 41) {
        return QSystemDeviceInfo::BatteryLow;
    }   else if(batteryLevel > 40) {
        return QSystemDeviceInfo::BatteryNormal;
    }

    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

//////// QSystemScreenSaver
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
{
}

bool QSystemScreenSaverPrivate::setScreenSaverEnabled(bool state)
{
    return false;
}

bool QSystemScreenSaverPrivate::setScreenBlankingEnabled(bool state)
{
    return false;
}

bool QSystemScreenSaverPrivate::screenSaverEnabled()
{
    return false;
}

bool QSystemScreenSaverPrivate::screenBlankingEnabled()
{
    return false;
}

bool QSystemScreenSaverPrivate::isScreenLockOn()
{
    return false;
}

//////// CDeviceInfo for getting S60 data
CDeviceInfo* CDeviceInfo::NewL()
{
    CDeviceInfo* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}
    
CDeviceInfo* CDeviceInfo::NewLC()
{
    CDeviceInfo* self = new (ELeave) CDeviceInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CDeviceInfo::ConstructL()
{   
    CActiveScheduler::Add(this);
    iTelephony = CTelephony::NewL();  
    iWait = new (ELeave) CActiveSchedulerWait();
}

void CDeviceInfo::DoCancel()
{
    iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
}
 
void CDeviceInfo::RunL()
{
    iWait->AsyncStop();
}

CDeviceInfo::CDeviceInfo()
:CActive(EPriorityNormal), iBatteryInfoV1Pkg(iBatteryInfoV1),
    iPhoneIdV1Pkg(iPhoneIdV1),
    iSignalStrengthV1Pckg(iSignalStrengthV1),
    iSubscriberIdV1Pckg(iSubscriberIdV1),
    iNetworkRegistrationV1Pckg(iNetworkRegistrationV1),
    iNetworkInfoV1Pckg(iNetworkInfoV1),
    iIndicatorV1Pckg(iIndicatorV1)
{
}

CDeviceInfo::~CDeviceInfo()
{
    Cancel();
    delete iTelephony;  
}

QSystemDeviceInfo::PowerState CDeviceInfo::currentPowerState()
{
    Cancel();
    iTelephony->GetBatteryInfo(iStatus,iBatteryInfoV1Pkg);
    SetActive();

    if (!iWait->IsStarted()) {
        iWait->Start();
    }

    if (iBatteryInfoV1.iStatus == CTelephony::EPowerStatusUnknown) {
        return QSystemDeviceInfo::UnknownPower;
    } else if (iBatteryInfoV1.iStatus == CTelephony::EPoweredByBattery) {
        return QSystemDeviceInfo::BatteryPower;
    } else if (iBatteryInfoV1.iStatus == CTelephony::EBatteryConnectedButExternallyPowered) {
        return QSystemDeviceInfo::WallPower;
    } else {
        return QSystemDeviceInfo::UnknownPower;
    }
}

QString CDeviceInfo::imei()
{
    Cancel();
    iTelephony->GetPhoneId(iStatus, iPhoneIdV1Pkg);
    SetActive();
    
    if (!iWait->IsStarted()) {
        iWait->Start();
    }
    TBuf<CTelephony::KPhoneSerialNumberSize> imei = iPhoneIdV1.iSerialNumber;
    return QString::fromUtf16(imei.Ptr(), imei.Length());
}

QString CDeviceInfo::imsi()
{
    Cancel();
    iTelephony->GetSubscriberId(iStatus,iSubscriberIdV1Pckg);
    SetActive();
    
    if (!iWait->IsStarted()) {
        iWait->Start();
    }
    TBuf<CTelephony::KIMSISize> imsi = iSubscriberIdV1.iSubscriberId;
    return QString::fromUtf16(imsi.Ptr(), imsi.Length());
}

QString CDeviceInfo::manufacturer()
{
    Cancel();
    iTelephony->GetPhoneId(iStatus, iPhoneIdV1Pkg);
    SetActive();
    
    if (!iWait->IsStarted()) {
        iWait->Start();
    }
    TBuf<CTelephony::KPhoneManufacturerIdSize> manufacturer = iPhoneIdV1.iManufacturer;
    return QString::fromUtf16(manufacturer.Ptr(), manufacturer.Length());
}

QString CDeviceInfo::model()
{
    Cancel();
    iTelephony->GetPhoneId(iStatus, iPhoneIdV1Pkg);
    SetActive();
    
    if (!iWait->IsStarted()) {
        iWait->Start();
    }
    TBuf<CTelephony::KPhoneModelIdSize> model = iPhoneIdV1.iModel;
    return QString::fromUtf16(model.Ptr(), model.Length());
}

bool CDeviceInfo::isBatteryCharging()
{
    bool chargeStatus = false;
    
    Cancel();
    iTelephony->GetIndicator(iStatus,iIndicatorV1Pckg);
    SetActive();
    if ( !iWait->IsStarted() ) {
        iWait->Start();
    }
    
    if (iIndicatorV1.iIndicator & CTelephony::KIndChargerConnected) {
        chargeStatus = true;
    }
    return chargeStatus;
}

TUint CDeviceInfo::batteryLevel()
{
    Cancel();
    iTelephony->GetBatteryInfo(iStatus,iBatteryInfoV1Pkg);
    SetActive();
    
    if (!iWait->IsStarted()) {
        iWait->Start();
    }
    return iBatteryInfoV1.iChargeLevel;
}

//////// For monitoring bluetooth state
CBluetoothMonitor::CBluetoothMonitor(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate)
    : iSystemDeviceInfoPrivate(aSystemDeviceInfoPrivate)
{
}

CBluetoothMonitor::~CBluetoothMonitor()
{
    delete iNotifyHandler;
    delete iSession;
}

CBluetoothMonitor* CBluetoothMonitor::NewL(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate)
{
    CBluetoothMonitor* self = CBluetoothMonitor::NewLC(aSystemDeviceInfoPrivate);
    CleanupStack::Pop(self);
    return self;
}
 
CBluetoothMonitor* CBluetoothMonitor::NewLC(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate)
{
    CBluetoothMonitor* self = new (ELeave) CBluetoothMonitor(aSystemDeviceInfoPrivate);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}
 
void CBluetoothMonitor::ConstructL()
{
    iSession = CRepository::NewL(KCRUidBluetoothPowerState);
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession,
        CCenRepNotifyHandler::EIntKey, KBTPowerState);
    iNotifyHandler->StartListeningL();
}


void CBluetoothMonitor::HandleNotifyInt(TUint32 aId, TInt aNewValue)
{
    if (aId == KBTPowerState  && aNewValue & 0x00000001) {
        emit iSystemDeviceInfoPrivate.bluetoothStateChanged(true);
    } else {
        emit iSystemDeviceInfoPrivate.bluetoothStateChanged(false);
    }
}

//////// For monitoring current profile
CProfileMonitor::CProfileMonitor(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate)
    : iSystemDeviceInfoPrivate(aSystemDeviceInfoPrivate)
{
}

CProfileMonitor::~CProfileMonitor()
{
    if(iProfileEngine) {
        iProfileEngine->Release();
    }
    delete iNotifyHandler;
    delete iSession;
}

CProfileMonitor* CProfileMonitor::NewL(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate)
{
    CProfileMonitor* self = CProfileMonitor::NewLC(aSystemDeviceInfoPrivate);
    CleanupStack::Pop(self);
    return self;
}
 
CProfileMonitor* CProfileMonitor::NewLC(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate)
{
    CProfileMonitor* self = new (ELeave) CProfileMonitor(aSystemDeviceInfoPrivate);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}
 
void CProfileMonitor::ConstructL()
{
    iProfileEngine = ProEngFactory::NewEngineL();
    iSession = CRepository::NewL(KCRUidProfileEngine);
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession,
        CCenRepNotifyHandler::EIntKey, KProEngActiveProfile);
    iNotifyHandler->StartListeningL();
}


void CProfileMonitor::HandleNotifyInt(TUint32 aId, TInt aNewValue)
{
    QSystemDeviceInfo::Profile currentProfile = QSystemDeviceInfo::UnknownProfile;
    if(iProfileEngine) {
        MProEngProfile* activeProfile = NULL;
        TRAP(iError, activeProfile = iProfileEngine->ActiveProfileL();)
        
        if (iError == KErrNone) {
            currentProfile = static_cast<QSystemDeviceInfo::Profile>
                (activeProfile->ProfileName().Id());
            activeProfile->Release();
        }
    }
    emit iSystemDeviceInfoPrivate.currentProfileChanged(currentProfile);
}

//////// For monitoring battery state
CBatteryStateObserver::CBatteryStateObserver(QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate,
    const TUid aUid, const TUint32 aKey) : CActive(EPriorityNormal), iSystemDeviceInfoPrivate(aSystemDeviceInfoPrivate),
    iUid( aUid ), iKey( aKey )
{
}

CBatteryStateObserver* CBatteryStateObserver::NewL(
    QSystemDeviceInfoPrivate& aSystemDeviceInfoPrivate, const TUid aUid, const TUint32 aKey)
{
    CBatteryStateObserver* self = new(ELeave) CBatteryStateObserver(aSystemDeviceInfoPrivate, aUid, aKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CBatteryStateObserver::ConstructL()
{
    User::LeaveIfError(iProperty.Attach(iUid, iKey));

    CActiveScheduler::Add(this);
    // initial subscription and process current property value
    RunL();
}

CBatteryStateObserver::~CBatteryStateObserver()
{
    Cancel();
    iProperty.Close();
}

void CBatteryStateObserver::DoCancel()
{
    iProperty.Cancel();
}

void CBatteryStateObserver::RunL()
{
    //resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
 
    switch(iKey) {
        case KHWRMBatteryLevel: {
            emit iSystemDeviceInfoPrivate.batteryLevelChanged(iSystemDeviceInfoPrivate.batteryLevel());
            
            int level = iSystemDeviceInfoPrivate.batteryLevel();
            if(level < 4) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryCritical) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryCritical);
                }
            } else if(level < 11) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryVeryLow) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryVeryLow);
                }
            } else if(level < 41) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryLow) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryLow);
                }
            } else if(level > 40) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryNormal) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryNormal);
                }
            }
            break;
        }
        case KHWRMBatteryStatus: {
            int level = iSystemDeviceInfoPrivate.batteryLevel();
            if(level < 4) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryCritical) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryCritical);
                }
            } else if(level < 11) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryVeryLow) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryVeryLow);
                }
            } else if(level < 41) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryLow) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryLow);
                }
            } else if(level > 40) {
                if (iSystemDeviceInfoPrivate.batteryStatus() != QSystemDeviceInfo::BatteryNormal) {
                    emit iSystemDeviceInfoPrivate.batteryStatusChanged(QSystemDeviceInfo::BatteryNormal);
                }
            }
            break;
        }
        case KHWRMChargingStatus: {
            if (iSystemDeviceInfoPrivate.isBatteryCharging()) {
                emit iSystemDeviceInfoPrivate.powerStateChanged(QSystemDeviceInfo::WallPower);
            } else if (!iSystemDeviceInfoPrivate.isBatteryCharging()) {
                emit iSystemDeviceInfoPrivate.powerStateChanged(QSystemDeviceInfo::BatteryPower);
            } else {
                emit iSystemDeviceInfoPrivate.powerStateChanged(QSystemDeviceInfo::UnknownPower);
            }
            break;
        }
        default:
            break;
    }
}

QT_END_NAMESPACE
