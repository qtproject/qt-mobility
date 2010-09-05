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
 * For testing symbian backend via QOrganizerItemManager async API.
 */
class tst_SymbianOmAsync : public QObject
{
    Q_OBJECT

public:
    tst_SymbianOmAsync();

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

private: // util functions
    void addManagers();

private:
    QOrganizerItemManager *m_om;
};

tst_SymbianOmAsync::tst_SymbianOmAsync() :
    m_om(0)
{
    qRegisterMetaType<QOrganizerItemAbstractRequest::State>("QOrganizerItemAbstractRequest::State");
}

void tst_SymbianOmAsync::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
}

void tst_SymbianOmAsync::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
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
    QSignalSpy stateSpy(&saveItemRequest, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&saveItemRequest, SIGNAL(resultsAvailable()));

    // Save
    QVERIFY(saveItemRequest.start());
    QCOMPARE(saveItemRequest.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QCOMPARE(m_om->itemIds().count(), 1);
    QCOMPARE(saveItemRequest.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(saveItemRequest.error(), QOrganizerItemManager::NoError);
    QCOMPARE(saveItemRequest.items().count(), 1);
    QVERIFY(saveItemRequest.items().at(0).localId() != 0);
}

void tst_SymbianOmAsync::fetchSimpleItem()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeTodo);

    // Save (synchronously)
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Create fetch request
    QOrganizerItemFetchRequest fetchItemRequest;
    fetchItemRequest.setManager(m_om);

    // Create signal spys for verification purposes
    QSignalSpy stateSpy(&fetchItemRequest, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
    QSignalSpy resultSpy(&fetchItemRequest, SIGNAL(resultsAvailable()));

    // Fetch
    QVERIFY(fetchItemRequest.start());
    QCOMPARE(fetchItemRequest.state(), QOrganizerItemAbstractRequest::ActiveState);
    QCOMPARE(stateSpy.count(), 1);
    QTRY_COMPARE(resultSpy.count(), 1);

    // Verify
    QCOMPARE(fetchItemRequest.state(), QOrganizerItemAbstractRequest::FinishedState);
    QCOMPARE(fetchItemRequest.error(), QOrganizerItemManager::NoError);
    QCOMPARE(fetchItemRequest.items().count(), 1);
    QVERIFY(fetchItemRequest.items().at(0).localId() != 0);
}

void tst_SymbianOmAsync::fetchWaitForFinished()
{
     // Create item
     QOrganizerItem item;
     item.setType(QOrganizerItemType::TypeTodo);

     // Save (synchronously)
     QVERIFY(m_om->saveItem(&item));
     QVERIFY(item.id().localId() != 0);
     QVERIFY(item.id().managerUri().contains(m_om->managerName()));

     // Create fetch request
     QOrganizerItemFetchRequest fetchItemRequest;
     fetchItemRequest.setManager(m_om);

     // Create signal spys for verification purposes
     QSignalSpy stateSpy(&fetchItemRequest,
         SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)));
     QSignalSpy resultSpy(&fetchItemRequest, SIGNAL(resultsAvailable()));

     // Fetch
     QVERIFY(fetchItemRequest.start());
     QCOMPARE(fetchItemRequest.state(), QOrganizerItemAbstractRequest::ActiveState);
     QCOMPARE(stateSpy.count(), 1);
     QTime startTime = QTime::currentTime();
     QVERIFY(fetchItemRequest.waitForFinished(5000)); // 5 seconds time-out

     // Verify that the fetch did not take over 2 secons
     // Note: at the moment we don't have any performance requirements defined,
     // but a single fetch request taking more than 2 seconds would be clearly
     // too slow
     QVERIFY(startTime.secsTo(QTime::currentTime()) < 2);
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_SymbianOmAsync::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
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
