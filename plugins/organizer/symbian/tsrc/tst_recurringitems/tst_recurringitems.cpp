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

private: // util functions
    void addManagers();
    void addEventsWithRecurrenceRules(QString managerName);

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
    //m_om->removeItems(m_om->itemIds(), 0);
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
        addEventsWithRecurrenceRules(managerName);
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
    if (recurrence != item.detail(QOrganizerItemRecurrence::DefinitionName)) {
        qDebug() << recurrence;
        qDebug() << item.detail(QOrganizerItemRecurrence::DefinitionName);
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
 * organizer item events with recurrence data.
 */
void tst_recurringItems::addEventsWithRecurrenceRules(QString managerName)
{
    QOrganizerItemRecurrenceRule daysofweekrrule;
    QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Monday);
    daysofweekrrule.setDaysOfWeek(daysOfWeek);
    daysofweekrrule.setCount(3);

    QTest::newRow(QString("[%1] Days of week=Monday, Count=3").arg(managerName).toLatin1().constData())
        << managerName
        << QOrganizerItemType::TypeEvent.latin1()
        << QDateTime::currentDateTime().addSecs(3600)
        << daysofweekrrule;

    daysOfWeek.append(Qt::Sunday);
    daysofweekrrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Days of week=Monday;Sunday Count=3").arg(managerName).toLatin1().constData())
        << managerName
        << QOrganizerItemType::TypeEvent.latin1()
        << QDateTime::currentDateTime().addSecs(3600)
        << daysofweekrrule;

    /* TODO: enable the test cases and implement on symbian backend
    QOrganizerItemRecurrenceRule daysofmonthrrule;
    QList<int> daysOfMonth;
    daysOfMonth.append(1);
    daysofmonthrrule.setDaysOfMonth(daysOfMonth);
    daysofmonthrrule.setCount(4);

    QTest::newRow(QString("[%1] Days of Month=1, Count=4").arg(managerName).toLatin1().constData())
        << managerName
        << QOrganizerItemType::TypeEvent.latin1()
        << QDateTime::currentDateTime().addSecs(3600)
        << daysofmonthrrule;

    QOrganizerItemRecurrenceRule daysofyearrrule;
    QList<int> daysOfYear;
    daysOfYear.append(32);
    daysofyearrrule.setDaysOfYear(daysOfYear);
    daysofyearrrule.setCount(1);

    QTest::newRow(QString("[%1] Days of Year=32, Count=1").arg(managerName).toLatin1().constData())
        << managerName
        << QOrganizerItemType::TypeEvent.latin1()
        << QDateTime::currentDateTime().addSecs(3600)
        << daysofyearrrule;
        */
}

QTEST_MAIN(tst_recurringItems);
#include "tst_recurringItems.moc"
