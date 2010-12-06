/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subatteryStatusidiary(-ies).
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
#include "qsysteminfo_win_p.h"
#include <qt_windows.h>

#include <QtCore/qmutex.h>

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
#include <QTimer>
#include <QBasicTimer>
#include <QtCore/qlibrary.h>
#include <QUuid>
#include <QSysInfo>

#include <qabstracteventdispatcher.h>

#include <locale.h>
#ifdef Q_OS_WINCE
#include <simmgr.h>
#include <Winbase.h>
#include <Winuser.h>
#include <Pm.h>
#endif
#include <HighLevelMonitorConfigurationAPI.h>

#include <PowrProf.h>
#include <Setupapi.h>

#include <BatClass.h>

#if !defined( Q_CC_MINGW)
#ifndef Q_OS_WINCE
#include "windows/qwmihelper_win_p.h"



enum NDIS_MEDIUM {
    NdisMedium802_3 = 0,
    NdisMediumWirelessWan = 9,
};

enum NDIS_PHYSICAL_MEDIUM {
    NdisPhysicalMediumUnspecified = 0,
    NdisPhysicalMediumWirelessLan = 1,
    NdisPhysicalMediumBluetooth = 10,
    NdisPhysicalMediumWiMax = 12,
};

#define OID_GEN_MEDIA_SUPPORTED 0x00010103
#define OID_GEN_PHYSICAL_MEDIUM 0x00010202

#define IOCTL_NDIS_QUERY_GLOBAL_STATS \
    CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, 0, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#endif
#endif



#define WLAN_MAX_NAME_LENGTH 256
#define DOT11_SSID_MAX_LENGTH 32
#define WLAN_NOTIFICATION_SOURCE_ALL 0x0000ffff

enum WLAN_INTF_OPCODE {
    wlan_intf_opcode_autoconf_start = 0x000000000,
    wlan_intf_opcode_autoconf_enabled,
    wlan_intf_opcode_background_scan_enabled,
    wlan_intf_opcode_media_streaming_mode,
    wlan_intf_opcode_radio_state,
    wlan_intf_opcode_batteryStatuss_type,
    wlan_intf_opcode_interface_state,
    wlan_intf_opcode_current_connection,
    wlan_intf_opcode_channel_number,
    wlan_intf_opcode_supported_infrastructure_auth_cipher_pairs,
    wlan_intf_opcode_supported_adhoc_auth_cipher_pairs,
    wlan_intf_opcode_supported_country_or_region_string_list,
    wlan_intf_opcode_current_operation_mode,
    wlan_intf_opcode_supported_safe_mode,
    wlan_intf_opcode_certified_safe_mode,
    wlan_intf_opcode_autoconf_end = 0x0fffffff,
    wlan_intf_opcode_msm_start = 0x10000100,
    wlan_intf_opcode_statistics,
    wlan_intf_opcode_rssi,
    wlan_intf_opcode_msm_end = 0x1fffffff,
    wlan_intf_opcode_security_start = 0x20010000,
    wlan_intf_opcode_security_end = 0x2fffffff,
    wlan_intf_opcode_ihv_start = 0x30000000,
    wlan_intf_opcode_ihv_end = 0x3fffffff
};

enum WLAN_OPCODE_VALUE_TYPE {
    wlan_opcode_value_type_query_only = 0,
    wlan_opcode_value_type_set_by_group_policy,
    wlan_opcode_value_type_set_by_user,
    wlan_opcode_value_type_invalid
};

enum WLAN_INTERFACE_STATE {
    wlan_interface_state_not_ready = 0,
    wlan_interface_state_connected,
    wlan_interface_state_ad_hoc_network_formed,
    wlan_interface_state_disconnecting,
    wlan_interface_state_disconnected,
    wlan_interface_state_associating,
    wlan_interface_state_discovering,
    wlan_interface_state_authenticating
};

struct WLAN_INTERFACE_INFO {
    GUID InterfaceGuid;
    WCHAR strInterfaceDescription[WLAN_MAX_NAME_LENGTH];
    WLAN_INTERFACE_STATE isState;
};

struct WLAN_INTERFACE_INFO_LIST {
    DWORD dwNumberOfItems;
    DWORD dwIndex;
    WLAN_INTERFACE_INFO InterfaceInfo[1];
};

struct WLAN_NOTIFICATION_DATA {
    DWORD NotificationSource;
    DWORD NotificationCode;
    GUID InterfaceGuid;
    DWORD dwDataSize;
    PVOID pData;
};

enum WLAN_CONNECTION_MODE {
    wlan_connection_mode_profile = 0,
    wlan_connection_mode_temporary_profile,
    wlan_connection_mode_discovery_secure,
    wlan_connection_mode_discovery_unsecure,
    wlan_connection_mode_auto,
    wlan_connection_mode_invalid
};

enum DOT11_PHY_TYPE {
        dot11_phy_type_unknown = 0,
        dot11_phy_type_any = dot11_phy_type_unknown,
        dot11_phy_type_fhss = 1,
        dot11_phy_type_dsss = 2,
        dot11_phy_type_irbaseband = 3,
        dot11_phy_type_ofdm = 4,
        dot11_phy_type_hrdsss = 5,
        dot11_phy_type_erp = 6,
        dot11_phy_type_ht = 7,
        dot11_phy_type_IHV_start = 0x80000000,
        dot11_phy_type_IHV_end = 0xffffffff
    };


enum DOT11_AUTH_ALGORITHM {
    DOT11_AUTH_ALGO_80211_OPEN = 1,
    DOT11_AUTH_ALGO_80211_SHARED_KEY = 2,
    DOT11_AUTH_ALGO_WPA = 3,
    DOT11_AUTH_ALGO_WPA_PSK = 4,
    DOT11_AUTH_ALGO_WPA_NONE = 5,
    DOT11_AUTH_ALGO_RSNA = 6,
    DOT11_AUTH_ALGO_RSNA_PSK = 7,
    DOT11_AUTH_ALGO_IHV_START = 0x80000000,
    DOT11_AUTH_ALGO_IHV_END = 0xffffffff
};


enum DOT11_CIPHER_ALGORITHM {
    DOT11_CIPHER_ALGO_NONE = 0x00,
    DOT11_CIPHER_ALGO_WEP40 = 0x01,
    DOT11_CIPHER_ALGO_TKIP = 0x02,
    DOT11_CIPHER_ALGO_CCMP = 0x04,
    DOT11_CIPHER_ALGO_WEP104 = 0x05,
    DOT11_CIPHER_ALGO_WPA_USE_GROUP = 0x100,
    DOT11_CIPHER_ALGO_RSN_USE_GROUP = 0x100,
    DOT11_CIPHER_ALGO_WEP = 0x101,
    DOT11_CIPHER_ALGO_IHV_START = 0x80000000,
    DOT11_CIPHER_ALGO_IHV_END = 0xffffffff
};


enum DOT11_batteryStatusS_TYPE {
    dot11_batteryStatusS_type_infrastructure = 1,
    dot11_batteryStatusS_type_independent = 2,
    dot11_batteryStatusS_type_any = 3
};


struct DOT11_SSID {
    ULONG uSSIDLength;
    UCHAR ucSSID[DOT11_SSID_MAX_LENGTH];
};

typedef UCHAR DOT11_MAC_ADDRESS[6];

struct WLAN_ASSOCIATION_ATTRIBUTES {
    DOT11_SSID dot11Ssid;
    DOT11_batteryStatusS_TYPE dot11batteryStatussType;
    DOT11_MAC_ADDRESS dot11batteryStatussid;
    DOT11_PHY_TYPE dot11PhyType;
    ULONG uDot11PhyIndex;
    ULONG wlanSignalQuality;
    ULONG ulRxRate;
    ULONG ulTxRate;
};

struct WLAN_SECURITY_ATTRIBUTES {
    BOOL batteryStatusecurityEnabled;
    BOOL bOneXEnabled;
    DOT11_AUTH_ALGORITHM dot11AuthAlgorithm;
    DOT11_CIPHER_ALGORITHM dot11CipherAlgorithm;
};

struct WLAN_CONNECTION_ATTRIBUTES {
    WLAN_INTERFACE_STATE isState;
    WLAN_CONNECTION_MODE wlanConnectionMode;
    WCHAR strProfileName[WLAN_MAX_NAME_LENGTH];
    WLAN_ASSOCIATION_ATTRIBUTES wlanAssociationAttributes;
    WLAN_SECURITY_ATTRIBUTES wlanSecurityAttributes;
};

