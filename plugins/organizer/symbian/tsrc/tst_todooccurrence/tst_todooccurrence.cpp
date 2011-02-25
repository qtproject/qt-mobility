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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <calcommon.h>
#include <calinstance.h>
#include <calsession.h>
#include <calchangecallback.h>
#include <calentryview.h>
#include <calinstanceview.h>

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

const QString managerNameSymbian("symbian");

Q_DECLARE_METATYPE(QOrganizerItemPriority)

class TestTodoOccurrence : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
	void addSimpleOccurrenceDetail_data();
	void addSimpleOccurrenceDetail();
    void addOccurrenceDetail_data();
    void addOccurrenceDetail();
    void editOccurrenceNegative_data();
    void editOccurrenceNegative();
	void addOccurrenceWithException_data();
	void addOccurrenceWithException();
	void editOccurrence_data();
	void editOccurrence();
	
    
private:
    void addSimpleOccurrenceData(QString itemType);
    void addOccurrencedata(QString itemType);
    
private:
    QOrganizerManager *m_om;
};

void TestTodoOccurrence::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerManager(managerNameSymbian);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds());
}

void TestTodoOccurrence::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestTodoOccurrence::addSimpleOccurrenceDetail_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QDateTime>("firstInstanceDueDateTime");
    QTest::addColumn<QDateTime>("secondInstanceDueDateTime");
    QTest::addColumn<QDateTime>("thirdInstanceDueDateTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    QTest::addColumn<int>("numberOfInstances");
    
    addSimpleOccurrenceData(QOrganizerItemType::TypeTodo);
}

void TestTodoOccurrence::addSimpleOccurrenceData(QString itemType)
{	
	QDateTime firstInstanceDueDateTime(QDate::currentDate());
	QDateTime secondInstanceDueDateTime(QDate::currentDate());
	QDateTime thirdInstanceDueDateTime(QDate::currentDate());
	
	QOrganizerRecurrenceRule dailyRrule;
	dailyRrule.setFrequency(QOrganizerRecurrenceRule::Daily);
	dailyRrule.setLimit(QDate::currentDate().addDays(4));
	dailyRrule.setInterval(2);
	secondInstanceDueDateTime.setDate(firstInstanceDueDateTime.date().addDays(2));
	thirdInstanceDueDateTime.setDate(firstInstanceDueDateTime.date().addDays(4));

	QTest::newRow("daily for 3 days alternatively")
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< dailyRrule
		<< 3; 
	
	QOrganizerRecurrenceRule weeklyRrule;
	weeklyRrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
	secondInstanceDueDateTime = firstInstanceDueDateTime.addDays(7);
	thirdInstanceDueDateTime = firstInstanceDueDateTime.addDays(14);
	weeklyRrule.setLimit(3);
	QSet<Qt::DayOfWeek> daysOfWeek;
	daysOfWeek << (Qt::DayOfWeek)QDate::currentDate().dayOfWeek();
	weeklyRrule.setDaysOfWeek(daysOfWeek);
	
	QTest::newRow("weekly on Wednesday for 3 weeks")
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< weeklyRrule
		<< 3;
	
	QOrganizerRecurrenceRule monthlyRrule;
	monthlyRrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
	secondInstanceDueDateTime = firstInstanceDueDateTime.addMonths(1);
	thirdInstanceDueDateTime = firstInstanceDueDateTime.addMonths(2);
	monthlyRrule.setLimit(3);
	QSet<int> days;
	days << QDate::currentDate().day();
	monthlyRrule.setDaysOfMonth(days);
	
	QTest::newRow("monthly for 3 months")
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< monthlyRrule
		<< 3;
	
	QOrganizerRecurrenceRule yearlyRrule;
	yearlyRrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
	secondInstanceDueDateTime = firstInstanceDueDateTime.addYears(1);
	thirdInstanceDueDateTime = firstInstanceDueDateTime.addYears(2);
	yearlyRrule.setLimit(3);

	QTest::newRow("yearly for 3 years")
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< yearlyRrule
		<< 3;
}

