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

#ifdef Q_OS_SYMBIAN
    #include <calcommon.h> // for SYMBIAN_CALENDAR_V2
#endif

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

/*!
 * For testing symbian backend via QOrganizerItemManager async API.
 */
class tst_SymbianOmAsync : public QObject
{
    Q_OBJECT

public:
    tst_SymbianOmAsync();

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases

    void addSimpleItem_data(){ addManagers(); };
    void addSimpleItem();
    void fetchSimpleItem_data(){ addManagers(); };
    void fetchSimpleItem();
    void fetchWaitForFinished_data(){ addManagers(); };
    void fetchWaitForFinished();
    void fetchItems_data(){ addManagers(); };
    void fetchItems();
    void fetchItemIds_data(){ addManagers(); };
    void fetchItemIds();
    void fetchItemsIdFilter_data(){ addManagers(); };
    void fetchItemsIdFilter();
    void fetchItemsDetailFilter_data(){ addManagers(); };
    void fetchItemsDetailFilter();
    void fetchItemsSortOrder_data(){ addManagers(); };
    void fetchItemsSortOrder();
    void addItems_data(){ addManagers(); };
    void addItems();
    void modifyItems_data(){ addManagers(); };
    void modifyItems();
    void removeItems_data(){ addManagers(); };
    void removeItems();
#ifdef SYMBIAN_CALENDAR_V2
    void addCollection_data(){ addManagers(); };
    void addCollection();
    void modifyCollection_data(){ addManagers(); };
    void modifyCollection();
    void removeCollection_data(){ addManagers(); };
    void removeCollection();
#endif

private: // util functions
    QOrganizerItem createItem(
        QString itemType, QString label, QDateTime startTime, QDateTime endTime = QDateTime());
    QList<QOrganizerItem> createItems(QString label, int itemCount);
    void addManagers();

private:
    QOrganizerItemManager *m_om;
};

tst_SymbianOmAsync::tst_SymbianOmAsync() :
    m_om(0)
{
    qRegisterMetaType<QOrganizerItemAbstractRequest::State>("QOrganizerItemAbstractRequest::State");
    qRegisterMetaType<QList<QOrganizerItemLocalId> >("QList<QOrganizerItemLocalId>");
    qRegisterMetaType<QList<QOrganizerCollectionLocalId> >("QList<QOrganizerCollectionLocalId>");
}

void tst_SymbianOmAsync::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);
    // Remove items on all collections
    m_om->removeItems(m_om->itemIds(), 0);
    // Remove all collections (except the default)
    foreach (QOrganizerCollectionLocalId id, m_om->collectionIds()) {
        if (id != m_om->defaultCollectionId())
            m_om->removeCollection(id);
    }
}

void tst_SymbianOmAsync::cleanup()
{
    m_om->removeItems(m_om->itemIds(), 0);
    // Remove all collections (except the default)
    foreach (QOrganizerCollectionLocalId id, m_om->collectionIds()) {
        if (id != m_om->defaultCollectionId())
            m_om->removeCollection(id);
    }
    delete m_om;
    m_om = 0;
}

