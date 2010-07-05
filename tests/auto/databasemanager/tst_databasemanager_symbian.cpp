/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QtCore>
#include <QThread>
#define private public
#include <qserviceinterfacedescriptor.h>
#include "qserviceinterfacedescriptor_p.h"
#include <qservicefilter.h>
#include "../qsfwtestutil.h"
#include "servicemetadata_p.h"
#include "databasemanager_symbian_p.h"

QTM_USE_NAMESPACE
class tst_DatabaseManager: public QObject
{
    Q_OBJECT
private slots:
        void initTestCase();
        void multipleInstances();   //Must be executed first
        void registerService();
        void getInterfaces();
        void getServiceNames();
        void defaultService();
        void unregisterService();
        void CWRTXmlCompatability();
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

        void modifyPermissionSet(QFile::Permissions &permsSet,
                                            int perm);
        
private:
        DatabaseManager *m_dbm;
        QDir m_testdir;
};

void tst_DatabaseManager::initTestCase()
{
    // Wait a millisecond so that QServiceManagers are destroyed and release
    // the database file (otherwise QFile::remove will get a permission denied -->
    // in next case, the isEmpty() check fails).
    QTest::qWait(1);
    QSfwTestUtil::removeDatabases_symbian();
    m_dbm = NULL;
}

void tst_DatabaseManager::registerService()
{
    if (!m_dbm) {
        m_dbm = new DatabaseManager;
    }

    m_testdir = QDir(QDir::currentPath() + "/testdata");
    ServiceMetaData parser("");

    QStringList userServiceFiles;
    userServiceFiles << "ServiceAcme.xml" << "ServiceLuthorCorp.xml"
                    << "ServicePrimatech.xml"
                    << "ServiceDharma_Swan.xml"
                    << "ServiceDharma_Pearl.xml"
                    << "ServiceDharma_Flame.xml"
                    << "ServiceDecepticon.xml";

    foreach (const QString &serviceFile, userServiceFiles) {
        qDebug() << m_testdir.absoluteFilePath(serviceFile);
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults parseResults = parser.parseResults();
        QVERIFY(m_dbm->registerService(parseResults, DatabaseManager::UserScope));
    }
    
    QSKIP("There is no difference between user and system scope in symbian", SkipAll);
    
    QStringList systemServiceFiles;
    systemServiceFiles << "ServiceOmni.xml" << "ServiceWayneEnt.xml"
                        << "ServiceDharma_Hydra.xml"
                        << "ServiceDharma_Orchid.xml"
                        << "ServiceAutobot.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults parseResults = parser.parseResults();
        QVERIFY(m_dbm->registerService(parseResults, DatabaseManager::SystemScope));
    }
}

