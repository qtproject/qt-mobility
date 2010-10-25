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
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

/*!
 * For testing symbian backend via QOrganizerManager async API.
 */
class tst_SymbianOmAsync : public QObject
{
    Q_OBJECT

public:
    tst_SymbianOmAsync();
    
public slots:
    void initTestCase();
    
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
    void fetchItemsNonExistingIds_data(){ addManagers(); };
    void fetchItemsNonExistingIds();
    void fetchItemsDetailFilter_data(){ addManagers(); };
    void fetchItemsDetailFilter();
    void fetchItemsSortOrder_data(){ addManagers(); };
    void fetchItemsSortOrder();
    void fetchItemsDeleteRequest_data(){ addManagers(); };
    void fetchItemsDeleteRequest();
    void addItems_data(){ addManagers(); };
    void addItems();
    void addItemsMultiReq_data(){ addManagers(); };
    void addItemsMultiReq();
    void modifyItems_data(){ addManagers(); };
    void modifyItems();
    void removeItems_data(){ addManagers(); };
    void removeItems();
    void addCollection_data(){ addManagers(); };
    void addCollection();
    void modifyCollection_data(){ addManagers(); };
    void modifyCollection();
    void removeCollection_data(){ addManagers(); };
    void removeCollection();
    void addCollectionMultiManager_data(){ addManagers(); };
    void addCollectionMultiManager();
    void detailDefinitionFetch_data(){ addManagers(); };
    void detailDefinitionFetch();
    void detailDefinitionSave_data(){ addManagers(); };
    void detailDefinitionSave();
    void detailDefinitionRemove_data(){ addManagers(); };
    void detailDefinitionRemove();
    void detailDefinitionRemoveDeleteRequest_data(){ addManagers(); };
    void detailDefinitionRemoveDeleteRequest();

protected slots:
    void deleteRequest();

private: // util functions
    QOrganizerItem createItem(
        QString itemType, QString label, QDateTime startTime, QDateTime endTime = QDateTime());
    QList<QOrganizerItem> createItems(QString label, int itemCount);
    void addManagers();

private:
    QOrganizerManager *m_om;
    bool m_customCollectionsSupported;
};

Q_DECLARE_METATYPE(QList<QOrganizerCollectionId>)

tst_SymbianOmAsync::tst_SymbianOmAsync() :
    m_om(0)
{
    qRegisterMetaType<QOrganizerAbstractRequest::State>("QOrganizerAbstractRequest::State");
    qRegisterMetaType<QList<QOrganizerItemId> >("QList<QOrganizerItemId>");
    qRegisterMetaType<QList<QOrganizerCollectionId> >("QList<QOrganizerCollectionId>");
}

void tst_SymbianOmAsync::initTestCase()
{
    // TODO: How could this be done dynamically? 
    // Some kind of manager feature flag would be nice.
#ifdef SYMBIAN_CALENDAR_V2
    m_customCollectionsSupported = true;
#else
    m_customCollectionsSupported = false;
#endif
}

void tst_SymbianOmAsync::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerManager(managerName);
    // Remove items on all collections
    m_om->removeItems(m_om->itemIds());
    // Remove all collections (except the default)
    foreach (const QOrganizerCollection& collection, m_om->collections()) {
        if (collection != m_om->defaultCollection())
            m_om->removeCollection(collection.id());
    }
}

