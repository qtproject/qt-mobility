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
#include <QtCore>
#define private public
#include <qserviceinterfacedescriptor.h>
#include <qserviceinterfacedescriptor_p.h>
#include "../qsfwtestutil.h"
#include <databasemanager_p.h>

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

QTM_USE_NAMESPACE

class tst_DatabaseManager: public QObject
{
    Q_OBJECT
private slots:
        void initTestCase();
        void registerService();
        void getInterfaces();
        void getServiceNames();
        void defaultService();
        void unregisterService();
#ifdef Q_OS_UNIX        
	void permissions();
        void onlyUserDbAvailable();
        void defaultServiceCornerCases();
#endif
        void nonWritableSystemDb();
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

        void clean();
        void modifyPermissionSet(QFile::Permissions &permsSet,
                                            int perm);

        DatabaseManager *m_dbm;
        QDir m_testdir;
};

void tst_DatabaseManager::initTestCase()
{
    QSfwTestUtil::setupTempUserDb();
    QSfwTestUtil::setupTempSystemDb();
    QSfwTestUtil::removeDirectory(QSfwTestUtil::tempSettingsPath());
    m_dbm = new DatabaseManager;
}

void tst_DatabaseManager::registerService()
{
    m_testdir = QDir(TESTDATA_DIR "/testdata");
    ServiceMetaData parser("");

    QStringList userServiceFiles;
    userServiceFiles << "ServiceAcme.xml" << "ServiceLuthorCorp.xml"
                    << "ServicePrimatech.xml"
                    << "ServiceDharma_Swan.xml"
                    << "ServiceDharma_Pearl.xml"
                    << "ServiceDharma_Flame.xml"
                    << "ServiceDecepticon.xml";

    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    }

    QStringList systemServiceFiles;
    systemServiceFiles << "ServiceOmni.xml" << "ServiceWayneEnt.xml"
                        << "ServiceDharma_Hydra.xml"
                        << "ServiceDharma_Orchid.xml"
                        << "ServiceAutobot.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::SystemScope));
    }
}

