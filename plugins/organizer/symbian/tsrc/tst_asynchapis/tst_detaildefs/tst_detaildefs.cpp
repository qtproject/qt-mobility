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

// Constants
const QString m_managerNameSymbian("symbian");
const int KNoOfItems = 100; 

// We need to be able to pass QOrganizerItem as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItem)

class TestDetailDefs : public QObject
{
   Q_OBJECT
   
private slots:
   void init();
   void cleanup();
   
private slots:
   void saveDetails();
   
public slots:
   void detailDefsSaveRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void detailDefsSaveRequestResultsAvailable();
   void detailDefsRemoveRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void detailDefsRemoveRequestResultsAvailable();
   void detailDefsFetchRequestStateChanged(
           QOrganizerItemAbstractRequest::State currentState);
   void detailDefsFetchRequestResultsAvailable();


private:
   void removeDetails();
   void fetchDetails();
   
private:
    QStringList                                     m_definitionNames;
    QOrganizerItemManager*                          m_om;
    QOrganizerItemDetailDefinitionSaveRequest*      m_detailDefsSaveRequest;
    QOrganizerItemDetailDefinitionRemoveRequest*    m_detailDefsRemoveRequest;
    QOrganizerItemDetailDefinitionFetchRequest*     m_detailDefsFetchRequest;

};

void TestDetailDefs::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(m_managerNameSymbian);
        
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
    
    // Create asynchronous request to save detail definitions
    m_detailDefsSaveRequest = new QOrganizerItemDetailDefinitionSaveRequest(this);
    // Connect for the state change signal
    connect(m_detailDefsSaveRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, SLOT(detailDefsSaveRequestStateChanged(
                    QOrganizerItemAbstractRequest::State)));
    connect(m_detailDefsSaveRequest, SIGNAL(resultsAvailable()), this, 
            SLOT(detailDefsSaveRequestResultsAvailable()));

    // Create asynchronous request to remvoe detail definitions
    m_detailDefsRemoveRequest = new QOrganizerItemDetailDefinitionRemoveRequest(this);
    // Connect for state change signals
    connect(m_detailDefsRemoveRequest, 
            SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, SLOT(detailDefsRemoveRequestStateChanged(
                    QOrganizerItemAbstractRequest::State)));
    connect(m_detailDefsRemoveRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(detailDefsRemoveRequestResultsAvailable()));
    
    // Create asynchronous request to remvoe detail definitions
    m_detailDefsFetchRequest = new QOrganizerItemDetailDefinitionFetchRequest(this);
    // Connect for state change signals
    connect(m_detailDefsFetchRequest, SIGNAL(
            stateChanged(QOrganizerItemAbstractRequest::State)), 
            this, SLOT(detailDefsFetchRequestStateChanged(
                    QOrganizerItemAbstractRequest::State)));
    connect(m_detailDefsFetchRequest, SIGNAL(resultsAvailable()), 
            this, SLOT(detailDefsFetchRequestResultsAvailable()));

}

void TestDetailDefs::cleanup()
{

    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}
   
void TestDetailDefs::saveDetails()
{
    QList<QOrganizerItemDetailDefinition> detailDefinitions;
    // Create a dummy detail definition to append to the list for
    // populating test data, as symbian doesn't support 
    QOrganizerItemDetailDefinition definition;
    for (int index(0); index < KNoOfItems; index++) {
        detailDefinitions.append(definition);
    }
    m_detailDefsSaveRequest->setDefinitions(detailDefinitions);
    m_detailDefsSaveRequest->setItemType(QOrganizerItemType::TypeNote);
    // Set manager
    m_detailDefsSaveRequest->setManager(m_om);
    // Start save details request
    QVERIFY(m_detailDefsSaveRequest->start());
    
    // Wait for the request to be completed
    QTest::qWait(1000);

}

void TestDetailDefs::detailDefsSaveRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            saveDetails();
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
            break;
        }
        default: {
            // Not handled
        }
    }
}

void TestDetailDefs::detailDefsSaveRequestResultsAvailable()
{
    // No definitions should be saved, so definitions() should return an 
    // empty list
    QList<QOrganizerItemDetailDefinition> savedDefinitions(
            m_detailDefsSaveRequest->definitions());
    int count(savedDefinitions.count());
    QCOMPARE(0, count);
    // Check the error map, error map should contain the number of errors
    // equal to number of definitions to be saved. Also all the erros returned
    // should be NotSupported
    QMap<int, QOrganizerItemManager::Error> errorMap(
            m_detailDefsSaveRequest->errorMap());
    count = errorMap.count();
    QCOMPARE(KNoOfItems, count);
    // Compare the errors returned
    for (int index(0); index < count; index++){
        QCOMPARE(QOrganizerItemManager::NotSupportedError, errorMap[index]);
    }
    removeDetails();
}

