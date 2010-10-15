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

QTM_USE_NAMESPACE

const QString managerName("maemo5");

class tst_Maemo5Om : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases
    void addSimpleItem();
    void removeSimpleItem();

    void addEvent();
    void addTodo();
    void addJournal();

    void addEventWithRecurrence();
    void addEventExceptions();
    void addEventExceptionWithGuid();
    void addWithIllegalParameters();

    void removeEventWithRecurrence();
    void removeEventExceptions();

    void getItemIds();
    void getItems();
    void getItemOccurrences();
    void setRecurrenceDates();

    //void getCollectionIds(); // this function is no longer provided by the API.
    void getCollections();
    void saveCollection();
    void removeCollection();
    void saveItemsToNewCollection();

    void asynchronousSaveAndFetch();
    void consecutiveAsynchronousRequests();
    void deleteRequest();

    void testReminders();

private:
    QOrganizerManager *m_om;
};


void tst_Maemo5Om::init()
{
    m_om = new QOrganizerManager(managerName);
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds());
}

void tst_Maemo5Om::cleanup()
{
    delete m_om;
}

void tst_Maemo5Om::addSimpleItem()
{
    QDate startDate1(2010, 8, 3);
    QDate endDate1(startDate1);
    QTime startTime1(10, 0, 0);
    QTime endTime1(11, 30, 0);

    QDate startDate2(2010, 8, 10);
    QDate endDate2(startDate2);
    QTime startTime2(12, 30, 0);
    QTime endTime2(13, 0, 28);

    QDate startDate3(2010, 8, 15);
    QDate endDate3(startDate3);
    QTime startTime3(12, 30, 0);
    QTime endTime3(13, 0, 28);

    // Create a simple event
    QOrganizerEvent event1;
    event1.setStartDateTime(QDateTime(startDate1, startTime1));
    event1.setEndDateTime(QDateTime(endDate1, endTime1));
    event1.setDisplayLabel("addSimpleItem");
    event1.setDescription("Simple test event description");

    // Save
    QVERIFY(m_om->saveItem(&event1));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event1.id().isNull());
    QVERIFY(event1.id().managerUri().contains(managerName));
    QVERIFY(!event1.guid().isEmpty());

    // Create another event
    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(startDate2, startTime2));
    event2.setEndDateTime(QDateTime(endDate2, endTime2));
    event2.setDisplayLabel("addSimpleItem, save with list parameter");
    event2.setDescription("Save with list parameter");

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(event2);
    QVERIFY(m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    foreach (QOrganizerItem item, items) {
        QVERIFY(!item.id().isNull());
        QVERIFY(item.id().managerUri().contains(managerName));
        QVERIFY(!item.guid().isEmpty());
    }

    // Create still one event
    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(startDate3, startTime3));
    event3.setEndDateTime(QDateTime(endDate3, endTime3));
    event3.setDisplayLabel("addSimpleItem, save with list parameter and error map");
    event3.setDescription("Save with list parameter and error map");

    // Save with list parameter and error map parameter
    QList<QOrganizerItem> items2;
    items2.append(event3);
    QVERIFY(m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(m_om->errorMap().count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY(!item2.id().isNull());
        QVERIFY(item2.id().managerUri().contains(managerName));
        QVERIFY(!item2.guid().isEmpty());
    }
}

void tst_Maemo5Om::removeSimpleItem()
{
    QDate startDate(2010, 8, 23);
    QDate endDate(startDate);
    QTime startTime(10, 0, 0);
    QTime endTime(11, 30, 0);

    // Create a simple event
    QOrganizerEvent event;
    event.setStartDateTime(QDateTime(startDate, startTime));
    event.setEndDateTime(QDateTime(endDate, endTime));
    event.setDisplayLabel("removeSimpleItem");
    event.setDescription("Simple test event description");

    // Save
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Remove
    QVERIFY(m_om->removeItem(event.id()));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);

    // Try to fetch back, should fail
    QOrganizerItem fetchItem = m_om->item(event.id());
    QVERIFY(fetchItem == QOrganizerItem());

    // Remove list
    QDateTime todoDueDateTime2(QDate(2010,8,30), QTime(15, 0, 0));
    QDateTime todoDueDateTime3(QDate(2010,8,31), QTime(13, 0, 0));
    QOrganizerTodo item2;
    item2.setDueDateTime( todoDueDateTime2 );
    QOrganizerTodo item3;
    item3.setDueDateTime( todoDueDateTime3 );
    QVERIFY(m_om->saveItem(&item2));
    QVERIFY(m_om->saveItem(&item3));
    QList<QOrganizerItemId> itemIds;
    itemIds.append(item2.id());
    itemIds.append(item3.id());
    QVERIFY(m_om->removeItems(itemIds));
    QVERIFY(m_om->errorMap().count() == 0);
}

void tst_Maemo5Om::addEvent() {
    // Create event and set details
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010,6,15), QTime(11, 13, 2)));
    event.setEndDateTime(QDateTime(QDate(2010,6,17),QTime(2,0,0)));
    event.setDisplayLabel("addEvent");
    event.setDescription("Test event description");
    event.setGuid("custom event GUID");
    event.setPriority(QOrganizerItemPriority::HighPriority);

    QOrganizerItemLocation loc = event.detail<QOrganizerItemLocation>();
    loc.setLatitude(45.0);
    loc.setLongitude(-179.0);
    loc.setLabel("Event location address");
    event.saveDetail(&loc);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));

    // Fetch back event
    QOrganizerEvent fetchEvent;
    QOrganizerItemId fetchId = event.id();
    fetchEvent = static_cast<QOrganizerEvent>(m_om->item(fetchId));

    QCOMPARE(fetchEvent.id(), event.id());
    QCOMPARE(fetchEvent.startDateTime(), event.startDateTime());
    QCOMPARE(fetchEvent.endDateTime(), event.endDateTime());
    QCOMPARE(fetchEvent.displayLabel(), event.displayLabel());
    QCOMPARE(fetchEvent.description(), event.description());
    QCOMPARE(fetchEvent.guid(), event.guid());
    QCOMPARE(fetchEvent.location(), event.location());
    QCOMPARE(fetchEvent.priority(), event.priority());
    QCOMPARE(fetchEvent.detail<QOrganizerItemLocation>(), event.detail<QOrganizerItemLocation>());
}