void tst_SymbianOmAsync::cleanup()
{
    m_om->removeItems(m_om->itemIds());
    // Remove all collections (except the default)
    foreach (const QOrganizerCollection& collection, m_om->collections()) {
        if (collection != m_om->defaultCollection())
            m_om->removeCollection(collection.id());
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

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&saveItemRequest, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&saveItemRequest, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemId>)));

    // Save
    QVERIFY(saveItemRequest.start());
    QCOMPARE(saveItemRequest.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1); // one item saved
    QTRY_COMPARE(addedSpy.count(), 1);  // one item saved

    // Verify
    QCOMPARE(m_om->itemIds().count(), 1);
    QCOMPARE(saveItemRequest.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(saveItemRequest.error(), QOrganizerManager::NoError);
    QCOMPARE(saveItemRequest.items().count(), 1);
    QVERIFY(saveItemRequest.items().at(0).id() != QOrganizerItemId());
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
    QVERIFY(item.id() != QOrganizerItemId());
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Create fetch request
    QOrganizerItemFetchForExportRequest fetchItemRequest;
    fetchItemRequest.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&fetchItemRequest, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&fetchItemRequest, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(fetchItemRequest.start());
    QCOMPARE(fetchItemRequest.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1); // 1 item fetched

    // Verify
    QCOMPARE(fetchItemRequest.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(fetchItemRequest.error(), QOrganizerManager::NoError);
    QCOMPARE(fetchItemRequest.items().count(), 1);
    QVERIFY(fetchItemRequest.items().at(0).id() != QOrganizerItemId());
}

void tst_SymbianOmAsync::fetchWaitForFinished()
{
     // Create item
     QOrganizerItem item;
     item.setType(QOrganizerItemType::TypeTodo);

     // Save (synchronously)
     QVERIFY(m_om->saveItem(&item));
     QVERIFY(item.id() != QOrganizerItemId());
     QVERIFY(item.id().managerUri().contains(m_om->managerName()));

     // Create fetch request
     QOrganizerItemFetchForExportRequest fetchItemRequest;
     fetchItemRequest.setManager(m_om);

     // Create signal spys for verification purposes
     QSignalSpy stateSpy(&fetchItemRequest,
         SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
     QSignalSpy resultSpy(&fetchItemRequest, SIGNAL(resultsAvailable()));

     // Fetch
     QVERIFY(fetchItemRequest.start());
     QCOMPARE(fetchItemRequest.state(), QOrganizerAbstractRequest::ActiveState);
     QCOMPARE(stateSpy.count(), 1);  // inactive > active
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
    QOrganizerItemFetchForExportRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QVERIFY(resultSpy.count() > 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    QVERIFY(req.items().at(0).id() != QOrganizerItemId());
}

void tst_SymbianOmAsync::fetchItemIds()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitems"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemIdFetchRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QVERIFY(resultSpy.count() >= 1);   // allow one or more "resultsAvailable"

    // Verify
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.itemIds().count(), itemCount);
    QVERIFY(req.itemIds().at(0) != QOrganizerItemId());
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
    QOrganizerItemFetchForExportRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch with local id filter
    QOrganizerItemIdFilter idFilter;
    idFilter.setIds(m_om->itemIds().mid(pos, length));
    req.setFilter(idFilter);
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QVERIFY(resultSpy.count() > 1);

    // Verify
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.items().count(), length);
    // Assuming the sort order is the same:
    QVERIFY(req.items().at(0).id() == m_om->itemIds().at(pos));
}

void tst_SymbianOmAsync::fetchItemsNonExistingIds()
{
    // Save 3 items (synchronously)
    const int itemCount(3);
    QList<QOrganizerItem> items = createItems(QString("fetchitems"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Remove the second one (synhcronously)
    QVERIFY(m_om->removeItem(items[1].id()));

    // Create fetch request with id filter
    QOrganizerItemFetchForExportRequest req;
    req.setManager(m_om);
    QOrganizerItemIdFilter idFilter;
    QList<QOrganizerItemId> ids;
    foreach (QOrganizerItem item, items) {
        ids.append(item.id());
    }
    idFilter.setIds(ids);
    req.setFilter(idFilter);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished

    // Verify
    QVERIFY(resultSpy.count() > 1);
    QCOMPARE(req.items().count(), itemCount - 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::DoesNotExistError);
}

void tst_SymbianOmAsync::fetchItemsDetailFilter()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitemsdetailfilter"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchForExportRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch with detail filter
    QOrganizerItemDetailFilter detailFilter;
    detailFilter.setDetailDefinitionName(QOrganizerItemDisplayLabel::DefinitionName);
    req.setFilter(detailFilter);
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QVERIFY(resultSpy.count() > 1);

    // Verify
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    QVERIFY(req.items().at(0).id() != QOrganizerItemId());
}

void tst_SymbianOmAsync::fetchItemsSortOrder()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitemssortorder"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchForExportRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
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
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QVERIFY(resultSpy.count() > 1);

    // Verify
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    QVERIFY(req.items().at(0).id() != QOrganizerItemId());
}

