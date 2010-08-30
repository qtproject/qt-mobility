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
    QTRY_COMPARE(spyAdded.count(), itemsAddedSignals); \
    QTRY_COMPARE(spyChanged.count(), itemsChangedSignals); \
    QTRY_COMPARE(spyRemoved.count(), itemsRemovedSignals); \
    QTRY_COMPARE(spyDataChanged.count(), dataChangedSignals); \
    if (spyAdded.count()) \
        { QCOMPARE(spyAdded.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsAdded); } \
    if (spyChanged.count()) \
        { QCOMPARE(spyChanged.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsChanged); } \
    if (spyRemoved.count()) \
        { QCOMPARE(spyRemoved.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsRemoved); }

#define QTRY_COMPARE_SIGNAL_COUNTS2() \
    QTRY_COMPARE(spyAdded2.count(), itemsAddedSignals); \
    QTRY_COMPARE(spyChanged2.count(), itemsChangedSignals); \
    QTRY_COMPARE(spyRemoved2.count(), itemsRemovedSignals); \
    QTRY_COMPARE(spyDataChanged2.count(), dataChangedSignals); \
    if (spyAdded2.count()) \
        { QCOMPARE(spyAdded2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsAdded); } \
    if (spyChanged2.count()) \
        { QCOMPARE(spyChanged2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsChanged); } \
    if (spyRemoved2.count()) \
        { QCOMPARE(spyRemoved2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsRemoved); }


const QString m_managerNameSymbian("symbian");

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
    c1.setMetaData("Color", QColor(Qt::red));
    c1.setMetaData("Enabled", true);
    QVERIFY(m_om->saveCollection(&c1));
    QVERIFY(c1.id().localId());
    // TODO: custom properties

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
    // Get collections
    QList<QOrganizerCollectionLocalId> ids = m_om->collectionIds();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QVERIFY(ids.count() >= 2); // default collection + collection saved at saveCollection
        
    // Remove all
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