void tst_DatabaseManager::getInterfaces()
{
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
    QServiceInterfaceDescriptor descriptor;

    //get a user default service at user scope
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                    DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.omni.device.accelerometer", "LuthorCorp", 1, 2));
    QVERIFY(descriptor.scope()==QService::UserScope);

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
    QVERIFY(descriptor.scope() == QService::SystemScope );

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
    QVERIFY(descriptor.scope() == QService::SystemScope );

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
    ServiceMetaDataResults results = parser.parseResults();
    QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));

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

    clean();
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
#ifdef Q_OS_UNIX
void tst_DatabaseManager::permissions()
{
    ServiceMetaDataResults results;
    //try create a user scope database with no permissions
    //to create the directory needed for the user db
    QString userDir =  QSfwTestUtil::userDirectory();
    QString systemDir = QSfwTestUtil::systemDirectory();

    QDir::root().mkpath(userDir);
    QDir::root().mkpath(systemDir);

    QFile::Permissions userPermsSet = QFile::permissions(userDir);
    modifyPermissionSet(userPermsSet, ~QFile::ExeOwner);
    QVERIFY(QFile::setPermissions(userDir, userPermsSet));

    m_testdir = QDir(TESTDATA_DIR "/testdata");
    ServiceMetaData parser("");
    parser.setDevice(new QFile(m_testdir.absoluteFilePath("ServiceAcme.xml")));
    QVERIFY(parser.extractMetadata());
    m_dbm = new DatabaseManager;
    results = parser.parseResults();
    QVERIFY(!m_dbm->registerService(results, DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);

    //try to create a system scope database with no permission to
    //create the directory needed for the system db
    QFile::Permissions systemPermsSet = QFile::permissions(systemDir);
    modifyPermissionSet(systemPermsSet, ~QFile::ExeOwner);
    QVERIFY(QFile::setPermissions(systemDir, systemPermsSet));

    QVERIFY(!m_dbm->registerService(results, DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);

    //restore directory permissions
    modifyPermissionSet(userPermsSet, QFile::ExeOwner);
    QVERIFY(QFile::setPermissions(userDir, userPermsSet));
    modifyPermissionSet(systemPermsSet, QFile::ExeOwner);
    QVERIFY(QFile::setPermissions(systemDir, systemPermsSet));

    //try to create a user scope database without sufficient permissions
    //for creation of the database file
    QDir::root().mkpath(userDir + "/Nokia/");
    userPermsSet = QFile::permissions(userDir + "/Nokia/");
    modifyPermissionSet(userPermsSet, ~QFile::WriteOwner);
    QVERIFY(QFile::setPermissions(userDir + "/Nokia/", userPermsSet));
    QVERIFY(!m_dbm->registerService(results, DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);

    //restore user directory permissions and create and populate a user database
    modifyPermissionSet(userPermsSet, QFile::WriteOwner);
    QVERIFY(QFile::setPermissions(userDir + "/Nokia", userPermsSet));
    QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    QString userDbFilePath = m_dbm->m_userDb->databasePath();

    //try to access database without read permissions
    userPermsSet = QFile::permissions(userDbFilePath);
    modifyPermissionSet(userPermsSet, ~QFile::ReadOwner);

    QVERIFY(QFile::setPermissions(userDbFilePath, userPermsSet));
    delete m_dbm;
    QServiceFilter filter;
    m_dbm = new DatabaseManager;
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors= m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QVERIFY(!m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);

    //restore permissions
    modifyPermissionSet(userPermsSet, QFile::ReadOwner);
    QVERIFY(QFile::setPermissions(userDbFilePath, userPermsSet));
    
    //try to access an artificially corrupted user database
    QFile userDbFile(userDbFilePath);
    QFile acmeFile(m_testdir.absoluteFilePath("ServiceAcme.xml"));
    QVERIFY(acmeFile.open(QIODevice::ReadWrite));
    QVERIFY(userDbFile.open(QIODevice::ReadWrite));
    userDbFile.resize(50);
    userDbFile.seek(100);
    QVERIFY(userDbFile.write(acmeFile.readAll()) != -1);
    userDbFile.close();
    acmeFile.close();
    descriptors= m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QVERIFY(!m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());
    QCOMPARE(m_dbm->lastError().code(), DBError::InvalidDatabaseFile);

    //recreate a valid user database
    userDbFile.remove();
    QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    filter.setServiceName("");
    filter.setInterface("");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 5);

    //try to register a service in the user db without write permissions
    delete m_dbm;
    userPermsSet = QFile::permissions(userDbFilePath);
    modifyPermissionSet(userPermsSet, ~QFile::WriteOwner);
    QVERIFY(QFile::setPermissions(userDbFilePath, userPermsSet));
    m_dbm = new DatabaseManager;
    parser.setDevice(new QFile(m_testdir.absoluteFilePath("ServiceLuthorCorp.xml")));
    QVERIFY(parser.extractMetadata());
    results = parser.parseResults();
    QVERIFY(!m_dbm->registerService(results, DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoWritePermissions);
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 5);

    modifyPermissionSet(userPermsSet, QFile::WriteOwner);
    QVERIFY(QFile::setPermissions(userDbFilePath, userPermsSet));
    clean();
}

void tst_DatabaseManager::onlyUserDbAvailable()
{
    QString userNokiaDir =  QSfwTestUtil::userDirectory() + "/Nokia/";
    QString systemNokiaDir = QSfwTestUtil::systemDirectory() + "/Nokia/";

    QDir::root().mkpath(userNokiaDir);
    QDir::root().mkpath(systemNokiaDir);
    QFile::Permissions systemPermsSet = QFile::permissions(systemNokiaDir);
    modifyPermissionSet(systemPermsSet, ~QFile::WriteOwner);
    QFile::setPermissions(systemNokiaDir, systemPermsSet);

    m_dbm = new DatabaseManager;

    //check that we can register services with the user database
    ServiceMetaData parser("");

    QStringList userServiceFiles;
    userServiceFiles << "ServiceAcme.xml" << "ServiceLuthorCorp.xml"
                    << "ServicePrimatech.xml";
    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
        QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    }

    QVERIFY(m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());

    parser.setDevice(new QFile(m_testdir.absoluteFilePath("ServiceOmni.xml")));
    ServiceMetaDataResults results = parser.parseResults();
    QVERIFY(!m_dbm->registerService(results, DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);
    
    QServiceFilter filter;
    filter.setServiceName("");
    filter.setInterface("");
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 8);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);

    //Use getInterfaces where the database has not already been opened
    //( via a previous call to register service)
    delete m_dbm;
    m_dbm = new DatabaseManager;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 8);
    QVERIFY(m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);

    //Use getServiceNames where the database has not already been opened
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QStringList serviceNames;
    serviceNames = m_dbm->getServiceNames("", DatabaseManager::UserScope);
    QCOMPARE(serviceNames.count(), 3);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);

    //Use defaultService where database has not already been opened
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QServiceInterfaceDescriptor descriptor;
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
            DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                            "LuthorCorp", 1, 2));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QVERIFY(m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());

    //Use setInterfaceDefault(servicename, interfacename, scope)
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QVERIFY(m_dbm->setInterfaceDefault("Primatech", "com.omni.device.accelerometer",
                        DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                "Primatech", 1, 4));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);

    QVERIFY(!m_dbm->setInterfaceDefault("Primatech", "com.omni.device.accelerometer",
                                DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);

    //Use setInterfaceDefault(descriptor, scope)
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.accelerometer", "1.2",
                                        QServiceFilter::ExactVersionMatch);
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(compareDescriptor(descriptors[0], "com.omni.device.accelerometer",
                                "Primatech", 1,2));
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0],
                                            DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                            DatabaseManager::UserScope);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                "Primatech", 1,2));
    QVERIFY(!m_dbm->setInterfaceDefault(descriptor, DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::InvalidDescriptorScope);

    //Use unregisterService()
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QVERIFY(m_dbm->unregisterService("primatech", DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    filter.setServiceName("Primatech");
    filter.setInterface("");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QCOMPARE(descriptors.count(),0);

    QVERIFY(!m_dbm->unregisterService("primatech", DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::CannotOpenServiceDb);

    //restore permissions so we can clean up the test directories
    modifyPermissionSet(systemPermsSet, QFile::WriteOwner);
    QFile::setPermissions(systemNokiaDir, systemPermsSet);
    clean();
}

void tst_DatabaseManager::defaultServiceCornerCases()
{
    m_dbm = new DatabaseManager;
    ServiceMetaData parser("");

    QStringList userServiceFiles;
    userServiceFiles << "ServiceAcme.xml" << "ServiceLuthorCorp.xml"
                    << "ServicePrimatech.xml"
                    << "ServiceDharma_Swan.xml"
                    << "ServiceDharma_Pearl.xml"
                    << "ServiceDharma_Flame.xml"
                    << "ServiceDecepticon.xml";

    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    }

    QStringList systemServiceFiles;
    systemServiceFiles << "ServiceOmni.xml" << "ServiceWayneEnt.xml"
                        << "ServiceDharma_Hydra.xml"
                        << "ServiceDharma_Orchid.xml"
                        << "ServiceAutobot.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::SystemScope));
    }

    // == set a system service interface implementation as a default at user scope then
    //    delete the system service and observe whether a new default
    //    has been chosen ==
    m_dbm->setInterfaceDefault("DharmaInitiative", "com.dharma.electro.discharge",
                                DatabaseManager::UserScope);
    QServiceInterfaceDescriptor descriptor =
                m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                "DharmaInitiative", 42, 0));
    QVERIFY(descriptor.scope() == QService::SystemScope);
    QVERIFY(m_dbm->unregisterService("DharmaInitiative", DatabaseManager::SystemScope));
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                "DharmaInitiative", 16, 0));
    //  check again because there was a bug where the default service
    //  was correctly returned the first time but not the second
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                "DharmaInitiative", 16, 0));

    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);

    // == set a system service interface as a default at user scope,
    //    remove availability to system db and confirm default cannot be found
    //    for that interface.  Then restore the system db and confirm
    //    that a default can indeed be found. ==
    QVERIFY(m_dbm->setInterfaceDefault("WayneEnt", "com.omni.device.accelerometer",
                                        DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                    "WayneEnt", 2, 0));
    QString systemDbFilePath = m_dbm->m_systemDb->databasePath();
    QFile::Permissions systemPermsSet = QFile::permissions(systemDbFilePath);
    modifyPermissionSet(systemPermsSet, ~QFile::ReadOwner);
    QFile::setPermissions(systemDbFilePath, systemPermsSet);

    delete m_dbm;
    m_dbm = new DatabaseManager;
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());
    QVERIFY(!descriptor.isValid());
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);

    modifyPermissionSet(systemPermsSet, QFile::ReadOwner);
    QFile::setPermissions(systemDbFilePath, systemPermsSet);
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(m_dbm->m_systemDb->isOpen());
    QVERIFY(descriptor.isValid());
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                    "WayneEnt", 2, 0));

    //== set a system service interface as a user scope default,
    //   delete the service from the system scope database,
    //   ensure that when the default interface is queried
    //   it no longer exists ==
    m_dbm->setInterfaceDefault("omni", "com.omni.device.lights", //(will set an entry in the
                                DatabaseManager::UserScope);   //defaults table of the user db database)
    descriptor = m_dbm->interfaceDefault("com.omni.device.lights",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.lights",
                                "omni", 9, 0));
    QVERIFY(m_dbm->unregisterService("omni", DatabaseManager::SystemScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.lights",
                                            DatabaseManager::UserScope);
    QVERIFY(!descriptor.isValid());
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);
    clean();
    //Reinitialise the database
    m_dbm = new DatabaseManager;

    userServiceFiles.clear();
    userServiceFiles << "ServiceAcme.xml" << "ServiceLuthorCorp.xml"
                    << "ServicePrimatech.xml"
                    << "ServiceDharma_Swan.xml"
                    << "ServiceDecepticon.xml";

    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    }
    systemServiceFiles.clear();
    systemServiceFiles << "ServiceOmni.xml" << "ServiceWayneEnt.xml"
                        << "ServiceDharma_Hydra.xml"
                        << "ServiceDharma_Orchid.xml"
                        << "ServiceAutobot.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::SystemScope));
    }

    //== Set a couple of external defaults in the user scope database,
    //   -remove the service that provides these defaults at the system scope.
    //    this will leave "hanging" default links in the user scope database
    //   -also set one default which will not be a hanging link
    //   -recreate the database manager and run getServiceNames so it will
    //    connect to both system and user scope databases.
    //   -verify that the hanging defaults links have been appropriately removed
    //    or reset to a new default interface implementation
    //   -also verify that the non-hanging link still exists ==
    QServiceFilter filter;
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("com.dharma.electro.discharge");
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QVERIFY(descriptors.count() > 0);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));

    filter.setServiceName("DharmaInitiative");
    filter.setInterface("com.dharma.wheel");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QVERIFY(descriptors.count() > 0);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));

    filter.setServiceName("WayneEnt");
    filter.setInterface("com.omni.device.accelerometer");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));

    QVERIFY(m_dbm->unregisterService("DharmaInitiative", DatabaseManager::SystemScope));
    QList<QPair<QString, QString> > externalDefaultsInfo = m_dbm->m_userDb->externalDefaultsInfo();
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QCOMPARE(externalDefaultsInfo.length(), 3);

    delete m_dbm;

    m_dbm = new DatabaseManager;
    m_dbm->getServiceNames("", DatabaseManager::UserScope);
    QCOMPARE(m_dbm->m_userDb->externalDefaultsInfo().count(), 1);
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.dharma.electro.discharge",
                            "DharmaInitiative", 4,0));
    descriptor = m_dbm->interfaceDefault("com.dharma.wheel",
                                        DatabaseManager::UserScope);
    QVERIFY(!descriptor.isValid());
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);

    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                            DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                            "WayneEnt", 2,0));

    // == set a system service interface as a default at user scope
    //        -remove the availability of the system db and confirm the default
    //         cannot be found
    //        -set a new default
    //        -restore availability of the system db and confirm the default
    //         is still the new default
    filter.setServiceName("omni");
    filter.setInterface("com.omni.device.accelerometer");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));
    delete m_dbm;

    systemPermsSet = QFile::permissions(systemDbFilePath);
    modifyPermissionSet(systemPermsSet, ~QFile::ReadOwner);
    QFile::setPermissions(systemDbFilePath, systemPermsSet);

    m_dbm = new DatabaseManager;
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(!descriptor.isValid());
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);

    filter.setServiceName("LuthorCorp");
    filter.setInterface("com.omni.device.accelerometer");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(m_dbm->m_userDb->isOpen());
    QVERIFY(!m_dbm->m_systemDb->isOpen());
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                        DatabaseManager::UserScope);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                "LuthorCorp", 1,2));
    modifyPermissionSet(systemPermsSet, QFile::ReadOwner);
    QFile::setPermissions(systemDbFilePath, systemPermsSet);
    m_dbm->getServiceNames("", DatabaseManager::UserScope);//this call is just to open the system db
    QVERIFY(m_dbm->m_systemDb->isOpen());

    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                            DatabaseManager::UserScope);
    QVERIFY(descriptor.isValid());
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                "LuthorCorp", 1,2));

    // == Try set a descriptor as a default but the descriptor is invalid
    //    because the service it belongs to has been removed ==
    descriptor = m_dbm->interfaceDefault("com.cybertron.transform",
                                         DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.cybertron.transform",
                                "Autobot", 2, 7));
    QVERIFY(m_dbm->unregisterService("Autobot",
                            DatabaseManager::SystemScope));
    QVERIFY(!m_dbm->setInterfaceDefault(descriptor, DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);

    QVERIFY(!m_dbm->setInterfaceDefault(descriptor, DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NotFound);

    clean();
}
#endif

void tst_DatabaseManager::nonWritableSystemDb()
{
    m_dbm = new DatabaseManager;
    ServiceMetaData parser("");

    // Setup Initial user and system databases
    QStringList userServiceFiles;
    userServiceFiles << "ServiceAcme.xml" << "ServiceLuthorCorp.xml"
                        << "ServicePrimatech.xml";
    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    }

    QStringList systemServiceFiles;
    systemServiceFiles << "ServiceOmni.xml" << "ServiceWayneEnt.xml"
                        << "ServiceDharma_Orchid.xml"
                        << "ServiceDharma_Hydra.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::SystemScope));
    }

    //make system database non-writable
    QString systemDbFilePath = m_dbm->m_systemDb->databasePath();
    QFile::Permissions systemPermsSet = QFile::permissions(systemDbFilePath);
    modifyPermissionSet(systemPermsSet, ~QFile::WriteOwner);
    QFile::setPermissions(systemDbFilePath, systemPermsSet);

    //== test registration() ==
    delete m_dbm;
    m_dbm = new DatabaseManager;
    parser.setDevice(new QFile(m_testdir.absoluteFilePath("ServiceDharma_Swan.xml")));
    QVERIFY(parser.extractMetadata());
    ServiceMetaDataResults results = parser.parseResults();
    QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);

    parser.setDevice(new QFile(m_testdir.absoluteFilePath("ServiceAutobot.xml")));
    QVERIFY(parser.extractMetadata());
    results = parser.parseResults();
    QVERIFY(!m_dbm->registerService(results, DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoWritePermissions);

    //== test getInterfaces() ==
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QList<QServiceInterfaceDescriptor> descriptors;
    QServiceFilter filter;
    filter.setServiceName("");
    filter.setInterface("");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 18);

    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 9);

    //  try a more restrictive filter with getInterfaces
    filter.setServiceName("");
    filter.setInterface("com.dharma.electro.discharge", "4.0");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 4);

    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 3);

    // ==test getServiceName() ==
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QStringList expectedNames;
    expectedNames << "LuthorCorp" << "Primatech" << "WayneEnt" << "omni";
    QStringList serviceNames = m_dbm->getServiceNames("com.omni.device.accelerometer",
                                            DatabaseManager::UserScope);
    foreach(const QString expectedName, expectedNames)
        QVERIFY(serviceNames.contains(expectedName, Qt::CaseInsensitive));
    QCOMPARE(serviceNames.count(), expectedNames.count());

    expectedNames.clear();
    expectedNames << "WayneEnt" << "omni";
    serviceNames = m_dbm->getServiceNames("com.omni.device.accelerometer",
                                            DatabaseManager::SystemScope);
    foreach(const QString expectedName, expectedNames)
        QVERIFY(serviceNames.contains(expectedName, Qt::CaseInsensitive));
    QCOMPARE(serviceNames.count(), expectedNames.count());

    //== test interfaceDefault() ==
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QServiceInterfaceDescriptor descriptor;
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                            DatabaseManager::UserScope);
    QStringList capabilities;
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                            "DharmaInitiative", 4, 0, capabilities, "C:/island/swan.dll"));

    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                            DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                            "DharmaInitiative", 4, 0, capabilities, "C:/island/orchid.dll"));
    //test setInterfaceDefault(service, interface, scope)
    delete m_dbm;
    m_dbm = new DatabaseManager;
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                "LuthorCorp", 1,2));
    QVERIFY(m_dbm->setInterfaceDefault("Primatech", "com.omni.device.accelerometer",
                                DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);

    //  try setting descriptor that originates from the system db
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                            DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                    "DharmaInitiative", 4, 0));
    QVERIFY(m_dbm->setInterfaceDefault("DharmaInitiative", "com.dhARMa.electro.discharge",
                                    DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.discharge",
                                            DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                    "DharmaInitiative", 42, 0));

    //  try setting a default at systems scope (this should fail)
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.Discharge",
                                            DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                    "DharmaInitiative", 4, 0));
    QVERIFY(!m_dbm->setInterfaceDefault("DharmaInitiative", "com.dharma.electro.discharge",
                                    DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoWritePermissions);
    descriptor = m_dbm->interfaceDefault("com.dharma.electro.Discharge",
                                            DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                    "DharmaInitiative", 4, 0));

    //== test setInterfaceDefault(descriptor,scope) ==
    delete m_dbm;
    m_dbm = new DatabaseManager;
    filter.setServiceName("primatech");
    filter.setInterface("com.omni.device.accelerometer", "1.2",
                                QServiceFilter::ExactVersionMatch);
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                "Primatech", 1, 4));
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0], DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                        "Primatech", 1, 2));

    //  try setting a descriptor that originates from the system db
    delete m_dbm;
    m_dbm = new DatabaseManager;
    filter.setServiceName("WayneEnt");
    filter.setInterface("com.omni.device.accelerometer", "2.0",
                            QServiceFilter::ExactVersionMatch);
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    QCOMPARE(descriptors[0].scope(), QService::SystemScope);
    QVERIFY(m_dbm->setInterfaceDefault(descriptors[0],
                                    DatabaseManager::UserScope));
    descriptor = m_dbm->interfaceDefault("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer",
                                    "WayneEnt", 2, 0));

    //  try setting a descriptor at system scope
    QVERIFY(!m_dbm->setInterfaceDefault(descriptor,
                                    DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoWritePermissions);

    //==test unregisterService()==
    delete m_dbm;
    m_dbm = new DatabaseManager;
    QVERIFY(m_dbm->unregisterService("primatech", DatabaseManager::UserScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    filter.setServiceName("Primatech");
    filter.setInterface("");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(m_dbm->lastError().code(), DBError::NoError);
    QCOMPARE(descriptors.count(), 0);

    QVERIFY(!m_dbm->unregisterService("DharmaInitiative",
                DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().code(), DBError::NoWritePermissions);

    //make system database writable again
    modifyPermissionSet(systemPermsSet, QFile::WriteOwner);
    QFile::setPermissions(systemDbFilePath, systemPermsSet);

    clean();
}

void tst_DatabaseManager::CWRTXmlCompatability()
{
    m_dbm = new DatabaseManager;
    ServiceMetaData parser("");

    QStringList userServiceFiles;
    userServiceFiles << "ServiceTest.xml" << "ServiceTest1.xml";
    foreach (const QString &serviceFile, userServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::UserScope));
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
        } else {
            QVERIFY(parser.extractMetadata());
        }
        ServiceMetaDataResults results = parser.parseResults();
        QVERIFY(m_dbm->registerService(results, DatabaseManager::SystemScope));
    }

    QServiceFilter filter;
    filter.setInterface("com.nokia.ILocation");
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.nokia.ILocation", "TestService", 1,0));
    QVERIFY(compareDescriptor(descriptors[1], "com.nokia.ILocation", "TestService1", 1,1));
    QVERIFY(compareDescriptor(descriptors[2], "com.nokia.ILocation", "TestService2", 1,2));
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

void tst_DatabaseManager::clean()
{
    if (m_dbm != 0 ) {
        delete m_dbm;
    }
    m_dbm = 0;

    QSfwTestUtil::removeDirectory(QSfwTestUtil::tempSettingsPath());
}

void tst_DatabaseManager::cleanupTestCase()
{
    clean();
}

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager.moc"