void tst_SymbianOmAsync::fetchItemsDeleteRequest()
{
    // Save items (synchronously)
    const int itemCount(100);
    QList<QOrganizerItem> items = createItems(QString("fetchitems"), itemCount);
    QVERIFY(m_om->saveItems(&items));

    // Create fetch request
    QOrganizerItemFetchForExportRequest *req = new QOrganizerItemFetchForExportRequest();
    QWeakPointer<QObject> obj(req);
    req->setManager(m_om);

    // Connect "resultsAvailable" to a slot that deletes the sender
    // That is, verify we don't crash when the request is deleted in the resultsAvailable slot function
    connect(req, SIGNAL(resultsAvailable()), this, SLOT(deleteRequest()));

    // Fetch
    QVERIFY(req->start());
    QTRY_COMPARE(obj.isNull(), true);
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
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemId>)));

    // Save
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2); // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QVERIFY(addedSpy.count() > 0);
    // Check that there were more than one "added" signal. The reasoning for this is that if you
    // create several items asynchronously, there is not much point in the async api if you only
    // get one signal. For example showing information like "54% complete" to the user is not
    // possible if you get only one signal once the request is completed.
    QVERIFY(addedSpy.count() > 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.items().count(), itemCount);
    foreach (QOrganizerItem item, req.items()) {
        QVERIFY(!item.id().isNull());
    }
}

