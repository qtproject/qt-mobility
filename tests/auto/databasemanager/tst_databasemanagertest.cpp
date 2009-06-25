/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
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
#include "../../../servicefw/serviceframework/servicemetadata/servicemetadata.cpp"
#include "../../../servicefw/serviceframework/servicedatabase/servicedatabase.cpp"
#include <qservicefilter.h>
#include <databasemanager.h>
#include "../../../servicefw/serviceframework/servicedatabase/databasemanager.cpp"
#include "../qsfwtestutil.h"

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

class DatabaseManagerUnitTest: public QObject
{
    Q_OBJECT

private slots:
        void initTestCase();
        void registerService();
        void getInterfaces();
        void getServiceNames();
        void defaultService();
        void unregisterService();
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

        DatabaseManager *m_dbm;
        QDir m_testdir;
};

void DatabaseManagerUnitTest::initTestCase()
{
    QSfwTestUtil::setupTempUserDb();
    QSfwTestUtil::setupTempSystemDb();
    m_dbm = new DatabaseManager;
}

void DatabaseManagerUnitTest::registerService()
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
        QVERIFY(m_dbm->registerService(parser, DatabaseManager::UserScope));
    }

    QStringList systemServiceFiles;
    systemServiceFiles << "ServiceOmni.xml" << "ServiceWayneEnt.xml"
                        << "ServiceDharma_Hydra.xml"
                        << "ServiceDharma_Orchid.xml"
                        << "ServiceAutobot.xml";
    foreach (const QString &serviceFile, systemServiceFiles) {
        parser.setDevice(new QFile(m_testdir.absoluteFilePath(serviceFile)));
        QVERIFY(parser.extractMetadata());
        QVERIFY(m_dbm->registerService(parser, DatabaseManager::SystemScope));
    }
}

void DatabaseManagerUnitTest::getInterfaces()
{
    QString iface("com.omni.device.accelerometer");
    QServiceFilter filter(iface);
    QList<QServiceInterfaceDescriptor> descriptors;
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 5);
    QVERIFY(compareDescriptor(descriptors[0], iface, "LuthorCorp", 1,2));
    QCOMPARE(descriptors[0].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[1], iface, "Primatech", 1, 4));
    QCOMPARE(descriptors[1].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[2], iface, "Primatech", 1, 2));
    QCOMPARE(descriptors[2].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[3], iface, "OMNI", 1, 1));
    QCOMPARE(descriptors[3].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[4], iface, "WayneEnt", 2, 0));
    QCOMPARE(descriptors[4].inSystemScope(), true);

    //check that we can get descriptors for a service spread
    //over the user and system databases
    filter.setServiceName("DharmaInitiative");
    filter.setInterface("");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(),10);

    QStringList capabilities;
    QVERIFY(compareDescriptor(descriptors[0], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/swan.dll"));
    QCOMPARE(descriptors[0].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[1],  "com.dharma.electro.discharge","DharmaInitiative", 8, 0, capabilities, "C:/island/pearl.dll"));
    QCOMPARE(descriptors[1].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[2], "com.dharma.electro.discharge", "DharmaInitiative", 15, 0, capabilities, "C:/island/flame.dll"));
    QCOMPARE(descriptors[2].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[3], "com.dharma.radio", "DharmaInitiative", 8, 15, capabilities, "C:/island/flame.dll"));
    QCOMPARE(descriptors[3].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[4], "com.dharma.electro.discharge", "DharmaInitiative", 16, 0, capabilities, "C:/island/flame.dll"));
    QCOMPARE(descriptors[4].inSystemScope(), false);
    QVERIFY(compareDescriptor(descriptors[5], "com.dharma.electro.discharge", "DharmaInitiative", 23, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[5].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[6], "com.dharma.electro.discharge", "DharmaInitiative", 42, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[6].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[7], "com.dharma.cage", "DharmaInitiative", 3, 16, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[7].inSystemScope(), true);
    //note; the following system interface implementation overlaps with an interface implementation already provided in the user database
    QVERIFY(compareDescriptor(descriptors[8], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[8].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[9], "com.dharma.wheel", "DharmaInitiative", 1, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[9].inSystemScope(), true);

    //do a system database only search
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 5);
    QVERIFY(compareDescriptor(descriptors[0], "com.dharma.electro.discharge", "DharmaInitiative", 23, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[0].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[1], "com.dharma.electro.discharge", "DharmaInitiative", 42, 0, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[1].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[2], "com.dharma.cage", "DharmaInitiative", 3, 16, capabilities, "C:/island/hydra.dll"));
    QCOMPARE(descriptors[2].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[3], "com.dharma.electro.discharge", "DharmaInitiative", 4, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[3].inSystemScope(), true);
    QVERIFY(compareDescriptor(descriptors[4], "com.dharma.wheel", "DharmaInitiative", 1, 0, capabilities, "C:/island/orchid.dll"));
    QCOMPARE(descriptors[4].inSystemScope(), true);
}

void DatabaseManagerUnitTest::getServiceNames()
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
}

