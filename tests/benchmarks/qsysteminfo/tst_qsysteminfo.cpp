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
#include <QtTest/QtTest>
#include "qsysteminfo.h"
#include <QDebug>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemInfo::Version);
Q_DECLARE_METATYPE(QSystemInfo::Feature);
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkStatus);
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkMode)

class tst_QSystemInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_createTime();

    void tst_currentLanguage();
    void tst_availableLanguages();

    void tst_versions_data();
    void tst_versions();

    void tst_hasFeatures_data();
    void tst_hasFeatures();
    
    void tst_networkinfo_mode_data();
    void tst_networkinfo_mode();
    
    void tst_networkinfo_misc_data();
    void tst_networkinfo_misc();
    
    void tst_screeninfo_data();
    void tst_screeninfo();
    
    void tst_storageinfo_data();
    void tst_storageinfo();

    void tst_deviceinfo_data();
    void tst_deviceinfo();

    void tst_screensaver_data();
    void tst_screensaver();
    
};

void tst_QSystemInfo::initTestCase()
{
    qRegisterMetaType<QSystemInfo::Version>("QSystemInfo::Version");
    qRegisterMetaType<QSystemInfo::Feature>("QSystemInfo::Feature");
}

void tst_QSystemInfo::tst_createTime()
{
    QBENCHMARK {
        QSystemInfo si;
    }
}

void tst_QSystemInfo::tst_currentLanguage()
{
    QSystemInfo si;
    QBENCHMARK {
        QString s = si.currentLanguage();
    }

}

void tst_QSystemInfo::tst_availableLanguages()
{
    QSystemInfo si;
    QBENCHMARK {
        QStringList available = si.availableLanguages();
    }
}

void tst_QSystemInfo::tst_versions_data()
{
    QTest::addColumn<QSystemInfo::Version>("version");
    QTest::addColumn<QString>("parameter");

    QTest::newRow("Os") << QSystemInfo::Os << "";
    QTest::newRow("QtCore") << QSystemInfo::QtCore << "";
    QTest::newRow("Firmware") << QSystemInfo::Firmware << "";

}

void tst_QSystemInfo::tst_versions()
{
  QFETCH(QSystemInfo::Version, version);
  QFETCH(QString, parameter);
  QSystemInfo si;
    
    QBENCHMARK { 
        QString vers = si.version(version, parameter);
    }
}

void tst_QSystemInfo::tst_hasFeatures_data()
{
    QTest::addColumn<QSystemInfo::Feature>("feature");

    QTest::newRow("Bluetooth") << QSystemInfo::BluetoothFeature;
    QTest::newRow("CameraFeature") << QSystemInfo::CameraFeature;
    QTest::newRow("FmradioFeature") << QSystemInfo::FmradioFeature;
    QTest::newRow("IrFeature") << QSystemInfo::IrFeature;
    QTest::newRow("LedFeature") << QSystemInfo::LedFeature;
    QTest::newRow("MemcardFeature") << QSystemInfo::MemcardFeature;
    QTest::newRow("UsbFeature") << QSystemInfo::UsbFeature;
    QTest::newRow("VibFeature") << QSystemInfo::VibFeature;
    QTest::newRow("WlanFeature") << QSystemInfo::WlanFeature;
    QTest::newRow("SimFeature") << QSystemInfo::SimFeature;
    QTest::newRow("LocationFeature") << QSystemInfo::LocationFeature;
    QTest::newRow("VideoOutFeature") << QSystemInfo::VideoOutFeature;
    QTest::newRow("HapticsFeature") << QSystemInfo::HapticsFeature;
}

void tst_QSystemInfo::tst_hasFeatures()
{
  QFETCH(QSystemInfo::Feature, feature);
  QSystemInfo si;
  
    QBENCHMARK {        
        si.hasFeatureSupported(feature);

    }
}

void tst_QSystemInfo::tst_networkinfo_misc_data() {
  QTest::addColumn<QString>("feature");
  QTest::addColumn<int>("property");
  
  QSystemNetworkInfo obj;
  const QMetaObject* metaObject = obj.metaObject();
  QStringList methods;
  for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
    QString name = QString::fromLatin1(metaObject->property(i).name());
    QTest::newRow(name.toAscii()) << name << i;    
  }
}

void tst_QSystemInfo::tst_networkinfo_misc() {
  QFETCH(QString, feature);
  QFETCH(int, property);  
  
  QSystemNetworkInfo obj;
  const QMetaObject* metaObject = obj.metaObject();
  QMetaProperty mprop = metaObject->property(property);
  
  if(mprop.isReadable()){
    QBENCHMARK {
      mprop.read(&obj);
    }
  }  
  else {
    QString s("Failed to find readable property: " + QString::fromLatin1(mprop.name()));
    QFAIL(s.toAscii());
  }
}


void tst_QSystemInfo::tst_networkinfo_mode_data()

