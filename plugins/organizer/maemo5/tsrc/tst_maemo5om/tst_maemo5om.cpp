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
    void initTestCase();
    void cleanupTestCase();

private slots:  // Test cases

    //void addSimpleItem();
    //void addSimpleEvent();
    //void addSimpleTodo();
    //void addSimpleJournal();
    //void removeSimpleItem();


    //void addSimpleEvent();
    //void addEventWithSimpleRecurrence();

    //void tmp();
    //void tmp2();
    void tmp3();

private:

    void tmp();
    void tmp2();
    //void tmp3();

    void addSimpleItem();
    void addSimpleEvent();
    void addSimpleTodo();
    void addSimpleJournal();
    void removeSimpleItem();


    void addEventWithSimpleRecurrence();


    void addWithIllegalParameters();
    //void fetchSimpleEvent();
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


void tst_Maemo5Om::initTestCase()
{
    m_om = new QOrganizerItemManager(managerName);
    // Remove all organizer items first (Note: ignores possible errors)
    //m_om->removeItems(m_om->itemIds(), 0);
}

void tst_Maemo5Om::cleanupTestCase()
{
    delete m_om;
}

void tst_Maemo5Om::tmp()
{
    /*
    QOrganizerItemFilter filter;
    QOrganizerItemSortOrder sorder;
    QList<QOrganizerItemSortOrder> solist; solist.append(sorder);
    QOrganizerItemFetchHint fetchHint;
    m_om->items( filter, solist, fetchHint );
    */

    QOrganizerEvent event;

    QDate date1( 2010, 6, 15 );
    QDate date2( 2010, 6, 15 );
    QTime startTime( 11, 13, 2 );
    QTime endTime( 12, 58, 4 );
    QDateTime eventStartTime( date1, startTime );
    QDateTime eventEndTime( date2, endTime );

    QString displayLabel("Simple event display label");
    QString description("This is a simple test event");

    event.setStartDateTime( eventStartTime );
    event.setEndDateTime( eventEndTime );
    event.setDisplayLabel( displayLabel );
    event.setDescription( description );
    event.setGuid(QString("eventti"));

    QList<QOrganizerItemRecurrenceRule> rrules;
    QOrganizerItemRecurrenceRule rule;
    rule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
    rule.setInterval(2);
    rrules << rule;
    event.setRecurrenceRules(rrules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));

    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(QDate(2010,6,12),QTime(9,0,0)));
    event2.setEndDateTime(QDateTime(QDate(2010,6,12),QTime(10,0,0)));;
    event2.setGuid(QString("eventti"));
    QVERIFY(m_om->saveItem(&event2));

    // Get occs
    QList<QOrganizerItem> instances = m_om->itemInstances(event,
                                                          QDateTime(QDate(2010,6,10), QTime(10,0,0)),
                                                          QDateTime(QDate(2010,6,29),QTime(23,0,0)), 4);
    for (int i = 0; i < instances.count(); ++i) {
        QOrganizerEventOccurrence o = static_cast<QOrganizerEventOccurrence>(instances[i]);
        qDebug() << o.startDateTime();
        qDebug() << o.endDateTime();
        qDebug() << o.originalDate();
        qDebug() << o.localId();
        qDebug() << o.parentLocalId();
        qDebug() << o.guid();
    }

    QOrganizerEventOccurrence occ = static_cast<QOrganizerEventOccurrence>(instances[2]);
    occ.setDescription("Muutettu");
    //occ.setOriginalDate(QDate(2009,1,1));
    occ.setParentLocalId(0);
    m_om->saveItem(&occ);


/*
    QOrganizerEventOccurrence eventOccurrence;

    QDate date2( 2010, 6, 12 );
    QDateTime eventStartTime2( date2, startTime );
    QDateTime eventEndTime2( date2, endTime );

    QString displayLabel2("Occurrence");
    QString description2("Occurrence");

    eventOccurrence.setStartDateTime( eventStartTime2 );
    eventOccurrence.setEndDateTime( eventEndTime2 );
    eventOccurrence.setDisplayLabel( displayLabel2 );
    eventOccurrence.setDescription( description2 );
    eventOccurrence.setGuid(QString("eventti"));
    eventOccurrence.setOriginalDate(date);

    QVERIFY(m_om->saveItem(&eventOccurrence));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
*/
}

