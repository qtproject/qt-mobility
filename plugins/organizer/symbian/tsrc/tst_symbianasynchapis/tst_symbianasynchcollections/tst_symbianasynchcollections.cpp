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

const QString managerNameSymbian("symbian");
const int KTimeToWait = 1000;
const int KNumberOfEntries = 2;

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

class tst_symbianasynchcollections : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void saveCollection();
    void fetchCollection();    
        
private slots:
    void saveItem();
    void removeCollection();
    void addCollectionSignals();
    void modifyCollectionSignals();
    void deleteCollectionSignals();
public slots:
   void requestStateChanged(QOrganizerAbstractRequest::State currentState);
   void requestResultsAvailable();
    
private:
   QList<QOrganizerItem> createItems(int noOfItems);
private:
    QOrganizerManager*                  m_om;
    QOrganizerAbstractRequest*          m_itemRequest;
    QList<QOrganizerCollectionId>           m_collectionIds;
};


void tst_symbianasynchcollections::initTestCase()
{
    m_om = new QOrganizerManager(managerNameSymbian);
    m_itemRequest = 0;
}

void tst_symbianasynchcollections::cleanupTestCase()
{
    delete m_om;
    m_om = 0;
    if(m_itemRequest) {
       delete m_itemRequest;
       m_itemRequest = 0;
    }
}

void tst_symbianasynchcollections::fetchCollection()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerCollectionFetchRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerCollectionFetchRequest * collectionFetchRequest(
        (QOrganizerCollectionFetchRequest*)m_itemRequest);

    // Start the request
    collectionFetchRequest->start();
    // Wait for KTimeToWait millisecs or until request is finished
    collectionFetchRequest->waitForFinished(KTimeToWait);
    // Verify if the request is finished
    QVERIFY(collectionFetchRequest->isFinished());
}

void tst_symbianasynchcollections::saveCollection()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerCollectionSaveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerCollectionSaveRequest * collectionSaveRequest(
        (QOrganizerCollectionSaveRequest*)m_itemRequest);

    // Set collections
    QList<QOrganizerCollection> collections;
    // Create a collection
    QOrganizerCollection collection;
    collection.setMetaData("Name", "saveCollection");
    collection.setMetaData("FileName", "c:saveCollection");
    collection.setMetaData("Description", "saveCollection test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    //collections.append(collections);
    // Set collection
    collectionSaveRequest->setCollection(collection);

    // Start the request
    collectionSaveRequest->start();
    // Wait for KTimeToWait millisecs or until request is finished
    collectionSaveRequest->waitForFinished(KTimeToWait);
    // Verify if the request is finished
    QVERIFY(collectionSaveRequest->isFinished());
}

void tst_symbianasynchcollections::removeCollection()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    // Create new request
    m_itemRequest = new QOrganizerCollectionRemoveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerCollectionRemoveRequest * collectionRemoveRequest(
        (QOrganizerCollectionRemoveRequest*)m_itemRequest);
    
    // Set collections
    collectionRemoveRequest->setCollectionIds(m_collectionIds);
    // Start the request
    collectionRemoveRequest->start();
    // Wait for KTimeToWait millisecs or until request is finished
    collectionRemoveRequest->waitForFinished(KTimeToWait);
    // Verify if the request is finished
    QVERIFY(collectionRemoveRequest->isFinished());
}

void tst_symbianasynchcollections::saveItem()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    
    // Create new request
    m_itemRequest = new QOrganizerItemSaveRequest(this);
    // Set manager
    m_itemRequest->setManager(m_om);

    // Connect for the state change signal 
    connect(m_itemRequest, 
        SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), 
        this, 
        SLOT(requestStateChanged(QOrganizerAbstractRequest::State)));
    connect(m_itemRequest, SIGNAL(resultsAvailable()), 
        this, SLOT(requestResultsAvailable()));

    // Type cast the new request to appropriate type
    QOrganizerItemSaveRequest * itemSaveRequest(
        (QOrganizerItemSaveRequest*)m_itemRequest);

    itemSaveRequest->setItems(createItems(KNumberOfEntries));
    itemSaveRequest->start();
    itemSaveRequest->waitForFinished(KTimeToWait);
}

QList<QOrganizerItem> tst_symbianasynchcollections::createItems(int noOfItems)
{
    QList<QOrganizerItem> itemsList;
    
    for (int index(0); index < noOfItems; index++) {
        // Create a new organizer item
        QOrganizerItem organizerItem;
        // Set the organizer item type
        organizerItem.setType(QOrganizerItemType::TypeEvent);
        // Create description string 
        QString description("myDescription");
        // Set organizer item description
        organizerItem.setDescription(description);
        // Create desplay label
        QString desplaylabel("myDescription");
        // Set display label
        organizerItem.setDisplayLabel(desplaylabel);
        
        // Set current time
        QOrganizerEventTime timeRange;
        QDateTime startTime;
        startTime.currentDateTime();
        timeRange.setStartDateTime(startTime.currentDateTime());
        
        QVERIFY(organizerItem.saveDetail(&timeRange));
        
        itemsList.append(organizerItem);
    }
    
    return itemsList;
}