void tst_DatabaseManager::getInterfaces()
{
    if (!m_dbm)
        m_dbm = new DatabaseManager;

    QSKIP("There is no difference between user and system scope in symbian", SkipAll);
    
    QString iface("com.omni.device.accelerometer");
    QServiceFilter filter(iface);
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 5);
    QVERIFY(compareDescriptor(descriptors[0], iface, "LuthorCorp", 1,2));
    QCOMPARE(descriptors[0].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[1], iface, "Primatech", 1, 4));
    QCOMPARE(descriptors[1].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[2], iface, "Primatech", 1, 2));
    QCOMPARE(descriptors[2].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[3], iface, "OMNI", 1, 1));
    QCOMPARE(descriptors[3].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[4], iface, "WayneEnt", 2, 0));
    QCOMPARE(descriptors[4].scope(), QService::SystemScope);

    //check that we can get descriptors for a service spread
    //over the user and system databases
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(),10);

    QStringList capabilities;
    QVERIFY(compareDescriptor(descriptors[0], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/swan.dll"));
    QCOMPARE(descriptors[0].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[1],  "com.dharma.electro.discharge","DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QCOMPARE(descriptors[1].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[2], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QCOMPARE(descriptors[2].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[3], "com.dharma.radio", "DharmaInitiative", 8, 15, capabilities, "C:/island/flame.dll"));
    QCOMPARE(descriptors[3].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[4], "com.dharma.electro.discharge", "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));
    QCOMPARE(descriptors[4].scope(), QService::UserScope);
    QVERIFY(compareDescriptor(descriptors[5], "com.dharma.electro.discharge", "DharmaInitiative", 23, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[5].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[6], "com.dharma.electro.discharge", "DharmaInitiative", 42, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[6].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[7], "com.dharma.cage", "DharmaInitiative", 3, 16, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[7].scope(), QService::SystemScope);
    //note; the following system interface implementation overlaps with an interface implementation already provided in the user database
    QVERIFY(compareDescriptor(descriptors[8], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[8].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[9], "com.dharma.wheel", "DharmaInitiative", 1, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[9].scope(), QService::SystemScope);

    //do a system database only search
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 5);
    QVERIFY(compareDescriptor(descriptors[0], "com.dharma.electro.discharge", "DharmaInitiative", 23, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[0].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[1], "com.dharma.electro.discharge", "DharmaInitiative", 42, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[1].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[2], "com.dharma.cage", "DharmaInitiative", 3, 16, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[2].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[3], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[3].scope(), QService::SystemScope);
    QVERIFY(compareDescriptor(descriptors[4], "com.dharma.wheel", "DharmaInitiative", 1, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[4].scope(), QService::SystemScope);

    //search for a non-existent interface
    filter.setServiceName("");
    filter.setInterface("com.omni.device.fluxcapacitor");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 0);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
}

void tst_DatabaseManager::getServiceNames()
{
    if (!m_dbm)
        m_dbm = new DatabaseManager;

    QSKIP("There is no difference between user and system scope in symbian", SkipAll);
    
    //try getting a lost of service names only in user database
    QStringList serviceNames;
    serviceNames = m_dbm->getServiceNames("", DatabaseManager::UserOnlyScope);
    QStringList expectedNames;
    expectedNames << "acme" << "LuthorCorp" << "Primatech" << "DharmaInitiative" << "Decepticon";
    QCOMPARE(serviceNames.count(), expectedNames.count());
    foreach(const QString &expectedName, expectedNames)
        QVERIFY(serviceNames.contains(expectedName, Qt::CaseInsensitive));

    //try getting a list of service names in the system database
    serviceNames = m_dbm->getServiceNames("", DatabaseManager::SystemScope);
    expectedNames.clear();
    expectedNames << "OMNI" << "WayneEnt" << "DharmaInitiative" << "Autobot";
    QCOMPARE(serviceNames.count(), expectedNames.count());
    foreach(const QString &expectedName, expectedNames)
        QVERIFY(serviceNames.contains(expectedName, Qt::CaseInsensitive));

    //try getting a list of service names from both databases
    //and ensure there are no duplicates
    serviceNames = m_dbm->getServiceNames("", DatabaseManager::UserScope);
    expectedNames.clear();
    expectedNames << "acme" << "LuthorCorp" << "Primatech" << "omni" << "WayneEnt"
                    << "DharmaInitiative" << "Autobot" << "Decepticon";
    QCOMPARE(serviceNames.count(), expectedNames.count());
    foreach(const QString &expectedName, expectedNames)
        QVERIFY(serviceNames.contains(expectedName, Qt::CaseInsensitive));

    //try getting names of services that implement particular interface
    serviceNames = m_dbm->getServiceNames("com.omni.device.accelerometer", DatabaseManager::UserScope);
    expectedNames.clear();
    expectedNames << "LuthorCorp" << "Primatech" << "omni" << "WayneEnt";
    QCOMPARE(serviceNames.count(), expectedNames.count());
    foreach(const QString &expectedName, expectedNames)
        QVERIFY(serviceNames.contains(expectedName, Qt::CaseInsensitive));
}

void tst_DatabaseManager::defaultService()
{
    if (!m_dbm)
        m_dbm = new DatabaseManager;

    QSKIP("There is no difference between user and system scope in symbian", SkipAll);
    
    QServiceInterfaceDescriptor descriptor;

    //get a user default service at user scope
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                    DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.omni.device.accelerometer", "LuthorCorp", 1, 2));
    QVERIFY(descriptor.scope()== QService::UserScope);

    //get a sytem default from user scope
    descriptor = m_dbm->interfaceDefault("com.Dharma.wheel", DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.dharma.wheel", "DharmaInitiative", 1,0));
    QVERIFY(descriptor.scope() == QService::SystemScope);

    //get a system default service at system scope
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                     DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "omni", 1, 1));

    //set a user default from a user interface implementation
    QList<QServiceInterfaceDescriptor> descriptors;
    QServiceFilter filter;
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.accelerometer", "1.4");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);

    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                    DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.omni.device.accelerometer", "Primatech", 1, 4));
    QVERIFY(descriptor.scope() == QService::UserScope);

    //set a system default from a  system interface implementation
    filter.setServiceName("WayneEnt");
    filter.setInterface("com.omni.device.accelerometer","2.0");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 1);

    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::SystemScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                    DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "WayneEnt", 2,0));
    QVERIFY(descriptor.scope() == QService::SystemScope);

    //set a user default with a system interface descriptor
    filter.setServiceName("omni");
    filter.setInterface("com.omni.device.accelerometer","1.1");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(descriptor.isValid());
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "omni", 1, 1));
    QVERIFY(descriptor.scope() == QService::SystemScope);

    //set a system default using a user interface descriptor
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.accelerometer","1.4");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);

    QVERIFY(!m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::InvalidDescriptorScope);

    // == try setting defaults using setInterfaceDefault(serviceName, interfaceName, ...)
    //set a local default in the user scope database
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "omni", 1, 1));
    QVERIFY(m_dbm->setInterfaceDefault("LuthorCorp", "com.omni.device.accelerometer",
                                        DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "LuthorCorp", 1, 2));

    //set a system default in the user scope database
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.disCHARGE",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                            "DharmaInitiative", 4, 0));
    QVERIFY(descriptor.scope() == QService::UserScope);
    m_dbm->setInterfaceDefault("DharmaInitiative", "com.dharma.electro.discharge",
                                        DatabaseManager::UserScope);

    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                                DatabaseManager::UserScope);
    QVERIFY(m_dbm->lastError().code() == DBError::NoError);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                "DharmaInitiative", 42, 0));

    //set a user default in the user scope database but where there
    //exist interface implementations in both user and system databases
    //also check that the system scope default has not changed
    filter.setServiceName("Autobot");
    filter.setInterface("com.cybertron.transform", "2.5", QServiceFilter::ExactVersionMatch );
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope);
    descriptor = m_dbm->interfaceDefault("com.cybertron.transform",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.cybertron.transform",
                                "Autobot", 2, 5));
    descriptor = m_dbm->interfaceDefault("com.cybertron.transform",
                                        DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.cybertron.transform",
                                "Autobot", 2, 5));

    m_dbm->setInterfaceDefault("Decepticon", "com.cybertron.transform", DatabaseManager::UserScope);
    descriptor = m_dbm->interfaceDefault("com.cybertron.transform",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.cybertron.transform",
                                "Decepticon", 5,3));
    descriptor = m_dbm->interfaceDefault("com.cybertron.transform",
                                        DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.cybertron.transform",
                                "Autobot", 2, 5));

    //Look up a default service for a non-existent interface
    descriptor = m_dbm->interfaceDefault("com.omni.device.fluxcapacitor", DatabaseManager::UserScope);
    QVERIFY(!descriptor.isValid());
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);
}

