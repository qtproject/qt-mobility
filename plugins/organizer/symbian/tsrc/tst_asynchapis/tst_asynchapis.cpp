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

QTM_USE_NAMESPACE


const QString m_managerNameSymbian("symbian");

// We need to be able to pass QOrganizerItem as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItem)

class TestAsychApis : public QObject
{
   Q_OBJECT
   
private slots:
   void init();
   void cleanup();
   
private slots:
   void saveItem();
   void saveItem_data();
   
public slots:
   void requestStateChanged(QOrganizerItemAbstractRequest::State currentState);
   void requestResultsAvailable();
   
private:
    QOrganizerItemManager *m_om;
    QOrganizerItem *m_organizerItem;
    QOrganizerItemSaveRequest* m_saveItemRequest;
};

void TestAsychApis::init()
{
    //QFETCH(QString, managerName);

    // Create a new item manager instance
    m_om = new QOrganizerItemManager(m_managerNameSymbian);
        
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
    
    // Create a new organizer item
    m_organizerItem = new QOrganizerItem();
    
    // Set the organizer item type
    m_organizerItem->setType(QOrganizerItemType::TypeEvent);
    
    // Set organizer item description
    m_organizerItem->setDescription("myDescription");
    
    // Set display label
    m_organizerItem->setDisplayLabel("myDisplaylabel");
            
    QOrganizerEventTimeRange timeRange;
    QDateTime startTime;
    startTime.currentDateTime();
    timeRange.setStartDateTime(startTime.currentDateTime());
    QVERIFY(m_organizerItem->saveDetail(&timeRange));

    
    // Create asynchronous request to save an item
    m_saveItemRequest = new QOrganizerItemSaveRequest(this);
    
    // Connect for the state change signal 
    connect(m_saveItemRequest, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), this, SLOT(requestStateChanged(QOrganizerItemAbstractRequest::State)));
    
    connect(m_saveItemRequest, SIGNAL(resultsAvailable()), this, SLOT(requestResultsAvailable()));
}

void TestAsychApis::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
    if (m_organizerItem) {
        delete m_organizerItem;
        m_organizerItem = 0;
    }
}
   
void TestAsychApis::saveItem()
{
    QList<QOrganizerItem> itemsList;
    itemsList.append(*m_organizerItem);
    
    // Create a new organizer item
    QOrganizerItem organizerItem;
    // Set the organizer item type
    organizerItem.setType(QOrganizerItemType::TypeEvent);
    // Set organizer item description
    organizerItem.setDescription("myDescription 1");
    // Set display label
    organizerItem.setDisplayLabel("myDisplaylabel 1");
            
    QOrganizerEventTimeRange timeRange;
    QDateTime startTime;
    startTime.currentDateTime();
    timeRange.setStartDateTime(startTime.currentDateTime());
    
    QVERIFY(organizerItem.saveDetail(&timeRange));

    //
    itemsList.append(organizerItem);
    m_saveItemRequest->setItems(itemsList);
    m_saveItemRequest->setManager(m_om);
    m_saveItemRequest->start();
    QTest::qWait(100000);
}

void TestAsychApis::saveItem_data()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // The test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // The test cases would not pass on skeleton backend

    foreach (QString managerName, managerNames) {
        
    }
    
}

void TestAsychApis::requestStateChanged(QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            saveItem();
            break;
        }
        case QOrganizerItemAbstractRequest::ActiveState: { 
            // Operation started, not yet finished operation already started
        break;
        }
        case QOrganizerItemAbstractRequest::CanceledState: { 
            // Operation is finished due to cancellation test not completed, failed
        break;
        }
        case QOrganizerItemAbstractRequest::FinishedState: { 
            // Operation either completed successfully or failed.  
            // No further results will be available.
            // test completed, compare the results
            QList<QOrganizerItem> itemsSaved = m_saveItemRequest->items();
            QVERIFY(itemsSaved.size() == 2);
            QVERIFY(itemsSaved[1].type() == QOrganizerItemType::TypeEvent);
            break;
        }
        default: {
            // Not handled
        }
    }
}

void TestAsychApis::requestResultsAvailable()
{
    // Results available
    // Verify the results
}

QTEST_MAIN(TestAsychApis);

#include "tst_asynchapis.moc"
