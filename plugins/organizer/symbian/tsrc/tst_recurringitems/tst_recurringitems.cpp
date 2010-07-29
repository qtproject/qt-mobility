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

// We need to be able to pass QOrganizerItemRecurrenceRule as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItemRecurrenceRule)

/*!
 * For testing recurrence rule implementation of organizer backends via
 * QOrganizerItemManager API. The target is  to implement test cases in a
 * platform independent manner so that this test module could be used with
 * every backends and potentially used as a QtMobility auto test with as little
 * porting work as possible.
 */
class tst_recurringItems : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases
    void addRecurrenceRule_data();
    void addRecurrenceRule();
    void removeRecurrenceRule_data();
    void removeRecurrenceRule();
    
private: // util functions
    void addManagers();
    void addItemsWeeklyRecurrence(QString managerName, QString itemType);
    void addItemsDailyRecurrence(QString managerName, QString itemType);
    void addItemsMonthlyRecurrence(QString managerName, QString itemType);
    void addItemsYearlyRecurrence(QString managerName, QString itemType);
    
private:
    QOrganizerItemManager *m_om;
};


void tst_recurringItems::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
}

void tst_recurringItems::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
    delete m_om;
    m_om = 0;
}

void tst_recurringItems::addRecurrenceRule_data()
{
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerItemRecurrenceRule>("rrule");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend

    foreach (QString managerName, managerNames) {
        addItemsWeeklyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsDailyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        //addItemsYearlyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsMonthlyRecurrence(managerName, QOrganizerItemType::TypeEvent);
    }
}

void tst_recurringItems::addRecurrenceRule()
{
    // TODO: refactor to support dynamic addition of test cases
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemRecurrenceRule, rrule);

    // Create item
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));

    // Save
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch and Verify
    item = m_om->item(item.localId());
        
    QOrganizerItemRecurrence resultRecurrence = item.detail(QOrganizerItemRecurrence::DefinitionName);
    QList<QOrganizerItemRecurrenceRule> resultRules;
    QOrganizerItemRecurrenceRule fetchedRule;
    
    resultRules = resultRecurrence.recurrenceRules();
    if (resultRules.count()) {
        fetchedRule = resultRules[0];
    }
    bool itemMismatch(false);
    if (rrule.frequency()!= fetchedRule.frequency()) {
        itemMismatch = true;
    }
    if (rrule.count()&&(rrule.count()!= fetchedRule.count())) {
        itemMismatch = true;
    }
    if ((!rrule.endDate().isNull())&&(rrule.endDate()!= fetchedRule.endDate())) {
        if (!fetchedRule.count()) {
            itemMismatch = true;
        }   
    }
    if (rrule.interval()&&(rrule.interval()!= fetchedRule.interval())) {
        itemMismatch = true;
    }
    if (rrule.months().count()&&(rrule.months()!= fetchedRule.months())) {
        itemMismatch = true;
    }
    if (rrule.daysOfWeek() .count()&&(rrule.daysOfWeek()!= fetchedRule.daysOfWeek())) {
        itemMismatch = true;
    }
    if (rrule.daysOfMonth().count()&&(rrule.daysOfMonth()!= fetchedRule.daysOfMonth())) {
        itemMismatch = true;
    }
    if (rrule.positions().count()&&(rrule.positions()!= fetchedRule.positions())) {
        //calentry fetches positions in ascending order      
        QList<int>orginalPosition(rrule.positions());
        qSort(orginalPosition);
        if (orginalPosition!= fetchedRule.positions()) {
            itemMismatch = true;
        }   
    }
    if (rrule.daysOfYear().count()&&(rrule.daysOfYear()!= fetchedRule.daysOfYear())) {
        itemMismatch = true;
    }
    if (rrule.weeksOfYear().count()&&(rrule.weeksOfYear()!= fetchedRule.weeksOfYear())) {
        itemMismatch = true;
    }
    if (rrule.weekStart()&&(rrule.weekStart()!= fetchedRule.weekStart())) {
        itemMismatch = true;
    } 
    if (itemMismatch) {
        qDebug() << "Expected: " << recurrence;
        qDebug() << "Actual:   " << item.detail(QOrganizerItemRecurrence::DefinitionName);
        QFAIL("Recurrence items do not match!");
    }
}

void tst_recurringItems::removeRecurrenceRule_data()
{
    addRecurrenceRule_data();
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerItemRecurrenceRule>("rrule");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
         
    QOrganizerItemRecurrenceRule rrule;
    QString type = QOrganizerItemType::TypeEvent;
    
    foreach (QString managerName, managerNames) {                   
            //Test case for an organizer item with a empty rule                                                     
        QTest::newRow(QString("[%1] organizer item with a empty rule").arg(managerName).toLatin1().constData())
                << managerName
                << type
                << QDateTime::currentDateTime().addSecs(3600)
                << rrule;
    }
}

