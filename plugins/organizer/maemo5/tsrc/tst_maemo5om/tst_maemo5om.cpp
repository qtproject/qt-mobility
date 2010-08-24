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

    void removeEventWithRecurrence();
    void removeEventExceptions();

    void addWithIllegalParameters();

    void getItemIds();
    void getItems();
    void getItemInstances();

    void setRecurrenceDates();
private:
    void getCollectionIds();
    void getCollections();
    void saveCollection();
    void removeCollection();
    void saveItemsToNewCollection();

    // TODO: Asynchronous requests testing

private:
    void addItem_data();
    void addItem();
    void fetchItem_data();
    void fetchItem();
    void fetchItemIds();

private:
    QString detailToQString(QString definitionName, QString field, QString value);
    bool parseDetails(QStringList detailsString, QList<QOrganizerItemDetail> &details);
    bool verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected);
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
    event1.setDisplayLabel("Simple test event");
    event1.setDescription("Simple test event description");

    // Save
    QVERIFY(m_om->saveItem(&event1));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event1.id().localId() != 0);
    QVERIFY(event1.id().managerUri().contains(managerName));
    QVERIFY(!event1.guid().isEmpty());

    // Create another event
    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(startDate2, startTime2));
    event2.setEndDateTime(QDateTime(endDate2, endTime2));
    event2.setDisplayLabel("Simple test event, save with list parameter");
    event2.setDescription("Save with list parameter");

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(event2);
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem item, items) {
        QVERIFY(item.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(managerName));
        QVERIFY(!item.guid().isEmpty());
    }

    // Create still one event
    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(startDate3, startTime3));
    event3.setEndDateTime(QDateTime(endDate3, endTime3));
    event3.setDisplayLabel("Simple test event, save with list parameter and error map");
    event3.setDescription("Save with list parameter and error map");

    // Save with list parameter and error map parameter
    QList<QOrganizerItem> items2;
    items2.append(event3);
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY(item2.id().localId() != 0);
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
    event.setDisplayLabel("Simple test event");
    event.setDescription("Simple test event description");

    // Save
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Remove
    QVERIFY(m_om->removeItem(event.id().localId()));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);

    // Try to fetch back, should fail
    QOrganizerItem fetchItem = m_om->item(event.id().localId());
    QVERIFY(fetchItem == QOrganizerItem());

    /* // Temporary commented out
    // Remove list
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
    QVERIFY(errorMap.count() == 0);*/
}

void tst_Maemo5Om::addEvent() {
    // Create event and set details
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010,6,15), QTime(11, 13, 2)));
    event.setEndDateTime(QDateTime(QDate(2010,6,17),QTime(2,0,0)));
    event.setDisplayLabel("Test event");
    event.setDescription("Test event description");
    event.setGuid("custom event GUID");
    event.setLocationAddress("Event location address");
    event.setLocationGeoCoordinates("Location geo coordinates");
    event.setLocationName("Location name");
    event.setPriority(QOrganizerItemPriority::HighPriority);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
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
    todo.setDisplayLabel("Test todo");
    todo.setDescription("Test todo description");
    todo.setGuid("custom todo GUID");
    todo.setPriority(QOrganizerItemPriority::LowestPriority);
    todo.setProgressPercentage(53);
    todo.setStatus(QOrganizerTodoProgress::StatusInProgress);

    // Save todo
    QVERIFY(m_om->saveItem(&todo));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(todo.id().localId() != 0);
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
    journal.setDisplayLabel("Test journal");
    journal.setDescription("Test journal description");
    journal.setGuid("custom journal GUID");

    // Save journal
    QVERIFY(m_om->saveItem(&journal));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(journal.id().localId() != 0);
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
    event.setDisplayLabel("Weekly recurring event");
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
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch all event instances
    QList<QOrganizerItem> instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 10);
    foreach(QOrganizerItem instance, instances) {
        QVERIFY(instance.guid() == event.guid());
        QVERIFY(instance.id().localId() == 0);
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
    QVERIFY(event.localId() != 0);
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
    QCOMPARE(secondEvent.localId(), (unsigned int)0);
    QCOMPARE(secondEvent.parentLocalId(), event.localId());
    // save a change to an occurrence's detail (ie. create an exception)
    secondEvent.setDisplayLabel(QLatin1String("seminar"));
    QVERIFY(m_om->saveItem(&secondEvent));

    // save a change to an occurrence's time
    QOrganizerEventOccurrence thirdEvent = static_cast<QOrganizerEventOccurrence>(items.at(2));
    QCOMPARE(thirdEvent.localId(), (unsigned int)0);
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
    QVERIFY(secondItem.localId() != 0);

    thirdEvent = static_cast<QOrganizerEventOccurrence>(items.at(2));
    QCOMPARE(thirdEvent.startDateTime(), QDateTime(QDate(2010, 1, 15), QTime(10, 0, 0)));
    QVERIFY(secondEvent.localId() != 0);
}

