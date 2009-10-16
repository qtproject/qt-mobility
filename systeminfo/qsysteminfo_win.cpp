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
#include "qsysteminfo_win_p.h"
#include <qt_windows.h>

#include <QtCore/qmutex.h>
#include <QtCore/private/qmutexpool_p.h>

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

#include <locale.h>

#if !defined( Q_CC_MINGW)
#ifndef Q_OS_WINCE
#include "qwmihelper_win_p.h"
#include <Wlanapi.h>
#include <Dshow.h>
#endif
#endif

#ifdef Q_OS_WINCE
//#include <af_irda.h>
#include <vibrate.h>
//#include <Led_drvr.h>
#include <simmgr.h>
//#include <Ifapi.h>
#include <Winbase.h>
#endif

#define _WCHAR_T_DEFINED
#define _TIME64_T_DEFINED

typedef struct _BLUETOOTH_FIND_RADIO_PARAMS {
    DWORD   dwSize;             //  IN  sizeof this structure
} BLUETOOTH_FIND_RADIO_PARAMS;
//typedef HANDLE      HBLUETOOTH_RADIO_FIND;


//
typedef DWORD (WINAPI *WlanOpenHandleProto)
    (DWORD dwClientVersion, PVOID pReserved, PDWORD pdwNegotiatedVersion, PHANDLE phClientHandle);
static WlanOpenHandleProto local_WlanOpenHandle = 0;
//
typedef DWORD (WINAPI *WlanCloseHandleProto)(HANDLE hClientHandle, PVOID pReserved);
static WlanCloseHandleProto local_WlanCloseHandle = 0;
//
typedef DWORD (WINAPI *WlanEnumInterfacesProto)
    (HANDLE hClientHandle, PVOID pReserved, PWLAN_INTERFACE_INFO_LIST* ppInterfaceList);
static WlanEnumInterfacesProto local_WlanEnumInterfaces = 0;
//
typedef DWORD (WINAPI *WlanQueryInterfaceProto)
    (HANDLE hClientHandle, const GUID *pInterfaceGuid, WLAN_INTF_OPCODE OpCode, PVOID pReserved,
     PDWORD pdwDataSize, PVOID *ppData, PWLAN_OPCODE_VALUE_TYPE pWlanOpcodeValueType);
static WlanQueryInterfaceProto local_WlanQueryInterface = 0;
//
typedef DWORD (WINAPI *WlanRegisterNotificationProto)
    (HANDLE hClientHandle, DWORD dwNotifSource, BOOL bIgnoreDuplicate,
     WLAN_NOTIFICATION_CALLBACK funcCallback, PVOID pCallbackContext,
     PVOID pReserved, PDWORD pdwPrevNotifSource);
static WlanRegisterNotificationProto local_WlanRegisterNotification = 0;
//
typedef VOID (WINAPI *WlanFreeMemoryProto)(PVOID pMemory);
static WlanFreeMemoryProto local_WlanFreeMemory = 0;
//
typedef BOOL (WINAPI *BluetoothFindRadioClose)(HANDLE hFind);
static BluetoothFindRadioClose local_BluetoothFindRadioClose=0;

//
typedef HANDLE (WINAPI *BluetoothFindFirstRadio)(const BLUETOOTH_FIND_RADIO_PARAMS * pbtfrp,HANDLE * phRadio);
static BluetoothFindFirstRadio local_BluetoothFindFirstRadio=0;


