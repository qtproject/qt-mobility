/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"
#include <QMessageBox>
#include <QTimer>

Dialog::Dialog() :
    QWidget(),
    saver(NULL), systemInfo(NULL), di(NULL), ni(NULL)
{
    setupUi(this);
    setupGeneral();

    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
    connect(versionComboBox,SIGNAL(activated(int)), this,SLOT(getVersion(int)));
    connect(featureComboBox,SIGNAL(activated(int)), this,SLOT(getFeature(int)));
    updateDeviceLockedState();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateDeviceLockedState()));
    timer->start(1000);
}

Dialog::~Dialog()
{
    delete systemInfo;
    delete di;
    delete saver;
}

void Dialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::tabChanged(int index)
{
#ifdef QTM_EXAMPLES_SMALL_SCREEN
    switch(index) {
    case 0:
        setupGeneral();
        break;
    case 1:
        setupGeneral();
        break;
    case 2:
        setupDevice();
        break;
    case 3:
        setupDevice();
        break;
    case 4:
        setupDevice();
        break;
    case 5:
        setupDisplay();
        break;
    case 6:
        setupStorage();
        break;
    case 7:
        setupNetwork();
        break;
    case 8:
        setupNetwork();
        break;
    case 9:
        setupNetwork();
        break;
    case 10:
        setupSaver();
        break;
    };
#else
    switch(index) {
    case 0:
        setupGeneral();
        break;
    case 1:
        setupDevice();
        break;
    case 2:
        setupDisplay();
        break;
    case 3:
        setupStorage();
        break;
    case 4:
        setupNetwork();
        break;
    case 5:
        setupSaver();
        break;
    };
#endif
}

void Dialog::setupGeneral()
{
    delete systemInfo;
//! [lang]
    systemInfo = new QSystemInfo(this);
    curLanguageLineEdit->setText( systemInfo->currentLanguage());
//! [lang]
    languagesComboBox->clear();
    languagesComboBox->insertItems(0,systemInfo->availableLanguages());
    countryCodeLabel->setText(systemInfo->currentCountryCode());
}

void Dialog::setupDevice()
{
    delete di;
//! [createdi]
    di = new QSystemDeviceInfo(this);
//! [createdi]
//! [batteryLevel]
    batteryLevelBar->setValue(di->batteryLevel());
//! [batteryLevel]

//! [sig batteryLevelChanged]
    connect(di,SIGNAL(batteryLevelChanged(int)),
            this,SLOT(updateBatteryStatus(int)));
//! [sig batteryLevelChanged]

    connect(di,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this,SLOT(displayBatteryStatus(QSystemDeviceInfo::BatteryStatus)));

    connect(di,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this,SLOT(updatePowerState(QSystemDeviceInfo::PowerState)));

    ImeiLabel->setText(di->imei());
    imsiLabel->setText(di->imsi());
//! [manuf-id]
    manufacturerLabel->setText(di->manufacturer());
//! [manuf-id]
    modelLabel->setText(di->model());
//! [productName]
    productLabel->setText(di->productName());
//! [productName]

    deviceLockPushButton->setChecked(di->isDeviceLocked());

    updateSimStatus();
    updateProfile();

    connect(di, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
        this, SLOT(updateProfile(QSystemDeviceInfo::Profile)));

    if(di->currentPowerState() == QSystemDeviceInfo::BatteryPower) {
        radioButton_2->setChecked(true);
    } else  if(di->currentPowerState() == QSystemDeviceInfo::WallPower) {
        radioButton_3->setChecked(true);
    } else if(di->currentPowerState() == QSystemDeviceInfo::WallPowerChargingBattery) {
        radioButton_4->setChecked(true);
    } else {
        radioButton->setChecked(true);
    }

//! [inputMethod flags]
    QSystemDeviceInfo::InputMethodFlags methods = di->inputMethodType();
    QStringList inputs;
    if((methods & QSystemDeviceInfo::Keys)){
        inputs << "Keys";
    }
    if((methods & QSystemDeviceInfo::Keypad)) {
        inputs << "Keypad";
    }
//! [inputMethod flags]
    if((methods & QSystemDeviceInfo::Keyboard)) {
        inputs << "Keyboard";
    }
    if((methods & QSystemDeviceInfo::SingleTouch)) {
        inputs << "Touch Screen";
    }
    if((methods & QSystemDeviceInfo::MultiTouch)) {
        inputs << "Multi touch";
    }
    if((methods & QSystemDeviceInfo::Mouse)){
        inputs << "Mouse";
    }

    inputMethodLabel->setText(inputs.join(" "));
}