void tst_SymbianOmAsync::addSimpleItem()
{
    // Create item and set it's details
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeTodo);

    // Create asynchronous request to save an item
    QOrganizerItemSaveRequest saveItemRequest;
    saveItemRequest.setManager(m_om);
    saveItemRequest.setItem(item);
    saveItemRequest.setCollectionId(QOrganizerCollectionLocalId());

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&saveItemRequest, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&saveItemRequest, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));

    // Save
    QVERIFY(saveItemRequest.start());
    QCOMPARE(saveItemRequest.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(addedSpy.count(), 1);

    // Verify
    QCOMPARE(m_om->itemIds().count(), 1);
    QCOMPARE(saveItemRequest.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(saveItemRequest.error(), QOrganizerItemManager::NoError);
    QCOMPARE(saveItemRequest.items().count(), 1);
    QVERIFY(saveItemRequest.items().at(0).localId() != QOrganizerItemLocalId());
}

void tst_SymbianOmAsync::fetchSimpleItem()
{
    // Create item
    QOrganizerItem item = createItem(
        QOrganizerItemType::TypeEvent,
        QString("fetchItem"),
        QDateTime::currentDateTime().addSecs(3600));

    // Save (synchronously)
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != QOrganizerItemLocalId());
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Create fetch request
    QOrganizerItemFetchRequest fetchItemRequest;
    fetchItemRequest.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&fetchItemRequest, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&fetchItemRequest, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(fetchItemRequest.start());
    QCOMPARE(fetchItemRequest.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QCOMPARE(fetchItemRequest.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(fetchItemRequest.error(), QOrganizerItemManager::NoError);
    QCOMPARE(fetchItemRequest.items().count(), 1);
    QVERIFY(fetchItemRequest.items().at(0).localId() != QOrganizerItemLocalId());
}

void tst_SymbianOmAsync::fetchWaitForFinished()
{
     // Create item
     QOrganizerItem item;
     item.setType(QOrganizerItemType::TypeTodo);

     // Save (synchronously)
     QVERIFY(m_om->saveItem(&item));
     QVERIFY(item.id().localId() != QOrganizerItemLocalId());
     QVERIFY(item.id().managerUri().contains(m_om->managerName()));

     // Create fetch request
     QOrganizerItemFetchRequest fetchItemRequest;
     fetchItemRequest.setManager(m_om);

     // Create signal spys for verification purposes
     QSignalSpy stateSpy(&fetchItemRequest,
         SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
     QSignalSpy resultSpy(&fetchItemRequest, SIGNAL(resultsAvailable()));

     // Fetch
     QVERIFY(fetchItemRequest.start());
     QCOMPARE(fetchItemRequest.state(), QOrganizerItemAbstractRequest::ActiveState);
     QCOMPARE(stateSpy.count(), 1);
     QTime startTime = QTime::currentTime();
     QVERIFY(fetchItemRequest.waitForFinished(5000)); // 5 seconds time-out

     // Verify that the fetch did not take over 2 secons
     // Note: at the moment we don't have any performance requirements defined,
     // but a single fetch request taking more than 2 seconds would be clearly
     // too slow
     QVERIFY(startTime.secsTo(QTime::currentTime()) < 2);
}

void tst_SymbianOmAsync::fetchItems()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitems"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(stateSpy.count(), 2);

    // Verify
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    QVERIFY(req.items().at(0).localId() != QOrganizerItemLocalId());
}

void tst_SymbianOmAsync::fetchItemIds()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitems"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemLocalIdFetchRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(stateSpy.count(), 2);

    // Verify
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.itemIds().count(), itemCount);
    QVERIFY(req.itemIds().at(0) != QOrganizerItemLocalId());
}

void tst_SymbianOmAsync::fetchItemsIdFilter()
{
    // Save items (synchronously)
    const int itemCount(100);
    // Fetch items from "pos" to "pos + length"
    const int pos(10);
    const int length(20);
    QList<QOrganizerItem> items = createItems(QString("fetchitemsidfilter"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch with local id filter
    QOrganizerItemLocalIdFilter localIdFilter;
    localIdFilter.setIds(m_om->itemIds().mid(pos, length));
    req.setFilter(localIdFilter);
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.items().count(), length);
    // Assuming the sort order is the same:
    QVERIFY(req.items().at(0).localId() == m_om->itemIds().at(pos));
}

void tst_SymbianOmAsync::fetchItemsDetailFilter()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitemsdetailfilter"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch with detail filter
    QOrganizerItemDetailFilter detailFilter;
    detailFilter.setDetailDefinitionName(QOrganizerItemDisplayLabel::DefinitionName);
    req.setFilter(detailFilter);
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    QVERIFY(req.items().at(0).localId() != QOrganizerItemLocalId());
}

void tst_SymbianOmAsync::fetchItemsSortOrder()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitemssortorder"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Set sort order
    QList<QOrganizerItemSortOrder> sortOrderlist;
    QOrganizerItemSortOrder sorting;
    sorting.setDetailDefinitionName(
        QOrganizerItemDisplayLabel::DefinitionName,
        QOrganizerItemDisplayLabel::FieldLabel);
    sorting.setBlankPolicy(QOrganizerItemSortOrder::BlanksLast);
    sortOrderlist.append(sorting);
    req.setSorting(sortOrderlist);

    // Set fetch hint
    QOrganizerItemFetchHint fetchHint;
    fetchHint.setOptimizationHints(QOrganizerItemFetchHint::AllRequired);
    req.setFetchHint(fetchHint);

    // Fetch
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    QVERIFY(req.items().at(0).localId() != QOrganizerItemLocalId());
}

void tst_SymbianOmAsync::addItems()
{
    // Create items
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("additems"), itemCount);

    // Create save request
    QOrganizerItemSaveRequest req;
    req.setManager(m_om);
    req.setItems(items);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));

    // Save
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QVERIFY(addedSpy.count() > 0);
    // Check that there were more than one "added" signal. The reasoning for this is that if you
    // create several items asynchronously, there is not much point in the async api if you only
    // get one signal. For example showing information like "54% complete" to the user is not
    // possible if you get only one signal once the request is completed.
    QVERIFY(addedSpy.count() > 1);
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    foreach (QOrganizerItem item, req.items()) {
        QVERIFY(!item.localId().isNull());
    }
}

void tst_SymbianOmAsync::modifyItems()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("modifyitems1"), itemCount);
    QVERIFY(m_om->saveItems(&items));
    foreach (QOrganizerItem item, items) {
        item.setDisplayLabel("modifyitems2");
    }

    // Create save request
    QOrganizerItemSaveRequest req;
    req.setManager(m_om);
    req.setItems(items);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy changedSpy(m_om, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));

    // Save
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QVERIFY(changedSpy.count() > 0);
    QVERIFY(changedSpy.count() > 1);
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
}