{
  QTest::addColumn<QString>("feature");
  QTest::addColumn<QString>("name");
  QTest::addColumn<QSystemNetworkInfo::NetworkMode>("mode");
  
  
  QList<QPair<QString, QSystemNetworkInfo::NetworkMode> > modes;
   
  modes += qMakePair(QString("GsmMode"), QSystemNetworkInfo::GsmMode);
  modes += qMakePair(QString("CdmaMode"), QSystemNetworkInfo::CdmaMode);
  modes += qMakePair(QString("WcdmaMode"), QSystemNetworkInfo::WcdmaMode);
  modes += qMakePair(QString("WlanMode"), QSystemNetworkInfo::WlanMode);
  modes += qMakePair(QString("EthernetMode"), QSystemNetworkInfo::EthernetMode);
  modes += qMakePair(QString("BluetoothMode"), QSystemNetworkInfo::BluetoothMode);
  modes += qMakePair(QString("WimaxMode"), QSystemNetworkInfo::WimaxMode);
  
  QPair<QString, QSystemNetworkInfo::NetworkMode> mode;
  foreach(mode, modes){    
    QTest::newRow(QString("networkStatus-" + mode.first).toAscii()) << "networkStatus" << mode.first << mode.second;  
    QTest::newRow(QString("networkSignalStrength-" + mode.first).toAscii()) << "networkSignalStrength" << mode.first << mode.second;
    QTest::newRow(QString("macAddress-" + mode.first).toAscii()) << "macAddress" << mode.first << mode.second;
    QTest::newRow(QString("networkName-" + mode.first).toAscii()) << "networkName" << mode.first << mode.second;
    QTest::newRow(QString("interfaceForMode-" + mode.first).toAscii()) << "interfaceForMode" << mode.first << mode.second;
  }
}

void tst_QSystemInfo::tst_networkinfo_mode()
{
  QFETCH(QString, feature);
  QFETCH(QString, name);
  QFETCH(QSystemNetworkInfo::NetworkMode, mode);
  
  QSystemNetworkInfo ni;
  
  if(feature == "networkStatus"){
    QSystemNetworkInfo::NetworkStatus s = QSystemNetworkInfo::UndefinedStatus;
    QBENCHMARK {
      s = ni.networkStatus(mode);
    }
    //qDebug() << "Status: " << s;
  }
  else if(feature == "networkSignalStrength") {
    QBENCHMARK {
      ni.networkSignalStrength(mode);
    }
  }
  else if(feature == "macAddress"){
    QBENCHMARK {
      ni.macAddress(mode);
    }
  }
  else if(feature == "networkName"){
    QBENCHMARK {
      ni.networkName(mode);
    }
  }
  else if(feature == "interfaceForMode") {
    QBENCHMARK {
      ni.interfaceForMode(mode);
    }
  }
}

void tst_QSystemInfo::tst_screeninfo_data(){ 
  QTest::addColumn<QString>("feature");
  
  QTest::newRow("construction") << "construction";
  QTest::newRow("displayBrightness") << "displayBrightness";
  QTest::newRow("colorDepth") << "colorDepth";
    
}

void tst_QSystemInfo::tst_screeninfo(){
  QFETCH(QString, feature);   

  if(feature == "construction"){
    QBENCHMARK {
      QSystemDeviceInfo di2;
    }
  }
  else if(feature == "displayBrightness"){
    QBENCHMARK {
      QSystemDisplayInfo::displayBrightness(0);// what screen shoiuld we pick?
    }
  }
  else if(feature == "colorDepth"){
    QBENCHMARK {
      QSystemDisplayInfo::colorDepth(0);
    }
  }
  else {    
    QFAIL("Unkown feature");
  }  
}

void tst_QSystemInfo::tst_storageinfo_data(){
  QTest::addColumn<QString>("feature");
  QTest::addColumn<QString>("drive");
  
  QString d;
  QStringList drives = QSystemStorageInfo::logicalDrives();
  
  QString f;
  QStringList features;
  
  features += "totalDiskSpace";
  features += "availableDiskSpace";
  features += "typeForDrive";
  
  foreach(d, drives){
    foreach(f, features){
      QString s = f + "_" + d;
      QTest::newRow(s.toAscii()) << f << d;
    }
  }
}

void tst_QSystemInfo::tst_storageinfo(){
  QFETCH(QString, feature);
  QFETCH(QString, drive);
  
  QSystemStorageInfo si;
  
  if(feature == "totalDiskSpace"){
   QBENCHMARK {
     si.totalDiskSpace(drive);
   }
  }
  else if(feature == "availableDiskSpace"){
    QBENCHMARK {
      si.availableDiskSpace(drive);
    }
    
  }
  else if(feature == "typeForDrive"){
    QBENCHMARK {
      si.typeForDrive(drive);
    }
  }
  else {
    QFAIL("Unkown feature");
  }
}

void tst_QSystemInfo::tst_deviceinfo_data() {
  QTest::addColumn<QString>("feature");
  QTest::addColumn<int>("property");
  
  QSystemDeviceInfo obj;
  const QMetaObject* metaObject = obj.metaObject();
  QStringList methods;
  for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
    QString name = QString::fromLatin1(metaObject->property(i).name());
    QTest::newRow(name.toAscii()) << name << i;       
  }
}

void tst_QSystemInfo::tst_deviceinfo() {
  QFETCH(QString, feature);
  QFETCH(int, property);
  
  QSystemDeviceInfo obj;
  const QMetaObject* metaObject = obj.metaObject();
  QMetaProperty mprop = metaObject->property(property);
  
  if(mprop.isReadable()){
    QBENCHMARK {
      mprop.read(&obj);
    }
  }  
}

void tst_QSystemInfo::tst_screensaver_data() {
  QTest::addColumn<QString>("feature");
  QTest::addColumn<int>("property");
  
  QSystemScreenSaver obj;
  const QMetaObject* metaObject = obj.metaObject();
  QStringList methods;
  for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
    QString name = QString::fromLatin1(metaObject->property(i).name());
    QTest::newRow(name.toAscii()) << name << i;       
  }
}

void tst_QSystemInfo::tst_screensaver() {
  QFETCH(QString, feature);
  QFETCH(int, property);
  
  QSystemScreenSaver obj;
  const QMetaObject* metaObject = obj.metaObject();
  QMetaProperty mprop = metaObject->property(property);
  
  if(mprop.isReadable()){
    QBENCHMARK {
      mprop.read(&obj);
    }
  }  
}


QTEST_MAIN(tst_QSystemInfo)
#include "tst_qsysteminfo.moc"
 