void DatabaseManagerUnitTest::defaultService()
{
    QServiceInterfaceDescriptor descriptor;

    //get a user default service at user scope
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                    DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.omni.device.accelerometer", "LuthorCorp", 1, 2));
    QVERIFY(!descriptor.inSystemScope());

    //user a sytem default from user scope
    descriptor = m_dbm->defaultServiceInterface("com.Dharma.wheel", DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.dharma.wheel", "DharmaInitiative", 1,0));
    QVERIFY(descriptor.inSystemScope());

    //get a system default service at system scope
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                     DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "omni", 1, 1));

    //set a user default from a user interface implementation
    QList<QServiceInterfaceDescriptor> descriptors;
    QServiceFilter filter;
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.accelerometer", "1.4");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);

    QVERIFY(m_dbm->setDefaultService(descriptors[0], DatabaseManager::UserScope));
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                    DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor,"com.omni.device.accelerometer", "Primatech", 1, 4));
    QVERIFY(!descriptor.inSystemScope());

    //set a system default from a  system interface implementation
    filter.setServiceName("WayneEnt");
    filter.setInterface("com.omni.device.accelerometer","2.0");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 1);

    QVERIFY(m_dbm->setDefaultService(descriptors[0], DatabaseManager::SystemScope));
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                    DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "WayneEnt", 2,0));
    QVERIFY(descriptor.inSystemScope());

    //set a user default with a system interface descriptor
    filter.setServiceName("omni");
    filter.setInterface("com.omni.device.accelerometer","1.1");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::SystemScope);
    QCOMPARE(descriptors.count(), 1);
    QVERIFY(m_dbm->setDefaultService(descriptors[0], DatabaseManager::UserScope));
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(descriptor.isValid());
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "omni", 1, 1));
    QVERIFY(descriptor.inSystemScope());

    //set a system default using a user interface descriptor
    filter.setServiceName("Primatech");
    filter.setInterface("com.omni.device.accelerometer","1.4");
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);

    QVERIFY(!m_dbm->setDefaultService(descriptors[0], DatabaseManager::SystemScope));
    QCOMPARE(m_dbm->lastError().errorCode(), DBError::InvalidDescriptorScope);

    // == try setting defaults using setDefaultService(serviceName, interfaceName, ...)
    //set a local default in the user scope database
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "omni", 1, 1));
    QVERIFY(m_dbm->setDefaultService("LuthorCorp", "com.omni.device.accelerometer",
                                        DatabaseManager::UserScope));
    descriptor = m_dbm->defaultServiceInterface("com.omni.device.accelerometer",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.omni.device.accelerometer", "LuthorCorp", 1, 2));

    //set a system default in the user scope database
    descriptor = m_dbm->defaultServiceInterface("com.dharma.electro.disCHARGE",
                                                DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                            "DharmaInitiative", 4, 0));
    QVERIFY(!descriptor.inSystemScope());
    m_dbm->setDefaultService("DharmaInitiative", "com.dharma.electro.discharge",
                                        DatabaseManager::UserScope);

    descriptor = m_dbm->defaultServiceInterface("com.dharma.electro.discharge",
                                                DatabaseManager::UserScope);
    QVERIFY(m_dbm->lastError().errorCode() == DBError::NoError);
    QVERIFY(compareDescriptor(descriptor, "com.dharma.electro.discharge",
                                "DharmaInitiative", 42, 0));

    //set a user default in the user scope database but where there
    //exist interface implementations in both user and system databases
    //also check that the system scope default has not changed
    filter.setServiceName("Autobot");
    filter.setInterface("com.cybertron.transform", "2.5", QServiceFilter::ExactVersionMatch );
    descriptors = m_dbm->getInterfaces(filter, DatabaseManager::UserScope);
    QCOMPARE(descriptors.count(), 1);
    m_dbm->setDefaultService(descriptors[0], DatabaseManager::UserScope);
    descriptor = m_dbm->defaultServiceInterface("com.cybertron.transform",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.cybertron.transform",
                                "Autobot", 2, 5));
    descriptor = m_dbm->defaultServiceInterface("com.cybertron.transform",
                                        DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.cybertron.transform",
                                "Autobot", 2, 5));

    m_dbm->setDefaultService("Decepticon", "com.cybertron.transform", DatabaseManager::UserScope);
    descriptor = m_dbm->defaultServiceInterface("com.cybertron.transform",
                                        DatabaseManager::UserScope);
    QVERIFY(compareDescriptor(descriptor, "com.cybertron.transform",
                                "Decepticon", 5,3));
    descriptor = m_dbm->defaultServiceInterface("com.cybertron.transform",
                                        DatabaseManager::SystemScope);
    QVERIFY(compareDescriptor(descriptors[0], "com.cybertron.transform",
                                "Autobot", 2, 5));
}

void DatabaseManagerUnitTest::unregisterService()
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
    QVERIFY(m_dbm->registerService(parser, DatabaseManager::UserScope));

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
}

bool DatabaseManagerUnitTest::compareDescriptor(QServiceInterfaceDescriptor interface,
        QString interfaceName, QString serviceName, int majorVersion, int minorVersion)
{
    if (interface.d == NULL )
        return false;
    interface.d->properties[QServiceInterfaceDescriptor::Capabilities] = QStringList();

    return compareDescriptor(interface, interfaceName, serviceName, majorVersion, minorVersion,
            QStringList());
}

bool DatabaseManagerUnitTest::compareDescriptor(QServiceInterfaceDescriptor interface,
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
        if (interface.property(QServiceInterfaceDescriptor::Location).toString() != filePath) {
            qWarning() << "File path mismatch: expected =" << filePath
                << " actual =" << interface.property(QServiceInterfaceDescriptor::Location).toString();
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

void DatabaseManagerUnitTest::cleanupTestCase()
{
    m_dbm->close();
    QSfwTestUtil::removeTempUserDb();
    QSfwTestUtil::removeTempSystemDb();
    delete m_dbm;
}

QTEST_MAIN(DatabaseManagerUnitTest)
#include "tst_databasemanagertest.moc"