void tst_Maemo5Om::addTodo() {
    // Create todo and set its details
    QOrganizerTodo todo;

    todo.setDueDateTime(QDateTime(QDate(2010,6,16), QTime(13, 0, 0)));
    todo.setDisplayLabel("addTodo");
    todo.setDescription("Test todo description");
    todo.setGuid("custom todo GUID");
    todo.setPriority(QOrganizerItemPriority::LowestPriority);
    todo.setProgressPercentage(53);
    todo.setStatus(QOrganizerTodoProgress::StatusInProgress);


    // Save todo
    QVERIFY(m_om->saveItem(&todo));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!todo.id().isNull());
    QVERIFY(todo.id().managerUri().contains(managerName));

    // Fetch back todo
    QOrganizerTodo fetchTodo;
    QOrganizerItemId fetchId = todo.id();
    fetchTodo = static_cast<QOrganizerTodo>(m_om->item(fetchId));
    QCOMPARE(fetchTodo.id(), todo.id());
    QCOMPARE(fetchTodo.dueDateTime(), todo.dueDateTime());
    QCOMPARE(fetchTodo.displayLabel(), todo.displayLabel());
    QCOMPARE(fetchTodo.description(), todo.description());
    QCOMPARE(fetchTodo.guid(), todo.guid());
    QCOMPARE(fetchTodo.priority(), todo.priority());
    QCOMPARE(fetchTodo.progressPercentage(), todo.progressPercentage());
    QCOMPARE(fetchTodo.status(), todo.status());
    QCOMPARE(fetchTodo.detail<QOrganizerItemLocation>(), todo.detail<QOrganizerItemLocation>());
}

void tst_Maemo5Om::addJournal() {
    // Create journal and set its details
    QOrganizerJournal journal;

    journal.setDateTime(QDateTime(QDate(2010,6,17), QTime(11, 13, 2)));
    journal.setDisplayLabel("addJournal");
    journal.setDescription("Test journal description");
    journal.setGuid("custom journal GUID");

    // Save journal
    QVERIFY(m_om->saveItem(&journal));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!journal.id().isNull());
    QVERIFY(journal.id().managerUri().contains(managerName));

    // Fetch back journal
    QOrganizerJournal fetchJournal;
    QOrganizerItemId fetchId = journal.id();
    fetchJournal = static_cast<QOrganizerJournal>(m_om->item(fetchId));
    QCOMPARE(fetchJournal.id(), journal.id());
    QCOMPARE(fetchJournal.dateTime(), journal.dateTime());
    QCOMPARE(fetchJournal.displayLabel(), journal.displayLabel());
    QCOMPARE(fetchJournal.description(), journal.description());
    QCOMPARE(fetchJournal.guid(), journal.guid());
}

void tst_Maemo5Om::addEventWithRecurrence()
{
    // Create an event with recurrence
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010, 8, 22), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 8, 22), QTime(13, 0, 0)));
    event.setDisplayLabel("addEventWithRecurrence");
    event.setDescription("A weekly recurring event");

    // Create recurrence
    QOrganizerRecurrenceRule recurrenceRule;
    recurrenceRule.setLimit(10);
    recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Weekly);

    QSet<QOrganizerRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch all event instances
    QList<QOrganizerItem> instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 10);
    foreach(QOrganizerItem instance, instances) {
        QVERIFY(instance.guid() == event.guid());
        QVERIFY(instance.id().isNull());
        QVERIFY(instance.displayLabel() == event.displayLabel());
        QVERIFY(instance.description() == event.description());
        QVERIFY(instance.type() == QOrganizerItemType::TypeEventOccurrence);
        if (instance.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instance);
            QVERIFY(occ.parentId() == event.id());
        }
    }
}

void tst_Maemo5Om::addEventExceptions()
{
    QOrganizerEvent event;
    event.setDisplayLabel(QLatin1String("meeting"));
    event.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(11, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 1, 1), QTime(12, 0, 0)));
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(3);
    event.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
    QVERIFY(m_om->saveItem(&event));
    QVERIFY(!event.id().isNull());
    event = m_om->item(event.id());
    // the guid must be set so when it is exported to iCalendar, the relationship can be represented
    QVERIFY(!event.guid().isEmpty());

    QList<QOrganizerItem> items =
        m_om->itemOccurrences(event, QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)),
                                 QDateTime(QDate(2010, 2, 1), QTime(0, 0, 0)));
    QCOMPARE(items.size(), 3);
    QOrganizerItem secondItem = items.at(1);
    QCOMPARE(secondItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence secondEvent = static_cast<QOrganizerEventOccurrence>(secondItem);
    QCOMPARE(secondEvent.startDateTime(), QDateTime(QDate(2010, 1, 8), QTime(11, 0, 0)));
    QCOMPARE(secondEvent.id(), QOrganizerItemId());
    QCOMPARE(secondEvent.parentId(), event.id());
    // save a change to an occurrence's detail (ie. create an exception)
    secondEvent.setDisplayLabel(QLatin1String("seminar"));
    QVERIFY(m_om->saveItem(&secondEvent));

    // save a change to an occurrence's time
    QOrganizerEventOccurrence thirdEvent = static_cast<QOrganizerEventOccurrence>(items.at(2));
    QCOMPARE(thirdEvent.id(), QOrganizerItemId());
    QCOMPARE(thirdEvent.parentId(), event.id());
    thirdEvent.setStartDateTime(QDateTime(QDate(2010, 1, 15), QTime(10, 0, 0)));
    QVERIFY(m_om->saveItem(&thirdEvent));

    items =
        m_om->itemOccurrences(event, QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)),
                                 QDateTime(QDate(2010, 2, 1), QTime(0, 0, 0)));
    QCOMPARE(items.size(), 3);
    QOrganizerItem firstItem = items.at(0);
    // check that saving an exception doesn't change other items
    QCOMPARE(firstItem.displayLabel(), QLatin1String("meeting"));
    secondItem = items.at(1);
    // the exception's changes have been persisted
    QCOMPARE(secondItem.displayLabel(), QLatin1String("seminar"));
    QVERIFY(!secondItem.id().isNull());

    thirdEvent = static_cast<QOrganizerEventOccurrence>(items.at(2));
    QCOMPARE(thirdEvent.startDateTime(), QDateTime(QDate(2010, 1, 15), QTime(10, 0, 0)));
    QVERIFY(!secondEvent.id().isNull());
}

