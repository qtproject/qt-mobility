#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <qsysteminfo.h>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setupGeneral();
    setupDevice();
    setupDisplay();
    setupMemory();
    setupNetwork();
    setupSaver();

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
   connect(ui->versionComboBox,SIGNAL(activated(int)), this,SLOT(getVersion(int)));
   connect(ui->featureComboBox,SIGNAL(activated(int)), this,SLOT(getFeature(int)));

}

Dialog::~Dialog()
{
    delete ui;
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
        setupMemory();
        break;
    case 4:
        setupNetwork();
        break;
        case 5:
        setupSaver();
        break;
    };
}

void Dialog::setupGeneral()
{
    QSystemInfo si;
    ui->curLanguageLineEdit->setText( si.currentLanguage());
    ui->languagesComboBox->insertItems(0,si.availableLanguages());
    ui->countryCodeLabel->setText(si.currentCountryCode());
}

void Dialog::setupDevice()
{
    QSystemDeviceInfo di;
    ui->batteryLevelBar->setValue(di.batteryLevel());
    ui->batteryChargingCheckBox->setChecked(di.isBatteryCharging());

    ui->ImeiLabel->setText(di.imei());
    ui->imsiLabel->setText(di.imsi());
    ui->manufacturerLabel->setText(di.manufacturer());
    ui->modelLabel->setText(di.model());
    ui->productLabel->setText(di.productName());

    ui->deviceLockCheckBox->setChecked(di.isDeviceLocked());

    if(di.currentPowerState() == QSystemDeviceInfo::BatteryPower) {
        ui->radioButton_2->setChecked(true);
    } else
        if(di.currentPowerState() == QSystemDeviceInfo::WallPower) {
        ui->radioButton_3->setChecked(true);
    } else {
        ui->radioButton->setChecked(true);
    }
}

void Dialog::setupDisplay()
{
    QSystemDisplayInfo di;
    ui->brightnessLineEdit->setText(QString::number(di.displayBrightness(0)));
    ui->colorDepthLineEdit->setText(QString::number(di.colorDepth((0))));

}

void Dialog::setupMemory()
{
    QSystemMemoryInfo mi;
    ui->memoryTreeWidget->clear();

    QStringList vols = mi.listOfVolumes();
    foreach(QString volName, vols) {
        QString type;
        QSystemMemoryInfo::VolumeType volType;
        volType = mi.volumeType(volName);
        if(volType == QSystemMemoryInfo::Internal) {
            type =  "Internal";
        } else
            if(volType == QSystemMemoryInfo::Removable) {
            type = "Removable";
        }
        if(volType == QSystemMemoryInfo::Cdrom) {
            type =  "Cdrom";
        }
        QStringList items;
        items << volName;
        items << type;
        items << QString::number(mi.totalDiskSpace(volName));
        items << QString::number(mi.availableDiskSpace(volName));
        QTreeWidgetItem *item = new QTreeWidgetItem(items);
        ui->memoryTreeWidget->addTopLevelItem(item);
    }
}

void Dialog::setupNetwork()
{
    QSystemNetworkInfo ni;
    connect(ui->netStrengthComboBox,SIGNAL(activated(int)),
            this, SLOT(netComboActivated(int)));
    connect(ui->netStatusComboBox,SIGNAL(activated(int)),
            this, SLOT(netStatusComboActivated(int)));

    ui->cellIdLabel->setText(QString::number(ni.cellId()));
    ui->locationAreaCodeLabel->setText(QString::number(ni.locationAreaCode()));
    ui->currentMMCLabel->setText(ni.currentMobileCountryCode());
    ui->currentMNCLabel->setText(ni.currentMobileNetworkCode());

    ui->homeMMCLabel->setText(ni.homeMobileCountryCode());
    ui->homeMNCLabel->setText(ni.homeMobileNetworkCode());
    ui->operatorNameLabel->setText(ni.operatorName());
}
void Dialog::netStatusComboActivated(int index)
{
    QString status;

    QString stat;
    QSystemNetworkInfo ni;
    switch(ni.networkStatus( (QSystemNetworkInfo::NetworkMode)index)) {
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
        stat = "Searching";
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

void Dialog::netComboActivated(int index)
{
    QSystemNetworkInfo::NetworkMode mode;
    switch(index) {
    case 1:
        mode = QSystemNetworkInfo::GsmMode;
        break;
    case 2:
        mode = QSystemNetworkInfo::CdmaMode;
        break;
    case 3:
        mode = QSystemNetworkInfo::WcdmaMode;
        break;
    case 4:
        mode = QSystemNetworkInfo::WlanMode;
        break;
    case 5:
        mode = QSystemNetworkInfo::EthMode;
        break;
    };
    QSystemNetworkInfo ni;
    ui->signalLevelProgressBar->setValue(ni.networkSignalStrength(mode));
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
        version = QSystemInfo::WrtCore;
        break;
    case 3:
        version = QSystemInfo::Webkit;
        break;
    case 4:
        version = QSystemInfo::ServiceFramework;
        break;
    case 5:
        version = QSystemInfo::WrtExtensions;
        break;
    case 6:
        version = QSystemInfo::ServiceProvider;
        break;
    case 7:
        version = QSystemInfo::NetscapePlugin;
        break;
    case 8:
        version = QSystemInfo::WebApp;
        break;
    case 9:
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
//    QSystemMemoryInfo mi;
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
//        QSystemMemoryInfo::VolumeType volType;
//        volType = mi.getVolumeType(vol);
//        if(volType == QSystemMemoryInfo::Internal) {
//                ui->diskSpaceLineEdit->setText( "Internal");
//        } else
//        if(volType == QSystemMemoryInfo::Removable) {
//                ui->diskSpaceLineEdit->setText( "Removable");
//        }
//        if(volType == QSystemMemoryInfo::Cdrom) {
//                ui->diskSpaceLineEdit->setText( "Cdrom");
//        }
//        break;
//    };
//
//}

void Dialog::setupSaver()
{
    saver = new QSystemScreenSaver();
    bool saverEnabled = saver->screenSaverEnabled();
    bool blankingEnabled = saver->screenBlankingEnabled();

    ui->saverEnabledPushButton->setChecked(saverEnabled);
    ui->blankingEnabledPushButton->setChecked(blankingEnabled);
    ui->screenLockCheckBox->setChecked(saver->isScreenLockOn());

    connect( ui->saverEnabledPushButton, SIGNAL(clicked(bool)),
             this,SLOT(setSaverEnabled(bool)));
    connect( ui->blankingEnabledPushButton, SIGNAL(clicked(bool)),
             this,SLOT(setBlankingEnabled(bool)));
}


void Dialog::setSaverEnabled(bool b)
{
    if(!saver->setScreenSaverEnabled(b))
        ui->saverEnabledPushButton->setChecked(!b);
}

void Dialog::setBlankingEnabled(bool b)
{
    if(!saver->setScreenBlankingEnabled(b))
        ui->blankingEnabledPushButton->setChecked(!b);
}