void Dialog::updateDeviceLockedState()
{
    if (di)
        deviceLockPushButton->setChecked(di->isDeviceLocked());
}

void Dialog::updateProfile(QSystemDeviceInfo::Profile /*profile*/)
{
   updateProfile();
}


void Dialog::setupDisplay()
{
    QSystemDisplayInfo di;
    brightnessLineEdit->setText(QString::number(di.displayBrightness(0)));
    colorDepthLineEdit->setText(QString::number(di.colorDepth((0))));

}

void Dialog::setupStorage()
{
    QSystemStorageInfo mi;
    storageTreeWidget->clear();
    storageTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

    QStringList vols = mi.logicalDrives();
    foreach(QString volName, vols) {
        QString type;
        QSystemStorageInfo::DriveType volType;
        volType = mi.typeForDrive(volName);
        if(volType == QSystemStorageInfo::InternalDrive) {
            type =  "Internal";
        }

        if(volType == QSystemStorageInfo::RemovableDrive) {
            type = "Removable";
        }
        if(volType == QSystemStorageInfo::CdromDrive) {
            type =  "Cdrom";
        }
        if(volType == QSystemStorageInfo::RemoteDrive) {
            type =  "Network";
        }
        QStringList items;
        items << volName;
        items << type;
        items << QString::number(mi.totalDiskSpace(volName));
        items << QString::number(mi.availableDiskSpace(volName));
        QTreeWidgetItem *item = new QTreeWidgetItem(items);
        storageTreeWidget->addTopLevelItem(item);
    }
}

void Dialog::setupNetwork()
{
    delete ni;
//! [networkInfo]
    ni = new QSystemNetworkInfo(this);
//! [networkInfo]

    connect(netStatusComboBox,SIGNAL(activated(int)),
            this, SLOT(netStatusComboActivated(int)));

//! [sig strength]
    connect(ni,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)),
            this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));
//! [sig strength]

    connect(ni,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
            this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));

    connect(ni,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
            this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));

    connect(ni,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
            this,SLOT(networkModeChanged(QSystemNetworkInfo::NetworkMode)));

    cellIdLabel->setText(QString::number(ni->cellId()));
    locationAreaCodeLabel->setText(QString::number(ni->locationAreaCode()));
    currentMMCLabel->setText(ni->currentMobileCountryCode());
    currentMNCLabel->setText(ni->currentMobileNetworkCode());

    homeMMCLabel->setText(ni->homeMobileCountryCode());
    homeMNCLabel->setText(ni->homeMobileNetworkCode());

    networkModeChanged(ni->currentMode());

}
void Dialog::netStatusComboActivated(int index)
{
    QString status;
    int reIndex = index;

    displayNetworkStatus(ni->networkStatus((QSystemNetworkInfo::NetworkMode)reIndex));

    macAddressLabel->setText(ni->macAddress((QSystemNetworkInfo::NetworkMode)reIndex));

    int strength = ni->networkSignalStrength((QSystemNetworkInfo::NetworkMode)reIndex);
    if(strength < 0)
        strength = 0;
    signalLevelProgressBar->setValue(strength);

    InterfaceLabel->setText(ni->interfaceForMode((QSystemNetworkInfo::NetworkMode)reIndex).humanReadableName());

    operatorNameLabel->setText(ni->networkName((QSystemNetworkInfo::NetworkMode)reIndex));
}

void Dialog::getVersion(int index)
{
    QSystemInfo::Version version;
    switch(index) {
    case 0:
        versionLineEdit->setText("");
        break;
    case 1:
//! [OS ver]
        version = QSystemInfo::Os;
//! [OS ver]
        break;
    case 2:
//! [Qt ver]
        version = QSystemInfo::QtCore;
//! [Qt ver]
        break;
    case 3:
//! [Firm ver]
        version = QSystemInfo::Firmware;
//! [Firm ver]
        break;
    };

    QSystemInfo si;
    versionLineEdit->setText(si.version(version));
}

