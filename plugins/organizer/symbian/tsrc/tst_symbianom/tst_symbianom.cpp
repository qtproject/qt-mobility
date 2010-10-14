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

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif



#define QTRY_COMPARE_SIGNAL_COUNTS() \
    QTRY_COMPARE(spyAdded.count(), itemsAddedSignals); \
    QTRY_COMPARE(spyChanged.count(), itemsChangedSignals); \
    QTRY_COMPARE(spyRemoved.count(), itemsRemovedSignals); \
    QTRY_COMPARE(spyDataChanged.count(), dataChangedSignals); \
    if (spyAdded.count()) \
        { QCOMPARE(spyAdded.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsAdded); } \
    if (spyChanged.count()) \
        { QCOMPARE(spyChanged.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsChanged); } \
    if (spyRemoved.count()) \
        { QCOMPARE(spyRemoved.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsRemoved); }

#define QTRY_COMPARE_SIGNAL_COUNTS2() \
    QTRY_COMPARE(spyAdded2.count(), itemsAddedSignals); \
    QTRY_COMPARE(spyChanged2.count(), itemsChangedSignals); \
    QTRY_COMPARE(spyRemoved2.count(), itemsRemovedSignals); \
    QTRY_COMPARE(spyDataChanged2.count(), dataChangedSignals); \
    if (spyAdded2.count()) \
        { QCOMPARE(spyAdded2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsAdded); } \
    if (spyChanged2.count()) \
        { QCOMPARE(spyChanged2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsChanged); } \
    if (spyRemoved2.count()) \
        { QCOMPARE(spyRemoved2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsRemoved); }


const QString m_managerNameSymbian("symbian");

/*!
 * A test helper class that acts as a placeholder for QOrganizerItemDetail data.
 */
class QTstDetailField
{
public:
    QTstDetailField(QString definitionName, QString fieldName, QVariant value)
    {
        m_definitionName = definitionName;
        m_fieldName = fieldName;
        m_value = value;
    }
    QString m_definitionName;
    QString m_fieldName;
    QVariant m_value;
};

typedef QList<QTstDetailField> QTstDetailFieldList;
Q_DECLARE_METATYPE(QTstDetailFieldList)

Q_DECLARE_METATYPE(QList<QOrganizerItemLocalId>)

/*!
 * For testing symbian backend via QOrganizerManager API. The target is
 * to implement test cases in a platform independent manner so that this test
 * module could be used also with other backends and potentially used as a
 * QtMobility auto test with as little porting work as possible.
 */
class tst_SymbianOm : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases
    void addSimpleItem_data(){ addManagers(); };
    void addSimpleItem();
    void fetchSimpleItem_data(){ addManagers(); };
    void fetchSimpleItem();
    void removeSimpleItem_data(){ addManagers(); };
    void removeSimpleItem();
    void fetchItems_data(){ addManagers(); };
    void fetchItems();
    void fetchItemIds_data(){ addManagers(); };
    void fetchItemIds();
    void uniqueIds_data(){ addManagers(); };
    void uniqueIds();
    void timeStamp_data(){ addManagers(); };
    void timeStamp();
    void addItem_data();
    void addItem();
    void signalEmission_data(){ addManagers(); };
    void signalEmission();
    void invalidDetail_data(){ addManagers(); };
    void invalidDetail();
    void addReminderToSingleInstance_data(){ addManagers(); };
	void addReminderToSingleInstance();
	void removeReminderFromSingleInstance_data(){ addManagers(); };
	void removeReminderFromSingleInstance();
	void timezone_data() { addManagers(); };
	void timezone();
    void supportedItemTypes_data() { addManagers(); };
    void supportedItemTypes();

private:
    // TODO: enable the following test cases by moving them to "private slots"
    // when the required functionality has been implemented in symbian backend

    // Negative test cases (testing for example with illegal parameters)
    void addNegative_data(){ addManagers(); };
    void addNegative();
    void fetchNegative_data(){ addManagers(); };
    void fetchNegative();
    void removeNegative_data(){ addManagers(); };
    void removeNegative();

private: // util functions
    void addManagers();
    void addItem_dataBasic(QString managerName, QString itemType);
    void addItem_dataEventTime(QString managerName, QString itemType);
    void addItem_dataTodoTime(QString managerName, QString itemType);
    void addItem_dataReminders(QString managerName, QString itemType);
    QString testCaseName(QString testCase, QString managerName, QString itemType);
    bool parseDetails(QTstDetailFieldList detailsString, QList<QOrganizerItemDetail> &details);
    bool verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected);
    bool isKnownIssue(QOrganizerItemDetail actual, QOrganizerItemDetail expected);
    bool compareItemLists(QList<QOrganizerItem> actual, QList<QOrganizerItem> expected, QStringList ignoredDetails = QStringList());
    bool compareItems(QOrganizerItem itemA, QOrganizerItem itemB, QStringList ignoredDetails = QStringList());    