void tst_recurringItems::removeRecurrenceRule()
{    
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemRecurrenceRule, rrule);

    // Create an item
    QOrganizerItem item;
    item.setType(itemType);
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
    
    // Remove a recurrence rule and save the detail to the item.
    QOrganizerItemRecurrence chkRecurrence = item.detail<QOrganizerItemRecurrence>();
     
	// Set an empty list.    
    chkRecurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>());
    QVERIFY(item.saveDetail(&chkRecurrence));
    
    //Save the item again without the recurrence rule.
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch again and Verify
    QOrganizerItem resultItem;
    resultItem = m_om->item(item.localId());
    
    if (item.detail(QOrganizerItemRecurrence::DefinitionName)!= resultItem.detail(QOrganizerItemRecurrence::DefinitionName)) {
        qDebug() << "Expected: " << item.detail(QOrganizerItemRecurrence::DefinitionName);
        qDebug() << "Actual:   " << resultItem.detail(QOrganizerItemRecurrence::DefinitionName);
        QFAIL("Recurrence items do not match!");
    }
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_recurringItems::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    //managerNames.removeAll("memory");
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend

    foreach(QString mgr, managerNames) {
        QTest::newRow(QString("[%1]").arg(mgr).toLatin1().constData()) << mgr;
    }
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with weekly recurrences.
 */