void TestTodoOccurrence::addSimpleOccurrenceDetail()
{
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QDateTime, firstInstanceDueDateTime);
    QFETCH(QDateTime, secondInstanceDueDateTime);
    QFETCH(QDateTime, thirdInstanceDueDateTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);
    QFETCH(int, numberOfInstances);

    // Set the item type
    QOrganizerTodo item;
    item.setStartDateTime(startTime);
    item.setDueDateTime(startTime);
   
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules << rrule;
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
   
    QVERIFY(item.saveDetail(&recurrence));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch the saved item
    item = m_om->item(item.id());
    
    QCOMPARE(item.type(), itemType);
    // Fetch the instances of the item.
    QList<QOrganizerItem> instanceList = 
                m_om->itemOccurrences(item,startTime,thirdInstanceDueDateTime);
    QCOMPARE(instanceList.size(), numberOfInstances);
    QOrganizerItem todoItem = instanceList.at(2);
    QCOMPARE(todoItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence thirdTodoOccurence = static_cast<QOrganizerTodoOccurrence>(todoItem);
    QCOMPARE(thirdTodoOccurence.dueDateTime(), thirdInstanceDueDateTime);
    QCOMPARE(thirdTodoOccurence.id(), QOrganizerItemId());
    QCOMPARE(thirdTodoOccurence.parentId(), item.id());
    
    // Fetch instances using maxcount only.
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),2);
    QCOMPARE(instanceList.size(), 2);
    todoItem = instanceList.at(0);
    QCOMPARE(todoItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence firstTodoOccurrence = static_cast<QOrganizerTodoOccurrence>(todoItem);
    QCOMPARE(firstTodoOccurrence.id(), QOrganizerItemId());
    QCOMPARE(firstTodoOccurrence.dueDateTime(), firstInstanceDueDateTime);
    QCOMPARE(firstTodoOccurrence.parentId(), item.id());   
    QCOMPARE(firstTodoOccurrence.status(), QOrganizerTodoProgress::StatusNotStarted);
    
    // Mark the third instance as done and check for the appropriate status.
    QOrganizerTodoProgress status;
    status.setStatus(QOrganizerTodoProgress::StatusComplete);
    status.setFinishedDateTime(thirdInstanceDueDateTime);
    thirdTodoOccurence.saveDetail(&status);
    QVERIFY(m_om->saveItem(&thirdTodoOccurence));
    instanceList.clear();
    instanceList = m_om->itemOccurrences
    		(item,startTime,thirdInstanceDueDateTime);
    todoItem = instanceList.at(2);
    thirdTodoOccurence = static_cast<QOrganizerTodoOccurrence>(todoItem);
    QCOMPARE(thirdTodoOccurence.status(), QOrganizerTodoProgress::StatusComplete);
}

void TestTodoOccurrence::addOccurrenceDetail_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    QTest::addColumn<QDateTime>("fourthInstanceDateTime");
    QTest::addColumn<QDateTime>("fifthInstanceDateTime");
    QTest::addColumn<QOrganizerItemPriority>("priority");
    
    addOccurrencedata(QOrganizerItemType::TypeTodo);
}

