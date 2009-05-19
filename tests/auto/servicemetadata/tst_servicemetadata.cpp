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
#include "servicemetadata_p.h"
#include "../../../servicefw/serviceframework/servicemetadata/servicemetadata.cpp"

class ServiceMetadataTest: public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void parseInvalidServiceXML_oldXml();
    void parseValidServiceXML();
    void parseInvalidServiceXML_data();
    void parseInvalidServiceXML();
    void noCapability();
    void checkVersion_data();
    void checkVersion();
    void latestInterfaceVersion_data();
    void latestInterfaceVersion();
    void cleanupTestCase();

private:
	QDir dir;

};

void ServiceMetadataTest::initTestCase()
{
    dir = QDir(TESTDATA_DIR "/testdata");
}

void ServiceMetadataTest::cleanupTestCase()
{
}

void ServiceMetadataTest::parseInvalidServiceXML_oldXml()
{
    ServiceMetaData parser(dir.absoluteFilePath("ServiceTestOld.xml"));
    QCOMPARE(parser.extractMetadata(), false);
}

void ServiceMetadataTest::parseValidServiceXML()
{
    ServiceMetaData parser(dir.absoluteFilePath("ServiceTest.xml"));
    QCOMPARE(parser.extractMetadata(),true);
    QCOMPARE(parser.name(), QString("com.nokia.TestService"));
    //QCOMPARE(parser.version(), QString("1.0.456"));
    //QCOMPARE(parser.capabilities(), QString("ReadUserData,WriteUserData"));
    QCOMPARE(parser.description(), QString("Test service description"));
    
    QCOMPARE(parser.filePath(), QString("C:/TestData/testservice.dll"));

    QCOMPARE(parser.interfaceCount(), 5);
    
    QList<QServiceInterfaceDescriptor> allInterfaces = parser.getInterfaces();
    QServiceInterfaceDescriptor aInterface = allInterfaces.at(0);
    QCOMPARE(aInterface.interfaceName(),QString("IDownloader"));
    QCOMPARE(aInterface.majorVersion(), 1);
    QCOMPARE(aInterface.minorVersion(), 4);
    QVERIFY(aInterface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().count() == 0);
    QCOMPARE(aInterface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString(), QString("Interface that provides download support"));
        
    aInterface = allInterfaces.at(1);
    QCOMPARE(aInterface.interfaceName(),QString("ILocation"));
    QCOMPARE(aInterface.majorVersion(), 1);
    QCOMPARE(aInterface.minorVersion(), 4);
    QVERIFY(aInterface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().count() == 0);
    QCOMPARE(aInterface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString(), QString("Interface that provides location support"));
    
    aInterface = allInterfaces.at(2);
    QCOMPARE(aInterface.interfaceName(),QString("ISysInfo"));
    QCOMPARE(aInterface.majorVersion(), 2);
    QCOMPARE(aInterface.minorVersion(), 3);
    QVERIFY(aInterface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().count() == 1);
    QVERIFY(aInterface.property(QServiceInterfaceDescriptor::Capabilities).toStringList().contains("ReadUserData"));
    QCOMPARE(aInterface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString(), QString("Interface that provides system information support"));
    

    aInterface = allInterfaces.at(3);
    QCOMPARE(aInterface.interfaceName(),QString("ISendMessage"));
    QCOMPARE(aInterface.majorVersion(), 3);
    QCOMPARE(aInterface.minorVersion(), 0);
    QStringList capabilities = aInterface.property(QServiceInterfaceDescriptor::Capabilities).toStringList();
    QVERIFY(capabilities.count() == 2);
    QVERIFY(capabilities.contains("ReadUserData"));
    QVERIFY(capabilities.contains("WriteUserData"));
    QCOMPARE(aInterface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString(), QString("Interface that provides message sending support"));

    aInterface = allInterfaces.at(4);
    QCOMPARE(aInterface.interfaceName(), QString("IReceiveMessage"));
    QCOMPARE(aInterface.majorVersion(), 1);
    QCOMPARE(aInterface.minorVersion(), 1);
    capabilities = aInterface.property(QServiceInterfaceDescriptor::Capabilities).toStringList();
    QVERIFY(capabilities.count() == 3);
    QVERIFY(capabilities.contains("ReadUserData"));
    QVERIFY(capabilities.contains("WriteUserData"));
    QVERIFY(capabilities.contains("ExecUserData"));
    QCOMPARE(aInterface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString(), QString("Interface that provides message receiving support"));
}

