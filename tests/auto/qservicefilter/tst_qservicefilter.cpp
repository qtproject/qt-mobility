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
#include <qservicefilter.h>

//the class name must match the friend class name in 
//qservicefilter.h
class QServiceManager: public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void versionMatching();
    void versionMatching_data();
    void setInterface();
    void testAssignmentOperator();
    void testConstructor();
#ifndef QT_NO_DATASTREAM
    void streamTest();
#endif 
    
};

void QServiceManager::initTestCase()
{
}

void QServiceManager::versionMatching_data()
{
    QTest::addColumn<QString>("version");
    QTest::addColumn<int>("majorV");
    QTest::addColumn<int>("minorV");
    QTest::addColumn<int>("rule");

    //invalid cases
    QTest::newRow("versionMatching_data():Invalid 1") << "" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 3") << "01.3" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 4") << "1.03" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 5") << "x.y" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 6") << "23" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 7") << "sdfsfs" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 8") << "%#5346" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 9") << ".66" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 10") << "1.3.4" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 11") << "1.a" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 12") << "b.1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 13") << "3." << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 14") << "-1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 16") << ".x" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 17") << "x." << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 18") << "1.  0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 19") << "1  .0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 20") << "1  0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 21") << "1 . 0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 22") << " 1.5" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 23") << "1.5 " << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 24") << " 1.5 " << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 25") << "1.5 1.6" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 26") << "-1.0" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 27") << "1.-1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 28") << "-5.-1" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 29") << "4,8" << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():Invalid 30") << "   " << -1 << -1 << (int)QServiceFilter::MinimumVersionMatch;

   
    //valid cases 
    QTest::newRow("versionMatching_data():ValidMin 1") << "1.0" << 1 << 0 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 2") << "1.00" << 1 << 0 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 3") << "99.99" << 99 << 99 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 4") << "2.3" << 2 << 3 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 5") << "10.3" << 10 << 3 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 6") << "5.10" << 5 << 10 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 7") << "10.10" << 10 << 10 << (int)QServiceFilter::MinimumVersionMatch;

    QTest::newRow("versionMatching_data():ValidMin 8") << "0.3" << 0 << 3 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 10") << "0.0" << 0 << 0 << (int)QServiceFilter::MinimumVersionMatch;
    QTest::newRow("versionMatching_data():ValidMin 11") << "00.00" << 0 << 0 << (int)QServiceFilter::MinimumVersionMatch;

    QTest::newRow("versionMatching_data():ValidExact 1") << "1.0" << 1 << 0 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 2") << "1.00" << 1 << 0 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 3") << "99.99" << 99 << 99 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 4") << "2.3" << 2 << 3 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 5") << "10.3" << 10 << 3 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 6") << "5.10" << 5 << 10 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 7") << "10.10" << 10 << 10 << (int)QServiceFilter::ExactVersionMatch;

    QTest::newRow("versionMatching_data():ValidExact 8") << "0.3" << 0 << 3 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 10") << "0.0" << 0 << 0 << (int)QServiceFilter::ExactVersionMatch;
    QTest::newRow("versionMatching_data():ValidExact 11") << "00.00" << 0 << 0 << (int)QServiceFilter::ExactVersionMatch;
}

void QServiceManager::versionMatching()
{
    QFETCH(QString, version);
    QFETCH(int, majorV);
    QFETCH(int, minorV);
    QFETCH(int, rule);

    QServiceFilter filter;
    QCOMPARE(filter.majorVersion, -1);
    QCOMPARE(filter.minorVersion, -1);
    QCOMPARE(filter.matchingRule, QServiceFilter::MinimumVersionMatch);

    filter.setInterface("com.nokia.qt.test",version, (QServiceFilter::VersionMatchRule)rule);
    QCOMPARE(filter.majorVersion, majorV);
    QCOMPARE(filter.minorVersion, minorV);
    QCOMPARE(filter.matchingRule, (QServiceFilter::VersionMatchRule)rule);
}