enum WLAN_NOTIFICATION_ACM {
    wlan_notification_acm_start = 0,
    wlan_notification_acm_autoconf_enabled,
    wlan_notification_acm_autoconf_disabled,
    wlan_notification_acm_background_scan_enabled,
    wlan_notification_acm_background_scan_disabled,
    wlan_notification_acm_batteryStatuss_type_change,
    wlan_notification_acm_power_setting_change,
    wlan_notification_acm_scan_complete,
    wlan_notification_acm_scan_fail,
    wlan_notification_acm_connection_start,
    wlan_notification_acm_connection_complete,
    wlan_notification_acm_connection_attempt_fail,
    wlan_notification_acm_filter_list_change,
    wlan_notification_acm_interface_arrival,
    wlan_notification_acm_interface_removal,
    wlan_notification_acm_profile_change,
    wlan_notification_acm_profile_name_change,
    wlan_notification_acm_profiles_exhausted,
    wlan_notification_acm_network_not_available,
    wlan_notification_acm_network_available,
    wlan_notification_acm_disconnecting,
    wlan_notification_acm_disconnected,
    wlan_notification_acm_adhoc_network_state_change,
    wlan_notification_acm_end
};

enum _WLAN_NOTIFICATION_MSM {
    wlan_notification_msm_start = 0,
    wlan_notification_msm_associating,
    wlan_notification_msm_associated,
    wlan_notification_msm_authenticating,
    wlan_notification_msm_connected,
    wlan_notification_msm_roaming_start,
    wlan_notification_msm_roaming_end,
    wlan_notification_msm_radio_state_change,
    wlan_notification_msm_signal_quality_change,
    wlan_notification_msm_disassociating,
    wlan_notification_msm_disconnected,
    wlan_notification_msm_peer_join,
    wlan_notification_msm_peer_leave,
    wlan_notification_msm_adapter_removal,
    wlan_notification_msm_adapter_operation_mode_change,
    wlan_notification_msm_end
};

typedef void (WINAPI *WLAN_NOTIFICATION_CALLBACK) (WLAN_NOTIFICATION_DATA *, PVOID);

#define _WCHAR_T_DEFINED
#define _TIME64_T_DEFINED

typedef struct _BLUETOOTH_FIND_RADIO_PARAMS {
    DWORD   dwSize;             //  IN  sizeof this structure
} BLUETOOTH_FIND_RADIO_PARAMS;


typedef DWORD (WINAPI *WlanOpenHandleProto)
    (DWORD dwClientVersion, PVOID pReserved, PDWORD pdwNegotiatedVersion, PHANDLE phClientHandle);
static WlanOpenHandleProto local_WlanOpenHandle = 0;

typedef DWORD (WINAPI *WlanCloseHandleProto)(HANDLE hClientHandle, PVOID pReserved);
static WlanCloseHandleProto local_WlanCloseHandle = 0;

#if !defined(Q_OS_WINCE)
typedef DWORD (WINAPI *WlanEnumInterfacesProto)
    (HANDLE hClientHandle, PVOID pReserved, WLAN_INTERFACE_INFO_LIST **ppInterfaceList);
static WlanEnumInterfacesProto local_WlanEnumInterfaces = 0;

typedef DWORD (WINAPI *WlanQueryInterfaceProto)
    (HANDLE hClientHandle, const GUID *pInterfaceGuid, WLAN_INTF_OPCODE OpCode, PVOID pReserved,
     PDWORD pdwDataSize, PVOID *ppData, WLAN_OPCODE_VALUE_TYPE *pWlanOpcodeValueType);
static WlanQueryInterfaceProto local_WlanQueryInterface = 0;

typedef DWORD (WINAPI *WlanRegisterNotificationProto)
    (HANDLE hClientHandle, DWORD dwNotifSource, BOOL bIgnoreDuplicate,
     WLAN_NOTIFICATION_CALLBACK funcCallback, PVOID pCallbackContext,
     PVOID pReserved, PDWORD pdwPrevNotifSource);
static WlanRegisterNotificationProto local_WlanRegisterNotification = 0;
#endif

typedef VOID (WINAPI *WlanFreeMemoryProto)(PVOID pMemory);
static WlanFreeMemoryProto local_WlanFreeMemory = 0;

typedef BOOL (WINAPI *BluetoothFindRadioClose)(HANDLE hFind);
static BluetoothFindRadioClose local_BluetoothFindRadioClose=0;

typedef HANDLE (WINAPI *BluetoothFindFirstRadio)(const BLUETOOTH_FIND_RADIO_PARAMS * pbtfrp,HANDLE * phRadio);
static BluetoothFindFirstRadio local_BluetoothFindFirstRadio=0;

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, dynamicLoadMutex, (QMutex::Recursive));

static void resolveLibrary()
{
#if !defined( Q_OS_WINCE)
            static volatile bool triedResolve = false;

    if (!triedResolve) {
#ifndef QT_NO_THREAD
        QMutexLocker locker(dynamicLoadMutex());
#endif

        if (!triedResolve) {
            local_WlanOpenHandle = (WlanOpenHandleProto)
                                   QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanOpenHandle");
            local_WlanRegisterNotification = (WlanRegisterNotificationProto)
                                             QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanRegisterNotification");
            local_WlanEnumInterfaces = (WlanEnumInterfacesProto)
                                       QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanEnumInterfaces");
            local_WlanQueryInterface = (WlanQueryInterfaceProto)
                                       QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanQueryInterface");
            local_WlanFreeMemory = (WlanFreeMemoryProto)
                                   QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanFreeMemory");
            local_WlanCloseHandle = (WlanCloseHandleProto)
                                    QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanCloseHandle");


            local_BluetoothFindFirstRadio = (BluetoothFindFirstRadio)
                                            QLibrary::resolve(QLatin1String("Bthprops.cpl"), "BluetoothFindFirstRadio");
            local_BluetoothFindRadioClose = (BluetoothFindRadioClose)
                                            QLibrary::resolve(QLatin1String("Bthprops.cpl"), "BluetoothFindRadioClose");

            triedResolve = true;
        }
    }
#endif
}

Q_GLOBAL_STATIC(QSystemNetworkInfoPrivate, qsystemNetworkInfoPrivate)

typedef struct _DISPLAY_BRIGHTNESS {
    UCHAR ucDisplayPolicy;
    UCHAR ucACBrightness;
    UCHAR ucDCBrightness;
} DISPLAY_BRIGHTNESS, *PDISPLAY_BRIGHTNESS;


#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
static WLAN_CONNECTION_ATTRIBUTES *getWifiConnectionAttributes()
{
    if(!local_WlanOpenHandle)
        return NULL;
    DWORD version =  0;
    HANDLE clientHandle = NULL;
    DWORD result;
    WLAN_CONNECTION_ATTRIBUTES  *connAtts = NULL;

    WLAN_INTERFACE_INFO_LIST *interfacesInfoList = NULL;
    result = local_WlanOpenHandle( 2, NULL, &version, &clientHandle );
    if( result != ERROR_SUCCESS) {
        local_WlanFreeMemory(connAtts);
        local_WlanCloseHandle(clientHandle,  0);
        return NULL;
    }
    result = local_WlanEnumInterfaces(clientHandle, NULL, &interfacesInfoList);

    if( result != ERROR_SUCCESS) {
        local_WlanCloseHandle(clientHandle, NULL);
        local_WlanFreeMemory(connAtts);
        return NULL;
    }

    for( uint i = 0; i < interfacesInfoList->dwNumberOfItems; i++ ) {
        WLAN_INTERFACE_INFO *interfaceInfo = &interfacesInfoList->InterfaceInfo[i];
        GUID& guid = interfaceInfo->InterfaceGuid;
        WLAN_INTERFACE_STATE wlanInterfaceState = interfaceInfo->isState;

        if( wlanInterfaceState == wlan_interface_state_not_ready ) {
            continue;
        }

        ULONG size = 0;
        result = local_WlanQueryInterface( clientHandle, &guid,  wlan_intf_opcode_current_connection, NULL, &size, (PVOID*) &connAtts, NULL );
        if( result != ERROR_SUCCESS ) {
            continue;
        }
    }
    local_WlanCloseHandle(clientHandle, NULL);
    return connAtts;
}
#endif

QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
    currentLanguageStr = currentLanguage();
    QTimer::singleShot(1000, this,SLOT(currentLanguageTimeout()));
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{

}