void tst_Maemo5Om::tmp2()
{
    QOrganizerEvent event;

    QDate date1( 2010, 7, 10 );
    QDate date2( 2010, 7, 10 );
    QTime startTime( 11, 13, 2 );
    QTime endTime( 12, 58, 4 );
    QDateTime eventStartTime( date1, startTime );
    QDateTime eventEndTime( date2, endTime );

    QString displayLabel("Blaa blaa");
    QString description("Blah blah");

    event.setStartDateTime( eventStartTime );
    event.setEndDateTime( eventEndTime );
    event.setDisplayLabel( displayLabel );
    event.setDescription( description );
    event.setGuid(QString("joopajoo"));

    /*
    QList<QOrganizerItemRecurrenceRule> rrules;
    QOrganizerItemRecurrenceRule rule;
    rule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rule.setInterval(1);
    rrules << rule;
    event.setRecurrenceRules(rrules);
    QList<QDate> dateList;
    dateList << QDate(event.startDateTime().date());
    event.setRecurrenceDates(dateList);
    */

    QList<QOrganizerItemRecurrenceRule> rrules;
    QOrganizerItemRecurrenceRule rule;
    rule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rule.setInterval(2);
    QList<QOrganizerItemRecurrenceRule::Month> months;
    months << QOrganizerItemRecurrenceRule::August;
    rule.setMonths(months);
    QList<int> daysOfMonth;
    daysOfMonth << 5 << 6 << 10 << 21;
    rule.setDaysOfMonth(daysOfMonth);

    rrules << rule;
    event.setRecurrenceRules(rrules);

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));

    /*
    QOrganizerEvent event2;
    event2.setStartDateTime(QDateTime(QDate(2010,7,20),QTime(9,0,0)));
    event2.setEndDateTime(QDateTime(QDate(2010,7,20),QTime(10,0,0)));;
    event2.setGuid(QString("joopajoo"));
    QVERIFY(m_om->saveItem(&event2));


    QOrganizerItemFetchHint::OptimizationHints optimizationHints(QOrganizerItemFetchHint::NoBinaryBlobs);
    optimizationHints |= QOrganizerItemFetchHint::NoActionPreferences;
    QOrganizerItemFetchHint fetchHints;
    fetchHints.setOptimizationHints(optimizationHints);

    QOrganizerItem fetchItem = m_om->item(event2.id().localId(), fetchHints);
    qDebug() << fetchItem.localId();
    qDebug() << fetchItem.guid();
    */


    /*
    // Get occs
    QList<QOrganizerItem> instances = m_om->itemInstances(event,
                                                          QDateTime(QDate(2010,7,1), QTime(1,0,0)),
                                                          QDateTime(QDate(2010,7,31),QTime(23,59,59)), 0);
    for (int i = 0; i < instances.count(); ++i) {
        QOrganizerEventOccurrence o = static_cast<QOrganizerEventOccurrence>(instances[i]);
        qDebug() << "From test:";
        qDebug() << o.startDateTime();
        qDebug() << o.endDateTime();
        qDebug() << o.originalDate();
        qDebug() << o.localId();
        qDebug() << o.parentLocalId();
        qDebug() << o.guid();
    }

    QOrganizerEventOccurrence o = static_cast<QOrganizerEventOccurrence>(instances[4]);
    o.setParentLocalId(0);
    o.setStartDateTime(QDateTime(QDate(2010,7,1),QTime(9,0,0)));
    o.setEndDateTime(QDateTime(QDate(2010,7,1),QTime(10,0,0)));
    m_om->saveItem(&o);
    */
}

void tst_Maemo5Om::tmp3 ()
{

}

