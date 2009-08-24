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
#include "qsysteminfo_p.h"
#include "wmihelper.h"
#include <qt_windows.h>

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

//#include <Winsock2.h>
//#include <mswsock.h>
//#include <ntddndis.h>

#include <locale.h>
#include <Wlanapi.h>
#include <Wtsapi32.h>
#include <Bthsdpdef.h>
//#include <ws2bth.h>

//#include <bthddi.h>

#include <BluetoothAPIs.h>
#include <Dshow.h>
#include <af_irda.h>

#ifdef Q_OS_WINCE
#include <vibrate.h>
#include <Led_drvr.h>
#include <simmgr.h>
#include <Ifapi.h>
#include <Winbase.h>
#endif

#define _WCHAR_T_DEFINED
#define _TIME64_T_DEFINED


typedef struct _DISPLAY_BRIGHTNESS {
    UCHAR ucDisplayPolicy;
    UCHAR ucACBrightness;
    UCHAR ucDCBrightness;
} DISPLAY_BRIGHTNESS, *PDISPLAY_BRIGHTNESS;



QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{

}

// 2 letter ISO 639-1
QString QSystemInfoPrivate::currentLanguage() const
{////Win32_Product Language
    return QString(setlocale(LC_ALL,NULL)).left(2);
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    QDir transDir(QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    QStringList langList;
//qWarning() << transDir.absolutePath();
    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << "qt_*.qm" ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach(QString localeName, localeList) {
            QString lang = localeName.mid(3,2);
            if(!langList.contains(lang) && !lang.isEmpty() && !lang.contains("help")) {
                langList <<lang;
            }
        }
        return langList;
    }
    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
/*QPair< int, double >*/ QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
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
    return QString(setlocale(LC_ALL,"")).mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
            BLUETOOTH_FIND_RADIO_PARAMS  radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
            HANDLE radio;
            if(BluetoothFindFirstRadio(&radioParams, &radio) != NULL) {
                qWarning() << "available";
                featureSupported = true;
                BluetoothFindRadioClose(radio);
            } else {

                qWarning() << "Not available" << GetLastError();
            }
        }
        break;
    case QSystemInfo::CameraFeature :
        {
            ICreateDevEnum *devEnum = NULL;
            IEnumMoniker *monikerEnum = NULL;

            HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                          reinterpret_cast<void**>(&devEnum));
            if (hr == S_OK) {
                hr = devEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &monikerEnum, 0);
                if(hr != S_FALSE) {
                    qWarning() << "available";
                    featureSupported = true;
                    break;
                } else {
                    qWarning() << "Not available";
                }
            }
        }
        break;
    case QSystemInfo::FmradioFeature :
        {
            //PageId.FMRadio?
        }
        break;
    case QSystemInfo::IrFeature :
        {
            WMIHelper *wHelper;
            wHelper = new WMIHelper();
            QVariant v = wHelper->getWMIData("root/cimv2", "Win32_InfraredDevice", "ConfigManagerErrorCode");
            if(v.toUInt() == 1) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::LedFeature :
        {
#ifdef Q_OS_WINCE
            NLED_COUNT_INFO info;
            NLedDriverGetDeviceInfo(NLED_COUNT_INFO_ID,&info);
            if(info > 0) {
                    featureSupported = true;
            }
#else
#endif
        }
        break;
    case QSystemInfo::MemcardFeature :
        {
            QSystemMemoryInfo mi;
            QStringList drives = mi.listOfVolumes();
            foreach(QString drive, drives) {
                if(mi.volumeType(drive) == QSystemMemoryInfo::Removable) {
                    featureSupported = true;
                }
            }
        }
        break;
    case QSystemInfo::UsbFeature :
        {
            WMIHelper *wHelper;
            wHelper = new WMIHelper();
            QVariant v = wHelper->getWMIData("root/cimv2", "Win32_USBHub", "ConfigManagerErrorCode");
            qWarning() << v.toUInt();
            if(v.toUInt() == 0) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::VibFeature :
        {
#ifdef Q_OS_WINCE
            VIBRATEDEVICECAPS caps;
            if(VibrateGetDeviceCaps(&caps) != 0) {
                featureSupported = true;
            }
#else
#endif
        }
        break;
    case QSystemInfo::WlanFeature :
        {
            QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
            if (interfaces.isEmpty())
                interfaces = QNetworkInterface::allInterfaces();

            while (!interfaces.isEmpty()) {
                QNetworkInterface netInterface = interfaces.takeFirst();

                if (!netInterface.isValid())
                    continue;

                if (netInterface.flags() & QNetworkInterface::IsLoopBack)
                    continue;

                unsigned long oid;
                DWORD bytesWritten;

                NDIS_MEDIUM medium;
                NDIS_PHYSICAL_MEDIUM physicalMedium;

                HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(netInterface.name()).utf16(), 0,
                                           FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
                if (handle == INVALID_HANDLE_VALUE)
                    return  false;

                oid = OID_GEN_MEDIA_SUPPORTED;
                bytesWritten = 0;
                bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                              &medium, sizeof(medium), &bytesWritten, 0);
                if (!result) {
                    CloseHandle(handle);
                    return  false;
                }
                oid = OID_GEN_PHYSICAL_MEDIUM;
                bytesWritten = 0;
                result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                         &physicalMedium, sizeof(physicalMedium), &bytesWritten, 0);
                if (!result) {
                    CloseHandle(handle);
                    if (medium == NdisMedium802_3) {
                        if(physicalMedium ==   NdisPhysicalMediumWirelessLan) {
                            featureSupported = true;
                            break;
                        }
                    }
                }
            } //end while interfaces
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
            HLOCATION location;
            location = LocationOpen(LOCATION_FRAMEWORK_VERSION_CURRENT,NULL,0);
            if(location != NULL) {
                featureSupported = true;
                LocationClose(location);
            }
#else
#endif
        }
        break;
    case QSystemInfo::VideoOutFeature :
        {
           //IOCTL_VIDEO_QUERY_AVAIL_MODES
//VIDEO_MODE_INFORMATION vInfo;
            WMIHelper *wHelper;
            wHelper = new WMIHelper();
            QVariant v = wHelper->getWMIData("root/wmi", "VideoModeDescriptor", "VideoStandardType");
            qWarning() << v.toUInt() ;
            if(v.toUInt() > 5) {
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
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
            DWORD version =  0;
            HANDLE clientHandle = NULL;
            DWORD result;

            PWLAN_INTERFACE_INFO_LIST interfacesInfoList = NULL;
            result = WlanOpenHandle( 2, NULL, &version, &clientHandle );
            if( result != ERROR_SUCCESS != result ) {
                qWarning() << "Error opening Wlanapi" << result ;
                return QSystemNetworkInfo::NoNetworkAvailable;
            }
            result = WlanEnumInterfaces(clientHandle, NULL, &interfacesInfoList);

            if( result != ERROR_SUCCESS) {
                qWarning() << "Error in enumerating wireless interfaces" << result;
                return QSystemNetworkInfo::NoNetworkAvailable;
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
                PWLAN_CONNECTION_ATTRIBUTES   connAtts = NULL;
                result = WlanQueryInterface( clientHandle, &guid,  wlan_intf_opcode_current_connection, NULL, &size, (PVOID *) &connAtts, NULL );

                if( result != ERROR_SUCCESS ) {
                    qWarning() << "Error querying wireless interfaces"<< result ;
                    continue;
                }

                if(connAtts->isState  == wlan_interface_state_connected) {
                    qWarning() << "Is connected";
                    return QSystemNetworkInfo::Connected;
                } else {
                    qWarning() << "is not connected";
                }
                WlanFreeMemory(connAtts);
            }
        }
        break;
        case QSystemNetworkInfo::EthMode:
        {
            WMIHelper *wHelper;
            wHelper = new WMIHelper();
            QVariant v = wHelper->getWMIData("root/cimv2", "Win32_NetworkConnection", "ConnectionState");
            qWarning() << v.toString();
        }
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    qWarning() << Q_FUNC_INFO << mode;
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
            DWORD version =  0;
            HANDLE clientHandle = NULL;
            DWORD result;

            PWLAN_INTERFACE_INFO_LIST interfacesInfoList = NULL;
            result = WlanOpenHandle( 2, NULL, &version, &clientHandle );
            if( result != ERROR_SUCCESS != result ) {
                qWarning() << "Error opening Wlanapi" << result ;
                return QSystemNetworkInfo::NoNetworkAvailable;
            }
            result = WlanEnumInterfaces(clientHandle, NULL, &interfacesInfoList);

            if( result != ERROR_SUCCESS) {
                qWarning() << "Error in enumerating wireless interfaces" << result;
                return QSystemNetworkInfo::NoNetworkAvailable;
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
                result = WlanQueryInterface( clientHandle, &guid,  wlan_intf_opcode_current_connection, NULL, &size, (PVOID*) &connAtts, NULL );

                if( result != ERROR_SUCCESS ) {
                    qWarning() << "Error querying wireless interfaces"<< result ;
                    continue;
                }
               ulong sig =  connAtts->wlanAssociationAttributes.wlanSignalQuality;
                WlanFreeMemory(connAtts);
                return sig;
            }
        }
        break;
        case QSystemNetworkInfo::EthMode:
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