void QSystemInfoPrivate::currentLanguageTimeout()
{
    const QString tmpLang = currentLanguage();
    if(currentLanguageStr != tmpLang) {
        currentLanguageStr = tmpLang;
        emit currentLanguageChanged(currentLanguageStr);
    }

    QTimer::singleShot(1000, this,SLOT(currentLanguageTimeout()));
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
    QDir transDir(QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    QStringList langList;
    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << "qt_*.qm" ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach(const QString localeName, localeList) {
            const QString lang = localeName.mid(3,2);
            if(!langList.contains(lang) && !lang.isEmpty() && !lang.contains("help")) {
                langList << lang;
            }
        }
        if(!langList.isEmpty()) {
            return langList;
        }
    }
    return QStringList() << currentLanguage();
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Available";
    bool useDate = false;
    if(parameter == "versionDate") {
        useDate = true;
    }
    switch(type) {
    case QSystemInfo::Os :
        {
#if !defined(Q_OS_WINCE)
            OSVERSIONINFOEX versionInfo;
            versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

            GetVersionEx((OSVERSIONINFO *) &versionInfo);
            return QString::number(versionInfo.dwMajorVersion) +"."
                    +QString::number(versionInfo.dwMinorVersion)+"."
                    +QString::number(versionInfo.dwBuildNumber)+"."
                    +QString::number(versionInfo.wServicePackMajor)+"."
                    +QString::number(versionInfo.wServicePackMinor);
#else
            OSVERSIONINFO versionInfo;
            versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

            GetVersionEx((OSVERSIONINFO *) &versionInfo);
            return QString::number(versionInfo.dwMajorVersion) +"."
                    +QString::number(versionInfo.dwMinorVersion)+"."
                    +QString::number(versionInfo.dwBuildNumber);
#endif
        }
        break;
    case QSystemInfo::QtCore :
       return  qVersion();
       break;
   case QSystemInfo::Firmware :
       {
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
    case QSystemInfo::BluetoothFeature :
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)

            resolveLibrary();
            if(local_BluetoothFindFirstRadio == 0 ) {
                qDebug() << "Bluetooth library could not resolve or be loaded";
                return false;
            }
            BLUETOOTH_FIND_RADIO_PARAMS  radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
            HANDLE radio;
            if(local_BluetoothFindFirstRadio(&radioParams, &radio) != NULL) {
                featureSupported = true;
                local_BluetoothFindRadioClose(radio);
            } else {

            }
#endif
        }
        break;
    case QSystemInfo::CameraFeature :
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
#endif
        }
        break;
    case QSystemInfo::FmradioFeature :
        {
            //PageId.FMRadio?
        }
        break;
    case QSystemInfo::IrFeature :
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_InfraredDevice");
            wHelper->setClassProperty(QStringList() << "ConfigManagerErrorCode");

            QVariant v = wHelper->getWMIData();
            if(v.toUInt() == 1) {
                featureSupported = true;
            }
#endif
        }
        break;
    case QSystemInfo::LedFeature :
        {
#ifdef Q_OS_WINCE

#else
#endif
        }
        break;
    case QSystemInfo::MemcardFeature :
        {
            QSystemStorageInfo mi;
            QStringList drives = mi.logicalDrives();
            foreach(const QString drive, drives) {
                if(mi.typeForDrive(drive) == QSystemStorageInfo::RemovableDrive) {
                    featureSupported = true;
                }
            }
        }
        break;
    case QSystemInfo::UsbFeature :
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_USBHub");
            wHelper->setClassProperty(QStringList() << "ConfigManagerErrorCode");
            QVariant v = wHelper->getWMIData();
            if(v.toUInt() == 0) {
                featureSupported = true;
            }
#endif
        }
        break;
    case QSystemInfo::VibFeature :
        {
#ifdef Q_OS_WINCE

#else
#endif
        }
        break;
    case QSystemInfo::WlanFeature :
        {
            QSystemNetworkInfo ni;
            if(ni.interfaceForMode(QSystemNetworkInfo::WlanMode).isValid()) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::SimFeature :
        {
#ifdef Q_OS_WINCE
            HSIM handle;
            HRESULT result = SimInitialize(0,NULL,NULL,&handle);
            if(result == S_OK) {
                featureSupported = true;
                SimDeinitialize(handle);
            }
#else
#endif
        }
        break;
    case QSystemInfo::LocationFeature :
        {
#ifdef Q_OS_WINCE

#else
#endif
        }
        break;
    case QSystemInfo::VideoOutFeature :
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("VideoModeDescriptor");
            wHelper->setClassProperty(QStringList() << "VideoStandardType");
            QVariant v = wHelper->getWMIData();
            if(v.toUInt() > 5) {
                featureSupported = true;
            }
#endif
        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
     case QSystemInfo::FmTransmitterFeature:
     default:
        featureSupported = false;
        break;
    };
    return featureSupported;
}

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo)::NetworkMode)
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QSystemNetworkInfo)::NetworkStatus)
QTM_BEGIN_NAMESPACE

#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)

void wlanNotificationCallback(WLAN_NOTIFICATION_DATA *pNotifyData, QSystemNetworkInfoPrivate *netInfo)
{
    // xp only supports disconnected and complete
    switch(pNotifyData->NotificationCode) {
    case wlan_notification_acm_scan_complete:
        break;
    case wlan_notification_acm_disconnecting:
        break;
    case wlan_notification_acm_disconnected:
    case wlan_notification_acm_connection_attempt_fail:
    case wlan_notification_acm_network_not_available:
        netInfo->emitNetworkStatusChanged(QSystemNetworkInfo::WlanMode,
                                          QSystemNetworkInfo::NoNetworkAvailable);
        break;
    case wlan_notification_acm_profile_name_change:
        break;
    case  wlan_notification_msm_connected:
        netInfo->emitNetworkStatusChanged(QSystemNetworkInfo::WlanMode,
                                          QSystemNetworkInfo::Connected);
        break;
    case wlan_notification_acm_connection_start:
        netInfo->emitNetworkStatusChanged(QSystemNetworkInfo::WlanMode,
                                          QSystemNetworkInfo::Searching);
        break;
    case wlan_notification_msm_signal_quality_change:
        netInfo->emitNetworkSignalStrengthChanged(QSystemNetworkInfo::WlanMode,
                                                  reinterpret_cast<qint32>(pNotifyData->pData));
        break;
    default:
        break;
    };
}
#endif

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
    : QObject(parent)
{
    wlanCallbackInitialized = false;
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");
    resolveLibrary();

    startWifiCallback();

     timerMs = 5000;
     switch(QSysInfo::WindowsVersion) {
     case  QSysInfo::WV_VISTA:
     case QSysInfo::WV_WINDOWS7:
         break;
     default:
         {
             if(local_WlanOpenHandle)
                 QTimer::singleShot(timerMs, this, SLOT(networkStrengthTimeout()));
         }
         break;
     };
 }

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
#if !defined( Q_OS_WINCE)
    if(hWlan != 0)
         local_WlanCloseHandle(hWlan, 0);
#endif
}

void QSystemNetworkInfoPrivate::startWifiCallback()
{
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
    if(networkStatus(QSystemNetworkInfo::WlanMode) != QSystemNetworkInfo::Connected
    && wlanCallbackInitialized){
        return;
    }
    DWORD version =  0;
    hWlan = NULL;
    DWORD result;
    if(local_WlanOpenHandle)
        result = local_WlanOpenHandle(2, NULL, &version, &hWlan );
    if( result != ERROR_SUCCESS ) {
        qDebug() << "Error opening Wlanapi 2" << result ;
        return ;
    }
    if( result != ERROR_SUCCESS) {
        qDebug() << "Error in enumerating wireless interfaces" << result;
        return ;
    }
    if (ERROR_SUCCESS != local_WlanRegisterNotification(hWlan,
                                                  WLAN_NOTIFICATION_SOURCE_ALL,
                                                  true,
                                                  WLAN_NOTIFICATION_CALLBACK(wlanNotificationCallback),
                                                  this, 0, 0)) {
        qDebug() << "failed";
    } else {
        wlanCallbackInitialized = true;
    }
#endif
}


QSystemNetworkInfoPrivate *QSystemNetworkInfoPrivate::instance()
{
    resolveLibrary();
    return qsystemNetworkInfoPrivate();
}

void QSystemNetworkInfoPrivate::emitNetworkStatusChanged(QSystemNetworkInfo::NetworkMode mode,
                                                         QSystemNetworkInfo::NetworkStatus status)
{
    emit networkStatusChanged(mode, status);
    if (status == QSystemNetworkInfo::NoNetworkAvailable
        || status == QSystemNetworkInfo::Connected) {
        startWifiCallback();
        emit networkNameChanged(mode, networkName(mode));
        if(isDefaultMode(mode)) {
            emit networkModeChanged(mode);
        }
    } else {
        networkSignalStrengthChanged(mode, 0);
    }
}

void QSystemNetworkInfoPrivate::emitNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode,int /*strength*/)
{
    switch(QSysInfo::WindowsVersion) {
    case QSysInfo::WV_VISTA:
    case QSysInfo::WV_WINDOWS7:
        break;
    default:
        QTimer::singleShot(timerMs, this, SLOT(networkStrengthTimeout()));
        break;
    };
    networkSignalStrength(mode);
}

void QSystemNetworkInfoPrivate::timerEvent(QTimerEvent *event)
{
   if (event->timerId() == netStrengthTimer.timerId()) {
      networkStrengthTimeout();
   } else {
      QObject::timerEvent(event);
   }
}


void QSystemNetworkInfoPrivate::networkStrengthTimeout()
{
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(const QSystemNetworkInfo::NetworkMode mode, modeList) {
        networkSignalStrength(mode);
    }
    switch(QSysInfo::WindowsVersion) {
    case QSysInfo::WV_VISTA:
    case QSysInfo::WV_WINDOWS7:
        break;
    default:
        if(local_WlanOpenHandle)
            QTimer::singleShot(timerMs, this, SLOT(networkStrengthTimeout()));
        break;
    };
 }