void tst_Maemo5Om::addEventExceptionWithGuid()
{
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
    QVERIFY(christmas.id().localId() != 0);

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

    // exception with no guid or parentId fails
    QOrganizerEventOccurrence exception;
    exception.setStartDateTime(QDateTime(QDate(2014, 12, 25), QTime(0, 0, 0)));
    exception.setEndDateTime(QDateTime(QDate(2014, 12, 26), QTime(0, 0, 0)));
    exception.setDisplayLabel(QLatin1String("Christmas"));
    exception.setDescription(QLatin1String("With the in-laws"));
    exception.addComment(QLatin1String("With the in-laws"));
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // exception with invalid guid fails
    exception.setGuid(QLatin1String("halloween"));
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // with the guid set, it should work
    exception.setGuid(QLatin1String("christmas"));
    QVERIFY(m_om->saveItem(&exception));
    QOrganizerEventOccurrence savedException = m_om->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());

    // with the guid and the parentId both set and consistent, it should work
    exception = savedException;
    QVERIFY(m_om->saveItem(&exception));
    savedException = m_om->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());

    // with the guid inconsistent with the parentId, it should fail
    exception.setParentLocalId(newYearsDay.localId());
    QVERIFY(!m_om->saveItem(&exception));

    // with just the parentId set, it should work
    exception.setGuid(QLatin1String(""));
    QVERIFY(m_om->saveItem(&exception));
    savedException = m_om->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), newYearsDay.localId());
    QCOMPARE(savedException.guid(), QLatin1String("newyear"));

    // can't set parentId to a non-event
    exception.setGuid(QLatin1String(""));
    exception.setParentLocalId(report.localId());
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // can't set guid to a non-event
    exception.setGuid(QLatin1String("report"));
    exception.setParentLocalId(0);
    QVERIFY(!m_om->saveItem(&exception));
    QCOMPARE(m_om->error(), QOrganizerItemManager::InvalidOccurrenceError);
}