QString QSystemNetworkInfoPrivate::operatorName()
{
    return "No Operator";
}

QString QSystemNetworkInfoPrivate::wlanSsid()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    return QString();
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
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/wmi", "WmiMonitorBrightness", "CurrentBrightness");
    qWarning() << v.toUInt();
    return v.toUInt();
//#else
//    //    Q_UNUSED(screen);
//    qint32 brightness = 0;
//    HANDLE display = CreateFile(L"\\\\.\\LCD",FILE_ANY_ACCESS,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
//    if(display != INVALID_HANDLE_VALUE) {
//        DISPLAY_BRIGHTNESS brightnessBuffer;
//        memset( &brightnessBuffer, 0, sizeof(brightnessBuffer));
//        DWORD bytesReturned = 0;
//        if(DeviceIoControl(display,IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,
//                           NULL,0,&brightnessBuffer,256,&bytesReturned,NULL)) {
//            if(bytesReturned > 0) {
//                brightness = brightnessBuffer.ucACBrightness;
//                qWarning()
//                        << brightnessBuffer.ucDisplayPolicy
//                        << brightnessBuffer.ucDCBrightness
//                        << brightnessBuffer.ucACBrightness
//                        << static_cast<int>(brightnessBuffer.ucACBrightness);
//            } else {
//                qWarning() << "bytes not returned" << bytesReturned << GetLastError();
//            }
//        }
//
//        CloseHandle(display);
//    } else {
//        qWarning() << "invalid handle";
//    }
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
// GetMonitorBrightness
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