static void resolveLibrary()
{
#if !defined( Q_OS_WINCE)
            static volatile bool triedResolve = false;

    if (!triedResolve) {
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&local_WlanOpenHandle));
#endif

        if (!triedResolve) {
            local_WlanOpenHandle = (WlanOpenHandleProto)
                                   QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanOpenHandle");
            local_WlanRegisterNotification = (WlanRegisterNotificationProto)
                                             QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanRegisterNotification");
            local_WlanEnumInterfaces = (WlanEnumInterfacesProto)
                                       QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanEnumInterfaces");
            //            local_WlanGetAvailableNetworkList = (WlanGetAvailableNetworkListProto)
            //                QLibrary::resolve(QLatin1String("wlanapi.dll"), "WlanGetAvailableNetworkList");
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
//

//static WlanRegisterNotificationProto local_WlanRegisterNotification = 0;
Q_GLOBAL_STATIC(QSystemNetworkInfoPrivate, qsystemNetworkInfoPrivate)

typedef struct _DISPLAY_BRIGHTNESS {
    UCHAR ucDisplayPolicy;
    UCHAR ucACBrightness;
    UCHAR ucDCBrightness;
} DISPLAY_BRIGHTNESS, *PDISPLAY_BRIGHTNESS;


#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
static PWLAN_CONNECTION_ATTRIBUTES  getWifiConnectionAttributes()
{
    DWORD version =  0;
    HANDLE clientHandle = NULL;
    DWORD result;
    PWLAN_CONNECTION_ATTRIBUTES  connAtts = NULL;

    PWLAN_INTERFACE_INFO_LIST interfacesInfoList = NULL;
    result = local_WlanOpenHandle( 2, NULL, &version, &clientHandle );
    if( result != ERROR_SUCCESS) {
//       qWarning() << "Error opening Wlanapi" << result ;
        local_WlanFreeMemory(connAtts);
        local_WlanCloseHandle(clientHandle,  0);
        return NULL;
    }
    result = local_WlanEnumInterfaces(clientHandle, NULL, &interfacesInfoList);

    if( result != ERROR_SUCCESS) {
 //       qWarning() << "Error in enumerating wireless interfaces" << result;
        local_WlanCloseHandle(clientHandle, NULL);
        local_WlanFreeMemory(connAtts);
        return NULL;
    }

    for( uint i = 0; i < interfacesInfoList->dwNumberOfItems; i++ ) {
        PWLAN_INTERFACE_INFO interfaceInfo = &interfacesInfoList->InterfaceInfo[i];
        GUID& guid = interfaceInfo->InterfaceGuid;
        WLAN_INTERFACE_STATE wlanInterfaceState = interfaceInfo->isState;

        if( wlanInterfaceState == wlan_interface_state_not_ready ) {
           // qWarning() << "Interface not ready";
            continue;
        }

        ULONG size = 0;
        result = local_WlanQueryInterface( clientHandle, &guid,  wlan_intf_opcode_current_connection, NULL, &size, (PVOID*) &connAtts, NULL );
        if( result != ERROR_SUCCESS ) {
         //   qWarning() << "Error querying wireless interfaces"<< result ;
            continue;
        }
    }
    local_WlanCloseHandle(clientHandle, NULL);
    return connAtts;
}
#endif

////////
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
    QString tmpLang = currentLanguage();
    if(currentLanguageStr != tmpLang) {
        currentLanguageStr = tmpLang;
        emit currentLanguageChanged(currentLanguageStr);
    }

    QTimer::singleShot(1000, this,SLOT(currentLanguageTimeout()));
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
//WM_SETTINGSCHANGE


// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    QDir transDir(QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    QStringList langList;
    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << "qt_*.qm" ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach(QString localeName, localeList) {
            QString lang = localeName.mid(3,2);
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
    case QSystemInfo::Os :
        {
            OSVERSIONINFOEX versionInfo;
            versionInfo .dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

            GetVersionEx((OSVERSIONINFO *) &versionInfo);
            return QString::number(versionInfo.dwMajorVersion) +"."
                    +QString::number(versionInfo.dwMinorVersion)+"."
                    +QString::number(versionInfo.dwBuildNumber)+"."
                    +QString::number(versionInfo.wServicePackMajor)+"."
                    +QString::number(versionInfo.wServicePackMinor);
        }
        break;
    case QSystemInfo::QtCore :
       return  qVersion();
       break;
    case QSystemInfo::WrtCore :
        {
        }
       break;
    case QSystemInfo::Webkit :
        {
        }
       break;
    case QSystemInfo::ServiceFramework :
        {
        }
       break;
    case QSystemInfo::WrtExtensions :
        {
        }
       break;
    case QSystemInfo::ServiceProvider :
        {
        }
       break;
    case QSystemInfo::NetscapePlugin :
        {
        }
       break;
    case QSystemInfo::WebApp :
        {
        }
       break;
   case QSystemInfo::Firmware :
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
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
//            WSADATA wsd;
//            WSAStartup (MAKEWORD(1,0), &wsd);
//
//            WSAQUERYSET wsaq;
//            HANDLE hLookup;
//            qWarning() << hLookup;
//            union {
//                CHAR buf[5000];
//                double __unused; // ensure proper alignment
//            };
//            LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET) buf;
//            DWORD dwSize  = sizeof(buf);
//            BOOL bHaveName;
//            ZeroMemory(&wsaq, sizeof(wsaq));
//            wsaq.dwSize = sizeof(wsaq);
//            wsaq.dwNameSpace = NS_BTH;
//            wsaq.lpcsaBuffer = NULL;
//            int res = WSALookupServiceBegin (&wsaq, LUP_RES_SERVICE, &hLookup);
//            qWarning() << __FUNCTION__ << res;
//            if (ERROR_SUCCESS != res) {
//                qWarning() << "WSALookupServiceBegin failed" << WSAGetLastError() << hLookup;
//                return false;
//            } else {
//                return true;
//            }
        //
//
            resolveLibrary();
            if(local_BluetoothFindFirstRadio == 0 ) {
                qWarning() << "Bluetooth library could not resolve or be loaded";
                return false;
            }
            BLUETOOTH_FIND_RADIO_PARAMS  radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
            HANDLE radio;
            if(local_BluetoothFindFirstRadio(&radioParams, &radio) != NULL) {
           //     qWarning() << "available";
                featureSupported = true;
                local_BluetoothFindRadioClose(radio);
            } else {

         //       qWarning() << "Not available" << GetLastError();
            }
#endif
            //            qWarning() << IsBluetoothVersionAvailable("1","0");
            //            QSystemNetworkInfo ni;
            //            if(ni.interfaceForMode(QSystemNetworkInfo::BluetoothMode).isValid()) {
            //                featureSupported = true;
            //            }
        }
        break;
    case QSystemInfo::CameraFeature :
        {
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)

            ICreateDevEnum *devEnum = NULL;
            IEnumMoniker *monikerEnum = NULL;
            QUuid qSystemDeviceEnumClsid(0x62BE5D10,0x60EB,0x11d0,0xBD,0x3B,0x00,0xA0,0xC9,0x11,0xCE,0x86);
            QUuid qCreateDevEnumIid = "29840822-5B84-11D0-BD3B-00A0C911CE86";
            QUuid qVideoInputDeviceCategoryClsid(0x860BB310,0x5D01,0x11d0,0xBD,0x3B,0x00,0xA0,0xC9,0x11,0xCE,0x86);

            HRESULT hr = CoCreateInstance(qSystemDeviceEnumClsid, NULL,
                                          CLSCTX_INPROC_SERVER, qCreateDevEnumIid,
                                          reinterpret_cast<void**>(&devEnum));
            if (hr == S_OK) {
                hr = devEnum->CreateClassEnumerator( qVideoInputDeviceCategoryClsid, &monikerEnum, 0);
                if(hr != S_FALSE) {
                  //  qWarning() << "available";
                    featureSupported = true;
                    break;
                } else {
                 //   qWarning() << "Not available";
                }
            }
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
//            NLED_COUNT_INFO info;
//            NLedDriverGetDeviceInfo(NLED_COUNT_INFO_ID,&info);
//            if(info > 0) {
//                    featureSupported = true;
 //           }
#else
#endif
        }
        break;
    case QSystemInfo::MemcardFeature :
        {
            QSystemStorageInfo mi;
            QStringList drives = mi.logicalDrives();
            foreach(QString drive, drives) {
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
//            VIBRATEDEVICECAPS caps;
//            if(VibrateGetDeviceCaps(&caps) != 0) {
//                featureSupported = true;
 //           }
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
//            HLOCATION location;
//            location = LocationOpen(LOCATION_FRAMEWORK_VERSION_CURRENT,NULL,0);
//            if(location != NULL) {
//                featureSupported = true;
//                LocationClose(location);
//            }
#else
#endif
        }
        break;
    case QSystemInfo::VideoOutFeature :
        {
           //IOCTL_VIDEO_QUERY_AVAIL_MODES
//VIDEO_MODE_INFORMATION vInfo;
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
    default:
        featureSupported = false;
        break;
    };
    return featureSupported;
}