void tst_Maemo5Om::removeEventWithRecurrence()
{
    // Create an event with recurrence
    QOrganizerEvent event;

    event.setStartDateTime(QDateTime(QDate(2010, 9, 22), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 9, 22), QTime(13, 0, 0)));
    event.setDisplayLabel("Daily recurring event");
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
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Fetch all event instances
    QList<QOrganizerItem> instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);
    QVERIFY(instances.count() == 20);
    foreach(QOrganizerItem instance, instances) {
        QVERIFY(instance.guid() == event.guid());
        QVERIFY(instance.id().localId() == 0);
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
    event.setDisplayLabel("Daily recurring event");
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
    QVERIFY(event.id().localId() != 0);
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

        /*
        foreach(QOrganizerItem i, instances) {
            qDebug() << i.localId();
            qDebug() << static_cast<QOrganizerEventOccurrence>(i).startDateTime();
        }*/

        // Create one extra occurrence
        QOrganizerEventOccurrence extraOccurrence;
        extraOccurrence.setGuid(event.guid());
        extraOccurrence.setOriginalDate(event.startDateTime().date().addDays(-1));
        extraOccurrence.setStartDateTime(QDateTime(QDate(2010,12,1), QTime(9, 0, 0)));
        extraOccurrence.setEndDateTime(QDateTime(QDate(2010,12,1), QTime(10, 0, 0)));
        extraOccurrence.setDisplayLabel("Extra occurrence");
        extraOccurrence.setDescription("Created an extra occurrence");
        extraOccurrence.setOriginalDate(event.startDateTime().date());

        // Save it
        QVERIFY(m_om->saveItem(&extraOccurrence));

        qDebug() << "-- " << extraOccurrence.localId();

        // Fetch all event instances
        instances = m_om->itemInstances(event, QDateTime(QDate(2010,1,1), QTime(0,0,0)), QDateTime(QDate(2010,12,31), QTime(23,59,59)), 0);

        /*
        foreach(QOrganizerItem i, instances) {
            qDebug() << i.localId();
            qDebug() << static_cast<QOrganizerEventOccurrence>(i).startDateTime();
        }*/

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
    qDebug() << "Add with illegal params";

    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0, 0, 0));
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
        event.setDisplayLabel("Event");
        event.setDescription("Event");
        QVERIFY(m_om->saveItem(&event));
        generatedIds << event.localId();

        QOrganizerTodo todo;
        todo.setId(QOrganizerItemId());
        todo.setStartDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        todo.setDisplayLabel("Todo");
        todo.setDescription("Todo");
        QVERIFY(m_om->saveItem(&todo));
        generatedIds << todo.localId();

        QOrganizerJournal journal;
        journal.setId(QOrganizerItemId());
        journal.setDateTime(QDateTime(QDate(2010,3,i), QTime(12,0,0)));
        journal.setDisplayLabel("Journal");
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
        event.setDisplayLabel("Event");
        event.setDescription("Event");
        QVERIFY(m_om->saveItem(&event));
        generatedItems << event;

        QOrganizerTodo todo;
        todo.setId(QOrganizerItemId());
        todo.setStartDateTime(QDateTime(QDate(2010,1,i), QTime(12,0,0)));
        todo.setDisplayLabel("Todo");
        todo.setDescription("Todo");
        QVERIFY(m_om->saveItem(&todo));
        generatedItems << todo;

        QOrganizerJournal journal;
        journal.setId(QOrganizerItemId());
        journal.setDateTime(QDateTime(QDate(2010,1,i), QTime(12,0,0)));
        journal.setDisplayLabel("Journal");
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
    event.setDisplayLabel("Daily recurring event");
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
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));
    QVERIFY(!event.guid().isEmpty());

    // Make a few exception occurrences too
    for (int i = 0; i < 3; ++i) {
        QOrganizerEventOccurrence occ;
        occ.setGuid(event.guid());
        occ.setStartDateTime(QDateTime(QDate(2010,1,20+i), QTime(14, 0, 0)));
        occ.setEndDateTime(QDateTime(QDate(2010,1,20+i), QTime(15, 0, 0)));
        occ.setOriginalDate(event.startDateTime().date());
        occ.setDisplayLabel("Exception occurrence");
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

void tst_Maemo5Om::addItem_data()
{
    QTest::addColumn<QStringList>("detailsString");

    // Item types
    QTest::newRow("Item type Event")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent));
    QTest::newRow("Item type EventOccurrence")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEventOccurrence));
    QTest::newRow("Item type Journal")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeJournal));
    QTest::newRow("Item type Todo")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo));
    QTest::newRow("Item type TodoOccurrence")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodoOccurrence));
    QTest::newRow("Item type Note")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote));

    // Other details
    QTest::newRow("Item type Event; details: description")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << detailToQString(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));

    // Commented out, QOrganizerItemNote is not declared
    /*
    QTest::newRow("Item type Event; details: description, note")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << detailToQString(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << detailToQString(QOrganizerItemComment::DefinitionName, QOrganizerItemComment::FieldComment, QString("Remember to wear blue jeans")));

    QTest::newRow("Item type Note; details: description, note")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote)
            << detailToQString(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << detailToQString(QOrganizerItemComment::DefinitionName, QOrganizerItemComment::FieldComment, QString("Remember to wear blue jeans")));
    */

    // TODO: recurrence and exceptions
    // TODO: timestamps
    // TODO: attendees?
    // TODO: display label
    // TODO: guid?
    // TODO: location
    // TODO: priority
    // TODO: ...
}