void QSystemNetworkInfoPrivate::networkStatusTimeout()
{
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(const QSystemNetworkInfo::NetworkMode mode, modeList) {
        networkStatus(mode);
    }

 }


QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
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
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            if(local_WlanOpenHandle) {
                WLAN_CONNECTION_ATTRIBUTES *connAtts = getWifiConnectionAttributes();
                if(connAtts != NULL) {
                    if(connAtts->isState  == wlan_interface_state_authenticating) {
                        local_WlanFreeMemory(connAtts);
                        return QSystemNetworkInfo::Searching;
                    }
                    if(connAtts->isState  == wlan_interface_state_connected) {
                        local_WlanFreeMemory(connAtts);
                        return QSystemNetworkInfo::Connected;
                    }
                }
                local_WlanFreeMemory(connAtts);
            }
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_NetworkAdapter");
            wHelper->setClassProperty(QStringList() << "NetConnectionStatus");
            QString cond;
            cond = QString("WHERE MACAddress = '%1'").arg( interfaceForMode(mode).hardwareAddress());
            wHelper->setConditional(cond.toLatin1());
            QVariant v = wHelper->getWMIData();
            if(v.toUInt() == 2) {
                return QSystemNetworkInfo::Connected;
            }
#endif
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_NetworkAdapter");
            wHelper->setClassProperty(QStringList() << "NetConnectionStatus");
            QString cond;
            cond = QString("WHERE MACAddress = '%1'").arg( interfaceForMode(mode).hardwareAddress());
            wHelper->setConditional(cond.toLatin1());
            QVariant v = wHelper->getWMIData();
            if(v.toUInt() == 2) {
                return QSystemNetworkInfo::Connected;
            }
#endif
        }
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return QSystemNetworkInfo::NoNetworkAvailable;
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

#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            if(local_WlanOpenHandle) {

                DWORD version =  0;
                DWORD result;

                WLAN_INTERFACE_INFO_LIST *interfacesInfoList = NULL;
                if (hWlan ==0) {
                    result = local_WlanOpenHandle( 2, NULL, &version, &hWlan );
                    if( result != ERROR_SUCCESS ) {
                        qDebug() << "Error opening Wlanapi 3" << result ;
                        local_WlanCloseHandle(hWlan,  0);
                        return 0;
                    }
                }
                result = local_WlanEnumInterfaces(hWlan, NULL, &interfacesInfoList);

                if( result != ERROR_SUCCESS) {
                    qDebug() << "Error in enumerating wireless interfaces" << result;
                    local_WlanCloseHandle(hWlan,  0);
                    return 0;
                }

                for( uint i = 0; i < interfacesInfoList->dwNumberOfItems; i++ ) {
                    WLAN_INTERFACE_INFO *interfaceInfo = &interfacesInfoList->InterfaceInfo[i];
                    GUID& guid = interfaceInfo->InterfaceGuid;
                    WLAN_INTERFACE_STATE wlanInterfaceState = interfaceInfo->isState;

                    if( wlanInterfaceState == wlan_interface_state_not_ready ) {
                        qDebug() << "Interface not ready";
                        continue;
                    }

                    ULONG size = 0;
                    WLAN_CONNECTION_ATTRIBUTES  *connAtts = NULL;
                    result = local_WlanQueryInterface( hWlan, &guid,  wlan_intf_opcode_current_connection, NULL, &size, (PVOID*) &connAtts, NULL );

                    if( result != ERROR_SUCCESS ) {
                        continue;
                    }
                    ulong sig =  connAtts->wlanAssociationAttributes.wlanSignalQuality;
                    local_WlanFreeMemory(connAtts);
                    local_WlanFreeMemory(interfacesInfoList);

                    if (sig != wifiStrength) {
                        emit networkSignalStrengthChanged(mode, sig);
                        wifiStrength = sig;
                    }
                    return sig;
                }
            }
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_NetworkAdapter");
            wHelper->setClassProperty(QStringList() << "NetConnectionStatus");
            QString cond;
            cond = QString("WHERE MACAddress = '%1'").arg( interfaceForMode(mode).hardwareAddress());
            wHelper->setConditional(cond.toLatin1());
            QVariant v = wHelper->getWMIData();
            quint32 strength = v.toUInt();
            quint32 tmpStrength;

            if( strength == 2
               || strength == 9) {
                tmpStrength = 100;
            } else {
               tmpStrength = 0;
            }

            if(tmpStrength != ethStrength) {
                ethStrength = tmpStrength;
                emit networkSignalStrengthChanged(mode, ethStrength);
            }

            return ethStrength;
#endif
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        break;
    case QSystemNetworkInfo::WimaxMode:
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
    return QString();
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString netname = "";
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            if(local_WlanOpenHandle) {
                netname = "";
                WLAN_CONNECTION_ATTRIBUTES *connAtts = getWifiConnectionAttributes();
                if(connAtts != NULL) {
                    DOT11_SSID ssid;
                    ssid = connAtts->wlanAssociationAttributes.dot11Ssid;
                    for(uint i = 0; i < ssid.uSSIDLength;i++) {
                        netname += ssid.ucSSID[i];
                    }
                }
                local_WlanFreeMemory(connAtts);
            }
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_ComputerSystem");
            wHelper->setClassProperty(QStringList() << "Domain");

            QVariant v = wHelper->getWMIData();
            netname = v.toString();
#endif
        }
        break;
    default:
        break;
    };
    return netname;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    QString mac = interfaceForMode(mode).hardwareAddress();
    return mac;
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    QList<QNetworkInterface> interfaceList;
    interfaceList = QNetworkInterface::allInterfaces();
    QListIterator<QNetworkInterface> i(interfaceList);
    while(i.hasNext()) {
       QNetworkInterface netInterface = i.next();
        if (!netInterface.isValid()
            || (netInterface.flags() & QNetworkInterface::IsLoopBack)
            || !(netInterface.flags() & QNetworkInterface::IsUp)
            || !(netInterface.flags() & QNetworkInterface::IsRunning)
            || netInterface.addressEntries().isEmpty()) {
            continue;
        }

#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)

        if(local_WlanOpenHandle) {
            unsigned long oid;
            DWORD bytesWritten;

            NDIS_MEDIUM medium ;
            NDIS_PHYSICAL_MEDIUM physicalMedium = NdisPhysicalMediumUnspecified;

            HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(netInterface.name()).utf16(), 0,
                                       FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

            if (handle == INVALID_HANDLE_VALUE) {
                continue;
            }

            oid = OID_GEN_MEDIA_SUPPORTED;
            bytesWritten;
            bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                          &medium, sizeof(medium), &bytesWritten, 0);
            if (!result) {
                CloseHandle(handle);
                qDebug() << "DeviceIo result is false";
                return QNetworkInterface();
                continue;
            }

            oid = OID_GEN_PHYSICAL_MEDIUM;
            bytesWritten = 0;
            result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                     &physicalMedium, sizeof(physicalMedium), &bytesWritten, 0);

            if (!result) {
                CloseHandle(handle);
                if (medium == NdisMedium802_3 && mode == QSystemNetworkInfo::EthernetMode) {
                    return netInterface;
                } else {
                    continue;
                }
            }

            CloseHandle(handle);
            if (physicalMedium == NdisMediumWirelessWan && mode == QSystemNetworkInfo::WlanMode) {
                //some wifi devices show up here
                return netInterface;
            }

            if (medium == NdisMedium802_3) {
                switch (physicalMedium) {
                case NdisPhysicalMediumUnspecified:
                    {
                        if(mode == QSystemNetworkInfo::EthernetMode) {
                            return netInterface;
                        }
                    }
                    break;
                case NdisPhysicalMediumWirelessLan:
                    {
                        if(mode == QSystemNetworkInfo::WlanMode) {
                            return netInterface;
                        }
                    }
                    break;
                case NdisPhysicalMediumBluetooth:
                    {
                        if(mode == QSystemNetworkInfo::BluetoothMode) {
                            return netInterface;
                        }
                    }
                    break;
#ifdef NDIS_SUPPORT_NDIS6
                case NdisPhysicalMediumWiMax:
                    {
                        if(mode == QSystemNetworkInfo::WimaxMode) {
                            return netInterface;
                        }
                    }
                    break;
#endif
                };
            }
        }
#endif
    }

    return QNetworkInterface();
}

