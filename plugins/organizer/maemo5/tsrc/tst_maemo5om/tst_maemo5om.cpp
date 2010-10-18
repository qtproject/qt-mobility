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
    void getItemInstances();
    void setRecurrenceDates();

    void getCollectionIds();
    void getCollections();
    void saveCollection();
    void removeCollection();
    void saveItemsToNewCollection();

    void asynchronousSaveAndFetch();
    void consecutiveAsynchronousRequests();
    void deleteRequest();

    void testReminders();

private:
    QOrganizerItemManager *m_om;
};


void tst_Maemo5Om::init()
{
    m_om = new QOrganizerItemManager(managerName);
    // Remove all organizer items first (Note: ignores possible errors)
    //m_om->removeItems(m_om->itemIds(), 0);
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
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event1.id().localId().isNull());
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
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem item, items) {
        QVERIFY(!item.id().localId().isNull());
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
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY(!item2.id().localId().isNull());
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
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Remove
    QVERIFY(m_om->removeItem(event.id().localId()));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);

    // Try to fetch back, should fail
    QOrganizerItem fetchItem = m_om->item(event.id().localId());
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
    QList<QOrganizerItemLocalId> itemIds;
    itemIds.append(item2.localId());
    itemIds.append(item3.localId());
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->removeItems(itemIds, &errorMap));
    QVERIFY(errorMap.count() == 0);
}

void tst_Maemo5Om::addEvent() {
    // Create event and set details
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010,6,15), QTime(11, 13, 2)));
    event.setEndDateTime(QDateTime(QDate(2010,6,17),QTime(2,0,0)));
    event.setDisplayLabel("addEvent");
    event.setDescription("Test event description");
    event.setGuid("custom event GUID");
    event.setLocationAddress("Event location address");
    event.setLocationGeoCoordinates("Location geo coordinates");
    event.setLocationName("Location name");
    event.setPriority(QOrganizerItemPriority::HighPriority);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));

    // Fetch back event
    QOrganizerEvent fetchEvent;
    QOrganizerItemLocalId fetchId = event.id().localId();
    fetchEvent = static_cast<QOrganizerEvent>(m_om->item(fetchId));

    QCOMPARE(fetchEvent.id(), event.id());
    QCOMPARE(fetchEvent.startDateTime(), event.startDateTime());
    QCOMPARE(fetchEvent.endDateTime(), event.endDateTime());
    QCOMPARE(fetchEvent.displayLabel(), event.displayLabel());
    QCOMPARE(fetchEvent.description(), event.description());
    QCOMPARE(fetchEvent.guid(), event.guid());
    QCOMPARE(fetchEvent.locationAddress(), event.locationAddress());
    QCOMPARE(fetchEvent.locationGeoCoordinates(), event.locationGeoCoordinates());
    QCOMPARE(fetchEvent.locationName(), event.locationName());
    QCOMPARE(fetchEvent.priority(), event.priority());
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
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!todo.id().localId().isNull());
    QVERIFY(todo.id().managerUri().contains(managerName));

    // Fetch back todo
    QOrganizerTodo fetchTodo;
    QOrganizerItemLocalId fetchId = todo.id().localId();
    fetchTodo = static_cast<QOrganizerTodo>(m_om->item(fetchId));
    QCOMPARE(fetchTodo.id(), todo.id());
    QCOMPARE(fetchTodo.dueDateTime(), todo.dueDateTime());
    QCOMPARE(fetchTodo.displayLabel(), todo.displayLabel());
    QCOMPARE(fetchTodo.description(), todo.description());
    QCOMPARE(fetchTodo.guid(), todo.guid());
    QCOMPARE(fetchTodo.priority(), todo.priority());
    QCOMPARE(fetchTodo.progressPercentage(), todo.progressPercentage());
    QCOMPARE(fetchTodo.status(), todo.status());
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
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!journal.id().localId().isNull());
    QVERIFY(journal.id().managerUri().contains(managerName));

    // Fetch back journal
    QOrganizerJournal fetchJournal;
    QOrganizerItemLocalId fetchId = journal.id().localId();
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
    QOrganizerItemRecurrenceRule recurrenceRule;
    recurrenceRule.setCount(10);
    recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);

    QList<QOrganizerItemRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch all event instances
    QList<QOrganizerItem> instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 10);
    foreach(QOrganizerItem instance, instances) {
        QVERIFY(instance.guid() == event.guid());
        QVERIFY(instance.id().localId().isNull());
        QVERIFY(instance.id().managerUri().contains(managerName));
        QVERIFY(instance.displayLabel() == event.displayLabel());
        QVERIFY(instance.description() == event.description());
        QVERIFY(instance.type() == QOrganizerItemType::TypeEventOccurrence);
        if (instance.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instance);
            QVERIFY(occ.parentLocalId() == event.id().localId());
        }
    }
}

