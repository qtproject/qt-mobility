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
#include "qsysteminfo_s60_p.h"

#include <QStringList>
#include <QDir>
#include <QRegExp>
#include <QTimer>

#include <SysUtil.h>
#include <ptiengine.h>
#include <FeatDiscovery.h>
#include <featureinfo.h>
#include <hwrmvibra.h>
#include <AknUtils.h>
#include <W32STD.H>
#include <centralrepository.h>
 
//////// QSystemInfo
QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
    : QObject(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
}

QString QSystemInfoPrivate::currentLanguage() const
{
    return TLanguageToISO639_1(User::Language());
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    QStringList languages;
    TRAPD(err,
        CPtiEngine *ptiEngine = CPtiEngine::NewL();
        CleanupStack::PushL(ptiEngine);
        RArray<TInt> languageCodes;
        CleanupClosePushL(languageCodes);
        ptiEngine->GetAvailableLanguagesL(languageCodes);
        for (int i = 0; i < languageCodes.Count(); ++i) {
            QString language = TLanguageToISO639_1(TLanguage(languageCodes[i]));
            if (!language.isEmpty()) {
                languages << language;
            }
        }
        CleanupStack::PopAndDestroy(2, ptiEngine);
    )
    return languages;
}

QString QSystemInfoPrivate::TLanguageToISO639_1(TLanguage language) const
{
    switch (language) {
        case ELangAmerican: 
        case ELangCanadianEnglish:
        case ELangInternationalEnglish:
        case ELangSouthAfricanEnglish:
        case ELangAustralian:
        case ELangEnglish: return "en";
        case ELangSwissFrench:
        case ELangInternationalFrench:
        case ELangCanadianFrench:
        case ELangBelgianFrench:
        case ELangFrench: return "fr";
        case ELangSwissGerman:
        case ELangAustrian:
        case ELangGerman: return "de";
        case ELangInternationalSpanish:
        case ELangLatinAmericanSpanish:
        case ELangSpanish: return "es";
        case ELangSwissItalian:
        case ELangItalian: return "it";
        case ELangFinlandSwedish:
        case ELangSwedish: return "sv";
        case ELangDanish: return "da";
        case ELangNorwegian: return "no";
        case ELangFinnish: return "fi";
        case ELangBrazilianPortuguese:
        case ELangPortuguese: return "pt";
        case ELangCyprusTurkish:
        case ELangTurkish: return "tr";
        case ELangIcelandic: return "is";
        case ELangRussian: return "ru";
        case ELangHungarian: return "hu";
        case ELangBelgianFlemish:
        case ELangDutch: return "nl";
        case ELangNewZealand: return "mi";  //Maori
        case ELangCzech: return "cs";
        case ELangSlovak: return "sk";
        case ELangPolish: return "pl";
        case ELangSlovenian: return "sl";
        case ELangTaiwanChinese:
        case ELangHongKongChinese:
        case ELangPrcChinese: return "zh";
        case ELangJapanese: return "ja";
        case ELangThai: return "th";
        case ELangAfrikaans: return "af";
        case ELangAlbanian: return "sq";
        case ELangAmharic: return "am";
        case ELangArabic: return "ar";
        case ELangArmenian: return "hy";
        case ELangTagalog: return "tl";
        case ELangBelarussian: return "be";
        case ELangBengali:  return "bn";
        case ELangBulgarian: return "bg";
        case ELangBurmese: return "my";
        case ELangCatalan: return "ca";
        case ELangCroatian: return "hr";
        case ELangEstonian: return "et";
        case ELangFarsi: return "fa";
        case ELangScotsGaelic: "gd";
        case ELangGeorgian: return "ka";
        case ELangGreek:
        case ELangCyprusGreek: "el";
        case ELangGujarati: return "gu";
        case ELangHebrew: return "he";
        case ELangHindi: return "hi";
        case ELangIndonesian: return "id";
        case ELangIrish: return "ga";
        case ELangKannada: return "kn";
        case ELangKazakh: return "kk";
        case ELangKhmer: return "km";
        case ELangKorean: return "ko";
        case ELangLao: return "lo";
        case ELangLatvian: return "lv";
        case ELangLithuanian: return "lt";
        case ELangMacedonian: return "mk";
        case ELangMalay: return "ms";
        case ELangMalayalam: return "ml";
        case ELangMarathi: return "mr";
        case ELangMoldavian: return "ro";
        case ELangMongolian: return "mn";
        case ELangNorwegianNynorsk: return "nn";
        case ELangPunjabi: return "pa";
        case ELangRomanian: return "ro";
        case ELangSerbian: return "sr";
        case ELangSomali: return "so";
        case ELangSwahili: return "sw";
        case ELangTamil: return "ta";
        case ELangTelugu: return "te";
        case ELangTibetan: return "bo";
        case ELangTigrinya: return "ti";
        case ELangTurkmen: return "tk";
        case ELangUkrainian: return "uk";
        case ELangUrdu: return "ur";
        case ELangVietnamese: return "vi";
        case ELangWelsh: return "cy";
        case ELangZulu: return "zu";
        case ELangSinhalese: return "si";
        case ELangTest:
        case ELangReserved1:
        case ELangReserved2:
        case ELangOther:
        case ELangNone:
        default:
            break;
    }
    return "";
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    switch (type) {
        case QSystemInfo::Os:
        {
            return S60Version();
        }
        case QSystemInfo::QtCore:
        {
            return qVersion();
        }
        case QSystemInfo::Firmware:
        {
            QString versionText;
            TBuf<KSysUtilVersionTextLength> versionBuf;
            if (SysUtil::GetSWVersion(versionBuf) == KErrNone) {
                versionText = QString::fromUtf16(versionBuf.Ptr(), versionBuf.Length());
            }
            return versionText.split("\n").at(0);
        }
        case QSystemInfo::WrtCore:  //Not available
        case QSystemInfo::Webkit:   //Not available
        case QSystemInfo::ServiceFramework: //Not available
        case QSystemInfo::WrtExtensions:    //Not available
        case QSystemInfo::ServiceProvider:  //Not available
        case QSystemInfo::NetscapePlugin:   //Not available
        case QSystemInfo::WebApp:   //Not available
        default:
            return QString();
    }
}