void tst_DatabaseManager::unregisterService()
{
    if (!m_dbm)
        m_dbm = new DatabaseManager;

    QSKIP("There is no difference between user and system scope in symbian", SkipAll);

    //try remove a service that only exists in the user database
    QServiceFilter filter;
    filter.setServiceName("acme");
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count() > 0);
    m_dbm->unregisterService("acme", DatabaseManager::UserScope);
    QCOMPARE(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count(), 0);

    //try remove  a service that only exists in the system database
    filter.setServiceName("omni");
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count() > 0);
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count() > 0);
    m_dbm->unregisterService("omni", DatabaseManager::SystemScope);
    QCOMPARE(m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count(), 0);
    QCOMPARE(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count(), 0);

    //try remove a service from the user database that also exists in the system database
    filter.setServiceName("dharmainitiative");
    int systemCount = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count();
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count() > 0);
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count() > 0);
    m_dbm->unregisterService("dharmainitiative", DatabaseManager::UserScope);
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count() > 0);
    QCOMPARE(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count(), systemCount);

    //re-add the service to the user database for the next test
    ServiceMetaData parser("");
    parser.setDevice(new QFile(m_testdir.absoluteFilePath("ServiceDharma_Swan.xml")));
    QVERIFY(parser.extractMetadata());
    ServiceMetaDataResults parseResults = parser.parseResults();
    QVERIFY(m_dbm->registerService(parseResults, DatabaseManager::UserScope));

    //try to remove a service from the system database that also exist in the user database
    filter.setServiceName("dharmainitiative");
    int userCount = m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count()
                    - m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count();
    QVERIFY(userCount > 0);
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count() > 0);
    QVERIFY(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count() > 0);
    m_dbm->unregisterService("dharmainitiative", DatabaseManager::SystemScope);
    QCOMPARE(m_dbm->getInterfaces(filter, DatabaseManager::SystemScope).count(), 0);
    QCOMPARE(m_dbm->getInterfaces(filter, DatabaseManager::UserScope).count(), userCount);

    //try and drop a table in the database by SQL injection
    QVERIFY(!m_dbm->unregisterService("; drop table Interface;", DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);

    QSfwTestUtil::removeDatabases_symbian();
}

