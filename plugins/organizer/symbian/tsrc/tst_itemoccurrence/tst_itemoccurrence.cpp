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

Q_DECLARE_METATYPE(QOrganizerItemPriority)
Q_DECLARE_METATYPE(QOrganizerItemLocation)

class tst_ItemOccurrence : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
    void addOccurrenceDetail_data();
    void addOccurrenceDetail();
    void fetchOccurrenceByFilterSort_data();
    void fetchOccurrenceByFilterSort();
    void addOccurrenceWithException_data();
    void addOccurrenceWithException();
    void editOccurrence_data();
    void editOccurrence();
    void editOccurrenceNegative_data();
    void editOccurrenceNegative();
    void updateOccurrenceId_data(){addManagers();};
    void updateOccurrenceId();
    void fetchNegative_data();
    void fetchNegative();
    void daylightSavingTime_data() { addManagers(); }
    void daylightSavingTime();
    void leapYear_data() { addManagers(); }
    void leapYear();	

private:
    void addManagers();
    QStringList getAvailableManagers();
    void addOccurrenceData(QString managerName, QString itemType);
    
private:
    QOrganizerManager *m_om;
};

void tst_ItemOccurrence::init()
{
    QFETCH(QString, managerName);
    
    // Create a new item manager instance
    m_om = new QOrganizerManager(managerName);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds());
}

void tst_ItemOccurrence::cleanup()
{
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds());

    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

QStringList tst_ItemOccurrence::getAvailableManagers()
{
    // Get the list of all available item managers
    QStringList availableManagers = QOrganizerManager::availableManagers();
    
    // Remove these since test would fail
    availableManagers.removeAll("invalid");
    availableManagers.removeAll("skeleton");
    availableManagers.removeAll("memory");
    
    return availableManagers;
}

void tst_ItemOccurrence::addOccurrenceDetail_data()
{
    QStringList availableManagers = getAvailableManagers();    
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    
    foreach(QString manager, availableManagers) {
        addOccurrenceData(manager, QOrganizerItemType::TypeEvent);
    }
}

void tst_ItemOccurrence::addOccurrenceData(QString managerName, QString itemType)
{
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(3);
    QSet<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.insert(Qt::Wednesday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] weekly on Wednesday for 3 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        <<  QDateTime(QDate(QDate::currentDate().year() , 9, 1))
        << rrule;
}

void tst_ItemOccurrence::addOccurrenceDetail()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QDateTime endTime(QDate(QDate::currentDate().year() , 9, 30));
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
        
    // Fetch the saved item
    item = m_om->item(item.id());
    
    // Fetch the item again
    QList<QOrganizerItem> instanceList = m_om->itemOccurrences(item,startTime,endTime);
    
    QCOMPARE(instanceList.size(), 3);
    QOrganizerItem lastItem = instanceList.at(2);
    QCOMPARE(lastItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence thirdEvent = static_cast<QOrganizerEventOccurrence>(lastItem);
    QCOMPARE(thirdEvent.startDateTime(), QDateTime(QDate(QDate::currentDate().year() , 9, 15)));
    QCOMPARE(thirdEvent.id(), QOrganizerItemId());
    QCOMPARE(thirdEvent.parentId(), item.id());
    
    //Fetch instances using maxcount only.
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),2);
    QCOMPARE(instanceList.size(), 2);
    QOrganizerItem secondItem = instanceList.at(1);
    QCOMPARE(secondItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence secondEvent = static_cast<QOrganizerEventOccurrence>(secondItem);
    QCOMPARE(secondEvent.startDateTime(), QDateTime(QDate(QDate::currentDate().year() , 9, 8)));
    QCOMPARE(secondEvent.id(), QOrganizerItemId());
    QCOMPARE(secondEvent.parentId(), item.id());    
}

void tst_ItemOccurrence::fetchOccurrenceByFilterSort_data() 
{
    QStringList availableManagers = getAvailableManagers();    
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    QTest::addColumn<QString>("label");
    QTest::addColumn<QString>("modifiedLabel");
    
    QString label("label");
    QString modifiedLabel("newLabel");
    QString itemType = QOrganizerItemType::TypeEvent;
    
    foreach(QString manager, availableManagers) {
        QOrganizerRecurrenceRule rrule;
        rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        rrule.setLimit(3);
        QSet<Qt::DayOfWeek> daysOfWeek;
        daysOfWeek.insert(Qt::Wednesday);
        rrule.setDaysOfWeek(daysOfWeek);
        QTest::newRow(QString("[%1] weekly on Wednesday for 3 weeks").arg(manager).toLatin1().constData())
            << manager
            << itemType
            << QDateTime(QDate(QDate::currentDate().year() , 9, 1))
            << rrule
            << label
            << modifiedLabel;
    }    
}