//////// QSystemNetworkInfo
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkMode)
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkStatus)

#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)

void wlanNotificationCallback(PWLAN_NOTIFICATION_DATA pNotifyData, QSystemNetworkInfoPrivate *netInfo)
{
    // xp only supports disconnected and complete
   // qWarning() << __FUNCTION__ << pNotifyData->NotificationCode;
    switch(pNotifyData->NotificationCode) {
    case wlan_notification_acm_scan_complete:
        break;
    case wlan_notification_acm_disconnecting:
       // qWarning() << "disconnecting";
        break;
    case wlan_notification_acm_disconnected:
    case wlan_notification_acm_connection_attempt_fail:
    case wlan_notification_acm_network_not_available:
        netInfo->emitNetworkStatusChanged(QSystemNetworkInfo::WlanMode,
                                          QSystemNetworkInfo::NoNetworkAvailable);
    //    qWarning() << "disconnected";
        break;
    case wlan_notification_acm_profile_name_change:
    //    qWarning() << "profile name change" << pNotifyData->pData;
        break;
    case  wlan_notification_msm_connected:
        netInfo->emitNetworkStatusChanged(QSystemNetworkInfo::WlanMode,
                                          QSystemNetworkInfo::Connected);
    //    qWarning() << "connection complete";
        break;
    case wlan_notification_acm_connection_start:
        netInfo->emitNetworkStatusChanged(QSystemNetworkInfo::WlanMode,
                                          QSystemNetworkInfo::Searching);
        //            qWarning() << "connection start";
        break;
    case wlan_notification_msm_signal_quality_change:
 //       qWarning() << "signal quality change" <<  QString::number((ulong)pNotifyData->pData);
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
  //   qWarning() << "Windows version"<<QSysInfo::WindowsVersion << QSysInfo::WV_VISTA << QSysInfo::WV_WINDOWS7;
     switch(QSysInfo::WindowsVersion) {
     case  QSysInfo::WV_VISTA:
     case QSysInfo::WV_WINDOWS7:
         break;
     default:
         QTimer::singleShot(timerMs, this, SLOT(networkStrengthTimeout()));
         break;
     };
 }

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
    if(hWlan != 0)
         local_WlanCloseHandle(hWlan, 0);
}

