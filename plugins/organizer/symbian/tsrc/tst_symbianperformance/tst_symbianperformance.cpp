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
#include <calcommon.h> // for SYMBIAN_CALENDAR_V2

QTM_USE_NAMESPACE

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

/*!
 * For testing symbian backend performance via QOrganizerManager API.
 */
class tst_symbianperformance : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases

    void items_data();
    void items();
    void itemsInCollections();

private: // util functions
    QOrganizerItem createItem(
        QString itemType, QString label, QDateTime startTime, QDateTime endTime, int recurrenceCount);

private:
    QOrganizerManager *m_om;
    bool m_customCollectionsSupported;
};


void tst_symbianperformance::init()
{
    // TODO: How could this be done dynamically?
    // Some kind of manager feature flag would be nice.
#ifdef SYMBIAN_CALENDAR_V2
    m_customCollectionsSupported = true;
#else
    m_customCollectionsSupported = false;
#endif

    m_om = new QOrganizerManager();

    // Remove items on all collections
    m_om->removeItems(m_om->itemIds());

    // Remove all collections (except the default)
    foreach (QOrganizerCollection collection, m_om->collections()) {
        if (collection.id() != m_om->defaultCollection().id())
            m_om->removeCollection(collection.localId());
    }
}

void tst_symbianperformance::cleanup()
{
    // Remove items on all collections
    m_om->removeItems(m_om->itemIds());

    // Remove all collections (except the default)
    foreach (QOrganizerCollection collection, m_om->collections()) {
        if (collection.id() != m_om->defaultCollection().id())
            m_om->removeCollection(collection.localId());
    }

    delete m_om;
    m_om = 0;
}

void tst_symbianperformance::items_data()
{
    QTest::addColumn<int>("itemCount");
    QTest::addColumn<int>("recurrenceCount");

    QTest::newRow(QString("1000 items").toAscii().constData()) << 1000 << 1;
    QTest::newRow(QString("1000 items, 2 occurrences").toAscii().constData()) << 1000 << 2;
    QTest::newRow(QString("1000 items, 4 occurrences").toAscii().constData()) << 1000 << 4;
}

void tst_symbianperformance::items()
{
    QFETCH(int, itemCount);
    QFETCH(int, recurrenceCount);

    QDateTime currentDateTime = QDateTime::currentDateTime();

    {
        // Create a list of items, one item per day for the next "itemCount" days
        QList<QOrganizerItem> items;
        for (int i(0); i < itemCount; i++) {
            items << createItem(
                    QOrganizerItemType::TypeEvent,
                    QString("addItem") + QString::number(i),
                    currentDateTime.addDays(i),
                    currentDateTime.addDays(i).addSecs(3600),
                    recurrenceCount);
        }

        // Save items
        QTime timerStart = QTime::currentTime();
        QVERIFY(m_om->saveItems(&items));
        QTime timerEnd = QTime::currentTime();
        qDebug() << "Save items [ms]: " << timerStart.msecsTo(timerEnd);
    }

    {
        // Fetch all exportable items
        QTime timerStart = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->itemsForExport();
        QTime timerEnd = QTime::currentTime();
        QCOMPARE(items.count(), itemCount);
        qDebug() << "Fetching all items took [ms]: " << timerStart.msecsTo(timerEnd);
    }

    {
        // Fetch all exportable items for the following week
        QDateTime startDateTime = currentDateTime;
        QDateTime endDateTime = currentDateTime.addDays(7);
        QTime timerStart = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->itemsForExport(startDateTime, endDateTime);
        QTime timerEnd = QTime::currentTime();
        // TODO: got "itemsForExport" is broken because not filtered with slowFilter any more: QCOMPARE(items.count(), 7);
        qDebug() << "Fetching 7 items with date filter took [ms]: " << timerStart.msecsTo(timerEnd);
    }

    {
        // Fetch all item instances
        QTime timerStart = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->items();
        QTime timerEnd = QTime::currentTime();
        QCOMPARE(items.count(), itemCount * recurrenceCount);
        qDebug() << "Fetching all item instances took [ms]: " << timerStart.msecsTo(timerEnd);
    }

    {
        // Fetch all item instances for the week in the middle of the time period
        QDateTime startDateTime = currentDateTime.addDays(itemCount / 2);
        QDateTime endDateTime = currentDateTime.addDays(itemCount / 2 + 7);
        QTime timerStart = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->items(startDateTime, endDateTime);
        QTime timerEnd = QTime::currentTime();
        // Known issue with symbian backend; the returned items may, or may not contain an extra item, depending on the platform
        qDebug() << "itemCount:" << items.count();
        QVERIFY(items.count() == recurrenceCount * 7 || items.count() == recurrenceCount * 8);
        qDebug() << "Fetching item instances with date filter took [ms]: " << timerStart.msecsTo(timerEnd);
    }

    {
        // Remove all items
        QList<QOrganizerItemLocalId> itemIds = m_om->itemIds();
        QTime timerStart = QTime::currentTime();
        QVERIFY(m_om->removeItems(itemIds));
        QTime timerEnd = QTime::currentTime();
        QCOMPARE(m_om->items().count(), 0);
        qDebug() << "Removing all item instances took [ms]: " << timerStart.msecsTo(timerEnd);
    }
}