/*!
 * TODO: to be refactored into several smaller test cases.
 */
void tst_ItemOccurrence::fetchOccurrenceByFilterSort()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);
    QFETCH(QString, label);
    QFETCH(QString, modifiedLabel);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));
    item.setDisplayLabel(label); 
    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
    
    //fetch instances and modify displaylabel for second and third instance
    QList<QOrganizerItem> instanceList;
    instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),10);
    QCOMPARE(instanceList.count(), 3);
    instanceList[1].setDisplayLabel(modifiedLabel);
    instanceList[2].setDisplayLabel(modifiedLabel);
    QVERIFY(m_om->saveItem(&instanceList[1]));
    QVERIFY(m_om->saveItem(&instanceList[2]));
    
    // Fetch itemoccurrence using sort and filter.
    /* filters */
    QOrganizerItemFilter f; // matches everything
    QOrganizerItemDetailFilter df;
    df.setDetailDefinitionName(QOrganizerItemDisplayLabel::DefinitionName,
                               QOrganizerItemDisplayLabel::FieldLabel);

    df.setMatchFlags(QOrganizerItemFilter::MatchExactly);     
    df.setValue(instanceList[1].detail(            
                QOrganizerItemDisplayLabel::DefinitionName).value(
                QOrganizerItemDisplayLabel::FieldLabel));
    
    QOrganizerItemSortOrder sortOrder;
    sortOrder.setDirection(Qt::DescendingOrder);
    sortOrder.setDetailDefinitionName(QOrganizerEventTime::DefinitionName,
                                     QOrganizerEventTime::FieldStartDateTime);
    QList<QOrganizerItemSortOrder> sortList;
    sortList.append(sortOrder);
    instanceList.clear();
    QOrganizerItemFetchHint fetchHint;
    instanceList = m_om->items(df,sortList,fetchHint);
    QCOMPARE(instanceList.size(), 2);
    
    QOrganizerItem firstItem = instanceList.at(0);
    QCOMPARE(firstItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(firstItem.detail<QOrganizerEventTime>().startDateTime(), QDateTime(QDate(QDate::currentDate().year(), 9, 15)));
    QCOMPARE(firstItem.displayLabel(), instanceList[1].displayLabel());

    //Search without filtering and sorting. Full instanceList is returned
    sortList.clear();    
    instanceList = m_om->items(f, sortList, fetchHint);
    QCOMPARE(instanceList.size(), 3);
    
    //Search full instance list in descending order without filtering
    sortList.append(sortOrder);
    instanceList = m_om->items(f, sortList, fetchHint);
    QCOMPARE(instanceList.size(), 3);
    QOrganizerItem thirdItem = instanceList.at(2);
    QCOMPARE(thirdItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(thirdItem.detail<QOrganizerEventTime>().startDateTime(), startTime);

    // Save another item with same attributes as first item
    QOrganizerItem secondItem;    
    secondItem.setType(itemType);
    QVERIFY(secondItem.saveDetail(&timeRange));
    secondItem.setDisplayLabel(modifiedLabel); 
    QVERIFY(secondItem.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&secondItem));

    instanceList = m_om->items(df, sortList, fetchHint);
    QCOMPARE(instanceList.size(), 5);
}