void tst_recurringItems::addItemsWeeklyRecurrence(QString managerName, QString itemType)
{
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(10);

    if (!managerName.contains(m_managerNameSymbian)) {
        // TODO: Symbian calendar server does not allow saving weekly entries
        // without "by day" data. To be studied if this could be implemented
        // on symbian with some work-around. But for now, let's just disable
        // the test case for symbian backend.
        QTest::newRow(QString("[%1] weekly for 10 occurrences").arg(managerName).toLatin1().constData())
            << managerName
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

    rrule.setCount(3);
    QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Tuesday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] weekly on Tuesday for 3 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule.setCount(5);
    daysOfWeek.clear();
    daysOfWeek.append(Qt::Tuesday);
    daysOfWeek.append(Qt::Thursday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setWeekStart(Qt::Sunday);
    QTest::newRow(QString("[%1] weekly on Tuesday and Thursday for 5 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
    
    //Every other week on monday,wednesday and friday until 24th dec 2010 starting on september 1 2010
    rrule.setCount(0);
    rrule.setInterval(2);
    daysOfWeek.clear();
    daysOfWeek.append(Qt::Monday);
    daysOfWeek.append(Qt::Wednesday);
    daysOfWeek.append(Qt::Friday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setEndDate(QDate(QDate::currentDate().year() , 12, 24));
    QTest::newRow(QString("[%1] Every other week on monday,wednesday and friday until 24th dec 2010").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime(QDate(QDate::currentDate().year() , 9, 1))
        << rrule;

    if (!managerName.contains(m_managerNameSymbian)) {
        // TODO: fails on symbian backend because symbian calendar server sets
        // both count and end date for a saved item (otherwise the item is
        // saved ok)
        QOrganizerItemRecurrenceRule rrule2 = rrule;
        rrule2.setEndDate(QDate(QDate::currentDate().year() + 1, 1, 24));
        QTest::newRow(QString("[%1] weekly on Tuesday and Thursday until 12/24 of next year").arg(managerName).toLatin1().constData())
            << managerName
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule2;
    }
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with monthly recurrences.
 */
void tst_recurringItems::addItemsMonthlyRecurrence(QString managerName, QString itemType)
{
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
    QList<Qt::DayOfWeek> daysOfWeek;
        	
    rrule.setCount(3);
    QList<int> daysOfMonth;
    daysOfMonth.append(15);
    rrule.setDaysOfMonth(daysOfMonth);
    QTest::newRow(QString("[%1] monthly on 15th day for 3 months").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    if (!managerName.contains(m_managerNameSymbian)) {
    // Symbian recurrence rule does not support positions so the test case
    // is disabled
    
    //Every other Month on first and last Sunday for 10 occurances
    QOrganizerItemRecurrenceRule rrule1;
    rrule1.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
    rrule1.setCount(10);
    rrule1.setInterval(2);
    daysOfWeek.append(Qt::Sunday);
    rrule1.setDaysOfWeek(daysOfWeek);
    QList<int> positions;
    positions.append(1);
    positions.append(-1);
    rrule1.setPositions(positions);
    
    QTest::newRow(QString("[%1] Every other Month on first and last Sunday for 10 occurances").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule1;

    //Every other month every tuesday
    QOrganizerItemRecurrenceRule rrule2;
    rrule2.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
    rrule2.setInterval(2);
    daysOfWeek.clear();
    daysOfWeek.append(Qt::Tuesday);
    rrule2.setDaysOfWeek(daysOfWeek);
      
    QTest::newRow(QString("[%1] Every other month every tuesday").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule2;

        rrule.setDaysOfMonth(QList<int>()); // clear days of month
        QList<Qt::DayOfWeek> daysOfWeek;
        daysOfWeek.append(Qt::Sunday);
        rrule.setDaysOfWeek(daysOfWeek);
        rrule.setCount(3);
        positions.clear();
        positions.append(1);
        rrule.setPositions(positions);
        QTest::newRow(QString("[%1] the 1st Sunday of the month for 3 months").arg(managerName).toLatin1().constData())
            << managerName
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with daily recurrences.
 */
void tst_recurringItems::addItemsDailyRecurrence(QString managerName, QString itemType)
{
    /* TODO: enable the test cases and implement on symbian backend */
    QOrganizerItemRecurrenceRule rrule;
    rrule.setCount(3);
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Daily);

    QTest::newRow(QString("[%1] daily, Count=3").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
    
    //occurance with interval
    rrule.setCount(5);
    rrule.setInterval(2);

    QTest::newRow(QString("[%1] daily, Count=5 Interval = 2").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
     
    rrule.setCount(0);
    rrule.setInterval(2);
    rrule.setEndDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day()+4));
    
    QTest::newRow(QString("[%1] daily, End Date Interval = 2").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
    
    //Every day in January for 3 years
    /*rrule.setCount(0);
    rrule.setInterval(1);
    rrule.setEndDate(QDate(QDate::currentDate().year()+3,1,31));
    QList<QOrganizerItemRecurrenceRule::Month> oneMonth;
    oneMonth.append(QOrganizerItemRecurrenceRule::January);
    rrule.setMonths(oneMonth);
    QTest::newRow(QString("[%1] Every day in January for 3 years").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime(QDate(QDate::currentDate().year(),1,1)).addSecs(3600)
        << rrule;*/
    
    // TODO: should this fail? (daily recurring event that has "days of week" set..?)
    /*QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);

    QTest::newRow(QString("[%1] daily, days of week=Monday;Sunday, Count=3").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
        */
}

void tst_recurringItems::addItemsYearlyRecurrence(QString managerName, QString itemType)
{
    /* TODO: enable and implement*/
    //Every year in january and march for 4 occurances
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(4);
    QList<QOrganizerItemRecurrenceRule::Month> months;
    months.append(QOrganizerItemRecurrenceRule::January);
    months.append(QOrganizerItemRecurrenceRule::March);
    rrule.setMonths(months);
    QTest::newRow(QString("[%1] yearly on January and March").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
      
    //Every other year in january,feburary and march for 4 occurances
    rrule.setCount(4);
    rrule.setInterval(2);
    months.clear();
    months.append(QOrganizerItemRecurrenceRule::January);
    months.append(QOrganizerItemRecurrenceRule::February);
    months.append(QOrganizerItemRecurrenceRule::March);
    rrule.setMonths(months);
    QTest::newRow(QString("[%1] every other year on January Feburary and March").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
        
    //Every 3rd year on 1st,100th and 200th day for 10 occurances
    QOrganizerItemRecurrenceRule dayOfYearRrule;
    dayOfYearRrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    dayOfYearRrule.setCount(10);
    dayOfYearRrule.setInterval(3);
    QList<int> daysOfYear;
    daysOfYear.append(1);
    daysOfYear.append(100);
    daysOfYear.append(200);
    dayOfYearRrule.setDaysOfYear(daysOfYear);
    QTest::newRow(QString("[%1] every 3rd year on 1st,100th and 200th day for 10 occurances").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << dayOfYearRrule;
            
        
     //Every 20th monday of the year,forever
    dayOfYearRrule.setCount(0);
    dayOfYearRrule.setInterval(1);
    QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Monday);
    dayOfYearRrule.setDaysOfWeek(daysOfWeek);
    QList<int> pos;
    pos.append(20);
    dayOfYearRrule.setPositions(pos);
    QTest::newRow(QString("[%1] Every 20th monday of the year,forever").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << dayOfYearRrule;

    //Monday of week number 20 forever    
    QOrganizerItemRecurrenceRule weekNumberRrule;
    weekNumberRrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    QList<int> weekNumber;
    weekNumber.append(20);
    weekNumberRrule.setWeeksOfYear(weekNumber);
    weekNumberRrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Monday of week number 20 forever").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << weekNumberRrule;
    
    //Every Monday in march forever   
    QOrganizerItemRecurrenceRule rrule2;
    rrule2.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    QList<QOrganizerItemRecurrenceRule::Month> oneMonth;
    oneMonth.append(QOrganizerItemRecurrenceRule::March);
    rrule2.setMonths(oneMonth);
    rrule2.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Every Monday in march forever").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule2;
    
    /* TODO: enable the test cases and implement on symbian backend*/
    QOrganizerItemRecurrenceRule daysofmonthrrule;
    daysofmonthrrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    QList<int> daysOfMonth;
    daysOfMonth.append(1);
    daysofmonthrrule.setDaysOfMonth(daysOfMonth);
    daysofmonthrrule.setCount(4);
    //Occurs every 1st day of the month for 4 occurances
    QTest::newRow(QString("[%1] Days of Month=1, Count=4").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << daysofmonthrrule;

    QOrganizerItemRecurrenceRule daysofyearrrule;
    daysOfYear.clear();
    daysOfYear.append(32);
    daysofyearrrule.setDaysOfYear(daysOfYear);
    daysofyearrrule.setCount(1);
    QTest::newRow(QString("[%1] Days of Year=32, Count=1").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << daysofyearrrule;
    
}

QTEST_MAIN(tst_recurringItems);
#include "tst_recurringItems.moc"
