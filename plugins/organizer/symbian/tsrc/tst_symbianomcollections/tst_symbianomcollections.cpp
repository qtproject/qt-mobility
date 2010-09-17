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
#include <QColor>

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
    QTRY_COMPARE(addedSpy1.count(), addedCount); \
    QTRY_COMPARE(addedSpy2.count(), addedCount); \
    QTRY_COMPARE(changedSpy1.count(), changedCount); \
    QTRY_COMPARE(changedSpy2.count(), changedCount); \
    QTRY_COMPARE(removedSpy1.count(), removedCount); \
    QTRY_COMPARE(removedSpy2.count(), removedCount);
    
Q_DECLARE_METATYPE(QList<QOrganizerItemLocalId>)

// TODO:
// We cannot declare this because its QOrganizerCollectionLocalId quint32 and so is QOrganizerItemLocalId.
// This means using QVariant::value<QList<QOrganizerCollectionLocalId> >() won't work.
// The upcoming API change on QOrganizerCollectionLocalId will however fix this issue.   
//Q_DECLARE_METATYPE(QList<QOrganizerCollectionLocalId>)

/*!
 * For testing symbian backend via QOrganizerItemManager API. The target is
 * to implement test cases in a platform independent manner so that this test
 * module could be used also with other backends and potentially used as a
 * QtMobility auto test with as little porting work as possible.
 */
class tst_symbianomcollections : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases

    void collectionIds_data(){ addManagers(); };
    void collectionIds();

    void fetchCollection_data(){ addManagers(); };
    void fetchCollection();
    
    void saveCollection_data(){ addManagers(); };
    void saveCollection();
    
    void removeCollection_data(){ addManagers(); };
    void removeCollection();
    
    void collectionSignalEmission_data(){ addManagers(); };
    void collectionSignalEmission();
    
    void itemSignalEmission_data(){ addManagers(); };
    void itemSignalEmission();

    void addItem_data(){ addManagers(); };
    void addItem();

    void fetchItem_data(){ addManagers(); };
    void fetchItem();

    void modifyItem_data(){ addManagers(); };
    void modifyItem();

    void removeItem_data(){ addManagers(); };
    void removeItem();

    void fetchItems_data(){ addManagers(); };
    void fetchItems();

    void fetchItemInstance_data(){ addManagers(); };
    void fetchItemInstance();

    // TODO: test all known properties
    //void collectionProperties_data();
    //void collectionProperties();

private: // util functions
    QOrganizerItem createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime = QDateTime());
    void addManagers();

private:
    QOrganizerItemManager *m_om;
};


void tst_symbianomcollections::init()
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

void tst_symbianomcollections::cleanup()
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

void tst_symbianomcollections::collectionIds()
{
    // Get default collection id
    QOrganizerCollectionLocalId dId = m_om->defaultCollectionId();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(dId > 0);

    // Get all collection ids
    QList<QOrganizerCollectionLocalId> ids = m_om->collectionIds();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(ids.contains(dId));
}

void tst_symbianomcollections::fetchCollection()
{
    // Fetch default collection id
    QOrganizerCollectionLocalId dId = m_om->defaultCollectionId();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(dId > 0);
    
    // Get all collections
    QList<QOrganizerCollection> cs = m_om->collections();
    QVERIFY(cs.count() >= 1); // default collection should always be present
    
    // Dump all to log
    foreach (QOrganizerCollection collection, cs) {
        qDebug() << "Collection id:" << collection.id();
        foreach (QString key, collection.metaData().keys())
            qDebug() << "\t" << key << collection.metaData().value(key);
    }
    
    // Get a specific collection
    cs = m_om->collections(QList<QOrganizerCollectionLocalId>() << dId);
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(cs.count() == 1);

    // Do a basic verify
    QVERIFY(cs[0].id().localId() == dId);
    QVERIFY(cs[0].id().managerUri() == m_om->managerUri());    
}