void QSystemNetworkInfoPrivate::startWifiCallback()
{
    if(networkStatus(QSystemNetworkInfo::WlanMode) != QSystemNetworkInfo::Connected
    && wlanCallbackInitialized){
        return;
    }
    DWORD version =  0;
    hWlan = NULL;
    DWORD result;
    result = local_WlanOpenHandle(2, NULL, &version, &hWlan );
    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error opening Wlanapi 2" << result ;
        return ;
    }
    if( result != ERROR_SUCCESS) {
        qWarning() << "Error in enumerating wireless interfaces" << result;
        return ;
    }
    if (ERROR_SUCCESS != local_WlanRegisterNotification(hWlan,
                                                  WLAN_NOTIFICATION_SOURCE_ALL,
                                                  true,
                                                  WLAN_NOTIFICATION_CALLBACK(wlanNotificationCallback),
                                                  this, 0, 0)) {
        qWarning() << "failed";
    } else {
        wlanCallbackInitialized = true;
    }
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

void QSystemNetworkInfoPrivate::emitNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode,int strength)
{
    //qWarning() << __FUNCTION__ << mode << strength;
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
//   qWarning() << __FUNCTION__;
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(QSystemNetworkInfo::NetworkMode mode, modeList) {
       networkSignalStrength(mode);
    }
    switch(QSysInfo::WindowsVersion) {
    case QSysInfo::WV_VISTA:
    case QSysInfo::WV_WINDOWS7:
        break;
    default:
        QTimer::singleShot(timerMs, this, SLOT(networkStrengthTimeout()));
        break;
    };
 }

