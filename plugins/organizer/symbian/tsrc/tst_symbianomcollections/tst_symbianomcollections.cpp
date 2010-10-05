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
#include <calcommon.h> // SYMBIAN_CALENDAR_V2 is defined here

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
Q_DECLARE_METATYPE(QList<QOrganizerCollectionLocalId>)

/*!
 * For testing symbian backend via QOrganizerItemManager API. The target is
 * to implement test cases in a platform independent manner so that this test
 * module could be used also with other backends and potentially used as a
 * QtMobility auto test with as little porting work as possible.
 */
class tst_symbianomcollections : public QObject
{
    Q_OBJECT
    
public slots:
    void initTestCase();

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

    void removeItems_data(){ addManagers(); };
    void removeItems();

    void fetchItemInstance_data(){ addManagers(); };
    void fetchItemInstance();

    void modifyItemInstance_data(){ addManagers(); };
    void modifyItemInstance();

    void itemsInDeletedCollection_data(){ addManagers(); };
    void itemsInDeletedCollection();

    // TODO: test all known properties
    //void collectionProperties_data();
    //void collectionProperties();

private: // util functions
    QOrganizerItem createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime = QDateTime());
    void addManagers();

private:
    QOrganizerItemManager *m_om;
    bool m_customCollectionsSupported;
};

void tst_symbianomcollections::initTestCase()
{
    // TODO: How could this be done dynamically? 
    // Some kind of manager feature flag would be nice.
#ifdef SYMBIAN_CALENDAR_V2
    m_customCollectionsSupported = true;
#else
    m_customCollectionsSupported = false;
#endif
}

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
    QVERIFY(!dId.isNull());

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
    QVERIFY(!dId.isNull());
    
    // Get collections but do not provide id's
    QList<QOrganizerCollection> cs = m_om->collections();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(cs.count() == 0);
    
    // Get all collections
    cs = m_om->collections(m_om->collectionIds());
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

    // fetch entries for an non existent calendar
    QOrganizerCollectionLocalId nonId;
    cs.clear();
    cs = m_om->collections(QList<QOrganizerCollectionLocalId>() << nonId);
    QVERIFY(m_om->error() == QOrganizerItemManager::DoesNotExistError);

    // Can we save collections? 
    if (!m_customCollectionsSupported)
        return;
    
    // add a collection and fetch
    QOrganizerCollection c1;
    c1.setMetaData("Name", "testname");
    c1.setMetaData("FileName", "c:testcalendar");
    c1.setMetaData("Description", "this is a test collection");
    c1.setMetaData("OwnerName", "test");
    c1.setMetaData("Color", QColor(Qt::red));
    c1.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&c1));
    cs.clear();
    cs = m_om->collections(m_om->collectionIds());
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(cs.count() == 2);
    
    //remove and then fetch the collections
    QVERIFY(m_om->removeCollection(c1.id().localId()));
        
    cs.clear();
    cs = m_om->collections(m_om->collectionIds());
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(cs.count() == 1);
    
    // fetch an already removed collection
    cs.clear();
    cs = m_om->collections(QList<QOrganizerCollectionLocalId>() << c1.id().localId());
    QVERIFY(m_om->error() == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(cs.count() == 0);
}

void tst_symbianomcollections::saveCollection()
{
    // Can we save collections? 
    if (!m_customCollectionsSupported) {
        QWARN("Saving a collection not supported!");
        // Verify it fails with correct error
        QOrganizerCollection c1;
        QVERIFY(!m_om->saveCollection(&c1));
        QVERIFY(m_om->error() == QOrganizerItemManager::NotSupportedError);        
        return;
    }

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
    QVERIFY(!c1.id().localId().isNull());

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
    
    // Try saving with unknown manager uri. Should fail.
    c2 = c1;
    QOrganizerCollectionId id = c2.id();
    id.setManagerUri("foobar");
    c2.setId(id);
    QVERIFY(!m_om->saveCollection(&c2));
    
    // Remove the collection
    QVERIFY(m_om->removeCollection(c1.localId()));
    
    // Try saving again without clearing local id. Should fail.
    QVERIFY(!m_om->saveCollection(&c1));
}

void tst_symbianomcollections::removeCollection()
{
    // Do we support removing collections?
    if (!m_customCollectionsSupported) {    
        QWARN("Removing a collection not supported!");
        // Verify it fails with correct error code
        QVERIFY(!m_om->removeCollection(m_om->defaultCollectionId()));
        QVERIFY(m_om->error() == QOrganizerItemManager::NotSupportedError);
        return;
    }
    
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
    // Check we can test this
    if (!m_customCollectionsSupported)
        QSKIP("Collection signals not supported!", SkipSingle);
    
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
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionLocalId> >().count(), 1);
    QVERIFY(addedSpy1.last().at(0).value<QList<QOrganizerCollectionLocalId> >().contains(c.id().localId()));
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionLocalId> >().count(), 1);
    QVERIFY(addedSpy2.last().at(0).value<QList<QOrganizerCollectionLocalId> >().contains(c.id().localId()));
    
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
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
        
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
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }

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
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }

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
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
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
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
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
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
    // Create a new collection
    QOrganizerCollection c;
    c.setMetaData("Name", "fetchItems");
    c.setMetaData("FileName", "c:fetchitems");
    QVERIFY(m_om->saveCollection(&c));

    // Create items to the collections
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