bool tst_DatabaseManager::compareDescriptor(QServiceInterfaceDescriptor interface,
        QString interfaceName, QString serviceName, int majorVersion, int minorVersion)
{

    if (interface.d == NULL )
        return false;
    interface.d->attributes[QServiceInterfaceDescriptor::Capabilities] = QStringList();

    return compareDescriptor(interface, interfaceName, serviceName, majorVersion, minorVersion,
            QStringList());
}

bool tst_DatabaseManager::compareDescriptor(QServiceInterfaceDescriptor interface,
    QString interfaceName, QString serviceName, int majorVersion, int minorVersion,
    QStringList capabilities, QString filePath, QString serviceDescription,
    QString interfaceDescription)
{

    if(interface.interfaceName().compare(interfaceName, Qt::CaseInsensitive) !=0) {
        qWarning() << "Interface name mismatch: expected =" << interfaceName
                    << " actual =" << interface.interfaceName();
        return false;
    }

    if (interface.serviceName().compare(serviceName, Qt::CaseInsensitive) != 0) {
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

    if (capabilities.count() != 0 || interface.attribute(QServiceInterfaceDescriptor::Capabilities).toStringList().count() != 0 ) {
        QStringList securityCapabilities;
        securityCapabilities = interface.attribute(QServiceInterfaceDescriptor::Capabilities).toStringList();

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
        if (interface.attribute(QServiceInterfaceDescriptor::Location).toString() != filePath) {
            qWarning() << "File path mismatch: expected =" << filePath
                << " actual =" << interface.attribute(QServiceInterfaceDescriptor::Location).toString();
            return false;
        }
    }
    if (!serviceDescription.isEmpty()) {
        if (interface.attribute(QServiceInterfaceDescriptor::ServiceDescription).toString() != serviceDescription) {
            qWarning() << "Service Description mismatch: expected =" << serviceDescription
                        << " actual=" << interface.attribute(QServiceInterfaceDescriptor::ServiceDescription).toString();
            return false;
        }
    }
    if (!interfaceDescription.isEmpty()) {
        if (interface.attribute(QServiceInterfaceDescriptor::InterfaceDescription).toString() != interfaceDescription) {
            qWarning() << "Interface Description mismatch: expected =" << interfaceDescription
                        << " actual =" << interface.attribute(QServiceInterfaceDescriptor::InterfaceDescription).toString();
            return false;
        }

    }
    return true;
}

void tst_DatabaseManager::CWRTXmlCompatability()
{
    if (m_dbm != 0 ) {
        delete m_dbm;
    }
    m_dbm = 0;
    m_dbm = new DatabaseManager;
    ServiceMetaData parser("");

    QStringList userServiceFiles;
    userServiceFiles << "ServiceTest.xml" << "ServiceTest1.xml";
    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults parseResults = parser.parseResults();
        QVERIFY(m_dbm->registerService(parseResults, DatabaseManager::UserScope));
    }

    QString test("Test");
    for(int i = 0; i <= 10; ++i) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(test + QString::number(i) + QLatin1String(".xml"))));
        if (i == 6)
            QVERIFY(parser.extractMetadata());
        else
            QVERIFY(!parser.extractMetadata());
    }

    QStringList systemServiceFiles;
    systemServiceFiles << "ServiceTest2.xml" << "ServiceTest3.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        if (serviceFile == "ServiceTest3.xml") {
            QVERIFY(!parser.extractMetadata());//versions less than 1.0 are not allowed
            continue;
        }
        else
            QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults parseResults = parser.parseResults();
        QVERIFY(m_dbm->registerService(parseResults, DatabaseManager::SystemScope));
    }

    QServiceFilter filter;
    filter.setInterface("com.nokia.ILocation");
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.nokia.ILocation", "TestService", 1,0));
    QVERIFY(compareDescriptor(descriptors[1], "com.nokia.ILocation", "TestService1", 1,1));
    QVERIFY(compareDescriptor(descriptors[2], "com.nokia.ILocation", "TestService2", 1,2));

}