private:
    QOrganizerManager *m_om;
    TTimeIntervalSeconds m_UTCOffset;
};


void tst_SymbianOm::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds());
	
    // Save UTC offset
    m_UTCOffset = User::UTCOffset();
}

void tst_SymbianOm::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds());
    delete m_om;
    m_om = 0;
	
    // Restore UTC offset
    User::SetUTCOffset(m_UTCOffset);
}

void tst_SymbianOm::addSimpleItem()
{
    // Create item and set it's details
    QOrganizerItem item;
    // This is the simplest possible item, not even a display label or time
    // range. The item type used is TypeTodo because adding event without
    // time range will crash (panic "Agenda model" 1) on symbian.
    item.setType(QOrganizerItemType::TypeTodo);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(item.id().localId() != QOrganizerItemLocalId());
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    foreach (QOrganizerItem listitem, items) {
        QVERIFY(listitem.id().localId() != QOrganizerItemLocalId());
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));
    }

    // Save with list parameter and error map parameter
    QVERIFY(m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(m_om->errorMap().count() == 0);
    foreach (QOrganizerItem listitem2, items) {
        QVERIFY(listitem2.id().localId() != QOrganizerItemLocalId());
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));
    }
}

void tst_SymbianOm::fetchSimpleItem()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeTodo);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != QOrganizerItemLocalId());
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Fetch
    QOrganizerItem fetchedItem = m_om->item(item.localId());
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(fetchedItem.id(), item.id());
    QVERIFY(fetchedItem.type() == QOrganizerItemType::TypeTodo);
}

void tst_SymbianOm::removeSimpleItem()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeTodo);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != QOrganizerItemLocalId());
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Remove
    QVERIFY(m_om->removeItem(item.localId()));
    
    // Remove again. Should fail.
    QVERIFY(!m_om->removeItem(item.localId()));

    // Remove list
    QOrganizerItem item2;
    item2.setType(QOrganizerItemType::TypeTodo);
    QOrganizerItem item3;
    item3.setType(QOrganizerItemType::TypeTodo);
    QVERIFY(m_om->saveItem(&item2));
    QVERIFY(m_om->saveItem(&item3));
    QList<QOrganizerItemLocalId> itemIds;
    itemIds.append(item2.localId());
    itemIds.append(item3.localId());
    QVERIFY(m_om->removeItems(itemIds));

    // Remove with itemIds
    QOrganizerItem item4;
    item4.setType(QOrganizerItemType::TypeTodo);
    QOrganizerItem item5;
    item5.setType(QOrganizerItemType::TypeTodo);
    QVERIFY(m_om->saveItem(&item4));
    QVERIFY(m_om->saveItem(&item5));
    QVERIFY(m_om->removeItems(m_om->itemIds()));
}

void tst_SymbianOm::fetchItems()
{
    // Add some items
    QList<QOrganizerItem> expectedItems;
    for (int i=0; i<10; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        expectedItems << item;
    }

    // Fetch ids
    QList<QOrganizerItem> actualItems = m_om->itemsForExport();
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
    
    // Verify
    QVERIFY(compareItemLists(actualItems, expectedItems));
}

void tst_SymbianOm::fetchItemIds()
{
    // Add some items
    QList<QOrganizerItemLocalId> expectedIds;
    for (int i=0; i<10; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        expectedIds << item.localId();
    }

    // Fetch ids
    QList<QOrganizerItemLocalId> actualIds = m_om->itemIds();
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
    
    // Verify
    QVERIFY(actualIds.count() == expectedIds.count());
    foreach (QOrganizerItemLocalId id, actualIds) {
        QVERIFY(id != QOrganizerItemLocalId());
        QVERIFY(expectedIds.contains(id));
    }
}