void TestTodoOccurrence::addOccurrencedata(QString itemType)
{	
	QDateTime fourthInstanceDateTime;
	QDateTime fifthInstanceDateTime;
	QOrganizerItemPriority priority;
	QSet<Qt::DayOfWeek> daysOfWeek;
	QSet<int> days;
	QSet<int> positions;
	
	// Repeating weekly on tuesday and thursday.
	QOrganizerRecurrenceRule weeklyRrule;
	weeklyRrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
	weeklyRrule.setLimit(6);
	daysOfWeek << Qt::Tuesday;
	daysOfWeek << Qt::Thursday;
	weeklyRrule.setDaysOfWeek(daysOfWeek);
	fourthInstanceDateTime.setDate(QDate(2010, 9, 16));
	fifthInstanceDateTime.setDate(QDate(2010, 9, 21));
	priority.setPriority(QOrganizerItemPriority::HighestPriority);

	QTest::newRow("weekly on monday and thursday for 3 weeks")
		<< itemType
		<< QDateTime(QDate(2010 , 9, 6))
		<< weeklyRrule
		<< fourthInstanceDateTime
		<< fifthInstanceDateTime
		<< priority;
	
	// Repeating for alternate weeks
	daysOfWeek.clear();
	weeklyRrule.setLimit(6);
	weeklyRrule.setInterval(2);
	fourthInstanceDateTime.setDate(QDate(2010, 9, 23));
	fifthInstanceDateTime.setDate(QDate(2010, 10, 5));
	priority.setPriority(QOrganizerItemPriority::LowestPriority);
	
	QTest::newRow("weekly on monday and thursday with interval 2")
		<< itemType
		<< QDateTime(QDate(2010 , 9, 6))
		<< weeklyRrule
		<< fourthInstanceDateTime
		<< fifthInstanceDateTime
		<< priority;
	
	// Monthly which repeats on every 6th and 22nd of the month.
	QOrganizerRecurrenceRule monthlyRrule;
	monthlyRrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
	monthlyRrule.setLimit(6);
	days << 6;
	days << 22;
	monthlyRrule.setDaysOfMonth(days);

	fourthInstanceDateTime.setDate(QDate(2010, 10,22));
	fifthInstanceDateTime.setDate(QDate(2010, 11, 6));

	QTest::newRow("monthly on monday and thursday")
		<< itemType
		<< QDateTime(QDate(2010 , 9, 6))
		<< monthlyRrule
		<< fourthInstanceDateTime
		<< fifthInstanceDateTime
		<< priority;
	
	// Monthly which repeats on every 2nd wednesday and thursday of the month.
	QOrganizerRecurrenceRule monthlyRruleUsingSetPosition;
	monthlyRruleUsingSetPosition.setFrequency(QOrganizerRecurrenceRule::Monthly);
	monthlyRruleUsingSetPosition.setLimit(6);
	positions << 2;
	daysOfWeek.clear();
	daysOfWeek << Qt::Wednesday;
	daysOfWeek << Qt::Thursday;
	monthlyRruleUsingSetPosition.setDaysOfWeek(daysOfWeek);
	monthlyRruleUsingSetPosition.setPositions(positions);
			
	fourthInstanceDateTime.setDate(QDate(2010, 10,14));
	fifthInstanceDateTime.setDate(QDate(2010, 11, 10));
	
	QTest::newRow("monthly on wednesday and thursday for 3 weeks")
		<< itemType
		<< QDateTime(QDate(2010 , 9, 6))
		<< monthlyRruleUsingSetPosition
		<< fourthInstanceDateTime
		<< fifthInstanceDateTime
		<< priority;
	
	// Yearly repeating on the month of september on 2nd tuesday of that month.
	QOrganizerRecurrenceRule yearlyRrule;
	yearlyRrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
	yearlyRrule.setLimit(6);
	QSet<QOrganizerRecurrenceRule::Month> months;
	months << QOrganizerRecurrenceRule::September;
	yearlyRrule.setMonthsOfYear(months);
	daysOfWeek.clear();
	daysOfWeek << Qt::Tuesday;
	yearlyRrule.setDaysOfWeek(daysOfWeek);
	positions.clear();
	positions << 2;
	yearlyRrule.setPositions(positions);

	fourthInstanceDateTime.setDate(QDate(2013, 9,10));
	fifthInstanceDateTime.setDate(QDate(2014, 9, 9));

	QTest::newRow("yearly on 2nd tuesday of every september")
		<< itemType
		<< QDateTime(QDate(2010 , 9, 6))
		<< yearlyRrule
		<< fourthInstanceDateTime
		<< fifthInstanceDateTime
		<< priority;
}