void tst_symbianasynchcollections::requestStateChanged(
    QOrganizerAbstractRequest::State currentState)
{
    switch(currentState) {
    case QOrganizerAbstractRequest::InactiveState: {
        // Verify if the request is in inactive state
        QVERIFY(m_itemRequest->isInactive());
        // Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerAbstractRequest::InactiveState);
        // Operation not yet started start the operation
        m_itemRequest->start();
    }
    break;
    case QOrganizerAbstractRequest::ActiveState: {
        // Verify if the request is in active state
        QVERIFY(m_itemRequest->isActive());
        // Operation started, not yet finished operation already started
        // Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerAbstractRequest::ActiveState);
    }
    break;
    case QOrganizerAbstractRequest::CanceledState: {
        // Verify if the request is in canceled state
        QVERIFY(m_itemRequest->isCanceled());
        // Operation is finished due to cancellation test not completed, 
        // failed Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerAbstractRequest::CanceledState);
    }
    break;
    case QOrganizerAbstractRequest::FinishedState: {
        // Verify if the request is in finished state
        QVERIFY(m_itemRequest->isFinished());
        // Operation either completed successfully or failed.  
        // No further results will be available.
        // test completed, compare the results
        // Compare the request state is set rightly
        QCOMPARE(m_itemRequest->state(), 
            QOrganizerAbstractRequest::FinishedState);
    }
    break;
    default: {
        // Not handled
    }
    break;
    }
}

void tst_symbianasynchcollections::requestResultsAvailable()
{
    QOrganizerAbstractRequest::RequestType reqType(m_itemRequest->type());
    switch (reqType) {
    case QOrganizerAbstractRequest::CollectionFetchRequest : {
        // Check error map
        QList<QOrganizerCollection> collections(
            ((QOrganizerCollectionFetchRequest*)(m_itemRequest))->collections());
        // Error map should contain zero errors to indicate successful deletion
        // of all the items
        int count(collections.count());
        QCOMPARE(count, 2);
    }
    break;
    case QOrganizerAbstractRequest::CollectionSaveRequest : {
        QList<QOrganizerCollection> savedCollections(
            ((QOrganizerCollectionSaveRequest*)(m_itemRequest))->collections());
        int count(savedCollections.count());
        m_collectionIds.clear();
        for (int index(0); index < count; index++) {
            m_collectionIds.append(savedCollections.at(index).id());
        }
        // Check the number of requests saved. magic number to be changed to a
        // constant
        QCOMPARE(count, 1);
        
        QMap<int, QOrganizerManager::Error> errorMap(
            ((QOrganizerCollectionSaveRequest*)(m_itemRequest))->errorMap());
        // Error map should contain zero errors to indicate successful saving
        // of all the collections
        QCOMPARE(0, errorMap.count());
    }
    break;
    case QOrganizerAbstractRequest::CollectionRemoveRequest : {
        // Check error map
        QMap<int, QOrganizerManager::Error> erroMap(
            ((QOrganizerCollectionRemoveRequest*)(m_itemRequest))->errorMap());
        // Error map should contain zero errors to indicate successful deletion
        // of all the items
        int count(erroMap.count());
        QCOMPARE(0, count);
    }
    break;
    }
}

//Signal emission testcases
void tst_symbianasynchcollections::addCollectionSignals()
{        
    qRegisterMetaType<QOrganizerCollectionId>("QOrganizerCollectionId");
    qRegisterMetaType<QList<QOrganizerCollectionId> >("QList<QOrganizerCollectionId>");
    qRegisterMetaType<QOrganizerAbstractRequest::State>("QOrganizerAbstractRequest::State");
    
    // Create a second manager
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));

    // Create request
    QOrganizerCollectionSaveRequest *req = new QOrganizerCollectionSaveRequest(this);
    req->setManager(m_om);

    // Setup signal spies
    QSignalSpy stateSpy(req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(req, SIGNAL(resultsAvailable()));
    QSignalSpy addedSpy1(m_om, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));
    QSignalSpy addedSpy2(om2.data(), SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));
  
    // Create a collection
    QOrganizerCollection collection;
    collection.setMetaData("Name", "saveEmission");
    collection.setMetaData("FileName", "c:saveEmission");
    collection.setMetaData("Description", "saveEmission test");
    collection.setMetaData("Color", QColor(Qt::red));
    collection.setMetaData("Enabled", true);
    req->setCollection(collection);

    // Start the request
    QVERIFY(req->start());
    QCOMPARE(req->state(), QOrganizerAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req->collections().count(), 1);
    QVERIFY(!req->collections().at(0).id().isNull());
    QVERIFY(!req->collections().at(0).id().managerUri().isEmpty());
    // Verify the count of collectionsAdded signals on both managers
    QTRY_COMPARE(addedSpy1.count(), 1);
    QTRY_COMPARE(addedSpy2.count(), 1);
    // Verify the argument counts of the collectionsAdded signals
    QCOMPARE(addedSpy1.last().count(), 1);
    QCOMPARE(addedSpy2.last().count(), 1);
    // Verify the arguments contain the id of the new collection
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(addedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req->collections().at(0).id());
    QCOMPARE(addedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req->collections().at(0).id());
    delete req;
}