void tst_SymbianOmAsync::addItemsMultiReq()
{
    // Create items
    QOrganizerItem item1 = createItem(QOrganizerItemType::TypeEvent, "multireq1", QDateTime::currentDateTime());
    QOrganizerItem item2 = createItem(QOrganizerItemType::TypeEvent, "multireq2", QDateTime::currentDateTime());
    QOrganizerItem item3 = createItem(QOrganizerItemType::TypeEvent, "multireq3", QDateTime::currentDateTime());

    // Create save requests
    QOrganizerItemSaveRequest req1;
    req1.setManager(m_om);
    req1.setItem(item1);
    QOrganizerItemSaveRequest req2;
    req2.setManager(m_om);
    req2.setItem(item2);
    QOrganizerItemSaveRequest req3;
    req3.setManager(m_om);
    req3.setItem(item3);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy1(&req1, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy1(&req1, SIGNAL(resultsAvailable()));
    QSignalSpy stateSpy2(&req2, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy2(&req2, SIGNAL(resultsAvailable()));
    QSignalSpy stateSpy3(&req3, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy3(&req3, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemId>)));

    // Save
    QVERIFY(req1.start());
    QVERIFY(req2.start());
    QVERIFY(req3.start());
    QCOMPARE(req1.state(), QOrganizerAbstractRequest::ActiveState);
    QCOMPARE(req2.state(), QOrganizerAbstractRequest::ActiveState);
    QCOMPARE(req3.state(), QOrganizerAbstractRequest::ActiveState);

    // Verify
    QTRY_COMPARE(stateSpy1.count(), 2); // inactive > active > finished
    QTRY_COMPARE(stateSpy2.count(), 2); // inactive > active > finished
    QTRY_COMPARE(stateSpy3.count(), 2); // inactive > active > finished
    QTRY_COMPARE(resultSpy1.count(), 1);
    QTRY_COMPARE(resultSpy2.count(), 1);
    QTRY_COMPARE(resultSpy3.count(), 1);
    QCOMPARE(addedSpy.count(), 3);
    QCOMPARE(req1.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req1.error(), QOrganizerManager::NoError);
    QCOMPARE(m_om->itemIds().count(), 3);
    foreach (QOrganizerItemId id, m_om->itemIds()) {
        QVERIFY(!id.isNull());
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
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy changedSpy(m_om, SIGNAL(itemsChanged(QList<QOrganizerItemId>)));

    // Save
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QVERIFY(changedSpy.count() > 0);
    QVERIFY(changedSpy.count() > 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
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
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy removedSpy(m_om, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)));

    // Remove
    req.setItemIds(m_om->itemIds());
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QVERIFY(removedSpy.count() > 0);
    QVERIFY(removedSpy.count() > 1);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(m_om->itemIds().count(), 0);
}

void tst_SymbianOmAsync::addCollection()
{
    // Create the request
    QOrganizerCollectionSaveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));

    // Create new collection
    QOrganizerCollection collection;
    collection.setMetaData("Name", "addCollection");
    collection.setMetaData("FileName", "c:addCollection");
    collection.setMetaData("Description", "addCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    req.setCollection(collection);
    
    // Not supported?
    if (!m_customCollectionsSupported) {
        QWARN("Saving collections not supported!");
        QVERIFY(!req.start());
        QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
        QCOMPARE(stateSpy.count(), 1);
        QTRY_COMPARE(resultSpy.count(), 1);
        return;
    }

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(addedSpy.count(), 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QList<QOrganizerCollection> collections = m_om->collections();
    QCOMPARE(collections.count(), 2); // the default plus the new one
    QCOMPARE(collections.at(1).metaData().value("Name").toString(), QString("addCollection"));
    // Verify the signal emitted contains the id of the new collection
    QCOMPARE(addedSpy.last().count(), 1);
    QCOMPARE(addedSpy.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(addedSpy.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req.collections().at(0).id());
}

void tst_SymbianOmAsync::modifyCollection()
{
    if (!m_customCollectionsSupported)
        QSKIP("Saving/modifying collections not supported!", SkipSingle);
        
    // Create async request
    QOrganizerCollectionSaveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy(m_om, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));
    QSignalSpy changedSpy(m_om, SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)));

    // Create new collection (synchronously)
    QOrganizerCollection collection;
    collection.setMetaData("Name", "modifyCollection");
    collection.setMetaData("FileName", "c:modifyCollection");
    collection.setMetaData("Description", "modifyCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&collection));

    // Modify the newly created collection asynchronously
    collection.setMetaData("Description", "modifyCollection test2");
    req.setCollection(collection);

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(addedSpy.count(), 1);
    QTRY_COMPARE(changedSpy.count(), 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QList<QOrganizerCollection> collections = m_om->collections();
    QCOMPARE(collections.count(), 2); // the default plus the new one
    QCOMPARE(collections.at(1).metaData().value("Name").toString(), QString("modifyCollection"));
    QCOMPARE(collections.at(1).metaData().value("Description").toString(), QString("modifyCollection test2"));
}

void tst_SymbianOmAsync::removeCollection()
{
    // Create async request
    QOrganizerCollectionRemoveRequest req;
    req.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy removedSpy(m_om, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)));
    
    // Not supported?
    if (!m_customCollectionsSupported) {
        QWARN("Removing collections not supported!");
        QVERIFY(!req.start());
        QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
        QCOMPARE(stateSpy.count(), 1);
        QTRY_COMPARE(resultSpy.count(), 1);
        return;
    }    

    // Create new collection (synchronously)
    QOrganizerCollection collection;
    collection.setMetaData("Name", "removeCollection");
    collection.setMetaData("FileName", "c:removecollection");
    collection.setMetaData("Description", "removeCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&collection));

    // Remove the new collection
    req.setCollectionId(collection.id());

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QTRY_COMPARE(removedSpy.count(), 1);

    // Verify
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(m_om->collections().count(), 1); // the default

    // Try to remove again, should fail
    req.setCollectionId(collection.id());
    QVERIFY(req.start());
    QTRY_COMPARE(resultSpy.count(), 2);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::DoesNotExistError);
}

void tst_SymbianOmAsync::addCollectionMultiManager()
{
    // Create a second manager
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));

    // Create request
    QOrganizerCollectionSaveRequest req;
    req.setManager(m_om);

    // Setup signal spies
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy1(m_om, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));
    QSignalSpy addedSpy2(om2.data(), SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));

    // Creating new collections is not supported on legacy symbian devices
    if (!m_customCollectionsSupported) {
        QWARN("Removing collections not supported!");
        QVERIFY(!req.start());
        QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
        QCOMPARE(stateSpy.count(), 1);
        QTRY_COMPARE(resultSpy.count(), 1);
        return;
    }    

    // Create a collection
    QOrganizerCollection collection;
    collection.setMetaData("Name", "saveEmission");
    collection.setMetaData("FileName", "c:saveEmission");
    collection.setMetaData("Description", "saveEmission test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    req.setCollection(collection);

    // Start the request
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req.collections().count(), 1);
    QVERIFY(!req.collections().at(0).id().isNull());
    QVERIFY(!req.collections().at(0).id().managerUri().isEmpty());
    // Verify the count of collectionsAdded signals on both managers
    QTRY_COMPARE(addedSpy1.count(), 1);
    QTRY_COMPARE(addedSpy2.count(), 1);
    // Verify the argument counts of the collectionsAdded signals
    QCOMPARE(addedSpy1.last().count(), 1);
    QCOMPARE(addedSpy2.last().count(), 1);
    // Verify the arguments contain the id of the new collection
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req.collections().at(0).id());
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req.collections().at(0).id());
}