void tst_symbianperformance::itemsInCollections()
{
    if (!m_customCollectionsSupported)
        QSKIP("Saving/modifying collections not supported!", SkipSingle);

    int itemCount = 500;
    int recurrenceCount = 3;
    QDateTime currentDateTime = QDateTime::currentDateTime();

    // Create two collections
    QOrganizerCollection c1;
    c1.setMetaData("Name", "itemsInCollections1");
    c1.setMetaData("FileName", "c:itemsincollections");
    QVERIFY(m_om->saveCollection(&c1));
    QOrganizerCollection c2;
    c2.setMetaData("Name", "itemsInCollections2");
    c2.setMetaData("FileName", "c:itemsincollections2");
    QVERIFY(m_om->saveCollection(&c2));

    {
        // Save n items to two collections

        QList<QOrganizerItem> items1;
        QList<QOrganizerItem> items2;
        for (int i(0); i < itemCount; i++) {
            QOrganizerItem item1 = createItem(
                    QOrganizerItemType::TypeEvent,
                    QString("addItem") + QString::number(i),
                    currentDateTime.addDays(i),
                    currentDateTime.addDays(i).addSecs(3600),
                    recurrenceCount);
            QOrganizerItem item2 = item1;
            item1.setCollectionId(c1.id());
            items1 << item1;
            item2.setCollectionId(c2.id());
            items2 << item2;
        }

        QTime startTime = QTime::currentTime();
        QVERIFY(m_om->saveItems(&items1));
        QVERIFY(m_om->saveItems(&items2));
        QTime endTime = QTime::currentTime();
        qDebug() << "Saving items took [ms]: " << startTime.msecsTo(endTime);
    }

    {
        // Fetch all items for export
        QTime startTime = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->itemsForExport();
        QTime endTime = QTime::currentTime();
        QCOMPARE(items.count(), itemCount * 2);
        qDebug() << "Fetching all items took [ms]: " << startTime.msecsTo(endTime);
    }

    {
        // Fetch items from collection c1
        QOrganizerItemCollectionFilter filter;
        filter.setCollectionId(c1.localId());
        QTime startTime = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->itemsForExport(QDateTime(), QDateTime(), filter);
        QTime endTime = QTime::currentTime();
        QCOMPARE(items.count(), itemCount);
        qDebug() << "Fetching 500 items with collection filter took [ms]: " << startTime.msecsTo(endTime);
    }

    {
        // Fetch all item instances
        QTime startTime = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->items();
        QTime endTime = QTime::currentTime();
        QCOMPARE(items.count(), itemCount * 2 * 3);
        qDebug() << "Fetching all item instances took [ms]: " << startTime.msecsTo(endTime);
    }

    {
        // Fetch item instances with a collection filter
        QOrganizerItemCollectionFilter filter;
        filter.setCollectionId(c1.localId());
        QTime startTime = QTime::currentTime();
        QList<QOrganizerItem> items = m_om->items(filter);
        QTime endTime = QTime::currentTime();
        QCOMPARE(items.count(), itemCount * 3);
        qDebug() << "Fetching 500 item instances with collection filter took [ms]: " << startTime.msecsTo(endTime);
    }

    {
        // Remove all items
        QList<QOrganizerItemLocalId> itemIds = m_om->itemIds();
        QTime startTime = QTime::currentTime();
        QVERIFY(m_om->removeItems(itemIds));
        QTime endTime = QTime::currentTime();
        QCOMPARE(m_om->items().count(), 0);
        qDebug() << "Removing all item instances took [ms]: " << startTime.msecsTo(endTime);
    }
}

/*!
 * A helper method for creating a QOrganizerItem instance.
 */
QOrganizerItem tst_symbianperformance::createItem(
        QString itemType, QString label, QDateTime startTime, QDateTime endTime, int recurrenceCount)
{
    QOrganizerItem item;
    item.setType(itemType);
    item.setDisplayLabel(label);
    if (itemType == QOrganizerItemType::TypeTodo) {
        QOrganizerTodoTime timeRange;
        if (startTime.isValid())
            timeRange.setStartDateTime(startTime);
        if (endTime.isValid())
            timeRange.setDueDateTime(endTime);
        if (!timeRange.isEmpty())
            item.saveDetail(&timeRange);
    } else {
        QOrganizerEventTime timeRange;
        if (startTime.isValid())
            timeRange.setStartDateTime(startTime);
        if (endTime.isValid())
            timeRange.setEndDateTime(endTime);
        if (!timeRange.isEmpty())
            item.saveDetail(&timeRange);
    }

    // Set recurrence rule if needed
    if (recurrenceCount > 1) {
        // Create a weekly recurrence rule
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule rrule;
        rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        rrule.setLimit(recurrenceCount);
        QSet<QOrganizerRecurrenceRule> ruleSet;
        ruleSet.insert(rrule);
        recurrence.setRecurrenceRules(ruleSet);
        item.saveDetail(&recurrence);
    }

    return item;
}

QTEST_MAIN(tst_symbianperformance);
#include "tst_symbianperformance.moc"