void QSystemNetworkInfoPrivate::networkStatusTimeout()
{
//   qWarning() << __FUNCTION__;
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(QSystemNetworkInfo::NetworkMode mode, modeList) {
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
            PWLAN_CONNECTION_ATTRIBUTES connAtts = getWifiConnectionAttributes();
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
            DWORD version =  0;
            DWORD result;

            PWLAN_INTERFACE_INFO_LIST interfacesInfoList = NULL;
            if (hWlan ==0) {
                result = local_WlanOpenHandle( 2, NULL, &version, &hWlan );
                if( result != ERROR_SUCCESS ) {
                    qWarning() << "Error opening Wlanapi 3" << result ;
                    local_WlanCloseHandle(hWlan,  0);
                    return 0;
                }
            }
            result = local_WlanEnumInterfaces(hWlan, NULL, &interfacesInfoList);

            if( result != ERROR_SUCCESS) {
                qWarning() << "Error in enumerating wireless interfaces" << result;
                local_WlanCloseHandle(hWlan,  0);
                return 0;
            }

            for( uint i = 0; i < interfacesInfoList->dwNumberOfItems; i++ ) {
                PWLAN_INTERFACE_INFO interfaceInfo = &interfacesInfoList->InterfaceInfo[i];
                GUID& guid = interfaceInfo->InterfaceGuid;
                WLAN_INTERFACE_STATE wlanInterfaceState = interfaceInfo->isState;

                if( wlanInterfaceState == wlan_interface_state_not_ready ) {
                    qWarning() << "Interface not ready";
                    continue;
                }

                ULONG size = 0;
                PWLAN_CONNECTION_ATTRIBUTES  connAtts = NULL;
                result = local_WlanQueryInterface( hWlan, &guid,  wlan_intf_opcode_current_connection, NULL, &size, (PVOID*) &connAtts, NULL );

                if( result != ERROR_SUCCESS ) {
//                    qWarning() << "Error querying wireless interfaces"<< result ;
                    continue;
                }
                ulong sig =  connAtts->wlanAssociationAttributes.wlanSignalQuality;
                local_WlanFreeMemory(connAtts);
                local_WlanFreeMemory(interfacesInfoList);

                if (sig != wifiStrength) {
                   emit networkSignalStrengthChanged(mode, sig);
                //   qWarning() << "signal strength changed" << sig;
                   wifiStrength = sig;
                }
                return sig;
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
//            qWarning() << strength << ethStrength;

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

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return QString();
}

// Mobile Network Code
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
            netname = "";
            PWLAN_CONNECTION_ATTRIBUTES connAtts = getWifiConnectionAttributes();
            if(connAtts != NULL) {
                DOT11_SSID ssid;
                ssid = connAtts->wlanAssociationAttributes.dot11Ssid;
                for(uint i = 0; i < ssid.uSSIDLength;i++) {
                    netname += ssid.ucSSID[i];
                }
            }
            local_WlanFreeMemory(connAtts);
#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            WMIHelper *wHelper;
            wHelper = new WMIHelper(this);
            wHelper->setWmiNamespace("root/cimv2");
            wHelper->setClassName("Win32_ComputerSystem");
            wHelper->setClassProperty(QStringList() << "Domain");

            QVariant v = wHelper->getWMIData();
            netname = v.toString();
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
        if (!netInterface.isValid() || (netInterface.flags() & QNetworkInterface::IsLoopBack)) {
            continue;
        }

#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)

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
            qWarning() << "DeviceIo result is false";
            return QNetworkInterface();
            continue;
        }

        oid = OID_GEN_PHYSICAL_MEDIUM;
        bytesWritten = 0;
        result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                 &physicalMedium, sizeof(physicalMedium), &bytesWritten, 0);

//        qWarning()
//                << netInterface.name()
//                << netInterface.hardwareAddress();
//
//        qWarning() << medium << physicalMedium << result;

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
            case NdisPhysicalMediumWiMax:
                {
                    if(mode == QSystemNetworkInfo::WimaxMode) {
                        return netInterface;
                    }
                }
                break;
            };
        }
#endif
    } //end interfaceList

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
//    qWarning() <<netInterface.name() << deviceSettings.value("DhcpDefaultGateway");

    if(!deviceSettings.value("DhcpDefaultGateway").toStringList().isEmpty()
         || !deviceSettings.value("DefaultGateway").toStringList().isEmpty()) {
        isDefaultGateway = true;
     //   qWarning() <<"Default!"<<netInterface.humanReadableName() << netInterface.hardwareAddress();
    }
    return isDefaultGateway;
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
//#if WINVER > 0x0600
#if !defined( Q_CC_MINGW) && !defined( Q_OS_WINCE)
    WMIHelper *wHelper;
    wHelper = new WMIHelper(this);
    wHelper->setWmiNamespace("root/wmi");
    wHelper->setClassName("WmiMonitorBrightness");
    wHelper->setClassProperty(QStringList() << "CurrentBrightness");

    QVariant v = wHelper->getWMIData();

    return v.toUInt();