void tst_symbianomcollections::saveCollection()
{
    // Save a collection
    // TODO: keys should be defined somewhere
    QOrganizerCollection c1;
    c1.setMetaData("Name", "testname");
    c1.setMetaData("FileName", "c:testcalendar");
    c1.setMetaData("Description", "this is a test collection");
    c1.setMetaData("OwnerName", "test");
    c1.setMetaData("Color", QColor(Qt::red));
    c1.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&c1));
    QVERIFY(c1.id().localId());

    // Verify
    QList<QOrganizerCollection> cs = m_om->collections(QList<QOrganizerCollectionLocalId>() << c1.id().localId());
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(cs.count() == 1);
    foreach (QString key, c1.metaData().keys()) {
        if (cs[0].metaData(key) != c1.metaData(key)) {
            qDebug() << "key:" << key;
            qDebug() << "saved value:" << c1.metaData(key);
            qDebug() << "fetched value:" << cs[0].metaData(key);            
            QFAIL("Meta data does not match!");
        }
    }
    
    // Modify and save
    c1.setMetaData("Name", "modified testname");
    c1.setMetaData("Color", QColor(Qt::green));
    c1.setMetaData("Enabled", false);
    QVERIFY(m_om->saveCollection(&c1));
    
    // Verify again
    cs = m_om->collections(QList<QOrganizerCollectionLocalId>() << c1.id().localId());
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(cs.count() == 1);
    foreach (QString key, c1.metaData().keys()) {
        if (cs[0].metaData(key) != c1.metaData(key)) {
            qDebug() << "key:" << key;
            qDebug() << "saved value:" << c1.metaData(key);
            qDebug() << "fetched value:" << cs[0].metaData(key);            
            QFAIL("Meta data does not match!");
        }
    }
     
    // Try to modify filename. Should fail.
    QOrganizerCollection c2 = c1;
    c2.setMetaData("FileName", "c:testcalendarmodified");
    QVERIFY(!m_om->saveCollection(&c2));
    
    // Try saving with unknown id. Should fail.
    c2 = c1;
    QOrganizerCollectionId id = c2.id();
    id.setLocalId(12345);
    c2.setId(id);
    QVERIFY(!m_om->saveCollection(&c2));
    
    // Try saving with unknown manager uri. Should fail.
    c2 = c1;
    id = c2.id();
    id.setManagerUri("foobar");
    c2.setId(id);
    QVERIFY(!m_om->saveCollection(&c2));    
}

void tst_symbianomcollections::removeCollection()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "testremove");
    c.setMetaData("FileName", "c:testremovecalendar");
    QVERIFY(m_om->saveCollection(&c));
    
    // Remove it
    // Backend should be able to remove the calendar file itself
    QVERIFY(m_om->removeCollection(c.id().localId()));
    
    // Save again
    c.setId(QOrganizerCollectionId());
    QVERIFY(m_om->saveCollection(&c));
    
    // Create a second manager which will open the same collection
    QScopedPointer<QOrganizerItemManager> om2(new QOrganizerItemManager(m_om->managerName()));
    QList<QOrganizerCollectionLocalId> ids = om2->collectionIds();
    QVERIFY(ids.contains(c.id().localId())); // we assume that collections local id is global between managers
    
    // Remove the collection again
    // Now the backend cannot remove the calendar file because om2 has it open.
    // It must set the MarkAsDelete flag instead. 
    QVERIFY(m_om->removeCollection(c.id().localId()));

    // Create a third manager
    QScopedPointer<QOrganizerItemManager> om3(new QOrganizerItemManager(m_om->managerName()));

    // Collection should not appear in any manager
    QTRY_COMPARE(m_om->collectionIds().count(), 1);
    QTRY_COMPARE(om2->collectionIds().count(), 1);
    QTRY_COMPARE(om3->collectionIds().count(), 1);

    // Save yet again. Backend should remove the MarkAsDelete flag from the calendar.
    c.setId(QOrganizerCollectionId());
    QVERIFY(m_om->saveCollection(&c));
    
    // Free collections for removing
    delete om2.take();
    delete om3.take();
    
    // Remove all collections
    ids = m_om->collectionIds();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(ids.count() >= 2); // default collection + collection saved
    foreach (QOrganizerCollectionLocalId id, ids) {
        if (id == m_om->defaultCollectionId())
            QVERIFY(!m_om->removeCollection(id)); // removing default collection not allowed
        else
            QVERIFY(m_om->removeCollection(id));
    }
    
    // Try removing the same ones again 
    foreach (QOrganizerCollectionLocalId id, ids) {
        QVERIFY(!m_om->removeCollection(id));
    }    
}