void tst_Maemo5Om::addItem()
{
    QFAIL("TODO");
    /*  
    QFETCH(QStringList, detailsString);
    QList<QOrganizerItemDetail> details(QList<QOrganizerItemDetail>());
    QVERIFY(parseDetails(detailsString, details));

    // Create item and set it's details
    QOrganizerItem item;
    foreach(QOrganizerItemDetail detail, details) {
        QVERIFY(item.saveDetail(&detail));
        QOrganizerItemDetailDefinition def = m_om->detailDefinition(detail.definitionName(), item.type());
        if (m_om->error() != QOrganizerItemManager::NoError) {
            // this detail is not supported, skip the test case
            QString skipMsg = QString("Detail ") + detail.definitionName() + QString(" not supported for item type " + item.type());
            QSKIP(skipMsg.toAscii(), SkipSingle);
        }
    }

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id() != QOrganizerItemId());

    // Fetch item to verify everything was saved successfully
    QOrganizerItem savedItem = m_om->item(item.localId());
    QVERIFY(verifyDetails(savedItem.details(), details));
    */
}

void tst_Maemo5Om::fetchItem_data()
{
    QTest::addColumn<QString>("itemType");

    QTest::newRow("Item type Event") << QString("Event");
    QTest::newRow("Item type Journal") << QString("Journal");
    QTest::newRow("Item type Todo") << QString("Todo");
    QTest::newRow("Item type Note") << QString("Note");
}

void tst_Maemo5Om::fetchItem()
{
    QFAIL("SKIPPED");
    return;

    // Create item
    QOrganizerItem item;
    item.setType(QString("Event"));
    /*item.setDescription(itemDescription);
    item.set*/

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id() != QOrganizerItemId());

    // Fetch
    QOrganizerItem fetchedItem = m_om->item(item.localId());
    QVERIFY(fetchedItem.id() != QOrganizerItemId());
    // TODO: TDD cases for other verifications
}

void tst_Maemo5Om::setRecurrenceDates()
{
    // Create an event with recurrence dates
    QOrganizerEvent event;
    event.setStartDateTime(QDateTime(QDate(2010, 8, 22), QTime(12, 0, 0)));
    event.setEndDateTime(QDateTime(QDate(2010, 8, 22), QTime(13, 0, 0)));

    QList<QDate> recurrenceDates;
    recurrenceDates << QDate(2010, 8, 30);
    recurrenceDates << QDate(2010, 9, 7);

    event.setRecurrenceDates(recurrenceDates);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
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
    qDebug() << defaultCollectionId;
    qDebug() << collectionIds;

    // TODO: Test case not ready
}

void tst_Maemo5Om::getCollections()
{
    QOrganizerCollectionLocalId defaultCollectionId = m_om->defaultCollectionId();
    QList<QOrganizerCollectionLocalId> collectionIds = m_om->collectionIds();
    QList<QOrganizerCollection> collections = m_om->collections();

    bool defaultCollectionExists = false;
    bool allCollectionIds = true;
    foreach(QOrganizerCollection collection, collections) {
        if (collection.id().localId() == defaultCollectionId)
            defaultCollectionExists = true;
        if (!collectionIds.contains(collection.id().localId()))
            allCollectionIds = false;

        qDebug() << "\n";
        qDebug() << collection.metaData();
    }

    QVERIFY(defaultCollectionExists);
    QVERIFY(allCollectionIds);

    // TODO: Test case not ready
}