#endif
//#else
//    //    Q_UNUSED(screen);
   // qint32 brightness = 0;
   // QString brightness;

//    HANDLE display = CreateFile(L"\\\\.\\LCD",GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE
//                                ,NULL,OPEN_EXISTING,0,NULL);
//    if(display != INVALID_HANDLE_VALUE) {
//        DISPLAY_BRIGHTNESS brightnessBuffer;
//        DWORD bytesReturned = 0;
//
//        if(DeviceIoControl(display,IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,
//                           NULL,0,&brightnessBuffer,sizeof(brightnessBuffer),
//                           &bytesReturned,NULL)) {
//            QString brightness;//((QChar *)brightnessBuffer.ucACBrightness);
//
//            char *uc = (char *)brightnessBuffer.ucACBrightness;
////            brightness = uc;
////            for(uint i = 0; sizeof(brightnessBuffer.ucACBrightness); i++) {
////                brightness += QChar(brightnessBuffer.ucACBrightness);
////            }
//
//            qWarning()
//                    << sizeof(brightnessBuffer.ucACBrightness)
//                    << QChar(brightnessBuffer.ucACBrightness)
//                 //   << brightness
//                    << brightnessBuffer.ucDisplayPolicy
//                    << brightnessBuffer.ucDCBrightness
//                    << brightnessBuffer.ucACBrightness;
//
//        }
//        CloseHandle(display);
//    } else {
//        qWarning() << "invalid handle";
//    }
  //  UCHAR
//#endif

    // Get the number of physical monitors.
    // vista only
//    HMONITOR hMonitor = NULL;
//
//    QDesktopWidget wid;
//
//    HWND hWnd = wid.screen(screen)->winId();
//    DWORD cPhysicalMonitors = 1;
//    LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;
//
//    // Get the monitor handle.
//    hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
//    if(hMonitor == NULL) {
//        qWarning() << "NULL";
//    }
//    bool bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &cPhysicalMonitors);
////    if (bSuccess) {
//        // Allocate the array of PHYSICAL_MONITOR structures.
//        pPhysicalMonitors = (LPPHYSICAL_MONITOR)malloc(
//                cPhysicalMonitors* sizeof(PHYSICAL_MONITOR));
//
//        if (pPhysicalMonitors != NULL) {
//            bSuccess = GetPhysicalMonitorsFromHMONITOR(
//                    hMonitor, cPhysicalMonitors, pPhysicalMonitors);
//
//            DWORD pdwMonitorCapabilities = 0;
//            DWORD pdwSupportedColorTemperatures = 0;
//
//            GetMonitorCapabilities(hMonitor, &pdwMonitorCapabilities, &pdwSupportedColorTemperatures);
//           // qWarning() << pdwMonitorCapabilities;
//
//            if (pdwMonitorCapabilities & MC_CAPS_BRIGHTNESS) {
//                qWarning() << "XXXXXXXXXXXXXXXX has brightness";
//            } else {
//                qWarning() << "XXXXXXXXXXXXXXXX NO brightness";
//
//            }
//
//            // Close the monitor handles.
//            bSuccess = DestroyPhysicalMonitors(
//                    cPhysicalMonitors,
//                    pPhysicalMonitors);
//
//            // Free the array.
//            free(pPhysicalMonitors);
//        } else {
//            qWarning() << "XXXXXXXXXXXXXX" << GetLastError();
//        }
//
//    }
    //////////////////////////////////

    return -1;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    QDesktopWidget wid;
    HWND hWnd = wid.screen(screen)->winId();
    HDC deviceContextHandle = GetDC(hWnd);
    int bpp = GetDeviceCaps(deviceContextHandle ,BITSPIXEL);
    int planes = GetDeviceCaps(deviceContextHandle, PLANES);
    if(planes > 1) {
        bpp = 1 << planes;
    }
    ReleaseDC(NULL, deviceContextHandle);

    return bpp;
}

//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
        : QObject(parent)
{
}


QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    qint64 freeBytes;
    qint64 totalBytes;
    qint64 totalFreeBytes;

    bool ok = GetDiskFreeSpaceEx((WCHAR *)driveVolume.utf16(),(PULARGE_INTEGER)&freeBytes, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes);
    if(!ok)
        totalFreeBytes = 0;
    return totalFreeBytes;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    qint64 freeBytes;
    qint64 totalBytes;
    qint64 totalFreeBytes;

    bool ok = GetDiskFreeSpaceEx((WCHAR *)driveVolume.utf16(),(PULARGE_INTEGER)&freeBytes, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes);
    if(!ok)
        totalBytes = 0;
    return totalBytes;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    uint result =  GetDriveType((WCHAR *)driveVolume.utf16());
    switch(result) {
    case 0:
    case 1: //unknown
        return QSystemStorageInfo::NoDrive;
        break;
    case 2://removable
        return QSystemStorageInfo::RemovableDrive;
        break;
    case 3:   //fixed
        return QSystemStorageInfo::InternalDrive;
        break;
    case 4: //remote:
        return QSystemStorageInfo::RemoteDrive;
        break;
    case 5: //cdrom
        return QSystemStorageInfo::CdromDrive;
        break;
    case 6: //ramdisk
        break;
    };
    return QSystemStorageInfo::NoDrive;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    QStringList drivesList;
    QFileInfoList drives = QDir::drives();
    foreach(QFileInfo drive, drives) {
        QString letter =  drive.absoluteFilePath();
        letter.chop(1);
        if(totalDiskSpace(letter) > 0) {
            drivesList.append(letter);
        }
    }
    return drivesList;
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
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

    int mouseResult = GetSystemMetrics(SM_CMOUSEBUTTONS);
    if(mouseResult > 0) {
        if((methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
            methods |= QSystemDeviceInfo::Mouse;

        }
    }
    int tabletResult = GetSystemMetrics(SM_TABLETPC);
    if(tabletResult > 0) {
        if((methods & QSystemDeviceInfo::SingleTouch) != QSystemDeviceInfo::SingleTouch) {
            methods |= QSystemDeviceInfo::SingleTouch;

        }
    }
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

    if(status.BatteryFlag & BATTERY_FLAG_CHARGING)
        return QSystemDeviceInfo::WallPowerChargingBattery;
    if(status.ACLineStatus  == AC_LINE_ONLINE)
        return QSystemDeviceInfo::WallPower;
    if(status.ACLineStatus  == AC_LINE_OFFLINE)
        return QSystemDeviceInfo::BatteryPower;

    return QSystemDeviceInfo::UnknownPower;
}

QString QSystemDeviceInfoPrivate::imei()
{
//    if(this->getSimStatus() == QSystemDeviceInfo::SimNotAvailable)
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::imsi()
{
//    if(getSimStatus() == QSystemDeviceInfo::SimNotAvailable)
        return "Sim Not Available";
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

    //    wHelper->setClassName("Win32_Processor");
//    wHelper->setClassProperty(QStringList() << "Architecture");
//    QVariant v = wHelper->getWMIData();
//    QString model;
//    switch(v.toUInt()) {
//    case 0:
//        model = "x86 ";
//        break;
//    case 1:
//        model = "MIPS ";
//        break;
//    case 2:
//        model = "Alpha ";
//        break;
//    case 3:
//        model = "PowerPC ";
//        break;
//    case 6:
//        model = "Intel Itanium ";
//        break;
//    case 9:
//        model = "x64 ";
//        break;
//    }


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

int QSystemDeviceInfoPrivate::batteryLevel() const
{
#ifdef Q_OS_WINCE
    SYSTEM_POWER_STATUS_EX status;
    if(GetSystemPowerStatusEx(&statusEx, true) ) {
        qWarning() <<"battery level" <</* status.ACLineStatus << status.BatteryFlag <<*/ status.BatteryLifePercent;
        return status.BatteryLifePercent;
} else {
       qWarning() << "Battery status failed";
    }
#else
    SYSTEM_POWER_STATUS status;
    if(GetSystemPowerStatus( &status) ) {
        return status.BatteryLifePercent;
}
#endif
    return 0;
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
            return QSystemDeviceInfo::SingleAvailable;
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
#endif
    return false;
}

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
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
}


bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    if(screenSaverSecure)
        return false;

    return SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,true,0,SPIF_SENDWININICHANGE);
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    return !screenSettings.value("SCRNSAVE.EXE").toString().isEmpty();
}
bool QSystemScreenSaverPrivate::isScreenLockOn()
{
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    if(screenSettings.value("ScreenSaverIsSecure").toString() == "1") {
        return true;
    }
    return false;
}


QT_END_NAMESPACE
