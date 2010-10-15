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

const QString managerNameSymbian("symbian");

// We need to be able to pass QOrganizerRecurrenceRule as parameter from
// test data functions
//Q_DECLARE_METATYPE(QOrganizerRecurrenceRule) // already declared.

/*!
 * For testing recurrence rule implementation of organizer backends via
 * QOrganizerManager API. The target is  to implement test cases in a
 * platform independent manner so that this test module could be used with
 * every backends and potentially used as a QtMobility auto test with as little
 * porting work as possible.
 */
class tst_recurringItems : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void initTestCase();
    void init();
    void cleanup();

private slots:  // Test cases
    void addRecurrenceRule_data();
    void addRecurrenceRule();
    void removeRecurrenceRule_data();
    void removeRecurrenceRule();
    void exceptionDates_data(){addManagers();};
    void exceptionDates();

private: // util functions
    void addManagers();
    void addItemsWeeklyRecurrence(QString managerName, QString itemType);
    void addItemsDailyRecurrence(QString managerName, QString itemType);
    void addItemsMonthlyRecurrence(QString managerName, QString itemType);
    void addItemsYearlyRecurrence(QString managerName, QString itemType);
    bool verifyRecurrenceRule(
        QOrganizerRecurrenceRule expectedRRule,
        QOrganizerRecurrenceRule actualRRule);
    QOrganizerItem createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime = QDateTime());

private:
    QOrganizerManager *m_om;
};

void tst_recurringItems::initTestCase()
{
    // Remove all organizer items from available managers
    QStringList managerNames = QOrganizerManager::availableManagers();
    managerNames.removeAll("invalid");
    managerNames.removeAll("skeleton");
    managerNames.removeAll("memory");
    foreach (QString managerName, managerNames) {
        m_om = new QOrganizerManager(managerName);
        m_om->removeItems(m_om->itemIds());
    }
}

void tst_recurringItems::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds());
}

void tst_recurringItems::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds());
    delete m_om;
    m_om = 0;
}

void tst_recurringItems::addRecurrenceRule_data()
{
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("displayLabel");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerRecurrenceRule>("rrule");

    QStringList managerNames = QOrganizerManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    managerNames.removeAll("memory");

    foreach (QString managerName, managerNames) {
        addItemsWeeklyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsDailyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsYearlyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsMonthlyRecurrence(managerName, QOrganizerItemType::TypeEvent);
    }
}

void tst_recurringItems::addRecurrenceRule()
{
    // TODO: refactor to support dynamic addition of test cases
    QFETCH(QString, managerName);
    QFETCH(QString, displayLabel);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);

    // Create item
    QOrganizerItem item;
    item.setType(itemType);
    item.setDisplayLabel(displayLabel);
    QOrganizerEventTime timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));

    // Save
    QVERIFY(m_om->saveItem(&item));

    // Fetch and Verify
    item = m_om->item(item.id());

    QOrganizerItemRecurrence resultRecurrence = item.detail(QOrganizerItemRecurrence::DefinitionName);

    // The test case uses a single recurrence rule, verify the count
    QCOMPARE(resultRecurrence.recurrenceRules().count(), 1);

    // TODO: The following does not check for extra fields in the retrieved
    // recurrence rule
    // TODO: check also other details
    QVERIFY(verifyRecurrenceRule(rrule, resultRecurrence.recurrenceRules().toList().first()));
}

/*
 * A helper method for verifying recurrence rule fields one-by-one to make
 * debugging easier
 */
