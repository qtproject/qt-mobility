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

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>

#include "qsysteminfo.h"

using namespace QtMobility;

#define X(expr) qDebug() << #expr << "->" << (expr);

struct symbol_t
{
  const char *key;
  int val;
};

int lookup(const symbol_t *stab, const char *key, int def)
{
  for(;stab->key;++stab) {
    if(!strcmp(stab->key,key)) return stab->val;
  }
  return def;
}

const char *rlookup(const symbol_t *stab, int val, const char *def)
{
  for(;stab->key; ++stab) {
    if(stab->val == val) return stab->key;
  }
  return def;
}

#define SYM(x) { #x, x }

static const symbol_t Version_lut[] =
{
  SYM(QSystemInfo::Os),
  SYM(QSystemInfo::QtCore),
  SYM(QSystemInfo::Firmware),
  SYM(QSystemInfo::QtMobility),
  {0,0}
};

static const symbol_t Feature_lut[] =
{
  SYM(QSystemInfo::BluetoothFeature),
  SYM(QSystemInfo::CameraFeature),
  SYM(QSystemInfo::FmradioFeature),
  SYM(QSystemInfo::IrFeature),
  SYM(QSystemInfo::LedFeature),
  SYM(QSystemInfo::MemcardFeature),
  SYM(QSystemInfo::UsbFeature),
  SYM(QSystemInfo::VibFeature),
  SYM(QSystemInfo::WlanFeature),
  SYM(QSystemInfo::SimFeature),
  SYM(QSystemInfo::LocationFeature),
  SYM(QSystemInfo::VideoOutFeature),
  SYM(QSystemInfo::HapticsFeature),
 SYM(QSystemInfo::FmTransmitterFeature),
  {0,0}
};

static const symbol_t NetworkStatus_lut[] =
{
  SYM(QSystemNetworkInfo::UndefinedStatus),
  SYM(QSystemNetworkInfo::NoNetworkAvailable),
  SYM(QSystemNetworkInfo::EmergencyOnly),
  SYM(QSystemNetworkInfo::Searching),
  SYM(QSystemNetworkInfo::Busy),
  SYM(QSystemNetworkInfo::Connected),
  SYM(QSystemNetworkInfo::HomeNetwork),
  SYM(QSystemNetworkInfo::Denied),
  SYM(QSystemNetworkInfo::Roaming),
  {0,0}
};

static const symbol_t NetworkMode_lut[] =
{
  SYM(QSystemNetworkInfo::UnknownMode),
  SYM(QSystemNetworkInfo::GsmMode),
  SYM(QSystemNetworkInfo::CdmaMode),
  SYM(QSystemNetworkInfo::WcdmaMode),
  SYM(QSystemNetworkInfo::WlanMode),
  SYM(QSystemNetworkInfo::EthernetMode),
  SYM(QSystemNetworkInfo::BluetoothMode),
  SYM(QSystemNetworkInfo::WimaxMode),
  SYM(QSystemNetworkInfo::GprsMode),
  SYM(QSystemNetworkInfo::EdgeMode),
  SYM(QSystemNetworkInfo::HspaMode),
  SYM(QSystemNetworkInfo::LteMode),
  {0,0}
};

/* ------------------------------------------------------------------------- *
 * test_systeminfo
 * ------------------------------------------------------------------------- */

static void test_systeminfo(void)
{
  QSystemInfo info;

  X(info.currentLanguage());
  X(info.availableLanguages());
  X(info.currentCountryCode());

  X(info.version(QSystemInfo::Os));
  X(info.version(QSystemInfo::QtCore));
  X(info.version(QSystemInfo::Firmware));
  X(info.version(QSystemInfo::QtMobility));

  X(info.hasFeatureSupported(QSystemInfo::BluetoothFeature));
  X(info.hasFeatureSupported(QSystemInfo::CameraFeature));
  X(info.hasFeatureSupported(QSystemInfo::FmradioFeature));
  X(info.hasFeatureSupported(QSystemInfo::IrFeature));
  X(info.hasFeatureSupported(QSystemInfo::LedFeature));
  X(info.hasFeatureSupported(QSystemInfo::MemcardFeature));
  X(info.hasFeatureSupported(QSystemInfo::UsbFeature));
  X(info.hasFeatureSupported(QSystemInfo::VibFeature));
  X(info.hasFeatureSupported(QSystemInfo::WlanFeature));
  X(info.hasFeatureSupported(QSystemInfo::SimFeature));
  X(info.hasFeatureSupported(QSystemInfo::LocationFeature));
  X(info.hasFeatureSupported(QSystemInfo::VideoOutFeature));
  X(info.hasFeatureSupported(QSystemInfo::HapticsFeature));
  X(info.hasFeatureSupported(QSystemInfo::FmTransmitterFeature));
}

/* ------------------------------------------------------------------------- *
 * test_systemdeviceinfo
 * ------------------------------------------------------------------------- */