void ServiceMetadataTest::parseInvalidServiceXML_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("expectedError");

    QTest::newRow("no such file") << "!#@!CSC" << (int)ServiceMetaData::SFW_ERROR_UNABLE_TO_OPEN_FILE;

    QTest::newRow("Test1.xml") << "Test1.xml" << (int)ServiceMetaData::SFW_ERROR_INVALID_XML_FILE;
    QTest::newRow("Test2.xml") << "Test2.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE;
    QTest::newRow("Test3.xml") << "Test3.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE_NAME;
    QTest::newRow("Test4.xml") << "Test4.xml" << (int)ServiceMetaData::SFW_ERROR_NO_INTERFACE_VERSION;
    QTest::newRow("Test5.xml") << "Test5.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE_FILEPATH;
    QTest::newRow("Test7.xml") << "Test7.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE_INTERFACE;
    QTest::newRow("Test8.xml") << "Test8.xml" << (int)ServiceMetaData::SFW_ERROR_NO_INTERFACE_NAME;
    QTest::newRow("Test9.xml") << "Test9.xml" << (int)ServiceMetaData::SFW_ERROR_PARSE_SERVICE; ///check error code
    QTest::newRow("Test10.xml") << "Test10.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE_INTERFACE;
    QTest::newRow("Test11.xml") << "Test11.xml" << (int)ServiceMetaData::SFW_ERROR_NO_INTERFACE_NAME;
    QTest::newRow("Test12.xml") << "Test12.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE_FILEPATH;

    QStringList badVersionXml;
    badVersionXml << "Test14.xml" << "Test15.xml" << "Test17.xml" << "Test18.xml";
    for (int i=0; i<badVersionXml.count(); i++)
        QTest::newRow(qPrintable(badVersionXml[i])) << badVersionXml[i] << (int)ServiceMetaData::SFW_ERROR_INVALID_VERSION;

    QTest::newRow("empty interface name") << "emptyInterfaceName.xml" << (int)ServiceMetaData::SFW_ERROR_NO_INTERFACE_NAME;
    QTest::newRow("empty service name") << "emptyServiceName.xml" << (int)ServiceMetaData::SFW_ERROR_NO_SERVICE_NAME;
    QTest::newRow("empty version") << "emptyVersion.xml" << (int)ServiceMetaData::SFW_ERROR_NO_INTERFACE_VERSION;
    QTest::newRow("duplicated interface A") << "Test13.xml" << (int)ServiceMetaData::SFW_ERROR_DUPLICATED_INTERFACE;
    QTest::newRow("duplicated interface B") << "Test19.xml" << (int)ServiceMetaData::SFW_ERROR_DUPLICATED_INTERFACE;
}

void ServiceMetadataTest::parseInvalidServiceXML()
{
    QFETCH(QString, fileName);
    QFETCH(int, expectedError);

    ServiceMetaData parser(dir.absoluteFilePath(fileName));
    QVERIFY(!parser.extractMetadata());
    QCOMPARE(parser.getLatestError(), expectedError);
}

void ServiceMetadataTest::noCapability()
{
    ServiceMetaData parser(dir.absoluteFilePath("Test6.xml"));
    QVERIFY(parser.extractMetadata());
}

void ServiceMetadataTest::checkVersion_data()
{
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("result");
    QTest::addColumn<int>("major");
    QTest::addColumn<int>("minor");

    QTest::newRow("checkVersion_data():Invalid 1") << "" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 2") << "0.3" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 3") << "01.3" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 4") << "1.03" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 5") << "x.y" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 6") << "23" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 7") << "sdfsfs" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 8") << "%#5346" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 9") << ".66" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 10") << "1.3.4" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 11") << "1.a" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 12") << "b.1" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 13") << "3." << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 14") << "-1" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 15") << "0.0" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 16") << ".x" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 17") << "x." << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 18") << "1. 0" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 19") << "1  .0" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 20") << "1  0" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 21") << "1 . 0" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 22") << " 1.5" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 23") << "1.5 " << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 24") << " 1.5 " << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 25") << "1.5 1.6" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 26") << "-1.0" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 27") << "1.-1" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 28") << "-5.-1" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 29") << "4,8" << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 30") << "  " << false << -1 << -1;
    QTest::newRow("checkVersion_data():Invalid 31") << "1.9b" << false << -1 << -1;

    QTest::newRow("checkVersion_data():Valid 1") << "1.0" << true << 1 << 0;
    QTest::newRow("checkVersion_data():Valid 2") << "1.00" << true << 1 << 0;
    QTest::newRow("checkVersion_data():Valid 3") << "99.99" << true << 99 << 99;
    QTest::newRow("checkVersion_data():Valid 4") << "2.3" << true << 2 << 3;
    QTest::newRow("checkVersion_data():Valid 5") << "10.3" << true << 10 << 3;
    QTest::newRow("checkVersion_data():Valid 6") << "5.10" << true << 5 << 10;
    QTest::newRow("checkVersion_data():Valid 7") << "10.10" << true << 10 << 10;
}

void ServiceMetadataTest::checkVersion()
{
    QFETCH(QString, version);
    QFETCH(bool, result);
    QFETCH(int, major);
    QFETCH(int, minor);

    ServiceMetaData parser(dir.absoluteFilePath("ServiceTest.xml"));
    QCOMPARE(parser.checkVersion(version), result);
    int majorVer;
    int minorVer;
    parser.transformVersion(version, &majorVer, &minorVer);
    QCOMPARE(majorVer, major);
    QCOMPARE(minorVer, minor);
}

void ServiceMetadataTest::latestInterfaceVersion_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("major");
    QTest::addColumn<int>("minor");

    //cases 1-3 contains services with more than one interface
    QTest::newRow("latestVersion_data() 1") << "latestVersion.xml" << 2 << 1; //latest version in middle
    QTest::newRow("latestVersion_data() 2") << "latestVersion2.xml" << 3 << 0; //latest version at end
    QTest::newRow("latestVersion_data() 3") << "latestVersion3.xml" << 5 << 7; //latest version at start
    QTest::newRow("latestVersion_data() 4") << "latestVersion4.xml" << 1 << 7; //only one version

}

void ServiceMetadataTest::latestInterfaceVersion()
{
    QFETCH(QString, fileName);
    QFETCH(int, major);
    QFETCH(int, minor);

    ServiceMetaData parser(dir.absoluteFilePath(fileName));
    QCOMPARE(parser.extractMetadata(), true);
    QServiceInterfaceDescriptor interface = parser.latestInterfaceVersion("com.nokia.service.contacts");
    QCOMPARE(interface.majorVersion(), major);
    QCOMPARE(interface.minorVersion(), minor);
    QCOMPARE(interface.property(QServiceInterfaceDescriptor::InterfaceDescription).toString(),
            QString("Contacts management service")); //make sure we're getting the right interface
}

QTEST_MAIN(ServiceMetadataTest)
#include "tst_servicemetadata.moc"
