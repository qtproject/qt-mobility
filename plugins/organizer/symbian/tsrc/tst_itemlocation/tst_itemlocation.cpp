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

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

const QString m_managerNameSymbian("symbian");

Q_DECLARE_METATYPE(QOrganizerItemLocation)
Q_DECLARE_METATYPE(QOrganizerItemManager::Error)


class TestItemLocation : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
	void addLocationDetail_data();
    void addLocationDetail();
    
private:
    void addLocationData(QString managerName, QString itemType);
    
private:
    QOrganizerItemManager *m_om;
    
};

void TestItemLocation::init()
{
    QFETCH(QString, managerName);
    
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerName);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
}

void TestItemLocation::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestItemLocation::addLocationDetail_data()
{
    // Get the list of all available item managers
    QStringList availableManagers = QOrganizerItemManager::availableManagers();
    
    // Remove these since test would fail
    availableManagers.removeAll("invalid");
    availableManagers.removeAll("skeleton");
    availableManagers.removeAll("memory");
    
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerItemLocation>("location");
    QTest::addColumn<QOrganizerItemManager::Error>("error");
    
    foreach(QString manager, availableManagers) {
    	addLocationData(manager, QOrganizerItemType::TypeEvent);
    }
}

void TestItemLocation::addLocationData(QString managerName, QString itemType)
{
    QOrganizerItemLocation location;
    location.setLocationName("TestLocationName");
    
    QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << location
        << QOrganizerItemManager::NoError;

    location.setLocationName("");
    QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << location
        << QOrganizerItemManager::NoError;

    location.setLocationName("#$%^");
    QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << location
        << QOrganizerItemManager::NoError;
    
    location.setLocationName("TestLocationName");
    location.setGeoLocation("20.356784;76.276748");
    QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
    
    location.setLocationName("TestLocationName");
	location.setGeoLocation("+20.356784;76.276748");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("+20.356784;76.276748");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
			<< managerName
			<< itemType
			<< QDateTime::currentDateTime().addDays(1)
			<< location
			<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("+20.356784;-76.276748");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
			<< managerName
			<< itemType
			<< QDateTime::currentDateTime().addDays(1)
			<< location
			<< QOrganizerItemManager::NoError;
		
	location.setLocationName("TestLocationName");
	location.setGeoLocation("20.356784;-76.276748");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("200.356784;-716.276748");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::BadArgumentError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("20;-716.276748");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::BadArgumentError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("20;-71");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("20;-71");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("0;0");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("0;76");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("20;0");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
	
	location.setLocationName("TestLocationName");
	location.setGeoLocation("0.128675;0.709");
	QTest::newRow(QString("[%1]").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< location
		<< QOrganizerItemManager::NoError;
}

void TestItemLocation::addLocationDetail()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemLocation, location);
    QFETCH(QOrganizerItemManager::Error, error);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    
    // Set the start time, without which Symbian does not save entries
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    item.saveDetail(&timeRange);
    
    // Set the location
    item.saveDetail(&location);
    
    // Now save the item
    if (error == QOrganizerItemManager::NoError) {
    	QVERIFY(m_om->saveItem(&item));
    } else {
    	// Check for the expected error while saving and return.
    	QVERIFY(!m_om->saveItem(&item));
    	QVERIFY (error == m_om->error());
    	return;
    }
    
    // Fetch the item again
    item = m_om->item(item.localId());
    
    // Check if the location has been set properly
    QOrganizerItemLocation itemLocation;
    itemLocation = item.detail(QOrganizerItemLocation::DefinitionName);
    QVERIFY(itemLocation.locationName() == location.locationName());
#ifdef AGENDA_EXT_SUPPORT
    QList<QString> itemLocationString = itemLocation.geoLocation().split(QRegExp(";"));
    QList<QString> locationString = location.geoLocation().split(QRegExp(";"));
    // Check the set and retrieved values only if the latitude and logitude values were set
    // which would otherwise not return anything to the list.
    if (itemLocationString.count() == 2) {
    	double latitudeRetrieved = itemLocationString.at(0).toDouble();
    	double longitudeRetrieved = itemLocationString.at(1).toDouble();
    	double latitudeSet = locationString.at(0).toDouble();
    	double longitudeSet = locationString.at(1).toDouble();
    	QCOMPARE(latitudeSet, latitudeRetrieved);
    	QCOMPARE(longitudeSet, longitudeRetrieved);
    }
#endif
}

QTEST_MAIN(TestItemLocation);

#include "tst_itemlocation.moc"