void QServiceManager::setInterface()
{
    //don't separate this out into test_data() function as we test the behavior
    //on the same filter object
    QServiceFilter filter;
    QCOMPARE(filter.majorVersion, -1);
    QCOMPARE(filter.minorVersion, -1);
    QCOMPARE(filter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString());

    filter.setInterface("com.nokia.qt.text", "1.0", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.majorVersion, 1);
    QCOMPARE(filter.minorVersion, 0);
    QCOMPARE(filter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString("com.nokia.qt.text"));

    filter.setInterface("com.nokia.qt.text", "1.5", QServiceFilter::MinimumVersionMatch);

    QCOMPARE(filter.majorVersion, 1);
    QCOMPARE(filter.minorVersion, 5);
    QCOMPARE(filter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString("com.nokia.qt.text"));

    //invalid version tag -> ignore the call
    filter.setInterface("com.nokia.qt.label", "f.0", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.majorVersion, 1);
    QCOMPARE(filter.minorVersion, 5);
    QCOMPARE(filter.matchingRule, QServiceFilter::MinimumVersionMatch); //default
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString("com.nokia.qt.text"));

    //empty version tag -> reset version
    filter.setInterface("com.nokia.qt.label", "", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.majorVersion, -1);
    QCOMPARE(filter.minorVersion, -1);
    QCOMPARE(filter.matchingRule, QServiceFilter::ExactVersionMatch); //default
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString("com.nokia.qt.label"));

    //empty interface tag -> ignore the call
    filter.setInterface("", "4.5", QServiceFilter::MinimumVersionMatch);

    QCOMPARE(filter.majorVersion, -1);
    QCOMPARE(filter.minorVersion, -1);
    QCOMPARE(filter.matchingRule, QServiceFilter::ExactVersionMatch); //default
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString("com.nokia.qt.label"));
    
    //set a valid 
    filter.setInterface("com.nokia.qt.valid", "4.77", QServiceFilter::ExactVersionMatch);

    QCOMPARE(filter.majorVersion, 4);
    QCOMPARE(filter.minorVersion, 77);
    QCOMPARE(filter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(filter.service, QString());
    QCOMPARE(filter.interface, QString("com.nokia.qt.valid"));

    filter.setServiceName("myService");
    QCOMPARE(filter.majorVersion, 4);
    QCOMPARE(filter.minorVersion, 77);
    QCOMPARE(filter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(filter.service, QString("myService"));
    QCOMPARE(filter.interface, QString("com.nokia.qt.valid"));

    //test default constructed version and matching rule
    filter.setInterface("com.nokia.qt.valid2");
    QCOMPARE(filter.majorVersion, -1);
    QCOMPARE(filter.minorVersion, -1);
    QCOMPARE(filter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(filter.service, QString("myService"));
    QCOMPARE(filter.interface, QString("com.nokia.qt.valid2"));
}

void QServiceManager::testAssignmentOperator()
{
    QServiceFilter emptyFilter;
    QServiceFilter tempFilter(emptyFilter);
   

    //assign empty filter to empty filter 
    QCOMPARE(emptyFilter.majorVersion, -1);
    QCOMPARE(emptyFilter.minorVersion, -1);
    QCOMPARE(emptyFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(emptyFilter.service, QString(""));
    QCOMPARE(emptyFilter.interface, QString(""));

    QCOMPARE(tempFilter.majorVersion, -1);
    QCOMPARE(tempFilter.minorVersion, -1);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.service, QString(""));
    QCOMPARE(tempFilter.interface, QString(""));

    tempFilter = emptyFilter;
    
    QCOMPARE(tempFilter.majorVersion, -1);
    QCOMPARE(tempFilter.minorVersion, -1);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.service, QString(""));
    QCOMPARE(tempFilter.interface, QString(""));
   
    //assign filter to new filter via constructor
    tempFilter.setInterface("com.nokia.qt.valid", "4.77", QServiceFilter::ExactVersionMatch);
    tempFilter.setServiceName("ServiceName");
    QCOMPARE(tempFilter.majorVersion, 4);
    QCOMPARE(tempFilter.minorVersion, 77);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter.service, QString("ServiceName"));
    QCOMPARE(tempFilter.interface, QString("com.nokia.qt.valid"));

    QServiceFilter constructFilter(tempFilter);
    QCOMPARE(constructFilter.majorVersion, 4);
    QCOMPARE(constructFilter.minorVersion, 77);
    QCOMPARE(constructFilter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(constructFilter.service, QString("ServiceName"));
    QCOMPARE(constructFilter.interface, QString("com.nokia.qt.valid"));

    //ensure that we don't have any potential references between tempFilter and 
    //constructedFilter
    tempFilter.setServiceName("NewServiceName");
    tempFilter.setInterface("com.nokia.qt.valid2", "5.88", QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.majorVersion, 5);
    QCOMPARE(tempFilter.minorVersion, 88);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.service, QString("NewServiceName"));
    QCOMPARE(tempFilter.interface, QString("com.nokia.qt.valid2"));
    QCOMPARE(constructFilter.majorVersion, 4);
    QCOMPARE(constructFilter.minorVersion, 77);
    QCOMPARE(constructFilter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(constructFilter.service, QString("ServiceName"));
    QCOMPARE(constructFilter.interface, QString("com.nokia.qt.valid"));

    //assign empty filter to filter with values
    constructFilter = emptyFilter;
    QCOMPARE(constructFilter.majorVersion, -1);
    QCOMPARE(constructFilter.minorVersion, -1);
    QCOMPARE(constructFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(constructFilter.service, QString(""));
    QCOMPARE(constructFilter.interface, QString(""));
}

void QServiceManager::testConstructor()
{
    QServiceFilter tempFilter1("");
    QCOMPARE(tempFilter1.majorVersion, -1);
    QCOMPARE(tempFilter1.minorVersion, -1);
    QCOMPARE(tempFilter1.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter1.service, QString());
    QCOMPARE(tempFilter1.interface, QString(""));

    QServiceFilter tempFilter2("com.nokia.qt.test");
    QCOMPARE(tempFilter2.majorVersion, -1);
    QCOMPARE(tempFilter2.minorVersion, -1);
    QCOMPARE(tempFilter2.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter2.service, QString());
    QCOMPARE(tempFilter2.interface, QString("com.nokia.qt.test"));

    QServiceFilter tempFilter3("com.nokia.qt.test", "10.5");
    QCOMPARE(tempFilter3.majorVersion, 10);
    QCOMPARE(tempFilter3.minorVersion, 5);
    QCOMPARE(tempFilter3.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter3.service, QString());
    QCOMPARE(tempFilter3.interface, QString("com.nokia.qt.test"));

    QServiceFilter tempFilter4("com.nokia.qt.test", "11.7", QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter4.majorVersion, 11);
    QCOMPARE(tempFilter4.minorVersion, 7);
    QCOMPARE(tempFilter4.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter4.service, QString());
    QCOMPARE(tempFilter4.interface, QString("com.nokia.qt.test"));
    
}

#ifndef QT_NO_DATASTREAM
void QServiceManager::streamTest()
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);

    QServiceFilter emptyFilter;
    QCOMPARE(emptyFilter.majorVersion, -1);
    QCOMPARE(emptyFilter.minorVersion, -1);
    QCOMPARE(emptyFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(emptyFilter.service, QString(""));
    QCOMPARE(emptyFilter.interface, QString(""));

    buffer.seek(0);
    stream << emptyFilter;
    
    QServiceFilter tempFilter;
    QCOMPARE(tempFilter.majorVersion, -1);
    QCOMPARE(tempFilter.minorVersion, -1);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.service, QString(""));
    QCOMPARE(tempFilter.interface, QString(""));

    buffer.seek(0);
    stream >> tempFilter;
    
    QCOMPARE(tempFilter.majorVersion, -1);
    QCOMPARE(tempFilter.minorVersion, -1);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(tempFilter.service, QString(""));
    QCOMPARE(tempFilter.interface, QString(""));
   
    //assign filter to new filter via constructor
    tempFilter.setInterface("com.nokia.qt.valid", "4.77", QServiceFilter::ExactVersionMatch);
    tempFilter.setServiceName("ServiceName");
    QCOMPARE(tempFilter.majorVersion, 4);
    QCOMPARE(tempFilter.minorVersion, 77);
    QCOMPARE(tempFilter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(tempFilter.service, QString("ServiceName"));
    QCOMPARE(tempFilter.interface, QString("com.nokia.qt.valid"));
    buffer.seek(0);
    stream << tempFilter;

    QServiceFilter constructFilter;
    buffer.seek(0);
    stream >> constructFilter;
    QCOMPARE(constructFilter.majorVersion, 4);
    QCOMPARE(constructFilter.minorVersion, 77);
    QCOMPARE(constructFilter.matchingRule, QServiceFilter::ExactVersionMatch);
    QCOMPARE(constructFilter.service, QString("ServiceName"));
    QCOMPARE(constructFilter.interface, QString("com.nokia.qt.valid"));

    //assign empty filter to filter with values

    buffer.seek(0);
    stream << emptyFilter;
    buffer.seek(0);
    stream >> constructFilter;
    QCOMPARE(constructFilter.majorVersion, -1);
    QCOMPARE(constructFilter.minorVersion, -1);
    QCOMPARE(constructFilter.matchingRule, QServiceFilter::MinimumVersionMatch);
    QCOMPARE(constructFilter.service, QString(""));
    QCOMPARE(constructFilter.interface, QString(""));
}
#endif 

void QServiceManager::cleanupTestCase()
{
}

QTEST_MAIN(QServiceManager)
#include "tst_qservicefilter.moc"