void tst_ItemOccurrence::addOccurrenceWithException_data()
{
    // Get the list of all available item managers
    QStringList availableManagers = getAvailableManagers();
   
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QDate>("rDate");
    QTest::addColumn<QDate>("exceptionDate");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    QTest::addColumn<QOrganizerItemPriority>("priority");
    QTest::addColumn<QOrganizerItemLocation>("location");
    QTest::addColumn<QString>("modifiedDescription");
    QTest::addColumn<QDateTime>("modifiedStartTime");

    
    QString itemType  = QOrganizerItemType::TypeEvent;
    QOrganizerItemPriority priority;
    priority.setPriority(QOrganizerItemPriority::HighestPriority);
    QOrganizerItemLocation location;
    location.setLabel("checkLocationName");
    QString description("checkoccurrence");
    QString newDescription("modifyOccurrence");
    
    foreach(QString manager, availableManagers) {
        QOrganizerRecurrenceRule rrule;
        rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
        rrule.setLimit(10);
        QDate rDate(QDate::currentDate().year() , 9, 11);
        QDate exceptionDate(QDate::currentDate().year() , 9, 3);
        QTest::newRow(QString("[%1] Daily event for 10 occurrences").arg(manager).toLatin1().constData())
            << manager
            << itemType
            << description
            << QDateTime(QDate(QDate::currentDate().year() , 9, 1),QTime(8,30))
            << rDate
            << exceptionDate
            << rrule
            << priority
            << location
            << newDescription
            << QDateTime(QDate(QDate::currentDate().year() , 8, 30));        
        
        QOrganizerRecurrenceRule monthRule;
        monthRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        monthRule.setLimit(5);

        QTest::newRow(QString("[%1] Monthly event for 5 occurrences").arg(manager).toLatin1().constData())
            << manager
            << itemType
            << description
            << QDateTime(QDate(QDate::currentDate().year() , 8, 3))
            << rDate
            << exceptionDate
            << monthRule
            << priority
            << location
            << newDescription
            << QDateTime(QDate(QDate::currentDate().year() , 8, 1));        
                    
        QOrganizerRecurrenceRule yearRule;
        yearRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        QSet<QOrganizerRecurrenceRule::Month> months;
        months.insert(QOrganizerRecurrenceRule::September);
        yearRule.setLimit(3);
        yearRule.setInterval(2);
        yearRule.setMonthsOfYear(months);
        QDate yearException(QDate(QDate::currentDate().year() , 9, 1));
        QTest::newRow(QString("[%1] yearly rule every other year").arg(manager).toLatin1().constData())
            << manager
            << itemType
            << description
            << QDateTime(QDate(QDate::currentDate().year() + 2, 9, 1))            
            << rDate
            << yearException
            << yearRule
            << priority
            << location
            << newDescription
            << QDateTime(QDate(QDate::currentDate().year() +2 , 9, 2));         
    }
}

void tst_ItemOccurrence::addOccurrenceWithException()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QString,description);
    QFETCH(QDateTime, startTime);
    QFETCH(QDate, rDate);
    QFETCH(QDate,exceptionDate );
    QFETCH(QOrganizerRecurrenceRule, rrule);
    QFETCH(QOrganizerItemPriority, priority);
    QFETCH(QOrganizerItemLocation, location);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    QSet<QDate> exceptionList;
    QSet<QDate> rDateList;
    rrules.insert(rrule);
    rDateList.insert(rDate);
    exceptionList.insert(exceptionDate);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    recurrence.setExceptionDates(exceptionList);
    recurrence.setRecurrenceDates(rDateList);
    QVERIFY(item.saveDetail(&recurrence));
    
    //Add other attributes to the item.
    item.setDescription(description);
    item.saveDetail(&priority);
    item.saveDetail(&location);
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));    
    item = m_om->item(item.id());
            
    //Fetch instance on the exception date.An empty list should be returned
    QList<QOrganizerItem> instanceList = m_om->itemOccurrences(item,QDateTime(exceptionDate),QDateTime(exceptionDate));
    QCOMPARE(instanceList.size(),0);

    //Fetch the instance on rdate should return one instance
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,QDateTime(rDate),QDateTime(rDate));
    QCOMPARE(instanceList.size(),1);
    QOrganizerItem rDateItem = instanceList.at(0);
    QCOMPARE(rDateItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence event = static_cast<QOrganizerEventOccurrence>(rDateItem);
    QCOMPARE(event.startDateTime(), QDateTime(QDate(QDate::currentDate().year() , 9, 11)));

    // Fetch the item again
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),20);
    QCOMPARE(instanceList.size(),rrule.limitCount());

    QOrganizerItem lastItem = instanceList.at(rrule.limitCount()-1);
    QCOMPARE(lastItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence lastEvent = static_cast<QOrganizerEventOccurrence>(lastItem);

    QCOMPARE(lastEvent.description(),item.description());
    QOrganizerItemPriority itemPriority = item.detail(QOrganizerItemPriority::DefinitionName);
    QCOMPARE(lastEvent.priority(),priority.priority());
    QOrganizerItemLocation itemLocation = item.detail(QOrganizerItemLocation::DefinitionName);
    QCOMPARE(lastEvent.location(),itemLocation.label());
    
    //Fetch instance on 1\9\2011.Since interval is 2 the insatnceList size should be 0.
    instanceList.clear();
    QDateTime intervalDate(QDate(QDate::currentDate().year()+ 1 , 9, 1));
    instanceList = m_om->itemOccurrences(item,intervalDate,intervalDate);
    QCOMPARE(instanceList.size(),0);
}

