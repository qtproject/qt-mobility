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

#include <SysUtil.h>
#include <ptiengine.h>

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
    QString lang = QLocale::system().name().left(2);
    if(lang.isEmpty() || lang == "C") {
        lang = "en";
    }
    return lang;
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
        QLocale::Language language(TLanguageToQLocale(TLanguage(languageCodes[i])));
        QString lang = QLocale(language).name().left(2);
        if(lang.isEmpty() || lang == "C") {
            lang = "en";
        }
        languages << lang;
    }
    CleanupStack::PopAndDestroy(2, ptiEngine);
    )
    return languages;
}

QLocale::Language QSystemInfoPrivate::TLanguageToQLocale(TLanguage language) const
{
    switch (language) {
        case ELangAmerican: 
        case ELangCanadianEnglish:
        case ELangInternationalEnglish:
        case ELangSouthAfricanEnglish:
        case ELangAustralian:
        case ELangEnglish: return QLocale::English;
        case ELangSwissFrench:
        case ELangInternationalFrench:
        case ELangCanadianFrench:
        case ELangBelgianFrench:
        case ELangFrench: return QLocale::French;
        case ELangSwissGerman:
        case ELangAustrian:
        case ELangGerman: return QLocale::German;
        case ELangInternationalSpanish:
        case ELangLatinAmericanSpanish:
        case ELangSpanish: return QLocale::Spanish;
        case ELangItalian: return QLocale::Italian;
        case ELangFinlandSwedish:
        case ELangSwedish: return QLocale::Swedish;
        case ELangDanish: return QLocale::Danish;
        case ELangNorwegian: return QLocale::Norwegian;
        case ELangFinnish: return QLocale::Finnish;
        case ELangPortuguese: return QLocale::Portuguese;
        case ELangCyprusTurkish:
        case ELangTurkish: return QLocale::Turkish;
        case ELangIcelandic: return QLocale::Icelandic;
        case ELangRussian: return QLocale::Russian;
        case ELangHungarian: return QLocale::Hungarian;
        case ELangBelgianFlemish:
        case ELangDutch: return QLocale::Dutch;
        case ELangNewZealand: return QLocale::Maori;
        case ELangCzech: return QLocale::Czech;
        case ELangSlovak: return QLocale::Slovak;
        case ELangPolish: return QLocale::Polish;
        case ELangSlovenian: return QLocale::Slovenian;
        case ELangTaiwanChinese:
        case ELangHongKongChinese:
        case ELangPrcChinese: return QLocale::Chinese;
        case ELangJapanese: return QLocale::Japanese;
        case ELangThai: return QLocale::Thai;
        case ELangAfrikaans: return QLocale::Afrikaans;
        case ELangAlbanian: return QLocale::Albanian;
        case ELangAmharic: return QLocale::Amharic;
        case ELangArabic: return QLocale::Arabic;
        case ELangArmenian: return QLocale::Armenian;
        case ELangTagalog: return QLocale::Tagalog;
        case ELangBelarussian:
        case ELangBengali:  return QLocale::Bengali;
        case ELangBulgarian: return QLocale::Bulgarian;
        case ELangBurmese: return QLocale::Burmese;
        case ELangCatalan: return QLocale::Catalan;
        case ELangCroatian: return QLocale::Croatian;
        case ELangEstonian: return QLocale::Estonian;
        case ELangFarsi: return QLocale::Persian;
        case ELangScotsGaelic: return QLocale::Gaelic;
        case ELangGeorgian: return QLocale::Georgian;
        case ELangGreek:
        case ELangCyprusGreek: return QLocale::Greek;
        case ELangGujarati: return QLocale::Gujarati;
        case ELangHebrew: return QLocale::Hebrew;
        case ELangHindi: return QLocale::Hindi;
        case ELangIndonesian: return QLocale::Indonesian;
        case ELangIrish: return QLocale::Irish;
        case ELangSwissItalian: return QLocale::Italian;
        case ELangKannada: return QLocale::Kannada;
        case ELangKazakh: return QLocale::Kazakh;
        case ELangKhmer: return QLocale::Cambodian;
        case ELangKorean: return QLocale::Korean;
        case ELangLao: return QLocale::Laothian;
        case ELangLatvian: return QLocale::Latvian;
        case ELangLithuanian: return QLocale::Lithuanian;
        case ELangMacedonian: return QLocale::Macedonian;
        case ELangMalay: return QLocale::Malay;
        case ELangMalayalam: return QLocale::Malayalam;
        case ELangMarathi: return QLocale::Marathi;
        case ELangMoldavian: return QLocale::Moldavian;
        case ELangMongolian: return QLocale::Mongolian;
        case ELangNorwegianNynorsk: return QLocale::NorwegianNynorsk;
        case ELangBrazilianPortuguese: return QLocale::Portuguese;
        case ELangPunjabi: return QLocale::Punjabi;
        case ELangRomanian: return QLocale::Romanian;
        case ELangSerbian: return QLocale::Serbian;
        case ELangSomali: return QLocale::Somali;
        case ELangSwahili: return QLocale::Swahili;
        case ELangTamil: return QLocale::Tamil;
        case ELangTelugu: return QLocale::Telugu;
        case ELangTibetan: return QLocale::Tibetan;
        case ELangTigrinya: return QLocale::Tigrinya;
        case ELangTurkmen: return QLocale::Turkmen;
        case ELangUkrainian: return QLocale::Ukrainian;
        case ELangUrdu: return QLocale::Urdu;
        case ELangVietnamese: return QLocale::Vietnamese;
        case ELangWelsh: return QLocale::Welsh;
        case ELangZulu: return QLocale::Zulu;
        case ELangSinhalese:
        case ELangTest:
        case ELangReserved1:
        case ELangReserved2:
        case ELangOther:
        case ELangNone:
        default:
            break;
    }
    return QLocale::C;
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    return QString();   //TODO
}

QString QSystemInfoPrivate::currentCountryCode() const
{
    return QString();   //TODO
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    return false;   //TODO
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
    return -1;  //TODO
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    return -1;  //TODO
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
    : QObject(parent)
{
}

QT_END_NAMESPACE
