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

/*!
 * A test helper class that acts as a placeholder for QOrganizerItemDetail data.
 */
class QTstDetail
{
public:
    QTstDetail(QString definitionName, QString fieldName, QVariant value)
    {
        m_definitionName = definitionName;
        m_fieldName = fieldName;
        m_value = value;
    }
    QString m_definitionName;
    QString m_fieldName;
    QVariant m_value;
};
typedef QList<QTstDetail> QTstDetailList;
Q_DECLARE_METATYPE(QTstDetailList)

/*!
 * For testing symbian backend via QOrganizerItemManager API. The target is
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
    void addEvent_data(QString managerName);
    void addJournal_data(QString managerName);
    void addNote_data(QString managerName);
    void addTodo_data(QString managerName);
    bool parseDetails(QTstDetailList detailsString, QList<QOrganizerItemDetail> &details);
    bool verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected);
    bool compareItemLists(QList<QOrganizerItem> actual, QList<QOrganizerItem> expected, QStringList ignoredDetails = QStringList());
    bool compareItems(QOrganizerItem itemA, QOrganizerItem itemB, QStringList ignoredDetails = QStringList());    

private:
    QOrganizerItemManager *m_om;
};


void tst_SymbianOm::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
}

void tst_SymbianOm::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
    delete m_om;
    m_om = 0;
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
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem listitem, items) {
        QVERIFY(listitem.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));
    }

    // Save with list parameter and error map parameter
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach (QOrganizerItem listitem2, items) {
        QVERIFY(listitem2.id().localId() != 0);
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
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Fetch
    QOrganizerItem fetchedItem = m_om->item(item.localId());
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
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
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Remove
    QVERIFY(m_om->removeItem(item.localId()));

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
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->removeItems(itemIds, &errorMap));
}

void tst_SymbianOm::fetchItems()
{
    // Remove all first
    QList<QOrganizerItemLocalId> ids = m_om->itemIds();
    m_om->removeItems(ids, 0);
    
    // Add some items
    QList<QOrganizerItem> expectedItems;
    for (int i=0; i<10; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        expectedItems << item;
    }

    // Fetch ids
    QList<QOrganizerItem> actualItems = m_om->items();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    
    // Verify
    QStringList ignoredDetails;
    // TODO: remove these when backend supports them properly
    ignoredDetails << QOrganizerItemDisplayLabel::DefinitionName;
    ignoredDetails << QOrganizerItemRecurrence::DefinitionName;
    ignoredDetails << QOrganizerTodoTimeRange::DefinitionName;
    ignoredDetails << QOrganizerTodoProgress::DefinitionName;
    ignoredDetails << QOrganizerItemDescription::DefinitionName;
    ignoredDetails << QOrganizerItemPriority::DefinitionName;
    QVERIFY(compareItemLists(actualItems, expectedItems, ignoredDetails));
}

void tst_SymbianOm::fetchItemIds()
{
    // Remove all first
    QList<QOrganizerItemLocalId> ids = m_om->itemIds();
    m_om->removeItems(ids, 0);
    
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
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    
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
        QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
        QVERIFY(lids[i] == item.localId());
        QVERIFY(guids[i] == item.guid());
    }
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
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QOrganizerItemTimestamp timeStamp3 = item2.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp3" << timeStamp3;
    QVERIFY(timeStamp3.lastModified() == timeStamp2.lastModified());
}

void tst_SymbianOm::addNegative()
{
    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QList<QOrganizerItem> items;
    QVERIFY(!m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

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
    QTest::addColumn<QTstDetailList>("detailsList");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend

    foreach (QString managerName, managerNames) {
        addEvent_data(managerName);
        addJournal_data(managerName);
        addNote_data(managerName);
        addTodo_data(managerName);

        // TODO: recurrence and exceptions
        // TODO: timestamps
        // TODO: attendees?
        // TODO: display label
        // TODO: guid?
        // TODO: location
        // TODO: priority
        // TODO: ...
    }
}

void tst_SymbianOm::addItem()
{
    // Fetch parameters
    QFETCH(int, expectedError);
    QFETCH(QTstDetailList, detailsList);

    QOrganizerItemManager::Error expectedErrorCode = (QOrganizerItemManager::Error) expectedError; 
    QList<QOrganizerItemDetail> details;
    QVERIFY(parseDetails(detailsList, details));

    // Initialize: Create test item and set it's details
    QOrganizerItem item;
    foreach(QOrganizerItemDetail detail, details) {
        QVERIFY(item.saveDetail(&detail));
        /* TODO: enable the following and implement detail definitions to symbian backend 
        QOrganizerItemDetailDefinition def = m_om->detailDefinition(detail.definitionName(), item.type());
        if (m_om->error() != QOrganizerItemManager::NoError) {
            // this detail is not supported, skip the test case
            QString skipMsg = QString("Detail ") + detail.definitionName() + QString(" not supported for item type " + item.type());
            QSKIP(skipMsg.toAscii(), SkipSingle);
        }
        */
    }

    // Add, fetch and verify result
    if (expectedErrorCode == QOrganizerItemManager::NoError) {
        // Save
        QVERIFY(m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
        QVERIFY(item.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));

        // Fetch item to verify everything was saved successfully
        QOrganizerItem savedItem = m_om->item(item.localId());
        QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
        QVERIFY(verifyDetails(savedItem.details(), details));
    } else {
        // An error is expected -> Check that save fails and the error code
        // is the correct one
        QVERIFY(!m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
    }
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_SymbianOm::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend

    foreach(QString mgr, managerNames) {
        QTest::newRow(QString("[%1]").arg(mgr).toLatin1().constData()) << mgr;
    }
}