void tst_SymbianOmAsync::removeItems()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("removeitems"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create remove request
    QOrganizerItemRemoveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy removedSpy(m_om, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));

    // Remove
    req.setItemIds(m_om->itemIds());
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(stateSpy.count(), 2);

    // Verify
    QVERIFY(removedSpy.count() > 0);
    QVERIFY(removedSpy.count() > 1);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(m_om->itemIds().count(), 0);
}

#ifdef SYMBIAN_CALENDAR_V2
void tst_SymbianOmAsync::addCollection()
{
    // Create the request
    QOrganizerCollectionSaveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(collectionsAdded(QList<QOrganizerCollectionLocalId>)));

    // Create new collection
    QOrganizerCollection collection;
    collection.setMetaData("Name", "addCollection");
    collection.setMetaData("FileName", "c:addCollection");
    collection.setMetaData("Description", "addCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    req.setCollection(collection);

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(addedSpy.count(), 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(m_om->collections().count(), 2); // the default plus the new one
    QCOMPARE(m_om->collections().at(1).metaData().value("Name").toString(), QString("addCollection"));
}

void tst_SymbianOmAsync::modifyCollection()
{
    // Create async request
    QOrganizerCollectionSaveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(collectionsAdded(QList<QOrganizerCollectionLocalId>)));
    QSignalSpy changedSpy(m_om, SIGNAL(collectionsChanged(QList<QOrganizerCollectionLocalId>)));

    // Create new collection (synchronously)
    QOrganizerCollection collection;
    collection.setMetaData("Name", "modifyCollection");
    collection.setMetaData("FileName", "c:modifycollection");
    collection.setMetaData("Description", "modifyCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&collection));

    // Modify the newly created collection asynchronously
    collection.setMetaData("Description", "modifyCollection test2");
    req.setCollection(collection);

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(addedSpy.count(), 1);
    QTRY_COMPARE(changedSpy.count(), 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(m_om->collections().count(), 2); // the default plus the new one
    QCOMPARE(m_om->collections().at(1).metaData().value("Name").toString(), QString("modifyCollection"));
    QCOMPARE(m_om->collections().at(1).metaData().value("Description").toString(), QString("modifyCollection test2"));
}

void tst_SymbianOmAsync::removeCollection()
{
    // Create async request
    QOrganizerCollectionRemoveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy removedSpy(m_om, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionLocalId>)));

    // Create new collection (synchronously)
    QOrganizerCollection collection;
    collection.setMetaData("Name", "removeCollection");
    collection.setMetaData("FileName", "c:removecollection");
    collection.setMetaData("Description", "removeCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&collection));

    // Remove the new collection
    req.setCollectionId(collection.localId());

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(removedSpy.count(), 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerItemManager::NoError);
    QCOMPARE(m_om->collections().count(), 1); // the default
}
#endif

/*!
 * A helper method for creating a QOrganizerItem instance.
 */
QOrganizerItem tst_SymbianOmAsync::createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime)
{
    QOrganizerItem item;
    item.setType(itemType);
    item.setDisplayLabel(label);
    if (itemType == QOrganizerItemType::TypeTodo) {
        QOrganizerTodoTimeRange timeRange;
        if (startTime.isValid())
            timeRange.setStartDateTime(startTime);
        if (endTime.isValid())
            timeRange.setDueDateTime(endTime);
        if (!timeRange.isEmpty())
            item.saveDetail(&timeRange);
    } else {
        QOrganizerEventTimeRange timeRange;
        if (startTime.isValid())
            timeRange.setStartDateTime(startTime);
        if (endTime.isValid())
            timeRange.setEndDateTime(endTime);
        if (!timeRange.isEmpty())
            item.saveDetail(&timeRange);
    }
    return item;
}

/*!
 * A helper method for adding several items.
 */
QList<QOrganizerItem> tst_SymbianOmAsync::createItems(QString label, int itemCount)
{
    QList<QOrganizerItem> items;
    for (int i(0); i < itemCount; i++) {
        QOrganizerItem item = createItem(
            QOrganizerItemType::TypeEvent,
            label + i,
            QDateTime::currentDateTime().addSecs(3600),
            QDateTime::currentDateTime().addSecs(7200));
        items.append(item);
    }
    return items;
}


/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_SymbianOmAsync::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    // TODO: memory backend does not have async API implemented
    managerNames.removeAll("memory");

    foreach(QString mgr, managerNames) {
        QTest::newRow(QString("[%1]").arg(mgr).toLatin1().constData()) << mgr;
    }
}

QTEST_MAIN(tst_SymbianOmAsync);
#include "tst_symbianomasync.moc"