void tst_Maemo5Om::addEventExceptions()
{
    QOrganizerEvent event;
    event.setDisplayLabel(QLatin1String("meeting"));
    event.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(11, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 1, 1), QTime(12, 0, 0)));
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(3);
    event.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << rrule);
    QVERIFY(m_om->saveItem(&event));
    QVERIFY(!event.localId().isNull());
    event = m_om->item(event.localId());
    // the guid must be set so when it is exported to iCalendar, the relationship can be represented
    QVERIFY(!event.guid().isEmpty());

    QList<QOrganizerItem> items =
        m_om->itemInstances(event, QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)),
                                 QDateTime(QDate(2010, 2, 1), QTime(0, 0, 0)));
    QCOMPARE(items.size(), 3);
    QOrganizerItem secondItem = items.at(1);
    QCOMPARE(secondItem.type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QOrganizerEventOccurrence secondEvent = static_cast<QOrganizerEventOccurrence>(secondItem);
    QCOMPARE(secondEvent.startDateTime(), QDateTime(QDate(2010, 1, 8), QTime(11, 0, 0)));
    QCOMPARE(secondEvent.localId(), QOrganizerItemLocalId());
    QCOMPARE(secondEvent.parentLocalId(), event.localId());
    // save a change to an occurrence's detail (ie. create an exception)
    secondEvent.setDisplayLabel(QLatin1String("seminar"));
    QVERIFY(m_om->saveItem(&secondEvent));

    // save a change to an occurrence's time
    QOrganizerEventOccurrence thirdEvent = static_cast<QOrganizerEventOccurrence>(items.at(2));
    QCOMPARE(thirdEvent.localId(), QOrganizerItemLocalId());
    QCOMPARE(thirdEvent.parentLocalId(), event.localId());
    thirdEvent.setStartDateTime(QDateTime(QDate(2010, 1, 15), QTime(10, 0, 0)));
    QVERIFY(m_om->saveItem(&thirdEvent));

    items =
        m_om->itemInstances(event, QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)),
                                 QDateTime(QDate(2010, 2, 1), QTime(0, 0, 0)));
    QCOMPARE(items.size(), 3);
    QOrganizerItem firstItem = items.at(0);
    // check that saving an exception doesn't change other items
    QCOMPARE(firstItem.displayLabel(), QLatin1String("meeting"));
    secondItem = items.at(1);
    // the exception's changes have been persisted
    QCOMPARE(secondItem.displayLabel(), QLatin1String("seminar"));
    QVERIFY(!secondItem.localId().isNull());

    thirdEvent = static_cast<QOrganizerEventOccurrence>(items.at(2));
    QCOMPARE(thirdEvent.startDateTime(), QDateTime(QDate(2010, 1, 15), QTime(10, 0, 0)));
    QVERIFY(!secondEvent.localId().isNull());
}

