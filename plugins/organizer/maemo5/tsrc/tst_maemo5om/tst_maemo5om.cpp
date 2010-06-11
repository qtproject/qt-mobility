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

class tst_Maemo5Om : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void initTestCase();
    void cleanupTestCase();

private slots:  // Test cases
    void addSimpleEvent();
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
    m_om = new QOrganizerItemManager("maemo5");
    //m_om = new QOrganizerItemManager("symbian");
    //m_om = new QOrganizerItemManager("memory");
    // Remove all organizer items first (Note: ignores possible errors)
    //m_om->removeItems(m_om->itemIds(), 0);
}

void tst_Maemo5Om::cleanupTestCase()
{
    //m_om->remove
    delete m_om;
}

void tst_Maemo5Om::addSimpleEvent()
{
    // Create item and set it's details
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeEvent);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id() != QOrganizerItemId());

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);

    // Save with list parameter and error map parameter
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
}

void tst_Maemo5Om::addWithIllegalParameters()
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
    QTest::newRow("Item type Event; details: description, note")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent)
            << detailToQString(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << detailToQString(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));
    QTest::newRow("Item type Note; details: description, note")
        << (QStringList()
            << detailToQString(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote)
            << detailToQString(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Memo for the meeting with Elbonian president"))
            << detailToQString(QOrganizerItemNote::DefinitionName, QOrganizerItemNote::FieldNote, QString("Remember to wear blue jeans")));

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
