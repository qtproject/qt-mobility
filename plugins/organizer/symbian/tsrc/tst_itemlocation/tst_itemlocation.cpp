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

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>
#include <calcommon.h> // for SYMBIAN_CALENDAR_V2

QTM_USE_NAMESPACE

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
    QOrganizerItemLocation loc;
    loc.setLocationName("TestLocationName");
    
    QTest::newRow(QString("[%1] LocationName=%2").arg(managerName).arg(loc.locationName()).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << loc
        << QOrganizerItemManager::NoError;

    loc.setLocationName("");
    QTest::newRow(QString("[%1] LocationName=%2").arg(managerName).arg(loc.locationName()).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << loc
        << QOrganizerItemManager::NoError;

    loc.setLocationName("#$%^");
    QTest::newRow(QString("[%1] LocationName=%2").arg(managerName).arg(loc.locationName()).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << loc
        << QOrganizerItemManager::NoError;

#ifdef SYMBIAN_CALENDAR_V2
    loc.setLocationName("TestLocationName");
    loc.setGeoLocation("20.356784;76.276748");
    QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
    
    loc.setLocationName("TestLocationName");
	loc.setGeoLocation("+20.356784;76.276748");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("+20.356784;-76.276748");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << loc
        << QOrganizerItemManager::NoError;

	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("20.356784;-76.276748");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("200.356784;-716.276748");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::BadArgumentError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("20;-716.276748");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::BadArgumentError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("20;-71");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("0;0");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("0;76");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("20;0");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
	
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("0.128675;0.709");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::NoError;
#else
	loc.setLocationName("TestLocationName");
	loc.setGeoLocation("0.128675;0.709");
	QTest::newRow(QString("[%1] LocationName=%2 GeoLocation=%3").arg(managerName).arg(loc.locationName()).arg(loc.geoLocation()).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime::currentDateTime().addDays(1)
		<< loc
		<< QOrganizerItemManager::InvalidDetailError; // GeoLocation is not supported so QOrganizerItemManagerEngine::validateItem will return this error.
#endif
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
#ifdef SYMBIAN_CALENDAR_V2
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
