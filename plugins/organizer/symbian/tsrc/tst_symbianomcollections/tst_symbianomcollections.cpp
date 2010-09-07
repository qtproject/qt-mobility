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
        
    // TODO: test all known properties
    //void collectionProperties_data();
    //void collectionProperties();

private: // util functions
    void addManagers();

private:
    QOrganizerItemManager *m_om;
};


void tst_symbianomcollections::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);
}

void tst_symbianomcollections::cleanup()
{
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
    ids = m_om->collectionIds();
    QVERIFY(!ids.contains(c.id().localId()));
    ids = om2->collectionIds();
    QVERIFY(!ids.contains(c.id().localId()));
    ids = om3->collectionIds();
    QVERIFY(!ids.contains(c.id().localId()));
        
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
    c.setMetaData("Name", "testsignalemission");
    c.setMetaData("FileName", "c:testsignalemission");
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
