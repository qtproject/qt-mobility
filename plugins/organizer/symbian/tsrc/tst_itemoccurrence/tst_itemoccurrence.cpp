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

Q_DECLARE_METATYPE(QOrganizerItemRecurrenceRule)

class TestItemOccurrence : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
    void addOccurrenceDetail_data();
    void addOccurrenceDetail();
    void fetchNegative_data();
    void fetchNegative();
private:
    void addOccurrenceData(QString managerName, QString itemType);
    
private:
    QOrganizerItemManager *m_om;
    
};

void TestItemOccurrence::init()
{
    QFETCH(QString, managerName);
    
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerName);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
}

void TestItemOccurrence::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestItemOccurrence::addOccurrenceDetail_data()
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
    QTest::addColumn<QOrganizerItemRecurrenceRule>("rrule");
    
    foreach(QString manager, availableManagers) {
        addOccurrenceData(manager, QOrganizerItemType::TypeEvent);
    }
}

void TestItemOccurrence::addOccurrenceData(QString managerName, QString itemType)
{
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(3);
    QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Wednesday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] weekly on Wednesday for 3 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        <<  QDateTime(QDate(QDate::currentDate().year() , 9, 1))
        << rrule;
    
}

void TestItemOccurrence::addOccurrenceDetail()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemRecurrenceRule, rrule);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QDateTime endTime(QDate(QDate::currentDate().year() , 9, 30));
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
        
    // Fetch the saved item
    item = m_om->item(item.localId());
    
    // Fetch the item again
    QList<QOrganizerItem> instanceList = m_om->itemInstances(item,startTime,endTime);
    
    QCOMPARE(instanceList.size(), 3);
    QOrganizerItem secondItem = instanceList.at(1);
    QCOMPARE(secondItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence secondEvent = static_cast<QOrganizerEventOccurrence>(secondItem);
    QCOMPARE(secondEvent.startDateTime(), QDateTime(QDate(QDate::currentDate().year() , 9, 8)));
    QCOMPARE(secondEvent.localId(), (unsigned int)0);
    QCOMPARE(secondEvent.parentLocalId(), item.localId());
}

void TestItemOccurrence::fetchNegative_data()
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
 
    QString itemType = QOrganizerItemType::TypeEvent;
    foreach(QString manager, availableManagers) {
       QTest::newRow(QString("[%1] non repeating entry").arg(manager).toLatin1().constData())
            << manager
            << itemType
            << QDateTime::currentDateTime().addSecs(3600);
    }    
}
void TestItemOccurrence::fetchNegative()
{
    //Fetch instances for a non recurring entry
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    
    // Set the item type
    QOrganizerItem item;
    QList<QOrganizerItem> instanceList;
    item.setType(itemType);
    QDateTime endTime(startTime);
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch the saved item
    item = m_om->item(item.localId());
        
    // Fetch the item instances for a non repeating entry
    instanceList = m_om->itemInstances(item,startTime,endTime);
    
    //Only a single instance should be returned for non-repeating entry
    QCOMPARE(instanceList.size(), 1);
    
    //Fetch instance for an item with invalid local uid,should return zero occurrence
    //QOrganizerItem invalidItem(item);
    //QOrganizerItemId id;
    //id.setLocalId(1000);
    //invalidItem.set;
    //instanceList = m_om->itemInstances(invalidItem,startTime,endTime);
    //QCOMPARE(instanceList.size(), 0);
    
    // Fetch the item instance with invalid count
    instanceList = m_om->itemInstances(item,startTime,QDateTime(),-2);
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);
    
   // Fetch the item instance with invalid starttime
   instanceList = m_om->itemInstances(item,QDateTime(),endTime);
   QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);
   
   // Fetch the item instance with invalid endtime
   instanceList = m_om->itemInstances(item,startTime,QDateTime());
   QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);
   

}
QTEST_MAIN(TestItemOccurrence);

#include "tst_itemoccurrence.moc"