void tst_Maemo5Om::addEventExceptionWithGuid()
{
    // remove old items otherwise test will fail
    m_om->removeItems(m_om->itemIds(QDateTime(QDate(2009, 12, 24)), QDateTime(QDate(2009, 12, 27)), QOrganizerItemFilter()));

    // Set up some recurring items
    QOrganizerEvent christmas;
    christmas.setGuid("christmas");
    christmas.setStartDateTime(QDateTime(QDate(2009, 12, 25), QTime(0, 0, 0)));
    christmas.setEndDateTime(QDateTime(QDate(2009, 12, 26), QTime(0, 0, 0)));
    christmas.setDisplayLabel(QLatin1String("Christmas"));
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
    christmas.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
    QVERIFY(m_om->saveItem(&christmas));
    QVERIFY(!christmas.id().managerUri().isEmpty());
    QVERIFY(!christmas.id().isNull());

    QOrganizerEvent newYearsDay;
    newYearsDay.setGuid("newyear");
    newYearsDay.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)));
    newYearsDay.setEndDateTime(QDateTime(QDate(2010, 1, 2), QTime(0, 0, 0)));
    newYearsDay.setDisplayLabel(QLatin1String("New Years Day"));
    newYearsDay.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
    QVERIFY(m_om->saveItem(&newYearsDay));

    QOrganizerTodo report;
    report.setGuid("report");
    report.setDueDateTime(QDateTime(QDate(2010, 1, 11), QTime(0, 0, 0)));
    report.setDisplayLabel(QLatin1String("Report"));
    QVERIFY(m_om->saveItem(&report));

    // The tests:
    // exception with no guid or parentId fails
    QOrganizerEventOccurrence exception;
    exception.setOriginalDate(QDate(2010, 12, 25));
    exception.setStartDateTime(QDateTime(QDate(2010, 12, 25), QTime(0, 0, 0)));
    exception.setEndDateTime(QDateTime(QDate(2010, 12, 26), QTime(0, 0, 0)));
    exception.setDisplayLabel(QLatin1String("Christmas"));
    exception.addComment(QLatin1String("With the in-laws"));
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    // exception with invalid guid fails
    exception.setId(QOrganizerItemId());
    exception.setGuid(QLatin1String("halloween"));
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    // with the guid set, it should work
    exception.setId(QOrganizerItemId());
    exception.setGuid(QLatin1String("christmas"));
    QVERIFY(m_om->saveItem(&exception));
    QVERIFY(!exception.id().isNull());
    QOrganizerEventOccurrence savedException = m_om->item(exception.id());
    QCOMPARE(savedException.parentId(), christmas.id()); // parentLocalId should be set by manager

    // with the id, guid and the parentId all set and consistent, it should work
    exception = savedException;
    QVERIFY(m_om->saveItem(&exception));
    savedException = m_om->item(exception.id());
    QCOMPARE(savedException.parentId(), christmas.id());

    // Make a fresh exception object on a fresh date to avoid clashing with the previously saved one
    // can't set parentId to a non-event
    QOrganizerEventOccurrence exception2;
    exception2.setOriginalDate(QDate(2011, 12, 25));
    exception2.setStartDateTime(QDateTime(QDate(2011, 12, 25), QTime(0, 0, 0)));
    exception2.setEndDateTime(QDateTime(QDate(2011, 12, 26), QTime(0, 0, 0)));
    exception2.setDisplayLabel(QLatin1String("Christmas"));
    exception2.addComment(QLatin1String("With the in-laws"));
    exception2.setParentId(report.id()); // report is not an event
    QVERIFY(!m_om->saveItem(&exception2));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    // can't set guid to a non-event
    exception2.setGuid(QLatin1String("report"));
    exception2.setParentId(QOrganizerItemId());
    QVERIFY(!m_om->saveItem(&exception2));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    // can't make the guid inconsistent with the parentId
    exception2.setParentId(christmas.id());
    exception2.setGuid(QLatin1String("newyear"));
    QVERIFY(!m_om->saveItem(&exception2));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    // with just the parentId set to a valid parent, it should work
    exception2.setGuid(QLatin1String(""));
    QVERIFY(m_om->saveItem(&exception2));
    savedException = m_om->item(exception2.id());
    QCOMPARE(savedException.parentId(), christmas.id());
    QCOMPARE(savedException.guid(), QLatin1String("christmas")); // guid should be set by manager

    // Make a fresh exception object on a fresh date to avoid clashing with the previously saved one
    // exception without originalDate fails
    QOrganizerEventOccurrence exception3;
    exception3.setStartDateTime(QDateTime(QDate(2012, 12, 25), QTime(0, 0, 0)));
    exception3.setEndDateTime(QDateTime(QDate(2012, 12, 26), QTime(0, 0, 0)));
    exception3.setDisplayLabel(QLatin1String("Christmas"));
    exception3.addComment(QLatin1String("With the in-laws"));
    exception3.setParentId(christmas.id());
    exception3.setGuid(QLatin1String("christmas"));
    QVERIFY(!m_om->saveItem(&exception3));
    QCOMPARE(m_om->error(), QOrganizerManager::InvalidOccurrenceError);

    // with original date, guid and parentId set and consistent, and id=0, it should work
    exception3.setOriginalDate(QDate(2012, 12, 25));
    QVERIFY(m_om->saveItem(&exception3));
    QVERIFY(!exception3.id().isNull());
    savedException = m_om->item(exception3.id());
    QCOMPARE(savedException.parentId(), christmas.id());
}

void tst_Maemo5Om::removeEventWithRecurrence()
{
    // Create an event with recurrence
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010, 9, 22), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 9, 22), QTime(13, 0, 0)));
    event.setDisplayLabel("removeEventWithRecurrence");
    event.setDescription("A daily recurring event");

    // Create recurrence
    QOrganizerRecurrenceRule recurrenceRule;
    recurrenceRule.setLimit(20);
    recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);

    QSet<QOrganizerRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch all event instances
    QList<QOrganizerItem> instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 20);
    foreach(QOrganizerItem instance, instances) {
        QVERIFY(instance.guid() == event.guid());
        QVERIFY(instance.id().isNull());
        QVERIFY(instance.displayLabel() == event.displayLabel());
        QVERIFY(instance.description() == event.description());
        QVERIFY(instance.type() == QOrganizerItemType::TypeEventOccurrence);
        if (instance.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instance);
            QVERIFY(occ.parentId() == event.id());
        }
    }

    // Remove event
    QVERIFY(m_om->removeItem(event.id()));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);

    // Fetch all event instances
    instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

    // Shouldn't get any
    QVERIFY(instances.isEmpty());
}

