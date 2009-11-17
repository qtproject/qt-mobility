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

#include "dialog.h"
#ifdef Q_OS_SYMBIAN
#include "ui_dialog_s60.h"
#else
#include "ui_dialog.h"
#endif
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    saver(NULL), systemInfo(NULL), di(NULL), ni(NULL)
{
    ui->setupUi(this);
    setupGeneral();

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
    connect(ui->versionComboBox,SIGNAL(activated(int)), this,SLOT(getVersion(int)));
    connect(ui->featureComboBox,SIGNAL(activated(int)), this,SLOT(getFeature(int)));

}

Dialog::~Dialog()
{
    delete ui;
    delete systemInfo;
    delete di;
    delete saver;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::tabChanged(int index)
{
#ifdef Q_OS_SYMBIAN
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
    systemInfo = new QSystemInfo(this);
    ui->curLanguageLineEdit->setText( systemInfo->currentLanguage());
    ui->languagesComboBox->clear();
    ui->languagesComboBox->insertItems(0,systemInfo->availableLanguages());
    ui->countryCodeLabel->setText(systemInfo->currentCountryCode());
}

void Dialog::setupDevice()
{
    delete di;
    di = new QSystemDeviceInfo(this);
    ui->batteryLevelBar->setValue(di->batteryLevel());

    connect(di,SIGNAL(batteryLevelChanged(int)),
            this,SLOT(updateBatteryStatus(int)));

    connect(di,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this,SLOT(displayBatteryStatus(QSystemDeviceInfo::BatteryStatus)));

    connect(di,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this,SLOT(updatePowerState(QSystemDeviceInfo::PowerState)));

    ui->ImeiLabel->setText(di->imei());
    ui->imsiLabel->setText(di->imsi());
    ui->manufacturerLabel->setText(di->manufacturer());
    ui->modelLabel->setText(di->model());
    ui->productLabel->setText(di->productName());

    ui->deviceLockCheckBox->setChecked(di->isDeviceLocked());

    ui->profileComboBox->setCurrentIndex(di->currentProfile());
    connect(di, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
        this, SLOT(updateProfile(QSystemDeviceInfo::Profile)));

    if(di->currentPowerState() == QSystemDeviceInfo::BatteryPower) {
        ui->radioButton_2->setChecked(true);
    } else  if(di->currentPowerState() == QSystemDeviceInfo::WallPower) {
        ui->radioButton_3->setChecked(true);
    } else if(di->currentPowerState() == QSystemDeviceInfo::WallPowerChargingBattery) {
        ui->radioButton_4->setChecked(true);
    } else {
        ui->radioButton->setChecked(true);
    }

    QSystemDeviceInfo::InputMethodFlags methods = di->inputMethodType();
    QStringList inputs;
    if((methods & QSystemDeviceInfo::Keys)){
        inputs << "Keys";
    }
    if((methods & QSystemDeviceInfo::Keypad)) {
        inputs << "Keypad";
    }
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

    ui->inputMethodLabel->setText(inputs.join(" "));
}

void Dialog::updateProfile(QSystemDeviceInfo::Profile profile)
{
    ui->profileComboBox->setCurrentIndex(profile);
}

void Dialog::setupDisplay()
{
    QSystemDisplayInfo di;
    ui->brightnessLineEdit->setText(QString::number(di.displayBrightness(0)));
    ui->colorDepthLineEdit->setText(QString::number(di.colorDepth((0))));

}

void Dialog::setupStorage()
{
    QSystemStorageInfo mi;
    ui->storageTreeWidget->clear();
    ui->storageTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

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
        ui->storageTreeWidget->addTopLevelItem(item);
    }
}

void Dialog::setupNetwork()
{
    delete ni;
    ni = new QSystemNetworkInfo(this);

    connect(ui->netStatusComboBox,SIGNAL(activated(int)),
            this, SLOT(netStatusComboActivated(int)));

    connect(ni,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)),
            this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));

    connect(ni,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
            this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)));

    connect(ni,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
            this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)));

    ui->cellIdLabel->setText(QString::number(ni->cellId()));
    ui->locationAreaCodeLabel->setText(QString::number(ni->locationAreaCode()));
    ui->currentMMCLabel->setText(ni->currentMobileCountryCode());
    ui->currentMNCLabel->setText(ni->currentMobileNetworkCode());

    ui->homeMMCLabel->setText(ni->homeMobileCountryCode());
    ui->homeMNCLabel->setText(ni->homeMobileNetworkCode());
}
void Dialog::netStatusComboActivated(int index)
{
    QString status;
    int reIndex = index +1;

    displayNetworkStatus(ni->networkStatus((QSystemNetworkInfo::NetworkMode)reIndex));

    ui->macAddressLabel->setText(ni->macAddress((QSystemNetworkInfo::NetworkMode)reIndex));

    int strength = ni->networkSignalStrength((QSystemNetworkInfo::NetworkMode)reIndex);
    if(strength < 0)
        strength = 0;
    ui->signalLevelProgressBar->setValue(strength);

    ui->InterfaceLabel->setText(ni->interfaceForMode((QSystemNetworkInfo::NetworkMode)reIndex).humanReadableName());

    ui->operatorNameLabel->setText(ni->networkName((QSystemNetworkInfo::NetworkMode)reIndex));
}

void Dialog::getVersion(int index)
{
    QSystemInfo::Version version;
    switch(index) {
    case 0:
        version = QSystemInfo::Os;
        break;
    case 1:
        version = QSystemInfo::QtCore;
        break;
    case 2:
        version = QSystemInfo::Firmware;
        break;
    };

    QSystemInfo si;
    ui->versionLineEdit->setText(si.version(version));
}