void TestTodoOccurrence::addOccurrenceDetail()
{
	QFETCH(QString, itemType);
	QFETCH(QDateTime, startTime);
	QFETCH(QOrganizerRecurrenceRule, rrule);
	QFETCH(QDateTime, fourthInstanceDateTime);
	QFETCH(QDateTime, fifthInstanceDateTime);
	QFETCH(QOrganizerItemPriority, priority);
	
	// Set the item type
	QOrganizerTodo item;
	QString displayLabel = "check display";
	item.setDisplayLabel(displayLabel);
	item.setStartDateTime(startTime);
	item.setDueDateTime(startTime);

	QSet<QOrganizerRecurrenceRule> rrules;
	rrules << rrule;
	QOrganizerItemRecurrence recurrence;
	recurrence.setRecurrenceRules(rrules);

	QVERIFY(item.saveDetail(&recurrence));
	QVERIFY(item.saveDetail(&priority));

	// Save item with recurrence rule.
	QVERIFY(m_om->saveItem(&item));

	// Fetch the saved item
	item = m_om->item(item.id());

	QCOMPARE(item.type(), itemType);
	QCOMPARE(item.displayLabel(), displayLabel);
	QOrganizerItemPriority todoPriority = item.detail(QOrganizerItemPriority::DefinitionName);
	QCOMPARE(todoPriority.priority(), priority.priority());
	// Fetch the instances of the item.
	QList<QOrganizerItem> instanceList = 
                        m_om->itemOccurrences(item,startTime,fifthInstanceDateTime.addDays(400));
	QCOMPARE(instanceList.size(), rrule.limitCount());
	QOrganizerItem todoItem = instanceList.at(4);
	QCOMPARE(todoItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
	QOrganizerTodoOccurrence fifthTodoOccurence = static_cast<QOrganizerTodoOccurrence>(todoItem);
	QCOMPARE(fifthTodoOccurence.startDateTime(), fifthInstanceDateTime);
	QCOMPARE(fifthTodoOccurence.dueDateTime(), fifthInstanceDateTime);
	QCOMPARE(fifthTodoOccurence.id(), QOrganizerItemId());
	QCOMPARE(fifthTodoOccurence.parentId(), item.id());
	
	todoItem = instanceList.at(3);
	QOrganizerTodoOccurrence fourthTodoOccurence = static_cast<QOrganizerTodoOccurrence>(todoItem);
	QCOMPARE(todoItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
	QCOMPARE(fourthTodoOccurence.dueDateTime(), fourthInstanceDateTime);
	QCOMPARE(fourthTodoOccurence.startDateTime(), fourthInstanceDateTime);
}

void TestTodoOccurrence::editOccurrenceNegative_data() 
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    
    QString itemType = QOrganizerItemType::TypeTodo;
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(10);

    QTest::newRow("daily repeating entry")
    << itemType
    << QDateTime::currentDateTime()
    << rrule;
}

void TestTodoOccurrence::editOccurrenceNegative() 
{
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);
     
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerTodoTime timeRange;
    timeRange.setStartDateTime(startTime);
    timeRange.setDueDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules << rrule;
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
     
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));    
    item = m_om->item(item.id());
    
    // Add comment to the item and save.
    QOrganizerItemComment comment;
    comment.setComment("check comment");
    // Comments are not supporeted.
    QVERIFY(item.saveDetail(&comment));  
    QVERIFY(!m_om->saveItem(&item));
    QVERIFY(m_om->error() == QOrganizerManager::InvalidDetailError);
    

    //Fetch first instance of the saved entry to modify
    QList<QOrganizerItem> instanceList = m_om->itemOccurrences(item,startTime,startTime);
    QCOMPARE(instanceList.size(),1);
    QOrganizerItem firstItem = instanceList.at(0);
    QCOMPARE(firstItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence firstInstance = static_cast<QOrganizerTodoOccurrence>(firstItem);
    QString instanceGuid (firstInstance.guid());
   
    //Try to save instance with invalid guid and parentId fails
    // TODO: Disabled because of API change. REFACTOR!
    //firstInstance.setGuid(QString(""));
    //firstInstance.setParentId(QOrganizerItemId(-1));
    //QVERIFY(!m_om->saveItem(&firstInstance));
    //QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);
    
    //change to invalid original Date of the instance and save 
    firstInstance.setGuid(instanceGuid);
    firstInstance.setOriginalDate(QDate(1000,1,1));
    QVERIFY(!m_om->saveItem(&firstInstance));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);
    
    //Save the instance with invalid id
    // TODO: Disabled because of API change. REFACTOR!
    //QOrganizerItemId itemId;
    //itemId.setId(1);
    //firstInstance.setId(itemId);
    //QVERIFY(!m_om->saveItem(&firstInstance));
    //QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);
}