void tst_symbianasynchcollections::modifyCollectionSignals()
{
    qRegisterMetaType<QOrganizerCollectionId>("QOrganizerCollectionId");
    qRegisterMetaType<QList<QOrganizerCollectionId> >("QList<QOrganizerCollectionId>");
    qRegisterMetaType<QList<QOrganizerAbstractRequest::State> >("QOrganizerAbstractRequest::State>");
    
    // Create a second manager
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));

    // Create request
    QOrganizerCollectionSaveRequest *req = new QOrganizerCollectionSaveRequest(this);
    req->setManager(m_om);

    // Setup signal spies
    QSignalSpy stateSpy(req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(req, SIGNAL(resultsAvailable()));
    QSignalSpy changedSpy1(m_om, SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)));
    QSignalSpy changedSpy2(om2.data(), SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)));

     
    //Fetch the saved collection
    QOrganizerCollection savedCollection = m_om->collections().at(1);
    
    savedCollection.setMetaData("Name", "modifyEmission");
    req->setCollection(savedCollection);

    // Start the request
    QVERIFY(req->start());
    QCOMPARE(req->state(), QOrganizerAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QCOMPARE(req->collections().count(), 1);
    QVERIFY(!req->collections().at(0).id().isNull());
    QVERIFY(!req->collections().at(0).id().managerUri().isEmpty());
    // Verify the count of collectionsChanged signals on both managers
    QTRY_COMPARE(changedSpy1.count(), 1);
    QTRY_COMPARE(changedSpy2.count(), 1);
    // Verify the argument counts of the collectionsChanged signals
    QCOMPARE(changedSpy1.last().count(), 1);
    QCOMPARE(changedSpy2.last().count(), 1);
    // Verify the arguments contain the id of the modified collection
    QCOMPARE(changedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(changedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(changedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req->collections().at(0).id());
    QCOMPARE(changedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), req->collections().at(0).id());
    //QCOMPARE(req.collections().at(0).metaData().value("Name"),savedCollection.metaData().value("Name"));
    
    delete req;
}

void tst_symbianasynchcollections::deleteCollectionSignals()
{
    // Make sure to delete the old request, if any
    delete m_itemRequest;
    m_itemRequest =0;
    qRegisterMetaType<QOrganizerCollectionId>("QOrganizerCollectionId");
    qRegisterMetaType<QList<QOrganizerCollectionId> >("QList<QOrganizerCollectionId>");
    qRegisterMetaType<QOrganizerAbstractRequest::State>("QOrganizerAbstractRequest::State");
    
    // Create a second manager
    QScopedPointer<QOrganizerManager> om2(new QOrganizerManager(m_om->managerName()));

    // Create request
    QOrganizerCollectionRemoveRequest *deleteReq = new QOrganizerCollectionRemoveRequest(this);
    deleteReq->setManager(m_om);

    // Setup signal spies
    QSignalSpy stateSpy(deleteReq, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)));
    QSignalSpy resultSpy(deleteReq, SIGNAL(resultsAvailable()));
    QSignalSpy removedSpy1(m_om, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)));
    QSignalSpy removedSpy2(om2.data(), SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)));
     
     //Fetch the saved collection
     QOrganizerCollectionId savedCollectionId = m_om->collections().at(1).id();
     int countBeforeDeletion = m_om->collections().count();
     qWarning() << countBeforeDeletion << "calendar/s are present currently for deletion";
    
    deleteReq->setCollectionId(savedCollectionId);

    // Start the request
    QVERIFY(deleteReq->start());
    QCOMPARE(deleteReq->state(), QOrganizerAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    
    // Verify the count of collectionsRemoved signals on both managers
    QTRY_COMPARE(removedSpy1.count(), 1);
    QTRY_COMPARE(removedSpy2.count(), 1);
    // Verify the argument counts of the collectionsRemoved signals
    QCOMPARE(removedSpy1.last().count(), 1);
    QCOMPARE(removedSpy2.last().count(), 1);
    // Verify the arguments contain the id of the removed collection
    QCOMPARE(removedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(removedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().count(), 1);
    QCOMPARE(removedSpy1.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), savedCollectionId);
    QCOMPARE(removedSpy2.last().at(0).value<QList<QOrganizerCollectionId> >().at(0), savedCollectionId);
    QCOMPARE(m_om->collections().count(), 1);
    delete deleteReq;
}

QTEST_MAIN(tst_symbianasynchcollections);
#include "tst_symbianasynchcollections.moc"
