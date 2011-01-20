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
#include "qsysteminfocommon_p.h"
#include "qsysteminfo_s60_p.h"

#include <QStringList>
#include <QDir>
#include <QRegExp>
#include <QTimer>
#include <QList>

#include <sysutil.h>
#ifdef HB_SUPPORTED
#include <hbinputkeymapfactory.h>
#include <hbinputlanguage.h>
#else
#include <PtiEngine.h>
#endif // HB_SUPPORTED
#include <featdiscovery.h>
#ifndef KFeatureIdMmc
#include <featureinfo.h>
#endif
#include <hwrmvibra.h>
#include <AknUtils.h>
#include <w32std.h>
#include <centralrepository.h>
#include <MProEngEngine.h>
#include <ProEngFactory.h>
#include <MProEngNotifyHandler.h>
#include <btserversdkcrkeys.h>
#include <bt_subscribe.h>
#include <bttypes.h>
#include <etel3rdparty.h>
#include <aknkeylock.h>
#define RD_STARTUP_CHANGE
#include <startupdomainpskeys.h>
#include <hwrmlight.h>
#include <hwrmfmtx.h>
#ifdef SYMBIAN_3_PLATFORM
#include <avkondomainpskeys.h>
#include <hwrmdomainpskeys.h>
#include <AvkonInternalCRKeys.h>
#include <AknFepInternalPSKeys.h>
#endif
#include <e32debug.h>
#include <QCryptographicHash>
#include <etel.h>
#include <etelmm.h>

const TUint32 KAknKeyboardType = 0x0000000B;
const float KMMPerTwip  = 0.0177f; //Milimeter Per Twip

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

#ifdef HB_SUPPORTED
QStringList QSystemInfoPrivate::availableLanguages() const
{
    QStringList languages;
    QList<HbInputLanguage> hblanguages = HbKeymapFactory::availableLanguages();
    foreach (HbInputLanguage lang, hblanguages) {
        QString language = QLocaleToISO639_1(lang.language());
        if (!language.isEmpty()) {
            languages << language;
        }
    }
    languages.removeDuplicates();
    return languages;
}
#else
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
#endif //HB_SUPPORTED

