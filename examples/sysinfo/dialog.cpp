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

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
   connect(ui->versionComboBox,SIGNAL(activated(int)), this,SLOT(getVersion(int)));
   connect(ui->featureComboBox,SIGNAL(activated(int)), this,SLOT(getFeature(int)));
   connect(ui->diskComboBox,SIGNAL(activated(int)), this,SLOT(doVolumes(int)));
   connect(ui->volumesComboBox,SIGNAL(activated(int)), this,SLOT(doVolumes(int)));

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
    };
}

void Dialog::setupGeneral()
{
    QSystemInfo si;
    ui->curLanguageLineEdit->setText( si.currentLanguage());
    ui->languagesComboBox->insertItems(0,si.availableLanguages());
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

    ui->screenLockCheckBox->setChecked(di.isScreenLockOn());
}

void Dialog::setupMemory()
{
    QSystemMemoryInfo mi;
      ui->volumesComboBox->clear();
    ui->volumesComboBox->insertItems(0,mi.listOfVolumes());

}

void Dialog::setupNetwork()
{

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
    ui->versionLineEdit->setText(si.getVersion(version));
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

void Dialog::doVolumes(int /*index*/)
{
    QSystemMemoryInfo mi;
    QString vol = ui->volumesComboBox->currentText();
    int index2 = ui->diskComboBox->currentIndex();
    switch(index2) {
    case 0:
        //total
        ui->diskSpaceLineEdit->setText( QString::number(mi.totalDiskSpace(vol)));
        break;
        case 1:
        //available
        ui->diskSpaceLineEdit->setText( QString::number(mi.availableDiskSpace(vol)));
        break;
        case 2:
        //type
        QSystemMemoryInfo::VolumeType volType;
        volType = mi.getVolumeType(vol);
        if(volType == QSystemMemoryInfo::Internal) {
                ui->diskSpaceLineEdit->setText( "Internal");
        } else
        if(volType == QSystemMemoryInfo::Removable) {
                ui->diskSpaceLineEdit->setText( "Removable");
        }
        if(volType == QSystemMemoryInfo::Cdrom) {
                ui->diskSpaceLineEdit->setText( "Cdrom");
        }
        break;
    };

}