void TestTodoOccurrence::addOccurrenceWithException_data()
{  
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QDate>("rDate");
    QTest::addColumn<QDate>("exceptionDate");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");
    QTest::addColumn<QOrganizerItemPriority>("priority");
    QTest::addColumn<QString>("modifiedDescription");
    QTest::addColumn<QDateTime>("modifiedStartTime");

    
    QString itemType  = QOrganizerItemType::TypeTodo;
    QOrganizerItemPriority priority;
    priority.setPriority(QOrganizerItemPriority::HighestPriority);
    QString description("checkoccurrence");
    QString newDescription("modifyOccurrence");
    
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(10);
    QDate rDate(QDate::currentDate().year() , 9, 11);
    QDate exceptionDate(QDate::currentDate().year() , 9, 3);
    QTest::newRow("Daily todo for 10 occurrences")
    << itemType
    << description
    << QDateTime(QDate(QDate::currentDate().year() , 9, 1),QTime(8,30))
    << rDate
    << exceptionDate
    << rrule
    << priority
    << newDescription
    << QDateTime(QDate(QDate::currentDate().year() , 8, 30));        

    QOrganizerRecurrenceRule monthRule;
    monthRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    monthRule.setLimit(5);

    QTest::newRow("Monthly todo for 5 occurrences")
    << itemType
    << description
    << QDateTime(QDate(QDate::currentDate().year() , 8, 3))
    << rDate
    << exceptionDate
    << monthRule
    << priority
    << newDescription
    << QDateTime(QDate(QDate::currentDate().year() , 8, 1));        

    QOrganizerRecurrenceRule yearRule;
    yearRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    QSet<QOrganizerRecurrenceRule::Month> months;
    months << QOrganizerRecurrenceRule::September;
    yearRule.setLimit(3);
    yearRule.setMonthsOfYear(months);
    QDate yearException(QDate(QDate::currentDate().year() , 9, 1));
    QTest::newRow("yearly rule every other year")
    << itemType
    << description
    << QDateTime(QDate(QDate::currentDate().year(), 9, 1))            
    << rDate
    << yearException
    << yearRule
    << priority
    << newDescription
    << QDateTime(QDate(QDate::currentDate().year(), 9, 2));         
}

void TestTodoOccurrence::addOccurrenceWithException()
{
    QFETCH(QString, itemType);
    QFETCH(QString,description);
    QFETCH(QDateTime, startTime);
    QFETCH(QDate, rDate);
    QFETCH(QDate,exceptionDate );
    QFETCH(QOrganizerRecurrenceRule, rrule);
    QFETCH(QOrganizerItemPriority, priority);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerTodoTime timeRange;
    timeRange.setStartDateTime(startTime);
    timeRange.setDueDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    QSet<QDate> exceptionList;
    QSet<QDate> rDateList;
    rrules << rrule;
    rDateList << rDate;
    exceptionList << exceptionDate;
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    recurrence.setExceptionDates(exceptionList);
    recurrence.setRecurrenceDates(rDateList);
    QVERIFY(item.saveDetail(&recurrence));
    
    //Add other attributes to the item.
    item.setDescription(description);
    item.saveDetail(&priority);
    
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
    QCOMPARE(rDateItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence todo = static_cast<QOrganizerTodoOccurrence>(rDateItem);
    QCOMPARE(todo.startDateTime(), QDateTime(QDate(QDate::currentDate().year() , 9, 11)));

    // Fetch the item again
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),20);
    QCOMPARE(instanceList.size(),rrule.limitCount());

    QOrganizerItem lastItem = instanceList.at(rrule.limitCount()-1);
    QCOMPARE(lastItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence lastTodo = static_cast<QOrganizerTodoOccurrence>(lastItem);

    QCOMPARE(lastTodo.description(),item.description());
    QOrganizerItemPriority itemPriority = item.detail(QOrganizerItemPriority::DefinitionName);
    QCOMPARE(lastTodo.priority(),priority.priority());
}