static void test_systemdeviceinfo(void)
{
  QSystemDeviceInfo deviceinfo;

  X(deviceinfo.batteryLevel());
  X(deviceinfo.batteryStatus());
  X(deviceinfo.currentBluetoothPowerState());
  X(deviceinfo.currentPowerState());
  X(deviceinfo.currentProfile());
  X(deviceinfo.imei());
  X(deviceinfo.imsi());
  X(deviceinfo.inputMethodType());
  X(deviceinfo.isDeviceLocked());
  X(deviceinfo.isKeyboardFlippedOpen());
  X(deviceinfo.isWirelessKeyboardConnected());
  X(deviceinfo.keyboardTypes());
  X(deviceinfo.manufacturer());
  X(deviceinfo.model());
  X(deviceinfo.productName());
  X(deviceinfo.simStatus());
  X(deviceinfo.lockStatus());
  X(deviceinfo.uniqueDeviceID());
}

/* ------------------------------------------------------------------------- *
 * test_systemdisplayinfo
 * ------------------------------------------------------------------------- */

static void test_systemdisplayinfo(void)
{
  QSystemDisplayInfo displayinfo;
  QDesktopWidget wid;
  for( int display = 0; display < wid.screenCount(); ++display )
  {
    qDebug() << "";
    qDebug() << "Display:" << display;
    int depth = displayinfo.colorDepth(display);
    qDebug() << "  displayinfo.colorDepth() ->" << depth;
    int value = displayinfo.displayBrightness(display);
    qDebug() << "  displayinfo.displayBrightness() ->" << value;
    QSystemDisplayInfo::DisplayOrientation orientation = displayinfo.getOrientation(display);
    qDebug() << "  displayinfo.getOrientation() ->" << orientation;
    float contrast = displayinfo.contrast(display);
    qDebug() << "  displayinfo.getContrast() ->" << contrast;
    int dpiWidth = displayinfo.getDPIWidth(display);
    qDebug() << "  displayinfo.getDPIWidth() ->" << dpiWidth;
    int dpiHeight = displayinfo.getDPIHeight(display);
    qDebug() << "  displayinfo.getDPIHeight() ->" << dpiHeight;
    int physicalHeight = displayinfo.physicalHeight(display);
    qDebug() << "  displayinfo.physicalHeight() ->" << physicalHeight;
    int physicalWidth = displayinfo.physicalWidth(display);
    qDebug() << "  displayinfo.physicalWidth() ->" << physicalWidth;
    QSystemDisplayInfo::BacklightState state = displayinfo.backlightStatus(display);
    qDebug() << "  displayinfo.backlightStatus() ->" << state;

  }
}

/* ------------------------------------------------------------------------- *
 * test_systemstorageinfo
 * ------------------------------------------------------------------------- */

static const char *human_size(qlonglong n)
{
  if(n == 0) return "0B";

  static char buf[256];
  char *pos = buf;
  char *end = buf + sizeof buf;

  int b = n & 1023; n >>= 10;
  int k = n & 1023; n >>= 10;
  int m = n & 1023; n >>= 10;
  int g = n & 1023; n >>= 10;

  *pos = 0;
#if defined(Q_WS_WIN)
  if(g) _snprintf_s(pos, sizeof(pos), end-pos,"%s%dGB", *buf?" ":"", g), pos = strchr(pos,0);
  if(m) _snprintf_s(pos, sizeof(pos), end-pos,"%s%dMB", *buf?" ":"", m), pos = strchr(pos,0);
  if(k) _snprintf_s(pos, sizeof(pos), end-pos,"%s%dkB", *buf?" ":"", k), pos = strchr(pos,0);
  if(b) _snprintf_s(pos, sizeof(pos), end-pos,"%s%dB",  *buf?" ":"", b), pos = strchr(pos,0);
#else
  if(g) snprintf(pos, end-pos, "%s%dGB", *buf?" ":"", g), pos = strchr(pos,0);
  if(m) snprintf(pos, end-pos, "%s%dMB", *buf?" ":"", m), pos = strchr(pos,0);
  if(k) snprintf(pos, end-pos, "%s%dkB", *buf?" ":"", k), pos = strchr(pos,0);
  if(b) snprintf(pos, end-pos, "%s%dB",  *buf?" ":"", b), pos = strchr(pos,0);
#endif
  return buf;
}

static void test_systemstorageinfo(void)
{
  QSystemStorageInfo storageinfo;

  QStringList lst = storageinfo.logicalDrives();

  qDebug() << "storageinfo.logicalDrives ->" << lst;

  for(int i = 0; i < lst.size(); ++i) {
    const QString &drv = lst.at(i);

    qDebug() << "Logical drive:" << drv;

    qlonglong avail = storageinfo.availableDiskSpace(drv);
    qDebug() << "  storageinfo.availableDiskSpace() ->" << human_size(avail);

    qlonglong total = storageinfo.totalDiskSpace(drv);
    qDebug() << "  storageinfo.totalDiskSpace() ->" << human_size(total);

    QSystemStorageInfo::DriveType dtype = storageinfo.typeForDrive(drv);
    qDebug() << "  storageinfo.typeForDrive() ->" << dtype;

    QString duri = storageinfo.uriForDrive(drv);
    qDebug() << "  storageinfo.uriForDrive() ->" << duri;

    QSystemStorageInfo::StorageState dstate = storageinfo.getStorageState(drv);
    qDebug() << "  storageinfo.getStorageState() ->" << dstate;
  }
}