void Dialog::getFeature(int index)
{
//! [feature]
    QSystemInfo::Feature feature;
    switch(index) {
//! [feature]
    case 0:
        return;
        break;
//! [feature-bluetooth]
    case 1:
        feature = QSystemInfo::BluetoothFeature;
//! [feature-bluetooth]
        break;
    case 2:
        feature = QSystemInfo::CameraFeature;
        break;
    case 3:
        feature = QSystemInfo::FmradioFeature;
        break;
    case 4:
        feature = QSystemInfo::IrFeature;
        break;
    case 5:
        feature = QSystemInfo::LedFeature;
        break;
    case 6:
        feature = QSystemInfo::MemcardFeature;
        break;
    case 7:
        feature = QSystemInfo::UsbFeature;
        break;
    case 8:
        feature = QSystemInfo::VibFeature;
        break;
    case 9:
        feature = QSystemInfo::WlanFeature;
        break;
    case 10:
        feature = QSystemInfo::SimFeature;
        break;
    case 11:
        feature = QSystemInfo::LocationFeature;
        break;
    case 12:
        feature = QSystemInfo::VideoOutFeature;
        break;
    case 13:
        feature = QSystemInfo::HapticsFeature;
        break;
    };
//! [feature test]
    QSystemInfo si;
    featuresLineEdit->setText((si.hasFeatureSupported(feature) ? "true":"false" ));
//! [feature test]
}

void Dialog::setupSaver()
{
    if (!saver) {
        saver = new QSystemScreenSaver(this);
    }

    bool saverEnabled = saver->screenSaverInhibited();


    connect( saverInhibitedCheckBox, SIGNAL(clicked(bool)),
             this,SLOT(setSaverEnabled(bool)));

    saverInhibitedCheckBox->setChecked(saverEnabled);
}


void Dialog::setSaverEnabled(bool b)
{
    if (b) {
        if (!saver) {
            saver = new QSystemScreenSaver(this);
        }
       if(saver->setScreenSaverInhibit()) {
        }
    } else {
        delete saver;
        saver = NULL;
    }
}


void Dialog::updateBatteryStatus(int level)
{
    batteryLevelBar->setValue(level);
}

void Dialog::updatePowerState(QSystemDeviceInfo::PowerState newState)
{

    switch (newState) {
    case QSystemDeviceInfo::BatteryPower:
        {
            radioButton_2->setChecked(true);
        }
        break;
    case QSystemDeviceInfo::WallPower:
        {
            radioButton_3->setChecked(true);
        }
        break;
    case QSystemDeviceInfo::WallPowerChargingBattery:
        {
            radioButton_4->setChecked(true);
        }
        break;
    case QSystemDeviceInfo::NoBatteryLevel:
        {
            radioButton->setChecked(true);
        }
        break;
    };
}

void Dialog::displayBatteryStatus(QSystemDeviceInfo::BatteryStatus status)
{
    // this wont annoy users will it?
    QString msg;
//    if(di->isBatteryCharging()) {
        switch(status) {
        case QSystemDeviceInfo::BatteryCritical:
            {
                msg = " Battery is Critical (4% or less), please save your work or plug in the charger.";
                QMessageBox::critical(this,"QSystemInfo",msg);
            }
            break;
        case QSystemDeviceInfo::BatteryVeryLow:
            {
                msg = "Battery is Very Low (10%), please plug in the charger soon";
                QMessageBox::warning(this,"QSystemInfo",msg);
            }
            break;
        case QSystemDeviceInfo::BatteryLow:
            {
                msg = "Battery is Low (40% or less)";
                QMessageBox::information(this,"QSystemInfo",msg);

            }
            break;
        case QSystemDeviceInfo::BatteryNormal:
            {
                msg = "Battery is Normal (greater than 40%)";
                QMessageBox::information(this,"QSystemInfo",msg);
            }
            break;
        case QSystemDeviceInfo::NoBatteryLevel:
            {

            }
            break;
        };
  //  }

}