void tst_SymbianOm::uniqueIds()
{
    const int count = 10;
    QList<QOrganizerItem> items;
    QList<QOrganizerItemLocalId> lids;
    QList<QString> guids;

    // Test that new items have unique ids
    for (int i=0; i<count; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        QVERIFY(!lids.contains(item.localId()));
        QVERIFY(!guids.contains(item.guid()));
        lids << item.localId();
        guids << item.guid();
        items << item;
    }
    
    // Modify and check that local id and global uid persists
    for (int i=0; i<count; i++) {
        QOrganizerItem &item = items[i];
        item.setDisplayLabel("foobar");
        QVERIFY(m_om->saveItem(&item));
        QVERIFY(lids[i] == item.localId());
        QVERIFY(guids[i] == item.guid());
    }
    
    // Load and check that ids are the still the same
    for (int i=0; i<count; i++) {
        QOrganizerItem item = m_om->item(lids[i]);
        QVERIFY(m_om->error() == QOrganizerManager::NoError);
        QVERIFY(lids[i] == item.localId());
        QVERIFY(guids[i] == item.guid());
    }
    
    // Try saving a removed item without clearing local id. Should fail
    QOrganizerTodo todo(items[0]);
    QVERIFY(m_om->removeItem(todo.localId()));
    QVERIFY(!m_om->saveItem(&todo));
    
    // Try saving a removed item without clearing local id and with own guid. Should fail.
    todo = QOrganizerTodo(items[1]);
    todo.setGuid("11111");
    QVERIFY(m_om->removeItem(todo.localId()));
    QVERIFY(!m_om->saveItem(&todo));
    
    // Save a new todo item with own guid. Should pass.
    todo = QOrganizerTodo();
    todo.setGuid("1234567890");
    QVERIFY(m_om->saveItem(&todo));
    QVERIFY(todo.guid() == "1234567890");
    
    // Save an existing todo item with guid only. Should pass.
    QOrganizerItemLocalId localId = todo.localId();
    todo.setDescription("foobar");
    todo.setId(QOrganizerItemId());
    QVERIFY(m_om->saveItem(&todo));
    QVERIFY(todo.localId() == localId); // local id should remain the same
    
    // Change manager uri and save again. Should fail.
    QOrganizerItemId id = todo.id();
    id.setManagerUri("foobar");
    todo.setId(id);
    QVERIFY(!m_om->saveItem(&todo));    
}

void tst_SymbianOm::timeStamp()
{
    // TODO: support for created time? is it possible with symbian?
    // TODO: check detail definitions
    
    QDateTime startTime = QDateTime::currentDateTime();
    //qDebug() << "starttime:" << startTime;
    
    // Save item
    QOrganizerTodo item1;
    item1.setDisplayLabel("do stuff");
    QVERIFY(m_om->saveItem(&item1));
    
    // Verify timestamp
    QOrganizerItemTimestamp timeStamp1 = item1.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp1" << timeStamp1;
    QVERIFY(!timeStamp1.isEmpty());
    QVERIFY(timeStamp1.created().isValid());
    QVERIFY(timeStamp1.lastModified().isValid());
    QVERIFY(timeStamp1.created() >= startTime);
    QVERIFY(timeStamp1.lastModified() >= startTime);    
        
    // Wait a while
    QTest::qSleep(1000);

    // Save again
    item1.setDisplayLabel("do more stuff");
    QVERIFY(m_om->saveItem(&item1));
    
    // Verify timestamp
    QOrganizerItemTimestamp timeStamp2 = item1.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp2" << timeStamp2;
    QVERIFY(!timeStamp2.isEmpty());
    QVERIFY(timeStamp2.created().isValid());
    QVERIFY(timeStamp2.created() == timeStamp1.created());
    QVERIFY(timeStamp2.lastModified().isValid());
    QVERIFY(timeStamp2.lastModified() > timeStamp1.lastModified());
        
    // Load the same item again
    QOrganizerTodo item2 = m_om->item(item1.localId());
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
    QOrganizerItemTimestamp timeStamp3 = item2.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp3" << timeStamp3;
    QVERIFY(timeStamp3.lastModified() == timeStamp2.lastModified());
}

void tst_SymbianOm::addNegative()
{
    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0));
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);

    QList<QOrganizerItem> items;
    QVERIFY(!m_om->saveItems(&items));
    QCOMPARE(m_om->error(), QOrganizerManager::BadArgumentError);

    // TODO: try to save an event with non-existing (non-zero) id and check that it fails
}

void tst_SymbianOm::fetchNegative()
{
    // TODO: negative cases: fetch with non-existing id, ...
}

void tst_SymbianOm::removeNegative()
{
    // TODO: negative cases: remove with empty list, non-existing item, multiple non-existing items...
}

void tst_SymbianOm::addItem_data()
{
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<int>("expectedError");
    QTest::addColumn<QTstDetailFieldList>("detailsList");

    QStringList managerNames = QOrganizerManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    managerNames.removeAll("memory");

    foreach (QString managerName, managerNames) {
        // TypeEvent
        addItem_dataBasic(managerName, QOrganizerItemType::TypeEvent);
        addItem_dataEventTime(managerName, QOrganizerItemType::TypeEvent);
        addItem_dataReminders(managerName, QOrganizerItemType::TypeEvent);

        // TypeTodo
        addItem_dataBasic(managerName, QOrganizerItemType::TypeTodo);
        addItem_dataTodoTime(managerName, QOrganizerItemType::TypeTodo);
        addItem_dataReminders(managerName, QOrganizerItemType::TypeTodo);

        // TypeJournal not implementable on symbian backend
        //addItem_dataBasic(managerName, QOrganizerItemType::TypeJournal);

        // TypeNote not supported in all platforms
#ifdef AGENDA_EXT_SUPPORT
        addItem_dataBasic(managerName, QOrganizerItemType::TypeNote);
#endif
    }
}