bool QSystemNetworkInfoPrivate::isDefaultMode(QSystemNetworkInfo::NetworkMode mode)
{
    bool isDefaultGateway = false;
    QNetworkInterface netInterface = interfaceForMode(mode);
    QString deviceNameReg;

    switch(QSysInfo::WindowsVersion) {
    case  QSysInfo::WV_VISTA:
    case QSysInfo::WV_WINDOWS7:
        deviceNameReg = QString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%1\\").arg(netInterface.name());
        break;
    default:
        deviceNameReg = QString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%1\\Parameters\\Tcpip").arg(netInterface.name());
        break;
    };

    QSettings deviceSettings(deviceNameReg, QSettings::NativeFormat);

    if(!deviceSettings.value("DhcpDefaultGateway").toStringList().isEmpty()
         || !deviceSettings.value("DefaultGateway").toStringList().isEmpty()) {
        isDefaultGateway = true;
    }
    return isDefaultGateway;
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    QList <QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::EthernetMode
            << QSystemNetworkInfo::WlanMode
            << QSystemNetworkInfo::BluetoothMode
            << QSystemNetworkInfo::WcdmaMode
            << QSystemNetworkInfo::CdmaMode
            << QSystemNetworkInfo::GsmMode
            << QSystemNetworkInfo::WimaxMode;

    for (int i = 0; i < modeList.size(); ++i) {
        if ( isDefaultMode(modeList.at(i)))
            return modeList.at(i);
    }

    return QSystemNetworkInfo::UnknownMode;
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
        : QObject(parent),deviceContextHandle(0)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
    ReleaseDC(NULL, deviceContextHandle);
}

int QSystemDisplayInfoPrivate::displayBrightness(int /*screen*/)
{
#if !defined( Q_CC_MINGW)
#if !defined( Q_OS_WINCE)
    WMIHelper *wHelper;
    wHelper = new WMIHelper(this);
    wHelper->setWmiNamespace("root/wmi");
    wHelper->setClassName("WmiMonitorBrightness");
    wHelper->setClassProperty(QStringList() << "CurrentBrightness");

    QVariant v = wHelper->getWMIData();
    return (quint8)v.toUInt();
#else
    // This could would detect the state of the backlight, which is as close as we're going to get
    // for WinCE.  Unfortunately, some devices don't honour the Microsoft power management API.
    // This means that the following code is not portable across WinCE devices and so shouldn't
    // be included.

    //CEDEVICE_POWER_STATE powerState;
    //
    //if (ERROR_SUCCESS != GetDevicePower(L"BKL1:", POWER_NAME, &powerState))
    //    return -1;
    //
    //// Backlight is on
    //if (powerState == D0)
    //    return 100;
    //
    //// Screen is on, backlight is off
    //if (powerState == D1)
    //    return 50;
    //
    //// Screen is off
    //return 0;
#endif
#endif

    return -1;
}

int QSystemDisplayInfoPrivate::getMonitorCaps(int caps, int screen)
{
    if(deviceContextHandle == 0) {
        QDesktopWidget wid;
        HWND hWnd = wid.screen(screen)->winId();
        deviceContextHandle = GetDC(hWnd);
    }
    int out = GetDeviceCaps(deviceContextHandle ,caps);
    return out;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    int bpp = getMonitorCaps(BITSPIXEL, screen);
    int planes = getMonitorCaps(PLANES, screen);
    if(planes > 1) {
        bpp = 1 << planes;
    }
    return bpp;
}

QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfoPrivate::getOrientation(int screen)
{
    QSystemDisplayInfo::DisplayOrientation orientation = QSystemDisplayInfo::Unknown;

    if(screen < 16 && screen > -1) {
        if(physicalWidth(screen) > physicalHeight(screen)) {
            orientation = QSystemDisplayInfo::Landscape;
        } else {
            orientation = QSystemDisplayInfo::Portrait;
        }
//        int rotation = 0;
//        switch(rotation) {
//        case 0:
//        case 360:
//            break;
//        case 90:
//            break;
//        case 180:
//            orientation = QSystemDisplayInfo::InvertedLandscape;
//            break;
//        case 270:
//            orientation = QSystemDisplayInfo::InvertedPortrait;
//            break;
//        };
    }
    return orientation;
}


float QSystemDisplayInfoPrivate::contrast(int screen)
{
    DWORD current = 0;
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) {

        HMONITOR monHandle = NULL;
        QDesktopWidget wid;
        HWND hWnd = wid.screen(screen)->winId();

        monHandle = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        LPPHYSICAL_MONITOR physMon = 0;
        DWORD numMonitors;

        if(GetNumberOfPhysicalMonitorsFromHMONITOR(monHandle, &numMonitors)) {

            physMon = (LPPHYSICAL_MONITOR)malloc( numMonitors* sizeof(PHYSICAL_MONITOR));

            GetPhysicalMonitorsFromHMONITOR(monHandle, numMonitors, physMon);


            DWORD caps, colorTemp;
            GetMonitorCapabilities(monHandle, &caps, &colorTemp);
            if (caps & MC_CAPS_CONTRAST ) {
                DWORD min=0;
                DWORD max = 0;
                GetMonitorContrast(monHandle, &min, &current, &max);
            }
            DestroyPhysicalMonitors(numMonitors,physMon);
            free(physMon);
        }
    }
    return (float)current;
}

int QSystemDisplayInfoPrivate::getDPIWidth(int screen)
{
    int dpi = getMonitorCaps(LOGPIXELSX,screen);
    if(screen < 16 && screen > -1) {
//
        }
    return dpi;
}

int QSystemDisplayInfoPrivate::getDPIHeight(int screen)
{
    int dpi = getMonitorCaps(LOGPIXELSY,screen);
    if(screen < 16 && screen > -1) {
//Win32_VideoController
    }
    return dpi;
}


int QSystemDisplayInfoPrivate::physicalHeight(int screen)
{
    int height = getMonitorCaps(VERTSIZE,screen);
    // root/wmi
// WmiMonitorBasicDisplayParams MaxHorizontalImageSize MaxVerticalImageSize
    return height;
}

int QSystemDisplayInfoPrivate::physicalWidth(int screen)
{
    int width = getMonitorCaps(HORZSIZE,screen);
    return width;
}

QSystemDisplayInfo::BacklightState  QSystemDisplayInfoPrivate::backlightStatus(int screen)
{
    Q_UNUSED(screen)
    return QSystemDisplayInfo::BacklightStateUnknown;
}

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
    : QObject(parent)
{
    logicalDrives();
#if !defined( Q_CC_MINGW)
#if !defined( Q_OS_WINCE)
    WMIHelper *wHelper;
    wHelper =  WMIHelper::instance();
    wHelper->setWmiNamespace("root/cimv2");
    wHelper->setClassName("Win32_VolumeChangeEvent");
    QString aString = "SELECT * FROM __InstanceOperationEvent WITHIN 1 WHERE (TargetInstance ISA 'Win32_LogicalDisk') AND (TargetInstance.DriveType = 5 OR TargetInstance.DriveType = 2)";

    wHelper->setupNotfication("root/cimv2",aString,QStringList());

    connect(wHelper,SIGNAL(wminotificationArrived()),this,SLOT(notificationArrived()));
#endif
#endif
}


QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

void QSystemStorageInfoPrivate::notificationArrived()
{
    QMap<QString, QString> oldDrives;

     oldDrives = mountEntriesMap;
     mountEntries();
     QString driveLetter;

     if(mountEntriesMap.count() < oldDrives.count()) {
         QMapIterator<QString, QString> i(oldDrives);
          while (i.hasNext()) {
              i.next();
              if(!mountEntriesMap.contains(i.key())) {
                  emit logicalDriveChanged(false, i.key());
              }
          }
      } else if(mountEntriesMap.count() > oldDrives.count()) {
         QMapIterator<QString, QString> i(mountEntriesMap);
          while (i.hasNext()) {
              i.next();

              if(oldDrives.contains(i.key()))
                  continue;
                 emit logicalDriveChanged(true,i.key());
         }
     }
 }

qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    qint64 freeBytes;
    qint64 totalBytes;
    qint64 totalFreeBytes;

#if !defined(Q_OS_WINCE)
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
    bool ok = GetDiskFreeSpaceEx((WCHAR *)driveVolume.utf16(),(PULARGE_INTEGER)&freeBytes, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes);
#if !defined(Q_OS_WINCE)
    SetErrorMode(0);
#endif
    if(!ok)
        totalFreeBytes = 0;
    return totalFreeBytes;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    qint64 freeBytes;
    qint64 totalBytes;
    qint64 totalFreeBytes;

#if !defined(Q_OS_WINCE)
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
    bool ok = GetDiskFreeSpaceEx((WCHAR *)driveVolume.utf16(),(PULARGE_INTEGER)&freeBytes, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes);
#if !defined(Q_OS_WINCE)
    SetErrorMode(0);
#endif
    if(!ok)
        totalBytes = 0;
    return totalBytes;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    uriForDrive(driveVolume);
#if !defined( Q_OS_WINCE)
    uint result =  GetDriveType((WCHAR *)driveVolume.utf16());
    switch(result) {
    case 0:
    case 1:
        return QSystemStorageInfo::NoDrive;
        break;
    case 2:
        return QSystemStorageInfo::RemovableDrive;
        break;
    case 3:
        return QSystemStorageInfo::InternalDrive;
        break;
    case 4:
        return QSystemStorageInfo::RemoteDrive;
        break;
    case 5:
        return QSystemStorageInfo::CdromDrive;
        break;
    case 6:
        break;
    };