void tst_Maemo5Om::addSimpleItem()
{
    /*
    QOrganizerItemLocalId fetchId;
    m_om->item(fetchId);*/

     //REMOVED TEMPORALLY
    QDate date( 2010, 6, 15 );
    QTime dueTime1( 11, 13, 2 );
    QTime dueTime2( 12, 58, 4 );
    QTime dueTime3( 13, 58, 4 );

    QDateTime dateTime1( date, dueTime1 );
    QDateTime dateTime2( date, dueTime2 );
    QDateTime dateTime3( date, dueTime3 );

    // Save
    QOrganizerTodo todo;
    todo.setDueDateTime(dateTime1);
    QVERIFY(m_om->saveItem(&todo));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(todo.id().localId() != 0);
    QVERIFY(todo.id().managerUri().contains(managerName));

    // Save with list parameter
    QOrganizerTodo todo2;
    todo2.setDueDateTime( dateTime2 );
    QList<QOrganizerItem> items;
    items.append(todo2);
    QVERIFY(m_om->saveItems(&items,0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach ( QOrganizerItem item, items ) {
        QVERIFY(item.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(managerName));
    }

    // Save with list parameter and error map parameter
    QOrganizerTodo todo3;
    todo3.setDueDateTime( dateTime3 );
    QList<QOrganizerItem> items2;
    items2.append(todo3);
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items2, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach ( QOrganizerItem item2, items ) {
        QVERIFY( item2.id().localId() != 0 );
        QVERIFY( item2.id().managerUri().contains(managerName));
    }




    /*
    // Create item and set its details
    QOrganizerEvent item;
    QDate d(2010,6,14);
    QTime t1(11,0,0);
    QTime t2(12,0,0);
    QDateTime dt1(d,t1);
    QDateTime dt2(d,t2);
   
    item.setStartDateTime(dt1);
    item.setEndDateTime(dt2);
    item.setDisplayLabel("Oma itemi");
    item.setDescription("Kuvaus");

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id() != QOrganizerItemId());

    QOrganizerItemLocalId test = item.localId();
    QOrganizerEvent returned = m_om->item( test );
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(returned.startDateTime() == dt1);
    */

    /*
    QOrganizerItemId iid;
    iid.setLocalId(5454);
    item.setId(iid);
    QVERIFY(m_om->removeItem(item.localId()));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    */

    // Save with list parameter
/*
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);

    // Save with list parameter and error map parameter
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    */
}

void tst_Maemo5Om::addSimpleEvent() {
    // Create event and set its details
    QOrganizerEvent event;

    QDate date( 2010, 6, 15 );
    QTime startTime( 11, 13, 2 );
    QTime endTime( 12, 58, 4 );
    QDateTime eventStartTime( date, startTime );
    QDateTime eventEndTime( date, endTime );

    QString displayLabel("Simple event display label");
    QString description("This is a simple test event");

    event.setStartDateTime( eventStartTime );
    event.setEndDateTime( eventEndTime );
    event.setDisplayLabel( displayLabel );
    event.setDescription( description );

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));

    // Fetch back event
    QOrganizerEvent fetchEvent;
    QOrganizerItemLocalId fetchId = event.id().localId();
    fetchEvent = static_cast<QOrganizerEvent>(m_om->item(fetchId));
    QCOMPARE( fetchEvent.id(), event.id() );
    QCOMPARE( fetchEvent.startDateTime(), event.startDateTime() );
    QCOMPARE( fetchEvent.endDateTime(), event.endDateTime() );
    QCOMPARE( fetchEvent.displayLabel(), event.displayLabel() );
    QCOMPARE( fetchEvent.description(), event.description() );
}

void tst_Maemo5Om::addSimpleTodo() {
    // Create todo and set its details
    QOrganizerTodo todo;

    QDate date( 2010, 6, 16 );
    QTime time( 11, 13, 2 );
    QDateTime todoDueDateTime( date, time );

    QString displayLabel("Simple todo display label");
    QString description("This is a simple test todo");

    todo.setDueDateTime( todoDueDateTime );
    todo.setDisplayLabel( displayLabel );
    todo.setDescription( description );

    // Save todo
    QVERIFY(m_om->saveItem(&todo));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(todo.id().localId() != 0);
    QVERIFY(todo.id().managerUri().contains(managerName));

    // Fetch back todo
    QOrganizerTodo fetchTodo;
    QOrganizerItemLocalId fetchId = todo.id().localId();
    fetchTodo = static_cast<QOrganizerTodo>(m_om->item(fetchId));

    // TODO: For some reason the following does not work with todos and journals
    // TODO: The manager uri disappears in conversion => test case fails.
    // TODO: Is this a problem in the API ? The backend seems to work fine (15th June 2010)
    qDebug() << "addSimpleTodo() fails, but see the test case! URI: " << m_om->item(fetchId).id().managerUri();
    qDebug() << "Converted URI: " << static_cast<QOrganizerTodo>(m_om->item(fetchId)).id().managerUri();

    QCOMPARE( fetchTodo.id(), todo.id() );
    QCOMPARE( fetchTodo.dueDateTime(), todo.dueDateTime() );
    QCOMPARE( fetchTodo.displayLabel(), todo.displayLabel() );
    QCOMPARE( fetchTodo.description(), todo.description() );
}

void tst_Maemo5Om::addSimpleJournal() {
    // Create journal and set its details
    QOrganizerJournal journal;

    QDate date( 2010, 6, 17 );
    QTime time( 11, 13, 2 );
    QDateTime dateTime( date, time );

    QString displayLabel("Simple todo display label");
    QString description("This is a simple test todo");

    journal.setDateTime( dateTime );
    journal.setDisplayLabel( displayLabel );
    journal.setDescription( description );

    // Save journal
    QVERIFY(m_om->saveItem(&journal));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(journal.id().localId() != 0);
    QVERIFY(journal.id().managerUri().contains(managerName));

    // Fetch back journal
    QOrganizerJournal fetchJournal;
    QOrganizerItemLocalId fetchId = journal.id().localId();
    fetchJournal = static_cast<QOrganizerJournal>(m_om->item(fetchId));
    QCOMPARE( fetchJournal.id(), journal.id() );
    QCOMPARE( fetchJournal.dateTime(), journal.dateTime() );
    QCOMPARE( fetchJournal.displayLabel(), journal.displayLabel() );
    QCOMPARE( fetchJournal.description(), journal.description() );
}

