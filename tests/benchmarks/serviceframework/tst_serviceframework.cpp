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
#include "qservicemanager.h"
#include <QDebug>

QTM_USE_NAMESPACE

class tst_serviceframework : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_createTime();
    void tst_addService_data();
    void tst_addService();

    void tst_findServices();

    void tst_loadService_data();
    void tst_loadService();

    void tst_invalidLoad();

private:
    QServiceManager *manager;

};

void tst_serviceframework::initTestCase()
{
    manager = new QServiceManager;
    QStringList exampleXmlFiles;
    //exampleXmlFiles <<"voipdialerservice.xml" << "landlinedialerservice.xml";
    exampleXmlFiles << "bm_sampleservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/plugins" + "/xmldata/" + fileName;
        //qDebug() << "Addning: " << path;
        manager->addService(path);
    }
}

void tst_serviceframework::tst_createTime()
{
    QBENCHMARK {
        QServiceManager qsm;
    }    
}

void tst_serviceframework::tst_addService_data()
{    
    QTest::addColumn<QString>("service");

    QStringList exampleXmlFiles;
    //exampleXmlFiles <<"voipdialerservice.xml" << "landlinedialerservice.xml";
    exampleXmlFiles << "bm_sampleservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {        
        QString path = QCoreApplication::applicationDirPath() + "/plugins" + "/xmldata/" + fileName;
        //qDebug() << "Addning: " << path;
        QTest::newRow(fileName.toAscii()) << path;
    }
}

void tst_serviceframework::tst_addService()
{
    QServiceManager qsm;
    QFETCH(QString, service);

    QBENCHMARK {
        qsm.addService(service);
    }
}

Q_DECLARE_METATYPE ( QServiceInterfaceDescriptor )

void tst_serviceframework::tst_loadService_data()
{
    qRegisterMetaType<QServiceInterfaceDescriptor>("QServiceInterfaceDescriptor");

    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("service");
    QTest::addColumn<QServiceInterfaceDescriptor>("interface");    

    QStringList exampleXmlFiles;
    //exampleXmlFiles <<"voipdialerservice.xml" << "landlinedialerservice.xml";
    exampleXmlFiles << "bm_sampleservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QServiceManager qsm;
        QString path = QCoreApplication::applicationDirPath() + "/plugins" + "/xmldata/" + fileName;
        //qDebug() << "Adding: " << path;
        qsm.addService(path);
        QStringList servicesList = qsm.findServices();
        foreach (QString service, servicesList) {
            //qDebug() << "Service: " << service;
            QList<QServiceInterfaceDescriptor> qlid = qsm.findInterfaces(service);
            while(!qlid.isEmpty()){
                QServiceInterfaceDescriptor id = qlid.takeFirst();
                QStringList name;
                QString verM, verm;
                verM.setNum(id.majorVersion(), 10);
                verm.setNum(id.minorVersion(), 10);
                name << "Service: " << service << " Interface: " << id.interfaceName() << " version: " << verM << "/" << verm;
                QTest::newRow(name.join("").toAscii()) << path << service << id;
            }
        }
    }
}

void tst_serviceframework::tst_loadService()
{
    QServiceManager qsm;
    QFETCH(QString, path);
    QFETCH(QString, service);
    QFETCH(QServiceInterfaceDescriptor, interface);    

    qsm.addService(path);

    QBENCHMARK {        
        QObject *o = qsm.loadInterface(interface);
        Q_UNUSED(o);
    }
}


void tst_serviceframework::tst_findServices()
{
    QServiceManager qsm;
    QStringList s;
    QBENCHMARK {
        s = qsm.findServices();
    }    
}

void tst_serviceframework::tst_invalidLoad()
{
    QServiceManager qsm;    

    QBENCHMARK {
        qsm.addService("invalid_filename_that_we_home_does_not_exist_aspasdm");
    }
}



QTEST_MAIN(tst_serviceframework)
#include "tst_serviceframework.moc"
