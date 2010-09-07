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

Q_DECLARE_METATYPE(QOrganizerItemRecurrenceRule)
Q_DECLARE_METATYPE(QOrganizerItemPriority)

class TestTodoOccurrence : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
    void addOccurrenceDetail_data();
    void addOccurrenceDetail();
private:
    QStringList getAvailableManagers();
    void addOccurrenceData(QString managerName, QString itemType);
    
private:
    QOrganizerItemManager *m_om;
    
};

void TestTodoOccurrence::init()
{
    QFETCH(QString, managerName);
    
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerNameSymbian);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
}

void TestTodoOccurrence::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

QStringList TestTodoOccurrence::getAvailableManagers()
{
    // Get the list of all available item managers
    QStringList availableManagers = QOrganizerItemManager::availableManagers();
    
    // Remove these since test would fail
    availableManagers.removeAll("invalid");
    availableManagers.removeAll("skeleton");
    availableManagers.removeAll("memory");
    
    return availableManagers;
}

void TestTodoOccurrence::addOccurrenceDetail_data()
{
    QStringList availableManagers = getAvailableManagers();    
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QDateTime>("firstInstanceDueDateTime");
    QTest::addColumn<QDateTime>("secondInstanceDueDateTime");
    QTest::addColumn<QDateTime>("thirdInstanceDueDateTime");
    QTest::addColumn<QOrganizerItemRecurrenceRule>("rrule");
    QTest::addColumn<int>("numberOfInstances");
    
    foreach(QString manager, availableManagers) {
        addOccurrenceData(manager, QOrganizerItemType::TypeTodo);
    }
}

void TestTodoOccurrence::addOccurrenceData(QString managerName, QString itemType)
{	
	QDateTime firstInstanceDueDateTime(QDate::currentDate());
	QDateTime secondInstanceDueDateTime(QDate::currentDate());
	QDateTime thirdInstanceDueDateTime(QDate::currentDate());
	
	QOrganizerItemRecurrenceRule dailyRrule;
	dailyRrule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
	dailyRrule.setEndDate(QDate::currentDate().addDays(4));
	dailyRrule.setInterval(2);
	secondInstanceDueDateTime.setDate(firstInstanceDueDateTime.date().addDays(2));
	thirdInstanceDueDateTime.setDate(firstInstanceDueDateTime.date().addDays(4));

	QTest::newRow(QString("[%1] daily for 3 days alternatively").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< dailyRrule
		<< 3; 
	
	QOrganizerItemRecurrenceRule weeklyRrule;
	weeklyRrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
	secondInstanceDueDateTime = firstInstanceDueDateTime.addDays(7);
	thirdInstanceDueDateTime = firstInstanceDueDateTime.addDays(14);
	weeklyRrule.setCount(3);
	QList<Qt::DayOfWeek> daysOfWeek;
	daysOfWeek.append((Qt::DayOfWeek)QDate::currentDate().dayOfWeek());
	weeklyRrule.setDaysOfWeek(daysOfWeek);
	
	QTest::newRow(QString("[%1] weekly on Wednesday for 3 weeks").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< weeklyRrule
		<< 3;
	
	QOrganizerItemRecurrenceRule monthlyRrule;
	monthlyRrule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
	secondInstanceDueDateTime = firstInstanceDueDateTime.addMonths(1);
	thirdInstanceDueDateTime = firstInstanceDueDateTime.addMonths(2);
	monthlyRrule.setCount(3);
	QList<int> days;
	days.append(QDate::currentDate().day());
	monthlyRrule.setDaysOfMonth(days);
	
	QTest::newRow(QString("[%1] monthly for 3 months").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< monthlyRrule
		<< 3;
	
	QOrganizerItemRecurrenceRule yearlyRrule;
	yearlyRrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
	secondInstanceDueDateTime = firstInstanceDueDateTime.addYears(1);
	thirdInstanceDueDateTime = firstInstanceDueDateTime.addYears(2);
	yearlyRrule.setCount(3);

	QTest::newRow(QString("[%1] yearly for 3 years").arg(managerName).toLatin1().constData())
		<< managerName
		<< itemType
		<< QDateTime(QDate::currentDate())
		<< firstInstanceDueDateTime
		<< secondInstanceDueDateTime
		<< thirdInstanceDueDateTime
		<< yearlyRrule
		<< 3;
}

void TestTodoOccurrence::addOccurrenceDetail()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QDateTime, firstInstanceDueDateTime);
    QFETCH(QDateTime, secondInstanceDueDateTime);
    QFETCH(QDateTime, thirdInstanceDueDateTime);
    QFETCH(QOrganizerItemRecurrenceRule, rrule);
    QFETCH(int, numberOfInstances);

    // Set the item type
    QOrganizerTodo item;
    item.setStartDateTime(startTime);
    item.setDueDateTime(startTime);
   
    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
   
    QVERIFY(item.saveDetail(&recurrence));
    
    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch the saved item
    item = m_om->item(item.localId());
    
    QCOMPARE(item.type(), itemType);
    // Fetch the instances of the item.
    QList<QOrganizerItem> instanceList = 
    		m_om->itemInstances(item,startTime,thirdInstanceDueDateTime);
    QCOMPARE(instanceList.size(), numberOfInstances);
    QOrganizerItem todoItem = instanceList.at(2);
    QCOMPARE(todoItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence thirdTodoOccurence = static_cast<QOrganizerTodoOccurrence>(todoItem);
    QCOMPARE(thirdTodoOccurence.dueDateTime(), thirdInstanceDueDateTime);
    QCOMPARE(thirdTodoOccurence.localId(), (unsigned int)0);
    QCOMPARE(thirdTodoOccurence.parentLocalId(), item.localId());
    
    // Fetch instances using maxcount only.
    instanceList.clear();
    instanceList = m_om->itemInstances(item,startTime,QDateTime(),2);
    QCOMPARE(instanceList.size(), 2);
    todoItem = instanceList.at(0);
    QCOMPARE(todoItem.type(), QLatin1String(QOrganizerItemType::TypeTodoOccurrence));
    QOrganizerTodoOccurrence firstTodoOccurrence = static_cast<QOrganizerTodoOccurrence>(todoItem);
    QCOMPARE(firstTodoOccurrence.localId(), (unsigned int)0);
    QCOMPARE(firstTodoOccurrence.dueDateTime(), firstInstanceDueDateTime);
    QCOMPARE(firstTodoOccurrence.parentLocalId(), item.localId());   
    QCOMPARE(firstTodoOccurrence.status(), QOrganizerTodoProgress::StatusNotStarted);
    
    // Mark the third instance as done and check for the appropriate status.
    QOrganizerTodoProgress status;
    status.setStatus(QOrganizerTodoProgress::StatusComplete);
    status.setFinishedDateTime(thirdInstanceDueDateTime);
    thirdTodoOccurence.saveDetail(&status);
    QVERIFY(m_om->saveItem(&thirdTodoOccurence));
    instanceList.clear();
    instanceList = m_om->itemInstances
    		(item,startTime,thirdInstanceDueDateTime);
    todoItem = instanceList.at(2);
    thirdTodoOccurence = static_cast<QOrganizerTodoOccurrence>(todoItem);
    QCOMPARE(thirdTodoOccurence.status(), QOrganizerTodoProgress::StatusComplete);
}
QTEST_MAIN(TestTodoOccurrence);

#include "tst_todooccurrence.moc"