QString QSystemInfoPrivate::S60Version() const
{
    QDir romInstallDir("z:\\system\\install\\");
    QStringList files = romInstallDir.entryList(QStringList("Series60v*.sis"), QDir::Files, QDir::Name | QDir::Reversed);
    if (files.size() > 0) {
        QRegExp rx("Series60v(.*).sis");
        if (rx.indexIn(files[0]) > -1) {
            return rx.cap(1);
        }
    }
    return QString();
}

QString QSystemInfoPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    TInt featureId = 0;
    switch (feature) {
        case QSystemInfo::BluetoothFeature: featureId = KFeatureIdBt; break;
        case QSystemInfo::CameraFeature: featureId = KFeatureIdCamera; break;
        case QSystemInfo::IrFeature: featureId = KFeatureIdIrda; break;
        case QSystemInfo::MemcardFeature: featureId = KFeatureIdMmc; break;
        case QSystemInfo::UsbFeature: featureId = KFeatureIdUsb; break;
        case QSystemInfo::WlanFeature: featureId = KFeatureIdProtocolWlan; break;
        case QSystemInfo::LocationFeature: featureId = KFeatureIdLocationFrameworkCore; break;
        case QSystemInfo::SimFeature:
        {
            return true;    //Always true in S60
        }
        case QSystemInfo::VibFeature:
        {
            TRAPD(err,
                //Leaves with KErrNotSupported if device doesn't support vibration feature.
                CHWRMVibra *vibra = CHWRMVibra::NewLC();
                CleanupStack::PopAndDestroy(vibra);
            )
            return err == KErrNone;
        }
        case QSystemInfo::HapticsFeature:
        {
            //TODO: Do something with the AVKON dependency
            return AknLayoutUtils::PenEnabled();
        }
        case QSystemInfo::FmradioFeature:   //Not available in public SDK
        case QSystemInfo::LedFeature:
        case QSystemInfo::VideoOutFeature:  //Accessory monitor available from S60 5.x onwards
        default:
            return false;
    }

    bool isFeatureSupported = false;
    TRAP_IGNORE(isFeatureSupported = CFeatureDiscovery::IsFeatureSupportedL(featureId);)
    return isFeatureSupported;
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
    : QObject(parent)
{
    DeviceInfo::instance()->cellSignalStrenghtInfo()->addObserver(this);
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
    DeviceInfo::instance()->cellSignalStrenghtInfo()->removeObserver(this);
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    //TODO: CDMA, WCDMA and WLAN modes
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        {
            CTelephony::TRegistrationStatus networkStatus = DeviceInfo::instance()
                ->cellNetworkRegistrationInfo()->cellNetworkStatus();

            switch(networkStatus) {
                case CTelephony::ERegistrationUnknown:
                    return QSystemNetworkInfo::UndefinedStatus;

                case CTelephony::ENotRegisteredNoService:
                    return QSystemNetworkInfo::NoNetworkAvailable;

                case CTelephony::ENotRegisteredEmergencyOnly:
                    return QSystemNetworkInfo::EmergencyOnly;

                case CTelephony::ENotRegisteredSearching:
                    return QSystemNetworkInfo::Searching;

                case CTelephony::ERegisteredBusy:
                    return QSystemNetworkInfo::Busy;
                /*
                case CTelephony::ERegisteredOnHomeNetwork: //How this case should be handled? There are no connected state in CTelephony. Should this be same as HomeNetwork?
                    return QSystemNetworkInfo::Connected;
                */
                case CTelephony::ERegisteredOnHomeNetwork:
                    return QSystemNetworkInfo::HomeNetwork;

                case CTelephony::ERegistrationDenied:
                    return QSystemNetworkInfo::Denied;

                case CTelephony::ERegisteredRoaming:
                    return QSystemNetworkInfo::Roaming;

            };
        }
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    //TODO: CDMA, WCDMA and WLAN modes
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
            return DeviceInfo::instance()->cellSignalStrenghtInfo()->cellNetworkSignalStrength();

        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return -1;  //TODO
}