QString QSystemInfoPrivate::QLocaleToISO639_1(QLocale::Language language) const
{
       switch (language) {
       case QLocale::English: return "en";
       case QLocale::Lithuanian: return "lt";
       case QLocale::Malay: return "ms";
       case QLocale::Polish: return "pl";
       case QLocale::Portuguese: return "pt";
       case QLocale::Romanian: return "ro";
       case QLocale::Serbian: return "sr";
       case QLocale::Slovak: return "sk";
       case QLocale::Slovenian: return "sl";
       case QLocale::Spanish: return "es";
       case QLocale::Swedish: return "sv";
       case QLocale::Tagalog: return "tl";
       case QLocale::Czech: return "cs";
       case QLocale::Dutch: return "nl";
       case QLocale::Turkish: return "tr";
       case QLocale::Estonian: return "et";
       case QLocale::French: return "fr";
       case QLocale::Greek: return "el";
       case QLocale::Icelandic: return "is";
       case QLocale::Indonesian: return "id";
       case QLocale::Italian: return "it";
       case QLocale::Latvian: return "lv";
       case QLocale::Croatian: return "hr";
       case QLocale::German: return "de";
       case QLocale::Hungarian: return "hu";
       case QLocale::Bulgarian: return "bg";
       case QLocale::Finnish: return "fi";
       case QLocale::Russian: return "ru";
       case QLocale::Danish: return "da";
       case QLocale::Norwegian: return "no";
       case QLocale::Ukrainian: return "uk";
       case QLocale::Arabic: return "ar";
       case QLocale::Hebrew: return "he";
       case QLocale::Thai: return "th";
       case QLocale::Japanese: return "ja";
       case QLocale::Vietnamese: return "vi";
       case QLocale::Persian: return "fa";
       case QLocale::Hindi: return "hi";
       case QLocale::Urdu: return "ur";
       case QLocale::Catalan: return "ca";
       case QLocale::Galician: return "gl";
       case QLocale::Basque: return "eu";
       case QLocale::Marathi: return "mr";
       case QLocale::Korean: return "ko";
       default:
           break;
       }
    return "";
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
        case QSystemInfo::FmTransmitterFeature:
        {
            TRAPD(err,
                //Leaves with KErrNotSupported if device doesn't support FmTransmitter feature.
                CHWRMFmTx *fmTX = CHWRMFmTx::NewL();
                delete fmTX;
            )
            return err == KErrNone;
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
    DeviceInfo::instance()->networkInfo()->addObserver(this);
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
    DeviceInfo::instance()->networkInfo()->removeObserver(this);
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    qDebug()<<"File-"<<__FILE__<<" : Line-"<<__LINE__<<"  :: Inside QSystemNetworkInfoPrivate::networkStatus -> Param mode = "<<mode;
    QSystemNetworkInfo::NetworkStatus networkStatus = QSystemNetworkInfo::UndefinedStatus;
    RMobilePhone::TMobilePhoneRegistrationStatus nStatus = RMobilePhone::ERegistrationUnknown;
    QSystemNetworkInfo::NetworkMode currMode =currentMode();
    if (currMode == mode)
    {
        qDebug("NetworkStatus for Current Mode");
        nStatus = DeviceInfo::instance()->networkInfo()->GetStatus();
    }
    switch (nStatus) {
    case RMobilePhone::ENotRegisteredNoService : networkStatus = QSystemNetworkInfo::NoNetworkAvailable; break;
    case RMobilePhone::ENotRegisteredEmergencyOnly : networkStatus = QSystemNetworkInfo::EmergencyOnly; break;
    case RMobilePhone::ENotRegisteredSearching : networkStatus = QSystemNetworkInfo::Searching; break;
    case RMobilePhone::ERegisteredBusy : networkStatus = QSystemNetworkInfo::Busy; break;
    case RMobilePhone::ERegisteredOnHomeNetwork : networkStatus = QSystemNetworkInfo::HomeNetwork; break;
    case RMobilePhone::ERegistrationDenied : networkStatus = QSystemNetworkInfo::Denied; break;
    case RMobilePhone::ERegisteredRoaming : networkStatus = QSystemNetworkInfo::Roaming; break;
    case RMobilePhone::ERegistrationUnknown : break;
    }
    qDebug()<<"File-"<<__FILE__<<" : Line-"<<__LINE__<<"  :: Returning From QSystemNetworkInfoPrivate::networkStatus -> "<<networkStatus;
    return networkStatus;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch (mode) {
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
    switch (mode) {
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

    switch (mode) {
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

void QSystemNetworkInfoPrivate::changedNetworkMode()
{
    emit networkModeChanged(currentMode());
}

void QSystemNetworkInfoPrivate::changedNetworkStatus()
{
    QSystemNetworkInfo::NetworkMode mode = currentMode();
    emit networkStatusChanged(mode, networkStatus(mode));
}

void QSystemNetworkInfoPrivate::changedCellId(int cellIdTel)
{
    emit cellIdChanged(cellIdTel);
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
    bool status = DeviceInfo::instance()->wlanInfo()->wlanNetworkConnectionStatus();
        if (status)
            emit networkNameChanged(QSystemNetworkInfo::WlanMode,DeviceInfo::instance()->wlanInfo()->wlanNetworkName());
        else
            networkNameChanged();//Restore default network name, as WLAN will leave name as blank
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
    qDebug()<<"INSIDE QSystemNetworkInfoPrivate::currentMode";
    QSystemNetworkInfo::NetworkMode networkMode = QSystemNetworkInfo::UnknownMode;
    RMobilePhone::TMobilePhoneNetworkMode nMode = RMobilePhone::ENetworkModeUnknown;
    nMode = DeviceInfo::instance()->networkInfo()->GetMode();
    if (nMode != RMobilePhone::ENetworkModeUnknown) {
        switch (nMode) {
            case RMobilePhone::ENetworkModeGsm : networkMode = QSystemNetworkInfo::GsmMode;
            break;
            case RMobilePhone::ENetworkModeAmps :
            case RMobilePhone::ENetworkModeCdma95 :
            case RMobilePhone::ENetworkModeCdma2000 : networkMode = QSystemNetworkInfo::CdmaMode;
            break;
            case RMobilePhone::ENetworkModeWcdma :
            case RMobilePhone::ENetworkModeTdcdma : networkMode = QSystemNetworkInfo::WcdmaMode;
            break;
            default : break;
        }
    }
    qDebug()<<"RETURNING QSystemNetworkInfoPrivate::currentMode : "<<networkMode;
    return networkMode;
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
    : QObject(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

bool QSystemDisplayInfoPrivate::getSizeandRotation(int screen, TPixelsTwipsAndRotation& sizeAndRotation)
{
    CWsScreenDevice *wsScreenDevice = NULL;
    TInt err = KErrNone;
    TRAP_IGNORE(
        RWsSession ws;
        User::LeaveIfError(ws.Connect());
        CleanupClosePushL(ws);
        wsScreenDevice = new (ELeave)CWsScreenDevice(ws);
        CleanupStack::PushL(wsScreenDevice);
        User::LeaveIfError(err = wsScreenDevice->Construct(screen));
        wsScreenDevice->GetScreenModeSizeAndRotation(screen,sizeAndRotation);
        CleanupStack::PopAndDestroy(2, &ws);
        )
    return (wsScreenDevice != NULL && err == KErrNone);
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


QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfoPrivate::orientation(int screen)
{
    QSystemDisplayInfo::DisplayOrientation orientationStatus = QSystemDisplayInfo::Unknown;
    TPixelsTwipsAndRotation sizeAndRotation;
    if (screen < 16 && screen > -1) {
    bool err = getSizeandRotation(screen, sizeAndRotation);
    if ( err )
        {
            CFbsBitGc::TGraphicsOrientation currentRotation = sizeAndRotation.iRotation;
            switch (currentRotation) {
            case 0:
            case 360:
                orientationStatus = QSystemDisplayInfo::Landscape;
                break;
            case 90:
                orientationStatus = QSystemDisplayInfo::Portrait;
                break;
            case 180:
                orientationStatus = QSystemDisplayInfo::InvertedLandscape;
                break;
            case 270:
                orientationStatus = QSystemDisplayInfo::InvertedPortrait;
                break;
            };
        orientationStatus = QSystemDisplayInfo::Unknown;
        }
    }
    return orientationStatus;
}


float QSystemDisplayInfoPrivate::contrast(int screen)
{
    Q_UNUSED(screen);

    return 0.0;
}

int QSystemDisplayInfoPrivate::getDPIWidth(int screen)
{
    int dpi = 0;
    TPixelsTwipsAndRotation sizeAndRotation;
    if (screen < 16 && screen > -1) {
    bool err = getSizeandRotation(screen, sizeAndRotation);
    if ( err )
        {
            int widthInPixels = sizeAndRotation.iPixelSize.iWidth;
            int widthInTwips = sizeAndRotation.iTwipsSize.iWidth;
            if (widthInPixels > 0 && widthInTwips > 0){
                int twipsPerPixelWidth =  widthInTwips/widthInPixels;
                dpi = KTwipsPerInch / twipsPerPixelWidth;
                }
        }
    }
    return dpi;
}

int QSystemDisplayInfoPrivate::getDPIHeight(int screen)
{
    int dpi=0;
    TPixelsTwipsAndRotation sizeAndRotation;
    if (screen < 16 && screen > -1) {
    bool err = getSizeandRotation(screen, sizeAndRotation);
    if ( err )
        {
            int heightInPixels = sizeAndRotation.iPixelSize.iHeight;
            int heightInTwips = sizeAndRotation.iTwipsSize.iHeight;
            if (heightInPixels > 0 && heightInTwips > 0){
                int twipsPerPixelHeight =  heightInTwips/heightInPixels;
                dpi = KTwipsPerInch / twipsPerPixelHeight;
            }
        }
    }
    return dpi;
}


int QSystemDisplayInfoPrivate::physicalHeight(int screen)
{
    // Physical Height is in MiliMeter
    int height = 0;
    TPixelsTwipsAndRotation sizeAndRotation;
    if (screen < 16 && screen > -1) {
    bool err = getSizeandRotation(screen, sizeAndRotation);
    if ( err )
        {
        int heightInTwips = sizeAndRotation.iTwipsSize.iHeight;
        height = heightInTwips * KMMPerTwip;
        }
    }
    return height;
}

int QSystemDisplayInfoPrivate::physicalWidth(int screen)
{
    // Physical Width is in MiliMeter
    int width=0;
    TPixelsTwipsAndRotation sizeAndRotation;
    if (screen < 16 && screen > -1) {
    bool err = getSizeandRotation(screen, sizeAndRotation);
    if ( err )
        {
        int widthInTwips = sizeAndRotation.iTwipsSize.iWidth;
        width = widthInTwips * KMMPerTwip;
        }
    }
    return width;
}

QSystemDisplayInfo::BacklightState  QSystemDisplayInfoPrivate::backlightStatus(int screen)
{
    QSystemDisplayInfo::BacklightState backlightState = QSystemDisplayInfo::BacklightStateUnknown;
    CHWRMLight::TLightStatus status = CHWRMLight::ELightStatusUnknown;
    //screen "0" is mapped to Primary Display and other screens are mapped to Secondary Display
    if (screen == 0)
    {
        TRAP_IGNORE(CHWRMLight* iLight = CHWRMLight::NewL();
            status = iLight->LightStatus(CHWRMLight::EPrimaryDisplay);
            delete iLight;)
    }
    else
    {
        TRAP_IGNORE(CHWRMLight* iLight = CHWRMLight::NewL();
            status = iLight->LightStatus(CHWRMLight::ESecondaryDisplay);
            qDebug(("Status in case of Secondary display is  %d"),status);
            delete iLight;)
    }

    switch(status){
    case 0:
        backlightState = QSystemDisplayInfo::BacklightStateUnknown;
        break;
    case 1:
        backlightState = QSystemDisplayInfo::BacklightStateOn;
        break;
    case 2:
        backlightState = QSystemDisplayInfo::BacklightStateOff;
        break;
    default:
        break;
    }

    return backlightState;
}

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
    : QObject(parent)
{
    iFs.Connect();
    DeviceInfo::instance()->mmcStorageStatus()->addObserver(this);

#ifdef SYMBIAN_3_PLATFORM
    CStorageDiskNotifier* storageNotifier = DeviceInfo::instance()->storagedisknotifier();
    if (storageNotifier != NULL){
        storageNotifier->AddObserver(this);
        }
#endif
}

QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
    iFs.Close();
    DeviceInfo::instance()->mmcStorageStatus()->removeObserver(this);
#ifdef SYMBIAN_3_PLATFORM
    CStorageDiskNotifier* storageNotifier = DeviceInfo::instance()->storagedisknotifier();
    if (storageNotifier != NULL){
        storageNotifier->RemoveObserver(this);
        }
#endif
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
        if (driveInfo.iType == EMediaHardDisk)
            return QSystemStorageInfo::InternalFlashDrive;
        else if (driveInfo.iType == EMediaNANDFlash)
            return QSystemStorageInfo::RamDrive;
        else
            return QSystemStorageInfo::InternalDrive;
    } else if (driveInfo.iDriveAtt & KDriveAttRemovable) {
        return QSystemStorageInfo::RemovableDrive;
    }

    return QSystemStorageInfo::NoDrive;
}

void QSystemStorageInfoPrivate::storageStatusChanged(bool added, const QString &aDriveVolume)
{
    emit logicalDriveChanged(added, aDriveVolume);
};

#ifdef SYMBIAN_3_PLATFORM
void QSystemStorageInfoPrivate::DiskSpaceChanged(const QString &aDriveVolume)
{
    QSystemStorageInfo::StorageState state = CheckDiskSpaceThresholdLimit(aDriveVolume);
    emit storageStateChanged(aDriveVolume, state);
}
#endif

QString QSystemStorageInfoPrivate::uriForDrive(const QString &driveVolume)
{
    if (driveVolume.size() != 1) {
        return (QString::null);
    }

    TInt drive;
    if (RFs::CharToDrive(TChar(driveVolume[0].toAscii()), drive) != KErrNone) {
        return (QString::null);
    }

    TVolumeInfo volumeInfo;
    if (iFs.Volume(volumeInfo, drive) != KErrNone) {
        return (QString::null);
    }
    return (QString::number(volumeInfo.iUniqueID));
}

QSystemStorageInfo::StorageState QSystemStorageInfoPrivate::getStorageState(const QString &Volume)
{
   return CheckDiskSpaceThresholdLimit(Volume);
}

QSystemStorageInfo::StorageState QSystemStorageInfoPrivate::CheckDiskSpaceThresholdLimit(const QString &aDriveVolume)
{
    TInt64 freeSpace(0);
    TInt64 volSize(0);
    TVolumeInfo volInfo;
    TInt drive(0);
    QSystemStorageInfo::StorageState state = QSystemStorageInfo::UnknownStorageState;

    if (RFs::CharToDrive(TChar(aDriveVolume[0].toAscii()), drive) == KErrNone){
        if (iFs.Volume( volInfo, drive) == KErrNone){
            freeSpace = volInfo.iFree;
            volSize = volInfo.iSize;
            }
        }
    else{
        return state;
        }

    // calculate the threshold limits for notification
    TInt64 lowLimit = (TInt64)(volSize * 0.4);
    TInt64 veryLowLimit = (TInt64)(volSize * 0.1);
    TInt64 criticalLimit = (TInt64)(volSize * 0.02);

    if (freeSpace >= lowLimit){
        state = QSystemStorageInfo::NormalStorageState;
        }
    else if (freeSpace < lowLimit && freeSpace >= veryLowLimit){
        state = QSystemStorageInfo::LowStorageState;
        }
    else if (freeSpace < veryLowLimit && freeSpace >= criticalLimit){
        state = QSystemStorageInfo::VeryLowStorageState;
        }
    else if (freeSpace < criticalLimit){
        state = QSystemStorageInfo::CriticalStorageState;
        }
    return state;
}

QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
    : QObject(parent), m_profileEngine(NULL), m_proEngNotifyHandler(NULL),
    m_bluetoothRepository(NULL), m_bluetoothNotifyHandler(NULL)
{
    DeviceInfo::instance()->batteryInfo()->addObserver(this);
    DeviceInfo::instance()->chargingStatus()->addObserver(this);
    m_previousBatteryStatus = QSystemDeviceInfo::NoBatteryLevel;
#ifdef SYMBIAN_3_PLATFORM
    DeviceInfo::instance()->keylockStatus()->addObserver(this);
    DeviceInfo::instance()->flipStatus()->addObserver(this);
#endif
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    DeviceInfo::instance()->chargingStatus()->removeObserver(this);
    DeviceInfo::instance()->batteryInfo()->removeObserver(this);
#ifdef SYMBIAN_3_PLATFORM
    DeviceInfo::instance()->keylockStatus()->removeObserver(this);
    DeviceInfo::instance()->flipStatus()->removeObserver(this);
#endif
    if (m_proEngNotifyHandler) {
        m_proEngNotifyHandler->CancelProfileActivationNotifications();
        delete m_proEngNotifyHandler;
    }

    if (m_profileEngine) {
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
    QSystemDeviceInfo::PowerState currentpwrstate = currentPowerState();
    if (batteryLevel < 15 ) {
        if ( (currentpwrstate == QSystemDeviceInfo::WallPowerChargingBattery) || (currentpwrstate == QSystemDeviceInfo::WallPower) ) {
                return QSystemDeviceInfo::BatteryLow;
        }   else  {
                return QSystemDeviceInfo::BatteryVeryLow;
        }
    }   else if (batteryLevel < 29) {
            return QSystemDeviceInfo::BatteryLow;
    }   else if (batteryLevel < 43) {
            if ( (currentpwrstate == QSystemDeviceInfo::WallPowerChargingBattery) || (currentpwrstate == QSystemDeviceInfo::WallPower) ){
                return QSystemDeviceInfo::BatteryNormal;
            }  else {
                return QSystemDeviceInfo::BatteryLow;
            }
    }   else if (batteryLevel > 42) {
            return QSystemDeviceInfo::BatteryNormal;
    }

    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
#ifdef SYMBIAN_3_1
    if (!DeviceInfo::instance()->subscriberInfo()->imsi().isEmpty())
        return QSystemDeviceInfo::SingleSimAvailable;
#else //SYMBIAN_3_1
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
#endif //SYMBIAN_3_1
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

    QSystemDeviceInfo::BatteryStatus currentstatus(batteryStatus());
    if (currentstatus != m_previousBatteryStatus) {
        emit batteryStatusChanged(currentstatus);
    }
    m_previousBatteryStatus = currentstatus;
}

void QSystemDeviceInfoPrivate::chargingStatusChanged()
{
    emit powerStateChanged(currentPowerState());
}

bool QSystemDeviceInfoPrivate::currentBluetoothPowerState()
{
    TInt btPowerState = EBTPowerOff;
    TRAP_IGNORE(
    CRepository* crep = CRepository::NewLC(KCRUidBluetoothPowerState);
    User::LeaveIfError(crep->Get(KBTPowerState, btPowerState));
    CleanupStack::PopAndDestroy(crep);
    )

    return btPowerState == EBTPowerOn;
}

QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfoPrivate::keyboardTypes()
{
    QSystemDeviceInfo::InputMethodFlags methods = inputMethodType();
    QSystemDeviceInfo::KeyboardTypeFlags keyboardFlags = QSystemDeviceInfo::UnknownKeyboard;

#ifdef SYMBIAN_3_PLATFORM
    TInt kbType = 0;
    if (methods & QSystemDeviceInfo::Keyboard ) {
    TRAP_IGNORE(
            CRepository* repository = CRepository::NewLC( KCRUidAvkon ) ;
            User::LeaveIfError(repository->Get( KAknKeyboardType, kbType));
            CleanupStack::PopAndDestroy(repository);
        )
    switch (kbType){
        case 1: keyboardFlags = QSystemDeviceInfo::ITUKeypad;
            break;
        case 2:
        case 3:
        case 4:
        case 6: keyboardFlags = QSystemDeviceInfo::FullQwertyKeyboard;
                break;
        case 5: keyboardFlags = QSystemDeviceInfo::HalfQwertyKeyboard;
                break;
        default:
                break;
                }
        bool filpKbStatus = DeviceInfo::instance()->flipStatus()->getKeyboardStatus();
        if ( filpKbStatus ) {
             if ( keyboardFlags == QSystemDeviceInfo::UnknownKeyboard)
                keyboardFlags = QSystemDeviceInfo::FlipKeyboard;
             else
                keyboardFlags |= QSystemDeviceInfo::FlipKeyboard;
             }
        }
        kbType=0;
        if ( (methods & QSystemDeviceInfo::SingleTouch) || (methods & QSystemDeviceInfo::MultiTouch) ) {
            //TBD:In 5.0 KAknFepVirtualKeyboardType is not defined
            if ( KErrNone == RProperty::Get(KPSUidAknFep, KAknFepVirtualKeyboardType, kbType) ) {
                if ( 0 !=  kbType) {
                    if ( keyboardFlags == QSystemDeviceInfo::UnknownKeyboard)
                        keyboardFlags = QSystemDeviceInfo::SoftwareKeyboard;
                    else
                        keyboardFlags |= QSystemDeviceInfo::SoftwareKeyboard;
                    }
                }
            }
        if (isWirelessKeyboardConnected()) {
            if ( keyboardFlags == QSystemDeviceInfo::UnknownKeyboard)
                keyboardFlags = QSystemDeviceInfo::WirelessKeyboard;
            else
                keyboardFlags |= QSystemDeviceInfo::WirelessKeyboard;
    }
#endif
    return keyboardFlags;
}

bool QSystemDeviceInfoPrivate::isWirelessKeyboardConnected()
{
    return false;
}

bool QSystemDeviceInfoPrivate::isKeyboardFlippedOpen()
{
#ifdef SYMBIAN_3_PLATFORM
    // It is functional only for the Grip open devices
    return ( (DeviceInfo::instance()->flipStatus()->getFlipStatus()) && (DeviceInfo::instance()->flipStatus()->getKeyboardStatus())  );
#else
    return false;
#endif
}

void QSystemDeviceInfoPrivate::keyboardConnected(bool connect)
{
    if (connect != hasWirelessKeyboardConnected)
        hasWirelessKeyboardConnected = connect;
    Q_EMIT wirelessKeyboardConnected(connect);
}

bool QSystemDeviceInfoPrivate::keypadLightOn(QSystemDeviceInfo::KeypadType type)
{
    CHWRMLight::TLightStatus status = CHWRMLight::ELightStatusUnknown;
    TInt symbianKBType = CHWRMLight::ENoTarget;
    if ( type == QSystemDeviceInfo::PrimaryKeypad )
        symbianKBType = CHWRMLight::EPrimaryKeyboard ;
    else if ( type == QSystemDeviceInfo::SecondaryKeypad )
        symbianKBType = CHWRMLight::ESecondaryKeyboard ;
    TRAP_IGNORE(CHWRMLight* iLight = CHWRMLight::NewL();
                status = iLight->LightStatus(symbianKBType);
                delete iLight;)
    return (status == CHWRMLight::ELightOn);
}

QUuid QSystemDeviceInfoPrivate::uniqueDeviceID()
{
    TInt driveNum = RFs::GetSystemDrive();
    TVolumeInfo volumeInfo;
    RFs rfs;
    if (rfs.Connect()!= KErrNone)
        return 0;
    if (rfs.Volume(volumeInfo, driveNum) != KErrNone)
        return 0;

    QString volInfo(QString::number(volumeInfo.iUniqueID));
    QByteArray bytes = imei().toLocal8Bit();
    QByteArray romDriveUID = volInfo.toLocal8Bit();
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(bytes);
    hash.addData(romDriveUID);
    rfs.Close();
    return QUuid(QString(hash.result().toHex()));
}

QSystemDeviceInfo::LockTypeFlags QSystemDeviceInfoPrivate::lockStatus()
{
    QSystemDeviceInfo::LockTypeFlags status = QSystemDeviceInfo::UnknownLock;

#ifdef SYMBIAN_3_PLATFORM
    int value = DeviceInfo::instance()->keylockStatus()->getLockStatus();
    switch ( value ){
             /*case EKeyguardNotActive:
                status = QSystemDeviceInfo::DeviceUnlocked;
                break;*/
             case EKeyguardLocked:
                status = QSystemDeviceInfo::TouchAndKeyboardLocked;
                break;
             case EKeyguardAutolockEmulation:
                status = QSystemDeviceInfo::PinLocked;
                break;
             default:
                status = QSystemDeviceInfo::UnknownLock;
                break;
             }
#endif
    return status;
}

#ifdef SYMBIAN_3_PLATFORM
void QSystemDeviceInfoPrivate::keylockStatusChanged(TInt aLockType)
{
    if (aLockType == EKeyguardLocked){
        emit lockStatusChanged(QSystemDeviceInfo::TouchAndKeyboardLocked );
        }
    else if (aLockType == EKeyguardAutolockEmulation){
        emit lockStatusChanged(QSystemDeviceInfo::PinLocked );
        }
    else {
        emit lockStatusChanged(QSystemDeviceInfo::UnknownLock );
        }
}

void QSystemDeviceInfoPrivate::flipStatusChanged(TInt aFlipType , TInt aFilpKeyBoard)
{
    emit keyboardFlipped((aFlipType ==  EPSHWRMGripOpen) && (aFilpKeyBoard == 1));
}
#endif

int QSystemDeviceInfoPrivate::messageRingtoneVolume()
{
    return voiceRingtoneVolume();
}

int QSystemDeviceInfoPrivate::voiceRingtoneVolume()
{
    TInt ringingVolume = 0;
    TRAP_IGNORE(
      CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
      User::LeaveIfError( repository->Get( KProEngActiveRingingVolume, ringingVolume ) );
      CleanupStack::PopAndDestroy(repository);
    )
    if (ringingVolume > 0)
        ringingVolume = 100*(ringingVolume-1)/9;// caliberating value 1 - 10 to scale of 0 - 100
    return ringingVolume;
}

bool QSystemDeviceInfoPrivate::vibrationActive()
{
    TInt vibrationState = 0;
    TRAP_IGNORE(
      CRepository* repository = CRepository::NewLC( KCRUidVibraCtrl );
      User::LeaveIfError( repository->Get( KVibraCtrlProfileVibraEnabled, vibrationState ) );
      CleanupStack::PopAndDestroy(repository);
    )
    bool vibrationStateFlag = vibrationState;
    return vibrationStateFlag;
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

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QObject *parent)
: QObject(parent) , m_batteryHWRM(NULL)
{
    m_batteryHWRM = CBatteryHWRM::New();
    bool chargestate,usbstate;
    DeviceInfo::instance()->batteryCommonInfo()->ChargerType(chargestate,usbstate);
    if (chargestate) {
        if (usbstate)
            m_charger = QSystemBatteryInfo::USBCharger;
        else
            m_charger = QSystemBatteryInfo::WallCharger;
    } else
        m_charger = QSystemBatteryInfo::NoCharger;
    m_previousChagrger = m_charger ;
    DeviceInfo::instance()->batteryCommonInfo()->AddObserver(this);
}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{
    DeviceInfo::instance()->batteryCommonInfo()->RemoveObserver(this);
    if (m_batteryHWRM)
    {
        delete(m_batteryHWRM);
        m_batteryHWRM = NULL;
    }
}


QSystemBatteryInfo::ChargerType QSystemBatteryInfoPrivate::chargerType() const
{
    bool chargestate,usbstate;
    DeviceInfo::instance()->batteryCommonInfo()->ChargerType(chargestate,usbstate);
    QSystemBatteryInfo::ChargerType result = QSystemBatteryInfo::UnknownCharger;
    if (chargestate) {
        if (usbstate)
            result = QSystemBatteryInfo::USBCharger;
        else
            result = QSystemBatteryInfo::WallCharger;
    } else
        result = QSystemBatteryInfo::NoCharger;
    return result;
}

QSystemBatteryInfo::ChargingState QSystemBatteryInfoPrivate::chargingState() const
{
    bool batteryChargingState = DeviceInfo::instance()->batteryCommonInfo()->ChargingState();
    QSystemBatteryInfo::ChargingState result = QSystemBatteryInfo::ChargingError;
    if (batteryChargingState)
        result = QSystemBatteryInfo::Charging;
    else
        result = QSystemBatteryInfo::NotCharging;
    return result;
}


int QSystemBatteryInfoPrivate::nominalCapacity() const
{
    return DeviceInfo::instance()->batteryCommonInfo()->NominalCapacity();
}

int QSystemBatteryInfoPrivate::remainingCapacityPercent() const
{
    return DeviceInfo::instance()->batteryCommonInfo()->RemainingCapacityPercent();
}

int QSystemBatteryInfoPrivate::remainingCapacity() const
{
    return DeviceInfo::instance()->batteryCommonInfo()->RemainingCapacity();
}


int QSystemBatteryInfoPrivate::voltage() const
{
    return DeviceInfo::instance()->batteryCommonInfo()->Voltage();
}

int QSystemBatteryInfoPrivate::remainingChargingTime() const
{
    return 0;
}

int QSystemBatteryInfoPrivate::currentFlow() const
{
    if ( m_batteryHWRM )
        return m_batteryHWRM->GetAvergaeCurrent( );
    else
        return -1;
}

int QSystemBatteryInfoPrivate::remainingCapacityBars() const
{
    return DeviceInfo::instance()->batteryCommonInfo()->RemainingCapacityBars();
}

int QSystemBatteryInfoPrivate::maxBars() const
{
    return KMaxBatteryBars ;
}

QSystemBatteryInfo::BatteryStatus QSystemBatteryInfoPrivate::batteryStatus() const
{
    QString logString;
    int batteryStatusVal = DeviceInfo::instance()->batteryCommonInfo()->BatteryStatus();
    QSystemBatteryInfo::BatteryStatus result = QSystemBatteryInfo::BatteryUnknown;
    switch (batteryStatusVal) {
                case EBatteryStatusOk : result = QSystemBatteryInfo:: BatteryOk ;
                        logString = "EBatteryStatusOk";
                        break;
                case EBatteryStatusLow : result = QSystemBatteryInfo::BatteryLow ;
                        logString = "EBatteryStatusLow";
                        break;
                case EBatteryStatusEmpty : result = QSystemBatteryInfo::BatteryEmpty ;
                        logString = "EBatteryStatusEmpty";
                        break;
                case EBatteryStatusUnknown :
                default : result = QSystemBatteryInfo::BatteryUnknown;
                        logString = "EBatteryStatusUnknown";
                        break;
    }
    return result;
}

void QSystemBatteryInfoPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargingStateChanged(QSystemBatteryInfo::ChargingState))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargerTypeChanged(QSystemBatteryInfo::ChargerType))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            nominalCapacityChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityPercentChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentFlowChanged(int))))) {
            if ( m_batteryHWRM ) {
                m_batteryHWRM->StartMeasurementAndSetObserver( this);
            }
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityBarsChanged(int))))) {
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingChargingTimeChanged(int))))) {
    }
}

void QSystemBatteryInfoPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
                batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))))) {
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentFlowChanged(int))))) {
            if ( m_batteryHWRM ) {
                m_batteryHWRM->StopCurrentFlowMeasurement( );
            }
    }
}

QSystemBatteryInfo::EnergyUnit QSystemBatteryInfoPrivate::energyMeasurementUnit() const
{
    return QSystemBatteryInfo::UnitmAh;

}

void QSystemBatteryInfoPrivate::changedBatteryStatus() {
    emit batteryStatusChanged(batteryStatus());
}

void QSystemBatteryInfoPrivate::changedChargingState() {
    emit chargingStateChanged(chargingState());
}

void QSystemBatteryInfoPrivate::changedChargerType() {
    bool chargestate,usbstate;
    DeviceInfo::instance()->batteryCommonInfo()->ChargerType(chargestate,usbstate);
    if (chargestate) {
        if (usbstate)
            m_charger = QSystemBatteryInfo::USBCharger;
        else
            m_charger = QSystemBatteryInfo::WallCharger;
    } else
        m_charger = QSystemBatteryInfo::NoCharger;
    if (m_previousChagrger != m_charger ) {
        emit chargerTypeChanged(m_charger);
        m_previousChagrger = m_charger ;
    }
}

void QSystemBatteryInfoPrivate::changedRemainingCapacityBars() {
    emit remainingCapacityBarsChanged(remainingCapacityBars());
}

void QSystemBatteryInfoPrivate::changedCurrentFlow(int value)  {
    emit currentFlowChanged(value);
}

#include "moc_qsysteminfo_s60_p.cpp"

QTM_END_NAMESPACE