bool tst_recurringItems::verifyRecurrenceRule(
        QOrganizerRecurrenceRule expectedRRule,
        QOrganizerRecurrenceRule actualRRule)
{
    bool match(true);

    if (expectedRRule.frequency()!= actualRRule.frequency()) {
        match = false;
    }

    if (expectedRRule.limitCount() != actualRRule.limitCount()) {
        // Allow counts to be different in case the expected count is zero.
        // This exception is needed because symbian calendar server calculates
        // the count based on the end date in this case.
        if (expectedRRule.limitCount() > 0) {
            qDebug() << "expected count" << expectedRRule.limitCount();
            qDebug() << "actual count" << actualRRule.limitCount();
            match = false;
        }
    }

    if (expectedRRule.limitDate() != actualRRule.limitDate()) {
        // Verifying end date is not straightforward;
        // Allow the exceptional case with no end time and no count set (in
        // that case the end date is set to year 2100 by symbian calendar server)
        if (expectedRRule.limitCount() == 0
            && expectedRRule.limitDate().isNull()
            && actualRRule.limitDate() == QDate(2100, 12, 31)) {
            qWarning() << "End date was set to maximum value";
        // Symbian calendar server sets both count and end date during save,
        // but count has higher priority. In practice this means that setting
        // end date for an item results in an item with count set.
        } else if (actualRRule.limitCount() > 0
            && !actualRRule.limitDate().isValid()) {
            qWarning() << "End date expected, count found";
        /*
        // Symbian calendar server may have updated the end date to match
        // the actual date of the last item instance -> Allow end date that
        // is before the expected end date
        // Note: this rule is not needed in the verification, because end date
        // will never be set on symbian backend. The rule is here for future
        // reference. See the previous verification rule for more details on
        // why end date is not set.
        } else if (expectedRRule.limitDate().isValid()
            && actualRRule.limitDate().isValid()
            && actualRRule.limitDate() < expectedRRule.limitDate()) {
            qWarning() << "End date changed";
        }
        */
        } else {
            qDebug() << "expected end date" << expectedRRule.limitDate();
            qDebug() << "actual end date" << actualRRule.limitDate();
            match = false;
        }
    }

    if (expectedRRule.interval() != actualRRule.interval()) {
        qDebug() << "expected interval" << expectedRRule.interval();
        qDebug() << "actual interval" << actualRRule.interval();
        match = false;
    }

    if (expectedRRule.monthsOfYear() != actualRRule.monthsOfYear()) {
        qDebug() << "expected months" << expectedRRule.monthsOfYear();
        qDebug() << "actual months" << actualRRule.monthsOfYear();
        match = false;
    }

    if (expectedRRule.daysOfWeek() != actualRRule.daysOfWeek()) {
        // Allow days of week to be different in case the expected days of week
        // is empty. This exception is needed because symbian calendar server
        // does not allow weekly recurring items without days of week. So the
        // solution in symbian backend is to generate days of week based on the
        // start date in this case. The end result from Qt Organizer API client
        // point of view is that the days of week will appear during saving.
        if (expectedRRule.daysOfWeek().isEmpty()
            && actualRRule.daysOfWeek().size() == 1) {
            qWarning() << "Non-empty days-of-week";
        } else {
            qDebug() << "expected daysOfWeek" << expectedRRule.daysOfWeek();
            qDebug() << "actual daysOfWeek" << actualRRule.daysOfWeek();
            match = false;
        }
    }

    if (expectedRRule.daysOfMonth()!= actualRRule.daysOfMonth()) {
        // Allow the special case where giving an empty days-of-month for a
        // monthly recurring item results the start date to be added to
        // days-of-month. This is needed because symbian calendar server does
        // not allow a monthly recurrence without days-of-month or
        // days-of-week. 
        if (expectedRRule.frequency() == QOrganizerRecurrenceRule::Monthly
            && expectedRRule.daysOfMonth().isEmpty()
            && actualRRule.daysOfMonth().size() == 1) {
            qWarning() << "Non-empty days-of-month";
        } else {
            qDebug() << "expected daysOfMonth" << expectedRRule.daysOfMonth();
            qDebug() << "actual daysOfMonth" << actualRRule.daysOfMonth();
            match = false;
        }
    }

    if (expectedRRule.positions() != actualRRule.positions()) {
        QSet<int> expectedPositions = expectedRRule.positions();
        QSet<int> actualPositions = actualRRule.positions();

        // Allow different sortings, symbian calendar server does not preserve
        // the original sorting, it returns positions in ascending order
        // XXX TODO: check this.  QSet isn't ordered.
        //qSort(expectedPositions);
        //qSort(actualPositions);

        if (expectedPositions != actualPositions) {
            QSet<int> allPositions;
            allPositions << 1 << 2 << 3 << 4 << -1;
            //qSort(allPositions); XXX TODO: check this.  QSet isn't ordered.

            // Allow empty positions to be converted into "all positions", this
            // is needed because symbian calendar server does not allow storing
            // monthly recurring items with days-of-week but without positions.
            // So the work-around is to convert empty positions into "all
            // positions" as defined by RFC-2445. From Qt client point of view
            // this means that saving monthly recurring item with "days-of-week"
            // and empty positions will have the positions set to "all
            // positions".
            if (expectedPositions.isEmpty()
                && actualPositions == allPositions) {
                qWarning() << "Non-empty positions";
            } else {
                qDebug() << "expected positions" << expectedPositions;
                qDebug() << "actual positions" << actualPositions;
                match = false;
            }
        }
    }

    if (expectedRRule.daysOfYear()!= actualRRule.daysOfYear()) {
        qDebug() << "expected daysOfYear" << expectedRRule.daysOfYear();
        qDebug() << "actual daysOfYear" << actualRRule.daysOfYear();
        match = false;
    }

    if (expectedRRule.weeksOfYear()!= actualRRule.weeksOfYear()) {
        qDebug() << "expected weeksOfYear" << expectedRRule.weeksOfYear();
        qDebug() << "actual weeksOfYear" << actualRRule.weeksOfYear();
        match = false;
    }

    if (expectedRRule.firstDayOfWeek()!= actualRRule.firstDayOfWeek()) {
        qDebug() << "expected weekStart" << expectedRRule.firstDayOfWeek();
        qDebug() << "actual weekStart" << actualRRule.firstDayOfWeek();
        match = false;
    }

    return match;
}