#endif
    return QSystemStorageInfo::NoDrive;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    mountEntriesMap.clear();
    QStringList drivesList;
    QFileInfoList drives = QDir::drives();
    foreach(QFileInfo drive, drives) {
        QString letter =  drive.absoluteFilePath();
        letter.chop(1);
        if(totalDiskSpace(letter) > 0) {
            drivesList.append(letter);
            mountEntriesMap.insert(letter,letter);
        }
    }
    return drivesList;
}

void QSystemStorageInfoPrivate::mountEntries()
{
    logicalDrives();
}

QString QSystemStorageInfoPrivate::uriForDrive(const QString &driveVolume)
{
    WCHAR volume[MAX_PATH];
    if (GetVolumeNameForVolumeMountPoint(reinterpret_cast<const wchar_t *>(driveVolume.utf16()), volume, MAX_PATH)) {
        QString returnStr(QString::fromUtf16(reinterpret_cast<const unsigned short *>(volume)));
        qDebug() << returnStr;
        return returnStr;
    }
    return QString();
}

QSystemStorageInfo::StorageState QSystemStorageInfoPrivate::getStorageState(const QString &driveVolume)
{
    QSystemStorageInfo::StorageState state = QSystemStorageInfo::UnknownStorageState;
   return state;
}

#if defined(Q_OS_WINCE)
QPowerNotificationThread::QPowerNotificationThread(QSystemDeviceInfoPrivate *parent)
    : parent(parent),
    done(false)
{
    wakeUpEvent = CreateEvent(NULL, FA      LSE, FALSE, NULL);
}

QPowerNotificationThread::~QPowerNotificationThread() {
    mutex.lock();

    done = true;
    SetEvent(wakeUpEvent);

    mutex.unlock();

    wait();

    CloseHandle(wakeUpEvent);
}

void QPowerNotificationThread::run()
{

    const int MaxMessageSize = sizeof(POWER_BROADCAST) + sizeof(POWER_BROADCAST_POWER_INFO)
        + MAX_PATH;

    MSGQUEUEOPTIONS messageQueueOptions = { 0 };
    messageQueueOptions.dwSize = sizeof(messageQueueOptions);
    messageQueueOptions.dwFlags = MSGQUEUE_NOPRECOMMIT;
    messageQueueOptions.dwMaxMessages = 0;
    messageQueueOptions.cbMaxMessage = MaxMessageSize;
    messageQueueOptions.bReadAccess = true;

    HANDLE messageQueue = CreateMsgQueue(NULL, &messageQueueOptions);

    if (messageQueue == NULL)
        return;

    HANDLE powerNotificationHandle = RequestPowerNotifications(messageQueue, PBT_TRANSITION|PBT_POWERINFOCHANGE);
    if (messageQueue == NULL)
        return;

    HANDLE events[2] = {messageQueue, wakeUpEvent};

    while(true) {
        DWORD dwRet = WaitForMultipleObjects(2, events, FALSE, INFINITE);

        mutex.lock();

        if (done) {
            mutex.unlock();
            break;
        }

        if (dwRet == WAIT_OBJECT_0) {

            BYTE buffer[MaxMessageSize];
            DWORD bytesRead = 0;
            DWORD messageProperties;

            if (!ReadMsgQueue(messageQueue, &buffer, MaxMessageSize, &bytesRead, 0, &messageProperties)) {
                continue;
            }

            if (bytesRead < sizeof(POWER_BROADCAST)) {
                continue;
            }

            POWER_BROADCAST *broadcast = (POWER_BROADCAST*) (buffer);
            if (broadcast->Message == PBT_POWERINFOCHANGE) {
                POWER_BROADCAST_POWER_INFO *info = (POWER_BROADCAST_POWER_INFO*) broadcast->SystemPowerState;
                parent->notificationArrived();
                qDebug() << "CHANGED!";
            }

            parent->currentPowerState();

        } else if (dwRet == WAIT_OBJECT_0 + 1) {
            // we should only be here if the wakeUpEvent was signalled
            // which only occurs when the thread is being stopped
            Q_ASSERT(done);
        } else if (dwRet = WAIT_FAILED) {
            continue;
        }

        mutex.unlock();
    }

    StopPowerNotifications(powerNotificationHandle);
    CloseMsgQueue(messageQueue);
    CloseHandle(messageQueue);
}

#endif

QSystemDeviceInfoPrivate *QSystemDeviceInfoPrivate::self = 0;
QSystemBatteryInfoPrivate *QSystemBatteryInfoPrivate::batself = 0;

#if !defined(Q_OS_WINCE)
bool qax_winEventFilter(void *message)
{
    MSG *pMsg = (MSG*)message;
    if( pMsg->message == WM_POWERBROADCAST) {

        qDebug() << Q_FUNC_INFO;
        POWERBROADCAST_SETTING* pps = (POWERBROADCAST_SETTING*) pMsg->lParam;
        if ( sizeof(int) == pps->DataLength &&
                pps->PowerSetting == GUID_BATTERY_PERCENTAGE_REMAINING ) {
            int nPercentLeft = *(int*)(DWORD_PTR) pps->Data;

            qDebug() <<Q_FUNC_INFO << nPercentLeft;
        }
        switch (pMsg->wParam) {
        case PBT_APMPOWERSTATUSCHANGE:
        {
//            QSystemDeviceInfoPrivate::instance()->batteryLevel();
//            QSystemDeviceInfoPrivate::instance()->currentPowerState();
        }
            break;
        default:
            break;
        };
    }

    return false;
}
#endif

QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
    batteryLevelCache = 0;
    currentPowerStateCache = QSystemDeviceInfo::UnknownPower;
    batteryStatusCache = QSystemDeviceInfo::NoBatteryLevel;
#if !defined(Q_OS_WINCE)
//    QAbstractEventDispatcher::instance()->setEventFilter(qax_winEventFilter);
#else
    powerNotificationThread = new QPowerNotificationThread(this);
    powerNotificationThread->start();
#endif
    if(!self)
        self = this;

}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
#if defined(Q_OS_WINCE)
    delete powerNotificationThread;
#endif
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;