void tst_Maemo5Om::removeSimpleItem() {
    QDate date( 2010, 6, 15 );
    QTime dueTime1( 11, 58, 2 );
    QTime dueTime2( 12, 58, 4 );
    QTime dueTime3( 13, 58, 4 );
    QDateTime todoDueDateTime1( date, dueTime1 );
    QDateTime todoDueDateTime2( date, dueTime2 );
    QDateTime todoDueDateTime3( date, dueTime3 );

    // Create item
    QOrganizerTodo item;
    item.setDueDateTime( todoDueDateTime1 );

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != 0 );
    QVERIFY(item.id().managerUri().contains(managerName));

    // Remove
    QVERIFY(m_om->removeItem(item.localId()));

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

void tst_Maemo5Om::addEventWithSimpleRecurrence()
{
    // Create event and set its details
    QOrganizerEvent event;

    QDate date( 2010, 6, 23 );
    QTime startTime( 11, 0, 0 );
    QTime endTime( 12, 0, 0 );
    QDateTime eventStartTime( date, startTime );
    QDateTime eventEndTime( date, endTime );

    QString displayLabel("Recurring event display label");
    QString description("This event repeats");

    event.setStartDateTime( eventStartTime );
    event.setEndDateTime( eventEndTime );
    event.setDisplayLabel( displayLabel );
    event.setDescription( description );


    QList<QOrganizerItemRecurrenceRule> rrules;

    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency( QOrganizerItemRecurrenceRule::Daily );
    //rrule.setCount( 10 );
    rrule.setInterval( 1 );
    //rrule.setWeekStart(Qt::Sunday);

    /*
    QList<int> daysOfMonth;
    daysOfMonth << 1 << 5 << 7 << 10;
    rrule.setDaysOfMonth( daysOfMonth );
    */


    /*
    QList<Qt::DayOfWeek> weekDays;
    weekDays << Qt::Sunday << Qt::Monday << Qt::Wednesday;
    rrule.setDaysOfWeek( weekDays );
    */


    //rrule.setEndDate( QDate( 2010, 12, 31 ) );


    /*
    QList<Qt::DayOfWeek> weekDays;
    weekDays << Qt::Thursday << Qt::Tuesday;
    rrule.setDaysOfWeek( weekDays );
    */

    rrules << rrule;
    event.setRecurrenceRules( rrules );

    QList<QDate> excDates;
    excDates << QDate(2010,6,25);
    event.setExceptionDates(excDates);

    /*
    qDebug() << rrules[0].interval();
    qDebug() << rrules[0].count();
    qDebug() << event.recurrenceRules()[0].interval();
    qDebug() << event.recurrenceRules()[0].count();
    */


    /*
    qDebug() << event.recurrenceRules()[0].interval();
    QOrganizerEvent* original = static_cast<QOrganizerEvent*>( &event );
    qDebug() << original->recurrenceRules()[0].interval();

    QList<QOrganizerItem> list;
    list.append( event );

    QOrganizerEvent* retrieved = static_cast<QOrganizerEvent*>( &list[0] );
    qDebug() << retrieved->recurrenceRules()[0].interval();
    */

    //event.setExceptionRules( rrules );

    // Save event
    QVERIFY(m_om->saveItem(&event));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(event.id().localId() != 0);
    QVERIFY(event.id().managerUri().contains(managerName));


    QOrganizerEvent fetchEvent;
    QOrganizerItemLocalId fetchId = event.id().localId();
    fetchEvent = static_cast<QOrganizerEvent>(m_om->item(fetchId));

    /*
    QOrganizerItemId emptyId;
    fetchEvent.setId(emptyId);
    fetchEvent.setDisplayLabel(QString("This is a copy"));
    m_om->saveItem(&fetchEvent);
    */



    /*
    QList<QOrganizerItem> result = m_om->itemInstances( event, QDateTime( QDate( 2010, 1, 1)), QDateTime(QDate(2010,12,31)), 10 );
    qDebug() << result.count();
    */

    // TODO: Verify recurrence when possible
}

void tst_Maemo5Om::addWithIllegalParameters()
{
    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    // TODO: Should this be that way? Personally I don't consider it as an
    // error if there's just nothing to save. -MarkoJM
    QList<QOrganizerItem> items;
    QVERIFY(!m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);
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
