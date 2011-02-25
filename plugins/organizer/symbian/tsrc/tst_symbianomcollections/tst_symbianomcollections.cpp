/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    
Q_DECLARE_METATYPE(QList<QOrganizerItemId>)
Q_DECLARE_METATYPE(QList<QOrganizerCollectionId>)

/*!
 * For testing symbian backend via QOrganizerManager API. The target is
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

    void fetchCollection_data(){ addManagers(); };
    void fetchCollection();
    
    void saveCollection_data(){ addManagers(); };
    void saveCollection();
    
    void removeCollection_data(){ addManagers(); };
    void removeCollection();
    
    void compatibleCollection_data(){ addManagers(); };
    void compatibleCollection();

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

    void fetchItemOccurrence_data(){ addManagers(); };
    void fetchItemOccurrence();

    void modifyItemOccurrence_data(){ addManagers(); };
    void modifyItemOccurrence();

    void itemsInDeletedCollection_data(){ addManagers(); };
    void itemsInDeletedCollection();

private: // util functions
    QOrganizerItem createItem(QString itemType, QString label, QDateTime startTime, QDateTime endTime = QDateTime());
    void addManagers();

private:
    QOrganizerManager *m_om;
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
    m_om = new QOrganizerManager(managerName);
    // Remove items on all collections
    m_om->removeItems(m_om->itemIds());
    // Remove all collections (except the default)
    foreach (const QOrganizerCollection& coll, m_om->collections()) {
        if (coll != m_om->defaultCollection())
            m_om->removeCollection(coll.id());
    }
}

void tst_symbianomcollections::cleanup()
{
    m_om->removeItems(m_om->itemIds());
    // Remove all collections (except the default)
    foreach (const QOrganizerCollection& coll, m_om->collections()) {
        if (coll != m_om->defaultCollection())
            m_om->removeCollection(coll.id());
    }
    delete m_om;
    m_om = 0;
}

void tst_symbianomcollections::fetchCollection()
{
    // Fetch default collection id
    QOrganizerCollectionId dId = m_om->defaultCollection().id();
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
    QVERIFY(!dId.isNull());

    // Get all collections
    QList<QOrganizerCollection> collections = m_om->collections();
    QVERIFY(collections.count() >= 1); // default collection should always be present
    QVERIFY(collections[0].id() == dId);
    foreach (QOrganizerCollection collection, collections) {
        QVERIFY(!collection.id().isNull());
        QVERIFY(collection.id().managerUri() == m_om->managerUri());
        // dump metadata into log
        foreach (QString key, collection.metaData().keys())
            qDebug() << "\t" << key << collection.metaData().value(key);
    }

    // Get a specific collection
    QOrganizerCollection defaultCollection = m_om->collection(dId);
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(!defaultCollection.id().isNull());

    // Can we save collections? 
    if (!m_customCollectionsSupported)
        return;

    // add a collection and fetch
    QOrganizerCollection c1;
    c1.setMetaData(QOrganizerCollection::KeyName, "fetchCollection");
    QVERIFY(m_om->saveCollection(&c1));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(m_om->collections().count(), 2);

    // fetch an already removed collection
    QVERIFY(m_om->removeCollection(c1.id()));
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QCOMPARE(m_om->collections().count(), 1);
    QVERIFY(m_om->collection(c1.id()) == QOrganizerCollection());
    QCOMPARE(m_om->error(), QOrganizerManager::DoesNotExistError);
}

void tst_symbianomcollections::saveCollection()
{
    // Can we save collections? 
    if (!m_customCollectionsSupported) {
        QWARN("Saving a collection not supported!");
        // Verify it fails with correct error
        QOrganizerCollection c1;
        QVERIFY(!m_om->saveCollection(&c1));
        QVERIFY(m_om->error() == QOrganizerManager::NotSupportedError);        
        return;
    }
    
    // Save a collection
    QOrganizerCollection c1;
    c1.setMetaData(QOrganizerCollection::KeyName, "testsave");
    QDateTime beforeSave = QDateTime::currentDateTime();
    QVERIFY(m_om->saveCollection(&c1));
    QDateTime afterSave = QDateTime::currentDateTime();
    QVERIFY(!c1.id().isNull());

    // Verify filename (generated from name)
    QVERIFY(c1.metaData(QOrganizerCollection::KeyName).toString().contains("testsave"));

    // Verify timestamps
    QVERIFY(c1.metaData("CreationTime").toDateTime() >= beforeSave);
    QVERIFY(c1.metaData("CreationTime").toDateTime() <= beforeSave);
    QVERIFY(c1.metaData("CreationTime").toDateTime() == c1.metaData("ModificationTime").toDateTime());
    QDateTime creationTime = c1.metaData("CreationTime").toDateTime();

    // Verify saved collection matches to fetched collection
    QList<QOrganizerCollection> cs;
    cs << m_om->collection(c1.id());
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
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
    c1.setMetaData(QOrganizerCollection::KeyName, "testsave modified");
    c1.setMetaData(QOrganizerCollection::KeyDescription, "some description");
    c1.setMetaData(QOrganizerCollection::KeyColor, QColor(Qt::green));
    c1.setMetaData(QOrganizerCollection::KeyImage, "someimage.jpg"); // saved as a custom property
    c1.setMetaData("IsValid", false); // cannot be controlled by client
    c1.setMetaData("Enabled", false);
    c1.setMetaData("FolderLUID", (uint) 0x12345);
    c1.setMetaData("CreationTime", QDateTime::currentDateTime().addYears(1)); // cannot be controlled by client 
    c1.setMetaData("ModificationTime", QDateTime::currentDateTime().addYears(1)); // cannot be controlled by client
    c1.setMetaData("SyncStatus", true);
    c1.setMetaData("IsSharedFolder", true);
    c1.setMetaData("GlobalUUID", "1234567890");
    c1.setMetaData("DeviceSyncServiceOwner", (uint) 0x12345);
    c1.setMetaData("OwnerName", "owner");
    c1.setMetaData("MarkAsDelete", false); // this should be hidden from client
    c1.setMetaData("customkey", "foobar");
    beforeSave = QDateTime::currentDateTime();
    QVERIFY(m_om->saveCollection(&c1));
    afterSave = QDateTime::currentDateTime();

    // Verify timestamps (again)
    QVERIFY(c1.metaData("CreationTime").toDateTime() == creationTime);
    QVERIFY(c1.metaData("ModificationTime").toDateTime() >= beforeSave);
    QVERIFY(c1.metaData("ModificationTime").toDateTime() <= afterSave);

    // Client cannot control these directly
    QVERIFY(c1.metaData("IsValid").toBool() == true);
    QVERIFY(!c1.metaData().contains("MarkAsDelete"));

    // Verify data saved as custom properties
    QVERIFY(c1.metaData(QOrganizerCollection::KeyImage).toString() == "someimage.jpg");
    QVERIFY(c1.metaData("customkey").toString() == "foobar");

    // Verify saved collection matches to fetched collection (again)
    cs.clear();
    cs << m_om->collection(c1.id());
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
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
    c2.setMetaData("FileName", "c:testsavemodified");
    QVERIFY(!m_om->saveCollection(&c2));
    
    // Try saving with unknown manager uri. Should fail. -- cannot do this any more.
    //c2 = c1;
    //QOrganizerCollectionId id = c2.id();
    //c2.setId(id);
    //QVERIFY(!m_om->saveCollection(&c2));
    
    // Remove the collection
    QVERIFY(m_om->removeCollection(c1.id()));
    
    // Try saving again without clearing local id. Should fail.
    QVERIFY(!m_om->saveCollection(&c1));
    
    // Try saving without any parameters. Should pass.
    c1 = QOrganizerCollection();
    QVERIFY(m_om->saveCollection(&c1));
    
    // Try saving with filename only. Should pass.
    c1 = QOrganizerCollection();
    c1.setMetaData("FileName", "c:testsave2");
    QVERIFY(m_om->saveCollection(&c1));
}

void tst_symbianomcollections::removeCollection()
{
    // Do we support removing collections?
    if (!m_customCollectionsSupported) {    
        QWARN("Removing a collection not supported!");
        // Verify it fails with correct error code
        QVERIFY(!m_om->removeCollection(m_om->defaultCollection().id()));
        QVERIFY(m_om->error() == QOrganizerManager::NotSupportedError);
        return;
    }
    
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData(QOrganizerCollection::KeyName, "testremove");
    QVERIFY(m_om->saveCollection(&c));
    
    // Remove it
    // Backend should be able to remove the calendar file itself
    QVERIFY(m_om->removeCollection(c.id()));
    
    // Save again
    c.setId(QOrganizerCollectionId());
    QVERIFY(m_om->saveCollection(&c));
    
    // Create a second manager which will open the same collection
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));
    QList<QOrganizerCollection> colls = om2->collections();
    QVERIFY(colls.contains(c)); // we assume that collections local id is global between managers
    
    // Remove the collection again
    // Now the backend cannot remove the calendar file because om2 has it open.
    // It must set the MarkAsDelete flag instead. 
    QVERIFY(m_om->removeCollection(c.id()));

    // Create a third manager
    QScopedPointer<QOrganizerManager> om3(new QOrganizerManager(m_om->managerName()));

    // Collection should not appear in any manager
    QTRY_COMPARE(m_om->collections().count(), 1);
    QTRY_COMPARE(om2->collections().count(), 1);
    QTRY_COMPARE(om3->collections().count(), 1);

    // Save yet again. Backend should remove the MarkAsDelete flag from the calendar.
    c.setId(QOrganizerCollectionId());
    QVERIFY(m_om->saveCollection(&c));
    
    // Free collections for removing
    delete om2.take();
    delete om3.take();
    
    // Remove all collections
    colls = m_om->collections();
    QVERIFY(m_om->error() == QOrganizerManager::NoError);
    QVERIFY(colls.count() >= 2); // default collection + collection saved
    foreach (const QOrganizerCollection& coll, colls) {
        if (coll == m_om->defaultCollection())
            QVERIFY(!m_om->removeCollection(coll.id())); // removing default collection not allowed
        else
            QVERIFY(m_om->removeCollection(coll.id()));
    }
    
    // Try removing the same ones again 
    foreach (const QOrganizerCollection& coll, colls) {
        QVERIFY(!m_om->removeCollection(coll.id()));
    }
}

void tst_symbianomcollections::compatibleCollection()
{
    if (!m_customCollectionsSupported)
        QSKIP("Collection signals not supported!", SkipSingle);

    // Try creating a compatible collection from an empty collection
    QOrganizerCollection collection;
    QOrganizerCollection compatibleCollection = m_om->compatibleCollection(collection);
    QCOMPARE(m_om->error(), QOrganizerManager::NoError);
    QVERIFY(m_om->saveCollection(&compatibleCollection));
    QVERIFY(!compatibleCollection.id().isNull());

    // Try creating a compatible collection from a legal collection and verify it
    // is the same as the original
    collection.setMetaData(QOrganizerCollection::KeyName, "testcompatible");
    compatibleCollection = m_om->compatibleCollection(collection);
    QCOMPARE(compatibleCollection, collection);
}

void tst_symbianomcollections::collectionSignalEmission()
{
    // Check we can test this
    if (!m_customCollectionsSupported)
        QSKIP("Collection signals not supported!", SkipSingle);
    
    // Create a second manager
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));
    
    // Setup signal spies
    qRegisterMetaType<QList<QOrganizerCollectionId> >("QList<QOrganizerCollectionId>");
    QSignalSpy addedSpy1(m_om, SIGNAL(collectionsAdded(const QList<QOrganizerCollectionId>&)));
    QSignalSpy addedSpy2(om2.data(), SIGNAL(collectionsAdded(const QList<QOrganizerCollectionId>&)));
    QSignalSpy changedSpy1(m_om, SIGNAL(collectionsChanged(const QList<QOrganizerCollectionId>&)));
    QSignalSpy changedSpy2(om2.data(), SIGNAL(collectionsChanged(const QList<QOrganizerCollectionId>&)));
    QSignalSpy removedSpy1(m_om, SIGNAL(collectionsRemoved(const QList<QOrganizerCollectionId>&)));
    QSignalSpy removedSpy2(om2.data(), SIGNAL(collectionsRemoved(const QList<QOrganizerCollectionId>&)));
    int addedCount = 0;
    int changedCount = 0;
    int removedCount = 0;

    // save collection
    QOrganizerCollection c;
    c.setMetaData(QOrganizerCollection::KeyName, "testcollectionsignalemission");
    QVERIFY(m_om->saveCollection(&c));
    addedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QVERIFY(addedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().contains(c.id()));
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QVERIFY(addedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().contains(c.id()));
    
    // Modify collection
    c.setMetaData(QOrganizerCollection::KeyName, "testsignalemissionmodified");
    QVERIFY(m_om->saveCollection(&c));
    changedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Remove collection
    // This will not actually remove the calendar file. It will only be marked for deletion.
    // This is because the calendar file is open at om2 (and possibly native symbian calendar).
    QVERIFY(m_om->removeCollection(c.id()));
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
    QVERIFY(m_om->removeCollection(c.id()));
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
    c.setMetaData(QOrganizerCollection::KeyName, "testitemsignalemission");
    QVERIFY(m_om->saveCollection(&c));
    
    // Create a second manager
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));
    
    // Setup signal spies
    qRegisterMetaType<QList<QOrganizerItemId> >("QList<QOrganizerItemId>");
    QSignalSpy addedSpy1(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemId>)));
    QSignalSpy addedSpy2(om2.data(), SIGNAL(itemsAdded(QList<QOrganizerItemId>)));
    QSignalSpy changedSpy1(m_om, SIGNAL(itemsChanged(QList<QOrganizerItemId>)));
    QSignalSpy changedSpy2(om2.data(), SIGNAL(itemsChanged(QList<QOrganizerItemId>)));
    QSignalSpy removedSpy1(m_om, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)));
    QSignalSpy removedSpy2(om2.data(), SIGNAL(itemsRemoved(QList<QOrganizerItemId>)));
    int addedCount = 0;
    int changedCount = 0;
    int removedCount = 0;
    
    // Save item
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                     QString("testitemsignalemission"),
                                     QDateTime::currentDateTime().addMSecs(3600));
    item.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&item));
    addedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Change item
    item.setDisplayLabel("testitemsignalemission changed");
    QVERIFY(m_om->saveItem(&item));
    changedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Remove item
    QVERIFY(m_om->removeItem(item.id()));
    removedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    
    // Save items - batch api
    QList<QOrganizerItem> items;
    QOrganizerItem listItem1, listItem2;
    listItem1 = createItem(QOrganizerItemType::TypeEvent,
                        QString("testitemsignalemission1"),
                        QDateTime::currentDateTime().addMSecs(3600));
    listItem2 = createItem(QOrganizerItemType::TypeEvent,
                        QString("testitemsignalemission2"),
                        QDateTime::currentDateTime().addMSecs(3600));
    listItem1.setCollectionId(c.id());
    listItem2.setCollectionId(c.id());
    items << listItem1 << listItem2;
    QVERIFY(m_om->saveItems(&items));
    addedCount++; // There should be one signal from both managers and the list should contain 2 items
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerItemId> >().count(), 2);
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerItemId> >().count(), 2);
        
    // Change items - batch api
    items[0].setDisplayLabel("testitemsignalemission1 changed");
    items[1].setDisplayLabel("testitemsignalemission2 changed");
    QVERIFY(m_om->saveItems(&items));
    changedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(changedSpy1.last().at(0).value<QList<QOrganizerItemId> >().count(), 2);
    QCOMPARE(changedSpy2.last().at(0).value<QList<QOrganizerItemId> >().count(), 2);
    
    // Remove items - batch api
    QList<QOrganizerItemId> ids;
    ids << items[0].id() << items[1].id();
    QVERIFY(m_om->removeItems(ids));
    removedCount++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QCOMPARE(removedSpy1.last().at(0).value<QList<QOrganizerItemId> >().count(), 2);
    QCOMPARE(removedSpy2.last().at(0).value<QList<QOrganizerItemId> >().count(), 2);
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
    c.setMetaData(QOrganizerCollection::KeyName, "addItem");
    QVERIFY(m_om->saveCollection(&c));

    // Save similar item to both of the collections
    QOrganizerItem item1 = createItem(QOrganizerItemType::TypeEvent,
                                      QString("additem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    QOrganizerItem item2 = item1;
    QVERIFY(m_om->saveItem(&item1));
    item2.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&item2)); // creates a new item in a different collection.

    // Verify
    QVERIFY(item1.id() != item2.id());
    QCOMPARE(item1.collectionId(), m_om->defaultCollection().id());
    QCOMPARE(item2.collectionId(), c.id());
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
    c.setMetaData(QOrganizerCollection::KeyName, "fetchItem");
    QVERIFY(m_om->saveCollection(&c));

    // Save an item to the newly created collection
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("fetchitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    item.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&item));

    // Fetch and verify the item
    // Note: collections need to be empty before executing the test case
    QCOMPARE(m_om->itemIds().count(), 1);
    QCOMPARE(m_om->item(item.id()).collectionId(), c.id());
    QCOMPARE(m_om->item(item.id()).displayLabel(), QString("fetchitem"));
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
    c.setMetaData(QOrganizerCollection::KeyName, "modifyItem");
    QVERIFY(m_om->saveCollection(&c));

    // Save similar item to both of the collections
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("additem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    item.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&item));

    // Modify and save again
    item.setDisplayLabel("modifyitem");
    QVERIFY(m_om->saveItem(&item));

    // Verify
    QCOMPARE(m_om->item(item.id()).displayLabel(), QString("modifyitem"));
    QCOMPARE(m_om->item(item.id()).collectionId(), c.id());
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
    c.setMetaData(QOrganizerCollection::KeyName, "removeItem");
    QVERIFY(m_om->saveCollection(&c));

    // Save an item to the newly created collection
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("removeitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    item.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&item));

    // Remove item
    QOrganizerItemId id = item.id();
    QVERIFY(m_om->removeItem(id));

    // Verify
    QVERIFY(m_om->item(id).isEmpty());
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
    c.setMetaData(QOrganizerCollection::KeyName, "fetchItems");
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
        item2.setCollectionId(c.id());
        QVERIFY(m_om->saveItem(&item2));
    }

    // Verify
    QCOMPARE(m_om->itemIds().count(), itemCount * 2);
    QCOMPARE(m_om->itemsForExport().count(), itemCount * 2);

    // Verify there are no duplicates in the list
    bool noduplicates(true);
    foreach(QOrganizerItemId id, m_om->itemIds()) {
        if (m_om->itemIds().count(id) > 1) {
            qWarning() << "duplicate id: " << id;
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
    c1.setMetaData(QOrganizerCollection::KeyName, "removeItems1");
    QVERIFY(m_om->saveCollection(&c1));
    QOrganizerCollection c2;
    c2.setMetaData(QOrganizerCollection::KeyName, "removeItems2");
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
        item1.setCollectionId(c1.id());
        QVERIFY(m_om->saveItem(&item1));
        item2.setCollectionId(c2.id());
        QVERIFY(m_om->saveItem(&item2));
    }

    // Verify
    QCOMPARE(m_om->itemIds().count(), itemCount * 3);

    // Filter for the first new collection
    QOrganizerItemCollectionFilter filter1;
    QSet<QOrganizerCollectionId> collectionIds1;
    collectionIds1.insert(c1.id());
    filter1.setCollectionIds(collectionIds1);

    // Filter for the second new collection
    QOrganizerItemCollectionFilter filter2;
    QSet<QOrganizerCollectionId> collectionIds2;
    collectionIds2.insert(c2.id());
    filter2.setCollectionIds(collectionIds2);

    // Remove from the first new collection and verify
    QVERIFY(m_om->removeItems(m_om->itemIds(filter1)));
    QCOMPARE(m_om->itemIds(filter1).count(), 0);
    QCOMPARE(m_om->itemIds(filter2).count(), itemCount);
    QCOMPARE(m_om->itemIds().count(), itemCount * 2);

    // Remove from the second new collection and verify
    QVERIFY(m_om->removeItems(m_om->itemIds(filter2)));
    QCOMPARE(m_om->itemIds(filter1).count(), 0);
    QCOMPARE(m_om->itemIds(filter2).count(), 0);
    QCOMPARE(m_om->itemIds().count(), itemCount);

    // Remove from all collections and verify
    QVERIFY(m_om->removeItems(m_om->itemIds()));
    QCOMPARE(m_om->itemIds().count(), 0);
}

void tst_symbianomcollections::fetchItemOccurrence()
{
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData(QOrganizerCollection::KeyName, "fetchItemOccurrence");
    QVERIFY(m_om->saveCollection(&c));

    // Save a weekly recurring item
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("fetchitemoccurrence"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    item.setCollectionId(c.id());
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(5);
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));

    // Verify
    QCOMPARE(m_om->itemsForExport().count(), 1);
    QCOMPARE(m_om->itemsForExport().at(0).collectionId(), c.id());
    QCOMPARE(m_om->items().count(), 5);
    QVERIFY(m_om->items().at(0).id().isNull());
    QVERIFY(m_om->items().at(1).id().isNull());
    QCOMPARE(m_om->items().at(0).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
    QCOMPARE(m_om->items().at(1).type(), QLatin1String(QOrganizerItemType::TypeEventOccurrence));
}

void tst_symbianomcollections::modifyItemOccurrence()
{
    // Can we test this?
    if (!m_customCollectionsSupported) {
        QSKIP("Only default collection supported", SkipSingle);
        // NOTE: Default collection already tested at tst_symbianom
    }
    
    // Save a collection
    QOrganizerCollection c;
    c.setMetaData(QOrganizerCollection::KeyName, "modifyItemOccurrence");
    QVERIFY(m_om->saveCollection(&c));

    // Save a weekly recurring item
    QOrganizerItem item = createItem(QOrganizerItemType::TypeEvent,
                                      QString("modifyitemoccurrence"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    item.setCollectionId(c.id());
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    rrule.setLimit(5);
    QSet<QOrganizerRecurrenceRule> rrules;
    rrules.insert(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->items().count(), 5);

    // Modify the second instance
    QOrganizerItem secondInstance = m_om->items().at(1);
    secondInstance.setDisplayLabel("secondoccurrence");
    QVERIFY(m_om->saveItem(&secondInstance));

    // Verify
    QCOMPARE(m_om->items().count(), 5);
    QCOMPARE(m_om->items().at(0).collectionId(), c.id());
    QCOMPARE(m_om->items().at(1).collectionId(), c.id());
    QCOMPARE(m_om->items().at(2).collectionId(), c.id());
    QVERIFY(m_om->items().at(0).id().isNull());
    QVERIFY(!m_om->items().at(1).id().isNull());
    QVERIFY(m_om->items().at(2).id().isNull());
    QCOMPARE(m_om->items().at(0).displayLabel(), QString("modifyitemoccurrence"));
    QCOMPARE(m_om->items().at(1).displayLabel(), QString("secondoccurrence"));
    QCOMPARE(m_om->items().at(2).displayLabel(), QString("modifyitemoccurrence"));
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
    c.setMetaData(QOrganizerCollection::KeyName, "itemsInDeletedCollection");
    QVERIFY(m_om->saveCollection(&c));

    // Save items to be used for testing
    QOrganizerItem itemSave = createItem(QOrganizerItemType::TypeEvent,
                                      QString("saveitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    itemSave.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&itemSave));

    QOrganizerItem itemRemove = createItem(QOrganizerItemType::TypeEvent,
                                      QString("removeitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    itemRemove.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&itemRemove));

    QOrganizerItem itemFetch= createItem(QOrganizerItemType::TypeEvent,
                                      QString("fethcitem"),
                                      QDateTime::currentDateTime().addMSecs(3600));
    itemFetch.setCollectionId(c.id());
    QVERIFY(m_om->saveItem(&itemFetch));

    // Remove the collection
    QVERIFY(m_om->removeCollection(c.id()));

    // Try to modify an item in the deleted collection
    QVERIFY(!m_om->saveItem(&itemSave));

    // Try to remove item from the deleted collection
    QVERIFY(!m_om->removeItem(itemRemove.id()));

    // Try to fetch an item from the deleted collection
    QVERIFY(m_om->item(itemFetch.id()).isEmpty());

    // Try to fetch item instances from the deleted collection
    QOrganizerItemCollectionFilter fil;
    QSet<QOrganizerCollectionId> filterCollectionIds;
    filterCollectionIds.insert(c.id());
    fil.setCollectionIds(filterCollectionIds);
    QVERIFY(m_om->items(fil).count() == 0);
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
 * Helper method for executing test cases with all the available managers
 */
void tst_symbianomcollections::addManagers()
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
QTEST_MAIN(tst_symbianomcollections);
#include "tst_symbianomcollections.moc"