void tst_symbianomcollections::collectionSignalEmission()
{
    // Create a second manager
    QScopedPointer<QOrganizerItemManager> om2(new QOrganizerItemManager(m_om->managerName()));
    
    // Setup signal spies
    qRegisterMetaType<QList<QOrganizerCollectionLocalId> >("QList<QOrganizerCollectionLocalId>");
    QSignalSpy addedSpy1(m_om, SIGNAL(collectionsAdded(const QList<QOrganizerCollectionLocalId>&)));
    QSignalSpy addedSpy2(om2.data(), SIGNAL(collectionsAdded(const QList<QOrganizerCollectionLocalId>&)));
    QSignalSpy changedSpy1(m_om, SIGNAL(collectionsChanged(const QList<QOrganizerCollectionLocalId>&)));
    QSignalSpy changedSpy2(om2.data(), SIGNAL(collectionsChanged(const QList<QOrganizerCollectionLocalId>&)));
    QSignalSpy removedSpy1(m_om, SIGNAL(collectionsRemoved(const QList<QOrganizerCollectionLocalId>&)));
    QSignalSpy removedSpy2(om2.data(), SIGNAL(collectionsRemoved(const QList<QOrganizerCollectionLocalId>&)));
    int addedCount = 0;
    int changedCount = 0;
    int removedCount = 0;

    // save collection
    QOrganizerCollection c;
    c.setMetaData("Name", "testcollectionsignalemission");
    c.setMetaData("FileName", "c:testcollectionsignalemission");
    QVERIFY(m_om->saveCollection(&c));
    addedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    // TODO: Disabled temporarily. See Q_DECLARE_METATYPE(QList<QOrganizerCollectionLocalId>) comments.
    //QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionLocalId> >().count(), 1);
    //QVERIFY(addedSpy1.last().at(0).value<QList<QOrganizerCollectionLocalId> >().contains(c.id().localId()));
    //QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionLocalId> >().count(), 1);
    //QVERIFY(addedSpy2.last().at(0).value<QList<QOrganizerCollectionLocalId> >().contains(c.id().localId()));
    
    // Modify collection
    c.setMetaData("Name", "testsignalemissionmodified");
    QVERIFY(m_om->saveCollection(&c));
    changedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Remove collection
    // This will not actually remove the calendar file. It will only be marked for deletion.
    // This is because the calendar file is open at om2 (and possibly native symbian calendar).
    QVERIFY(m_om->removeCollection(c.id().localId()));
    removedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Save collection again. Now the calendar file exists so saving the collection
    // will just remove the EMarkAsDelete flag.
    c.setId(QOrganizerCollectionId());
    QVERIFY(m_om->saveCollection(&c));
    addedCount++;
    QTRY_COMPARE(addedSpy1.count(), addedCount);
    QTRY_COMPARE(addedSpy2.count(), addedCount);
    //QTRY_COMPARE(changedSpy1.count(), changedCount);
    QWARN("Known issue: Enabling a collection which was marked for deletion causes an extra changed signal!");
    QTRY_COMPARE(changedSpy2.count(), changedCount);
    QTRY_COMPARE(removedSpy1.count(), removedCount);
    QTRY_COMPARE(removedSpy2.count(), removedCount);
    
    // Release the calendar file for deleting
    delete om2.take();
    QVERIFY(m_om->removeCollection(c.id().localId()));
    removedCount++;
    QTRY_COMPARE(removedSpy1.count(), removedCount);
}

void tst_symbianomcollections::itemSignalEmission()
{
    // NOTE: Default collection signals are already tested with tst_SymbianOm::signalEmission
    
    // Save a new collection
    QOrganizerCollection c;
    c.setMetaData("Name", "testitemsignalemission");
    c.setMetaData("FileName", "c:testitemsignalemission");
    QVERIFY(m_om->saveCollection(&c));
    
    // Create a second manager
    QScopedPointer<QOrganizerItemManager> om2(new QOrganizerItemManager(m_om->managerName()));
    
    // Setup signal spies
    qRegisterMetaType<QList<QOrganizerItemLocalId> >("QList<QOrganizerItemLocalId>");
    QSignalSpy addedSpy1(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy addedSpy2(om2.data(), SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy changedSpy1(m_om, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy changedSpy2(om2.data(), SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy removedSpy1(m_om, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    QSignalSpy removedSpy2(om2.data(), SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    int addedCount = 0;
    int changedCount = 0;
    int removedCount = 0;
    
    // Save item
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                     QString("testitemsignalemission"),
                                     QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&item, c.id().localId()));
    addedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Change item
    item.setDisplayLabel("testitemsignalemission changed");
    QVERIFY(m_om->saveItem(&item, c.id().localId()));
    changedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Remove item
    QVERIFY(m_om->removeItem(item.localId()));
    removedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Save items - batch api
    QList<QOrganizerItem> items;
    items << createItem(QOrganizerItemType::TypeEvent,
                        QString("testitemsignalemission1"),
                        QDateTime::currentDateTime().addMSecs(3600));
    items << createItem(QOrganizerItemType::TypeEvent,
                        QString("testitemsignalemission2"),
                        QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItems(&items, c.id().localId()));
    addedCount++; // There should be one signal from both managers and the list should contain 2 items
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), 2);
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), 2);
        
    // Change items - batch api
    items[0].setDisplayLabel("testitemsignalemission1 changed");
    items[1].setDisplayLabel("testitemsignalemission2 changed");
    QVERIFY(m_om->saveItems(&items, c.id().localId()));
    changedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(changedSpy1.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), 2);
    QCOMPARE(changedSpy2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), 2);
    
    // Remove items - batch api
    QList<QOrganizerItemLocalId> ids;
    ids << items[0].localId() << items[1].localId();
    QVERIFY(m_om->removeItems(ids, 0));
    removedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(removedSpy1.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), 2);
    QCOMPARE(removedSpy2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), 2);
}