void tst_Maemo5Om::removeEventExceptions()
{
    // Create an event with recurrence
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010, 10, 1), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 10, 1), QTime(13, 0, 0)));
    event.setDisplayLabel("removeEventExceptions");
    event.setDescription("A daily recurring event");

    // Create recurrence
    QOrganizerRecurrenceRule recurrenceRule;
    recurrenceRule.setLimit(10);
    recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);

    QSet<QOrganizerRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch one event instance
    QList<QOrganizerItem> instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,10,5), QTime(0,0,0)), QDateTime(QDate(2010,10,5), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 1);

    if (instances.count() == 1) {
        QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instances[0]);

        // Modify occurrence start and end time
        occ.setStartDateTime(QDateTime(QDate(2010,11,1), QTime(14,0,0)));
        occ.setEndDateTime(QDateTime(QDate(2010,11,1), QTime(15,0,0)));

        // Save back occurrence
        QVERIFY(m_om->saveItem(&occ));

        // Fetch all event instances
        instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

        // Should still get 10
        QCOMPARE(instances.count(), 10);

        // Create one extra occurrence
        QOrganizerEventOccurrence extraOccurrence;
        extraOccurrence.setGuid(event.guid());
        extraOccurrence.setOriginalDate(event.startDateTime().date().addDays(-1));
        extraOccurrence.setStartDateTime(QDateTime(QDate(2010,12,1), QTime(9, 0, 0)));
        extraOccurrence.setEndDateTime(QDateTime(QDate(2010,12,1), QTime(10, 0, 0)));
        extraOccurrence.setDisplayLabel("removeEventExceptions");
        extraOccurrence.setDescription("Created an extra occurrence");
        extraOccurrence.setOriginalDate(event.startDateTime().date());

        // Save it
        QVERIFY(m_om->saveItem(&extraOccurrence));

        // Fetch all event instances
        instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

        // Should now get 11
        QCOMPARE(instances.count(), 11);

        // Delete event
        QVERIFY(m_om->removeItem(event.id()));

        // Fetch all event instances
        instances = m_om->itemOccurrences(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

        // Shouldn't get any
        QVERIFY(instances.isEmpty());
    }
}

void tst_Maemo5Om::addWithIllegalParameters()
{
    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0));
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);

    QList<QOrganizerItem> items;
    QVERIFY(!m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);
}

void tst_Maemo5Om::getItemIds()
{
    QList<QOrganizerItemId> generatedIds;

    // Create a set of items
    for (int i = 1; i < 11; ++i) {
        QOrganizerEvent event;
        event.setId(QOrganizerItemId());
        event.setStartDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        event.setEndDateTime(QDateTime(QDate(2010,3,i), QTime(13,0,0)));
        event.setDisplayLabel("getItemIds");
        event.setDescription("Event");
        QVERIFY(m_om->saveItem(&event));
        generatedIds << event.id();

        QOrganizerTodo todo;
        todo.setId(QOrganizerItemId());
        todo.setStartDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        todo.setDisplayLabel("getItemIds");
        todo.setDescription("Todo");
        QVERIFY(m_om->saveItem(&todo));
        generatedIds << todo.id();

        QOrganizerJournal journal;
        journal.setId(QOrganizerItemId());
        journal.setDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        journal.setDisplayLabel("getItemIds");
        journal.setDescription("Journal");
        QVERIFY(m_om->saveItem(&journal));
        generatedIds << journal.id();
    }

    // Get items ids
    QList<QOrganizerItemId> ids = m_om->itemIds(QDateTime(QDate(2010,3,1)), QDateTime(QDate(2010,3,11)), QOrganizerItemFilter());

    // Check that all the item ids exist in result
    foreach(QOrganizerItemId id, generatedIds) {
        QVERIFY(ids.contains(id));
    }
}

void tst_Maemo5Om::getItems()
{
    QList<QOrganizerItem> generatedItems;

    // Create a set of items
    for (int i = 1; i < 11; ++i) {
        QOrganizerEvent event;
        event.setId(QOrganizerItemId());
        event.setStartDateTime(QDateTime(QDate(2010,1,i), QTime(12,0,0)));
        event.setEndDateTime(QDateTime(QDate(2010,1,i), QTime(13,0,0)));
        event.setDisplayLabel("getItems");
        event.setDescription("Event");
        QVERIFY(m_om->saveItem(&event));
        generatedItems << event;

        QOrganizerTodo todo;
        todo.setId(QOrganizerItemId());
        todo.setStartDateTime(QDateTime(QDate(2010,1,i), QTime(12,0,0)));
        todo.setDisplayLabel("getItems");
        todo.setDescription("Todo");
        QVERIFY(m_om->saveItem(&todo));
        generatedItems << todo;

        QOrganizerJournal journal;
        journal.setId(QOrganizerItemId());
        journal.setDateTime(QDateTime(QDate(2010,1,i), QTime(12,0,0)));
        journal.setDisplayLabel("getItems");
        journal.setDescription("Journal");
        QVERIFY(m_om->saveItem(&journal));
        generatedItems << journal;
    }

    // Get items
    QList<QOrganizerItem> items = m_om->items(QOrganizerItemFilter());

    // Check that all the items exist in result
    foreach(QOrganizerItem item, generatedItems) {
        bool itemFound = false;
        for (int i = 0; i < items.count(); ++i) {
            QOrganizerItem curr = items[i];
            if (curr.id() == item.id()) {
                QCOMPARE(curr.type(), item.type());
                itemFound = true;
                break;
            }
        }
        QVERIFY(itemFound);
    }

}