void TestTodoOccurrence::editOccurrence_data() 
{
    addOccurrenceWithException_data();    
}

void TestTodoOccurrence::editOccurrence() 
{
    QFETCH(QString, itemType);
    QFETCH(QString,description);
    QFETCH(QDateTime, startTime);
    QFETCH(QDate, rDate);
    QFETCH(QDate,exceptionDate );
    QFETCH(QOrganizerRecurrenceRule, rrule);
    QFETCH(QOrganizerItemPriority, priority);
    QFETCH(QString,modifiedDescription);
    QFETCH(QDateTime, modifiedStartTime);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerTodoTime timeRange;
    timeRange.setStartDateTime(startTime);
    timeRange.setDueDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules << rrule;
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    
    //Add other attributes to the item.
    item.setDescription(description);
    item.saveDetail(&priority);
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));    
    item = m_om->item(item.id());

    //Fetch first and third instance of the saved entry to modify
    QList<QOrganizerItem> instanceList = m_om->itemOccurrences(item,startTime,QDateTime(),3);
    QCOMPARE(instanceList.size(), 3);
            
    QOrganizerItem firstItem = instanceList.at(0);
    QCOMPARE(firstItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence firstInstance = static_cast<QOrganizerTodoOccurrence>(firstItem);
    QCOMPARE(firstItem.guid(), item.guid());
    
    QOrganizerItem thirdItem = instanceList.at(2);
    QCOMPARE(thirdItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence thirdInstance = static_cast<QOrganizerTodoOccurrence>(thirdItem);
    
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
    QCOMPARE(changedFirstItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    
    // compare start datetime,description, priority and location
    QOrganizerTodoOccurrence changedFirstInstance = static_cast<QOrganizerTodoOccurrence>(changedFirstItem);
    QCOMPARE(changedFirstInstance.startDateTime(),firstInstance.startDateTime());
    QCOMPARE(changedFirstInstance.description(),firstInstance.description());
    QCOMPARE(changedFirstInstance.priority(),firstInstance.priority());
    
    //Check for the modified third instance and verify
    QOrganizerItem modifiedThirdItem = instanceList.at(0);
    QCOMPARE(modifiedThirdItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence modifiedThirdInstance = static_cast<QOrganizerTodoOccurrence>(modifiedThirdItem);
    QCOMPARE(modifiedThirdInstance.description(),thirdInstance.description());
    
   //Modify the start date of the first instance and verify
    firstInstance.setStartDateTime(modifiedStartTime);
    firstInstance.setDueDateTime(modifiedStartTime);
    QVERIFY(m_om->saveItem(&firstInstance));
    
    instanceList.clear();
    instanceList = m_om->itemOccurrences(item,modifiedStartTime,modifiedStartTime);
    QCOMPARE(instanceList.size(),1);
    QOrganizerTodoOccurrence newInstance = static_cast<QOrganizerTodoOccurrence>(instanceList[0]);
    QCOMPARE(newInstance.startDateTime(),firstInstance.startDateTime());
}

QTEST_MAIN(TestTodoOccurrence);

#include "tst_todooccurrence.moc"
