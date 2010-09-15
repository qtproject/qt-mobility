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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QObject>
#include <qmobilityglobal.h>
#include <qnamespace.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

// Constants
const QString managerNameSymbian("symbian");
const int KNumberOfEntries = 5; 

// We need to be able to pass QOrganizerItem as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItem)

class TestItemIds : public QObject
{
   Q_OBJECT
   
private slots:
   void init();
   void cleanup();
   
private slots:
   void saveItems(); // Start the test case by saving items into Agenda DB
   // fetchItems would be called as and when the results for saveItems are
   // available
   
public slots:
   void fetchRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void fetchRequestResultsAvailable();
   void saveRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void saveRequestResultsAvailable();
   void removeRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void removeRequestResultsAvailable();
   void localIdFetchRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void localIdFetchRequestResultsAvailable();
   
private:
   QList<QOrganizerItem> createItems(int noOfItems);
   void removeItems(QList<QOrganizerItemLocalId>& itemsIds);
   void fetchlocalIds();
   void fetchItems();
   
private:
   QList<QOrganizerItemLocalId>        m_itemIds;
   QOrganizerItemManager*              m_om;
   QOrganizerItemFetchRequest*         m_fetchItemRequest;
   QOrganizerItemSaveRequest*          m_saveItemRequest;
   QOrganizerItemRemoveRequest*        m_removeItemRequest;
   QOrganizerItemLocalIdFetchRequest*  m_localIdfetchRequest;
};

void TestItemIds::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerNameSymbian);
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
    
    // Create asynchronous request to save an item
    m_saveItemRequest = new QOrganizerItemSaveRequest(this);
    // Connect for the state change signal 
    connect(m_saveItemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), this, 
            SLOT(saveRequestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_saveItemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(saveRequestResultsAvailable()));

    // Create asynchronous request to fetch item/s
    m_fetchItemRequest = new QOrganizerItemFetchRequest(this);
    // Connect for the state change signal 
    connect(m_fetchItemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, 
            SLOT(fetchRequestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_fetchItemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(fetchRequestResultsAvailable()));
    
    // Create asynchronous request to remove items
    m_removeItemRequest = new QOrganizerItemRemoveRequest(this);
    // Connect for the state change signals
    connect(m_removeItemRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
             this, 
             SLOT(removeRequestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_removeItemRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(removeRequestResultsAvailable()));
    
    // Create asynchronous request to fetchlocalid/s
    m_localIdfetchRequest = new QOrganizerItemLocalIdFetchRequest();
    // Connect for the state change signals
    connect(m_localIdfetchRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
              this, 
              SLOT(localIdFetchRequestStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(m_localIdfetchRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(localIdFetchRequestResultsAvailable()));

}

void TestItemIds::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestItemIds::saveItems()
{
    // Set items to be saved
    m_saveItemRequest->setItems(createItems(KNumberOfEntries));
    // Set item manager
    m_saveItemRequest->setManager(m_om);
    // Start save request
    m_saveItemRequest->start();
    // Wait for the request to be completed
    // Also wait for the fetch test to complete
    QTest::qWait(10000);
}
   
// Returns a list of noOfItems
QList<QOrganizerItem> TestItemIds::createItems(int noOfItems)
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
        QOrganizerEventTimeRange timeRange;
        QDateTime startTime;
        startTime.currentDateTime();
        timeRange.setStartDateTime(startTime.currentDateTime());
        
        QVERIFY(organizerItem.saveDetail(&timeRange));
        
        itemsList.append(organizerItem);
    }
    return itemsList;
}

// request status changed for save request
void TestItemIds::saveRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            saveItems();
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, 
            // failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compare the results when results available 
            break;
        }
        default: {
            // Not handled
        }
    }
}

// Save request completed/results available
void TestItemIds::saveRequestResultsAvailable()
{
    QList<QOrganizerItem> items = m_saveItemRequest->items();
    // compare the number of items saved
    int count(items.count());
    QCOMPARE(KNumberOfEntries, count);
    // Fetch all the items ids
    fetchlocalIds();
}

void TestItemIds::fetchlocalIds()
{
    // Set manager
    m_localIdfetchRequest->setManager(m_om);
    // Start remove/delete request
    m_localIdfetchRequest->start();    
}

// request status changed for fetchLocalids request
void TestItemIds::localIdFetchRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            fetchlocalIds();
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, 
            // failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compare the results when results available 
            break;
        }
        default: {
            // Not handled
        }
    }
}

// localIdFetch request completed/results available
void TestItemIds::localIdFetchRequestResultsAvailable()
{
    m_itemIds.clear();
    m_itemIds.append(m_localIdfetchRequest->itemIds());
    // Compare the number of ids retrieved
    int count(m_itemIds.count());
    QCOMPARE(KNumberOfEntries, count);
    // Remove all items present with local id in itemIds
    // Start fetchItems test now
    removeItems(m_itemIds);
}

void TestItemIds::removeItems(QList<QOrganizerItemLocalId>& itemsIds)
{
    // Set items Ids to be deleted
    m_removeItemRequest->setItemIds(itemsIds);
    // Set manager
    m_removeItemRequest->setManager(m_om);
    // Start remove/delete request
    m_removeItemRequest->start();
}

// Remove request state changed
void TestItemIds::removeRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            removeItems(m_itemIds);
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, 
            // failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compare the results while available
        }
        default: {
            // Not handled
        }
    }
}

// Remove request available compare results
void TestItemIds::removeRequestResultsAvailable()
{
    // Check error map
    QMap<int, QOrganizerItemManager::Error> erroMap;
    erroMap = m_removeItemRequest->errorMap();
    // Check the number of errors encoutered in remove operation
    TInt count(erroMap.count());
    // Error count should be 0 if all the items are deleted successfully
    QCOMPARE(0, count);
    // Fetch db once again to cross check if the m_itemIds are still present in 
    // db
    fetchItems();
}

void TestItemIds::fetchItems()
{
    // Set items local Id filter
    QOrganizerItemLocalIdFilter localIdFilter;
    localIdFilter.setIds(m_itemIds);
    m_fetchItemRequest->setFilter(localIdFilter);
    
    // Set ItemDetailsFilter
    QOrganizerItemDetailFilter detailsFilter;
    detailsFilter.setDetailDefinitionName(
            QOrganizerItemDisplayLabel::DefinitionName, 
            QOrganizerItemDisplayLabel::FieldLabel); 
    detailsFilter.setValue("myDescription");
    detailsFilter.setMatchFlags(QOrganizerItemFilter::MatchContains);
    //m_fetchItemRequest->setFilter(detailsFilter);

    // Set manager
    m_fetchItemRequest->setManager(m_om);
    // Start the request
    m_fetchItemRequest->start();
}

// Fetch request state changed
void TestItemIds::fetchRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            fetchItems();
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, 
            // failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compare the results while available
        }
        default: {
            // Not handled
        }
    }
}

// Fetch request available compare results
void TestItemIds::fetchRequestResultsAvailable()
{
    QList<QOrganizerItem> items = m_fetchItemRequest->items();
    int count(items.count());
    // No entry should be found with the given ids
    QCOMPARE(0, count);
}

QTEST_MAIN(TestItemIds);

#include "tst_symbianasynchitemids.moc"
