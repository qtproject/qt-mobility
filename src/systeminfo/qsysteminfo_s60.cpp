/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <sysutil.h>
#include <ptiengine.h>
#include <featdiscovery.h>
#ifndef KFeatureIdMmc
#include <featureinfo.h>
#endif
#include <hwrmvibra.h>
#include <aknutils.h>
#include <w32std.h>
#include <centralrepository.h>
#include <mproengengine.h>
#include <proengfactory.h>
#include <mproengnotifyhandler.h>
#include <btserversdkcrkeys.h>
#include <bt_subscribe.h>
#include <bttypes.h>
#include <etel3rdparty.h>
#include <aknkeylock.h>
#define RD_STARTUP_CHANGE
#include <startupdomainpskeys.h>

QTM_BEGIN_NAMESPACE

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
    TRAP_IGNORE(
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
    languages.removeDuplicates();
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
        case ELangScotsGaelic: return "gd";
        case ELangGeorgian: return "ka";
        case ELangGreek:
        case ELangCyprusGreek: return "el";
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

QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString & /*parameter*/)
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
            if (CCoeEnv::Static()) {
                //TODO: Do something with the AVKON dependency
                return AknLayoutUtils::PenEnabled();
            }
            return false;
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

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
    : QObject(parent)
{
    DeviceInfo::instance()->cellSignalStrenghtInfo()->addObserver(this);
    DeviceInfo::instance()->cellNetworkInfo()->addObserver(this);
    DeviceInfo::instance()->cellNetworkRegistrationInfo()->addObserver(this);
    connect(DeviceInfo::instance()->wlanInfo(), SIGNAL(wlanNetworkNameChanged()),
        this, SLOT(wlanNetworkNameChanged()));
    connect(DeviceInfo::instance()->wlanInfo(), SIGNAL(wlanNetworkSignalStrengthChanged()),
        this, SLOT(wlanNetworkSignalStrengthChanged()));
    connect(DeviceInfo::instance()->wlanInfo(), SIGNAL(wlanNetworkStatusChanged()),
        this, SLOT(wlanNetworkStatusChanged()));
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
    DeviceInfo::instance()->cellSignalStrenghtInfo()->removeObserver(this);
    DeviceInfo::instance()->cellNetworkInfo()->removeObserver(this);
    DeviceInfo::instance()->cellNetworkRegistrationInfo()->removeObserver(this);
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
        case QSystemNetworkInfo::WcdmaMode:
        {
            CTelephony::TRegistrationStatus networkStatus = DeviceInfo::instance()
                ->cellNetworkRegistrationInfo()->cellNetworkStatus();

            CTelephony::TNetworkMode networkMode = DeviceInfo::instance()->cellNetworkInfo()->networkMode();
            if (networkMode == CTelephony::ENetworkModeGsm && mode != QSystemNetworkInfo::GsmMode)
                return QSystemNetworkInfo::NoNetworkAvailable;

            if ((networkMode == CTelephony::ENetworkModeCdma95 || networkMode == CTelephony::ENetworkModeCdma2000) &&
                mode != QSystemNetworkInfo::CdmaMode)
                return QSystemNetworkInfo::NoNetworkAvailable;

            if (networkMode == CTelephony::ENetworkModeWcdma && mode != QSystemNetworkInfo::WcdmaMode)
                return QSystemNetworkInfo::NoNetworkAvailable;

            switch(networkStatus) {
                case CTelephony::ERegistrationUnknown: return QSystemNetworkInfo::UndefinedStatus;
                case CTelephony::ENotRegisteredNoService: return QSystemNetworkInfo::NoNetworkAvailable;
                case CTelephony::ENotRegisteredEmergencyOnly: return QSystemNetworkInfo::EmergencyOnly;
                case CTelephony::ENotRegisteredSearching: return QSystemNetworkInfo::Searching;
                case CTelephony::ERegisteredBusy: return QSystemNetworkInfo::Busy;
                case CTelephony::ERegisteredOnHomeNetwork: return QSystemNetworkInfo::HomeNetwork;
                case CTelephony::ERegistrationDenied: return QSystemNetworkInfo::Denied;
                case CTelephony::ERegisteredRoaming: return QSystemNetworkInfo::Roaming;
                default:
                    break;
            };
        }
        case QSystemNetworkInfo::WlanMode:
        {
            if (DeviceInfo::instance()->wlanInfo()->wlanNetworkConnectionStatus())
                return QSystemNetworkInfo::Connected;
            else
                return QSystemNetworkInfo::NoNetworkAvailable;
        }
        case QSystemNetworkInfo::EthernetMode:
        case QSystemNetworkInfo::BluetoothMode:
        case QSystemNetworkInfo::WimaxMode:
        default:
            break;
    };
    return QSystemNetworkInfo::UndefinedStatus;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
        case QSystemNetworkInfo::WcdmaMode:
        {
            CTelephony::TNetworkMode networkMode = DeviceInfo::instance()->cellNetworkInfo()->networkMode();
            if (networkMode == CTelephony::ENetworkModeGsm && mode != QSystemNetworkInfo::GsmMode)
                return -1;

            if ((networkMode == CTelephony::ENetworkModeCdma95 || networkMode == CTelephony::ENetworkModeCdma2000) &&
                mode != QSystemNetworkInfo::CdmaMode)
                return -1;

            if (networkMode == CTelephony::ENetworkModeWcdma && mode != QSystemNetworkInfo::WcdmaMode)
                return -1;

            return DeviceInfo::instance()->cellSignalStrenghtInfo()->cellNetworkSignalStrength();
        }

        case QSystemNetworkInfo::WlanMode:
            return DeviceInfo::instance()->wlanInfo()->wlanNetworkSignalStrength();
        case QSystemNetworkInfo::EthernetMode:
        case QSystemNetworkInfo::BluetoothMode:
        case QSystemNetworkInfo::WimaxMode:
        default:
            break;
    };
    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
	return DeviceInfo::instance()->cellNetworkInfo()->cellId();
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->locationAreaCode();
}

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->countryCode();
}

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
    CTelephony::TRegistrationStatus networkStatus = DeviceInfo::instance()
        ->cellNetworkRegistrationInfo()->cellNetworkStatus();
    if (networkStatus == CTelephony::ERegisteredOnHomeNetwork) {
        return DeviceInfo::instance()->cellNetworkInfo()->networkCode();
    }
    return QString();
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString name;
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
        case QSystemNetworkInfo::WcdmaMode:
        {
            CTelephony::TNetworkMode networkMode = DeviceInfo::instance()->cellNetworkInfo()->networkMode();
            if (networkMode == CTelephony::ENetworkModeGsm && mode != QSystemNetworkInfo::GsmMode)
                return QString();

            if ((networkMode == CTelephony::ENetworkModeCdma95 || networkMode == CTelephony::ENetworkModeCdma2000) &&
                mode != QSystemNetworkInfo::CdmaMode)
                return QString();

            if (networkMode == CTelephony::ENetworkModeWcdma && mode != QSystemNetworkInfo::WcdmaMode)
                return QString();

            return DeviceInfo::instance()->cellNetworkInfo()->networkName();
        }
        case QSystemNetworkInfo::WlanMode:
            return DeviceInfo::instance()->wlanInfo()->wlanNetworkName();
        case QSystemNetworkInfo::EthernetMode:
        case QSystemNetworkInfo::BluetoothMode:
        case QSystemNetworkInfo::WimaxMode:
        default:
            break;
    };
    return name;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    QString address;

    switch(mode) {
        case QSystemNetworkInfo::WlanMode:
        {
            const TUid KPSUidWlan = {0x101f8ec5};
            const TUint KPSWlanMacAddress = 0x00000001;
            const TUint KPSWlanMacAddressLength = 6;
            TBuf8<KPSWlanMacAddressLength> wlanMacAddr;
            if (RProperty::Get(KPSUidWlan, KPSWlanMacAddress, wlanMacAddr) == KErrNone) {
                for (TUint i = 0; i < KPSWlanMacAddressLength - 1; ++i) {
                    address += QString(QByteArray((const char*)wlanMacAddr.Mid(i, 1).Ptr(), 1).toHex());
                    address += ":";
                }
                address += QString(QByteArray((const char*)wlanMacAddr.Mid(KPSWlanMacAddressLength - 1, 1).Ptr(), 1).toHex());
            }
            break;
        }
        case QSystemNetworkInfo::BluetoothMode:
        {
            TBuf<20> bluetoothAddr;
            TPckgBuf<TBTDevAddr> bluetoothAddrPckg;
            if (RProperty::Get(KUidSystemCategory,
                KPropertyKeyBluetoothGetLocalDeviceAddress, bluetoothAddrPckg) == KErrNone) {
                bluetoothAddrPckg().GetReadable(bluetoothAddr, KNullDesC, _L(":"), KNullDesC);
                address = QString::fromUtf16(bluetoothAddr.Ptr(), bluetoothAddr.Length());
            }
            break;
        }
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
        case QSystemNetworkInfo::WcdmaMode:
        case QSystemNetworkInfo::WimaxMode:
        case QSystemNetworkInfo::EthernetMode:
        default:
            break;
    };
    return address;
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode /*mode*/)
{
    return QNetworkInterface();
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
    emit networkNameChanged(currentMode(), DeviceInfo::instance()->cellNetworkInfo()->networkName());
}