/**
 * Adds event related test data for test case "addItem" for backend "managerName".
 */
void tst_SymbianOm::addEvent_data(QString managerName)
{
    /**** event items ****/
    // There is a bug in Symbian calendar engine. It does not allow
    // creating an event without start time.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        // This test case matches the iCalendar specification
        QTest::newRow(QString("[%1] Item type Event").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent));
    } else {
        // This test case is against the iCalendar specification
        QTest::newRow(QString("[%1] Item type Event").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::BadArgumentError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent));
    }
    QTest::newRow(QString("[%1] Item type Event; details: start time").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
    // There is a bug in symbian calendar server: An end datetime that is
    // exactly the same as the start datetime is ignored.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(QString("[%1] Item type Event; details: start time = end time").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
                << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
                << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime()));
    }
    QTest::newRow(QString("[%1] Item type Event; details: start time < end time").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    QTest::newRow(QString("[%1] Item type Event; details: start time > end time").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addSecs(3600))
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime()));
    // Symbian calendar server makes the client process panic in case the start
    // time of an event (calendar event of type EAppt) is left empty.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        // This test case matches the iCalendar specification
        QTest::newRow(QString("[%1] Item type Event; details: end time").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
                << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    } else {
        // This test case is against the iCalendar specification
        QTest::newRow(QString("[%1] Item type Event; details: end time").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::BadArgumentError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
                << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    }

    /* TODO: Enable and implement
    QTest::newRow(QString("[%1] Item type Event; details: description").arg(managerName).toLatin1().constData())
        << managerName
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    QTest::newRow(QString("[%1] Item type Event; details: description, note").arg(managerName).toLatin1().constData())
        << managerName
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << QTstDetail(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
     */

    /* TODO: enable and implement
        QTest::newRow(QString("[%1] Item type EventOccurrence")
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEventOccurrence));
                */
}

/**
 * Adds journal related test data for test case "addItem" for backend "managerName".
 */
void tst_SymbianOm::addJournal_data(QString managerName)
{

    /* TODO: enable and implement
        QTest::newRow(QString("[%1] Item type Journal").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeJournal));
    */
}

/**
 * Adds note related test data for test case "addItem" for backend "managerName".
 */
void tst_SymbianOm::addNote_data(QString managerName)
{
/* TODO: Enable and implement
    QTest::newRow(QString("[%1] Item type Note").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote));
    QTest::newRow(QString("[%1] Item type Note; details: description, note").arg(managerName).toLatin1().constData())
        << managerName
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << QTstDetail(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
*/
}

/**
 * Adds todo item related test data for test case "addItem" for backend "managerName".
 */
void tst_SymbianOm::addTodo_data(QString managerName)
{
    QTest::newRow(QString("[%1] Item type Todo").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo));
    // There is a bug in symbian calendar server that makes an empty
    // "not-before" datetime to be replaced with the same value as "due"
    // datetime if only "due" datetime was defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(QString("[%1] Item type Todo; details: due time").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
                << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())); 
    }
    QTest::newRow(QString("[%1] Item type Todo; details: due time == not-before time").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
    QTest::newRow(QString("[%1] Item type Todo; details: due time > not-before time").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
    QTest::newRow(QString("[%1] Item type Todo; details: due time < not-before time").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));

    // There is a bug in symbian calendar server which makes the "not-before"
    // datetime to be ignored if no "due" datetime is defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(QString("[%1] Item type Todo; details: not-before time").arg(managerName).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
                << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));
    }
    
/* TODO: Enable and implement
    QTest::newRow(QString("[%1] Item type TodoOccurrence").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodoOccurrence));
    QTest::newRow(QString("[%1] Item type Note").arg(managerName).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote));
    QTest::newRow(QString("[%1] Item type Note; details: description, note").arg(managerName).toLatin1().constData())
        << managerName
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << QTstDetail(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
*/
}

/*!
 * Helper function for parsing QTstDetails into organizer item details
 */
bool tst_SymbianOm::parseDetails(QTstDetailList detailsList, QList<QOrganizerItemDetail> &details)
{
    foreach (QTstDetail tstdetail, detailsList) {
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
                if (actualDetail.variantValues() != expectedDetail.variantValues()) {
                    qDebug() << "Detail value mismatch.";
                    qDebug() << "Actual  :" << actualDetail.variantValues();
                    qDebug() << "Expected:" << expectedDetail.variantValues();
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