void TestDetailDefs::removeDetails()
    {
    QStringList stringList;
    QString string((QString)QOrganizerItemTimestamp::DefinitionName);
    for (int index(0); index < KNoOfItems; index++) {
        stringList.append(string);
    }
    m_detailDefsRemoveRequest->setDefinitionNames(stringList);
    m_detailDefsRemoveRequest->setItemType(QOrganizerItemType::TypeNote);
    m_detailDefsRemoveRequest->setManager(m_om);
    m_detailDefsRemoveRequest->start();
    }

void TestDetailDefs::detailDefsRemoveRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
            removeDetails();
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
            break;
        }
        default: {
            // Not handled
        }
    }
}

void TestDetailDefs::detailDefsRemoveRequestResultsAvailable()
{
    // Check the error map, error map should contain the number of errors
    // equal to number of definitions to be saved. Also all the erros returned
    // should be NotSupported
    QMap<int, QOrganizerItemManager::Error> errorMap(
            m_detailDefsRemoveRequest->errorMap());
    int count = errorMap.count();
    QCOMPARE(KNoOfItems, count);
    // Compare the errors returned, as remove detail defs is not supported
    for (int index(0); index < count; index++){
        QCOMPARE(QOrganizerItemManager::NotSupportedError, errorMap[index]);
    }
    fetchDetails();
}

void TestDetailDefs::fetchDetails()
{
    
    m_definitionNames.append(QOrganizerItemType::DefinitionName);
    m_definitionNames.append(QOrganizerItemGuid::DefinitionName);
    m_definitionNames.append(QOrganizerItemTimestamp::DefinitionName);
    m_definitionNames.append(QOrganizerItemDisplayLabel::DefinitionName);
    m_definitionNames.append(QOrganizerItemDescription::DefinitionName);
    // Comment - Not supported
    m_definitionNames.append(QOrganizerItemComment::DefinitionName);
    m_definitionNames.append(QOrganizerItemRecurrence::DefinitionName);
    m_definitionNames.append(QOrganizerEventTimeRange::DefinitionName);
    m_definitionNames.append(QOrganizerItemPriority::DefinitionName);
    m_definitionNames.append(QOrganizerItemLocation::DefinitionName);
    // Instance origin - Not supported
    m_definitionNames.append(QOrganizerItemInstanceOrigin::DefinitionName);
    // To-do progress - Not supported
    m_definitionNames.append(QOrganizerTodoProgress::DefinitionName);
    // To-do time range - Not supported
    m_definitionNames.append(QOrganizerTodoTimeRange::DefinitionName);
    // Journal time range - Not supported
    m_definitionNames.append(QOrganizerJournalTimeRange::DefinitionName);

    m_detailDefsFetchRequest->setDefinitionNames(m_definitionNames);
    m_detailDefsFetchRequest->setItemType(QOrganizerItemType::TypeEvent);
    m_detailDefsFetchRequest->setManager(m_om);
    m_detailDefsFetchRequest->start();
}

void TestDetailDefs::detailDefsFetchRequestStateChanged(
        QOrganizerItemAbstractRequest::State currentState)
{
    switch(currentState) {
        case QOrganizerItemAbstractRequest::InactiveState: { 
            // Operation not yet started start the operation
        fetchDetails();
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
            break;
        }
        default: {
            // Not handled
        }
    }
}

void TestDetailDefs::detailDefsFetchRequestResultsAvailable()
{
    // Get the error map
    QMap<int, QOrganizerItemManager::Error> errorMap(
            m_detailDefsFetchRequest->errorMap());
    // Get the iterator to the error map
    QMapIterator<int, QOrganizerItemManager::Error> iter(errorMap);
    // Compare the errors returned
    while (iter.hasNext()){
        // Advance the iterator to point to next item in the error map
        iter.next();
        // Compare the error value
        QCOMPARE(QOrganizerItemManager::DoesNotExistError, iter.value());
        // Get the index for the error returned
        int keyIndex(iter.key());
        
        qWarning() << m_definitionNames[keyIndex] << "is not supported";
    }
    
    // Get the definitions map
    QMap<QString, QOrganizerItemDetailDefinition> definitionsmap( 
            m_detailDefsFetchRequest->definitions());
    // Get the number of definitions supported
    int count(definitionsmap.count());
    // Get the expected count as total definitions minus error count
    int expectedItems(m_definitionNames.count() - errorMap.count());
    // Compare the expected and actual definitions supported
    QCOMPARE(expectedItems, count);
}

QTEST_MAIN(TestDetailDefs);

#include "tst_detaildefs.moc"