void tst_symbianomcollections::addItem()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "addItem");
    c.setMetaData("FileName", "c:additem");
    QVERIFY(m_om->saveCollection(&c));

    // Save similar item to both of the collections
    QOrganizerItem item1 = createItem(QOrganizerItemType::TypeEvent,
                                      QString("additem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QOrganizerItem item2 = item1;
    QVERIFY(m_om->saveItem(&item1));
    QVERIFY(m_om->saveItem(&item2, c.id().localId()));

    // Verify
    QVERIFY(item1.localId() != item2.localId());
    QCOMPARE(item1.collectionId().localId(), m_om->defaultCollectionId());
    QCOMPARE(item2.collectionId().localId(), c.id().localId());
}

void tst_symbianomcollections::fetchItem()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "fetchItem");
    c.setMetaData("FileName", "c:fetchitem");
    QVERIFY(m_om->saveCollection(&c));

    // Save an item to the newly created collection
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("fetchitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&item, c.id().localId()));

    // Fetch and verify the item
    // Note: collections need to be empty before executing the test case
    QCOMPARE(m_om->itemIds().count(), 1);
    QCOMPARE(m_om->item(item.localId()).collectionId().localId(), c.id().localId());
    QCOMPARE(m_om->item(item.localId()).displayLabel(), QString("fetchitem"));
}

void tst_symbianomcollections::modifyItem()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "modifyItem");
    c.setMetaData("FileName", "c:modifyitem");
    QVERIFY(m_om->saveCollection(&c));

    // Save similar item to both of the collections
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("additem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&item, c.id().localId()));

    // Modify and save again
    item.setDisplayLabel("modifyitem");
    QVERIFY(m_om->saveItem(&item, c.id().localId()));

    // Verify
    QCOMPARE(m_om->item(item.localId()).displayLabel(), QString("modifyitem"));
    QCOMPARE(m_om->item(item.localId()).collectionId().localId(), c.id().localId());
}

void tst_symbianomcollections::removeItem()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "removeItem");
    c.setMetaData("FileName", "c:removeitem");
    QVERIFY(m_om->saveCollection(&c));

    // Save an item to the newly created collection
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("removeitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&item, c.id().localId()));

    // Remove item
    QOrganizerItemLocalId localId = item.localId();
    QVERIFY(m_om->removeItem(localId));

    // Verify
    QVERIFY(m_om->item(localId).isEmpty());
}

void tst_symbianomcollections::fetchItems()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "fetchItems");
    c.setMetaData("FileName", "c:fetchitems");
    QVERIFY(m_om->saveCollection(&c));

    const int itemCount(100);
    for (int i(0); i < itemCount; i++) {
        // Save similar item to both of the collections
        QOrganizerItem item1 = createItem(QOrganizerItemType::TypeEvent,
                                          QString("additem")+ i,
                                          QDateTime::currentDateTime().addMSecs(3600));
        QOrganizerItem item2 = item1;
        QVERIFY(m_om->saveItem(&item1));
        QVERIFY(m_om->saveItem(&item2, c.id().localId()));
    }

    // Verify
    QCOMPARE(m_om->itemIds().count(), itemCount * 2);
    QCOMPARE(m_om->items().count(), itemCount * 2);
    // Verify there are no duplicates in the list
    bool noduplicates(true);
    foreach(QOrganizerItemLocalId localid, m_om->itemIds()) {
        if (m_om->itemIds().count(localid) > 1) {
            qWarning() << "duplicate local id: " << localid;
            noduplicates = false;
        }
    }
    QVERIFY(noduplicates);
}

void tst_symbianomcollections::fetchItemInstance()
{
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "fetchItemInstance");
    c.setMetaData("FileName", "c:fetchiteminstance");
    QVERIFY(m_om->saveCollection(&c));

    // Save a weekly recurring item
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("fetchiteminstance"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(5);
    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item, c.id().localId()));

    // Verify
    QCOMPARE(m_om->itemInstances().count(), 5);
    QCOMPARE(m_om->items().count(), 1);
    QCOMPARE(m_om->items().at(0).collectionId(), c.id());
}

/*!
 * A helper method for creating a QOrganizerItem instance.
 */
QOrganizerItem tst_symbianomcollections::createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime)
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
 * Helper method for executing test cases with all the available managers
 */
void tst_symbianomcollections::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    managerNames.removeAll("memory");

    foreach(QString mgr, managerNames) {
        QTest::newRow(QString("[%1]").arg(mgr).toLatin1().constData()) << mgr;
    }
}
QTEST_MAIN(tst_symbianomcollections);
#include "tst_symbianomcollections.moc"