void tst_Maemo5Om::getItemOccurrences()
{
    // Create an event with recurrence
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 1, 1), QTime(13, 0, 0)));
    event.setDisplayLabel("getItemOccurrences");
    event.setDescription("A daily recurring event");

    // Create recurrence
    QOrganizerRecurrenceRule recurrenceRule;
    recurrenceRule.setLimit(15);
    recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);

    QSet<QOrganizerRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Make a few exception occurrences too
    for (int i = 0; i < 3; ++i) {
        QOrganizerEventOccurrence occ;
        occ.setGuid(event.guid());
        occ.setStartDateTime(QDateTime(QDate(2010,1,20+i), QTime(14, 0, 0)));
        occ.setEndDateTime(QDateTime(QDate(2010,1,20+i), QTime(15, 0, 0)));
        occ.setOriginalDate(event.startDateTime().date());
        occ.setDisplayLabel("getItemOccurrences");
        occ.setDescription("Exception occurrence");
        QVERIFY(m_om->saveItem(&occ));
        QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    }

    // Get all the instances occurring in January 2010
    QList<QOrganizerItem> januaryInstances = m_om->items(QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,2,1), QTime(0,0,0)), QOrganizerItemFilter());

    int instancesWithRightGuidCount = 0;
    foreach(QOrganizerItem instance, januaryInstances) {
        if (instance.guid() == event.guid())
            ++instancesWithRightGuidCount;
    }
    QCOMPARE(instancesWithRightGuidCount, 15 + 3);
}

void tst_Maemo5Om::setRecurrenceDates()
{
    // Create an event with recurrence dates
    QOrganizerEvent event;
    event.setStartDateTime(QDateTime(QDate(2010, 8, 22), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 8, 22), QTime(13, 0, 0)));
    event.setDisplayLabel("setRecurrenceDates");
    event.setDescription("Recurrence date test event");

    QSet<QDate> recurrenceDates;
    recurrenceDates << QDate(2010, 8, 30);
    recurrenceDates << QDate(2010, 9, 7);

    event.setRecurrenceDates(recurrenceDates);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event.id().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    QOrganizerEvent fetchedEvent = static_cast<QOrganizerEvent>(m_om->item(event.id()));
    QSet<QDate> fetchedRecurrenceDates = fetchedEvent.recurrenceDates();

    QCOMPARE(recurrenceDates, fetchedRecurrenceDates);
}

// the collectionIds function is no longer offered by the API
//void tst_Maemo5Om::getCollectionIds()
//{
//    QOrganizerCollectionLocalId defaultCollectionId = m_om->defaultCollectionId();
//    QList<QOrganizerCollectionLocalId> collectionIds = m_om->collectionIds();
//    QVERIFY(collectionIds.contains(defaultCollectionId));
//}

void tst_Maemo5Om::getCollections()
{
// the API which provides this functionality has changed;
// now takes no arguments, and returns all collections.
//    QOrganizerCollectionLocalId defaultCollectionId = m_om->defaultCollectionId();
//    QList<QOrganizerCollectionLocalId> collectionIds = m_om->collectionIds();
//    QList<QOrganizerCollection> collections = m_om->collections(collectionIds);
//
//    bool defaultCollectionExists = false;
//    bool allCollectionIds = true;
//    foreach(QOrganizerCollection collection, collections) {
//        if (collection.id().localId() == defaultCollectionId)
//            defaultCollectionExists = true;
//        if (!collectionIds.contains(collection.id().localId()))
//            allCollectionIds = false;
//    }
//
//    QVERIFY(defaultCollectionExists);
//    QVERIFY(allCollectionIds);

    QOrganizerCollection defaultCollection = m_om->defaultCollection();
    QList<QOrganizerCollection> allCollections = m_om->collections();
    QVERIFY(allCollections.contains(defaultCollection));
}

void tst_Maemo5Om::saveCollection()
{
    //remove old collections with the same name if exists
    QList<QOrganizerCollection> colls = m_om->collections();
    foreach (QOrganizerCollection c, colls) {
        if (c.metaData("Name") == "New calendar") {
            m_om->removeCollection(c.id());
            break;
        }
    }
    QOrganizerCollection newCollection;
    // These metadata fields are Maemo5 specific:
    newCollection.setMetaData("Name", "New calendar");
    newCollection.setMetaData("Color", "Red");
    newCollection.setMetaData("Readonly", "1");
    newCollection.setMetaData("Visible", "1");
    newCollection.setMetaData("Type", "Local");
    newCollection.setMetaData("Tune", "");
    newCollection.setMetaData("Version", "1.0");

    QVERIFY(m_om->saveCollection(&newCollection));
    QOrganizerCollectionId newCollectionId = newCollection.id();
    QVERIFY(newCollectionId.managerUri() == m_om->managerUri());
    QVERIFY(!newCollectionId.isNull());

    QOrganizerCollection fetchCollection = m_om->collection(newCollectionId);
    QVERIFY(fetchCollection.id() == newCollection.id());
    QVERIFY(fetchCollection.metaData("Name") == newCollection.metaData("Name"));
    QVERIFY(fetchCollection.metaData("Color") == newCollection.metaData("Color"));
    QVERIFY(fetchCollection.metaData("Readonly") == newCollection.metaData("Readonly"));
    QVERIFY(fetchCollection.metaData("Visible") == newCollection.metaData("Visible"));
    QVERIFY(fetchCollection.metaData("Type") == newCollection.metaData("Type"));
    QVERIFY(fetchCollection.metaData("Tune") == newCollection.metaData("Tune"));
    QVERIFY(fetchCollection.metaData("Version") == newCollection.metaData("Version"));
}

void tst_Maemo5Om::removeCollection()
{
    QOrganizerCollection newCollection;
    newCollection.setMetaData("Name", "Temporary calendar");
    newCollection.setMetaData("Color", "Red");

    QVERIFY(m_om->saveCollection(&newCollection));

    QVERIFY(m_om->collections().contains(newCollection));

    QVERIFY(m_om->removeCollection(newCollection.id()));
    QVERIFY(!m_om->collections().contains(newCollection));

    // Not possible to remove again
    QVERIFY(!m_om->removeCollection(newCollection.id()));

    // now resave and try the other removeCollection() function
    newCollection.setId(QOrganizerCollectionId());

    QVERIFY(m_om->saveCollection(&newCollection));

    QVERIFY(m_om->collections().contains(newCollection));

    QVERIFY(m_om->removeCollection(newCollection.id()));
    QVERIFY(!m_om->collections().contains(newCollection));

    // Not possible to remove again
    QVERIFY(!m_om->removeCollection(newCollection.id()));
}