void tst_Maemo5Om::addEventExceptionWithGuid()
{
    // Set up some recurring items
    QOrganizerEvent christmas;
    christmas.setGuid("christmas");
    christmas.setStartDateTime(QDateTime(QDate(2009, 12, 25), QTime(0, 0, 0)));
    christmas.setEndDateTime(QDateTime(QDate(2009, 12, 26), QTime(0, 0, 0)));
    christmas.setDisplayLabel(QLatin1String("Christmas"));
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    christmas.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << rrule);
    QVERIFY(m_om->saveItem(&christmas));
    QVERIFY(!christmas.id().managerUri().isEmpty());
    QVERIFY(!christmas.id().localId().isNull());

    QOrganizerEvent newYearsDay;
    newYearsDay.setGuid("newyear");
    newYearsDay.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)));
    newYearsDay.setEndDateTime(QDateTime(QDate(2010, 1, 2), QTime(0, 0, 0)));
    newYearsDay.setDisplayLabel(QLatin1String("New Years Day"));
    newYearsDay.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << rrule);
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
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // exception with invalid guid fails
    exception.setId(QOrganizerItemId());
    exception.setGuid(QLatin1String("halloween"));
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // with the guid set, it should work
    exception.setId(QOrganizerItemId());
    exception.setGuid(QLatin1String("christmas"));
    QVERIFY(m_om->saveItem(&exception));
    QVERIFY(!exception.localId().isNull());
    QOrganizerEventOccurrence savedException = m_om->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId()); // parentLocalId should be set by manager

    // with the localId, guid and the parentId all set and consistent, it should work
    exception = savedException;
    QVERIFY(m_om->saveItem(&exception));
    savedException = m_om->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());

    // Make a fresh exception object on a fresh date to avoid clashing with the previously saved one
    // can't set parentId to a non-event
    QOrganizerEventOccurrence exception2;
    exception2.setOriginalDate(QDate(2011, 12, 25));
    exception2.setStartDateTime(QDateTime(QDate(2011, 12, 25), QTime(0, 0, 0)));
    exception2.setEndDateTime(QDateTime(QDate(2011, 12, 26), QTime(0, 0, 0)));
    exception2.setDisplayLabel(QLatin1String("Christmas"));
    exception2.addComment(QLatin1String("With the in-laws"));
    exception2.setParentLocalId(report.localId()); // report is not an event
    QVERIFY(!m_om->saveItem(&exception2));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // can't set guid to a non-event
    exception2.setGuid(QLatin1String("report"));
    exception2.setParentLocalId(QOrganizerItemLocalId());
    QVERIFY(!m_om->saveItem(&exception2));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // can't make the guid inconsistent with the parentId
    exception2.setParentLocalId(christmas.localId());
    exception2.setGuid(QLatin1String("newyear"));
    QVERIFY(!m_om->saveItem(&exception2));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // with just the parentId set to a valid parent, it should work
    exception2.setGuid(QLatin1String(""));
    QVERIFY(m_om->saveItem(&exception2));
    savedException = m_om->item(exception2.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());
    QCOMPARE(savedException.guid(), QLatin1String("christmas")); // guid should be set by manager

    // Make a fresh exception object on a fresh date to avoid clashing with the previously saved one
    // exception without originalDate fails
    QOrganizerEventOccurrence exception3;
    exception3.setStartDateTime(QDateTime(QDate(2012, 12, 25), QTime(0, 0, 0)));
    exception3.setEndDateTime(QDateTime(QDate(2012, 12, 26), QTime(0, 0, 0)));
    exception3.setDisplayLabel(QLatin1String("Christmas"));
    exception3.addComment(QLatin1String("With the in-laws"));
    exception3.setParentLocalId(christmas.localId());
    exception3.setGuid(QLatin1String("christmas"));
    QVERIFY(!m_om->saveItem(&exception3));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // with original date, guid and parentId set and consistent, and localId=0, it should work
    exception3.setOriginalDate(QDate(2012, 12, 25));
    QVERIFY(m_om->saveItem(&exception3));
    QVERIFY(!exception3.localId().isNull());
    savedException = m_om->item(exception3.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());
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
    QOrganizerItemRecurrenceRule recurrenceRule;
    recurrenceRule.setCount(20);
    recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Daily);

    QList<QOrganizerItemRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch all event instances
    QList<QOrganizerItem> instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 20);
    foreach(QOrganizerItem instance, instances) {
        QVERIFY(instance.guid() == event.guid());
        QVERIFY(instance.id().localId().isNull());
        QVERIFY(instance.id().managerUri().contains(managerName));
        QVERIFY(instance.displayLabel() == event.displayLabel());
        QVERIFY(instance.description() == event.description());
        QVERIFY(instance.type() == QOrganizerItemType::TypeEventOccurrence);
        if (instance.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instance);
            QVERIFY(occ.parentLocalId() == event.id().localId());
        }
    }

    // Remove event
    QVERIFY(m_om->removeItem(event.id().localId()));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);

    // Fetch all event instances
    instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

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
    QOrganizerItemRecurrenceRule recurrenceRule;
    recurrenceRule.setCount(10);
    recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Daily);

    QList<QOrganizerItemRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch one event instance
    QList<QOrganizerItem> instances = m_om->itemInstances(event, QDateTime(QDate(2010,10,5), QTime(0,0,0)), QDateTime(QDate(2010,10,5), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 1);

    if (instances.count() == 1) {
        QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instances[0]);

        // Modify occurrence start and end time
        occ.setStartDateTime(QDateTime(QDate(2010,11,1), QTime(14,0,0)));
        occ.setEndDateTime(QDateTime(QDate(2010,11,1), QTime(15,0,0)));

        // Save back occurrence
        QVERIFY(m_om->saveItem(&occ));

        // Fetch all event instances
        instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

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
        instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

        // Should now get 11
        QCOMPARE(instances.count(), 11);

        // Delete event
        QVERIFY(m_om->removeItem(event.id().localId()));

        // Fetch all event instances
        instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

        // Shouldn't get any
        QVERIFY(instances.isEmpty());
    }
}

