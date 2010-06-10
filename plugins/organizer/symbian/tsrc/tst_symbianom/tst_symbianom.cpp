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

const QString managerName("symbian");
//const QString managerName("memory");

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
Q_DECLARE_METATYPE(QTstDetailList);

class tst_SymbianOm : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void initTestCase();
    void cleanupTestCase();

private slots:  // Test cases

    // Simple test cases without any data
    void addSimpleEvent();
    void fetchSimpleEvent();
    void removeSimpleEvent();

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

    void fetchItemIds();

private:
    bool parseDetails(QTstDetailList detailsString, QList<QOrganizerItemDetail> &details);
    bool verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected);
    QOrganizerItemManager *m_om;
};


void tst_SymbianOm::initTestCase()
{
    // You could also use the override without the manager name; that should
    // default to the platform specific default manager
    m_om = new QOrganizerItemManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    //m_om->removeItems(m_om->itemIds(), 0);
}

void tst_SymbianOm::cleanupTestCase()
{
    //m_om->removeItems
    delete m_om;
}

void tst_SymbianOm::addSimpleEvent()
{
    // Create item and set it's details
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeEvent);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(managerName));

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem listitem, items) {
        QVERIFY(listitem.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(managerName));
    }

    // Save with list parameter and error map parameter
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach (QOrganizerItem listitem2, items) {
        QVERIFY(listitem2.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(managerName));
    }
}

void tst_SymbianOm::fetchSimpleEvent()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeEvent);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(managerName));

    // Fetch
    QOrganizerItem fetchedItem = m_om->item(item.localId());
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QCOMPARE(fetchedItem.id(), item.id());
    QVERIFY(fetchedItem.type() == QOrganizerItemType::TypeEvent);
}

void tst_SymbianOm::removeSimpleEvent()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeEvent);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(managerName));

    // Remove
    QVERIFY(m_om->removeItem(item.localId()));

    // Remove list
    QOrganizerItem item2;
    item2.setType(QOrganizerItemType::TypeEvent);
    QVERIFY(m_om->saveItem(&item2));
    QVERIFY(m_om->saveItem(&item));
    QList<QOrganizerItemLocalId> itemIds;
    itemIds.append(item.localId());
    itemIds.append(item2.localId());
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->removeItems(itemIds, &errorMap));
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
    
    QTest::addColumn<QTstDetailList>("detailsList");

    // Item types
    QTest::newRow("Item type Event")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent));
/* TODO: enable and implement these details
    QTest::newRow("Item type EventOccurrence")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEventOccurrence));
    QTest::newRow("Item type Journal")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeJournal));
*/
    QTest::newRow("Item type Todo")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo));
/* TODO: enable and implement these details
    QTest::newRow("Item type TodoOccurrence")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodoOccurrence));
    QTest::newRow("Item type Note")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote));
*/

    // Other details
    QTest::newRow("Item type Event; details: start time")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemEventTimeRange::DefinitionName, QOrganizerItemEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));
    QTest::newRow("Item type Event; details: start time, end time")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemEventTimeRange::DefinitionName, QOrganizerItemEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetail(QOrganizerItemEventTimeRange::DefinitionName, QOrganizerItemEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
/*
    QTest::newRow("Item type Event; details: description")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    QTest::newRow("Item type Event; details: description, note")
        << (QTstDetailList()
            << QTstDetail(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << QTstDetail(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << QTstDetail(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
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

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(managerName));

    // Fetch item to verify everything was saved successfully
    QOrganizerItem savedItem = m_om->item(item.localId());
    QVERIFY(verifyDetails(savedItem.details(), details));
}

void tst_SymbianOm::fetchItemIds()
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

QTEST_MAIN(tst_SymbianOm);
#include "tst_symbianom.moc"