void tst_Maemo5Om::saveItemsToNewCollection()
{
    //remove old collections with the same name if exists
    QList<QOrganizerCollection> colls = m_om->collections();
    foreach (QOrganizerCollection c, colls) {
        if (c.metaData("Name") == "New items") {
            m_om->removeCollection(c.id());
            break;
        }
    }
    QOrganizerCollection newCollection;
    newCollection.setMetaData("Name", "New items");
    newCollection.setMetaData("Color", "White");
    QVERIFY(m_om->saveCollection(&newCollection));
    QOrganizerCollectionId collId = newCollection.id();
    QVERIFY(!collId.isNull());

    // Save an item to the new collection
    QDate startDate1(2010, 8, 3);
    QDate endDate1(startDate1);
    QTime startTime1(10, 0, 0);
    QTime endTime1(11, 30, 0);

    QDate startDate2(2010, 8, 10);
    QDate endDate2(startDate2);
    QTime startTime2(12, 30, 0);
    QTime endTime2(13, 0, 28);

    QDate startDate3(2010, 8, 15);
    QDate endDate3(startDate3);
    QTime startTime3(12, 30, 0);
    QTime endTime3(13, 0, 28);

    // Create a simple event
    QOrganizerEvent event1;
    event1.setStartDateTime(QDateTime(startDate1, startTime1));
    event1.setEndDateTime(QDateTime(endDate1, endTime1));
    event1.setDisplayLabel("saveItemsToNewCollection, Event1");
    event1.setDescription("Simple test event description");
    event1.setCollectionId(newCollection.id());

    // Save
    QVERIFY(m_om->saveItem(&event1));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!event1.id().isNull());
    QVERIFY(event1.id().managerUri().contains(managerName));
    QVERIFY(!event1.guid().isEmpty());

    // Create another event
    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(startDate2, startTime2));
    event2.setEndDateTime(QDateTime(endDate2, endTime2));
    event2.setDisplayLabel("saveItemsToNewCollection, Event2");
    event2.setDescription("Save with list parameter");
    event2.setCollectionId(collId);

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(event2);
    QVERIFY(m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    foreach (QOrganizerItem item, items) {
        QVERIFY(!item.id().isNull());
        QVERIFY(item.id().managerUri().contains(managerName));
        QVERIFY(!item.guid().isEmpty());
    }

    // Create still one event
    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(startDate3, startTime3));
    event3.setEndDateTime(QDateTime(endDate3, endTime3));
    event3.setDisplayLabel("saveItemsToNewCollection, Event3");
    event3.setDescription("Save with list parameter and error map");
    event3.setCollectionId(collId);

    // Save with list parameter and error map parameter
    QList<QOrganizerItem> items2;
    items2.append(event3);
    QVERIFY(m_om->saveItems(&items2));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(m_om->errorMap().count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY(!item2.id().isNull());
        QVERIFY(item2.id().managerUri().contains(managerName));
        QVERIFY(!item2.guid().isEmpty());
    }

    // Save with recurrence
    QOrganizerEvent recurrenceEvent;
    recurrenceEvent.setStartDateTime(QDateTime(QDate(2010, 8, 22), QTime(12, 0, 0)));
    recurrenceEvent.setEndDateTime(QDateTime(QDate(2010, 8, 22), QTime(13, 0, 0)));
    recurrenceEvent.setDisplayLabel("saveItemsToNewCollection, Weekly recurring event");
    recurrenceEvent.setDescription("A weekly recurring event");
    recurrenceEvent.setCollectionId(collId);

    // Create recurrence
    QOrganizerRecurrenceRule recurrenceRule;
    recurrenceRule.setLimit(10);
    recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Weekly);

    QSet<QOrganizerRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    recurrenceEvent.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&recurrenceEvent));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!recurrenceEvent.id().isNull());
    QVERIFY(recurrenceEvent.id().managerUri().contains(managerName));
    QVERIFY(!recurrenceEvent.guid().isEmpty());

    // Save an extra occurrence to the reccurring event
    QOrganizerEventOccurrence recurrenceEventOccurrence;
    recurrenceEventOccurrence.setOriginalDate(recurrenceEvent.startDateTime().date());
    recurrenceEventOccurrence.setParentId(recurrenceEvent.id());
    recurrenceEventOccurrence.setStartDateTime(QDateTime(QDate(2010,8,31), QTime(14,0,0)));
    recurrenceEventOccurrence.setEndDateTime(QDateTime(QDate(2010,8,31), QTime(15,0,0)));
    recurrenceEventOccurrence.setDisplayLabel("saveItemsToNewCollection, Extra occurrence");
    recurrenceEventOccurrence.setDescription("Extra occurrence test");

    // Save event occurrence - NOTE should automatically be saved in the parent's collection
    QVERIFY(m_om->saveItem(&recurrenceEventOccurrence));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(recurrenceEventOccurrence.collectionId(), newCollection.id());

    // Try to fetch items
    QOrganizerItem f1 = m_om->item(event1.id());
    QOrganizerItem f2 = m_om->item(items.at(0).id());
    QOrganizerItem f3 = m_om->item(items2.at(0).id());

    QCOMPARE(f1.id(), event1.id());
    QCOMPARE(f1.guid(), event1.guid());

    QCOMPARE(f2.id(), items.at(0).id());
    QCOMPARE(f2.guid(), items.at(0).guid());

    QCOMPARE(f3.id(), items2.at(0).id());
    QCOMPARE(f3.guid(), items2.at(0).guid());

    // Get items with no filtering
    QOrganizerItemFilter noFilter;

    // Define a collection filter for the new collection
    QOrganizerItemCollectionFilter newCollectionFilter;
    newCollectionFilter.setCollectionIds(QSet<QOrganizerCollectionId>() << collId);

    // Define a collection filter for the default calendar
    QOrganizerItemCollectionFilter defaultCollectionFilter;
    defaultCollectionFilter.setCollectionIds(QSet<QOrganizerCollectionId>() << (m_om->defaultCollection().id()));

    // Define a union filter of the previous two collections
    QOrganizerItemUnionFilter unionFilter;
    unionFilter.setFilters(QList<QOrganizerItemFilter>() << newCollectionFilter << defaultCollectionFilter);

    // Define an intersection filter of the previous two collections
    QOrganizerItemIntersectionFilter intersectionFilter;
    intersectionFilter.setFilters(QList<QOrganizerItemFilter>() << newCollectionFilter << defaultCollectionFilter);

    // No sorting
    QList<QOrganizerItemSortOrder> noSort;

    // Get all item ids of the new collection
    QList<QOrganizerItemId> newCollectionItemIds = m_om->itemIds(newCollectionFilter, noSort);
    QCOMPARE(newCollectionItemIds.count(), 5);
    QVERIFY(newCollectionItemIds.contains(event1.id()));
    QVERIFY(newCollectionItemIds.contains(items.at(0).id()));
    QVERIFY(newCollectionItemIds.contains(items2.at(0).id()));
    QVERIFY(newCollectionItemIds.contains(recurrenceEvent.id()));
    QVERIFY(newCollectionItemIds.contains(recurrenceEventOccurrence.id()));

    // Get all item ids of the default collection
    QList<QOrganizerItemId> defaultCollectionItemIds = m_om->itemIds(defaultCollectionFilter, noSort);

    // Get all item ids of the union collection
    QList<QOrganizerItemId> unionItemIds = m_om->itemIds(unionFilter, noSort);

    // Number of item ids in the new collection + number of ids in the default collection
    // should match to the number of ids in union
    QCOMPARE(newCollectionItemIds.count() + defaultCollectionItemIds.count(), unionItemIds.count());

    // Get all item ids of the intersection collection
    QList<QOrganizerItemId> intersectionItemIds = m_om->itemIds(intersectionFilter, noSort);

    // There should be no items that exist in both collections
    QCOMPARE(intersectionItemIds.count(), 0);

    // When no filtering is set, all the item ids should be returned ( = equal to number of ids with union filtering)
    QList<QOrganizerItemId> noFilteringItemIds = m_om->itemIds(noFilter, noSort);
    QCOMPARE(noFilteringItemIds.count(), unionItemIds.count());

    // Get all items of the new collection
    QList<QOrganizerItem> newCollectionItems = m_om->itemsForExport(QDateTime(), QDateTime(), newCollectionFilter, noSort);
    QCOMPARE(newCollectionItems.count(), 5);

    // The collection ids should match
    foreach(QOrganizerItem newCollectionItem, newCollectionItems)
        QCOMPARE(newCollectionItem.collectionId(), collId);

    // Get all the instances of the new collection
    QList<QOrganizerItem> newCollectionInstances = m_om->items(newCollectionFilter, noSort);

    // The count should be 14 (10 instances are generated with the recurrence rule and one is added later + the 3 static ones)
    QCOMPARE(newCollectionInstances.count(), 14);

    // Get all the instances of the default collection
    QList<QOrganizerItem> defaultCollectionInstances = m_om->items(defaultCollectionFilter, noSort);

    // Get all the instances with the union filter
    QList<QOrganizerItem> unionInstances = m_om->items(unionFilter, noSort);
    QCOMPARE(newCollectionInstances.count() + defaultCollectionInstances.count(), unionInstances.count());

    // Get all the instances with the intersection filter
    QList<QOrganizerItem> intersectionInstances = m_om->items(intersectionFilter, noSort);
    QCOMPARE(intersectionInstances.count(), 0);
}