void tst_Maemo5Om::addWithIllegalParameters()
{
    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0, QOrganizerCollectionLocalId(), 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QList<QOrganizerItem> items;
    QVERIFY(!m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);
}

void tst_Maemo5Om::getItemIds()
{
    QList<QOrganizerItemLocalId> generatedIds;

    // Create a set of items
    for (int i = 1; i < 11; ++i) {
        QOrganizerEvent event;
        event.setId(QOrganizerItemId());
        event.setStartDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        event.setEndDateTime(QDateTime(QDate(2010,3,i), QTime(13,0,0)));
        event.setDisplayLabel("getItemIds");
        event.setDescription("Event");
        QVERIFY(m_om->saveItem(&event));
        generatedIds << event.localId();

        QOrganizerTodo todo;
        todo.setId(QOrganizerItemId());
        todo.setStartDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        todo.setDisplayLabel("getItemIds");
        todo.setDescription("Todo");
        QVERIFY(m_om->saveItem(&todo));
        generatedIds << todo.localId();

        QOrganizerJournal journal;
        journal.setId(QOrganizerItemId());
        journal.setDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        journal.setDisplayLabel("getItemIds");
        journal.setDescription("Journal");
        QVERIFY(m_om->saveItem(&journal));
        generatedIds << journal.localId();
    }

    // Get items ids
    QList<QOrganizerItemSortOrder> sortOrders;
    QList<QOrganizerItemLocalId> ids = m_om->itemIds(sortOrders);

    // Check that all the item ids exist in result
    foreach(QOrganizerItemLocalId id, generatedIds) {
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
    QList<QOrganizerItemSortOrder> sortOrders;
    QOrganizerItemFetchHint fetchHint;
    QList<QOrganizerItem> items = m_om->items(sortOrders, fetchHint);

    // Check that all the items exist in result
    foreach(QOrganizerItem item, generatedItems) {
        bool itemFound = false;
        for (int i = 0; i < items.count(); ++i) {
            QOrganizerItem curr = items[i];
            if (curr.localId() == item.localId()) {
                QCOMPARE(curr.type(), item.type());
                itemFound = true;
                break;
            }
        }
        QVERIFY(itemFound);
    }

}

void tst_Maemo5Om::getItemInstances()
{
    // Create an event with recurrence
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 1, 1), QTime(13, 0, 0)));
    event.setDisplayLabel("getItemInstances");
    event.setDescription("A daily recurring event");

    // Create recurrence
    QOrganizerItemRecurrenceRule recurrenceRule;
    recurrenceRule.setCount(15);
    recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Daily);

    QList<QOrganizerItemRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    event.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Make a few exception occurrences too
    for (int i = 0; i < 3; ++i) {
        QOrganizerEventOccurrence occ;
        occ.setGuid(event.guid());
        occ.setStartDateTime(QDateTime(QDate(2010,1,20+i), QTime(14, 0, 0)));
        occ.setEndDateTime(QDateTime(QDate(2010,1,20+i), QTime(15, 0, 0)));
        occ.setOriginalDate(event.startDateTime().date());
        occ.setDisplayLabel("getItemInstances");
        occ.setDescription("Exception occurrence");
        QVERIFY(m_om->saveItem(&occ));
        QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    }

    // Create a filter for fetching instances of January 2010
    QOrganizerItemDateTimePeriodFilter filter;
    filter.setStartPeriod(QDateTime(QDate(2010,1,1), QTime(0,0,0)));
    filter.setEndPeriod(QDateTime(QDate(2010,2,1), QTime(0,0,0)));

    // Create empty sortorder list
    QList<QOrganizerItemSortOrder> sortOrders;

    // Create empty fetch hint
    QOrganizerItemFetchHint fetchHint;

    // Get all the instances occurring in January 2010
    QList<QOrganizerItem> januaryInstances = m_om->itemInstances(filter, sortOrders, fetchHint);

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

    QList<QDate> recurrenceDates;
    recurrenceDates << QDate(2010, 8, 30);
    recurrenceDates << QDate(2010, 9, 7);

    event.setRecurrenceDates(recurrenceDates);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event.id().localId().isNull());
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    QOrganizerEvent fetchedEvent = static_cast<QOrganizerEvent>(m_om->item(event.id().localId()));
    QList<QDate> fetchedRecurrenceDates = fetchedEvent.recurrenceDates();

    QCOMPARE(recurrenceDates, fetchedRecurrenceDates);
}