void Dialog::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode , int strength)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        if(netStatusComboBox->currentText() == "Wlan") {
            signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        if(netStatusComboBox->currentText() == "Ethernet") {
            signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        if(netStatusComboBox->currentText() == "Gsm") {
            signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        if(netStatusComboBox->currentText() == "Cdma") {
            signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        if(netStatusComboBox->currentText() == "Wcdma") {
            signalLevelProgressBar->setValue(strength);
        }
    }

}

void Dialog::networkNameChanged(QSystemNetworkInfo::NetworkMode mode,const QString &text)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        if(netStatusComboBox->currentText() == "Wlan") {
            operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        if(netStatusComboBox->currentText() == "Ethernet") {
            operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        if(netStatusComboBox->currentText() == "Gsm") {
            operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        if(netStatusComboBox->currentText() == "Cdma") {
            operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        if(netStatusComboBox->currentText() == "Wcdma") {
            operatorNameLabel->setText(text);
        }
    }

}

void Dialog::networkStatusChanged(QSystemNetworkInfo::NetworkMode mode , QSystemNetworkInfo::NetworkStatus status)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        if(netStatusComboBox->currentText() == "Wlan") {
           displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        if(netStatusComboBox->currentText() == "Ethernet") {
           displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        if(netStatusComboBox->currentText() == "Gsm") {
            displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        if(netStatusComboBox->currentText() == "Cdma") {
            displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        if(netStatusComboBox->currentText() == "Wcdma") {
            displayNetworkStatus(status);
        }
    }

}

void Dialog::networkModeChanged(QSystemNetworkInfo::NetworkMode mode)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        primaryModeLabel->setText("Wlan");
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        primaryModeLabel->setText("Ethernet");
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        primaryModeLabel->setText("Gsm");
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        primaryModeLabel->setText("Cdma");
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        primaryModeLabel->setText("Wcdma");
    }
    if(mode == QSystemNetworkInfo::UnknownMode) {
        primaryModeLabel->setText("None");
    }
}


void Dialog::displayNetworkStatus(QSystemNetworkInfo::NetworkStatus status)
{
    QString stat;
    switch(status) {
    case QSystemNetworkInfo::UndefinedStatus:
        stat = "Undefined";
        break;
    case QSystemNetworkInfo::NoNetworkAvailable:
        stat = "No Network Available";
        break;
    case QSystemNetworkInfo::EmergencyOnly:
        stat = "Emergency Only";
        break;
    case QSystemNetworkInfo::Searching:
        stat = "Searching or Connecting";
        break;
    case QSystemNetworkInfo::Busy:
        stat = "Busy";
        break;
    case QSystemNetworkInfo::Connected:
        stat = "Connected";
        break;
    case QSystemNetworkInfo::HomeNetwork:
        stat = "Home Network";
        break;
    case QSystemNetworkInfo::Denied:
        stat = "Denied";
        break;
    case QSystemNetworkInfo::Roaming:
        stat = "Roaming";
        break;
    };
    cellNetworkStatusLabel->setText(stat);
}

void Dialog::updateProfile()
{
    if(di) {
        QString profilestring;
        switch(di->currentProfile()) {
            case QSystemDeviceInfo::UnknownProfile:
            {
                profilestring = "Unknown";
            }
            break;
            case QSystemDeviceInfo::SilentProfile:
            {
                profilestring = "Silent";
            }
            break;
            case QSystemDeviceInfo::NormalProfile:
            {
                profilestring = "Normal";
            }
            break;
            case QSystemDeviceInfo::LoudProfile:
            {
                profilestring = "Loud";
            }
            break;
            case QSystemDeviceInfo::VibProfile:
            {
                profilestring = "Vibrate";
            }
            break;
            case QSystemDeviceInfo::OfflineProfile:
            {
                profilestring = "Offline";
            }
            break;
            case QSystemDeviceInfo::PowersaveProfile:
            {
                profilestring = "Powersave";
            }
            break;
            case QSystemDeviceInfo::CustomProfile:
                {
                    profilestring = "custom";
                }
                break;
        };
        profileLabel->setText(profilestring);
    }
}


void Dialog::updateSimStatus()
{
    if(di) {
        QString simstring;
        switch(di->simStatus()) {
        case QSystemDeviceInfo::SimLocked:
            {
                simstring = "Sim Locked";
            }
            break;
        case QSystemDeviceInfo::SimNotAvailable:
            {
                simstring = "Sim not available";
            }
            break;
        case QSystemDeviceInfo::SingleSimAvailable:
            {
                simstring = "Single Sim Available";

            }
            break;
        case QSystemDeviceInfo::DualSimAvailable:
            {
                simstring = "Dual Sim available";
            }
            break;

        };
        simStatusLabel->setText(simstring);
    }
}