void tst_ItemOccurrence::editOccurrence_data() 
{
    addOccurrenceWithException_data();    
}

void tst_ItemOccurrence::editOccurrence() 
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QString,description);
    QFETCH(QDateTime, startTime);
    QFETCH(QDate, rDate);
    QFETCH(QDate,exceptionDate );
    QFETCH(QOrganizerRecurrenceRule, rrule);
    QFETCH(QOrganizerItemPriority, priority);
    QFETCH(QOrganizerItemLocation, location);
    QFETCH(QString,modifiedDescription);
    QFETCH(QDateTime, modifiedStartTime);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    
    //Add other attributes to the item.
    item.setDescription(description);
    item.saveDetail(&priority);
    item.saveDetail(&location);
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));    
    item = m_om->item(item.id());

    //Fetch first and third instance of the saved entry to modify
    QList<QOrganizerItem> instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),3);
    QCOMPARE(instanceList.size(), 3);
            
    QOrganizerItem firstItem = instanceList.at(0);
    QCOMPARE(firstItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence firstInstance = static_cast<QOrganizerEventOccurrence>(firstItem);
    QCOMPARE(firstItem.guid(), item.guid());
    
    QOrganizerItem thirdItem = instanceList.at(2);
    QCOMPARE(thirdItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence thirdInstance = static_cast<QOrganizerEventOccurrence>(thirdItem);
    
    //Modify the first instance and save it
    firstInstance.setDescription(modifiedDescription);
    QVERIFY(m_om->saveItem(&firstInstance));    
    
    //Modify the description of third instance and save it
    thirdInstance.setDescription(modifiedDescription);
    QVERIFY(m_om->saveItem(&thirdInstance));    
    
    //Fetch the first and third instances and verify
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),3);
    QCOMPARE(instanceList.size(),3);
    QOrganizerItem changedFirstItem = instanceList.at(0);
    QVERIFY(changedFirstItem.type() == QLatin1String(QOrganizerItemType::TypeEvent)
        || changedFirstItem.type() == QLatin1String(QOrganizerItemType::TypeEventOccurrence));

    // compare start datetime,description, priority and location
    QCOMPARE(changedFirstItem.detail<QOrganizerEventTime>().startDateTime(), firstInstance.startDateTime());
    QCOMPARE(changedFirstItem.description(), firstInstance.description());
    QCOMPARE(changedFirstItem.detail<QOrganizerItemPriority>().priority(), firstInstance.priority());
    QCOMPARE(changedFirstItem.detail<QOrganizerItemLocation>().label(), firstInstance.location());

    //Check for the modified third instance and verify
    QOrganizerItem modifiedThirdItem = instanceList.at(0);
    QVERIFY(modifiedThirdItem.type() == QLatin1String(QOrganizerItemType::TypeEvent)
        || modifiedThirdItem.type() == QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(modifiedThirdItem.description(), modifiedThirdItem.description());
    
   //Modify the start date of the first instance and verify
    firstInstance.setStartDateTime(modifiedStartTime);
    QVERIFY(m_om->saveItem(&firstInstance));
    
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item, modifiedStartTime, modifiedStartTime);
    QCOMPARE(instanceList.size(), 1);
    QCOMPARE(instanceList[0].detail<QOrganizerEventTime>().startDateTime(), firstInstance.startDateTime());
}

void tst_ItemOccurrence::editOccurrenceNegative_data() 
{
    // Get the list of all available item managers
    QStringList availableManagers = getAvailableManagers();
 
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    
    QString itemType = QOrganizerItemType::TypeEvent;
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(10);
    foreach(QString manager, availableManagers) {
        QTest::newRow(QString("[%1] daily repeating entry").arg(manager).toLatin1().constData())
            << manager
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }    
}

