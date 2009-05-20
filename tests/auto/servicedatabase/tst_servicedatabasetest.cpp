/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QtCore>
#define private public
#include <qserviceinterfacedescriptor.h>
#include <qserviceinterfacedescriptor_p.h>
#include "../../../servicefw/serviceframework/serviceresolution/qserviceinterfacedescriptor.cpp"
#include <servicedatabase.h>
#include <qservicefilter.h>

#define RESOLVERDATABASE "services.db"
   
class ServiceDatabaseUnitTest: public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void testRegistration();
    void getInterfaces();
    void searchByInterfaceName();
    void searchByInterfaceAndService();
    void getServiceNames();
    void getService();
    void defaultServiceInterface();
    void setDefaultService_strings();
    void setDefaultService_descriptor();
    void unregister();
    void cleanupTestCase();

private:

     bool compareDescriptor(QServiceInterfaceDescriptor interface,
                            QString interfaceName,
                            QString serviceName,
                            int majorVersion,
                            int minorVersion);

    bool compareDescriptor(QServiceInterfaceDescriptor interface,
                            QString interfaceName,
                            QString serviceName,
                            int majorVersion,
                            int minorVersion,
                            QStringList capabilities,
                            QString filePath="",
                            QString serviceDescription="",
                            QString interfaceDescription="");

    bool compareService(const ServiceInfo &service,
                            const QString &name,
                            const QString &filePath,
                            const QString &description);

	ServiceMetaData* parser;
	QDir dir;
        ServiceDatabase database;

};

void ServiceDatabaseUnitTest::initTestCase()
{
    /*#ifdef __SYMBIAN32__
    database.setDatabasePath(QString("C:\\Data\\"));
    #endif*/
    dir = database.databasePath();
    QCOMPARE(database.open(), (int)ServiceDatabase::SFW_ERROR_DB_RECREATED);
    database.close();
}

void ServiceDatabaseUnitTest::testRegistration()
{
    QDir testdir = QDir(TESTDATA_DIR "/testdata" );

    ServiceMetaData parser(testdir.absoluteFilePath("ServiceAcme.xml"));
    parser.extractMetadata();
    QVERIFY(!database.registerService(parser));
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);

    QCOMPARE(database.open(), 0);
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceOmni.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceLuthorCorp.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceWayneEnt.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServicePrimatech.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceCyberdyne.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceSkynet.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceAutobot.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    //try to register an already registered service
    QVERIFY(!database.registerService(parser));
    QCOMPARE(database.lastError().errorCode(), DBError::ComponentAlreadyRegistered);

    //try to register a service with a dll that provides interface implementations
    //that are already provided by a currently registered service
    parser.setDevice(new QFile(testdir.absoluteFilePath("ServicePrimatech2error.xml")));
    QVERIFY(parser.extractMetadata());
    QVERIFY(!database.registerService(parser));
    QCOMPARE(database.lastError().errorCode(), DBError::IfaceImplAlreadyRegistered);

    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceYamagatoError.xml")));
    QVERIFY(parser.extractMetadata());
    QVERIFY(!database.registerService(parser));
    QCOMPARE(database.lastError().errorCode(), DBError::ComponentAlreadyRegistered);

    //make sure errors above are corectly rolled back by
    //registering a valid service
    parser.setDevice(new QFile(testdir.absoluteFilePath("ServiceDecepticon.xml")));
    parser.extractMetadata();
    QVERIFY(database.registerService(parser));

    QStringList xmlFiles;
    xmlFiles << "ServiceDharma_Swan.xml"
             << "ServiceDharma_Pearl.xml"
             << "ServiceDharma_Flame.xml";

    foreach(const QString &file, xmlFiles) {
        parser.setDevice(new QFile(testdir.absoluteFilePath(file)));
        QVERIFY(parser.extractMetadata());
        QVERIFY(database.registerService(parser));
    }

    QCOMPARE(database.close(),0);
}