int QSystemNetworkInfoPrivate::cellId()
{
	return DeviceInfo::instance()->cellNetworkInfo()->cellId();
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->locationAreaCode();
}

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->countryCode();
}

// Mobile Network Code
QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->networkCode();
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    QString imsi = DeviceInfo::instance()->subscriberInfo()->imsi();
    if (imsi.length() >= 3) {
        return imsi.left(3);
    }
	return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return QString();   //TODO
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    //TODO: CDMA, WCDMA and WLAN modes
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
            return DeviceInfo::instance()->cellNetworkInfo()->networkName();

        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return QString();   //TODO
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    return QNetworkInterface();     //TODO
}

void QSystemNetworkInfoPrivate::countryCodeChanged()
{
    emit currentMobileCountryCodeChanged(DeviceInfo::instance()->cellNetworkInfo()->countryCode());
}

void QSystemNetworkInfoPrivate::networkCodeChanged()
{
    emit currentMobileNetworkCodeChanged(DeviceInfo::instance()->cellNetworkInfo()->networkCode());
}

void QSystemNetworkInfoPrivate::networkNameChanged()
{
    emit networkNameChanged(QSystemNetworkInfo::GsmMode, DeviceInfo::instance()->cellNetworkInfo()->networkName());
}

void QSystemNetworkInfoPrivate::cellNetworkSignalStrengthChanged()
{
    emit networkSignalStrengthChanged(QSystemNetworkInfo::GsmMode,
        DeviceInfo::instance()->cellSignalStrenghtInfo()->cellNetworkSignalStrength());
}