void tst_ItemOccurrence::editOccurrenceNegative() 
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);
     
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
     
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));    
    item = m_om->item(item.id());

    //Fetch first instance of the saved entry to modify
    QList<QOrganizerItem> instanceList = m_om->itemOccurrences(item,startTime,startTime);
    QCOMPARE(instanceList.size(),1);
    QOrganizerItem firstItem = instanceList.at(0);
    QCOMPARE(firstItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence firstInstance = static_cast<QOrganizerEventOccurrence>(firstItem);
    QString instanceGuid (firstInstance.guid());

    //Try to save instance with invalid guid and parentid fails
    // TODO: Disabled because of API change. REFACTOR!
    //firstInstance.setGuid(QString(""));
    //firstInstance.setParentId(QOrganizerItemId(-1));
    //QVERIFY(!m_om->saveItem(&firstInstance));
    //QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    //change to invalid original Date of the instance and save 
    firstInstance.setGuid(instanceGuid);
    firstInstance.setOriginalDate(QDate(1000,1,1));
    QVERIFY(!m_om->saveItem(&firstInstance));
    // Allow undefined error code, the engine should iterate through item instances to see if the
    // original date is valid to be able to give a specific error code
    QVERIFY(m_om->error() != QOrganizerManager::NoError);
    
    firstInstance = static_cast<QOrganizerEventOccurrence>(firstItem);
    firstInstance.setStartDateTime(startTime.addDays(-1));
    QVERIFY(m_om->saveItem(&firstInstance));
    
    //Save the instance with invalid localid
    // TODO: Disabled because of API change. REFACTOR!
    //QOrganizerItemId itemId;
    //itemId.setId(1);
    //firstInstance.setId(itemId);
    //QVERIFY(!m_om->saveItem(&firstInstance));
    //QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);
}

void tst_ItemOccurrence::updateOccurrenceId()
{
    // Create recurring event
    QOrganizerEvent christmas;
    christmas.setGuid("christmas");
    christmas.setStartDateTime(QDateTime(QDate(2010, 12, 25), QTime(0, 0, 0)));
    christmas.setEndDateTime(QDateTime(QDate(2010, 12, 26), QTime(0, 0, 0)));
    christmas.setDisplayLabel(QLatin1String("Christmas"));
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    christmas.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
    QVERIFY(m_om->saveItem(&christmas));

    // Save an exception
    QOrganizerEventOccurrence exception;
    exception.setOriginalDate(QDate(2010, 12, 25));
    exception.setStartDateTime(QDateTime(QDate(2010, 12, 25), QTime(0, 0, 0)));
    exception.setEndDateTime(QDateTime(QDate(2010, 12, 26), QTime(0, 0, 0)));
    exception.setDisplayLabel(QLatin1String("Xmass"));
    exception.setParentId(christmas.id());
    QVERIFY(m_om->saveItem(&exception));
    QVERIFY(exception.id() != QOrganizerItemId(0));

    // Modify the exception
    exception.setDisplayLabel(QLatin1String("Christmas"));
    exception.setParentId(QOrganizerItemId(0));
    QVERIFY(m_om->saveItem(&exception));
}

void tst_ItemOccurrence::fetchNegative_data()
{
    // Get the list of all available item managers
    QStringList availableManagers = getAvailableManagers();
    
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

void tst_ItemOccurrence::fetchNegative()
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
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch the saved item
    item = m_om->item(item.id());
        
    // Fetch the item instances for a non repeating entry
    instanceList = m_om->itemOccurrences(item,startTime,endTime);
    
    //Only a single instance should be returned for non-repeating entry
    QCOMPARE(instanceList.size(), 1);
    
    //Fetch instance when end period is less than start period
    QDateTime previousEndTime(startTime);
    previousEndTime.setDate(QDate(startTime.date().year() - 2, startTime.date().month(),startTime.date().day()));
    instanceList = m_om->itemOccurrences(item,startTime,previousEndTime);
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);
    
    //Fetch itemoccurrence for invalid itemtype eventoccurrence
    QOrganizerEventOccurrence invalidItem;
    instanceList = m_om->itemOccurrences(invalidItem,startTime,endTime);
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidItemTypeError);
    
    // Fetch the item instance with negative count
    instanceList = m_om->itemOccurrences(item, startTime, QDateTime(), -2);
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(instanceList.count(), 1);

    // Fetch the item instance with undefined starttime
    instanceList = m_om->itemOccurrences(item, QDateTime(), endTime);
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(instanceList.count(), 1);

    // Fetch the item instance with undefined endtime
    instanceList = m_om->itemOccurrences(item, startTime, QDateTime());
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(instanceList.count(), 1);
}