#if !defined(Q_OS_WINCE)
    int mouseResult = GetSystemMetrics(SM_CMOUSEBUTTONS);
    if(mouseResult > 0) {
        if((methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
            methods |= QSystemDeviceInfo::Mouse;

        }
    }
# if defined(SM_TABLETPC)
    int tabletResult = GetSystemMetrics(SM_TABLETPC);
    if(tabletResult > 0) {
        if((methods & QSystemDeviceInfo::SingleTouch) != QSystemDeviceInfo::SingleTouch) {
            methods |= QSystemDeviceInfo::SingleTouch;

        }
    }
# endif
#else
    // detect the presence of a mouse
    RECT rect;
    if (GetClipCursor(&rect)) {
        if ((methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
            methods |= QSystemDeviceInfo::Mouse;
        }
    }
    // We could also try to detect the presence of a stylus / single touch input.
    // A team from Microsoft was unable to do this in a way which scaled across multiple devices.
    // For more details see:
    // http://blogs.msdn.com/netcfteam/archive/2006/10/02/Platform-detection-III_3A00_-How-to-detect-a-touch-screen-on-Windows-CE-in-.NET-CF.aspx
    // Since all non-Qt apps on non-compliant devices will be able to use the touch screen
    // (by virtue of being written for one particular device) shipping a library which will cause
    // just the Qt apps to fail may not be the best move.
#endif
    int keyboardType = GetKeyboardType(0);
    switch(keyboardType) {
    case 1:
    case 3:
        {
            if((methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                methods |= QSystemDeviceInfo::Keyboard;

            }
        }
        break;
    case 2:
    case 4:
        {
            if((methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                methods |= QSystemDeviceInfo::Keyboard;

            }
            if((methods & QSystemDeviceInfo::Keypad) != QSystemDeviceInfo::Keypad) {
                methods |= QSystemDeviceInfo::Keypad;

            }
        }
        break;
    case 5:
        {
            if((methods & QSystemDeviceInfo::Keypad) != QSystemDeviceInfo::Keypad) {
                methods |= QSystemDeviceInfo::Keypad;

            }
        }
        break;
    default:
        break;

    };

    return methods;
}


QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
#ifdef Q_OS_WINCE
    SYSTEM_POWER_STATUS_EX status;
    GetSystemPowerStatusEx(&status, true);

#else
    SYSTEM_POWER_STATUS status;
    GetSystemPowerStatus(&status);
#endif
    QSystemDeviceInfo::PowerState state = QSystemDeviceInfo::UnknownPower;

    if(status.ACLineStatus  == AC_LINE_ONLINE) {
        state = QSystemDeviceInfo::WallPower;
    }
    if(status.ACLineStatus  == AC_LINE_OFFLINE) {
        state = QSystemDeviceInfo::BatteryPower;
    }
    if(status.BatteryFlag & BATTERY_FLAG_CHARGING) {
        state = QSystemDeviceInfo::WallPowerChargingBattery;
    }
    if( currentPowerStateCache != state) {
        currentPowerStateCache = state;
        emit powerStateChanged(state);
    }

    return state;
}

QString QSystemDeviceInfoPrivate::imei()
{
        return "";
}

QString QSystemDeviceInfoPrivate::imsi()
{
        return "";
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
   QString manu;
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
    WMIHelper *wHelper;
    wHelper = new WMIHelper(this);
    wHelper->setWmiNamespace("root/cimv2");
    wHelper->setClassName("Win32_ComputerSystemProduct");
    wHelper->setClassProperty(QStringList() << "Vendor");
    QVariant v = wHelper->getWMIData();
    manu = v.toString();
    if(manu.isEmpty()) {
        manu = "System manufacturer";
    }
    delete wHelper;
#endif
    return manu;
}

QString QSystemDeviceInfoPrivate::model()
{
   QString model;
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
    WMIHelper *wHelper;
    wHelper = new WMIHelper(this);
    wHelper->setWmiNamespace("root/cimv2");

    wHelper->setClassName("Win32_ComputerSystem");
    wHelper->setClassProperty(QStringList() << "Model");
    QVariant v = wHelper->getWMIData();
    model = v.toString();

    wHelper->setClassName("Win32_ComputerSystem");
    wHelper->setClassProperty(QStringList() << "PCSystemType");
    v = wHelper->getWMIData();
    switch(v.toUInt()) {
    case 0:
        model += "";
        break;
    case 1:
        model += "Desktop";
        break;
    case 2:
        model += "Mobile";
        break;
    case 3:
        model += "Workstation";
        break;
    case 4:
        model += "Enterprise Server";
        break;
    case 5:
        model += "Small/Home Server";
        break;
    case 6:
        model += "Applicance PC";
        break;
    case 7:
        model += "Performace Server";
        break;
    case 8:
        model += "Maximum";
        break;

    };
    delete wHelper;
#endif
    return model;
}

QString QSystemDeviceInfoPrivate::productName()
{
   QString name;
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
    WMIHelper *wHelper;
    wHelper = new WMIHelper(this);
    wHelper->setWmiNamespace("root/cimv2");
    wHelper->setClassName("Win32_ComputerSystemProduct");
    wHelper->setClassProperty(QStringList() << "Name");
    QVariant v = wHelper->getWMIData();
    name = v.toString();

    if(name.isEmpty()) {
        wHelper->setClassName("Win32_ComputerSystem");
        wHelper->setClassProperty(QStringList() << "PCSystemType");
        v = wHelper->getWMIData();
        name = v.toString();
        if(name.isEmpty()) {
            name = "Unspecified product";
        }
    }

    delete wHelper;
#endif
    return name;
}

int QSystemDeviceInfoPrivate::batteryLevel()
{
    int bat = 0;
#ifdef Q_OS_WINCE
    SYSTEM_POWER_STATUS_EX status;
    if(GetSystemPowerStatusEx(&status, true) ) {
        bat = status.BatteryLifePercent;
    } else {
       qDebug() << "Battery status failed";
       return 0;
    }
#else
    SYSTEM_POWER_STATUS status;
    if(GetSystemPowerStatus( &status) ) {
        bat = status.BatteryLifePercent;
    } else {
       qDebug() << "Battery status failed";
       return 0;
    }
#endif
    if(bat == 255) //battery unknown level status
        bat = 0;

    if(batteryLevelCache != bat) {
       batteryLevelCache = bat;
       emit batteryLevelChanged(bat);
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

    return bat;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
#ifdef Q_OS_WINCE
    HSIM handle;
    DWORD lockedState;
    HRESULT result = SimInitialize(0,NULL,NULL,&handle);
    if(result == S_OK) {
        SimGetPhoneLockedState(handle,&lockedState);
        if(lockedState == SIM_LOCKEDSTATE_READY) {
            return QSystemDeviceInfo::SingleSimAvailable;
        } else {
            return QSystemDeviceInfo::SimLocked;
        }


    } else if(result == SIM_E_NOSIM) {
        return QSystemDeviceInfo::SimNotAvailable;
    }
    SimDeinitialize(handle);

#endif
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::currentBluetoothPowerState()
{
    return false;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
#ifdef Q_OS_WINCE
    HSIM handle;
    DWORD lockedState;
    HRESULT result = SimInitialize(0,NULL,NULL,&handle);
    if(result == S_OK) {
        SimGetPhoneLockedState(handle,&lockedState);
        if(lockedState != SIM_LOCKEDSTATE_READY) {
            return true;
        }
    }
#else
    QSystemScreenSaverPrivate si;
    if( SystemParametersInfo( SPI_GETSCREENSAVERRUNNING,true,0,0)
        && si.screenSaverSecureEnabled() )
        return true;

#endif
    return false;
}

QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfoPrivate::keyboardType()
{
    QSystemDeviceInfo::InputMethodFlags methods = inputMethodType();
    QSystemDeviceInfo::KeyboardTypeFlags keyboardFlags = QSystemDeviceInfo::UnknownKeyboard;

    if((methods & QSystemDeviceInfo::Keyboard)) {
        keyboardFlags = (keyboardFlags | QSystemDeviceInfo::FullQwertyKeyboard);
  }
    if(isWirelessKeyboardConnected()) {
        keyboardFlags = (keyboardFlags | QSystemDeviceInfo::WirelessKeyboard);
    }
// how to check softkeys on desktop?
    return keyboardFlags;
}

bool QSystemDeviceInfoPrivate::isWirelessKeyboardConnected()
{
    return hasWirelessKeyboardConnected;
}

bool QSystemDeviceInfoPrivate::isKeyboardFlipOpen()
{
    return false;
}

void QSystemDeviceInfoPrivate::keyboardConnected(bool connect)
{
    if(connect != hasWirelessKeyboardConnected)
        hasWirelessKeyboardConnected = connect;
    Q_EMIT wirelessKeyboardConnected(connect);
}

bool QSystemDeviceInfoPrivate::keypadLightOn(QSystemDeviceInfo::keypadType type)
{
    return false;
}

QUuid QSystemDeviceInfoPrivate::uniqueID()
{
    WMIHelper *wHelper;
    wHelper = new WMIHelper(this);
    wHelper->setWmiNamespace("root/cimv2");
    wHelper->setClassName("Win32_ComputerSystemProduct");
    wHelper->setClassProperty(QStringList() << "UUID");

    return  QUuid(wHelper->getWMIData().toString());
}

QSystemDeviceInfo::LockType QSystemDeviceInfoPrivate::lockStatus()
{
    return QSystemDeviceInfo::UnknownLock;
}

QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
{
    screenSaverSecureEnabled();
}

bool QSystemScreenSaverPrivate::screenSaverSecureEnabled()
{
    screenSaverSecure = false;
    QSettings screenSettingsTmp("HKEY_CURRENT_USER\\Software\\Policies\\Microsoft\\Windows\\Control Panel\\Desktop", QSettings::NativeFormat);
    if(!screenSettingsTmp.value("SCRNSAVE.EXE").toString().isEmpty()) {
        settingsPath = "HKEY_CURRENT_USER\\Software\\Policies\\Microsoft\\Windows\\Control Panel\\Desktop";
    } else {
        settingsPath = "HKEY_CURRENT_USER\\Control Panel\\Desktop";
    }

    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    if(screenSettings.value("ScreenSaverIsSecure").toString() == "1") {
        screenSaverSecure = true;
    }
    screenPath = screenSettings.value("SCRNSAVE.EXE").toString();
    return screenSaverSecure;
}


bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    if(screenSaverSecure)
        return false;
#if !defined(Q_OS_WINCE)
    return SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,true,0,SPIF_SENDWININICHANGE);
#else
    return false;
#endif
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    return !screenSettings.value("SCRNSAVE.EXE").toString().isEmpty();
}

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QObject *parent)
: QObject(parent)
{
    if(!batself)
        batself = this;
    setConnection();
    getBatteryStatus();

}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{

}


QSystemBatteryInfo::ChargerType QSystemBatteryInfoPrivate::chargerType() const
{
    return curChargeType;
}

QSystemBatteryInfo::ChargingState QSystemBatteryInfoPrivate::chargingState() const
{
    return curChargeState;
}


int QSystemBatteryInfoPrivate::nominalCapacity() const
{
    return capacity;
}

int QSystemBatteryInfoPrivate::remainingCapacityPercent() const
{
    return currentBatLevelPercent;
}

int QSystemBatteryInfoPrivate::remainingCapacity() const
{
    return remainingEnergy;
}


int QSystemBatteryInfoPrivate::voltage() const
{
    return currentVoltage;
}

int QSystemBatteryInfoPrivate::remainingChargingTime() const
{
    return timeToFull;
}

int QSystemBatteryInfoPrivate::currentFlow() const
{
    return dischargeRate;
}

int QSystemBatteryInfoPrivate::remainingCapacityBars() const
{
    return 0;
}

int QSystemBatteryInfoPrivate::maxBars() const
{
    return 0;
}

QSystemBatteryInfo::BatteryStatus QSystemBatteryInfoPrivate::batteryStatus() const
{
   return QSystemBatteryInfo::BatteryUnknown;
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
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityBarsChanged(int))))) {
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingChargingTimeChanged(int))))) {
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            voltageChanged(int))))) {
    }

}