void tst_Maemo5Om::saveCollection()
{
    QOrganizerCollection newCollection;
    newCollection.setMetaData("Name", "New calendar");
    newCollection.setMetaData("Color", "Red");
    newCollection.setMetaData("Readonly", "1");

    QVERIFY(m_om->saveCollection(&newCollection));

    // TODO: Test case not ready
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
    event1.setDisplayLabel("Simple test event");
    event1.setDescription("Simple test event description");

    // Save
    QVERIFY(m_om->saveItem(&event1, collId));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event1.id().localId() != 0);
    QVERIFY(event1.id().managerUri().contains(managerName));
    QVERIFY(!event1.guid().isEmpty());

    // Create another event
    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(startDate2, startTime2));
    event2.setEndDateTime(QDateTime(endDate2, endTime2));
    event2.setDisplayLabel("Simple test event, save with list parameter");
    event2.setDescription("Save with list parameter");

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(event2);
    QVERIFY(m_om->saveItems(&items, collId, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem item, items) {
        QVERIFY(item.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(managerName));
        QVERIFY(!item.guid().isEmpty());
    }

    // Create still one event
    QOrganizerEvent event3;
    event3.setStartDateTime(QDateTime(startDate3, startTime3));
    event3.setEndDateTime(QDateTime(endDate3, endTime3));
    event3.setDisplayLabel("Simple test event, save with list parameter and error map");
    event3.setDescription("Save with list parameter and error map");

    // Save with list parameter and error map parameter
    QList<QOrganizerItem> items2;
    items2.append(event3);
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items2, collId, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY(item2.id().localId() != 0);
        QVERIFY(item2.id().managerUri().contains(managerName));
        QVERIFY(!item2.guid().isEmpty());
    }

    // Save with recurrence
    QOrganizerEvent recurrenceEvent;

    recurrenceEvent.setStartDateTime(QDateTime(QDate(2010, 8, 22), QTime(12, 0, 0)));
    recurrenceEvent.setEndDateTime(QDateTime(QDate(2010, 8, 22), QTime(13, 0, 0)));
    recurrenceEvent.setDisplayLabel("Weekly recurring event");
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
    QVERIFY(recurrenceEvent.id().localId() != 0);
    QVERIFY(recurrenceEvent.id().managerUri().contains(managerName));
    QVERIFY(!recurrenceEvent.guid().isEmpty());

    // Save an extra occurrence to the reccurring event
    QOrganizerEventOccurrence recurrenceEventOccurrence;

    recurrenceEventOccurrence.setOriginalDate(recurrenceEvent.startDateTime().date());
    recurrenceEventOccurrence.setParentLocalId(recurrenceEvent.localId());
    recurrenceEventOccurrence.setStartDateTime(QDateTime(QDate(2010,8,31), QTime(14,0,0)));
    recurrenceEventOccurrence.setEndDateTime(QDateTime(QDate(2010,8,31), QTime(15,0,0)));
    recurrenceEventOccurrence.setDisplayLabel("Extra occurrence");
    recurrenceEventOccurrence.setDescription("Extra occurrence test");

    // Save event occurrence
    QVERIFY(m_om->saveItem(&recurrenceEventOccurrence, collId));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
}

void tst_Maemo5Om::fetchItemIds()
{
    // TODO: initialize calendar with test data and
    // set the expected item count according to the test data!

    QList<QOrganizerItemLocalId> ids = m_om->itemIds();
    QVERIFY(ids.count() > 0);
    foreach (QOrganizerItemLocalId id, ids) {
        QVERIFY(id != QOrganizerItemLocalId());
    }
}

/*!
 * Helper function for presenting detail field values as QStrings
 */
QString tst_Maemo5Om::detailToQString(QString definitionName, QString field, QString value)
{
    return definitionName + QString(":") + field + QString(":") + value;
}

/*!
 * Helper function for parsing QStrings into organizer item details
 */
bool tst_Maemo5Om::parseDetails(QStringList detailsString, QList<QOrganizerItemDetail> &details)
{
    foreach (QString detailString, detailsString) {
        QStringList detailParts = detailString.split(QChar(':'));
        if (detailParts.count() != 3) {
            qDebug() << QString("Failed to parse details");
            return false;
        }
        QOrganizerItemDetail detail(detailParts[0]);
        detail.setValue(detailParts[1], detailParts[2]);
        details.append(detail);
    }
    return true;
}

/*!
 * Helper function for verifying details against reference details
 */
bool tst_Maemo5Om::verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected)
{
    foreach (QOrganizerItemDetail expectedDetail, expected) {
        bool found(false);
        foreach (QOrganizerItemDetail actualDetail, actual) {
            if (actualDetail.definitionName() == expectedDetail.definitionName()) {
                if (actualDetail.variantValues() != expectedDetail.variantValues()) {
                    qDebug()
                        << "Detail value mismatch. Actual: " << actualDetail.variantValues()
                        << "Expected:" << expectedDetail.variantValues();
                    return false;
                } else {
                    // A valid detail with the same field values found
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            qDebug() << "Expected detail not found";
            return false;
        }
    }
    return true;
}

QTEST_MAIN(tst_Maemo5Om);
#include "tst_maemo5om.moc"