void tst_symbianomcollections::removeItems()
{
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
    // Create two new collections
    QOrganizerCollection c1;
    c1.setMetaData("Name", "removeItems1");
    c1.setMetaData("FileName", "c:removeitems1");
    QVERIFY(m_om->saveCollection(&c1));
    QOrganizerCollection c2;
    c2.setMetaData("Name", "removeItems2");
    c2.setMetaData("FileName", "c:removeitems2");
    QVERIFY(m_om->saveCollection(&c2));

    // Create items to the collections
    const int itemCount(100);
    for (int i(0); i < itemCount; i++) {
        // Save similar item to both of the collections
        QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                          QString("removeitem")+ i,
                                          QDateTime::currentDateTime().addMSecs(3600));
        QOrganizerItem item1 = item;
        QOrganizerItem item2 = item;
        QVERIFY(m_om->saveItem(&item));
        QVERIFY(m_om->saveItem(&item1, c1.localId()));
        QVERIFY(m_om->saveItem(&item2, c2.localId()));
    }

    // Verify
    QCOMPARE(m_om->itemIds().count(), itemCount * 3);

    // Filter for the first new collection
    QOrganizerItemCollectionFilter filter1;
    QSet<QOrganizerCollectionLocalId> collectionIds1;
    collectionIds1.insert(c1.localId());
    filter1.setCollectionIds(collectionIds1);

    // Filter for the second new collection
    QOrganizerItemCollectionFilter filter2;
    QSet<QOrganizerCollectionLocalId> collectionIds2;
    collectionIds2.insert(c2.localId());
    filter2.setCollectionIds(collectionIds2);

    // Remove from the first new collection and verify
    QVERIFY(m_om->removeItems(m_om->itemIds(filter1), 0));
    QCOMPARE(m_om->itemIds(filter1).count(), 0);
    QCOMPARE(m_om->itemIds(filter2).count(), itemCount);
    QCOMPARE(m_om->itemIds().count(), itemCount * 2);

    // Remove from the second new collection and verify
    QVERIFY(m_om->removeItems(m_om->itemIds(filter2), 0));
    QCOMPARE(m_om->itemIds(filter1).count(), 0);
    QCOMPARE(m_om->itemIds(filter2).count(), 0);
    QCOMPARE(m_om->itemIds().count(), itemCount);

    // Remove from all collections and verify
    QVERIFY(m_om->removeItems(m_om->itemIds(), 0));
    QCOMPARE(m_om->itemIds().count(), 0);
}

void tst_symbianomcollections::fetchItemInstance()
{
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
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
    QCOMPARE(m_om->items().count(), 1);
    QCOMPARE(m_om->items().at(0).collectionId(), c.id());
    QCOMPARE(m_om->itemInstances().count(), 5);
    QVERIFY(m_om->itemInstances().at(0).localId().isNull());
    QVERIFY(m_om->itemInstances().at(1).localId().isNull());
    QCOMPARE(m_om->itemInstances().at(0).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(m_om->itemInstances().at(1).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
}

void tst_symbianomcollections::modifyItemInstance()
{
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "modifyItemInstance");
    c.setMetaData("FileName", "c:modifyiteminstance");
    QVERIFY(m_om->saveCollection(&c));

    // Save a weekly recurring item
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("modifyiteminstance"),
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
    QCOMPARE(m_om->itemInstances().count(), 5);

    // Modify the second instance
    QOrganizerItem secondInstance = m_om->itemInstances().at(1);
    secondInstance.setDisplayLabel("secondinstance");
    QVERIFY(m_om->saveItem(&secondInstance));

    // Verify
    QCOMPARE(m_om->itemInstances().count(), 5);
    QCOMPARE(m_om->itemInstances().at(0).collectionId(), c.id());
    QCOMPARE(m_om->itemInstances().at(1).collectionId(), c.id());
    QCOMPARE(m_om->itemInstances().at(2).collectionId(), c.id());
    QVERIFY(m_om->itemInstances().at(0).localId().isNull());
    QVERIFY(!m_om->itemInstances().at(1).localId().isNull());
    QVERIFY(m_om->itemInstances().at(2).localId().isNull());
    QCOMPARE(m_om->itemInstances().at(0).displayLabel(), QString("modifyiteminstance"));
    QCOMPARE(m_om->itemInstances().at(1).displayLabel(), QString("secondinstance"));
    QCOMPARE(m_om->itemInstances().at(2).displayLabel(), QString("modifyiteminstance"));
}

void tst_symbianomcollections::itemsInDeletedCollection()
{
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection cannot be removed
    }
    
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData("Name", "itemsInDeletedCollection");
    c.setMetaData("FileName", "c:itemsindeletedcollection");
    QVERIFY(m_om->saveCollection(&c));

    // Save items to be used for testing
    QOrganizerItem itemSave = createItem(QOrganizerItemType::TypeEvent,
                                      QString("saveitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&itemSave, c.id().localId()));

    QOrganizerItem itemRemove = createItem(QOrganizerItemType::TypeEvent,
                                      QString("removeitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&itemRemove, c.id().localId()));

    QOrganizerItem itemFetch= createItem(QOrganizerItemType::TypeEvent,
                                      QString("fethcitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QVERIFY(m_om->saveItem(&itemFetch, c.id().localId()));

    // Remove the collection
    QVERIFY(m_om->removeCollection(c.localId()));

    // Try to modify an item in the deleted collection
    QVERIFY(!m_om->saveItem(&itemSave));

    // Try to remove item from the deleted collection
    QVERIFY(!m_om->removeItem(itemRemove.localId()));

    // Try to fetch an item from the deleted collection
    QVERIFY(m_om->item(itemFetch.localId()).isEmpty());

    // Try to fetch item instances from the deleted collection
    QVERIFY(m_om->itemInstances(itemFetch).count() == 0);
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