void tst_ItemOccurrence::daylightSavingTime()
{
    // Create a monthly event for one year 
    QOrganizerEvent event;
    event.setDisplayLabel("test daylight saving time");
    QDateTime startDateTime(QDate(2010, 1, 1));
    //startDateTime = startDateTime.addSecs(60*60);
    QDateTime endDateTime = startDateTime.addSecs(60*60);
    event.setStartDateTime(startDateTime);
    event.setEndDateTime(endDateTime);
    
    QOrganizerRecurrenceRule rule;
    rule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    rule.setLimit(QDate(2010, 12, 31));
    event.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rule);

    QVERIFY(m_om->saveItem(&event));

    //qDebug() << "event:";
    //qDebug() << "\tstart time utc   :" << event.startDateTime().toUTC();
    //qDebug() << "\tstart time local :" << event.startDateTime().toLocalTime();
    //qDebug() << "\tend time utc     :" << event.endDateTime().toUTC();
    //qDebug() << "\tend time local   :" << event.endDateTime().toLocalTime();

    // Verify that event occurs the same time every month
    QList<QOrganizerItem> events = m_om->itemOccurrences(event, event.startDateTime(), QDateTime(rule.limitDate()));
    QVERIFY(events.count() == 12);
    foreach(QOrganizerEventOccurrence occurence, events) {

        //qDebug() << "event occurrence:";
        //qDebug() << "\tstart time utc   :" << occurence.startDateTime().toUTC();
        //qDebug() << "\tstart time local :" << occurence.startDateTime().toLocalTime();
        //qDebug() << "\tend time utc     :" << occurence.endDateTime().toUTC();
        //qDebug() << "\tend time local   :" << occurence.endDateTime().toLocalTime();

        QVERIFY(occurence.startDateTime().toUTC().time() == startDateTime.toUTC().time());
        QVERIFY(occurence.endDateTime().toUTC().time() == endDateTime.toUTC().time());
        QVERIFY(occurence.startDateTime().toLocalTime().time() == startDateTime.toLocalTime().time());
        QVERIFY(occurence.endDateTime().toLocalTime().time() == endDateTime.toLocalTime().time());
        QVERIFY(occurence.startDateTime().toLocalTime().date().day() == startDateTime.toLocalTime().date().day());
        QVERIFY(occurence.endDateTime().toLocalTime().date().day() == endDateTime.toLocalTime().date().day());
    }

}

void tst_ItemOccurrence::leapYear()
{
    // Add a yearly recurring event on leap day
    QOrganizerEvent event;
    event.setDisplayLabel("test leap year");
    QDateTime startDateTime(QDate(2012, 2, 29), QTime(00,00), Qt::UTC);
    QDateTime endDateTime = startDateTime.addSecs(60*60);
    event.setStartDateTime(startDateTime);
    event.setEndDateTime(endDateTime);
    QOrganizerRecurrenceRule rule;
    rule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rule.setDaysOfMonth(QSet<int>() << 29);
    rule.setLimit(QDate(2020, 3, 1));
    event.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rule);
    QVERIFY(m_om->saveItem(&event));

    // Verify
    QList<QOrganizerItem> events = m_om->itemOccurrences(event, event.startDateTime(), QDateTime(rule.limitDate()));
    QVERIFY(events.count() == 9);
    foreach(QOrganizerEventOccurrence occurence, events) {
        if (occurence.startDateTime().date().year() % 4 == 0)
            QVERIFY(occurence.startDateTime().date().day() == 29); // leap year
        else
            QVERIFY(occurence.startDateTime().date().day() == 28); // normal year
    }
}

/*!
 * Helper function for adding available manager as test case data
 */
void tst_ItemOccurrence::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList availableManagers = getAvailableManagers();
    foreach(QString manager, availableManagers) {
        QTest::newRow(QString("[%1]").arg(manager).toLatin1().constData())
            << manager;
    }    
}

QTEST_MAIN(tst_ItemOccurrence);

#include "tst_itemoccurrence.moc"
