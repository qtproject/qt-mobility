<<<<<<< HEAD:systeminfo/qsysteminfo_win.cpp
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

#include <locale.h>
//#include <windows.h>
#include <Wlanapi.h>
#include <Wtsapi32.h>
// #include <afxwin.h>

//#include <Dxva2.lib>
#include <HighLevelMonitorConfigurationAPI.h>
#include <Wbemidl.h>
#include <Bthsdpdef.h>
#include <BluetoothAPIs.h>
#include <Dshow.h>
#include <af_irda.h>
//#include <mswsock.h>

//#include <winsock2.h>
#ifdef Q_OS_WINCE
#include <vibrate.h>
#include <Led_drvr.h>
#include <simmgr.h>
#include <Ifapi.h>
#include <Winbase.h>
#endif
//#include <Winsock2.h>

#define _WCHAR_T_DEFINED
#define _TIME64_T_DEFINED

//#include <api/ntddvdeo.h>
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
    QString rSubKey = "SOFTWARE\\Classes\\MIME\\Database\\Rfc1766";
    QStringList lgList;
    QSettings languageSetting("HKEY_LOCAL_MACHINE\\" + rSubKey, QSettings::NativeFormat);
    QStringList grp = languageSetting.childKeys();
    for (int i = 0; i < grp.count(); i++) {
        QString lg = languageSetting.value(grp.at(i)).toString().left(2);
        if(!lgList.contains(lg)) {
            lgList <<  lg;
         //   qWarning() << lg;
        }
    }
    return lgList;
}