void tst_Maemo5Om::asynchronousSaveAndFetch()
{
    // Create event and set details
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010,12,15), QTime(11, 13, 2)));
    event.setEndDateTime(QDateTime(QDate(2010,12,17),QTime(2,0,0)));
    event.setDisplayLabel("asynchronousSaveAndFetch");
    event.setDescription("Asynchronous save test");
    event.setGuid("asynchronous event guid");
    event.setCollectionId(m_om->defaultCollection().id());

    // Create a save request
    QOrganizerItemSaveRequest saveRequest;
    saveRequest.setItem(event);
    saveRequest.setManager(m_om);

    // Start the request
    saveRequest.start();

    // Wait until the request processing begins (otherwise waitForFinished returns immediately)
    while (saveRequest.state() == QOrganizerAbstractRequest::InactiveState)
        QThread::yieldCurrentThread();

    // Wait for request finished (should return true as there's no timeout)
    QVERIFY(saveRequest.waitForFinished());

    // Try to fetch the saved item with an asynchronous request
    QList<QOrganizerCollection> allCollections = m_om->collections();
    QSet<QOrganizerCollectionId> allCollectionsLocalIds;
    foreach (const QOrganizerCollection& collection, allCollections) {
        allCollectionsLocalIds.insert(collection.id());
    }
    QOrganizerItemCollectionFilter collectionFilter;
    collectionFilter.setCollectionIds(allCollectionsLocalIds);
    QList<QOrganizerItemSortOrder> noSorting;

    QOrganizerItemFetchRequest fetchRequest;
    fetchRequest.setFilter(collectionFilter);
    fetchRequest.setSorting(noSorting);
    fetchRequest.setManager(m_om);

    // Start the request
    fetchRequest.start();

    // Wait until the request processing begins (otherwise waitForFinished returns immediately)
    while (fetchRequest.state() == QOrganizerAbstractRequest::InactiveState)
        QThread::yieldCurrentThread();

    // Wait for request finished (should return true as there's no timeout)
    QVERIFY(fetchRequest.waitForFinished());

    QList<QOrganizerItem> resultItems = fetchRequest.items();
    QOrganizerItem resultItem;
    foreach(QOrganizerItem itm, resultItems) {
        if (itm.guid() == event.guid()) {
            resultItem = itm;
            break;
        }
    }
    QVERIFY(resultItem.type() == QOrganizerItemType::TypeEvent);
    QOrganizerEvent resultEvent = static_cast<QOrganizerEvent>(resultItem);
    QCOMPARE(resultEvent.startDateTime(), event.startDateTime());
    QCOMPARE(resultEvent.endDateTime(), event.endDateTime());
    QCOMPARE(resultEvent.displayLabel(), event.displayLabel());
    QCOMPARE(resultEvent.description(), event.description());
}