void tst_recurringItems::removeRecurrenceRule_data()
{
    // Use the same recurrence rule data as the add test cases use
    addRecurrenceRule_data();
}

void tst_recurringItems::removeRecurrenceRule()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerRecurrenceRule, rrule);

    // Create an item
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
    QVERIFY(m_om->saveItem(&item));

    // Fetch the saved item & check that recurrence rule was saved
    item = m_om->item(item.id());
    QVERIFY(item.details(QOrganizerItemRecurrence::DefinitionName).count() == 1);

    // Remove a recurrence rule detail & save & verify it does not exist
    recurrence = item.detail<QOrganizerItemRecurrence>();
    item.removeDetail(&recurrence);
    QVERIFY(m_om->saveItem(&item));
    item = m_om->item(item.id());
    QVERIFY(item.details(QOrganizerItemRecurrence::DefinitionName).count() == 0);
    
    // Save the recurrence again & fetch & verify
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));
    item = m_om->item(item.id());
    QVERIFY(item.details(QOrganizerItemRecurrence::DefinitionName).count() == 1);
    
    // Set empty recurrence rule detail & save & verify it does not exist
    recurrence = item.detail<QOrganizerItemRecurrence>();
    recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>());
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));
    item = m_om->item(item.id());
    QVERIFY(item.details(QOrganizerItemRecurrence::DefinitionName).count() == 0);
}