// "major.minor.build" format.
/*QPair< int, double >*/ QString QSystemInfoPrivate::getVersion(QSystemInfo::Version type,  const QString &parameter)
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
           qWarning() << (quint32)versionInfo.dwMajorVersion << versionInfo.dwMinorVersion << versionInfo.dwBuildNumber << versionInfo.dwPlatformId;
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
         //   BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams;
            BLUETOOTH_FIND_RADIO_PARAMS  radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
            HANDLE radio;

            //BLUETOOTH_DEVICE_INFO btDeviceInfo;
          //  ZeroMemory(&searchParams, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));
            if(BluetoothFindFirstRadio(&radioParams, &radio) != NULL) {
//            if(BluetoothFindFirstDevice(&searchParams, &btDeviceInfo) != NULL) {
                qWarning() << "available";
                featureSupported = true;
            } else {
                qWarning() << "Not available" << GetLastError();
            }
        }
            break;
        case QSystemInfo::CameraFeature :
        {
            ICreateDevEnum *pDevEnum = NULL;
            IEnumMoniker *pEnum = NULL;

            HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                          reinterpret_cast<void**>(&pDevEnum));
            if (hr == S_OK) {
                hr = pDevEnum->CreateClassEnumerator(
                        CLSID_VideoInputDeviceCategory,
                        &pEnum, 0);
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
                if(mi.getVolumeType(drive) == QSystemMemoryInfo::Removable) {
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
            WMIHelper *wHelper;
            wHelper = new WMIHelper();
            QVariant v = wHelper->getWMIData("root/cimv2", "Win32_NetworkAdapter", "AdapterType");
            qWarning() << v.toString();
            if(v.toString() == "Wireless") {
                featureSupported = true;
            }
//            qWarning() << "wlan";
//            DWORD clientVersion;
//            HANDLE handle;
//            DWORD result = WlanOpenHandle(1, 0, &clientVersion, &handle);
//            if (result == ERROR_SUCCESS) {
//                WLAN_INTERFACE_INFO_LIST *interfaceList;
//                DWORD result = WlanEnumInterfaces(handle, 0, &interfaceList);
//                if (result == ERROR_SUCCESS) {
//                    qWarning() << interfaceList->dwNumberOfItems;
//                    if(interfaceList->dwNumberOfItems > 1) {
//                        featureSupported = true;
//                    }
//                } else {
//                    qWarning()  <<"2 not success" << result;
//                }
//            } else {
//                qWarning() << "1 not success"<< result;
//            }
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
        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    case QSystemInfo::UnknownFeature :
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

QSystemNetworkInfo::CellNetworkStatus QSystemNetworkInfoPrivate::getCellNetworkStatus()
{
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    Q_UNUSED(mode);
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

bool QSystemNetworkInfoPrivate::isWLANAccessible() const
{
    HANDLE phClientHandle = NULL;
    DWORD result;
    DWORD pdwNegotiatedVersion = 0;

    PWLAN_INTERFACE_INFO_LIST pIntfList = NULL;

    result = WlanOpenHandle(WLAN_API_VERSION, NULL, &pdwNegotiatedVersion, &phClientHandle);
    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error opening Wlanapi" << result ;
        return false;
    }

    result = WlanEnumInterfaces(phClientHandle, NULL, &pIntfList);

    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error in WlanEnumInterfaces" <<  result;
        return false;
    }
    return false;
}

QString QSystemNetworkInfoPrivate::operatorName()
{
    return "No Operator";
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
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/wmi", "WmiMonitorBrightness", "CurrentBrightness");
    qWarning() << v.toUInt();
    return v.toUInt();

    //    Q_UNUSED(screen);
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

bool QSystemDisplayInfoPrivate::isScreenLockOn()
{
    return false;
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

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::getVolumeType(const QString &driveVolume)
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

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::getCurrentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethods QSystemDeviceInfoPrivate::getInputMethodType()
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
    GetSystemPowerStatusEx(&statusEx, true);
    if(statusEx.BatteryFlag & BATTERY_FLAG_CHARGING)
        isCharging = true;
#else
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_Battery", "BatteryStatus");
    if(v.toUInt() == 6) {
        isCharging = true;
    }
#endif
    return isCharging;
}

QSystemDeviceInfo::BatteryLevel QSystemDeviceInfoPrivate::batteryLevel() const
{
#ifdef Q_OS_WINCE
    SYSTEM_POWER_STATUS_EX statusEx;
    GetSystemPowerStatusEx(&statusEx, true);
    switch(statusEx.BatteryFlag) {
    case BATTERY_FLAG_HIGH:
        return QSystemDeviceInfo::BatteryNormal;
        break;
    case BATTERY_FLAG_LOW:
        return QSystemDeviceInfo::BatteryLow;
        break;
    case BATTERY_FLAG_CRITICAL:
        return QSystemDeviceInfo::BatteryCritical;
        break;
    case BATTERY_FLAG_CHARGING:
    case BATTERY_FLAG_UNKNOWN:
    case BATTERY_FLAG_NO_BATTERY:
        return QSystemDeviceInfo::NoBatteryLevel;
        break;
   };
#else
    WMIHelper *wHelper;
    wHelper = new WMIHelper();
    QVariant v = wHelper->getWMIData("root/cimv2", "Win32_Battery", "BatteryStatus");
    switch(v.toUInt()) {
    case 3: //full
        return QSystemDeviceInfo::BatteryNormal;
        break;
    case 4: //low
        return QSystemDeviceInfo::BatteryLow;
        break;
    case 5: //critical
        return QSystemDeviceInfo::BatteryCritical;
        break;
    };
#endif
    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::getSimStatus()
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
        : QSystemScreenSaver(parent)
{

}

bool QSystemScreenSaverPrivate::setScreenSaverEnabled(QSystemScreenSaver::ScreenSaverState state)
{
    Q_UNUSED(state);

    return false;
}

bool QSystemScreenSaverPrivate::setScreenBlankingEnabled(QSystemScreenSaver::ScreenSaverState state)
{
    Q_UNUSED(state);
    return false;
}

QSystemScreenSaver::ScreenSaverState QSystemScreenSaverPrivate::screenSaverEnabled()
{
    return QSystemScreenSaver::UnknownScreenSaverState;
}

QSystemScreenSaver::ScreenSaverState QSystemScreenSaverPrivate::screenBlankingEnabled()
{
    return QSystemScreenSaver::UnknownScreenSaverState;
}


QT_END_NAMESPACE
=======
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

#include <locale.h>
//#include <windows.h>
#include <Wlanapi.h>
#include <Wtsapi32.h>
// #include <afxwin.h>

//#include <Dxva2.lib>
#include <HighLevelMonitorConfigurationAPI.h>
#include <Wbemidl.h>
#include <Bthsdpdef.h>
#include <BluetoothAPIs.h>
#include <Dshow.h>
#include <af_irda.h>
//#include <mswsock.h>

//#include <winsock2.h>
#ifdef Q_OS_WINCE
#include <vibrate.h>
#include <Led_drvr.h>
#include <simmgr.h>
#include <Ifapi.h>
#endif
//#include <Winsock2.h>

#define _WCHAR_T_DEFINED
#define _TIME64_T_DEFINED

//#include <api/ntddvdeo.h>
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
{
    return QString(setlocale(LC_ALL,NULL)).left(2);
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    QString rSubKey = "SOFTWARE\\Classes\\MIME\\Database\\Rfc1766";
    QStringList lgList;
    QSettings languageSetting("HKEY_LOCAL_MACHINE\\" + rSubKey, QSettings::NativeFormat);
    QStringList grp = languageSetting.childKeys();
    for (int i = 0; i < grp.count(); i++) {
        QString lg = languageSetting.value(grp.at(i)).toString().left(2);
        if(!lgList.contains(lg)) {
            lgList <<  lg;
         //   qWarning() << lg;
        }
    }
    return lgList;
}

// "major.minor.build" format.
/*QPair< int, double >*/ QString QSystemInfoPrivate::getVersion(QSystemInfo::Version type,  const QString &parameter)
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
           qWarning() << (quint32)versionInfo.dwMajorVersion << versionInfo.dwMinorVersion << versionInfo.dwBuildNumber << versionInfo.dwPlatformId;
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
         //   BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams;
            BLUETOOTH_FIND_RADIO_PARAMS  radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
            HANDLE radio;

            //BLUETOOTH_DEVICE_INFO btDeviceInfo;
          //  ZeroMemory(&searchParams, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));
            if(BluetoothFindFirstRadio(&radioParams, &radio) != NULL) {
//            if(BluetoothFindFirstDevice(&searchParams, &btDeviceInfo) != NULL) {
                qWarning() << "available";
                featureSupported = true;
            } else {
                qWarning() << "Not available" << GetLastError();
            }
        }
            break;
        case QSystemInfo::CameraFeature :
        {
            ICreateDevEnum *pDevEnum = NULL;
            IEnumMoniker *pEnum = NULL;

            HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                          reinterpret_cast<void**>(&pDevEnum));
            if (hr == S_OK) {
                hr = pDevEnum->CreateClassEnumerator(
                        CLSID_VideoInputDeviceCategory,
                        &pEnum, 0);
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
            WORD      WSAVerReq = MAKEWORD(1,1);
            WSADATA      WSAData;

            if (WSAStartup(WSAVerReq, &WSAData) == 0) {
                // wrong winsock dlls?

                SOCKET ServSock;
                if ((ServSock = socket(AF_IRDA, SOCK_STREAM, 0)) != INVALID_SOCKET) {
                    featureSupported = true;
                }  else {
//                    qWarning() << WSAGetLastError();
                }
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
                if(mi.getVolumeType(drive) == QSystemMemoryInfo::Removable) {
                    featureSupported = true;
                }
            }
        }
        break;
    case QSystemInfo::UsbFeature :
        {
            //IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME
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
            qWarning() << "wlan";
            DWORD clientVersion;
            HANDLE handle;
            DWORD result = WlanOpenHandle(1, 0, &clientVersion, &handle);
            if (result == ERROR_SUCCESS) {
                WLAN_INTERFACE_INFO_LIST *interfaceList;
                DWORD result = WlanEnumInterfaces(handle, 0, &interfaceList);
                if (result == ERROR_SUCCESS) {
                    qWarning() << interfaceList->dwNumberOfItems;
                    if(interfaceList->dwNumberOfItems > 1) {
                        featureSupported = true;
                    }
                } else {
                    qWarning()  <<"2 not success" << result;
                }
            } else {
                qWarning() << "1 not success"<< result;
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
        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    case QSystemInfo::UnknownFeature :
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

QSystemNetworkInfo::CellNetworkStatus QSystemNetworkInfoPrivate::getCellNetworkStatus()
{
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    Q_UNUSED(mode);
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

bool QSystemNetworkInfoPrivate::isWLANAccessible() const
{
    HANDLE phClientHandle = NULL;
    DWORD result;
    DWORD pdwNegotiatedVersion = 0;

    PWLAN_INTERFACE_INFO_LIST pIntfList = NULL;

    result = WlanOpenHandle(WLAN_API_VERSION, NULL, &pdwNegotiatedVersion, &phClientHandle);
    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error opening Wlanapi" << result ;
        return false;
    }

    result = WlanEnumInterfaces(phClientHandle, NULL, &pIntfList);

    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error in WlanEnumInterfaces" <<  result;
        return false;
    }
    return false;
}

QString QSystemNetworkInfoPrivate::operatorName()
{
    return "No Operator";
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
//    Q_UNUSED(screen);
    qint32 brightness = 0;
    HANDLE display = CreateFile(L"\\\\.\\LCD",FILE_ANY_ACCESS,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(display != INVALID_HANDLE_VALUE) {
        DISPLAY_BRIGHTNESS brightnessBuffer;
        memset( &brightnessBuffer, 0, sizeof(brightnessBuffer));
        DWORD bytesReturned = 0;
        if(DeviceIoControl(display,IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,
                           NULL,0,&brightnessBuffer,256,&bytesReturned,NULL)) {
            if(bytesReturned > 0) {
                brightness = brightnessBuffer.ucACBrightness;
                qWarning()
                        << brightnessBuffer.ucDisplayPolicy
                        << brightnessBuffer.ucDCBrightness
                        << brightnessBuffer.ucACBrightness
                        << static_cast<int>(brightnessBuffer.ucACBrightness);
            } else {
                qWarning() << "bytes not returned" << bytesReturned << GetLastError();
            }
        }

        CloseHandle(display);
    } else {
        qWarning() << "invalid handle";
    }


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

#if 0 //vista only
HRESULT hres;
hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (hres == S_FALSE) {
        qWarning() << "Failed to initialize COM library. Error code = 0x" << hex << hres;
        return -1;
    }

////////////////
IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &pLoc);

    if (FAILED(hres)) {
        qWarning() << "Failed to create IWbemLocator object." << " Err code = 0x"<< hex << hres;
        CoUninitialize();
        return -1;
    }

///////////
IWbemServices *pSvc = NULL;

    // Connect to the local root\cimv2 namespace
    // and obtain pointer pSvc to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        L"root\\wmi",NULL,NULL,0,NULL,0,0,&pSvc);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "Could not connect. Error code = 0x" << hex << hres;
        pLoc->Release();
        CoUninitialize();
        return -1;
    }

/////////////////////
 hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities
    );

    if (FAILED(hres)) {
        qWarning() << "Could not set proxy blanket. Error code = 0x"
            << hex << hres;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return -1;
    }
////////////////////////
    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR bstrWQL = SysAllocString(L"WQL");
    BSTR bstrQuery = SysAllocString(L"SELECT * FROM WmiMonitorBrightness");
//    BSTR bstrQuery = SysAllocString(L"SELECT * FROM Win32_OperatingSystem");

    hres = pSvc->ExecQuery(
            bstrWQL,
            bstrQuery,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

    if (hres != WBEM_S_NO_ERROR){
        qWarning() << "Query for monitor brightness failed."
            << " Error code = 0x"
            << hex << hres;
        pSvc->Release();
        pLoc->Release();
        pEnumerator->Release();
        CoUninitialize();
        return -1;
    }
    ///////////////////////
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                                       &pclsObj, &uReturn);
        if(hr != WBEM_S_NO_ERROR) {

            qWarning() << "enumerating monitor brightness failed."
                    << " Error code = 0x"
                    << hex << hres << hr << GetLastError();

        }

        if(0 == uReturn){
            break;
        }

        VARIANT vtProp;
        // Get the value of the Name property

        hr = pclsObj->Get(L"CurrentBrightness", 0, &vtProp, 0, 0);
        qWarning() << " brightness : " << vtProp.bVal;
        //        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        //        qWarning() << " brightness : " << vtProp.bstrVal;
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
#endif
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

    Q_UNUSED(screen);
    return bpp;
}

bool QSystemDisplayInfoPrivate::isScreenLockOn()
{
    return false;
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

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::getVolumeType(const QString &driveVolume)
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

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::getCurrentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethods QSystemDeviceInfoPrivate::getInputMethodType()
{
    QSystemDeviceInfo::InputMethod methods;
    return methods;
}


QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
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
    return QString();
}

QString QSystemDeviceInfoPrivate::model()
{
    return QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    return QString();
}

bool QSystemDeviceInfoPrivate::isBatteryCharging()
{
    bool isCharging = false;
    return isCharging;
}

QSystemDeviceInfo::BatteryLevel QSystemDeviceInfoPrivate::batteryLevel() const
{

    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::getSimStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QSystemScreenSaver(parent)
{

}

bool QSystemScreenSaverPrivate::setScreenSaverEnabled(QSystemScreenSaver::ScreenSaverState state)
{
    Q_UNUSED(state);

    return false;
}

bool QSystemScreenSaverPrivate::setScreenBlankingEnabled(QSystemScreenSaver::ScreenSaverState state)
{
    Q_UNUSED(state);
    return false;
}

QSystemScreenSaver::ScreenSaverState QSystemScreenSaverPrivate::screenSaverEnabled()
{
    return QSystemScreenSaver::UnknownScreenSaverState;
}

QSystemScreenSaver::ScreenSaverState QSystemScreenSaverPrivate::screenBlankingEnabled()
{
    return QSystemScreenSaver::UnknownScreenSaverState;
}


QT_END_NAMESPACE
>>>>>>> 079c80a28db950d7f41482323f8d4d74b13a66ae:systeminfo/qsysteminfo_win.cpp