void tst_Maemo5Om::consecutiveAsynchronousRequests()
{
    // Create three events
    QOrganizerEvent event1;
    event1.setStartDateTime(QDateTime(QDate(2010,11,1), QTime(11, 0, 0)));
    event1.setEndDateTime(QDateTime(QDate(2010,11,1),QTime(12, 0, 0)));
    event1.setDisplayLabel("consecutiveAsynchronousRequests");
    event1.setGuid("consecutiveAsynchronousRequests1");
    event1.setCollectionId(m_om->defaultCollection().id());

    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(QDate(2010,11,2), QTime(11, 0, 0)));
    event2.setEndDateTime(QDateTime(QDate(2010,11,2),QTime(12, 0, 0)));
    event2.setDisplayLabel("consecutiveAsynchronousRequests");
    event2.setGuid("consecutiveAsynchronousRequests2");
    event2.setCollectionId(m_om->defaultCollection().id());

    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(QDate(2010,11,3), QTime(11, 0, 0)));
    event3.setEndDateTime(QDateTime(QDate(2010,11,3),QTime(12, 0, 0)));
    event3.setDisplayLabel("consecutiveAsynchronousRequests");
    event3.setGuid("consecutiveAsynchronousRequests3");
    event3.setCollectionId(m_om->defaultCollection().id());

    // Create three save requests
    QOrganizerItemSaveRequest saveRequest1;
    saveRequest1.setItem(event1);
    saveRequest1.setManager(m_om);

    QOrganizerItemSaveRequest saveRequest2;
    saveRequest2.setItem(event2);
    saveRequest2.setManager(m_om);

    QOrganizerItemSaveRequest saveRequest3;
    saveRequest3.setItem(event3);
    saveRequest3.setManager(m_om);

    // Start all the requests
    saveRequest1.start();
    saveRequest2.start();
    saveRequest3.start();

    // Wait until the request processing begins (otherwise waitForFinished returns immediately)
    while (saveRequest1.state() == QOrganizerAbstractRequest::InactiveState ||
           saveRequest2.state() == QOrganizerAbstractRequest::InactiveState ||
           saveRequest3.state() == QOrganizerAbstractRequest::InactiveState) {
        QThread::yieldCurrentThread();
    }

    // Wait for all requests to finish
    QVERIFY(saveRequest1.waitForFinished());
    QVERIFY(saveRequest2.waitForFinished());
    QVERIFY(saveRequest3.waitForFinished());

    QList<QOrganizerItem> resultItems = m_om->items();
    int foundCount = 0;
    foreach(QOrganizerItem itm, resultItems) {
        if (itm.guid() == event1.guid() || itm.guid() == event2.guid() || itm.guid() == event3.guid())
            ++foundCount;
    }

    // All three events should exist in db
    QCOMPARE(foundCount, 3);
}

void tst_Maemo5Om::deleteRequest()
{
    // Create two fetch requests
    QOrganizerItemCollectionFilter collectionFilter;
    QList<QOrganizerCollection> allCollections = m_om->collections();
    QSet<QOrganizerCollectionId> allCollectionLocalIds;
    foreach (const QOrganizerCollection& collection, allCollections) {
        allCollectionLocalIds.insert(collection.id());
    }
    collectionFilter.setCollectionIds(allCollectionLocalIds);
    QList<QOrganizerItemSortOrder> noSorting;

    QOrganizerItemFetchRequest *fetchRequest1 = new QOrganizerItemFetchRequest();
    fetchRequest1->setFilter(collectionFilter);
    fetchRequest1->setSorting(noSorting);
    fetchRequest1->setManager(m_om);

    QOrganizerItemFetchRequest *fetchRequest2 = new QOrganizerItemFetchRequest();
    fetchRequest2->setFilter(collectionFilter);
    fetchRequest2->setSorting(noSorting);
    fetchRequest2->setManager(m_om);

    // Start both requests
    fetchRequest1->start();
    fetchRequest2->start();

    // Wait until the first request processing begins
    while (fetchRequest1->state() == QOrganizerAbstractRequest::InactiveState)
        QThread::yieldCurrentThread();

    // Delete both requests
    delete fetchRequest1;
    fetchRequest1 = 0;
    delete fetchRequest2;
    fetchRequest2 = 0;

    // (There are no explicit checks, the test just should not crash.)
}

void tst_Maemo5Om::testReminders()
{
    QDate date = QDateTime::currentDateTime().date().addDays(1);

    // Create an event and set its details
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(date, QTime(13, 0, 0)));
    event.setEndDateTime(QDateTime(date, QTime(14, 0, 0)));
    event.setDisplayLabel("testReminders event");
    event.setDescription("Test event reminder description");

    QOrganizerItemVisualReminder eventReminder = event.detail<QOrganizerItemVisualReminder>();
    eventReminder.setMessage("Event reminder message");
    eventReminder.setRepetition(10, 60);
    eventReminder.setSecondsBeforeStart(60 * 60);
    event.saveDetail(&eventReminder);

    // Save event
    QVERIFY(m_om->saveItem(&event));

    // Fetch event
    QOrganizerItem fetchItem = m_om->item(event.id());
    QOrganizerItemVisualReminder fetchEventReminder = fetchItem.detail<QOrganizerItemVisualReminder>();
    QCOMPARE(fetchEventReminder.dataUrl(), eventReminder.dataUrl());
    QCOMPARE(fetchEventReminder.message(), eventReminder.message());
    QCOMPARE(fetchEventReminder.repetitionCount(), eventReminder.repetitionCount());
    //QCOMPARE(reminder.repetitionDelay(), fetchEventReminder.repetitionDelay()); // Maemo5 does not support repetition delay
    QCOMPARE(fetchEventReminder.secondsBeforeStart(), eventReminder.secondsBeforeStart());

    // Create a todo and set its details
    QOrganizerTodo todo;
    todo.setDueDateTime(QDateTime(date, QTime(14, 0, 0)));
    todo.setStartDateTime(QDateTime(date, QTime(13, 0, 0)));
    todo.setDisplayLabel("testReminders todo");
    todo.setDescription("Test todo reminder description");

    QOrganizerItemVisualReminder todoReminder = todo.detail<QOrganizerItemVisualReminder>();
    todoReminder.setMessage("Todo reminder message");
    todoReminder.setRepetition(2, 10);
    todoReminder.setSecondsBeforeStart(3 * 60 * 60);
    todo.saveDetail(&todoReminder);

    // Save todo
    QVERIFY(m_om->saveItem(&todo));

    // Fetch todo
    QOrganizerItem fetchTodo = m_om->item(todo.id());
    QOrganizerItemVisualReminder fetchTodoReminder = fetchTodo.detail<QOrganizerItemVisualReminder>();
    QCOMPARE(fetchTodoReminder.dataUrl(), todoReminder.dataUrl());
    QCOMPARE(fetchTodoReminder.message(), todoReminder.message());
    QCOMPARE(fetchTodoReminder.repetitionCount(), todoReminder.repetitionCount());
    //QCOMPARE(reminder.repetitionDelay(), fetchEventReminder.repetitionDelay()); // Maemo5 does not support repetition delay
    QCOMPARE(fetchTodoReminder.secondsBeforeStart(), todoReminder.secondsBeforeStart());
}

QTEST_MAIN(tst_Maemo5Om);
#include "tst_maemo5om.moc"