void QSystemNetworkInfoPrivate::networkModeChanged()
{
    emit networkModeChanged(currentMode());
}

void QSystemNetworkInfoPrivate::cellNetworkSignalStrengthChanged()
{
    emit networkSignalStrengthChanged(currentMode(),
        DeviceInfo::instance()->cellSignalStrenghtInfo()->cellNetworkSignalStrength());
}

void QSystemNetworkInfoPrivate::cellNetworkStatusChanged()
{
    QSystemNetworkInfo::NetworkMode mode = currentMode();
    emit networkStatusChanged(mode, networkStatus(mode));
}

void QSystemNetworkInfoPrivate::wlanNetworkNameChanged()
{
    emit networkNameChanged(QSystemNetworkInfo::WlanMode,
        DeviceInfo::instance()->wlanInfo()->wlanNetworkName());
}

void QSystemNetworkInfoPrivate::wlanNetworkSignalStrengthChanged()
{
    emit networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode,
        DeviceInfo::instance()->wlanInfo()->wlanNetworkSignalStrength());
}

//TODO: There are no WLAN specific modes (Not connected, Infrastructure, Adhoc, Secure Infrastructure and Searching)
void QSystemNetworkInfoPrivate::wlanNetworkStatusChanged()
{
    bool status = DeviceInfo::instance()->wlanInfo()->wlanNetworkConnectionStatus();
    if (status)
        emit networkStatusChanged(QSystemNetworkInfo::WlanMode, QSystemNetworkInfo::Connected);
    else
        emit networkStatusChanged(QSystemNetworkInfo::WlanMode, QSystemNetworkInfo::NoNetworkAvailable);
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    CTelephony::TNetworkMode networkMode = DeviceInfo::instance()->cellNetworkInfo()->networkMode();
    switch (networkMode) {
        case CTelephony::ENetworkModeGsm: mode = QSystemNetworkInfo::GsmMode; break;
        case CTelephony::ENetworkModeCdma95:
        case CTelephony::ENetworkModeCdma2000: mode = QSystemNetworkInfo::CdmaMode; break;
        case CTelephony::ENetworkModeWcdma: mode = QSystemNetworkInfo::WcdmaMode; break;
        default:
            break;
    }
    return mode;
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
    : QObject(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int /*screen*/)
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
        depth = TDisplayModeUtils::NumDisplayModeBitsPerPixel(wsScreenDevice->DisplayMode());
        CleanupStack::PopAndDestroy(2, &ws);
    )
    return depth;
}

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

QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
    : QObject(parent), m_profileEngine(NULL), m_proEngNotifyHandler(NULL),
    m_bluetoothRepository(NULL), m_bluetoothNotifyHandler(NULL)
{
    DeviceInfo::instance()->batteryInfo()->addObserver(this);
    DeviceInfo::instance()->chargingStatus()->addObserver(this);
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    DeviceInfo::instance()->chargingStatus()->removeObserver(this);
    DeviceInfo::instance()->batteryInfo()->removeObserver(this);

    if (m_proEngNotifyHandler) {
        m_proEngNotifyHandler->CancelProfileActivationNotifications();
        delete m_proEngNotifyHandler;
    }

    if(m_profileEngine) {
        m_profileEngine->Release();
    }

    delete m_bluetoothNotifyHandler;
    delete m_bluetoothRepository;
}

void QSystemDeviceInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile))) {
        if (!m_proEngNotifyHandler) {
            TRAPD(err,
                m_proEngNotifyHandler = ProEngFactory::NewNotifyHandlerL();
                m_proEngNotifyHandler->RequestProfileActivationNotificationsL(*this);
            )
            if (err != KErrNone) {
                delete m_proEngNotifyHandler;
                m_proEngNotifyHandler = NULL;
            }
        }
    } else if (QLatin1String(signal) == SIGNAL(bluetoothStateChanged(bool))) {
        if (!m_bluetoothRepository) {
            TRAPD(btErr,
                m_bluetoothRepository = CRepository::NewL(KCRUidBluetoothPowerState);
                m_bluetoothNotifyHandler = CCenRepNotifyHandler::NewL(
                    *this, *m_bluetoothRepository, CCenRepNotifyHandler::EIntKey, KBTPowerState);
                m_bluetoothNotifyHandler->StartListeningL();
            )
            if (btErr != KErrNone) {
                delete m_bluetoothNotifyHandler;
                m_bluetoothNotifyHandler = NULL;
                delete m_bluetoothRepository;
                m_bluetoothRepository = NULL;
            }
        }
    }
}

void QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate::HandleNotifyInt(TUint32 aId, TInt aNewValue)
{
    if (aId == KBTPowerState) {
        emit bluetoothStateChanged(aNewValue & 0x00000001 != 0);
    }
}

void QSystemDeviceInfoPrivate::HandleProfileActivatedL(TInt aProfileId)
{
    QSystemDeviceInfo::Profile profile = s60ProfileIdToProfile(aProfileId);
    emit currentProfileChanged(profile);
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    QSystemDeviceInfo::Profile profile = QSystemDeviceInfo::UnknownProfile;

    if (!m_profileEngine) {
        TRAP_IGNORE(m_profileEngine = ProEngFactory::NewEngineL();)
    }

    if (m_profileEngine) {
        return s60ProfileIdToProfile(m_profileEngine->ActiveProfileId());
    }

    return profile;
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::s60ProfileIdToProfile(TInt profileId) const
{
    QSystemDeviceInfo::Profile profile = QSystemDeviceInfo::UnknownProfile;

    //From profileenginesdkcrkeys.h:
    //0 = General profile (default value)<br>
    //1 = Silent profile<br>
    //2 = Meeting profile<br>
    //3 = Outdoor profile<br>
    //4 = Pager profile<br>
    //5 = Off-line profile<br>
    //6 = Drive profile<br>
    //30-49 = User-created profiles<br>

    switch (profileId) {
    case 0: profile = QSystemDeviceInfo::NormalProfile; break;
    case 1: profile = QSystemDeviceInfo::SilentProfile; break;
    case 2: profile = QSystemDeviceInfo::CustomProfile; break;
    case 3: profile = QSystemDeviceInfo::LoudProfile; break;
    case 4: profile = QSystemDeviceInfo::CustomProfile; break;
    case 5: profile = QSystemDeviceInfo::OfflineProfile; break;
    case 6: profile = QSystemDeviceInfo::CustomProfile; break;
    default:
        {
            if (profileId >= 30 && profileId <= 49) {
                profile = QSystemDeviceInfo::CustomProfile;
            } else {
                profile = QSystemDeviceInfo::UnknownProfile; break;
            }
        }
    }
    return profile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;

    methods |= QSystemDeviceInfo::Keys;
    methods |= QSystemDeviceInfo::Keypad;

    if (CCoeEnv::Static()) {
        //TODO: Do something with the AVKON dependency
        if (AknLayoutUtils::PenEnabled()) methods |= QSystemDeviceInfo::SingleTouch;
    }

    TRAP_IGNORE(
        if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdQwertyInput)) {
            methods |= QSystemDeviceInfo::Keyboard;
        }
    )
    return methods;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    switch (DeviceInfo::instance()->chargingStatus()->chargingStatus()) {
    case EChargingStatusNotConnected:
    case EChargingStatusNotCharging:
    case EChargingStatusError:
        return QSystemDeviceInfo::BatteryPower;
    case EChargingStatusCharging:
    case EChargingStatusChargingContinued:
    case EChargingStatusAlmostComplete:
        return QSystemDeviceInfo::WallPowerChargingBattery;
    case EChargingStatusChargingComplete:
        return QSystemDeviceInfo::WallPower;
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
    if (simStatus() == QSystemDeviceInfo::SimNotAvailable)
        return QString();
    else
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
    TInt lockStatus = 0;
    TInt err = RProperty::Get(KPSUidStartup, KStartupSimLockStatus, lockStatus);
    if (err == KErrNone && (TPSSimLockStatus)lockStatus != ESimLockOk) {
        return QSystemDeviceInfo::SimLocked;
    }

    TInt simStatus = 0;
    err = RProperty::Get(KPSUidStartup, KPSSimStatus, simStatus);
    if (err == KErrNone && TPSSimStatus(simStatus) == ESimUsable) {
        return QSystemDeviceInfo::SingleSimAvailable;
    }

    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    bool isLocked = false;

    RAknKeylock2 keyLock;
    TInt err = keyLock.Connect();
    if (err == KErrNone) {
        isLocked = keyLock.IsKeyLockEnabled();
        keyLock.Close();
    }

    return isLocked;
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

void QSystemDeviceInfoPrivate::chargingStatusChanged()
{
    emit powerStateChanged(currentPowerState());
}

DeviceInfo *DeviceInfo::m_instance = NULL;

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

#include "moc_qsysteminfo_s60_p.cpp"

QTM_END_NAMESPACE