void tst_SymbianOmAsync::detailDefinitionFetch()
{
    // Create request
    QOrganizerItemDetailDefinitionFetchRequest req;
    req.setManager(m_om);
    
    // Setup signal spies
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    
    // Fetch by item type only
    req.setItemType(QOrganizerItemType::TypeEvent);
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(req.error(), QOrganizerManager::NoError);
    QCOMPARE(req.errorMap().count(), 0);
    QVERIFY(req.definitions().count() > 0);
    stateSpy.clear();
    resultSpy.clear();
    
    // Fetch by item type and detail definitions (some not supported) 
    req.setItemType(QOrganizerItemType::TypeEvent);
    QStringList names;
    names << QOrganizerItemDisplayLabel::DefinitionName;
    names << QOrganizerItemComment::DefinitionName; // not supported
    req.setDefinitionNames(names);
    QVERIFY(req.start());
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(req.error(), QOrganizerManager::DoesNotExistError);
    QCOMPARE(req.errorMap().count(), 1);
    QCOMPARE(req.errorMap().value(1), QOrganizerManager::DoesNotExistError);
    QVERIFY(req.definitions().count() == 1);    
    stateSpy.clear();
    resultSpy.clear();
    
    // Try fetching with no parameters
    req.setItemType(QString());
    req.setDefinitionNames(QStringList());
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
    QCOMPARE(req.errorMap().count(), 0);
    QVERIFY(req.definitions().count() == 0);
    stateSpy.clear();
    resultSpy.clear();
        
    // Fetch by not supported item type
    req.setItemType(QOrganizerItemType::TypeJournal);
    QVERIFY(req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::ActiveState);
    QTRY_COMPARE(stateSpy.count(), 2);  // inactive > active > finished
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
    QCOMPARE(req.errorMap().count(), 0);
    QVERIFY(req.definitions().count() == 0);
    stateSpy.clear();
    resultSpy.clear();   
}

void tst_SymbianOmAsync::detailDefinitionSave()
{
    // Create request
    QOrganizerItemDetailDefinitionSaveRequest req;
    req.setManager(m_om);
    
    // Setup signal spies
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    
    // Saving detail definitions is not supported so verify it cannot be started 
    QVERIFY(!req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
    QCOMPARE(req.errorMap().count(), 0);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);    
}

void tst_SymbianOmAsync::detailDefinitionRemove()
{
    // Create request
    QOrganizerItemDetailDefinitionRemoveRequest req;
    req.setManager(m_om);
    
    // Setup signal spies
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    
    // Removing detail definitions is not supported so verify it cannot be started 
    QVERIFY(!req.start());
    QCOMPARE(req.state(), QOrganizerAbstractRequest::FinishedState);
    QCOMPARE(req.error(), QOrganizerManager::NotSupportedError);
    QCOMPARE(req.errorMap().count(), 0);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);   
}

void tst_SymbianOmAsync::detailDefinitionRemoveDeleteRequest()
{
    // Create request
    QOrganizerItemDetailDefinitionRemoveRequest *req = new QOrganizerItemDetailDefinitionRemoveRequest();
    QWeakPointer<QObject> obj(req);
    req->setManager(m_om);

    // Connect signals "resultsAvailable" to a slot that deletes the sender
    // That is, verify we don't crash when the request is deleted in the slot function.
    connect(req, SIGNAL(resultsAvailable()), this, SLOT(deleteRequest()));

    // Removing detail definitions is not supported so verify it cannot be started
    QVERIFY(!req->start());
    QTRY_COMPARE(obj.isNull(), true);
}

/*!
 * A slot function for deleting the sender.
 */
void tst_SymbianOmAsync::deleteRequest()
{
    delete sender();
}

/*!
 * A helper method for creating a QOrganizerItem instance.
 */
QOrganizerItem tst_SymbianOmAsync::createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime)
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

    QStringList managerNames = QOrganizerManager::availableManagers();
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
