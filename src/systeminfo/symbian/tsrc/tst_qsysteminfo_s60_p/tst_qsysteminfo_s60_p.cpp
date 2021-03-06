/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qsysteminfo_s60_p.h"

class tst_DeviceInfo : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void tst_phoneInfo();
    void tst_subscriberInfo();
    void tst_chargingStatus();
    void tst_batteryInfo();
    void tst_cellNetworkInfo();
    void tst_cellNetworkRegistrationInfo();
    void tst_cellSignalStrenghtInfo();
    void tst_wlanInfo();
    void tst_mmcStorageStatus();
#ifdef DISKNOTIFY_SUPPORTED
    void tst_storagedisknotifier();
#endif
#ifdef LOCKANDFLIP_SUPPORTED
    void tst_keylockStatus();
    void tst_flipStatus();
#endif
#ifdef THERMALSTATUS_SUPPORTED
    void tst_thermalStatus();
#endif
    void tst_batteryCommonInfo();
    void tst_networkInfo();

private:
    QtMobility::DeviceInfo* deviceInfo;
};

void tst_DeviceInfo::init() {
    deviceInfo = new QtMobility::DeviceInfo();
}

void tst_DeviceInfo::cleanup()
{
    delete deviceInfo;
    deviceInfo = NULL;
}

void tst_DeviceInfo::tst_phoneInfo() {
    deviceInfo->phoneInfo();
}

void tst_DeviceInfo::tst_subscriberInfo() {
    deviceInfo->subscriberInfo();
}

void tst_DeviceInfo::tst_chargingStatus() {
    deviceInfo->chargingStatus();
}

void tst_DeviceInfo::tst_batteryInfo() {
    deviceInfo->batteryInfo();
}

void tst_DeviceInfo::tst_cellNetworkInfo() {
    deviceInfo->cellNetworkInfo();
}

void tst_DeviceInfo::tst_cellNetworkRegistrationInfo() {
    deviceInfo->cellNetworkRegistrationInfo();
}

void tst_DeviceInfo::tst_cellSignalStrenghtInfo() {
    deviceInfo->cellSignalStrenghtInfo();
}

void tst_DeviceInfo::tst_wlanInfo() {
    deviceInfo->wlanInfo();
}

void tst_DeviceInfo::tst_mmcStorageStatus() {
    deviceInfo->mmcStorageStatus();
}

#ifdef DISKNOTIFY_SUPPORTED
void tst_DeviceInfo::tst_storagedisknotifier() {
    deviceInfo->storagedisknotifier();
}
#endif

#ifdef LOCKANDFLIP_SUPPORTED
void tst_DeviceInfo::tst_keylockStatus() {
    deviceInfo->keylockStatus();
}

void tst_DeviceInfo::tst_flipStatus() {
    deviceInfo->flipStatus();
}
#endif

#ifdef THERMALSTATUS_SUPPORTED
void tst_DeviceInfo::tst_thermalStatus() {
    deviceInfo->thermalStatus();
}
#endif

void tst_DeviceInfo::tst_batteryCommonInfo() {
    deviceInfo->batteryCommonInfo();
}

void tst_DeviceInfo::tst_networkInfo() {
    deviceInfo->networkInfo();
}

QTEST_MAIN(tst_DeviceInfo)
#include "tst_qsysteminfo_s60_p.moc"