void tst_SymbianOm::addItem()
{
    // Fetch parameters
    QFETCH(int, expectedError);
    QFETCH(QTstDetailFieldList, detailsList);

    QOrganizerManager::Error expectedErrorCode = (QOrganizerManager::Error) expectedError; 
    QList<QOrganizerItemDetail> details;
    QVERIFY(parseDetails(detailsList, details));

    // Initialize: Create test item and set it's details
    QOrganizerItem item;
    foreach(QOrganizerItemDetail detail, details) {
        QVERIFY(item.saveDetail(&detail));
        /* TODO: enable the following and implement detail definitions to symbian backend 
        QOrganizerItemDetailDefinition def = m_om->detailDefinition(detail.definitionName(), item.type());
        if (m_om->error() != QOrganizerManager::NoError) {
            // this detail is not supported, skip the test case
            QString skipMsg = QString("Detail ") + detail.definitionName() + QString(" not supported for item type " + item.type());
            QSKIP(skipMsg.toAscii(), SkipSingle);
        }
        */
    }

    // Add, fetch and verify result
    if (expectedErrorCode == QOrganizerManager::NoError) {
        // Save
        QVERIFY(m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
        QVERIFY(item.id().localId() != QOrganizerItemLocalId());
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));

        // Fetch item to verify everything was saved successfully
        QOrganizerItem savedItem = m_om->item(item.localId());
        QCOMPARE(m_om->error(), QOrganizerManager::NoError);
        QVERIFY(verifyDetails(savedItem.details(), details));
    } else {
        // An error is expected -> Check that save fails and the error code
        // is the correct one
        QVERIFY(!m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
    }
}