/* ------------------------------------------------------------------------- *
 * test_systemnetworkinfo
 * ------------------------------------------------------------------------- */

static void test_systemnetworkinfo(void)
{
  QSystemNetworkInfo networkinfo;

  X(networkinfo.cellId());
  X(networkinfo.currentMobileCountryCode());
  X(networkinfo.currentMobileNetworkCode());
  X(networkinfo.homeMobileCountryCode());
  X(networkinfo.homeMobileNetworkCode());
  X(networkinfo.locationAreaCode());

  for(const symbol_t *sym = NetworkMode_lut; sym->key; ++sym) {
    QtMobility::QSystemNetworkInfo::NetworkMode mode =
    (QtMobility::QSystemNetworkInfo::NetworkMode) sym->val;

    qDebug() << "";
    qDebug() << "NetworkMode:" << sym->key;

    QNetworkInterface iface = networkinfo.interfaceForMode(mode);
    qDebug() << "  networkinfo.interfaceForMode() ->" << iface;

    QString macaddr = networkinfo.macAddress(mode);
    qDebug() << "  networkinfo.macAddress() ->" << macaddr;

    QSystemNetworkInfo::NetworkStatus status = networkinfo.networkStatus(mode);
    qDebug() << "  networkinfo.networkStatus() ->" << status;

    QString network = networkinfo.networkName(mode);
    qDebug() << "  networkinfo.netwoerkName() ->" << network;

    int sigstr = networkinfo.networkSignalStrength(mode);
    qDebug() << "  networkinfo.networkSignalStrength() ->" << sigstr;
  }
}

static void test_systemscreensaver(void)
{
  QSystemScreenSaver screensaver;

  X(screensaver.screenSaverInhibited());
  X(screensaver.setScreenSaverInhibit());
}

static void test_systembatteryinfo(void)
{
    QSystemBatteryInfo batInfo;
    X(batInfo.chargerType());
    X(batInfo.chargingState() );
    X(batInfo.nominalCapacity());
    X(batInfo.remainingCapacityPercent());
    X(batInfo.remainingCapacity());
    X(batInfo.voltage());
    X(batInfo.remainingChargingTime());
    X(batInfo.currentFlow());
    X(batInfo.remainingCapacityBars());
    X(batInfo.maxBars());
    X(batInfo.batteryStatus());
    X(batInfo.energyMeasurementUnit());
}

struct dummy_t
{
  const char *name;
  void (*func)(void);
} lut[] = {
#define ADD(x) {#x, test_##x }
  ADD(systeminfo),
  ADD(systemdeviceinfo),
  ADD(systemstorageinfo),
  ADD(systemnetworkinfo),
  ADD(systemscreensaver),
  ADD(systemdisplayinfo),
  ADD(systembatteryinfo),
#undef ADD
  {0,0}
};

static bool endswith(const char *str, const char *pat)
{
  int slen = strlen(str);
  int plen = strlen(pat);
  return (slen >= plen) && !strcmp(str+slen-plen, pat);
}

int lookup_test(const char *name)
{
  for(int i = 0; lut[i].name; ++i) {
    if(!strcmp(lut[i].name, name)) return i;
  }
  for(int i = 0; lut[i].name; ++i) {
    if(endswith(lut[i].name, name)) return i;
  }
  for(int i = 0; lut[i].name; ++i) {
    if(strstr(lut[i].name, name)) return i;
  }
  return -1;
}

int main(int ac, char **av)
{
#if !defined(Q_WS_WIN)
    if(!getenv("DISPLAY")) {
    qDebug() << "$DISPLAY not set, assuming :0";
    setenv("DISPLAY", ":0", 1);
  }
  if(!getenv("DBUS_SESSION_BUS_ADDRESS")) {
    qDebug() << "session bus not configured";
  }
#endif
  QApplication app(ac, av, true);

  if(ac < 2) {
    qDebug() << "available tests:";
    for(int k = 0; lut[k].name; ++k) {
      qDebug() << *av << lut[k].name;
    }
    exit(0);
  }

  for(int i = 1; i < ac; ++i) {
    const char *name = av[i];

    int k = lookup_test(name);

    if(k != -1) {
      qDebug() << "";
      qDebug() << "----(" << lut[k].name << ")----";
      qDebug() << "";
      lut[k].func();
    } else if( !strcmp(name, "all")) {
      for(int k = 0; lut[k].name; ++k) {
        qDebug() << "";
        qDebug() << "----(" << lut[k].name << ")----";
        qDebug() << "";
        lut[k].func();
      }
    } else {
      break;
    }
  }
}

// EOF