void QSystemBatteryInfoPrivate::setConnection()
{
#if !defined(Q_OS_WINCE)
    QAbstractEventDispatcher::instance()->setEventFilter(qax_winEventFilter);
#else
    powerNotificationThread = new QPowerNotificationThread(this);
    powerNotificationThread->start();
#endif

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(notificationArrived()));
    timer->start(1000);

#if !defined( Q_CC_MINGW)
#if !defined( Q_OS_WINCE)
//    WMIHelper *wHelper;
//    wHelper =  WMIHelper::instance();
//    wHelper->setWmiNamespace("root/cimv2");
//    wHelper->setClassName("Win32_PowerManagementEvent");
//    QString aString = "SELECT * FROM Win32_PowerManagementEvent";
//    wHelper->setupNotfication("root/cimv2",aString,QStringList());

//    connect(wHelper,SIGNAL(wminotificationArrived()),this,SLOT(notificationArrived()));
#endif
#endif
}

void QSystemBatteryInfoPrivate::notificationArrived()
{
    getBatteryStatus();
}

void QSystemBatteryInfoPrivate::disconnectNotify(const char *signal)
{

}

qint32 QSystemBatteryInfoPrivate::startCurrentMeasurement(qint32 rate)
{
 return 0;
}

QSystemBatteryInfo::EnergyUnit QSystemBatteryInfoPrivate::energyMeasurementUnit()
{
   return QSystemBatteryInfo::UnitmWh;
}

void QSystemBatteryInfoPrivate::getBatteryStatus()
{
    int cLevel = 0;
    int cEnergy = 0;
    int cVoltage = 0;
    int cTime = 0;
    int rEnergy = 0;

    QSystemBatteryInfo::ChargingState cState = QSystemBatteryInfo::ChargingError;
    QSystemBatteryInfo::ChargerType cType = QSystemBatteryInfo::UnknownCharger;
    QSystemBatteryInfo::BatteryStatus batStatus = QSystemBatteryInfo::BatteryUnknown;

    NTSTATUS powerStatus;
    SYSTEM_BATTERY_STATE systemBatteryState;
    powerStatus = CallNtPowerInformation(SystemBatteryState,NULL,0,&systemBatteryState,sizeof(systemBatteryState));

    if(systemBatteryState.Charging) {
        cState = QSystemBatteryInfo::Charging;
    } else {
        cState = QSystemBatteryInfo::NotCharging;
    }

    if(cState != curChargeState ) {
        curChargeState = cState;
        emit chargingStateChanged(curChargeState);
    }


    capacity = systemBatteryState.MaxCapacity;

    rEnergy = systemBatteryState.RemainingCapacity;
    if(rEnergy != remainingEnergy) {
        remainingEnergy = rEnergy;
        emit remainingCapacityChanged(remainingEnergy);
    }

    cEnergy = systemBatteryState.Rate;
    if (cEnergy != dischargeRate) {
        dischargeRate = cEnergy;
        emit currentFlowChanged(dischargeRate);
    }

    if (systemBatteryState.AcOnLine) {
        cType = QSystemBatteryInfo::WallCharger;
    } else {
        cType = QSystemBatteryInfo::NoCharger;
    }
    if (cType != curChargeType) {
        curChargeType = cType;
        emit chargerTypeChanged(curChargeType);
    }

    cTime = systemBatteryState.EstimatedTime;
    if(cTime != timeToFull) {
        timeToFull = cTime;
        emit remainingChargingTimeChanged(timeToFull);
    }

    SYSTEM_POWER_STATUS status;
    if(GetSystemPowerStatus( &status) ) {

        cLevel = status.BatteryLifePercent;
        if(cLevel != currentBatLevelPercent) {
            currentBatLevelPercent = cLevel;
            emit remainingCapacityPercentChanged(currentBatLevelPercent);
        }
        switch(status.BatteryFlag) {
        case (1):
            batStatus = QSystemBatteryInfo::BatteryOk;
            break;
        case (2):
            batStatus = QSystemBatteryInfo::BatteryLow;
            break;
        case (3):
            batStatus = QSystemBatteryInfo::BatteryVeryLow;
            break;
        case (4):
            batStatus = QSystemBatteryInfo::BatteryCritical;
            break;
        case (128):
        case (255):
            batStatus = QSystemBatteryInfo::BatteryUnknown;
            break;
        }
    }
    if(batStatus != currentBatStatus) {
        currentBatStatus = batStatus;
        emit batteryStatusChanged(currentBatStatus);
    }


#define HASBATTERY 0x1
#define ONBATTERY  0x2

    QUuid guidDeviceBattery(0x72631e54L,0x78A4,0x11d0,0xbc,0xf7,0x00,0xaa,0x00,0xb7,0xb3,0x2a);
    GUID GUID_DEVICE_BATTERY = static_cast<GUID>(guidDeviceBattery);

    DWORD dwResult = ONBATTERY;
    HDEVINFO hdevInfo = SetupDiGetClassDevs(&GUID_DEVICE_BATTERY,0,0,DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (INVALID_HANDLE_VALUE != hdevInfo) {
        for (int i = 0; i < 100; i++) {
            SP_DEVICE_INTERFACE_DATA deviceInterfaceData = {0};
            deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);

            if (SetupDiEnumDeviceInterfaces(hdevInfo,0,&GUID_DEVICE_BATTERY,i,&deviceInterfaceData)){
                DWORD cbRequired = 0;

                SetupDiGetDeviceInterfaceDetail(hdevInfo, &deviceInterfaceData,0, 0, &cbRequired, 0);
                if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
                    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetail =
                            (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
                    if (deviceInterfaceDetail){
                        deviceInterfaceDetail->cbSize = sizeof(*deviceInterfaceDetail);
                        if (SetupDiGetDeviceInterfaceDetail(hdevInfo, &deviceInterfaceData, deviceInterfaceDetail, cbRequired, &cbRequired, 0)) {

                            HANDLE batteryHandle = CreateFile(deviceInterfaceDetail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                            if (INVALID_HANDLE_VALUE != batteryHandle){

                                BATTERY_QUERY_INFORMATION batteryQueryInfo = {0};

                                DWORD inBuf = 0;
                                DWORD dwOut;

                                if (DeviceIoControl(batteryHandle,IOCTL_BATTERY_QUERY_TAG, &inBuf, sizeof(inBuf), &batteryQueryInfo.BatteryTag, sizeof(batteryQueryInfo.BatteryTag), &dwOut, NULL)
                                        && batteryQueryInfo.BatteryTag) {
                                    BATTERY_INFORMATION batteryInfo = {0};
                                    batteryQueryInfo.InformationLevel = BatteryInformation;

                                    if (DeviceIoControl(batteryHandle, IOCTL_BATTERY_QUERY_INFORMATION,&batteryQueryInfo,sizeof(batteryQueryInfo),&batteryInfo,sizeof(batteryInfo), &dwOut, NULL)) {

                                        if (batteryInfo.Capabilities & BATTERY_SYSTEM_BATTERY) {
                                            if (!(batteryInfo.Capabilities & BATTERY_IS_SHORT_TERM)) {
                                                dwResult |= HASBATTERY;
                                            }

                                            BATTERY_WAIT_STATUS batteryWaitStatus = {0};
                                            batteryWaitStatus.BatteryTag = batteryQueryInfo.BatteryTag;

                                            BATTERY_STATUS batteryStatus;
                                            if (DeviceIoControl(batteryHandle, IOCTL_BATTERY_QUERY_STATUS, &batteryWaitStatus, sizeof(batteryWaitStatus), &batteryStatus, sizeof(batteryStatus), &dwOut, NULL)) {
                                                if (batteryStatus.PowerState & BATTERY_POWER_ON_LINE) {
                                                    dwResult &= ~ONBATTERY;
                                                }
                                                cVoltage = batteryStatus.Voltage;
                                                if(cVoltage != currentVoltage) {
                                                    currentVoltage= cVoltage;
                                                    emit voltageChanged(currentVoltage);
                                                }
                                            }
                                        }
                                    }
                                }
                                CloseHandle(batteryHandle);
                            }
                        }
                        LocalFree(deviceInterfaceDetail);
                    }
                }
            }
            else  if (ERROR_NO_MORE_ITEMS == GetLastError()) {
                break;
            }
        }
        SetupDiDestroyDeviceInfoList(hdevInfo);
    }
    if (!(dwResult & HASBATTERY))
        dwResult &= ~ONBATTERY;
}


#include "moc_qsysteminfo_win_p.cpp"

QTM_END_NAMESPACE