void tst_Maemo5Om::getCollectionIds()
{
    QOrganizerCollectionLocalId defaultCollectionId = m_om->defaultCollectionId();
    QList<QOrganizerCollectionLocalId> collectionIds = m_om->collectionIds();
    QVERIFY(collectionIds.contains(defaultCollectionId));
}

void tst_Maemo5Om::getCollections()
{
    QOrganizerCollectionLocalId defaultCollectionId = m_om->defaultCollectionId();
    QList<QOrganizerCollectionLocalId> collectionIds = m_om->collectionIds();
    QList<QOrganizerCollection> collections = m_om->collections(collectionIds);

    bool defaultCollectionExists = false;
    bool allCollectionIds = true;
    foreach(QOrganizerCollection collection, collections) {
        if (collection.id().localId() == defaultCollectionId)
            defaultCollectionExists = true;
        if (!collectionIds.contains(collection.id().localId()))
            allCollectionIds = false;
    }

    QVERIFY(defaultCollectionExists);
    QVERIFY(allCollectionIds);
}

void tst_Maemo5Om::saveCollection()
{
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
    QVERIFY(!newCollectionId.localId().isNull());

    QList<QOrganizerCollection> fetchCollectionList =
            m_om->collections(QList<QOrganizerCollectionLocalId>() << newCollectionId.localId());
    QCOMPARE(fetchCollectionList.count(), 1);
    QOrganizerCollection fetchCollection = fetchCollectionList[0];
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

    QVERIFY(m_om->collectionIds().contains(newCollection.id().localId()));

    QVERIFY(m_om->removeCollection(newCollection.id().localId()));
    QVERIFY(!m_om->collectionIds().contains(newCollection.id().localId()));

    // Not possible to remove again
    QVERIFY(!m_om->removeCollection(newCollection.id().localId()));
}