void tst_recurringItems::exceptionDates()
{
    // Create item
    QOrganizerItem item = createItem(
        QOrganizerItemType::TypeEvent,
        "itemWExceptions",
        QDateTime(QDate(2010, 10, 1), QTime(10, 0, 0)));

    // Add recurring item "weekly for 10 weeks"
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(10);
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));

    // Verify
    QCOMPARE(m_om->items().count(), 10);
    QCOMPARE(m_om->itemOccurrences(item).count(), 10);

    // Delete the second instance; "weekly for 10 weeks, except on 2nd week"
    QSet<QDate> xdates;
    xdates.insert(QDate(2010, 10, 8));
    recurrence.setExceptionDates(xdates);
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));

    // Verify
    QCOMPARE(m_om->items().count(), 9);
    QCOMPARE(m_om->itemOccurrences(item).count(), 9);
    QCOMPARE(m_om->items().at(0).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(QOrganizerEventOccurrence(m_om->items().at(0)).startDateTime().date(), QDate(2010, 10, 1));
    QCOMPARE(m_om->items().at(1).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(QOrganizerEventOccurrence(m_om->items().at(1)).startDateTime().date(), QDate(2010, 10, 15));

    // Delete 1st instance; "weekly for 10 weeks, except on 1st and 2nd week"
    xdates.insert(QDate(2010, 10, 1));
    recurrence.setExceptionDates(xdates);
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));

    // Verify
    QCOMPARE(m_om->items().count(), 8);
    QCOMPARE(m_om->itemOccurrences(item).count(), 8);
    QCOMPARE(m_om->items().at(0).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(QOrganizerEventOccurrence(m_om->items().at(0)).startDateTime().date(), QDate(2010, 10, 15));
    QCOMPARE(m_om->items().at(1).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(QOrganizerEventOccurrence(m_om->items().at(1)).startDateTime().date(), QDate(2010, 10, 22));
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_recurringItems::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    managerNames.removeAll("memory");

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
    QOrganizerRecurrenceRule rrule;
    QSet<Qt::DayOfWeek> daysOfWeek;

    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    QTest::newRow(QString("[%1] weekly forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 0")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    if (managerName.contains(managerNameSymbian)) {
        // TODO: Symbian calendar server does not allow saving weekly entries
        // without "by day" data. To be studied if this could be implemented
        // on symbian with some work-around. But for now, let's just disable
        // the test case for symbian backend.
        qWarning("Symbian: Weekly recurrences without \"by day\" not supported");
    } else {
        rrule = QOrganizerRecurrenceRule(); // reset
        rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        rrule.setLimit(10);
        QTest::newRow(QString("[%1] weekly for 10 occurrences").arg(managerName).toLatin1().constData())
            << managerName
            << QString("weekly 1")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(3);
    daysOfWeek.clear(); // reset
    daysOfWeek.insert(Qt::Tuesday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] weekly on Tuesday for 3 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(5);
    daysOfWeek.clear(); // reset
    daysOfWeek.insert(Qt::Tuesday);
    daysOfWeek.insert(Qt::Thursday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setFirstDayOfWeek(Qt::Sunday);
    QTest::newRow(QString("[%1] weekly on Tuesday and Thursday for 5 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 3")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setInterval(2);
    daysOfWeek.clear(); // reset
    daysOfWeek.insert(Qt::Monday);
    daysOfWeek.insert(Qt::Wednesday);
    daysOfWeek.insert(Qt::Friday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setLimit(QDate(QDate::currentDate().year() , 12, 24));
    QTest::newRow(QString("[%1] Every other week on monday,wednesday and friday until 24th dec 2010").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 4")
        << itemType
        << QDateTime(QDate(QDate::currentDate().year() , 9, 1))
        << rrule;

    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    daysOfWeek.clear(); // reset
    daysOfWeek.insert(Qt::Tuesday);
    daysOfWeek.insert(Qt::Thursday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setLimit(QDate(QDate::currentDate().year() + 1, 1, 24));
    QTest::newRow(QString("[%1] weekly on Tuesday and Thursday until 12/24 of next year").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 5")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with monthly recurrences.
 */
void tst_recurringItems::addItemsMonthlyRecurrence(QString managerName, QString itemType)
{
    // On 15th day for 3 months
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    rrule.setLimit(3);
    QSet<int> daysOfMonth;
    daysOfMonth.insert(15);
    rrule.setDaysOfMonth(daysOfMonth);
    QTest::newRow(QString("[%1] monthly on 15th day for 3 months").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Monthly for 3 months
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    rrule.setLimit(3);
    QTest::newRow(QString("[%1] Monthly for 3 months").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Every other Month on first and last Sunday for 10 occurances
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    rrule.setLimit(10);
    rrule.setInterval(2);
    QSet<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.insert(Qt::Sunday);
    rrule.setDaysOfWeek(daysOfWeek);
    QSet<int> positions;
    positions.insert(1);
    positions.insert(-1);
    rrule.setPositions(positions);
    QTest::newRow(QString("[%1] Every other Month on first and last Sunday for 10 occurances").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Every third Month on first and third Sunday for 10 occurances
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    rrule.setLimit(10);
    rrule.setInterval(3);
    daysOfWeek.clear();
    daysOfWeek.insert(Qt::Sunday);
    rrule.setDaysOfWeek(daysOfWeek);
    positions.clear();
    positions.insert(1);
    positions.insert(3);
    rrule.setPositions(positions);
    QTest::newRow(QString("[%1] Every third Month on first and third Sunday for 10 occurances").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 3")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Every other month every tuesday
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    rrule.setInterval(2);
    daysOfWeek.clear();
    daysOfWeek.insert(Qt::Tuesday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Every other month every tuesday").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 4")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // The 2nd Sunday of the month for 3 months
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
    daysOfWeek.clear();
    daysOfWeek.insert(Qt::Sunday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setLimit(3);
    positions.clear();
    positions.insert(2);
    rrule.setPositions(positions);
    QTest::newRow(QString("[%1] the 2nd Sunday of the month for 3 months").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 5")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with daily recurrences.
 */
void tst_recurringItems::addItemsDailyRecurrence(QString managerName, QString itemType)
{
    // Daily for 3 days
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(3);
    QTest::newRow(QString("[%1] Daily for 3 days").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    if (managerName.contains(managerNameSymbian)) {
        // Symbian calendar server ignores week start for every other but weekly
        // recurring items, so the test case is disabled.
        qWarning("Symbian: Week start not supported for daily recurring item");
    } else {
        // Daily for 3 days, wk start on Sunday
        rrule = QOrganizerRecurrenceRule(); // reset
        rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
        rrule.setLimit(3);
        rrule.setFirstDayOfWeek(Qt::Sunday);
        QTest::newRow(QString("[%1] Daily for 3 days, wk start on Sunday").arg(managerName).toLatin1().constData())
            << managerName
            << QString("daily 1")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

    // Daily for 3 days, wk start on Monday
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(3);
    rrule.setFirstDayOfWeek(Qt::Monday);
    QTest::newRow(QString("[%1] Daily for 3 days, wk start on Monday").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Every second day for 5 occurrences
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(5);
    rrule.setInterval(2);
    QTest::newRow(QString("[%1] Every second day for 5 occurrences").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Every second day for four days
    rrule = QOrganizerRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setInterval(2);
    rrule.setLimit(QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day() + 4));
    QTest::newRow(QString("[%1] Every second day for four days").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 3")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    if (managerName.contains(managerNameSymbian)) {
        // Symbian TCalRRule does not support months for a daily recurring event
        // so the test case is disabled
        qWarning("Symbian: Daily recurring events do not support months");
    } else {
        // Every day in January for 3 years
        rrule = QOrganizerRecurrenceRule(); // reset
        rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
        rrule.setLimit(QDate(QDate::currentDate().year() + 3, 1, 31));
        QSet<QOrganizerRecurrenceRule::Month> oneMonth;
        oneMonth.insert(QOrganizerRecurrenceRule::January);
        rrule.setMonthsOfYear(oneMonth);
        QTest::newRow(QString("[%1] Every day in January for 3 years").arg(managerName).toLatin1().constData())
            << managerName
            << QString("daily 4")
            << itemType
            << QDateTime(QDate(QDate::currentDate().year(),1,1)).addSecs(3600)
            << rrule;
    }

    if (managerName.contains(managerNameSymbian)) {
        // Symbian TCalRRule does not support days for a daily recurrence, so the
        // test case is disabled
        qWarning("Symbian: Daily recurrences with \"days\" not supported");
    } else {
        // Note: This test case may not be valid, at least it does not seem
        // reasonable that a daily recurring event that has "days of week" set.
        // I.e. recurrence rule "daily, on Mondays" does not make much sense.
        rrule = QOrganizerRecurrenceRule(); // reset
        rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
        QSet<Qt::DayOfWeek> daysOfWeek;
        daysOfWeek.insert(Qt::Monday);
        rrule.setDaysOfWeek(daysOfWeek);
        QTest::newRow(QString("[%1] daily, days of week=Monday").arg(managerName).toLatin1().constData())
            << managerName
            << QString("daily 5")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }
}

void tst_recurringItems::addItemsYearlyRecurrence(QString managerName, QString itemType)
{
    // Every year
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    QTest::newRow(QString("[%1] yearly").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 0")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Every year for 3 occurances
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rrule.setLimit(3);
    QTest::newRow(QString("[%1] yearly for three years").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Yearly on first Monday of January for 2 occurrences
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rrule.setLimit(2);
    QSet<QOrganizerRecurrenceRule::Month> months;
    months.insert(QOrganizerRecurrenceRule::January);
    rrule.setMonthsOfYear(months);
    QSet<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.insert(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);
    QSet<int> positions;
    positions.insert(1);
    rrule.setPositions(positions);
    QTest::newRow(QString("[%1] yearly on first Monday of January").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    if (managerName.contains(managerNameSymbian)) {
        // Symbian recurrence rule supports only one position for a yearly
        // recurrence so the test case is disabled.
        qWarning("Symbian: only one position supported");
    } else {
        // Yearly on first and third Monday of January for 2 occurrences
        rrule = QOrganizerRecurrenceRule();
        rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        rrule.setLimit(2);
        months.clear();
        months.insert(QOrganizerRecurrenceRule::January);
        rrule.setMonthsOfYear(months);
        daysOfWeek.clear();
        daysOfWeek.insert(Qt::Monday);
        rrule.setDaysOfWeek(daysOfWeek);
        positions.clear();
        positions.insert(1);
        positions.insert(3);
        rrule.setPositions(positions);
        QTest::newRow(QString("[%1] yearly on first and third Monday of January").arg(managerName).toLatin1().constData())
            << managerName
            << QString("yearly 3")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

    if (managerName.contains(managerNameSymbian)) {
        // Symbian recurrence rule does not support yearly recurrence with days of
        // week defined but positions left empty, so the test case is disabled.
        // According to the RFC-2445 the default would be to use all positions if
        // the position is not available.
        qWarning("Symbian: yearly recurrences with days-of-week and without positions not supported");
    } else {
        // yearly on every Monday of January
        rrule = QOrganizerRecurrenceRule();
        rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        months = QSet<QOrganizerRecurrenceRule::Month>();
        months.insert(QOrganizerRecurrenceRule::January);
        rrule.setMonthsOfYear(months);
        daysOfWeek = QSet<Qt::DayOfWeek>();
        daysOfWeek.insert(Qt::Monday);
        rrule.setDaysOfWeek(daysOfWeek);
        QTest::newRow(QString("[%1] yearly on every Monday of January").arg(managerName).toLatin1().constData())
            << managerName
            << QString("yearly 4")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

    if (managerName.contains(managerNameSymbian)) {
        // Symbian calendar server does not support yearly events with
        // days-of-month so the test case is disabled
        qWarning("Symbian: yearly recurrences with days-of-month not supported");
    } else {
        // Yearly on 15th day of January
        rrule = QOrganizerRecurrenceRule();
        rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        rrule.setLimit(2);
        QSet<QOrganizerRecurrenceRule::Month> months;
        months.insert(QOrganizerRecurrenceRule::January);
        rrule.setMonthsOfYear(months);
        QSet<int> daysOfMonth;
        daysOfMonth.insert(15);
        rrule.setDaysOfMonth(daysOfMonth);
        QTest::newRow(QString("[%1] yearly on 15th day of January").arg(managerName).toLatin1().constData())
            << managerName
            << QString("yearly 5")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

/*  TODO: Enable the following test cases and implement them on symbian backend

    //Every year in january and march for 4 occurances
    // TODO: Symbian calendar server does not allow setting a yearly recurring
    // event with "months" set but without setting "day of month"
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rrule.setLimit(4);
    months = QSet<QOrganizerRecurrenceRule::Month>();
    months.insert(QOrganizerRecurrenceRule::January);
    months.insert(QOrganizerRecurrenceRule::March);
    rrule.setMonthsOfYear(months);
    QTest::newRow(QString("[%1] yearly on January and March").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 6")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every other year in january,feburary and march for 4 occurances
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rrule.setLimit(4);
    rrule.setInterval(2);
    months.clear();
    months.insert(QOrganizerRecurrenceRule::January);
    months.insert(QOrganizerRecurrenceRule::February);
    months.insert(QOrganizerRecurrenceRule::March);
    rrule.setMonthsOfYear(months);
    QTest::newRow(QString("[%1] every other year on January Feburary and March").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 7")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every 3rd year on 1st,100th and 200th day for 10 occurances
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rrule.setLimit(10);
    rrule.setInterval(3);
    QSet<int> daysOfYear;
    daysOfYear.insert(1);
    daysOfYear.insert(100);
    daysOfYear.insert(200);
    rrule.setDaysOfYear(daysOfYear);
    QTest::newRow(QString("[%1] every 3rd year on 1st,100th and 200th day for 10 occurances").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 8")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every 20th monday of the year,forever
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    rrule.setLimit(0);
    rrule.setInterval(1);
    daysOfWeek = QSet<Qt::DayOfWeek>();
    daysOfWeek.insert(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);
    QSet<int> pos;
    pos.insert(20);
    rrule.setPositions(pos);
    QTest::newRow(QString("[%1] Every 20th monday of the year,forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 9")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Monday of week number 20 forever
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    QSet<int> weekNumber;
    weekNumber.insert(20);
    rrule.setWeeksOfYear(weekNumber);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Monday of week number 20 forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 10")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every Monday in march forever
    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    QSet<QOrganizerRecurrenceRule::Month> oneMonth;
    oneMonth.insert(QOrganizerRecurrenceRule::March);
    rrule.setMonthsOfYear(oneMonth);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Every Monday in march forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 11")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    QSet<int> daysOfMonth;
    daysOfMonth.insert(1);
    rrule.setDaysOfMonth(daysOfMonth);
    rrule.setLimit(4);
    //Occurs every 1st day of the month for 4 occurances
    QTest::newRow(QString("[%1] Days of Month=1, Count=4").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 12")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerRecurrenceRule();
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    daysOfYear.clear();
    daysOfYear.insert(32);
    rrule.setDaysOfYear(daysOfYear);
    rrule.setLimit(1);
    QTest::newRow(QString("[%1] Days of Year=32, Count=1").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 13")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
*/
}

/*!
 * A helper method for creating a QOrganizerItem instance.
 */
QOrganizerItem tst_recurringItems::createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime)
{
    QOrganizerItem item;
    item.setType(itemType);
    item.setDisplayLabel(label);
    if (itemType == QOrganizerItemType::TypeTodo) {
        QOrganizerTodoTime timeRange;
        if (startTime.isValid())
            timeRange.setStartDateTime(startTime);
        if (endTime.isValid())
            timeRange.setDueDateTime(endTime);
        if (!timeRange.isEmpty())
            item.saveDetail(&timeRange);
    } else {
        QOrganizerEventTime timeRange;
        if (startTime.isValid())
            timeRange.setStartDateTime(startTime);
        if (endTime.isValid())
            timeRange.setEndDateTime(endTime);
        if (!timeRange.isEmpty())
            item.saveDetail(&timeRange);
    }
    return item;
}

QTEST_MAIN(tst_recurringItems);
#include "tst_recurringItems.moc"