class DatabaseManagerThread : QThread
{
public:
    DatabaseManagerThread() : m_dbm(NULL)
    {
        start();
    }
    
    ~DatabaseManagerThread()
    {
        wait();
        delete m_dbm;
    }

    void run()
    {
        m_dbm = new DatabaseManager;
        QEventLoop loop;
        QTimer::singleShot(100, &loop, SLOT(quit()));
        loop.exec();
    }

private:
    DatabaseManager *m_dbm;
};

void tst_DatabaseManager::multipleInstances()
{
    QVERIFY(!m_dbm);
    DatabaseManagerThread *threads[5];
    for (int i = 0; i < 5; ++i) {
        threads[i] = new DatabaseManagerThread;
    }
    
    for (int i = 0; i < 5; ++i) {
        delete threads[i];
    }
}

void tst_DatabaseManager::modifyPermissionSet(QFile::Permissions &permsSet,
                                                    int perm)
{

    switch(perm) {
        case(QFile::ReadOwner):
            permsSet |= QFile::ReadOwner;
            permsSet |= QFile::ReadUser;
            permsSet |= QFile::ReadGroup;
            permsSet |= QFile::ReadOther;
            break;
        case(~QFile::ReadOwner):
            permsSet &= ~QFile::ReadOwner;
            permsSet &= ~QFile::ReadUser;
            permsSet &= ~QFile::ReadGroup;
            permsSet &= ~QFile::ReadOther;
            break;
        case(QFile::WriteOwner):
            permsSet |= QFile::WriteOwner;
            permsSet |= QFile::WriteUser;
            permsSet |= QFile::WriteGroup;
            permsSet |= QFile::WriteOther;
            break;
        case(~QFile::WriteOwner):
            permsSet &= ~QFile::WriteOwner;
            permsSet &= ~QFile::WriteUser;
            permsSet &= ~QFile::WriteGroup;
            permsSet &= ~QFile::WriteOther;
            break;
        case(QFile::ExeOwner):
            permsSet |= QFile::ExeOwner;
            permsSet |= QFile::ExeUser;
            permsSet |= QFile::ExeGroup;
            permsSet |= QFile::ExeOther;
            break;
        case(~QFile::ExeOwner):
            permsSet &= ~QFile::ExeOwner;
            permsSet &= ~QFile::ExeUser;
            permsSet &= ~QFile::ExeGroup;
            permsSet &= ~QFile::ExeOther;
            break;
        default:
            break;
    }
}

void tst_DatabaseManager::cleanupTestCase()
{
    QTest::qWait(100);
    delete m_dbm;
    QSfwTestUtil::removeDatabases_symbian();
}

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager_symbian.moc"