void Dialog::getFeature(int index)
{
    QSystemInfo::Feature feature;
    switch(index) {
    case 0:
        feature = QSystemInfo::BluetoothFeature;
        break;
    case 1:
        feature = QSystemInfo::CameraFeature;
        break;
    case 2:
        feature = QSystemInfo::FmradioFeature;
        break;
    case 3:
        feature = QSystemInfo::IrFeature;
        break;
    case 4:
        feature = QSystemInfo::LedFeature;
        break;
    case 5:
        feature = QSystemInfo::MemcardFeature;
        break;
    case 6:
        feature = QSystemInfo::UsbFeature;
        break;
    case 7:
        feature = QSystemInfo::VibFeature;
        break;
    case 8:
        feature = QSystemInfo::WlanFeature;
        break;
    case 9:
        feature = QSystemInfo::SimFeature;
        break;
    case 10:
        feature = QSystemInfo::LocationFeature;
        break;
    case 11:
        feature = QSystemInfo::VideoOutFeature;
        break;
    case 12:
        feature = QSystemInfo::HapticsFeature;
        break;
    };
    QSystemInfo si;
    ui->featuresLineEdit->setText((si.hasFeatureSupported(feature) ? "true":"false" ));
}

//void Dialog::doVolumes(int /*index*/)
//{
//    QSystemStorageInfo mi;
//    QString vol = ui->volumesComboBox->currentText();
//    int index2 = ui->diskComboBox->currentIndex();
//    switch(index2) {
//    case 0:
//        //total
//        ui->diskSpaceLineEdit->setText( QString::number(mi.totalDiskSpace(vol)));
//        break;
//        case 1:
//        //available
//        ui->diskSpaceLineEdit->setText( QString::number(mi.availableDiskSpace(vol)));
//        break;
//        case 2:
//        //type
//        QSystemStorageInfo::VolumeType volType;
//        volType = mi.getVolumeType(vol);
//        if(volType == QSystemStorageInfo::Internal) {
//                ui->diskSpaceLineEdit->setText( "Internal");
//        } else
//        if(volType == QSystemStorageInfo::Removable) {
//                ui->diskSpaceLineEdit->setText( "Removable");
//        }
//        if(volType == QSystemStorageInfo::Cdrom) {
//                ui->diskSpaceLineEdit->setText( "Cdrom");
//        }
//        break;
//    };
//
//}

void Dialog::setupSaver()
{
    if (!saver) {
        saver = new QSystemScreenSaver(this);
    }

    bool saverEnabled = saver->screenSaverInhibited();


    connect( ui->saverInhibitedCheckBox, SIGNAL(clicked(bool)),
             this,SLOT(setSaverEnabled(bool)));

    ui->saverInhibitedCheckBox->setChecked(saverEnabled);
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
    ui->batteryLevelBar->setValue(level);
}

void Dialog::updatePowerState(QSystemDeviceInfo::PowerState newState)
{

    switch (newState) {
    case QSystemDeviceInfo::BatteryPower:
        {
            ui->radioButton_2->setChecked(true);
        }
        break;
    case QSystemDeviceInfo::WallPower:
        {
            ui->radioButton_3->setChecked(true);
        }
        break;
    case QSystemDeviceInfo::WallPowerChargingBattery:
        ui->radioButton_4->setChecked(true);
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
        };
  //  }

}

void Dialog::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode , int strength)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        if(ui->netStatusComboBox->currentText() == "Wlan") {
            ui->signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        if(ui->netStatusComboBox->currentText() == "Ethernet") {
            ui->signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        if(ui->netStatusComboBox->currentText() == "Gsm") {
            ui->signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        if(ui->netStatusComboBox->currentText() == "Cdma") {
            ui->signalLevelProgressBar->setValue(strength);
        }
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        if(ui->netStatusComboBox->currentText() == "Wcdma") {
            ui->signalLevelProgressBar->setValue(strength);
        }
    }

}

void Dialog::networkNameChanged(QSystemNetworkInfo::NetworkMode mode,const QString &text)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        if(ui->netStatusComboBox->currentText() == "Wlan") {
            ui->operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        if(ui->netStatusComboBox->currentText() == "Ethernet") {
            ui->operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        if(ui->netStatusComboBox->currentText() == "Gsm") {
            ui->operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        if(ui->netStatusComboBox->currentText() == "Cdma") {
            ui->operatorNameLabel->setText(text);
        }
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        if(ui->netStatusComboBox->currentText() == "Wcdma") {
            ui->operatorNameLabel->setText(text);
        }
    }

}

void Dialog::networkStatusChanged(QSystemNetworkInfo::NetworkMode mode , QSystemNetworkInfo::NetworkStatus status)
{
    if(mode == QSystemNetworkInfo::WlanMode) {
        if(ui->netStatusComboBox->currentText() == "Wlan") {
           displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::EthernetMode) {
        if(ui->netStatusComboBox->currentText() == "Ethernet") {
           displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::GsmMode) {
        if(ui->netStatusComboBox->currentText() == "Gsm") {
            displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::CdmaMode) {
        if(ui->netStatusComboBox->currentText() == "Cdma") {
            displayNetworkStatus(status);
        }
    }

    if(mode == QSystemNetworkInfo::WcdmaMode) {
        if(ui->netStatusComboBox->currentText() == "Wcdma") {
            displayNetworkStatus(status);
        }
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
    ui->cellNetworkStatusLabel->setText(stat);
}