void tst_SymbianOm::signalEmission()
{
    QFETCH(QString, managerName);
    
    // Create a second manager
    QOrganizerManager om2(managerName);
    
    // Connect signal spies
    qRegisterMetaType<QOrganizerItemLocalId>("QOrganizerItemLocalId");
    qRegisterMetaType<QList<QOrganizerItemLocalId> >("QList<QOrganizerItemLocalId>");
    QSignalSpy spyAdded(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyChanged(m_om, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyRemoved(m_om, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyDataChanged(m_om, SIGNAL(dataChanged()));
    QSignalSpy spyAdded2(&om2, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyChanged2(&om2, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyRemoved2(&om2, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyDataChanged2(&om2, SIGNAL(dataChanged()));
    int itemsAddedSignals = 0;
    int itemsChangedSignals = 0;
    int itemsRemovedSignals = 0;
    int dataChangedSignals = 0;
    int itemsAdded = 0;
    int itemsChanged = 0;
    int itemsRemoved = 0;
    
    // Save
    QOrganizerTodo todo;
    QVERIFY(m_om->saveItem(&todo));
    itemsAddedSignals++;
    itemsAdded = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Change
    todo.setDescription("foobar");
    QVERIFY(m_om->saveItem(&todo));
    itemsChangedSignals++;
    itemsChanged = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Change but with guid only (local id not defined)
    todo.setDescription("barfoo");
    todo.setId(QOrganizerItemId());
    QVERIFY(m_om->saveItem(&todo));
    itemsChangedSignals++;
    itemsChanged = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();    
    
    // Remove
    QVERIFY(m_om->removeItem(todo.localId()));
    itemsRemovedSignals++;
    itemsRemoved = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Save - batch
    todo.setId(QOrganizerItemId());
    todo.setGuid(QString());
    QOrganizerTodo todo2;
    QList<QOrganizerItem> items;
    items << todo;
    items << todo2;
    QVERIFY(m_om->saveItems(&items));
    itemsAddedSignals++;
    itemsAdded = 2;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Change - batch
    items[0].setDescription("foobar1");
    items[1].setDescription("foobar2");
    QVERIFY(m_om->saveItems(&items));
    itemsChangedSignals++;
    itemsChanged = 2;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Remove - batch
    QList<QOrganizerItemLocalId> itemIds;
    itemIds << items[0].localId();
    itemIds << items[1].localId();
    QVERIFY(m_om->removeItems(itemIds));
    itemsRemovedSignals++;
    itemsRemoved = 2;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
}

void tst_SymbianOm::invalidDetail()
{
    // NOTE: There is an auto test about this (tst_QOrganizerManager::itemValidation)
    // but its not working currently on symbian backend. This test can be removed
    // when it's compatible with symbian backend.
    QOrganizerTodo todo;
    QOrganizerItemDetail invalidDetail("invalid");
    invalidDetail.setValue("foo", "bar");
    QVERIFY(todo.saveDetail(&invalidDetail));
    QVERIFY(!m_om->saveItem(&todo));
    QVERIFY(m_om->error() == QOrganizerManager::InvalidDetailError);
}

/*!
 * Creates an exceptional entry by adding reminder detail to single instance of a repeating entry
 */
void tst_SymbianOm::addReminderToSingleInstance()
{
    // Repeating event without reminder
    QOrganizerItem repeatingEvent;
    repeatingEvent.setType(QOrganizerItemType::TypeEvent);
    
    // Add the start and end time
    QOrganizerEventTime timeRange;
    QDateTime startDateTime = QDateTime::currentDateTime();
    timeRange.setStartDateTime(startDateTime);
    timeRange.setEndDateTime(startDateTime.addSecs(3000));
    
    // Create a daily recurrence rule
    QOrganizerItemRecurrence recurrence;
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(3);
    QSet<QOrganizerRecurrenceRule> list;
    list.insert(rrule);
    recurrence.setRecurrenceRules(list);
    
    // Save the item
    repeatingEvent.saveDetail(&timeRange);
    repeatingEvent.saveDetail(&recurrence);
    QVERIFY(m_om->saveItem(&repeatingEvent));
    
    // Fetch the instances
    QList<QOrganizerItem> itemOccurrences = m_om->itemOccurrences(repeatingEvent, startDateTime, QDateTime(), 3);
    QVERIFY(itemOccurrences.count() == 3);


    // Verify that reminder detail is empty
    QOrganizerItem instance1 = itemOccurrences.at(0);
    QOrganizerItemReminder rptReminder = instance1.detail<QOrganizerItemReminder>();
    QVERIFY(rptReminder.isEmpty());
    
    // Add reminder detail to create an exceptional entry
    rptReminder.setSecondsBeforeStart(300);
    instance1.saveDetail(&rptReminder);
    QVERIFY(m_om->saveItem(&instance1));

    // Verify that the exceptional entry has been created
    instance1 = m_om->item(instance1.localId());
    rptReminder = instance1.detail<QOrganizerItemReminder>();
    QVERIFY(!rptReminder.isEmpty());

    // Verify that the other instances have not been modified
    itemOccurrences = m_om->itemOccurrences(repeatingEvent, startDateTime, QDateTime(), 3);
    QCOMPARE(itemOccurrences.count(), 3);
    QVERIFY(itemOccurrences.at(1).detail<QOrganizerItemReminder>().isEmpty());
}

/*!
 * Creates an exceptional entry by removing reminder detail from a single instance of a repeating entry
 */
void tst_SymbianOm::removeReminderFromSingleInstance()
{
    // Repeating event with reminder
    QOrganizerItem repeatingEvent;
    repeatingEvent.setType(QOrganizerItemType::TypeEvent);
    
    // Add the start and end time
    QOrganizerEventTime timeRange;
    QDateTime startDateTime = QDateTime::currentDateTime();
    timeRange.setStartDateTime(startDateTime);
    timeRange.setEndDateTime(startDateTime.addSecs(3000));
    
    // Create a daily recurrence rule
    QOrganizerItemRecurrence recurrence;
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrule.setLimit(3);
    QSet<QOrganizerRecurrenceRule> list;
    list.insert(rrule);
    recurrence.setRecurrenceRules(list);
    
    // Add reminder detail
    QOrganizerItemReminder repeatReminder;
    repeatReminder.setSecondsBeforeStart(300);
    
    // Save the item
    repeatingEvent.saveDetail(&timeRange);
    repeatingEvent.saveDetail(&recurrence);
    repeatingEvent.saveDetail(&repeatReminder);
    QVERIFY(m_om->saveItem(&repeatingEvent));
    
    // Fetch the instances
    QList<QOrganizerItem> itemOccurrences = m_om->itemOccurrences(repeatingEvent, startDateTime, QDateTime(), 3);
    QVERIFY(itemOccurrences.count() == 3);
    
    // Modify first instance by removing reminder detail and save
    QOrganizerItem instance1 = itemOccurrences.at(0);
    QOrganizerItemReminder rptReminder = instance1.detail<QOrganizerItemReminder>();
    QCOMPARE(rptReminder.secondsBeforeStart(), repeatReminder.secondsBeforeStart());
    instance1.removeDetail(&rptReminder);
    QVERIFY(m_om->saveItem(&instance1));

    // Verify that an exception has been created
    instance1 = m_om->item(instance1.localId());
    rptReminder = instance1.detail<QOrganizerItemReminder>();
    QVERIFY(rptReminder.isEmpty());
	
    // Check if the other instances are intact
    itemOccurrences = m_om->itemOccurrences(repeatingEvent, startDateTime, QDateTime(), 3);
    QCOMPARE(itemOccurrences.count(), 3);
    QVERIFY(!itemOccurrences.at(1).detail<QOrganizerItemReminder>().isEmpty());
}

void tst_SymbianOm::timezone()
{
    // Set local time to UTC+2
    User::SetUTCOffset(2*60*60);
    
    // Save a simple event 
    QOrganizerEvent event;
    event.setDisplayLabel("test timezone");
    QDateTime startDateTime(QDate(2010, 1, 1));
    QDateTime endDateTime = startDateTime.addSecs(60*60);
    event.setStartDateTime(startDateTime);
    event.setEndDateTime(endDateTime);
    QVERIFY(m_om->saveItem(&event));
    
    // Fetch & verify
    event = m_om->item(event.localId());
    QVERIFY(event.startDateTime() == startDateTime);
    QVERIFY(event.endDateTime() == endDateTime);
       
    // Set local time to UTC+3
    User::SetUTCOffset(3*60*60);
    
    // Fetch & verify
    event = m_om->item(event.localId());
    QVERIFY(event.startDateTime() == startDateTime);
    QVERIFY(event.endDateTime() == endDateTime);
}

void tst_SymbianOm::supportedItemTypes()
{
    // Verify item type support
    QVERIFY(m_om->supportedItemTypes().count() > 0);
    foreach (QString itemType, m_om->supportedItemTypes()) {
        QVERIFY(!m_om->detailDefinitions(itemType).isEmpty());
        // TODO: Try saving an item with the given type.
        // This would require some logic though.
    }
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_SymbianOm::addManagers()
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

/**
 * Adds event related test data for test case "addItem" for backend "managerName".
 */
void tst_SymbianOm::addItem_dataBasic(QString managerName, QString itemType)
{
    // The default detail list contains only the type (event)
    QTstDetailFieldList itemTypeDetails;
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    // There is a bug in Symbian calendar engine. It does not allow
    // creating an event without a start time.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName == m_managerNameSymbian
        && itemType == QOrganizerItemType::TypeEvent) {
        QTest::newRow(testCaseName("item without details", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::BadArgumentError // Symbian calendar server requires start time for an event
            << QTstDetailFieldList(itemTypeDetails);

        QTest::newRow(testCaseName("item with description", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::BadArgumentError // Symbian calendar server requires start time for an event
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    } else {
        QTest::newRow(testCaseName("item without details", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << QTstDetailFieldList(itemTypeDetails);

        QTest::newRow(testCaseName("item with description", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    }
}

/*!
 * Helper function for adding time rage related test data (QTest rows).
 * Note: the test cases are actually type dependent, because the time range
 * detail is type dependent.
 */
void tst_SymbianOm::addItem_dataEventTime(QString managerName, QString itemType)
{
    // The default detail list contains only the type (event)
    QTstDetailFieldList itemTypeDetails = QTstDetailFieldList();
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    QTest::newRow(testCaseName("item with start time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldStartDateTime, QDateTime::currentDateTime()));

    // There is a bug in symbian calendar server: An end datetime that is
    // exactly the same as the start datetime is ignored.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with start time = end time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldStartDateTime, QDateTime::currentDateTime())
                << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldEndDateTime, QDateTime::currentDateTime()));
    }

    QTest::newRow(testCaseName("item with start time < end time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));

    QTest::newRow(testCaseName("item with start time > end time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldStartDateTime, QDateTime::currentDateTime().addSecs(3600))
            << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldEndDateTime, QDateTime::currentDateTime()));

    // Symbian calendar server makes the client process panic in case the start
    // time of an event (calendar event of type EAppt) is left empty.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        // This test case matches the iCalendar specification
        QTest::newRow(testCaseName("item with end time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    } else {
        // This test case is against the iCalendar specification
        QTest::newRow(testCaseName("item with end time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::BadArgumentError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerEventTime::DefinitionName, QOrganizerEventTime::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    }
}

void tst_SymbianOm::addItem_dataReminders(QString managerName, QString itemType)
{
    QTstDetailFieldList itemTypeDetails;
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    QString timeRangeDefinition = QOrganizerEventTime::DefinitionName;
    QString fieldStartDateTime = QOrganizerEventTime::FieldStartDateTime;
    QString fieldEndDateTime = QOrganizerEventTime::FieldEndDateTime;

    if (itemType == QOrganizerItemType::TypeTodo) {
        timeRangeDefinition = QLatin1String(QOrganizerTodoTime::DefinitionName);
        fieldStartDateTime = QLatin1String(QOrganizerTodoTime::FieldStartDateTime);
        fieldEndDateTime = QLatin1String(QOrganizerTodoTime::FieldDueDateTime);
    }

    QTest::newRow(testCaseName("item with reminder time delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // datetime reminder after half an hour
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, 1800));

    // There is a known issue in symbian calendar server, the datetimes seem to
    // be rounded to one day accuracy. In other words you cannot create for
    // example an ETodo CCalEntry with startDateTime after one hour and
    // dueDateTime after 24 hours. Also, you cannot create an ETodo CCalEntry
    // with only a startDateTime.
    // *** TODO: File a bug report against symbian calendar server in
    // Symbian S^3 and S^4!
    if (itemType != QOrganizerItemType::TypeTodo) {
        QTest::newRow(testCaseName("item with reminder starttime", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
                // start after an hour
                << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addSecs(3600))
                // datetime reminder after half an hour
                << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, 1800));
    }

    QTest::newRow(testCaseName("item with reminder starttime after 3 hours, 2 secs", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start now
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // datetime reminder ~3 hours ago
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, 11802));

    QTest::newRow(testCaseName("item with reminder starttime after 30 days", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start after 30 days
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(30))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(31))
            // datetime reminder after (29 days, 23 hours and 59 minutes)
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, 2591940));

    QTest::newRow(testCaseName("item with reminder starttime after 365 days", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start after 365 days
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(365))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(366))
            // datetime reminder after 364 days
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, 364*24*60*60));

    QTest::newRow(testCaseName("item with reminder starttime in the past", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start now
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(1))
            // datetime reminder half an hour ago
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart,-1800));

    QTest::newRow(testCaseName("item with reminder delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // delta reminder half an hour before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, int(1800)));

    QTest::newRow(testCaseName("item with small reminder delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // delta reminder 65 seconds before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, int(65)));

    QTest::newRow(testCaseName("item with smallest reminder delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // delta reminder 1 second before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, int(1)));

    QTest::newRow(testCaseName("item with reminder delta in past", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start now
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(1))
            // delta reminder half an hour before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldSecondsBeforeStart, int(-1800)));
}

/*!
 * Helper function for adding time rage related test data (QTest rows).
 * Note: the test cases are actually type dependent, because the time range
 * detail is type dependent.
 */
void tst_SymbianOm::addItem_dataTodoTime(QString managerName, QString itemType)
{
    // The default detail list contains only the type (todo)
    QTstDetailFieldList itemTypeDetails;
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    // There is a bug in symbian calendar server that makes an empty
    // "not-before" datetime to be replaced with the same value as "due"
    // datetime if only "due" datetime was defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with due time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime())); 
    }

    QTest::newRow(testCaseName("item with due time == not-before time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime()));

    QTest::newRow(testCaseName("item with due time > not-before time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime()));

    QTest::newRow(testCaseName("item with due time < not-before time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));

    // There is a bug in symbian calendar server which makes the "not-before"
    // datetime to be ignored if no "due" datetime is defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with not-before time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));
    }

    QTstDetailField timeStart(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime());
    QTstDetailField timeDue(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1));
    
    QTest::newRow(testCaseName("item with progress StatusNotStarted", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusNotStarted));
    
    QTest::newRow(testCaseName("item with progress StatusInProgress", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusInProgress));
    
    QTest::newRow(testCaseName("item with progress StatusComplete", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete)
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime, QDateTime::currentDateTime()));    

    QTest::newRow(testCaseName("item with progress StatusComplete, no finished date", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete));    

    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with progress percentage", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << timeStart
                << timeDue
                << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete, 50));
    } else {
        QTest::newRow(testCaseName("item with progress percentage", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerManager::InvalidDetailError
            << (QTstDetailFieldList(itemTypeDetails)
                << timeStart
                << timeDue
                << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete, 50));
    }
}

/*!
 * Helper function for generating test case name. The name is given in the
 * following format: "[managerName][itemType] testCase"
 */
QString tst_SymbianOm::testCaseName(QString testCase, QString managerName, QString itemType)
{
    return QString("[%1][%2] %3")
        .arg(managerName)
        .arg(itemType)
        .arg(testCase)
        .toLatin1().constData();
}

/*!
 * Helper function for parsing QTstDetailFields into organizer item details
 */
bool tst_SymbianOm::parseDetails(QTstDetailFieldList detailsList, QList<QOrganizerItemDetail> &details)
{
    foreach (QTstDetailField tstdetail, detailsList) {
        QOrganizerItemDetail detail(tstdetail.m_definitionName);
        int existingIndex(-1);
        foreach (QOrganizerItemDetail existingDetail, details) {
            if (existingDetail.definitionName() == detail.definitionName()) {
                // Replace the existing detail in the list so that the new
                // field is added to the existing detail
                // TODO: check that the field does not exist already
                existingIndex = details.indexOf(existingDetail);
                detail = existingDetail;
            }
        }

        if (detail.setValue(tstdetail.m_fieldName, tstdetail.m_value)) {
            // If a detail with same name exists, replace
            if (existingIndex != -1) {
                details.replace(existingIndex, detail);
            } else {
                details.append(detail);
            }
        } else {
            qDebug() << QString("Failed to set field value!");
            return false;
        }
    }
    return true;
}

/*!
 * Helper function for verifying details against reference details
 */
bool tst_SymbianOm::verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected)
{
	foreach (QOrganizerItemDetail expectedDetail, expected) {
        bool found(false);
        foreach (QOrganizerItemDetail actualDetail, actual) {

            if (actualDetail.definitionName() == expectedDetail.definitionName()) {
                // Compare 
                if (actualDetail.variantValues() == expectedDetail.variantValues()) {
                    // A valid detail with the same field values found
                    found = true;
                    break;
                } else {
                    // Check if this is one of the known issues
                    if (isKnownIssue(actualDetail, expectedDetail)) {
                        found = true;
                    } else {
                        qDebug() << "Detail value mismatch " << expectedDetail.definitionName();
                        qDebug() << "Actual  :" << actualDetail.variantValues();
                        qDebug() << "Expected:" << expectedDetail.variantValues();
                        return false;
                    }
                }
            }
        }

        if (!found) {
            qDebug() << "Expected detail not found:" << expectedDetail.definitionName();
            return false;
        }
    }

    return true;
}

bool tst_SymbianOm::isKnownIssue(QOrganizerItemDetail actual, QOrganizerItemDetail expected)
{
    if(actual.definitionName() == QOrganizerItemReminder::DefinitionName
        && expected.definitionName() == QOrganizerItemReminder::DefinitionName) {
        QOrganizerItemReminder actualReminder = actual;
        QOrganizerItemReminder expectedReminder = expected;

        // Known issue:
        // Symbian calendar server saves reminder delta as minutes.
        // QOrganizerItemReminder::FieldSecondsBeforeStart uses seconds.
        // This causes a rounding issue.
        if(expectedReminder.secondsBeforeStart()
            && actualReminder.secondsBeforeStart() != expectedReminder.secondsBeforeStart()
            && actualReminder.secondsBeforeStart() == ((expectedReminder.secondsBeforeStart() / 60) * 60)) {
            qWarning() << "Known issue 1";
            return true;
        }
    }

    return false;
}

bool tst_SymbianOm::compareItemLists(QList<QOrganizerItem> actual, QList<QOrganizerItem> expected, QStringList ingoredDetails)
{
    // Remove matching contacts
    foreach (QOrganizerItem a, actual) {
        foreach (QOrganizerItem b, expected) {
            if (compareItems(a, b, ingoredDetails)) {
                actual.removeOne(a);
                expected.removeOne(b);
                break;
            }
        }
    }
    
    if (actual.count()) {
        qDebug() << "Extra items:";
        qDebug() << actual;
    }
    
    if (expected.count()) {
        qDebug() << "Missing items:";
        qDebug() << expected;
    }    
    
    if (actual.count() == 0 && expected.count() == 0)
        return true;
    
    return false;
}

bool tst_SymbianOm::compareItems(QOrganizerItem itemA, QOrganizerItem itemB, QStringList ignoredDetails)
{
    if (itemA.localId() != itemB.localId())
        return false;
    
    QList<QOrganizerItemDetail> aDetails = itemA.details();
    QList<QOrganizerItemDetail> bDetails = itemB.details();
    
    // Remove ignored details
    foreach (QString ignored, ignoredDetails) { 
        foreach (QOrganizerItemDetail d, aDetails) {
            if (d.definitionName() == ignored)
                itemA.removeDetail(&d);
        }
    }
    
    foreach (QString ignored, ignoredDetails) { 
        foreach (QOrganizerItemDetail d, bDetails) {
            if (d.definitionName() == ignored)
                itemB.removeDetail(&d);
        }
    }
    
    // They can be in any order, so loop
    foreach (QOrganizerItemDetail d, aDetails) {
        foreach (QOrganizerItemDetail d2, bDetails) {
            if (d == d2) {
                itemA.removeDetail(&d);
                itemB.removeDetail(&d2);
                break;
            }
        }
    }
    
    if (itemA == itemB)
        return true;
    
    return false;
}

QTEST_MAIN(tst_SymbianOm);
#include "tst_symbianom.moc"