//    } else {
//        qWarning() << "not successful" << GetLastError();
//    }

    //GetMonitorBrightness
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
    qWarning() << planes << bpp;
    if(planes > 1) {
        bpp = 1 << planes;
    }
    ReleaseDC(NULL, deviceContextHandle);

    return bpp;
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
    qint64 freeBytes;
    qint64 totalBytes;
    qint64 totalFreeBytes;

    bool ok = GetDiskFreeSpaceEx(driveVolume.utf16(),(PULARGE_INTEGER)&freeBytes, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes);
//    qWarning() << ok << freeBytes << totalBytes << totalFreeBytes;
    if(!ok)
        totalFreeBytes = -1;
    return totalFreeBytes;
}

qint64 QSystemMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    qint64 freeBytes;
    qint64 totalBytes;
    qint64 totalFreeBytes;

    bool ok = GetDiskFreeSpaceEx(driveVolume.utf16(),(PULARGE_INTEGER)&freeBytes, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes);
//    qWarning() << ok << freeBytes << totalBytes << totalFreeBytes;
    if(!ok)
        totalBytes = -1;
    return totalBytes;
}

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::volumeType(const QString &driveVolume)
{
    Q_UNUSED(driveVolume);
    uint result =   GetDriveType(driveVolume.utf16());
    switch(result) {
    case 0:
    case 1: //unknown
  return QSystemMemoryInfo::NoVolume;
        break;
    case 2://removable
        return QSystemMemoryInfo::Removable;
        break;
    case 3:   //fixed
        return QSystemMemoryInfo::Internal;
        break;
    case 4: //remote:
        break;
    case 5: //cdrom
        return QSystemMemoryInfo::Cdrom;
        break;
    case 6: //ramdisk
        break;
    };
  return QSystemMemoryInfo::NoVolume;
}

QStringList QSystemMemoryInfoPrivate::listOfVolumes()
{
    HANDLE fndHld = INVALID_HANDLE_VALUE;
    WCHAR volumeName[MAX_PATH] = L"";
    QStringList drivesList;
    bool ok;
    fndHld = FindFirstVolumeW(volumeName, ARRAYSIZE(volumeName));
//    qWarning() <<"First volume"<< QString::fromStdWString(VolumeName);

    DWORD count = MAX_PATH + 1;
    PWCHAR pathNames = (PWCHAR) new BYTE [count * sizeof(WCHAR)];

//    if ( !pathNames )  {
//        //  If memory can't be allocated, return.
//        return QStringList();
//    }

    ok = GetVolumePathNamesForVolumeNameW( volumeName, pathNames, count, &count );

    QString drive =  QString::fromWCharArray(pathNames);
    if(!drivesList.contains(drive))
        drivesList << drive;

    for (;;)   {
        ok = FindNextVolumeW(fndHld, volumeName, ARRAYSIZE(volumeName));
        if ( !ok )  {
            break;
        }

        ok = GetVolumePathNamesForVolumeNameW( volumeName, pathNames, count, &count );

        QString drive =  QString::fromWCharArray(pathNames);
        if(!drivesList.contains(drive))
            drivesList << drive;
        if ( !ok )  {
            if (GetLastError() != ERROR_NO_MORE_FILES)  {
                break;
            }
        }
    }
    FindVolumeClose(fndHld);
    fndHld = INVALID_HANDLE_VALUE;
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

QSystemDeviceInfo::InputMethods QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethod methods;
    return methods;
}


QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
#ifdef Q_OS_WINCE
    SYSTEM_POWER_STATUS_EX statusEx;
    GetSystemPowerStatusEx(&statusEx, true);
    if(statusEx.ACLineStatus  == AC_LINE_ONLINE;
        return QSystemDeviceInfo::WallPower;
    if(statusEx.BatteryFlag & BATTERY_FLAG_CHARGING)
        return QSystemDeviceInfo::BatteryPower;
#else
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_Battery", "BatteryStatus");
    if(v.toUInt() == 1) {
        return QSystemDeviceInfo::BatteryPower;
    } else {
        return QSystemDeviceInfo::WallPower;
    }
#endif
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
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_ComputerSystem", "Manufacturer");
    return v.toString();
}

QString QSystemDeviceInfoPrivate::model()
{
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_ComputerSystem", "Model");
    return v.toString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_ComputerSystemProduct", "Name");
    return v.toString();
    return QString();
}

bool QSystemDeviceInfoPrivate::isBatteryCharging()
{
    bool isCharging = false;
#ifdef Q_OS_WINCE
    SYSTEM_POWER_STATUS_EX statusEx;
    if(GetSystemPowerStatusEx(&statusEx, true)) {
    if(statusEx.BatteryFlag & BATTERY_FLAG_CHARGING)
        isCharging = true;
}
#else
        SYSTEM_POWER_STATUS status;
    if(GetSystemPowerStatus( &status) ) {
        qWarning() << status.ACLineStatus << status.BatteryFlag << status.BatteryLifePercent;
        if(status.ACLineStatus == 1) {
            isCharging = true;
        }
    } else {
       qWarning() << "Battery status failed";
    }
#endif
    return isCharging;
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
        qWarning() <<"battery level" <</* status.ACLineStatus << status.BatteryFlag <<*/ status.BatteryLifePercent;
        return status.BatteryLifePercent;
} else {
       qWarning() << "Battery status failed";
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
    qWarning() << "Original screenpath is" << screenPath;
}


bool QSystemScreenSaverPrivate::setScreenSaverEnabled(bool state)
{
    if(screenSaverSecure)
        return false;
    QString save =  "0";
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    if(state ) {
        save =  "1";
        screenSettings.setValue("SCRNSAVE.EXE",  screenPath);
    } else {
        screenSettings.remove("SCRNSAVE.EXE");
    }

    screenSettings.setValue("ScreenSaveActive", save);
    qWarning() << screenSettings.value("ScreenSaveActive").toString();
    return screenSettings.value("ScreenSaveActive").toString() == save;
}

bool QSystemScreenSaverPrivate::setScreenBlankingEnabled(bool state)
{
    if(screenSaverSecure)
        return false;
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    QString winDir;
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_OperatingSystem", "SystemDirectory");
    if(settingsPath.contains("Policies")) {
        winDir = "";
    } else {
        winDir = v.toString();
    }

    if(state) {
        screenSettings.setValue("SCRNSAVE.EXE", winDir+"\\scrnsave.scr");
        if(screenSettings.value("SCRNSAVE.EXE").toString().contains("scrnsave.scr")) {
            return true;
        }
    } else {
        screenSettings.setValue("SCRNSAVE.EXE", screenPath);
        if(screenSettings.value("SCRNSAVE.EXE").toString().contains(screenPath)) {
            return true;
        }
    }
    return false;
}

bool QSystemScreenSaverPrivate::screenSaverEnabled()
{
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    return !screenSettings.value("SCRNSAVE.EXE").toString().isEmpty();
}

bool QSystemScreenSaverPrivate::screenBlankingEnabled()
{
    QSettings screenSettings(settingsPath, QSettings::NativeFormat);
    return screenSettings.value("SCRNSAVE.EXE").toString().contains("scrnsave.scr");
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