void QSystemNetworkInfoPrivate::cellNetworkStatusChanged()
{
    //TODO
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
    const TUid KCRUidLightSettings = {0x10200C8C};
    const TUint32 KLightSensorSensitivity = 0x00000002;
    int ret = 0;
    TRAP_IGNORE(
        CRepository *lightRepository = CRepository::NewLC(KCRUidLightSettings);
        User::LeaveIfError(lightRepository->Get(KLightSensorSensitivity, ret));
        if (ret == 0) {
            ret = 1;
        }
        CleanupStack::PopAndDestroy(lightRepository);
    )
    return ret;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    int depth = 0;
    TRAP_IGNORE(
        RWsSession ws;
        User::LeaveIfError(ws.Connect());
        CleanupClosePushL(ws);
        CWsScreenDevice *wsScreenDevice = new (ELeave)CWsScreenDevice(ws);
        CleanupStack::PushL(wsScreenDevice);
        User::LeaveIfError(wsScreenDevice->Construct(screen));
        TDisplayMode mode = wsScreenDevice->DisplayMode();
        switch (mode) {
            case EGray2: depth = 1; break;
            case EGray4: depth = 2; break;
            case EGray16:
            case EColor16: depth = 4; break;
            case EGray256:
            case EColor256: depth = 8; break;
            case EColor4K: depth = 12; break;
            case EColor64K: depth = 16; break;
            case EColor16M:
            case EColor16MA: depth = 24; break;
            case EColor16MU: depth = 32; break;
            case ENone:
            case ERgb:
            case EColorLast:
            default: depth = 0;
                break;
        }
        CleanupStack::PopAndDestroy(2, &ws);
    )
    
    return depth;
}

//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
    : QObject(parent)
{
    iFs.Connect();
}

QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
    iFs.Close();
}

qlonglong QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    if (driveVolume.size() != 1) {
        return -1;
    }

    TInt drive;
    if (RFs::CharToDrive(TChar(driveVolume[0].toAscii()), drive) != KErrNone) {
        return -1;
    }

    TVolumeInfo volumeInfo;
    if (iFs.Volume(volumeInfo, drive) != KErrNone) {
        return -1;
    }

    return volumeInfo.iSize;
}

qlonglong QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    if (driveVolume.size() != 1) {
        return -1;
    }

    TInt drive;
    if (RFs::CharToDrive(TChar(driveVolume[0].toAscii()), drive) != KErrNone) {
        return -1;
    }

    TVolumeInfo volumeInfo;
    if (iFs.Volume(volumeInfo, drive) != KErrNone) {
        return -1;
    }

    return volumeInfo.iFree;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    QStringList logicalDrives;
    RFs fsSession;
    TRAPD(err,
        User::LeaveIfError(fsSession.Connect());
        CleanupClosePushL(fsSession);
        TDriveList drivelist;
        User::LeaveIfError(fsSession.DriveList(drivelist));
        for (int i = 0; i < KMaxDrives; ++i) {
            if (drivelist[i] != 0) {
                TChar driveChar;
                User::LeaveIfError(RFs::DriveToChar(i, driveChar));
                logicalDrives << QChar(driveChar);
            }
        }
        CleanupStack::PopAndDestroy(&fsSession);
    )
    if (err != KErrNone) {
        return QStringList();
    }
    return logicalDrives;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    if (driveVolume.size() != 1) {
        return QSystemStorageInfo::NoDrive;
    }

    TInt drive;
    if (RFs::CharToDrive(TChar(driveVolume[0].toAscii()), drive) != KErrNone) {
        return QSystemStorageInfo::NoDrive;
    }

    TDriveInfo driveInfo;
    if (iFs.Drive(driveInfo, drive) != KErrNone) {
        return QSystemStorageInfo::NoDrive;
    }

    if (driveInfo.iType == EMediaRemote) {
        return QSystemStorageInfo::RemoteDrive;
    } else if (driveInfo.iType == EMediaCdRom) {
        return QSystemStorageInfo::CdromDrive;
    }
    
    if (driveInfo.iDriveAtt & KDriveAttInternal) {
        return QSystemStorageInfo::InternalDrive;
    } else if (driveInfo.iDriveAtt & KDriveAttRemovable) {
        return QSystemStorageInfo::RemovableDrive;
    }

    return QSystemStorageInfo::NoDrive;
};


