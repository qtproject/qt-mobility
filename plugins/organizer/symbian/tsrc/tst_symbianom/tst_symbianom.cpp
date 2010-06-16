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

const QString m_managerName("symbian");
//const QString m_managerName("memory");

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
    void initTestCase();
    void cleanupTestCase();

private slots:  // Test cases
    // Simple test cases without any data
    void addSimpleItem();
    void fetchSimpleItem();
    void removeSimpleItem();
    void fetchItems();
    void fetchItemIds();
    void uniqueIds();
    void timeStamp();

    // Tests with data
    void addItem_data();
    void addItem();

private:
    // TODO: enable the following test cases by moving them to "private slots"
    // when the required functionality has been implemented in symbian backend

    // Negative test cases (testing for example with illegal parameters)
    void addNegative();
    void fetchNegative();
    void removeNegative();

private: // util functions
    bool parseDetails(QTstDetailList detailsString, QList<QOrganizerItemDetail> &details);
    bool verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected);
    bool compareItemLists(QList<QOrganizerItem> actual, QList<QOrganizerItem> expected, QStringList ignoredDetails = QStringList());
    bool compareItems(QOrganizerItem itemA, QOrganizerItem itemB, QStringList ignoredDetails = QStringList());    

private:
    QOrganizerItemManager *m_om;
};


void tst_SymbianOm::initTestCase()
{
    // You could also use the override without the manager name; that should
    // default to the platform specific default manager
    m_om = new QOrganizerItemManager(m_managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    //m_om->removeItems(m_om->itemIds(), 0);
}

void tst_SymbianOm::cleanupTestCase()
{
    //m_om->removeItems
    delete m_om;
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
    QVERIFY(item.id().managerUri().contains(m_managerName));

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem listitem, items) {
        QVERIFY(listitem.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_managerName));
    }

    // Save with list parameter and error map parameter
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach (QOrganizerItem listitem2, items) {
        QVERIFY(listitem2.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_managerName));
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
    QVERIFY(item.id().managerUri().contains(m_managerName));

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
    QVERIFY(item.id().managerUri().contains(m_managerName));

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
    // Test that new items have unique ids
    QList<QOrganizerItemLocalId> lids;
    QList<QString> guids;
    for (int i=0; i<10; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        QVERIFY(!lids.contains(item.localId()));
        lids << item.localId();
        QVERIFY(!guids.contains(item.guid()));
        guids << item.guid();
    }
}

void tst_SymbianOm::timeStamp()
{
    // TODO: support for created time? is it possible with symbian?
    // TODO: check detail definitions
    
    // Save item
    QOrganizerTodo item1;
    item1.setDisplayLabel("do stuff");
    QVERIFY(m_om->saveItem(&item1));
    
    // Verify timestamp
    QOrganizerItemTimestamp timeStamp1 = item1.detail<QOrganizerItemTimestamp>();
    QVERIFY(!timeStamp1.isEmpty());
    QVERIFY(timeStamp1.lastModified().isValid());
    
    // Wait a while
    QTest::qSleep(1000);

    // Save again
    item1.setDisplayLabel("do more stuff");
    QVERIFY(m_om->saveItem(&item1));
    
    // Verify timestamp
    QOrganizerItemTimestamp timeStamp2 = item1.detail<QOrganizerItemTimestamp>();
    QVERIFY(!timeStamp2.isEmpty());
    QVERIFY(timeStamp2.lastModified().isValid());
    QVERIFY(timeStamp2.lastModified() > timeStamp1.lastModified()); 
    
    // Load the same item again
    QOrganizerTodo item2 = m_om->item(item1.localId());
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QOrganizerItemTimestamp timeStamp3 = item2.detail<QOrganizerItemTimestamp>();
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
    QTest::addColumn<int>("expectedError");
    QTest::addColumn<QTstDetailList>("detailsList");

    /**** event items ****/
    QTest::newRow("Item type Event")
        << (int) QOrganizerItemManager::BadArgumentError // Symbian calendar API does not allow creating an event without start time
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent));
    QTest::newRow("Item type Event; details: start time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
#ifndef Q_OS_SYMBIAN
    // There is a bug in symbian calendar server: An end datetime that is
    // exactly the same as the start datetime is ignored.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    QTest::newRow("Item type Event; details: start time = end time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime()));
#endif
    QTest::newRow("Item type Event; details: start time < end time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    QTest::newRow("Item type Event; details: start time > end time")
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addSecs(3600))
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime()));
#ifndef Q_OS_SYMBIAN
    // Symbian calendar server makes the client process panic in case the start
    // time of an event (calendar event of type EAppt) is left empty.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    QTest::newRow("Item type Event; details: end time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
#else
    QTest::newRow("Item type Event; details: end time")
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
#endif

    /* TODO: Enable and implement
    QTest::newRow("Item type Event; details: description")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    QTest::newRow("Item type Event; details: description, note")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << QTstDetail(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
     */

    /* TODO: enable and implement
        QTest::newRow("Item type EventOccurrence")
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEventOccurrence));
                */

    /* TODO: enable and implement
        QTest::newRow("Item type Journal")
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailList()
                << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeJournal));
    */

    /**** todo items ****/
    QTest::newRow("Item type Todo")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo));
#ifndef Q_OS_SYMBIAN
    // There is a bug in symbian calendar server that makes an empty
    // "not-before" datetime to be replaced with the same value as "due"
    // datetime if only "due" datetime was defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    QTest::newRow("Item type Todo; details: due time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())); 
#endif
    QTest::newRow("Item type Todo; details: due time == not-before time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
    QTest::newRow("Item type Todo; details: due time > not-before time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
    QTest::newRow("Item type Todo; details: due time < not-before time")
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));

#ifndef Q_OS_SYMBIAN
    // There is a bug in symbian calendar server which makes the "not-before"
    // datetime to be ignored if no "due" datetime is defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    QTest::newRow("Item type Todo; details: not-before time")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo)
            << QTstDetail(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldNotBeforeDateTime, QDateTime::currentDateTime().addDays(1)));
#endif

/* TODO: Enable and implement
    QTest::newRow("Item type TodoOccurrence")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodoOccurrence));
    QTest::newRow("Item type Note")
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote));
    QTest::newRow("Item type Note; details: description, note")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << QTstDetail(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
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

void tst_SymbianOm::addItem()
{
    QFETCH(int, expectedError);
    QOrganizerItemManager::Error expectedErrorCode = (QOrganizerItemManager::Error) expectedError; 
    QFETCH(QTstDetailList, detailsList);
    QList<QOrganizerItemDetail> details;
    QVERIFY(parseDetails(detailsList, details));

    // Create item and set it's details
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

    if (expectedErrorCode == QOrganizerItemManager::NoError) {
        // Save
        QVERIFY(m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
        QVERIFY(item.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_managerName));

        // Fetch item to verify everything was saved successfully
        QOrganizerItem savedItem = m_om->item(item.localId());
        QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
        QVERIFY(verifyDetails(savedItem.details(), details));
    } else {
        // Check that save fails
        QVERIFY(!m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
    }
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