void ServiceDatabaseUnitTest::getInterfaces()
{
    QServiceFilter filter;
    QList<QServiceInterfaceDescriptor> interfaces;
    bool ok;

    filter.service = "acme";
    interfaces = database.getInterfaces(filter, &ok);
    QVERIFY(!ok);
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);

    QCOMPARE(database.open(), 0);
    interfaces = database.getInterfaces(filter, &ok);
    QVERIFY(ok);
    QCOMPARE(interfaces.count(), 5);

    QStringList capabilities;
    compareDescriptor(interfaces[0], "com.acme.service.downloader", "acme", 1, 0, capabilities, "C:/TestData/testservice.dll");

    QServiceInterfaceDescriptor interface;
    QVERIFY(compareDescriptor(interfaces[1], "come.acme.service.location", "acme", 1,0, capabilities, "C:/TestData/testservice.dll"));
    capabilities.append(QString("ReadUserData"));
    QVERIFY(compareDescriptor(interfaces[2], "com.acme.device.sysinfo", "acme", 2, 3, capabilities, "C:/TestData/testservice.dll"));
    capabilities.clear();
    capabilities.append("ReadUserData");
    capabilities.append("WriteUserData");
    QVERIFY(compareDescriptor(interfaces[3], "com.acme.device.sendMessage", "acme", 3, 0, capabilities, "C:/TestData/testservice.dll"));

    capabilities.clear();
    capabilities.append("ReadUserData");
    capabilities.append("WriteUserData");
    capabilities.append("ExecUserData");
    QVERIFY(compareDescriptor(interfaces[4], "com.acme.device.receiveMessage", "acme", 1, 1, capabilities, "C:/TestData/testservice.dll"));

    //check that searching is case insensitive
    filter.service = "OmNi";
    interfaces = database.getInterfaces(filter, &ok);
    QVERIFY(ok);
    QCOMPARE(interfaces.count(), 3);

    capabilities.clear();
    capabilities << "SurroundingsDD";
    QVERIFY(compareDescriptor(interfaces[0], "com.omni.device.Accelerometer", "OMNI", 1, 1, capabilities, "C:/OmniInc/omniinc.dll"));

    capabilities.clear();
    QVERIFY(compareDescriptor(interfaces[1], "com.omni.device.Lights","OMNI", 9, 0, capabilities, "C:/OmniInc/omniinc.dll"));

    capabilities.clear();
    capabilities << "MultimediaDD" << "NetworkServices";
    QVERIFY(compareDescriptor(interfaces[2], "com.omni.service.Video", "OMNI", 1, 4, capabilities, "C:/OmniInc/omniinc.dll"));

    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::searchByInterfaceName()
{
    QServiceFilter filter;
    QList<QServiceInterfaceDescriptor> interfaces;

    QCOMPARE(database.open(), 0);

    QString iface = "com.omni.device.Accelerometer";
    // == search via interface name only ==
    filter.setInterface(iface);
    interfaces = database.getInterfaces(filter);

    QCOMPARE(interfaces.count(), 5);
    QStringList capabilities;
    capabilities << "SurroundingsDD";
    QVERIFY(compareDescriptor(interfaces[0], iface, "OMNI", 1, 1, capabilities,"C:/OmniInc/omniinc.dll"));
    QVERIFY(compareDescriptor(interfaces[1], iface, "LuthorCorp", 1, 2, capabilities,"C:/Metropolis/kryptonite.dll"));
    QVERIFY(compareDescriptor(interfaces[2], iface, "WayneEnt", 2, 0, capabilities,"C:/Gotham/knight.dll"));
    QVERIFY(compareDescriptor(interfaces[3], iface, "Primatech", 1, 4, capabilities,"C:/NewYork/kensei.dll"));
    QVERIFY(compareDescriptor(interfaces[4], iface, "Primatech", 1, 2, capabilities,"C:/NewYork/kensei.dll"));

    //search with non-existent interface name
    filter.setInterface("com.omni.device.FluxCapacitor");
    interfaces = database.getInterfaces(filter);

    QCOMPARE(interfaces.count(), 0);

    // == search for an exact version match ==
    filter.setInterface(iface, "1.4", QServiceFilter::ExactVersionMatch);
    interfaces = database.getInterfaces(filter);

    QCOMPARE(interfaces.count(), 1);
    QVERIFY(compareDescriptor(interfaces[0], iface, "Primatech", 1, 4));

    //try exact version match but with multiple expected instances returned.
    filter.setInterface("com.oMNi.device.accelerometer", "1.2", QServiceFilter::ExactVersionMatch); //also test case insensitivity
    interfaces = database.getInterfaces(filter);

    QCOMPARE(interfaces.count(), 2);
    QVERIFY(compareDescriptor(interfaces[0], iface, "LuthorCorp", 1, 2));
    QVERIFY(compareDescriptor(interfaces[1], iface, "Primatech", 1, 2));

    //try exact match for an interface that exists but a version that doesn't
    filter.setInterface(iface,"1.3", QServiceFilter::ExactVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 0);

    // == search for a minimum version match ==
    filter.setInterface(iface,"1.2", QServiceFilter::MinimumVersionMatch);//use existent version
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 4);

    QVERIFY(compareDescriptor(interfaces[0], iface, "LuthorCorp", 1, 2));
    QVERIFY(compareDescriptor(interfaces[1], iface, "WayneEnt", 2, 0));
    QVERIFY(compareDescriptor(interfaces[2], iface, "Primatech", 1, 4));
    QVERIFY(compareDescriptor(interfaces[3], iface, "Primatech", 1, 2));

    filter.setInterface(iface, "1.3", QServiceFilter::MinimumVersionMatch);//use non-existent version
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 2);
    QVERIFY(compareDescriptor(interfaces[0], iface, "WayneEnt", 2, 0));
    QVERIFY(compareDescriptor(interfaces[1], iface, "Primatech", 1, 4));

    //try minimum version match that will find all available versions
    filter.setInterface(iface,"1.0", QServiceFilter::MinimumVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 5);

    //try minimum version match but no implementations are available
    filter.setInterface(iface,"9.0", QServiceFilter::MinimumVersionMatch);//use non-existent version
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 0);

    //try setting an invalid version (should default to interface name search)
    filter.setInterface(iface,"-1.0", QServiceFilter::MinimumVersionMatch);//use non-existent version
    interfaces =database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 0);

    // == search for an interface spread over multiple plugins by a single service==
    iface = "com.dharma.electro.discharge";
    filter.setInterface(iface);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 4);
    capabilities.clear();
    QVERIFY(compareDescriptor(interfaces[0], iface, "DharmaInitiative", 4, 0, capabilities, "C:/island/swan.dll"));
    QVERIFY(compareDescriptor(interfaces[1], iface, "DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QVERIFY(compareDescriptor(interfaces[2], iface, "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QVERIFY(compareDescriptor(interfaces[3], iface, "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));

    //try searching by minimum version for interface implementation spread over multiple plugins
    filter.setInterface(iface, "5.0", QServiceFilter::MinimumVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 3);
    QVERIFY(compareDescriptor(interfaces[0], iface, "DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QVERIFY(compareDescriptor(interfaces[1], iface, "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QVERIFY(compareDescriptor(interfaces[2], iface, "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));

    //try searching for a single version of an interface implementation
    filter.setInterface(iface, "8.0", QServiceFilter::ExactVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 1);
    QVERIFY(compareDescriptor(interfaces[0], iface, "DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));

    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::searchByInterfaceAndService()
{
    QServiceFilter filter;
    QList<QServiceInterfaceDescriptor> interfaces;

    QCOMPARE(database.open(), 0);

    // == search using only interface and service name ==
    filter.setServiceName("Omni");
    filter.setInterface("com.omni.device.Lights");
    interfaces = database.getInterfaces(filter);
    QVERIFY(compareDescriptor(interfaces[0], "com.omni.device.Lights", "OMNI", 9, 0));

    QCOMPARE(interfaces.count(), 1);

    //try searching with service that implements the same interface
    //more than once
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.Accelerometer");
    interfaces = database.getInterfaces(filter);

    QCOMPARE(interfaces.count(), 2);
    QVERIFY(compareDescriptor(interfaces[0], "com.omni.device.Accelerometer", "Primatech", 1, 4));
    QVERIFY(compareDescriptor(interfaces[1], "com.omni.device.Accelerometer", "Primatech", 1, 2));

    //try and existing service but non-existent interface
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.FluxCapacitor");

    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 0);

    //try an non-existing service but an existing interface
    filter.setServiceName("StarkInd");
    filter.setInterface("com.omni.device.Accelerometer");

    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 0);

    // == search using interface and service name and exact version match ==
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.Accelerometer", "1.2", QServiceFilter::ExactVersionMatch);
    interfaces = database.getInterfaces(filter);

    QCOMPARE(interfaces.count(), 1);
    QVERIFY(compareDescriptor(interfaces[0], "com.omni.device.Accelerometer", "Primatech", 1, 2));

    //try an exact match for and non-existent interface version
    filter.setInterface("com.omni.device.Accelerometer","1.3", QServiceFilter::ExactVersionMatch);
    interfaces = database.getInterfaces(filter);

    // == search using interface and service name and minimum version match ==
    filter.setServiceName("Cyberdyne");
    filter.setInterface("com.cyberdyne.terminator","1.6", QServiceFilter::MinimumVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 4);

    QVERIFY(compareDescriptor(interfaces[0], "com.cyberdyne.terminator", "Cyberdyne", 2, 1));
    QVERIFY(compareDescriptor(interfaces[1], "com.cyberdyne.terminator", "Cyberdyne", 2, 0));
    QVERIFY(compareDescriptor(interfaces[2], "com.cyberdyne.terminator", "Cyberdyne", 1, 7));
    QVERIFY(compareDescriptor(interfaces[3], "com.cyberdyne.terminator", "Cyberdyne", 1, 6));

    //try again with the same interface but a different service
    filter.setServiceName("Skynet");
    filter.setInterface("com.cyberdyne.terminator", "1.6", QServiceFilter::MinimumVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 4);

    QVERIFY(compareDescriptor(interfaces[0], "com.cyberdyne.terminator", "skynet", 3, 6));
    QVERIFY(compareDescriptor(interfaces[1], "com.cyberdyne.terminator", "skynet", 2, 0));
    QVERIFY(compareDescriptor(interfaces[2], "com.cyberdyne.terminator", "skynet", 1, 8));
    QVERIFY(compareDescriptor(interfaces[3], "com.cyberdyne.terminator", "skynet", 1, 6));

    //try with a non-existent interface version (but later versions exist)
    filter.setServiceName("Skynet");
    filter.setInterface("com.cyberdyne.terminator","1.9", QServiceFilter::MinimumVersionMatch);
    interfaces = database.getInterfaces(filter);
    QVERIFY(compareDescriptor(interfaces[0], "com.cyberdyne.terminator", "skynet", 3, 6));
    QVERIFY(compareDescriptor(interfaces[1], "com.cyberdyne.terminator", "skynet", 2, 0));
    QCOMPARE(interfaces.count(), 2);

    // == using wildcard matching when searching, ie service and/or interface field is empty
    filter.setServiceName("");
    filter.setInterface("");
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 36);

    // == searches on a service which is made up of multiple plugins
    // try searching for all interfaces offered by the service
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("");
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 5);

    QStringList capabilities;
    QVERIFY(compareDescriptor(interfaces[0], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/swan.dll"));
    QVERIFY(compareDescriptor(interfaces[1], "com.dharma.electro.discharge", "DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QVERIFY(compareDescriptor(interfaces[2], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QVERIFY(compareDescriptor(interfaces[3], "com.dharma.radio", "DharmaInitiative", 8, 15, capabilities, "C:/island/flame.dll"));
    QVERIFY(compareDescriptor(interfaces[4], "com.dharma.electro.discharge", "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));

    // try searching for all implementations of a specific interface offered by the service
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("com.dharma.electro.discharge");
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 4);
    QVERIFY(compareDescriptor(interfaces[0], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/swan.dll"));
    QVERIFY(compareDescriptor(interfaces[1], "com.dharma.electro.discharge", "DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QVERIFY(compareDescriptor(interfaces[2], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QVERIFY(compareDescriptor(interfaces[3], "com.dharma.electro.discharge", "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));

    //try doing a minimum version search
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("com.dharma.electro.discharge", "7.9", QServiceFilter::MinimumVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 3);
    QVERIFY(compareDescriptor(interfaces[0], "com.dharma.electro.discharge", "DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QVERIFY(compareDescriptor(interfaces[1], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QVERIFY(compareDescriptor(interfaces[2], "com.dharma.electro.discharge", "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));

    //try doing a exact version search
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("com.dharma.electro.discharge", "15.0", QServiceFilter::ExactVersionMatch);
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 1);
    QVERIFY(compareDescriptor(interfaces[0], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));

    //trying setting invalid interface parameters, supply a version without an interface
    filter.setInterface("", "3.0", QServiceFilter::MinimumVersionMatch); //this call should be ignored
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 1);
    QVERIFY(compareDescriptor(interfaces[0], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));

    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::getServiceNames()
{
    QStringList services;
    bool ok;
    services = database.getServiceNames("com.acme.device.sysinfo");
    QVERIFY(!ok);
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);
    QCOMPARE(services.count(), 0);
    QCOMPARE(database.open(), 0);

    //try wildcard match to get all services
    services = database.getServiceNames("", &ok);
    QVERIFY(ok);
    QCOMPARE(services.count(), 10);

    //try an interface that is implemented by only one service
    services = database.getServiceNames("com.acme.device.sysinfo", &ok);
    QVERIFY(ok);
    QCOMPARE(services.count(), 1);
    QCOMPARE(services[0], QString("acme"));

    //try an interface which  is implmented by multiple services
    services = database.getServiceNames("COM.omni.device.ACCELerometer",&ok); //also test case insensitivity
    QVERIFY(ok);
    QCOMPARE(services.count(), 4);
    QVERIFY(services.contains("LuthorCorp"));
    QVERIFY(services.contains("OMNI"));
    QVERIFY(services.contains("Primatech"));
    QVERIFY(services.contains("WayneEnt"));

    //try again but with services that have multiple implementation versions of a particular implementation
    services = database.getServiceNames("com.cyberdyne.terminator", &ok);
    QVERIFY(ok);
    QCOMPARE(services.count(), 2);
    QVERIFY(services.contains("Cyberdyne"));
    QVERIFY(services.contains("skynet"));

    //try with an interface implemented in multiple plugins
    services = database.getServiceNames("com.dharma.electro.discharge", &ok);
    QVERIFY(ok);
    QCOMPARE(services.count(), 1);
    QVERIFY(services.contains("DharmaInitiative"));

    //try with a non-existing interface
    services = database.getServiceNames("com.omni.device.FluxCapacitor", &ok);
    QVERIFY(ok);
    QCOMPARE(services.count(), 0);

    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::getService()
{
    QServiceFilter filter;
    ServiceInfo service;
    QList<QServiceInterfaceDescriptor> interfaces;

    QCOMPARE(database.open(), 0);
    filter.setServiceName("acme");
    filter.setInterface("com.acme.device.sysinfo","2.3");

    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(),1);

    QCOMPARE(database.close(), 0);
    bool ok;
    service = database.getService(interfaces[0], &ok);
    QVERIFY(!ok);
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);
    QCOMPARE(database.open(), 0);
    service = database.getService(interfaces[0]);
    QVERIFY(service.isValid());
    QVERIFY(compareService(service,  "acme", "C:/TestData/testservice.dll", "Acme services"));

    //try a service that has multiple interface implementations, and with other services implmementing
    //the same interface
    filter.setServiceName("SkYneT");//test for case insensitivity
    filter.setInterface("Com.cyberDyne.tErminator", "1.6", QServiceFilter::ExactVersionMatch);
    interfaces.clear();
    interfaces =database.getInterfaces(filter);
    QCOMPARE(interfaces.count(),1);
    service = database.getService(interfaces[0], &ok);
    QVERIFY(ok);
    QVERIFY(service.isValid());
    QVERIFY(compareService(service, "skynet", "C:/California/dyson.dll", "Skynet Termination Services"));

    //try a service that is composed of multiple plugins
    QServiceInterfaceDescriptor interface;
    interface.d = new QServiceInterfaceDescriptorPrivate;
    interface.d->serviceName = "DharmaInitiative";
    interface.d->interfaceName = "com.dharma.electro.discharge";
    interface.d->major = 8;
    interface.d->minor = 0;
    service = database.getService(interface, &ok);
    QVERIFY(ok);
    QVERIFY(compareService(service, "DharmaInitiative", "C:/island/pearl.dll",
                "Department of Heuristics And Research on Material Applications(P)"));

    interface.d->serviceName = "DharmaInitiative";
    interface.d->interfaceName = "com.dharma.electro.discharge";
    interface.d->major = 4;
    interface.d->minor = 0;
    service = database.getService(interface, &ok);
    QVERIFY(ok);
    QVERIFY(compareService(service, "DharmaInitiative", "C:/island/swan.dll",
                "Department of Heuristics And Research on Material Applications(S)"));

    //try a an interface descriptor whose service doesn't exist;
    interface.d->serviceName = "StarkInd";
    interface.d->interfaceName = "com.omni.device.Accelerometer";
    interface.d->major = 1;
    interface.d->minor = 1;

    service = database.getService(interface, &ok);
    QVERIFY(ok); //no error condition occurred, the service simply didn't exist
    QCOMPARE(service.isValid(), false);

    //try a an interface descriptor whose interface name doesn't exist;
    interface.d->serviceName = "OMNI";
    interface.d->interfaceName = "com.omni.device.FluxCapacitor";
    interface.d->major = 1;
    interface.d->minor = 1;

    service = database.getService(interface);
    QCOMPARE(service.isValid(), false);

    //try a an interface descriptor whose version name doesn't exist;
    interface.d->serviceName = "OMNI";
    interface.d->interfaceName = "com.omni.device.Accelerometer";
    interface.d->major = 1;
    interface.d->minor = 9;

    service = database.getService(interface);
    QCOMPARE(service.isValid(), false);

    //try an invalid descriptor
    interface.d = NULL;
    service = database.getService(interface);
    QCOMPARE(service.isValid(), false);
    QCOMPARE(database.lastError().errorCode(), DBError::InvalidSearchCriteria);

    QCOMPARE(database.close(), 0);
}

bool ServiceDatabaseUnitTest::compareDescriptor(QServiceInterfaceDescriptor interface,
    QString interfaceName, QString serviceName, int majorVersion, int minorVersion)
{
    interface.d->properties[QServiceInterfaceDescriptor::Capabilities] = QStringList();

    return compareDescriptor(interface, interfaceName, serviceName, majorVersion, minorVersion,
            QStringList());
}

bool ServiceDatabaseUnitTest::compareDescriptor(QServiceInterfaceDescriptor interface,
    QString interfaceName, QString serviceName, int majorVersion, int minorVersion,
    QStringList capabilities, QString filePath, QString serviceDescription,
    QString interfaceDescription)
{

    if(interface.interfaceName() != interfaceName) {
        qWarning() << "Interface name mismatch: expected =" << interfaceName
                    << " actual =" << interface.interfaceName();
        return false;
    }

    if (interface.serviceName() != serviceName) {
        qWarning() << "Service name mismatch: expected =" << serviceName
                    << " actual =" << interface.serviceName();
        return false;
    }

    if (interface.majorVersion() != majorVersion) {
        qWarning() << "Major version mismatch: expected =" << majorVersion
                        << " actual =" << interface.majorVersion();
        return false;
    }

    if (interface.minorVersion() != minorVersion) {
        qWarning() << "Minor version mismatch: expected =" << minorVersion
                    << " actual =" << interface.minorVersion();
        return false;
    }

    if (capabilities.count() != 0 || interface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().count() != 0 ) {
        QStringList securityCapabilities;
        securityCapabilities = interface.property(QServiceInterfaceDescriptor::Capabilities).toStringList();

        if(securityCapabilities.count() != capabilities.count()) {
            qWarning() << "Capabilities count mismatch: expected =" << capabilities.count()
                        << " actual="<< securityCapabilities.count()
                        << "\texpected capabilities =" << capabilities
                        << "actual capabilities =" << securityCapabilities;
            return false;
        }

        for (int i = 0; i < securityCapabilities.count(); ++i) {
            if (securityCapabilities[i] != capabilities[i]) {
                qWarning() << "Capability mismatch: expected =" << capabilities[i]
                            << " actual =" << securityCapabilities[i];
                return false;
            }
        }
    }

    if (!filePath.isEmpty()) {
        if (interface.property(QServiceInterfaceDescriptor::FilePath).toString() != filePath) {
            qWarning() << "File path mismatch: expected =" << filePath
                << " actual =" << interface.property(QServiceInterfaceDescriptor::FilePath).toString();
            return false;
        }
    }
    if (!serviceDescription.isEmpty()) {
        if (interface.property(QServiceInterfaceDescriptor::ServiceDescription).toString() != serviceDescription) {
            qWarning() << "Service Description mismatch: expected =" << serviceDescription
                        << " actual=" << interface.property(QServiceInterfaceDescriptor::ServiceDescription).toString();
            return false;
        }
    }
    if (!interfaceDescription.isEmpty()) {
        if (interface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString() != interfaceDescription) {
            qWarning() << "Interface Description mismatch: expected =" << interfaceDescription
                        << " actual =" << interface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString();
            return false;
        }

    }
    return true;
}

void ServiceDatabaseUnitTest::defaultServiceInterface()
{
    QServiceInterfaceDescriptor interface;
    bool ok;

    //try getting the default service interface implementation when database is not open
    interface = database.defaultServiceInterface("com.cyberdyne.terminator", &ok);
    QCOMPARE(ok, false);
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);
    QVERIFY(!interface.isValid());

    //try getting a valid default, in this case only one implementation exists
    QCOMPARE(database.open(), 0);
    interface = database.defaultServiceInterface("com.omni.device.Lights", &ok);
    QCOMPARE(ok, true);
    QVERIFY(interface.isValid());
    QStringList capabilities;
    QVERIFY(compareDescriptor(interface, "com.omni.device.Lights",
                                "OMNI", 9, 0, capabilities,
                                "C:/OmniInc/omniinc.dll",
                                "Omni mobile",
                                "Interface that provides access to device lights"));

    //try getting a valid default, in this case two services implement the interface
    ok = false;
    interface = database.defaultServiceInterface("com.CyBerDynE.Terminator", &ok);
    QCOMPARE(ok, true);
    QVERIFY(interface.isValid());

    capabilities << "NetworkServices";
    QVERIFY(compareDescriptor(interface, "com.cyberdyne.terminator",
                                    "Cyberdyne", 2,1, capabilities,
                                    "C:/California/connor.dll",
                                    "Cyberdyne Termination Services",
                                    "Remote communications interface for the T-800"));

    //try getting a valid default, in this case multiple services implement the interface
    ok = false;
    interface = database.defaultServiceInterface("com.omni.device.Accelerometer", &ok);
    QCOMPARE(ok, true);
    QVERIFY(interface.isValid());
    capabilities.clear();
    capabilities << "SurroundingsDD";
    QVERIFY(compareDescriptor(interface, "com.omni.device.Accelerometer",
                                    "OMNI", 1, 1, capabilities,
                                    "C:/OmniInc/omniinc.dll",
                                    "Omni mobile",
                                    "Interface that provides accelerometer readings"));

    //try searching for an interface that isn't registered
    interface = database.defaultServiceInterface("com.omni.device.FluxCapacitor", &ok);
    QCOMPARE(ok, true);
    QVERIFY(!interface.isValid());

    //try getting the default interface impl for a service that is made up of multiple
    //plugins
    interface = database.defaultServiceInterface("com.dharma.electro.discharge");
    QVERIFY(interface.isValid());
    capabilities.clear();
    QVERIFY(compareDescriptor(interface, "com.dharma.electro.discharge",
                                "DharmaInitiative", 4, 0,
                                capabilities, "C:/island/swan.dll"));

    //trying getting the default using an empty interface name
    interface = database.defaultServiceInterface("", &ok);
    QVERIFY(ok);
    QVERIFY(!interface.isValid());
    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::setDefaultService_strings()
{
    //try setting a default when the database is not open
    QCOMPARE(database.setDefaultService("Skynet", "com.cyberdyne.terminator"), false);
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);

    QCOMPARE(database.open(),0);

    bool ok = false;
    QServiceInterfaceDescriptor interface;
    QStringList capabilities;
    QString errorText = "No implementation for interface \"%1\" found for service \"%2\"";

    interface = database.defaultServiceInterface("com.CyBerDynE.Terminator");
    QVERIFY(compareDescriptor(interface, "com.cyberdyne.terminator",
                                    "Cyberdyne", 2, 1));

    //try setting a default to another service, check for case insensitive behaviour
    //and try setting same default twice.
    for (int i = 0; i < 2; ++i ) {
    QVERIFY(database.setDefaultService("SkyNET", "COM.cyberdyne.TERMinator"));

    interface = database.defaultServiceInterface("com.CyBerDynE.Terminator", &ok);
    QCOMPARE(ok, true);
    QVERIFY(interface.isValid());
    capabilities.clear();
    capabilities << "NetworkServices";

    QVERIFY(compareDescriptor(interface, "com.cyberdyne.terminator",
                                    "skynet", 3,6, capabilities,
                                    "C:/California/dyson.dll",
                                    "Skynet Termination Services",
                                    "Remote communications interface for the T-800"));
    }

    //try setting the default service back to it's original value
    QVERIFY(database.setDefaultService("Cyberdyne", "com.cyberdyne.terminator"));
    ok = false;
    interface = database.defaultServiceInterface("com.cyberdyne.terminator", &ok);
    QVERIFY(ok);
    QVERIFY(interface.isValid());

    QVERIFY(compareDescriptor(interface, "com.cyberdyne.terminator",
                                "Cyberdyne", 2,1, capabilities,
                                "C:/California/connor.dll",
                                "Cyberdyne Termination Services",
                                "Remote communications interface for the T-800"));

    //try setting the default on a service that is comprised of multiple plugins
    QVERIFY(database.setDefaultService("DharmaInitiative", "com.dharma.electro.discharge"));
    interface = database.defaultServiceInterface("com.dharma.electro.discharge", &ok);
    QVERIFY(ok);
    QVERIFY(interface.isValid());
    capabilities.clear();
    QVERIFY(compareDescriptor(interface, "com.dharma.electro.discharge",
                        "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));

    //try setting the default for a service whose interfaces were not
    //registered in order of version. (ie. the xml specified interface versions
    //in an arbitrary manner  rather than latest versions first).
    interface = database.defaultServiceInterface("com.cybertron.transform");
    QVERIFY(interface.isValid());
    QVERIFY(compareDescriptor(interface, "com.cybertron.transform",
                        "Autobot", 2, 7));
    QVERIFY(database.setDefaultService("Decepticon", "com.cybertron.transform"));
    interface = database.defaultServiceInterface("com.cybertron.transform");
    QVERIFY(compareDescriptor(interface, "com.cybertron.transform",
                        "Decepticon", 5, 3));

    //try setting the a service which does not implement the interface
    QVERIFY(!database.setDefaultService("acme", "com.cyberdyne.terminator"));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    QCOMPARE(database.lastError().text(), errorText.arg("com.cyberdyne.terminator").arg("acme"));
    interface = database.defaultServiceInterface("com.cyberdyne.terminator");
    QVERIFY(interface.isValid());
    QVERIFY(compareDescriptor(interface, "com.cyberdyne.terminator", "Cyberdyne", 2, 1));

    //try setting the default for a non-existent interface.
    QVERIFY(!database.setDefaultService("omni", "com.omni.device.FluxCapacitor"));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    QCOMPARE(database.lastError().text(), errorText.arg("com.omni.device.FluxCapacitor").arg("omni"));

    //trying setting the default with a non-existent service
    QVERIFY(!database.setDefaultService("StarkInd", "com.omni.device.Accelerometer"));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    QCOMPARE(database.lastError().text(),
            errorText.arg("com.omni.device.Accelerometer").arg("StarkInd"));

    //trying setting the default with a non-existent service and non-existent interface
    QVERIFY(!database.setDefaultService("StarkInd", "com.omni.device.FluxCapacitor"));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    QCOMPARE(database.lastError().text(),
            errorText.arg("com.omni.device.FluxCapacitor").arg("StarkInd"));

    //Use an empty service string when setting a default
    QVERIFY(!database.setDefaultService("", "com.cyberdyne.terminator"));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    QCOMPARE(database.lastError().text(), errorText.arg("com.cyberdyne.terminator").arg(""));

    //Use an empty interface string when setting a default
    QVERIFY(!database.setDefaultService("skynet", ""));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    QCOMPARE(database.lastError().text(), errorText.arg("").arg("skynet"));

    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::setDefaultService_descriptor()
{
    QServiceInterfaceDescriptor interface;
    interface.d = new QServiceInterfaceDescriptorPrivate;
    interface.d->serviceName = "cyberdyne";
    interface.d->interfaceName = "com.cyberdyne.terminator";
    interface.d->major = 1;
    interface.d->minor = 6;

    QVERIFY(!database.setDefaultService(interface));
    QCOMPARE(database.lastError().errorCode(), DBError::DatabaseNotOpen);

    QCOMPARE(database.open(),0);

    //try setting the default to a older version provided by the same
    //service
    QServiceInterfaceDescriptor defaultInterface;
    defaultInterface = database.defaultServiceInterface("com.cyberdyne.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "Cyberdyne", 2, 1));

    QVERIFY(database.setDefaultService(interface));
    defaultInterface = database.defaultServiceInterface("com.cyberdyne.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "Cyberdyne", 1, 6));

    //try setting the default to another service
    interface.d->serviceName = "SKYnet"; //check that behaviour is case insensitive
    interface.d->interfaceName = "COM.cyberdyne.terminaTOR";
    interface.d->major = 1;
    interface.d->minor = 5;

    QVERIFY(database.setDefaultService(interface));
    defaultInterface = database.defaultServiceInterface("com.CYBERDYNE.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "skynet", 1, 5));

    //try setting the default of a service that is made up of multiple plugins
    interface.d->serviceName = "DharmaInitiative";
    interface.d->interfaceName = "com.dharma.electro.discharge";
    interface.d->major = 8;
    interface.d->minor = 0;
    QVERIFY(database.setDefaultService(interface));
    QStringList capabilities;
    defaultInterface = database.defaultServiceInterface("com.dharma.electro.discharge");
    QVERIFY(compareDescriptor(defaultInterface, "com.dharma.electro.discharge",
                                "DharmaInitiative", 8, 0, capabilities,
                                "C:/island/pearl.dll"));

    //try setting the default to a implementation verison not supplied
    //by the service
    interface.d->serviceName = "SKYnet"; //check that behaviour is case insensitive
    interface.d->interfaceName = "COM.cyberdyne.terminaTOR";
    interface.d->major = 1;
    interface.d->minor = 9;

    QVERIFY(!database.setDefaultService(interface));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    defaultInterface = database.defaultServiceInterface("com.CYBERDYNE.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "skynet", 1, 5));

    //try setting the default of an interface that doesn't exist
    interface.d->serviceName = "SKYnet"; //check that behaviour is case insensitive
    interface.d->interfaceName = "com.omni.device.FluxCapacitor";
    interface.d->major = 1;
    interface.d->minor = 5;

    QVERIFY(!database.setDefaultService(interface));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    defaultInterface = database.defaultServiceInterface("com.CYBERDYNE.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "skynet", 1, 5));

    //try setting an interface default to a service that does not implement the interface
    interface.d->serviceName = "Primatech";
    interface.d->interfaceName = "com.cyberdyne.terminator";
    interface.d->major = 1;
    interface.d->minor = 4;
    QVERIFY(!database.setDefaultService(interface));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    defaultInterface = database.defaultServiceInterface("com.CYBERDYNE.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "skynet", 1, 5));

    //try setting an interface default to a non-existent service
    interface.d->serviceName = "StarkInd"; //check that behaviour is case insensitive
    interface.d->interfaceName = "COM.cyberdyne.terminaTOR";
    interface.d->major = 1;
    interface.d->minor = 5;
    QVERIFY(!database.setDefaultService(interface));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    defaultInterface = database.defaultServiceInterface("com.CYBERDYNE.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "skynet", 1, 5));

    //try setting the default using an invalid interface
    QServiceInterfaceDescriptor invalidInterface;
    QVERIFY(!invalidInterface.isValid());
    QVERIFY(!database.setDefaultService(invalidInterface));
    QCOMPARE(database.lastError().errorCode(), DBError::NotFound);
    defaultInterface = database.defaultServiceInterface("com.CYBERDYNE.terminator");
    QVERIFY(compareDescriptor(defaultInterface, "com.cyberdyne.terminator",
                                        "skynet", 1, 5));

    QCOMPARE(database.close(), 0);
}

void ServiceDatabaseUnitTest::unregister()
{
    QCOMPARE(database.unregisterService("acme"),(int)ServiceDatabase::SFW_ERROR_DATABASE_NOT_OPEN);
    QCOMPARE(database.open(), 0);

    //try unregister a non-existing service
    QCOMPARE(database.unregisterService("StarkInd"),(int)ServiceDatabase::SFW_ERROR_NO_SERVICE_FOUND);

    QServiceFilter filter;

    // == check that the service to delete is already in the database ==
    //try a search for descriptors by service name 
    filter.setServiceName("omni");
    QList<QServiceInterfaceDescriptor> interfaces;
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 3);

    //try a search by descriptor
    ServiceInfo service;
    QServiceInterfaceDescriptor oldDescriptor = interfaces[0];
    service = database.getService(oldDescriptor);
    QCOMPARE(service.isValid(), true);
    QCOMPARE(service.filePath(), QString("C:/OmniInc/omniinc.dll"));

    //search for service via interface name
    QStringList services;
    services = database.getServiceNames("com.omni.device.Accelerometer");
    QCOMPARE(services.count(), 4);
    QVERIFY(services.contains("OMNI"));

    //search for descriptors via interface name
    filter.setServiceName("");
    filter.setInterface("com.omni.device.Accelerometer");
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 5);
    bool serviceFound = false;
    foreach (QServiceInterfaceDescriptor interface, interfaces) {
        if(interface.serviceName() == "OMNI")
            serviceFound = true;
    }
    QVERIFY(serviceFound);

    QCOMPARE(database.unregisterService("omni"),0);

    //  == check that deleted service and associated interfaces cannot be found ==
    //try a search for descriptors by service name
    QServiceFilter serviceFilter;
    serviceFilter.setServiceName("omni");
    interfaces = database.getInterfaces(serviceFilter);
    QCOMPARE(interfaces.count(), 0);

    //try a search by descriptor
    service = database.getService(oldDescriptor);
    QCOMPARE(service.isValid(), false);
    QCOMPARE(service.filePath(), QString(""));

    //search for service via interface name
    services = database.getServiceNames("com.omni.device.Accelerometer");
    QCOMPARE(services.count(), 3);
    QVERIFY(!services.contains("OMNI"));

    //search for descriptors via interface name
    filter.setServiceName("");
    filter.setInterface("com.omni.device.Accelerometer");
    interfaces = database.getInterfaces(filter);
    QCOMPARE(interfaces.count(), 4);
    serviceFound = false;
    foreach (QServiceInterfaceDescriptor interface, interfaces) {
        if(interface.serviceName() == "OMNI")
            serviceFound = true;
    }
    QVERIFY(!serviceFound);
}

bool ServiceDatabaseUnitTest::compareService(const ServiceInfo &service,
                            const QString &name, const QString &filePath,
                            const QString &description)
{
    if (service.name() != name)
    {
        qWarning() <<"Service name mismatch: expected =" << name << " actual = " << service.name();
        return false;
    }
    if (service.filePath() != filePath) {
        qWarning() <<"File  path mismatch expected: expected =" << filePath << " actual = " << service.filePath();
        return false;
    }
    if (service.description() != description) {
        qWarning() <<"Service name mismatch: expected =" << description << " actual = " << service.description();
        return false;
    }
    return true;
}

void ServiceDatabaseUnitTest::cleanupTestCase()
{
    if(dir.exists(RESOLVERDATABASE)){
        dir.remove(RESOLVERDATABASE);
    }
}
QTEST_MAIN(ServiceDatabaseUnitTest)
#include "tst_servicedatabasetest.moc"