//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
    : QObject(parent)
{
    DeviceInfo::instance()->batteryInfo()->addObserver(this);
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    DeviceInfo::instance()->batteryInfo()->removeObserver(this);
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;   //TODO
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;    //TODO
    return methods;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    CTelephony::TBatteryStatus powerState = DeviceInfo::instance()->batteryInfo()->powerState();

    switch (powerState) {
        case CTelephony::EPoweredByBattery:
            return QSystemDeviceInfo::BatteryPower;

        case CTelephony::EBatteryConnectedButExternallyPowered:
        {
            if (DeviceInfo::instance()->batteryInfo()->batteryLevel() < 100) { //TODO: Use real indicator, EPSHWRMChargingStatus::EChargingStatusNotCharging?
                return QSystemDeviceInfo::WallPowerChargingBattery;
            } else {
                return QSystemDeviceInfo::WallPower;
            }
            return QSystemDeviceInfo::WallPower;
        }
        case CTelephony::ENoBatteryConnected:
            return QSystemDeviceInfo::WallPower;

        case CTelephony::EPowerFault:
        case CTelephony::EPowerStatusUnknown:
        default:
            return QSystemDeviceInfo::UnknownPower;
    }
}

QString QSystemDeviceInfoPrivate::imei()
{
    return DeviceInfo::instance()->phoneInfo()->imei();
}

QString QSystemDeviceInfoPrivate::imsi()
{
    return DeviceInfo::instance()->subscriberInfo()->imsi();
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    return DeviceInfo::instance()->phoneInfo()->manufacturer();
}

QString QSystemDeviceInfoPrivate::model()
{
    return DeviceInfo::instance()->phoneInfo()->model();
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

int QSystemDeviceInfoPrivate::batteryLevel() const
{
    return DeviceInfo::instance()->batteryInfo()->batteryLevel();
}

QSystemDeviceInfo::BatteryStatus QSystemDeviceInfoPrivate::batteryStatus()
{
    //TODO: To be moved in QSystemDeviceInfo?
    int batteryLevel = DeviceInfo::instance()->batteryInfo()->batteryLevel();
    if(batteryLevel < 4) {
        return QSystemDeviceInfo::BatteryCritical;
    }   else if (batteryLevel < 11) {
        return QSystemDeviceInfo::BatteryVeryLow;
    }  else if (batteryLevel < 41) {
        return QSystemDeviceInfo::BatteryLow;
    }   else if (batteryLevel > 40) {
        return QSystemDeviceInfo::BatteryNormal;
    }

    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;  //TODO
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;   //TODO
}

void QSystemDeviceInfoPrivate::batteryLevelChanged()
{
    emit batteryLevelChanged(batteryLevel());
    
    int batteryLevel = DeviceInfo::instance()->batteryInfo()->batteryLevel();
    QSystemDeviceInfo::BatteryStatus status(batteryStatus());
    
    if(batteryLevel < 4 && status != QSystemDeviceInfo::BatteryCritical) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryCritical);
    } else if (batteryLevel < 11 && status != QSystemDeviceInfo::BatteryVeryLow) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryVeryLow);
    } else if (batteryLevel < 41 && status != QSystemDeviceInfo::BatteryLow) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryLow);
    } else if (batteryLevel > 40 && status != QSystemDeviceInfo::BatteryNormal) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryNormal);
    } else {
        emit batteryStatusChanged(QSystemDeviceInfo::NoBatteryLevel);
    }
}

void QSystemDeviceInfoPrivate::powerStateChanged()
{
    emit powerStateChanged(currentPowerState());
}

DeviceInfo *DeviceInfo::m_instance = NULL;

//////// QSystemScreenSaver
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
    : QObject(parent), m_screenSaverInhibited(false)
{
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return m_screenSaverInhibited;
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    if (m_screenSaverInhibited) {
        return true;
    }

    m_screenSaverInhibited = true;
    resetInactivityTime();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(resetInactivityTime()));
    timer->start(3000); //3 seconds interval

    return true;
}

void QSystemScreenSaverPrivate::resetInactivityTime()
{
    User::ResetInactivityTime();
}

QT_END_NAMESPACE