void tst_Maemo5Om::saveItemsToNewCollection()
{
    QOrganizerCollection newCollection;
    newCollection.setMetaData("Name", "New items");
    newCollection.setMetaData("Color", "White");
    QVERIFY(m_om->saveCollection(&newCollection));
    QOrganizerCollectionLocalId collId = newCollection.id().localId();
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

    // Save
    QVERIFY(m_om->saveItem(&event1, collId));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!event1.id().localId().isNull());
    QVERIFY(event1.id().managerUri().contains(managerName));
    QVERIFY(!event1.guid().isEmpty());

    // Create another event
    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(startDate2, startTime2));
    event2.setEndDateTime(QDateTime(endDate2, endTime2));
    event2.setDisplayLabel("saveItemsToNewCollection, Event2");
    event2.setDescription("Save with list parameter");

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(event2);
    QVERIFY(m_om->saveItems(&items, collId, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem item, items) {
        QVERIFY(!item.id().localId().isNull());
        QVERIFY(item.id().managerUri().contains(managerName));
        QVERIFY(!item.guid().isEmpty());
    }

    // Create still one event
    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(startDate3, startTime3));
    event3.setEndDateTime(QDateTime(endDate3, endTime3));
    event3.setDisplayLabel("saveItemsToNewCollection, Event3");
    event3.setDescription("Save with list parameter and error map");

    // Save with list parameter and error map parameter
    QList<QOrganizerItem> items2;
    items2.append(event3);
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items2, collId, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY(!item2.id().localId().isNull());
        QVERIFY(item2.id().managerUri().contains(managerName));
        QVERIFY(!item2.guid().isEmpty());
    }

    // Save with recurrence
    QOrganizerEvent recurrenceEvent;
    recurrenceEvent.setStartDateTime(QDateTime(QDate(2010, 8, 22), QTime(12, 0, 0)));
    recurrenceEvent.setEndDateTime(QDateTime(QDate(2010, 8, 22), QTime(13, 0, 0)));
    recurrenceEvent.setDisplayLabel("saveItemsToNewCollection, Weekly recurring event");
    recurrenceEvent.setDescription("A weekly recurring event");

    // Create recurrence
    QOrganizerItemRecurrenceRule recurrenceRule;
    recurrenceRule.setCount(10);
    recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);

    QList<QOrganizerItemRecurrenceRule> recurrenceRules;
    recurrenceRules << recurrenceRule;

    // Set recurrence
    recurrenceEvent.setRecurrenceRules(recurrenceRules);

    // Save event
    QVERIFY(m_om->saveItem(&recurrenceEvent, collId));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(!recurrenceEvent.id().localId().isNull());
    QVERIFY(recurrenceEvent.id().managerUri().contains(managerName));
    QVERIFY(!recurrenceEvent.guid().isEmpty());

    // Save an extra occurrence to the reccurring event
    QOrganizerEventOccurrence recurrenceEventOccurrence;
    recurrenceEventOccurrence.setOriginalDate(recurrenceEvent.startDateTime().date());
    recurrenceEventOccurrence.setParentLocalId(recurrenceEvent.localId());
    recurrenceEventOccurrence.setStartDateTime(QDateTime(QDate(2010,8,31), QTime(14,0,0)));
    recurrenceEventOccurrence.setEndDateTime(QDateTime(QDate(2010,8,31), QTime(15,0,0)));
    recurrenceEventOccurrence.setDisplayLabel("saveItemsToNewCollection, Extra occurrence");
    recurrenceEventOccurrence.setDescription("Extra occurrence test");

    // Save event occurrence
    QVERIFY(m_om->saveItem(&recurrenceEventOccurrence, collId));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);

    // Try to fetch items
    QOrganizerItem f1 = m_om->item(event1.localId());
    QOrganizerItem f2 = m_om->item(items.at(0).localId());
    QOrganizerItem f3 = m_om->item(items2.at(0).localId());

    QCOMPARE(f1.id(), event1.id());
    QCOMPARE(f1.guid(), event1.guid());

    QCOMPARE(f2.id(), items.at(0).id());
    QCOMPARE(f2.guid(), items.at(0).guid());

    QCOMPARE(f3.id(), items2.at(0).id());
    QCOMPARE(f3.guid(), items2.at(0).guid());

    // Get items with no filtering
    QOrganizerItemFilter noFilter;

    // Define a collection filter for the next collection
    QOrganizerItemCollectionFilter newCollectionFilter;
    newCollectionFilter.setCollectionIds(QSet<QOrganizerCollectionLocalId>() << collId);

    // Define a collection filter for the default calendar
    QOrganizerItemCollectionFilter defaultCollectionFilter;
    defaultCollectionFilter.setCollectionIds(QSet<QOrganizerCollectionLocalId>() << m_om->defaultCollectionId());

    // Define a union filter of the previous two collections
    QOrganizerItemUnionFilter unionFilter;
    unionFilter.setFilters(QList<QOrganizerItemFilter>() << newCollectionFilter << defaultCollectionFilter);

    // Define an intersection filter of the previous two collections
    QOrganizerItemIntersectionFilter intersectionFilter;
    intersectionFilter.setFilters(QList<QOrganizerItemFilter>() << newCollectionFilter << defaultCollectionFilter);

    // No sorting
    QList<QOrganizerItemSortOrder> noSort;

    // Get all item ids of the new collection
    QList<QOrganizerItemLocalId> newCollectionItemIds = m_om->itemIds(newCollectionFilter, noSort);
    QCOMPARE(newCollectionItemIds.count(), 5);
    QVERIFY(newCollectionItemIds.contains(event1.localId()));
    QVERIFY(newCollectionItemIds.contains(items.at(0).localId()));
    QVERIFY(newCollectionItemIds.contains(items2.at(0).localId()));
    QVERIFY(newCollectionItemIds.contains(recurrenceEvent.localId()));
    QVERIFY(newCollectionItemIds.contains(recurrenceEventOccurrence.localId()));

    // Get all item ids of the default collection
    QList<QOrganizerItemLocalId> defaultCollectionItemIds = m_om->itemIds(defaultCollectionFilter, noSort);

    // Get all item ids of the union collection
    QList<QOrganizerItemLocalId> unionItemIds = m_om->itemIds(unionFilter, noSort);

    // Number of item ids in the new collection + number of ids in the default collection
    // should match to the number of ids in union
    QCOMPARE(newCollectionItemIds.count() + defaultCollectionItemIds.count(), unionItemIds.count());

    // Get all item ids of the intersection collection
    QList<QOrganizerItemLocalId> intersectionItemIds = m_om->itemIds(intersectionFilter, noSort);

    // There should be items that exist in both collections
    QCOMPARE(intersectionItemIds.count(), 0);

    // When no filtering is set, all the item ids should be returned ( = equal to number of ids with union filtering)
    QList<QOrganizerItemLocalId> noFilteringItemIds = m_om->itemIds(noFilter, noSort);
    QCOMPARE(noFilteringItemIds.count(), unionItemIds.count());

    // Get all items of the new collection
    QList<QOrganizerItem> newCollectionItems = m_om->items(newCollectionFilter, noSort);
    QCOMPARE(newCollectionItems.count(), 5);

    // The collection ids should match
    foreach(QOrganizerItem newCollectionItem, newCollectionItems)
        QCOMPARE(newCollectionItem.collectionId().localId(), collId);

    // Get all the instances of the new collection
    QList<QOrganizerItem> newCollectionInstances = m_om->itemInstances(newCollectionFilter, noSort);

    // The count should be 11 (10 instances are generated with the recurrence rule and one is added later)
    QCOMPARE(newCollectionInstances.count(), 11);

    // Get all the instances of the default collection
    QList<QOrganizerItem> defaultCollectionInstances = m_om->itemInstances(defaultCollectionFilter, noSort);

    // Get all the instances with the union filter
    QList<QOrganizerItem> unionInstances = m_om->itemInstances(unionFilter, noSort);
    QCOMPARE(newCollectionInstances.count() + defaultCollectionInstances.count(), unionInstances.count());

    // Get all the instances with the intersection filter
    QList<QOrganizerItem> intersectionInstances = m_om->itemInstances(intersectionFilter, noSort);
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

    // Create a save request
    QOrganizerItemSaveRequest saveRequest;
    saveRequest.setCollectionId(m_om->defaultCollectionId());
    saveRequest.setItem(event);
    saveRequest.setManager(m_om);

    // Start the request
    saveRequest.start();

    // Wait until the request processing begins (otherwise waitForFinished returns immediately)
    while (saveRequest.state() == QOrganizerItemAbstractRequest::InactiveState)
        QThread::yieldCurrentThread();

    // Wait for request finished (should return true as there's no timeout)
    QVERIFY(saveRequest.waitForFinished());

    // Try to fetch the saved item with an asynchronous request
    QOrganizerItemCollectionFilter collectionFilter;
    collectionFilter.setCollectionIds(m_om->collectionIds().toSet());
    QList<QOrganizerItemSortOrder> noSorting;

    QOrganizerItemFetchRequest fetchRequest;
    fetchRequest.setFilter(collectionFilter);
    fetchRequest.setSorting(noSorting);
    fetchRequest.setManager(m_om);

    // Start the request
    fetchRequest.start();

    // Wait until the request processing begins (otherwise waitForFinished returns immediately)
    while (fetchRequest.state() == QOrganizerItemAbstractRequest::InactiveState)
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

    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(QDate(2010,11,2), QTime(11, 0, 0)));
    event2.setEndDateTime(QDateTime(QDate(2010,11,2),QTime(12, 0, 0)));
    event2.setDisplayLabel("consecutiveAsynchronousRequests");
    event2.setGuid("consecutiveAsynchronousRequests2");

    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(QDate(2010,11,3), QTime(11, 0, 0)));
    event3.setEndDateTime(QDateTime(QDate(2010,11,3),QTime(12, 0, 0)));
    event3.setDisplayLabel("consecutiveAsynchronousRequests");
    event3.setGuid("consecutiveAsynchronousRequests3");

    // Create three save requests
    QOrganizerItemSaveRequest saveRequest1;
    saveRequest1.setCollectionId(m_om->defaultCollectionId());
    saveRequest1.setItem(event1);
    saveRequest1.setManager(m_om);

    QOrganizerItemSaveRequest saveRequest2;
    saveRequest2.setCollectionId(m_om->defaultCollectionId());
    saveRequest2.setItem(event2);
    saveRequest2.setManager(m_om);

    QOrganizerItemSaveRequest saveRequest3;
    saveRequest3.setCollectionId(m_om->defaultCollectionId());
    saveRequest3.setItem(event3);
    saveRequest3.setManager(m_om);

    // Start all the requests
    saveRequest1.start();
    saveRequest2.start();
    saveRequest3.start();

    // Wait until the request processing begins (otherwise waitForFinished returns immediately)
    while (saveRequest1.state() == QOrganizerItemAbstractRequest::InactiveState ||
           saveRequest2.state() == QOrganizerItemAbstractRequest::InactiveState ||
           saveRequest3.state() == QOrganizerItemAbstractRequest::InactiveState) {
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
    collectionFilter.setCollectionIds(m_om->collectionIds().toSet());
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
    while (fetchRequest1->state() == QOrganizerItemAbstractRequest::InactiveState)
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
    eventReminder.setDateTime(QDateTime(date, QTime(12, 0, 0)));
    eventReminder.setRepetition(10, 60);
    eventReminder.setTimeDelta(60 * 60);
    event.saveDetail(&eventReminder);

    // Save event
    QVERIFY(m_om->saveItem(&event));

    // Fetch event
    QOrganizerItem fetchItem = m_om->item(event.localId());
    QOrganizerItemVisualReminder fetchEventReminder = fetchItem.detail<QOrganizerItemVisualReminder>();
    QCOMPARE(fetchEventReminder.dataUrl(), eventReminder.dataUrl());
    QCOMPARE(fetchEventReminder.dateTime(), eventReminder.dateTime());
    QCOMPARE(fetchEventReminder.message(), eventReminder.message());
    QCOMPARE(fetchEventReminder.repetitionCount(), eventReminder.repetitionCount());
    //QCOMPARE(reminder.repetitionDelay(), fetchEventReminder.repetitionDelay()); // Maemo5 does not support repetition delay
    QCOMPARE(fetchEventReminder.timeDelta(), eventReminder.timeDelta());

    // Create a todo and set its details
    QOrganizerTodo todo;
    todo.setDueDateTime(QDateTime(date, QTime(14, 0, 0)));
    todo.setStartDateTime(QDateTime(date, QTime(13, 0, 0)));
    todo.setDisplayLabel("testReminders todo");
    todo.setDescription("Test todo reminder description");

    QOrganizerItemVisualReminder todoReminder = todo.detail<QOrganizerItemVisualReminder>();
    todoReminder.setMessage("Todo reminder message");
    todoReminder.setDateTime(QDateTime(date, QTime(10, 0, 0)));
    todoReminder.setRepetition(2, 10);
    todoReminder.setTimeDelta(3 * 60 * 60);
    todo.saveDetail(&todoReminder);

    // Save todo
    QVERIFY(m_om->saveItem(&todo));

    // Fetch todo
    QOrganizerItem fetchTodo = m_om->item(todo.localId());
    QOrganizerItemVisualReminder fetchTodoReminder = fetchTodo.detail<QOrganizerItemVisualReminder>();
    QCOMPARE(fetchTodoReminder.dataUrl(), todoReminder.dataUrl());
    QCOMPARE(fetchTodoReminder.dateTime(), todoReminder.dateTime());
    QCOMPARE(fetchTodoReminder.message(), todoReminder.message());
    QCOMPARE(fetchTodoReminder.repetitionCount(), todoReminder.repetitionCount());
    //QCOMPARE(reminder.repetitionDelay(), fetchEventReminder.repetitionDelay()); // Maemo5 does not support repetition delay
    QCOMPARE(fetchTodoReminder.timeDelta(), todoReminder.timeDelta());
}

QTEST_MAIN(tst_Maemo5Om);
#include "tst_maemo5om.moc"
