/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

//TESTED_COMPONENT=src/location

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

#include <qlandmark.h>
#include <qlandmarkid.h>
#include <qlandmarkcategory.h>
#include <qlandmarkcategoryid.h>
#include <qlandmarkmanager.h>
#include <qgeocoordinate.h>

#include <qlandmarkfilter.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkproximityfilter.h>
#include <qlandmarkcategoryfilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkintersectionfilter.h>
#include <qlandmarkunionfilter.h>
#include <qlandmarkidfilter.h>

#include <qlandmarksortorder.h>
#include <qlandmarknamesort.h>
#include <qlandmarkdistancesort.h>
#include <qlandmarkabstractrequest.h>
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarkcategoryidfetchrequest.h>
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategorysaverequest.h>
#include <qlandmarkcategoryremoverequest.h>
#include <qlandmarkimportrequest.h>
#include <qlandmarkexportrequest.h>
#include <QMetaType>
#include <QDebug>

#include <QFile>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <float.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QLandmarkCategoryId>);
Q_DECLARE_METATYPE(QList<QLandmarkId>);
Q_DECLARE_METATYPE(QList<QLandmark>);
Q_DECLARE_METATYPE(QList<QLandmarkCategory>);
Q_DECLARE_METATYPE(QLandmarkAbstractRequest::State)
Q_DECLARE_METATYPE(QLandmarkAbstractRequest *)
Q_DECLARE_METATYPE(QLandmarkFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkManager::Error)
Q_DECLARE_METATYPE(QLandmarkSaveRequest *)
Q_DECLARE_METATYPE(QLandmarkRemoveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategorySaveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryRemoveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryFetchRequest *)

class ManagerListener : public QObject
{
    Q_OBJECT
public slots:
    void landmarksAdded(const QList<QLandmarkId>) {}
    void landmarksChanged(const QList<QLandmarkId>){}
    void landmarksRemoved(const QList<QLandmarkId>){}
    void categoriesAdded(const QList<QLandmarkCategoryId>){}
    void categoriesChanged(const QList<QLandmarkCategoryId>){}
    void categoriesRemoved(const QList<QLandmarkCategoryId>){}
};

class tst_QLandmarkManagerEngineSqlite : public QObject
{
    Q_OBJECT
public:
    tst_QLandmarkManagerEngineSqlite() {
    qRegisterMetaType<QList<QLandmarkCategoryId> >();
    qRegisterMetaType<QList<QLandmarkId> >();
    qRegisterMetaType<QList<QLandmark> >();
    qRegisterMetaType<QList<QLandmarkCategory> >();
    qRegisterMetaType<QLandmarkAbstractRequest::State>();
    qRegisterMetaType<QLandmarkAbstractRequest *>();
    qRegisterMetaType<QLandmarkFetchRequest *>();
    qRegisterMetaType<QLandmarkSaveRequest *>();
    qRegisterMetaType<QLandmarkRemoveRequest *>();
    qRegisterMetaType<QLandmarkCategoryFetchRequest *>();
    qRegisterMetaType<QLandmarkCategorySaveRequest *>();
    qRegisterMetaType<QLandmarkCategoryRemoveRequest *>();
    qRegisterMetaType<QLandmarkManager::Error>();
    }

private:
    QLandmarkManager *m_manager;
    ManagerListener *m_listener;

    bool waitForAsync(QSignalSpy &spy, QLandmarkAbstractRequest *request,
                    QLandmarkManager::Error error = QLandmarkManager::NoError,
                    int ms=1500, QLandmarkAbstractRequest::State state = QLandmarkAbstractRequest::FinishedState) {
        bool ret = true;
        QTest::qWait(ms);
        if (spy.count() != 2) {
            qWarning() << "Spy count mismatch, expected = " << 2 << ", actual = " << spy.count();
            ret = false;
        }
        spy.clear();

        if (request->error() != error) {
            qWarning() << "Error mismatch, expected = " << error
                                       << ", actual=" << request->error();
            qWarning() << "Error string =" << request->errorString();
            ret = false;
        }

        if (error == QLandmarkManager::NoError && !request->errorString().isEmpty()) {
            qWarning() << "Error string is not empty as expected, error string = " << request->errorString();
            ret = false;
        }

        if (request->state() != state) {
            qWarning() << "Request State mismatch, expected = " << state
                                               << ", actual =" << request->state();
            ret = false;
        }
        return ret;
    }

    bool checkIds(const QList<QLandmark> &lms, const QList<QLandmarkId> &lmIds)
    {
        if (lms.count() != lmIds.count())
            return false;
        for (int i=0; i < lms.count(); ++i) {
            if (lms.at(i).landmarkId() != lmIds.at(i)) {
                return false;
            }
        }
    }

    //checks if an id fetch request will return the ids for the same landmarks as in \a lms
    bool checkIdFetchRequest(const QList<QLandmark> &lms, const QLandmarkFilter &filter,
                            const QList<QLandmarkSortOrder> &sorting = QList<QLandmarkSortOrder>(),
                            const QLandmarkFetchHint &fetchHint = QLandmarkFetchHint()) {
        //check that the id request will return the same results
        QLandmarkIdFetchRequest idFetchRequest(m_manager);
        idFetchRequest.setFilter(filter);
        idFetchRequest.setSorting(sorting);
        idFetchRequest.setFetchHint(fetchHint);

        QSignalSpy spyId(&idFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
        idFetchRequest.start();

        if (!waitForAsync(spyId, &idFetchRequest))
            return false;
        QList<QLandmarkId> lmIds = idFetchRequest.landmarkIds();
        if (lmIds.count() != lms.count())
            return false;
        for (int i=0; i < lmIds.count(); ++i) {
            if( !(lms.at(i).landmarkId() == lmIds.at(i)))
                return false;
        }

        return true;
    }

    bool checkIdFetchRequest(const QList<QLandmark> &lms, const QLandmarkFilter &filter,
                            const QLandmarkSortOrder &sorting,
                            const QLandmarkFetchHint &fetchHint = QLandmarkFetchHint())
    {
        QList<QLandmarkSortOrder> sortOrders;
        sortOrders << sorting;
        return  checkIdFetchRequest(lms, filter, sortOrders, fetchHint);
    }

    bool checkCategoryIdFetchRequest(const QList<QLandmarkCategory> &cats, const QLandmarkNameSort &nameSort)
    {
        QLandmarkCategoryIdFetchRequest catIdFetchRequest(m_manager);
        catIdFetchRequest.setSorting(nameSort);
        QSignalSpy spyCatId(&catIdFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
        catIdFetchRequest.start();
        if (!waitForAsync(spyCatId, &catIdFetchRequest))
            return false;
         QList<QLandmarkCategoryId> catIds = catIdFetchRequest.categoryIds();
         if (catIds.count() != cats.count())
            return false;
         for (int i=0; i < catIds.count(); ++i) {
            if (!(cats.at(i).categoryId() == catIds.at(i)))
                return false;
         }
        return true;
    }

        //ensure connectNotify is called by m_manager
    void connectNotifications() {
        Q_ASSERT(m_manager);
        Q_ASSERT(m_listener);
        connect(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)),
                m_listener, SLOT(landmarksAdded(QList<QLandmarkId>)));
        connect(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)),
                m_listener, SLOT(landmarksChanged(QList<QLandmarkId>)));
        connect(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
                m_listener, SLOT(landmarksRemoved(QList<QLandmarkId>)));
        connect(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
                m_listener,SLOT(categoriesAdded(QList<QLandmarkCategoryId>)));
        connect(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
                m_listener,SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
        connect(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
                m_listener, SLOT(categoriesRemoved(QList<QLandmarkCategoryId>)));
     }

      void disconnectNotifications() {
        Q_ASSERT(m_manager);
        Q_ASSERT(m_listener);
        disconnect(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)),
                m_listener, SLOT(landmarksAdded(QList<QLandmarkId>)));
        disconnect(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)),
                m_listener, SLOT(landmarksChanged(QList<QLandmarkId>)));
        disconnect(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
                m_listener, SLOT(landmarksRemoved(QList<QLandmarkId>)));
        disconnect(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
                m_listener,SLOT(categoriesAdded(QList<QLandmarkCategoryId>)));
        disconnect(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
                m_listener,SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
        disconnect(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
                m_listener, SLOT(categoriesRemoved(QList<QLandmarkCategoryId>)));
     }

    void createDb() {
        QMap<QString, QString> map;
        map["filename"] = "test.db";

        if (m_manager != 0) {
            delete m_manager;
            m_manager =0;
        }

        if (m_listener !=0) {
            delete m_listener;
            m_listener =0;
        }

        m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
        m_listener = new ManagerListener;
        connectNotifications();
    }

    void deleteDb() {
        {
            QSqlDatabase db = QSqlDatabase::database("landmarks");
            QSqlQuery q1("delete from landmark;", db);
            q1.exec();
            QSqlQuery q2("delete from category;", db);
            q2.exec();
            QSqlQuery q3("delete from landmark_category;", db);
            q3.exec();
        }

        delete m_manager;
        m_manager = 0;

        delete m_listener;
        m_listener =0;
        QFile file("test.db");
        file.remove();
    }

    bool tablesExist() {
        QStringList tables = QSqlDatabase::database("landmarks").tables();
        tables.sort();

        QStringList knownTables;
        knownTables << "category";
        knownTables << "category_attribute";
        knownTables << "category_notification";
        knownTables << "landmark";
        knownTables << "landmark_attribute";
        knownTables << "landmark_category";
        knownTables << "landmark_notification";

        return (tables == knownTables);
    }

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void createDbNew();
    void createDbExists();

    void retrieveCategory();
    void retrieveCategoryAsync();

    void categoryFetchCancelAsync();

    void retrieveLandmark();
    void retrieveLandmarkAsync();

    void asyncLandmarkFetchCancel();

    void addCategory();
    void addCategoryAsync();

     void updateCategory();
     void updateCategoryAsync();

    void addLandmark();
    void addLandmarkAsync();

    void updateLandmark();

    void removeCategoryId();
    void removeCategoryIdAsync();

    void removeLandmark();
    void removeLandmarkAsync();

    void retrieveMultipleCategories();
     void retrieveMultipleCategoriesAsync();

    void retrieveMultipleLandmarks();
    void retrieveMultipleLandmarksAsync();

    void saveMultipleLandmarks();

    void removeMultipleLandmarks();
    void removeMultipleLandmarksAsync();

    void listCategoryIds();
    void filterLandmarksLimitMatches();

    void filterLandmarksLimitMatchesAsync();
    void filterLandmarksDefault();

    void filterLandmarksName();
    void filterLandmarksNameAsync();

    void filterLandmarksProximity();
    void filterLandmarksProximityAsync();

    void filterLandmarksNearest();
    void filterLandmarksNearestAsync();

    void filterLandmarksCategory();
    void filterLandmarksCategoryAsync();

    void filterLandmarksBox();
    void filterLandmarksBoxAsync();

    void filterLandmarksIntersection();
    void filterLandmarksIntersectionAsync();

    void filterLandmarksUnion();
    void filterLandmarksUnionAsync();

    void sortLandmarksNull();

    void sortLandmarksName();
    void sortLandmarksNameAsync();

    void sortLandmarksDistance();
    void sortLandmarksDistanceAsync();


    void importGpx();
    //void importGpxAsync();

    void exportGpx();
    void exportGpxAsync();

    /*
    void sortLandmarksNameDistance();
    void filterDistanceSortName();
    void filterNameSortDistance();
    void filterLandmarksCustom();
    void sortLandmarksCustom();
    */
};


void tst_QLandmarkManagerEngineSqlite::initTestCase() {
    m_manager = 0;
    m_listener = 0;
}

void tst_QLandmarkManagerEngineSqlite::init() {
    createDb();
}

void tst_QLandmarkManagerEngineSqlite::cleanup() {
    deleteDb();
}

void tst_QLandmarkManagerEngineSqlite::createDbNew() {
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());

    deleteDb();
    createDb();

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());
}

void tst_QLandmarkManagerEngineSqlite::createDbExists() {
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());

    createDb();

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());
}

void tst_QLandmarkManagerEngineSqlite::retrieveCategory() {
    QLandmarkCategoryId id1;
    id1.setManagerUri(m_manager->managerUri());
    id1.setLocalId("1");
    QCOMPARE(m_manager->category(id1).categoryId().isValid(), false);

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategoryId id2;
    id2.setManagerUri(m_manager->managerUri());
    id2.setLocalId(cat2.categoryId().localId());
    QCOMPARE(cat2, m_manager->category(id2));

    // add  - with attributes
    // get present
}

void tst_QLandmarkManagerEngineSqlite::retrieveCategoryAsync() {
        QLandmarkCategoryId id1;
        id1.setManagerUri(m_manager->managerUri());
        id1.setLocalId("1");

        QLandmarkCategoryFetchRequest fetchRequest(m_manager);
        QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
        fetchRequest.setCategoryId(id1);
        fetchRequest.start();

        QVERIFY(waitForAsync(spy, &fetchRequest));
        QCOMPARE(fetchRequest.categories().count(), 0);

        fetchRequest.setMatchingScheme(QLandmarkCategoryFetchRequest::MatchAll);
        fetchRequest.start();
        QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::DoesNotExistError));
        QCOMPARE(fetchRequest.categories().count(), 0);

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategoryId id2;
        id2.setManagerUri(m_manager->managerUri());
        id2.setLocalId(cat2.categoryId().localId());

        fetchRequest.setCategoryId(id2);
        fetchRequest.start();
        QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::NoError));
        QCOMPARE(fetchRequest.categories().count(),1);
        QCOMPARE(cat2, fetchRequest.categories().at(0));

        // add  - with attributes
        // get present
}


void tst_QLandmarkManagerEngineSqlite::categoryFetchCancelAsync() {
    disconnectNotifications();
    QLandmarkCategory cat;
    for(int i=0; i < 1550; ++i) {
        cat.clear();
        cat.setName(QString("CAT") + QString::number(i));
        QVERIFY(m_manager->saveCategory(&cat));
    }

    QLandmarkCategoryFetchRequest catFetchRequest(m_manager);
    QSignalSpy spy(&catFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    catFetchRequest.start();
    QTest::qWait(50);
    catFetchRequest.cancel();
    QVERIFY(waitForAsync(spy, &catFetchRequest, QLandmarkManager::CancelError));
    QCOMPARE(catFetchRequest.categories().count(), 0);

    /*
    Testing cancel for category id fetch is difficult because on
            desktop, the operation is too quick to cancel.  We could
            try populating the database with enough categories but
            that would take a very long time.

            for(int i=0; i < 350; ++i) {
        cat.clear();
        cat.setName(QString("CAT") + QString::number(i));
        QVERIFY(m_manager->saveCategory(&cat));
    }

    QLandmarkCategoryIdFetchRequest catIdFetchRequest(m_manager);
    QSignalSpy spy2(&catIdFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    catIdFetchRequest.start();
    QTest::qWait(10);
    catIdFetchRequest.cancel();
    QVERIFY(waitForAsync(spy2, &catIdFetchRequest, QLandmarkManager::CancelError));
    */
}


void tst_QLandmarkManagerEngineSqlite::retrieveLandmark() {
    QLandmarkId id1;
    id1.setManagerUri(m_manager->managerUri());
    id1.setLocalId("1");
    QCOMPARE(m_manager->landmark(id1).landmarkId().isValid(), false);

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmarkId id2;
    id2.setManagerUri(m_manager->managerUri());
    id2.setLocalId(lm2.landmarkId().localId());
    QCOMPARE(lm2, m_manager->landmark(id2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmarkId id3;
    id3.setManagerUri(m_manager->managerUri());
    id3.setLocalId(lm3.landmarkId().localId());
    QCOMPARE(lm3, m_manager->landmark(id3));
}

 void tst_QLandmarkManagerEngineSqlite::retrieveLandmarkAsync() {
     //test non-existent landmark id
     QLandmarkId id1;
     id1.setManagerUri(m_manager->managerUri());
     id1.setLocalId("1");

     QLandmarkFetchRequest fetchRequest(m_manager,this);
     QLandmarkIdFilter idFilter;
     idFilter.append(id1);
     fetchRequest.setFilter(idFilter);
     QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
     fetchRequest.start();

     QTest::qWait(1000);
     QCOMPARE(spy.count(),2);
     spy.clear();
     QCOMPARE(fetchRequest.landmarks().count(),0);

     //test retrieval of existing landmark via id
     QLandmark lm2;
     lm2.setName("LM2");
     QVERIFY(m_manager->saveLandmark(&lm2));

     QLandmarkId id2;
     id2.setManagerUri(m_manager->managerUri());
     id2.setLocalId(lm2.landmarkId().localId());
     idFilter.clear();
     idFilter.append(id2);
     fetchRequest.setFilter(idFilter);
     spy.clear();
     fetchRequest.start();

     QTest::qWait(2000);
     QCOMPARE(spy.count(),2);

     QCOMPARE(fetchRequest.landmarks().count(), 1);
     QCOMPARE(lm2, fetchRequest.landmarks().at(0));

     QLandmarkCategory cat3;
     cat3.setName("CAT3");
     QVERIFY(m_manager->saveCategory(&cat3));

     //test retrieval of existing landmark
     //that has categories via id
     QLandmark lm3;
     lm3.setName("LM3");
     lm3.addCategoryId(cat3.categoryId());
     QVERIFY(m_manager->saveLandmark(&lm3));

     QLandmarkId id3;
     id3.setManagerUri(m_manager->managerUri());
     id3.setLocalId(lm3.landmarkId().localId());

     idFilter.clear();
     idFilter.append(id3);
     fetchRequest.setFilter(idFilter);
     spy.clear();
     fetchRequest.start();

     QTest::qWait(2000);
     QCOMPARE(spy.count(),2);
     QCOMPARE(fetchRequest.landmarks().count(), 1);
     QCOMPARE(lm3, fetchRequest.landmarks().at(0));

     //try a filter with no ids
     idFilter.clear();
     fetchRequest.setFilter(idFilter);
     spy.clear();
     fetchRequest.start();

     QTest::qWait(2000);
     QCOMPARE(spy.count(),2);
     QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
     QCOMPARE(fetchRequest.landmarks().count(), 0);
}

 void tst_QLandmarkManagerEngineSqlite::asyncLandmarkFetchCancel() {
     //test that we can cancel a fetch for landmarks
     QLandmark lm;
     for(int i =0; i < 75; ++i) {
         lm.clear();
         lm.setName(QString("LM") + QString::number(i));
         lm.setCoordinate(QGeoCoordinate(5.0, 5.0));
         QVERIFY(m_manager->saveLandmark(&lm));
     }

     QLandmarkFetchRequest fetchRequest(m_manager);
     QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));

     //we use a lot of intersection and union filters to try slow down the fetching
     //enough so that we can cancel the fetching operation.
     QLandmarkProximityFilter proximityFilter(QGeoCoordinate(5.0,5.0), 1000);
     proximityFilter.setSelection(QLandmarkProximityFilter::SelectAll);

     QLandmarkIntersectionFilter intersectionFilter;
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);
     intersectionFilter.append(proximityFilter);

     QLandmarkUnionFilter unionFilter;
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);
     unionFilter.append(intersectionFilter);

     //test canceling of a landmark fetch
     fetchRequest.setFilter(unionFilter);
     fetchRequest.start();
     QTest::qWait(50);
     fetchRequest.cancel();
     QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::CancelError));
     QCOMPARE(fetchRequest.landmarks().count(), 0);

     //test canceling of a landmark id fetch
     QLandmarkIdFetchRequest idFetchRequest(m_manager);
     idFetchRequest.setFilter(unionFilter);
     QSignalSpy spy2(&idFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
     idFetchRequest.start();
     QTest::qWait(50);
     idFetchRequest.cancel();
     QVERIFY(waitForAsync(spy2, &idFetchRequest, QLandmarkManager::CancelError));
     QCOMPARE(idFetchRequest.landmarkIds().count(), 0);
 }

 void tst_QLandmarkManagerEngineSqlite::addCategory() {
     QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));

     // add - no attributes
     QLandmarkCategory cat1;
     cat1.setName("CAT1");
     QVERIFY(m_manager->saveCategory(&cat1));
     QCOMPARE(cat1, m_manager->category(cat1.categoryId()));

     QTest::qWait(10);
     QCOMPARE(spyAdd.count(), 1);
     QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.categoryId());

     // add - with attributes
}

void tst_QLandmarkManagerEngineSqlite::addCategoryAsync() {
    QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));

    // add - no attributes
    QLandmarkCategory cat1;
    cat1.setName("CAT1");

    QLandmarkCategorySaveRequest saveCategoryRequest(m_manager);
    QSignalSpy spy(&saveCategoryRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    saveCategoryRequest.setCategory(cat1);
    saveCategoryRequest.start();

    QVERIFY(waitForAsync(spy, &saveCategoryRequest));
    QCOMPARE(saveCategoryRequest.categories().count(),1);
    QVERIFY(saveCategoryRequest.categories().at(0).categoryId().isValid());
    cat1.setCategoryId(saveCategoryRequest.categories().at(0).categoryId());
    QCOMPARE(cat1, saveCategoryRequest.categories().at(0));
    QCOMPARE(cat1, m_manager->category(saveCategoryRequest.categories().at(0).categoryId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.categoryId());

    // add - with attributes
}

void tst_QLandmarkManagerEngineSqlite::updateCategory() {
    QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
    QSignalSpy spyChange(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));

    // bad manager uri
    QLandmarkCategory badCat1;
    QLandmarkCategoryId badCatId1;

    badCat1.setName("BAD1");
    badCatId1.setLocalId("1");
    badCatId1.setManagerUri("bad manager uri");
    badCat1.setCategoryId(badCatId1);

    QVERIFY(!m_manager->saveCategory(&badCat1));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(m_manager->errorString(), QString("Category id comes from different landmark manager."));

    // non existent id
    QLandmarkCategory badCat2;
    QLandmarkCategoryId badCatId2;

    badCat2.setName("BAD2");
    badCatId2.setLocalId("bad integer id");
    badCatId2.setManagerUri(m_manager->managerUri());
    badCat2.setCategoryId(badCatId2);

    QVERIFY(!m_manager->saveCategory(&badCat2));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(m_manager->errorString(), QString("Category id does not exist in this landmark manager."));

    // add - with attributes
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    //cat1.setAttribute("Key1", "Value1");
    QVERIFY(m_manager->saveCategory(&cat1));
    QCOMPARE(cat1, m_manager->category(cat1.categoryId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.categoryId());
    spyAdd.clear();

    // update core
    cat1.setName("CAT1New");
    QVERIFY(m_manager->saveCategory(&cat1));
    QCOMPARE(cat1, m_manager->category(cat1.categoryId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.categoryId());

    // update attributes - existing
    // update attributes - added
    // update attributes - removed
}

void tst_QLandmarkManagerEngineSqlite::updateCategoryAsync() {
    QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
    QSignalSpy spyChange(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));

    // bad manager uri
    QLandmarkCategory badCat1;
    QLandmarkCategoryId badCatId1;

    badCat1.setName("BAD1");
    badCatId1.setLocalId("1");
    badCatId1.setManagerUri("bad manager uri");
    badCat1.setCategoryId(badCatId1);

    QLandmarkCategorySaveRequest saveCategoryRequest(m_manager);
    QSignalSpy spy(&saveCategoryRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    saveCategoryRequest.setCategory(badCat1);
    saveCategoryRequest.start();

    QVERIFY(waitForAsync(spy, &saveCategoryRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(saveCategoryRequest.categories().count(),1);
    QCOMPARE(saveCategoryRequest.errorMap().count(),1);
    QCOMPARE(saveCategoryRequest.errorMap().value(0) , QLandmarkManager::DoesNotExistError);
    QCOMPARE(saveCategoryRequest.errorString(), QString("Category id comes from different landmark manager."));

    // non existent id
    QLandmarkCategory badCat2;
    QLandmarkCategoryId badCatId2;

    badCat2.setName("BAD2");
    badCatId2.setLocalId("bad integer id");
    badCatId2.setManagerUri(m_manager->managerUri());
    badCat2.setCategoryId(badCatId2);

    saveCategoryRequest.setCategory(badCat2);
    saveCategoryRequest.start();

    QVERIFY(waitForAsync(spy, &saveCategoryRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(saveCategoryRequest.errorMap().count(),1);
    QCOMPARE(saveCategoryRequest.errorMap().value(0) , QLandmarkManager::DoesNotExistError);
    QCOMPARE(saveCategoryRequest.errorString(), QString("Category id does not exist in this landmark manager."));

    // add - with attributes
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    //cat1.setAttribute("Key1", "Value1");

    saveCategoryRequest.setCategory(cat1);
    saveCategoryRequest.start();

    QVERIFY(waitForAsync(spy, &saveCategoryRequest));
    QCOMPARE(saveCategoryRequest.errorMap().count(),1);
    QCOMPARE(saveCategoryRequest.errorMap().value(0) , QLandmarkManager::NoError);
    QCOMPARE(saveCategoryRequest.categories().count(), 1);
    QLandmarkCategory cat1new = cat1;
    cat1new.setCategoryId(saveCategoryRequest.categories().at(0).categoryId());
    QCOMPARE(saveCategoryRequest.categories().at(0), m_manager->category(cat1new.categoryId()));
    QCOMPARE(saveCategoryRequest.categories().at(0), cat1new);

    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1new.categoryId());
    spyAdd.clear();

    // update core
    cat1.setCategoryId(cat1new.categoryId());
    cat1.setName("CAT1Changed");
    saveCategoryRequest.setCategory(cat1);
    saveCategoryRequest.start();
    QVERIFY(waitForAsync(spy, &saveCategoryRequest));
    QCOMPARE(saveCategoryRequest.errorMap().count(),1);
    QCOMPARE(saveCategoryRequest.errorMap().value(0) , QLandmarkManager::NoError);
    QCOMPARE(saveCategoryRequest.categories().count(), 1);

    QVERIFY(saveCategoryRequest.categories().at(0).categoryId().isValid());
    cat1new = cat1;
    cat1new.setCategoryId(saveCategoryRequest.categories().at(0).categoryId());
    QCOMPARE(saveCategoryRequest.categories().at(0), m_manager->category(cat1new.categoryId()));
    QCOMPARE(saveCategoryRequest.categories().at(0), cat1new);

    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1new.categoryId());
    spyChange.clear();

    QLandmarkCategory goodCat1;
    goodCat1.setName("GOOD1");

    //save multiple categories where the
    //first is a new category, second should fail, third is an update
    QList<QLandmarkCategory> categories;
    categories << goodCat1 << badCat1 << cat1new;
    saveCategoryRequest.setCategories(categories);
    saveCategoryRequest.start();
    QVERIFY(waitForAsync(spy, &saveCategoryRequest, QLandmarkManager::DoesNotExistError,2000));
    QCOMPARE(saveCategoryRequest.categories().count(), 3);
    QCOMPARE(saveCategoryRequest.errorMap().count(),3);
    QCOMPARE(saveCategoryRequest.errorMap().value(0) , QLandmarkManager::NoError);
    QCOMPARE(saveCategoryRequest.errorMap().value(1) , QLandmarkManager::DoesNotExistError);
    QCOMPARE(saveCategoryRequest.errorMap().value(2) , QLandmarkManager::NoError);

    QLandmarkCategoryId firstCatId  = saveCategoryRequest.categories().at(0).categoryId();
    QLandmarkCategoryId secondCatId  = saveCategoryRequest.categories().at(1).categoryId();
    QLandmarkCategoryId thirdCatId  = saveCategoryRequest.categories().at(2).categoryId();

    goodCat1.setCategoryId(firstCatId);
    cat1new.setCategoryId(thirdCatId);

    QCOMPARE(goodCat1, m_manager->category(firstCatId));
    QVERIFY(!m_manager->category(secondCatId).categoryId().isValid());
    QCOMPARE(m_manager->error(), QLandmarkManager::BadArgumentError);
    QCOMPARE(cat1new, m_manager->category(thirdCatId));

    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), firstCatId);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), thirdCatId);


    // update attributes - existing
    // update attributes - added
    // update attributes - removed
}

void tst_QLandmarkManagerEngineSqlite::addLandmark() {
    QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
    // add - no attributes
    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));


    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyAdd.clear();

    // add - with attributes
    // add - with categories

    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));
    QCOMPARE(cat1, m_manager->category(cat1.categoryId()));

    QLandmark lm2;
    lm2.setName("LM2");
    lm2.addCategoryId(cat1.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm2));
    QCOMPARE(lm2, m_manager->landmark(lm2.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm2.landmarkId());
}

void tst_QLandmarkManagerEngineSqlite::addLandmarkAsync() {
    QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));

    // add - no attributes
    QLandmark lm1;
    lm1.setName("LM1");
    QLandmarkSaveRequest saveRequest(m_manager);
    saveRequest.setLandmark(lm1);

    QSignalSpy spy(&saveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));

    saveRequest.start();
    QVERIFY(waitForAsync(spy, &saveRequest));

    QCOMPARE(saveRequest.landmarks().count(),1);
    QLandmarkId id1 = saveRequest.landmarks().at(0).landmarkId();
    QCOMPARE(saveRequest.landmarks().at(0), m_manager->landmark(id1));

    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), id1);
    spyAdd.clear();

    // add - with attributes

    // add - with categories
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));
    QCOMPARE(cat1, m_manager->category(cat1.categoryId()));

    QLandmark lm2;
    lm2.setName("LM2");
    lm2.addCategoryId(cat1.categoryId());

    saveRequest.setLandmark(lm2);
    saveRequest.start();

    QVERIFY(waitForAsync(spy, &saveRequest));

    QCOMPARE(saveRequest.landmarks().count(),1);
    QLandmarkId id2 = saveRequest.landmarks().at(0).landmarkId();
    QCOMPARE(saveRequest.landmarks().at(0), m_manager->landmark(id2));

    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), id2);
    spyAdd.clear();

    QLandmarkNameFilter nameFilter;
    nameFilter.setName("LM2");
    QList<QLandmark> landmarks = m_manager->landmarks(nameFilter);

    QCOMPARE(landmarks.count(), 1);
    QCOMPARE(saveRequest.landmarks().at(0), landmarks.at(0));

    //try assigning a non-existient category to a landmark and saving it
    QLandmarkCategoryId catId3;
    catId3.setManagerUri(cat1.categoryId().managerUri());
    catId3.setLocalId("10");

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(catId3);

    saveRequest.setLandmark(lm3);
    saveRequest.start();

    QVERIFY(waitForAsync(spy, &saveRequest, QLandmarkManager::BadArgumentError));
    QVERIFY(saveRequest.errorMap().size() == 1);
    QVERIFY(!saveRequest.landmarks().at(0).landmarkId().isValid());

    nameFilter.setName("LM3");
    landmarks = m_manager->landmarks(nameFilter);
    QCOMPARE(landmarks.count(), 0);
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    QCOMPARE(spyAdd.count(), 0);

    //try saving a landmark with a non-existent id
    QLandmarkId lmId4;
    lmId4.setLocalId("10");
    lmId4.setManagerUri(m_manager->managerUri());

    QLandmark lm4;
    lm4.setName("LM4");
    lm4.setLandmarkId(lmId4);

    saveRequest.setLandmark(lm4);
    saveRequest.start();
    QVERIFY(waitForAsync(spy, &saveRequest, QLandmarkManager::DoesNotExistError));
    QLandmark landmark = m_manager->landmark(lmId4);

    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);

    QCOMPARE(spyAdd.count(), 0);

    nameFilter.setName("LM4");
    landmarks = m_manager->landmarks(nameFilter);

    QCOMPARE(landmarks.count(), 0);
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    //try saving a list of landmarks where one landmark has a non-existent id
    QLandmark lm5;
    lm5.setName("LM5");

    QLandmark lm6;
    lm6.setName("LM6");
    QLandmarkId lmId6;
    lmId6.setLocalId("900"); //non-existent id
    lmId6.setManagerUri(m_manager->managerUri());
    lm6.setLandmarkId(lmId6);

    QLandmark lm7;
    lm7.setName("LM7");

    lm5.setDescription("landmark 5;");
    lm6.setDescription("landmark 6;");
    lm7.setDescription("landmark 7;");

    QList<QLandmark> svLandmarks;
    svLandmarks << lm5 << lm6 << lm7;

    saveRequest.setLandmarks(svLandmarks);
    saveRequest.start();
    QVERIFY(waitForAsync(spy, &saveRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(saveRequest.errorMap().count(), 3);
    QCOMPARE(saveRequest.errorMap().value(0), QLandmarkManager::NoError);
    QCOMPARE(saveRequest.errorMap().value(1), QLandmarkManager::DoesNotExistError);
    QCOMPARE(saveRequest.errorMap().value(2), QLandmarkManager::NoError);
    QCOMPARE(saveRequest.landmarks().count(), 3);
    landmark = m_manager->landmark(saveRequest.landmarks().at(0).landmarkId());
    QCOMPARE(landmark.name(), lm5.name());
    QCOMPARE(lm6, saveRequest.landmarks().value(1)); //failed landmark remains the same
    landmark = m_manager->landmark(saveRequest.landmarks().at(2).landmarkId());
    QCOMPARE(landmark.name(), lm7.name());

    nameFilter.setName("LM6");
    landmarks = m_manager->landmarks(nameFilter);
    QCOMPARE(landmarks.count(), 0);
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    QCOMPARE(spyAdd.count(), 2);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), saveRequest.landmarks().at(0).landmarkId());
    QCOMPARE(spyAdd.at(1).at(0).value<QList<QLandmarkId> >().at(0), saveRequest.landmarks().at(2).landmarkId());

    //test cancel
    landmarks.clear();
    QLandmark lm;
    for (int i =0; i < 1000; i++) {
        lm.clear();
        lm.setName(QString("LM") + QString::number(i));
        landmarks.append(lm);
    }

    //we disable notifications because we will be flooded with notifications
    //during the save and this will interefere with detecting when the operation
    //is finshidd TODO: handle multiple saves so that we don't get flooded with notifications
    this->disconnectNotifications();

    saveRequest.setLandmarks(landmarks);
    saveRequest.start();
    QTest::qWait(500);
    saveRequest.cancel();
    QVERIFY(waitForAsync(spy, &saveRequest,QLandmarkManager::CancelError));
    bool foundCancelError = false;
    for (int i=0; i < saveRequest.landmarks().count(); ++i) {
        if (saveRequest.errorMap().value(i) == QLandmarkManager::CancelError) {
            foundCancelError = true;
            for (int j=i; j < saveRequest.landmarks().count(); ++j) {
                QVERIFY(saveRequest.errorMap().value(j) == QLandmarkManager::CancelError);
            }
        } else {
            QVERIFY(saveRequest.errorMap().value(i) == QLandmarkManager::NoError);
        }

    }
    QVERIFY(foundCancelError);
}

void tst_QLandmarkManagerEngineSqlite::updateLandmark() {
    QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
    QSignalSpy spyChange(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)));

    // bad manager uri
    QLandmark badLm1;
    QLandmarkId badLmId1;

    badLm1.setName("BAD1");
    badLmId1.setLocalId("1");
    badLmId1.setManagerUri("bad manager uri");
    badLm1.setLandmarkId(badLmId1);

    QVERIFY(!m_manager->saveLandmark(&badLm1));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(m_manager->errorString(), QString("Landmark id comes from different landmark manager."));

    // non existent id
    QLandmark badLm2;
    QLandmarkId badLmId2;

    badLm2.setName("BAD2");
    badLmId2.setLocalId("bad integer id");
    badLmId2.setManagerUri(m_manager->managerUri());
    badLm2.setLandmarkId(badLmId2);

    QVERIFY(!m_manager->saveLandmark(&badLm2));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(m_manager->errorString(), QString("Landmark id does not exist in this landmark manager."));

    // add - with attributes
    QLandmark lm1;
    lm1.setName("LM1");
    //lm1.setAttribute("Key1", "Value1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyAdd.clear();

    // update core
    lm1.setName("LM1New");
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();

    // update attributes - existing
    // update attributes - added
    // update attributes - removed

    // add and removing categories to landmarks

    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    // adding categories

    lm1.addCategoryId(cat1.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();

    lm1.addCategoryId(cat2.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();

    lm1.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();

    // removing categories

    lm1.removeCategoryId(cat1.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();

    lm1.removeCategoryId(cat2.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();

    lm1.removeCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyChange.clear();
}

void tst_QLandmarkManagerEngineSqlite::removeCategoryId() {
    QSignalSpy spyRemove(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));

    QLandmarkCategoryId id1;
    id1.setManagerUri(m_manager->managerUri());
    id1.setLocalId("1");

    QVERIFY(m_manager->removeCategory(id1));
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QCOMPARE(m_manager->errorString(), QString());

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 0);

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QVERIFY(m_manager->removeCategory(cat2.categoryId()));

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat2.categoryId());
    spyRemove.clear();

    // with attributes

    // effect on landmarks

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    lm1.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    QVERIFY(m_manager->removeCategory(cat3.categoryId()));

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat3.categoryId());
    spyRemove.clear();

    QLandmark lm2 = m_manager->landmark(lm1.landmarkId());
    QVERIFY(lm1 != lm2);
    QCOMPARE(lm1.categoryIds().size(), 1);
    QCOMPARE(lm2.categoryIds().size(), 0);

    QLandmark lmA;
    lmA.setName("LMA");
    QVERIFY(m_manager->saveLandmark(&lmA));

    QLandmarkCategory catX;
    catX.setName("CATX");
    QVERIFY(m_manager->saveCategory(&catX));

    QLandmarkCategory catY;
    catY.setName("CATY");
    QVERIFY(m_manager->saveCategory(&catY));

    QLandmarkCategory catZ;
    catZ.setName("CATZ");
    QVERIFY(m_manager->saveCategory(&catZ));

    lmA.addCategoryId(catX.categoryId());
    lmA.addCategoryId(catY.categoryId());
    lmA.addCategoryId(catZ.categoryId());
    QVERIFY(m_manager->saveLandmark(&lmA));

    QVERIFY(m_manager->removeCategory(catY.categoryId()));
    QLandmark lmAnew = m_manager->landmark(lmA.landmarkId());
    QVERIFY(lmA != lmAnew);
    QCOMPARE(lmA.categoryIds().count(), 3);
    QCOMPARE(lmAnew.categoryIds().count(), 2);
    QCOMPARE(lmAnew.categoryIds().at(0), catX.categoryId());
    QCOMPARE(lmAnew.categoryIds().at(1), catZ.categoryId());

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), catY.categoryId());
    spyRemove.clear();
}

void tst_QLandmarkManagerEngineSqlite::removeCategoryIdAsync() {
    QSignalSpy spyRemove(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));

    QLandmarkCategoryId id1;
    id1.setManagerUri(m_manager->managerUri());
    id1.setLocalId("1");

    QLandmarkCategoryRemoveRequest removeRequest(m_manager);
    QSignalSpy spy(&removeRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    removeRequest.setCategoryId(id1);
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::NoError));
    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);

    QCOMPARE(spyRemove.count(), 0);

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    removeRequest.setCategoryId(cat2.categoryId());
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::NoError));
    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);

    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat2.categoryId());
    spyRemove.clear();

    // with attributes

    // effect on landmarks

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    lm1.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));
    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));

    removeRequest.setCategoryId(cat3.categoryId());
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::NoError));
    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);

    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat3.categoryId());
    spyRemove.clear();

    QLandmark lm2 = m_manager->landmark(lm1.landmarkId());
    QVERIFY(lm1 != lm2);
    QCOMPARE(lm1.categoryIds().size(), 1);
    QCOMPARE(lm2.categoryIds().size(), 0);

    QLandmark lmA;
    lmA.setName("LMA");
    QVERIFY(m_manager->saveLandmark(&lmA));

    QLandmarkCategory catX;
    catX.setName("CATX");
    QVERIFY(m_manager->saveCategory(&catX));

    QLandmarkCategory catY;
    catY.setName("CATY");
    QVERIFY(m_manager->saveCategory(&catY));

    QLandmarkCategory catZ;
    catZ.setName("CATZ");
    QVERIFY(m_manager->saveCategory(&catZ));

    lmA.addCategoryId(catX.categoryId());
    lmA.addCategoryId(catY.categoryId());
    lmA.addCategoryId(catZ.categoryId());

    QVERIFY(m_manager->saveLandmark(&lmA));
    removeRequest.setCategoryId(catY.categoryId());
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::NoError));
    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);

    QLandmark lmAnew = m_manager->landmark(lmA.landmarkId());
    QVERIFY(lmA != lmAnew);
    QCOMPARE(lmA.categoryIds().count(), 3);
    QCOMPARE(lmAnew.categoryIds().count(), 2);
    QCOMPARE(lmAnew.categoryIds().at(0), catX.categoryId());
    QCOMPARE(lmAnew.categoryIds().at(1), catZ.categoryId());

    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), catY.categoryId());
    spyRemove.clear();
}

void tst_QLandmarkManagerEngineSqlite::removeLandmark() {
    QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));

    //remove non-existent landmark
    QLandmarkId id1;
    id1.setManagerUri(m_manager->managerUri());
    id1.setLocalId("1");

    QVERIFY(!m_manager->removeLandmark(id1));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 0);

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QVERIFY(m_manager->removeLandmark(lm2.landmarkId()));
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm2.landmarkId());
    spyRemove.clear();

    // with attributes

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm3));

    QVERIFY(m_manager->removeLandmark(lm3.landmarkId()));
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    //remove a landmark that's just been removed
    QVERIFY(!m_manager->removeLandmark(lm3.landmarkId()));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);

    QTest::qWait(10);
    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm3.landmarkId());
    spyRemove.clear();
}

void tst_QLandmarkManagerEngineSqlite::removeLandmarkAsync() {
    //TODO: notifications
    //QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));

    //remove non-existent landmark;
    QLandmarkId id1;
    id1.setManagerUri(m_manager->managerUri());
    id1.setLocalId("1");

    QLandmarkRemoveRequest removeRequest(m_manager);
    removeRequest.setLandmarkId(id1);
    QSignalSpy spy(&removeRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::DoesNotExistError);

    //TODO: notifications
    //QCOMPARE(spyRemove.count(), 0);

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));

    removeRequest.setLandmarkId(lm2.landmarkId());
    removeRequest.start();
    QVERIFY(waitForAsync(spy, &removeRequest));

    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);

    //TODO: notifications
    //QCOMPARE(spyRemove.count(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm2.landmarkId());
    //spyRemove.clear();

    // with attributes

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm3));

    removeRequest.setLandmarkId(lm3.landmarkId());
    removeRequest.start();
    QVERIFY(waitForAsync(spy, &removeRequest));

    QCOMPARE(removeRequest.error(), QLandmarkManager::NoError);
    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);

    //remove a landmark that's just been removed.
    removeRequest.setLandmarkId(lm3.landmarkId());
    removeRequest.start();
    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::DoesNotExistError));

    QCOMPARE(removeRequest.errorMap().count(),1);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::DoesNotExistError);
    //TODO: notifications
    //QCOMPARE(spyRemove.count(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm3.landmarkId());
    ///spyRemove.clear();
}

void tst_QLandmarkManagerEngineSqlite::retrieveMultipleCategories() {
    QList<QLandmarkCategoryId> catIds;

    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));
    catIds << cat1.categoryId();

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));
    catIds << cat3.categoryId();

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));
    catIds << cat2.categoryId();

    QString uri = m_manager->managerUri();
    int i = 1;

    QList<QLandmarkCategory> cats = m_manager->categories(catIds);
    QCOMPARE(cats.size(), 3);
    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(0).categoryId().isValid(), true);
    QCOMPARE(cats.at(1).name(), QString("CAT3"));
    QCOMPARE(cats.at(1).categoryId().isValid(), true);
    QCOMPARE(cats.at(2).name(), QString("CAT2"));
    QCOMPARE(cats.at(2).categoryId().isValid(), true);

    QList<QLandmarkCategoryId> invalidCatIds;

    while (invalidCatIds.size() < 3) {
        QLandmarkCategoryId id;
        id.setManagerUri(uri);
        id.setLocalId(QString::number(i));
        QLandmarkCategory cat = m_manager->category(id);
        if (!cat.categoryId().isValid()) {
            invalidCatIds << id;
        }
        ++i;
    }

    catIds.insert(1, invalidCatIds.at(0));
    catIds.insert(3, invalidCatIds.at(1));
    catIds.insert(5, invalidCatIds.at(2));

    cats = m_manager->categories(catIds);
    QCOMPARE(cats.count(), 0);
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);

    m_manager->removeCategory(cat1.categoryId());
    cat1.setCategoryId(QLandmarkCategoryId());
    m_manager->removeCategory(cat2.categoryId());
    cat2.setCategoryId(QLandmarkCategoryId());
    m_manager->removeCategory(cat3.categoryId());
    cat3.setCategoryId(QLandmarkCategoryId());

    QLandmarkCategory cat1lc;
    cat1lc.setName("cat1");

    QLandmarkCategory cat2lc;
    cat2lc.setName("cat2");

    QLandmarkCategory cat3lc;
    cat3lc.setName("cat3");

    //save categories of different case in a randomish order
    QVERIFY(m_manager->saveCategory(&cat2lc));
    QVERIFY(m_manager->saveCategory(&cat1));
    QVERIFY(m_manager->saveCategory(&cat3));
    QVERIFY(m_manager->saveCategory(&cat3lc));
    QVERIFY(m_manager->saveCategory(&cat2));
    QVERIFY(m_manager->saveCategory(&cat1lc));

    //retrieve all categories in ascending order, case insensitive
    cats = m_manager->categories();

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(1).name(), QString("cat1"));
    QCOMPARE(cats.at(2).name(), QString("cat2"));
    QCOMPARE(cats.at(3).name(), QString("CAT2"));
    QCOMPARE(cats.at(4).name(), QString("CAT3"));
    QCOMPARE(cats.at(5).name(), QString("cat3"));

    //try descending order, case insensitive
    QLandmarkNameSort nameSort;
    nameSort.setDirection(Qt::DescendingOrder);
    cats = m_manager->categories(nameSort);

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("CAT3"));
    QCOMPARE(cats.at(1).name(), QString("cat3"));
    QCOMPARE(cats.at(2).name(), QString("cat2"));
    QCOMPARE(cats.at(3).name(), QString("CAT2"));
    QCOMPARE(cats.at(4).name(), QString("CAT1"));
    QCOMPARE(cats.at(5).name(), QString("cat1"));

    //try ascending order, case sensitive
    nameSort.setDirection(Qt::AscendingOrder);
    nameSort.setCaseSensitivity(Qt::CaseSensitive);
    cats = m_manager->categories(nameSort);

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(1).name(), QString("CAT2"));
    QCOMPARE(cats.at(2).name(), QString("CAT3"));
    QCOMPARE(cats.at(3).name(), QString("cat1"));
    QCOMPARE(cats.at(4).name(), QString("cat2"));
    QCOMPARE(cats.at(5).name(), QString("cat3"));

    //try descending order, case sensitive
    nameSort.setDirection(Qt::DescendingOrder);
    nameSort.setCaseSensitivity(Qt::CaseSensitive);
    cats = m_manager->categories(nameSort);

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("cat3"));
    QCOMPARE(cats.at(1).name(), QString("cat2"));
    QCOMPARE(cats.at(2).name(), QString("cat1"));
    QCOMPARE(cats.at(3).name(), QString("CAT3"));
    QCOMPARE(cats.at(4).name(), QString("CAT2"));
    QCOMPARE(cats.at(5).name(), QString("CAT1"));
}

void tst_QLandmarkManagerEngineSqlite::retrieveMultipleCategoriesAsync() {
    QList<QLandmarkCategoryId> catIds;

    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));
    catIds << cat1.categoryId();

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));
    catIds << cat3.categoryId();

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));
    catIds << cat2.categoryId();

    QString uri = m_manager->managerUri();
    int i = 1;

    QLandmarkCategoryFetchRequest fetchRequest(m_manager);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.setMatchingScheme(QLandmarkCategoryFetchRequest::MatchAll);
    fetchRequest.setCategoryIds(catIds);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmarkCategory> cats = fetchRequest.categories();
    QCOMPARE(cats.size(), 3);

    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(0).categoryId().isValid(), true);
    QCOMPARE(cats.at(1).name(), QString("CAT3"));
    QCOMPARE(cats.at(1).categoryId().isValid(), true);
    QCOMPARE(cats.at(2).name(), QString("CAT2"));
    QCOMPARE(cats.at(2).categoryId().isValid(), true);

    //try matching subset
    fetchRequest.setMatchingScheme(QLandmarkCategoryFetchRequest::MatchSubset);
    fetchRequest.setCategoryIds(catIds);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    cats = fetchRequest.categories();
    QCOMPARE(cats.size(), 3);

    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(0).categoryId().isValid(), true);
    QCOMPARE(cats.at(1).name(), QString("CAT3"));
    QCOMPARE(cats.at(1).categoryId().isValid(), true);
    QCOMPARE(cats.at(2).name(), QString("CAT2"));
    QCOMPARE(cats.at(2).categoryId().isValid(), true);

    //
    QList<QLandmarkCategoryId> invalidCatIds;

    while (invalidCatIds.size() < 3) {
        QLandmarkCategoryId id;
        id.setManagerUri(uri);
        id.setLocalId(QString::number(i));
        QLandmarkCategory cat = m_manager->category(id);
        if (!cat.categoryId().isValid()) {
            invalidCatIds << id;
        }
        ++i;
    }

    catIds.insert(1, invalidCatIds.at(0));
    catIds.insert(3, invalidCatIds.at(1));
    catIds.insert(5, invalidCatIds.at(2));

    fetchRequest.setMatchingScheme(QLandmarkCategoryFetchRequest::MatchAll);
    fetchRequest.setCategoryIds(catIds);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(fetchRequest.categories().count(), 0);

    fetchRequest.setMatchingScheme(QLandmarkCategoryFetchRequest::MatchSubset);
    fetchRequest.setCategoryIds(catIds);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::NoError));
    cats = fetchRequest.categories();
    QCOMPARE(cats.size(), 3);

    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(0).categoryId().isValid(), true);
    QCOMPARE(cats.at(1).name(), QString("CAT3"));
    QCOMPARE(cats.at(1).categoryId().isValid(), true);
    QCOMPARE(cats.at(2).name(), QString("CAT2"));
    QCOMPARE(cats.at(2).categoryId().isValid(), true);

    //Setup categories of different case and spelling
    //saved in random order
    m_manager->removeCategory(cat1.categoryId());
    cat1.setCategoryId(QLandmarkCategoryId());
    m_manager->removeCategory(cat2.categoryId());
    cat2.setCategoryId(QLandmarkCategoryId());
    m_manager->removeCategory(cat3.categoryId());
    cat3.setCategoryId(QLandmarkCategoryId());

    QLandmarkCategory cat1lc;
    cat1lc.setName("cat1");

    QLandmarkCategory cat2lc;
    cat2lc.setName("cat2");

    QLandmarkCategory cat3lc;
    cat3lc.setName("cat3");

    QVERIFY(m_manager->saveCategory(&cat2lc));
    QVERIFY(m_manager->saveCategory(&cat1));
    QVERIFY(m_manager->saveCategory(&cat3));
    QVERIFY(m_manager->saveCategory(&cat3lc));
    QVERIFY(m_manager->saveCategory(&cat2));
    QVERIFY(m_manager->saveCategory(&cat1lc));


    //try fetching all categories in ascending order, case insensitive
    QLandmarkNameSort nameSort;
    fetchRequest.setMatchingScheme(QLandmarkCategoryFetchRequest::MatchAll);//this should be ignored since we're requesting all categories
    fetchRequest.setCategoryIds(QList<QLandmarkCategoryId>());
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::NoError));
    cats = fetchRequest.categories();
    QVERIFY(checkCategoryIdFetchRequest(cats,nameSort));

    QCOMPARE(cats.size(), 6);
    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(1).name(), QString("cat1"));
    QCOMPARE(cats.at(2).name(), QString("cat2"));
    QCOMPARE(cats.at(3).name(), QString("CAT2"));
    QCOMPARE(cats.at(4).name(), QString("CAT3"));
    QCOMPARE(cats.at(5).name(), QString("cat3"));

    //try descnding order case insensitive
    nameSort.setDirection(Qt::DescendingOrder);
    fetchRequest.setSorting(nameSort);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::NoError));
    cats = fetchRequest.categories();
    QVERIFY(checkCategoryIdFetchRequest(cats,nameSort));

    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("CAT3"));
    QCOMPARE(cats.at(1).name(), QString("cat3"));
    QCOMPARE(cats.at(2).name(), QString("cat2"));
    QCOMPARE(cats.at(3).name(), QString("CAT2"));
    QCOMPARE(cats.at(4).name(), QString("CAT1"));
    QCOMPARE(cats.at(5).name(), QString("cat1"));

    //try asending order, case sensitive
    nameSort.setDirection(Qt::AscendingOrder);
    nameSort.setCaseSensitivity(Qt::CaseSensitive);
    fetchRequest.setSorting(nameSort);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::NoError));
    cats = fetchRequest.categories();
    QVERIFY(checkCategoryIdFetchRequest(cats,nameSort));

    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("CAT1"));
    QCOMPARE(cats.at(1).name(), QString("CAT2"));
    QCOMPARE(cats.at(2).name(), QString("CAT3"));
    QCOMPARE(cats.at(3).name(), QString("cat1"));
    QCOMPARE(cats.at(4).name(), QString("cat2"));
    QCOMPARE(cats.at(5).name(), QString("cat3"));

    //try descending order, case sensitive
    nameSort.setDirection(Qt::DescendingOrder);
    nameSort.setCaseSensitivity(Qt::CaseSensitive);
    fetchRequest.setSorting(nameSort);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::NoError));
    cats = fetchRequest.categories();
    QVERIFY(checkCategoryIdFetchRequest(cats,nameSort));

    QCOMPARE(cats.count(), 6);
    QCOMPARE(cats.at(0).name(), QString("cat3"));
    QCOMPARE(cats.at(1).name(), QString("cat2"));
    QCOMPARE(cats.at(2).name(), QString("cat1"));
    QCOMPARE(cats.at(3).name(), QString("CAT3"));
    QCOMPARE(cats.at(4).name(), QString("CAT2"));
    QCOMPARE(cats.at(5).name(), QString("CAT1"));
}

void tst_QLandmarkManagerEngineSqlite::retrieveMultipleLandmarks() {
    QList<QLandmarkId> lmIds;

    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    lmIds << lm1.landmarkId();

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));
    lmIds << lm2.landmarkId();

    QLandmark lm3;
    lm3.setName("LM3");
    QVERIFY(m_manager->saveLandmark(&lm3));
    lmIds << lm3.landmarkId();

    QString uri = m_manager->managerUri();
    int i = 1;

    QList<QLandmarkId> invalidLmIds;

    while (invalidLmIds.size() < 3) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setLocalId(QString::number(i));
        QLandmark lm = m_manager->landmark(id);
        if (!lm.landmarkId().isValid()) {
            invalidLmIds << id;
        }
        ++i;
    }
    QList<QLandmark> lms = m_manager->landmarks(lmIds);

    QCOMPARE(lms.size(), 3);

    QCOMPARE(lms.at(0).name(), QString("LM1"));
    QCOMPARE(lms.at(0).landmarkId().isValid(), true);
    QCOMPARE(lms.at(1).name(), QString("LM2"));
    QCOMPARE(lms.at(1).landmarkId().isValid(), true);
    QCOMPARE(lms.at(2).name(), QString("LM3"));
    QCOMPARE(lms.at(2).landmarkId().isValid(), true);

    lmIds.insert(1, invalidLmIds.at(0));
    lmIds.insert(3, invalidLmIds.at(1));
    lmIds.insert(5, invalidLmIds.at(2));
    lms = m_manager->landmarks(lmIds);
    QCOMPARE(lms.count(), 0);
    QVERIFY(m_manager->error() == QLandmarkManager::DoesNotExistError);

    QLandmarkIdFilter idFilter(lmIds);
    idFilter.setMatchingScheme(QLandmarkIdFilter::MatchAll);
    lms = m_manager->landmarks(idFilter);
    QCOMPARE(lms.count(), 0);
    QVERIFY(m_manager->error() == QLandmarkManager::DoesNotExistError);

    idFilter.setMatchingScheme(QLandmarkIdFilter::MatchSubset);
    lms = m_manager->landmarks(idFilter);
    QCOMPARE(lms.size(), 3);
    QCOMPARE(lms.at(0).name(), QString("LM1"));
    QCOMPARE(lms.at(0).landmarkId().isValid(), true);
    QCOMPARE(lms.at(1).name(), QString("LM2"));
    QCOMPARE(lms.at(1).landmarkId().isValid(), true);
    QCOMPARE(lms.at(2).name(), QString("LM3"));
    QCOMPARE(lms.at(2).landmarkId().isValid(), true);
}

void tst_QLandmarkManagerEngineSqlite::retrieveMultipleLandmarksAsync() {
    QList<QLandmarkId> lmIds;

    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    lmIds << lm1.landmarkId();

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));
    lmIds << lm2.landmarkId();

    QLandmark lm3;
    lm3.setName("LM3");
    QVERIFY(m_manager->saveLandmark(&lm3));
    lmIds << lm3.landmarkId();

    QString uri = m_manager->managerUri();
    int i = 1;

    QList<QLandmarkId> invalidLmIds;

    while (invalidLmIds.size() < 3) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setLocalId(QString::number(i));
        QLandmark lm = m_manager->landmark(id);
        if (!lm.landmarkId().isValid()) {
            invalidLmIds << id;
        }
        ++i;
    }

    QLandmarkIdFilter idFilter(lmIds, QLandmarkIdFilter::MatchAll);
    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(idFilter);

    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark> lms = fetchRequest.landmarks();

    QCOMPARE(lms.size(), 3);

    QCOMPARE(lms.at(0).name(), QString("LM1"));
    QCOMPARE(lms.at(0).landmarkId().isValid(), true);
    QCOMPARE(lms.at(1).name(), QString("LM2"));
    QCOMPARE(lms.at(1).landmarkId().isValid(), true);
    QCOMPARE(lms.at(2).name(), QString("LM3"));
    QCOMPARE(lms.at(2).landmarkId().isValid(), true);

    lmIds.insert(1, invalidLmIds.at(0));
    lmIds.insert(3, invalidLmIds.at(1));
    lmIds.insert(5, invalidLmIds.at(2));

    idFilter.setLandmarkIds(lmIds);
    fetchRequest.setFilter(idFilter);
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest, QLandmarkManager::DoesNotExistError));
    lms = fetchRequest.landmarks();
    QCOMPARE(lms.count(), 0);

    idFilter.setMatchingScheme(QLandmarkIdFilter::MatchSubset);
    fetchRequest.setFilter(idFilter);
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QCOMPARE(lms.size(), 3);
    QCOMPARE(lms.at(0).name(), QString("LM1"));
    QCOMPARE(lms.at(0).landmarkId().isValid(), true);
    QCOMPARE(lms.at(1).name(), QString("LM2"));
    QCOMPARE(lms.at(1).landmarkId().isValid(), true);
    QCOMPARE(lms.at(2).name(), QString("LM3"));
    QCOMPARE(lms.at(2).landmarkId().isValid(), true);
}

void tst_QLandmarkManagerEngineSqlite::saveMultipleLandmarks() {
    QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
    QSignalSpy spyChange(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)));

    QList<QLandmark> lms1;

    QLandmark lm1;
    lm1.setName("LM1");
    lms1 << lm1;

    QLandmark lm2;
    lm2.setName("LM2");
    lms1 << lm2;

    QLandmark lm3;
    lm3.setName("LM3");
    lms1 << lm3;

    QVERIFY(m_manager->saveLandmarks(&lms1, 0));

    lm1 = lms1.at(0);
    lm2 = lms1.at(1);
    lm3 = lms1.at(2);

    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));
    QCOMPARE(lm2, m_manager->landmark(lm2.landmarkId()));
    QCOMPARE(lm3, m_manager->landmark(lm3.landmarkId()));

    //TODO: notifications
    //QCOMPARE(spyAdd.count(), 1);
    //QCOMPARE(spyChange.count(), 0);
    //QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().size(), 3);
    //QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    //QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.landmarkId());
    //QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(2), lm3.landmarkId());
    //spyAdd.clear();

    QList<QLandmark> lms2;

    lm1.setName("LM1New");
    lms2 << lm1;

    lm2.setName("LM2New");
    lms2 << lm2;

    lm3.setName("LM3New");
    lms2 << lm3;

    QVERIFY(m_manager->saveLandmarks(&lms2, 0));

    lm1 = lms2.at(0);
    lm2 = lms2.at(1);
    lm3 = lms2.at(2);

    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));
    QCOMPARE(lm2, m_manager->landmark(lm2.landmarkId()));
    QCOMPARE(lm3, m_manager->landmark(lm3.landmarkId()));

    //TODO: notifications
    //QCOMPARE(spyAdd.count(), 0);
    //QCOMPARE(spyChange.count(), 1);
    //QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().size(), 3);
    //QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    //QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.landmarkId());
    //QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(2), lm3.landmarkId());
    //spyChange.clear();
}

void tst_QLandmarkManagerEngineSqlite::removeMultipleLandmarks() {
    //TODO: notifications
    //QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));

    QList<QLandmarkId> lmIds1;
    QList<QLandmarkId> lmIds2;

    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    lmIds1 << lm1.landmarkId();

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));
    lmIds1 << lm2.landmarkId();

    QLandmark lm3;
    lm3.setName("LM3");
    QVERIFY(m_manager->saveLandmark(&lm3));
    lmIds2 << lm3.landmarkId();

    QString uri = m_manager->managerUri();
    int i = 1;

    QList<QLandmarkId> invalidLmIds;

    while (invalidLmIds.size() < 3) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setLocalId(QString::number(i));
        QLandmark lm = m_manager->landmark(id);
        if (!lm.landmarkId().isValid()) {
            invalidLmIds << id;
        }
        ++i;
    }

    lmIds1.insert(1, invalidLmIds.at(0));
    lmIds2.insert(0, invalidLmIds.at(1));
    lmIds2.insert(2, invalidLmIds.at(2));

    QCOMPARE(m_manager->landmark(lmIds1.at(0)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds1.at(1)).landmarkId().isValid(), false);
    QCOMPARE(m_manager->landmark(lmIds1.at(2)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds2.at(0)).landmarkId().isValid(), false);
    QCOMPARE(m_manager->landmark(lmIds2.at(1)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds2.at(2)).landmarkId().isValid(), false);

    QMap<int, QLandmarkManager::Error> errors;
    QVERIFY(!m_manager->removeLandmarks(lmIds1, &errors));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(errors.count(), 3);
    QCOMPARE(errors.value(0), QLandmarkManager::NoError);
    QCOMPARE(errors.value(1), QLandmarkManager::DoesNotExistError);
    QCOMPARE(errors.value(2), QLandmarkManager::NoError);

    //TODO: notifications
    //QCOMPARE(spyRemove.count(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().size(), 2);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.landmarkId());
    //spyRemove.clear();

    for (int i = 0; i < lmIds1.size(); ++i)
        QCOMPARE(m_manager->landmark(lmIds1.at(i)).landmarkId().isValid(), false);

    QCOMPARE(m_manager->landmark(lmIds2.at(0)).landmarkId().isValid(), false);
    QCOMPARE(m_manager->landmark(lmIds2.at(1)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds2.at(2)).landmarkId().isValid(), false);

    QVERIFY(!m_manager->removeLandmarks(lmIds2, &errors));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(errors.count(), 3);
    QCOMPARE(errors.value(0), QLandmarkManager::DoesNotExistError);
    QCOMPARE(errors.value(1), QLandmarkManager::NoError);
    QCOMPARE(errors.value(2), QLandmarkManager::DoesNotExistError);

    //TODO: Notifications
    //QCOMPARE(spyRemove.count(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().size(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm3.landmarkId());
    //spyRemove.clear();

    for (int i = 0; i < lmIds1.size(); ++i)
        QCOMPARE(m_manager->landmark(lmIds1.at(i)).landmarkId().isValid(), false);
    for (int i = 0; i < lmIds2.size(); ++i)
        QCOMPARE(m_manager->landmark(lmIds2.at(i)).landmarkId().isValid(), false);

    //test all ids to remove were valid existing ids
    QLandmark lm4;
    lm4.setName("LM4");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("LM5");
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmark lm6;
    lm3.setName("LM6");
    QVERIFY(m_manager->saveLandmark(&lm6));

    QList<QLandmarkId> lmIds3;
    lmIds3 << lm4.landmarkId() << lm5.landmarkId() << lm6.landmarkId();

    QVERIFY(m_manager->removeLandmarks(lmIds3, &errors));
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QCOMPARE(errors.count(), 3);
    QCOMPARE(errors.value(0), QLandmarkManager::NoError);
    QCOMPARE(errors.value(1), QLandmarkManager::NoError);
    QCOMPARE(errors.value(2), QLandmarkManager::NoError);

    for (int i = 0; i < lmIds3.size(); ++i) {
        QCOMPARE(m_manager->landmark(lmIds3.at(i)).landmarkId().isValid(), false);
    }
}

void tst_QLandmarkManagerEngineSqlite::removeMultipleLandmarksAsync() {
    //TODO: notifications
    //QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));
    this->disconnectNotifications();
    QList<QLandmarkId> lmIds1;
    QList<QLandmarkId> lmIds2;

    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));
    lmIds1 << lm1.landmarkId();

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));
    lmIds1 << lm2.landmarkId();

    QLandmark lm3;
    lm3.setName("LM3");
    QVERIFY(m_manager->saveLandmark(&lm3));
    lmIds2 << lm3.landmarkId();

    QString uri = m_manager->managerUri();
    int i = 1;

    QList<QLandmarkId> invalidLmIds;

    while (invalidLmIds.size() < 3) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setLocalId(QString::number(i));
        QLandmark lm = m_manager->landmark(id);
        if (!lm.landmarkId().isValid()) {
            invalidLmIds << id;
        }
        ++i;
    }

    lmIds1.insert(1, invalidLmIds.at(0));
    lmIds2.insert(0, invalidLmIds.at(1));
    lmIds2.insert(2, invalidLmIds.at(2));

    QCOMPARE(m_manager->landmark(lmIds1.at(0)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds1.at(1)).landmarkId().isValid(), false);
    QCOMPARE(m_manager->landmark(lmIds1.at(2)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds2.at(0)).landmarkId().isValid(), false);
    QCOMPARE(m_manager->landmark(lmIds2.at(1)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds2.at(2)).landmarkId().isValid(), false);

    QLandmarkRemoveRequest removeRequest(m_manager);
    QSignalSpy spy(&removeRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    removeRequest.setLandmarkIds(lmIds1);
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(removeRequest.errorMap().count(),3);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);
    QCOMPARE(removeRequest.errorMap().value(1), QLandmarkManager::DoesNotExistError);
    QCOMPARE(removeRequest.errorMap().value(2), QLandmarkManager::NoError);


    //TODO: notifications
    //QCOMPARE(spyRemove.count(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().size(), 2);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.landmarkId());
    //spyRemove.clear();

    for (int i = 0; i < lmIds1.size(); ++i)
        QCOMPARE(m_manager->landmark(lmIds1.at(i)).landmarkId().isValid(), false);

    QCOMPARE(m_manager->landmark(lmIds2.at(0)).landmarkId().isValid(), false);
    QCOMPARE(m_manager->landmark(lmIds2.at(1)).landmarkId().isValid(), true);
    QCOMPARE(m_manager->landmark(lmIds2.at(2)).landmarkId().isValid(), false);

    removeRequest.setLandmarkIds(lmIds2);
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest, QLandmarkManager::DoesNotExistError));
    QCOMPARE(removeRequest.errorMap().count(),3);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::DoesNotExistError);
    QCOMPARE(removeRequest.errorMap().value(1), QLandmarkManager::NoError);
    QCOMPARE(removeRequest.errorMap().value(2), QLandmarkManager::DoesNotExistError);

    //TODO: Notifications
    //QCOMPARE(spyRemove.count(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().size(), 1);
    //QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm3.landmarkId());
    //spyRemove.clear();

    for (int i = 0; i < lmIds1.size(); ++i)
        QCOMPARE(m_manager->landmark(lmIds1.at(i)).landmarkId().isValid(), false);
    for (int i = 0; i < lmIds2.size(); ++i)
        QCOMPARE(m_manager->landmark(lmIds2.at(i)).landmarkId().isValid(), false);

    //test all ids to remove were valid existing ids
    QLandmark lm4;
    lm4.setName("LM4");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("LM5");
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmark lm6;
    lm3.setName("LM6");
    QVERIFY(m_manager->saveLandmark(&lm6));

    QList<QLandmarkId> lmIds3;
    lmIds3 << lm4.landmarkId() << lm5.landmarkId() << lm6.landmarkId();

    removeRequest.setLandmarkIds(lmIds3);
    removeRequest.start();

    QVERIFY(waitForAsync(spy, &removeRequest));
    QCOMPARE(removeRequest.errorMap().count(),3);
    QCOMPARE(removeRequest.errorMap().value(0), QLandmarkManager::NoError);
    QCOMPARE(removeRequest.errorMap().value(1), QLandmarkManager::NoError);
    QCOMPARE(removeRequest.errorMap().value(2), QLandmarkManager::NoError);

    for (int i = 0; i < lmIds3.size(); ++i) {
        QCOMPARE(m_manager->landmark(lmIds3.at(i)).landmarkId().isValid(), false);
    }

    //test canceling the remove request
    QList<QLandmark> landmarks;
    landmarks.clear();
    QLandmark lm;
    for (int i =0; i < 1000; i++) {
        lm.clear();
        lm.setName(QString("LM") + QString::number(i));
        landmarks.append(lm);
    }

    QVERIFY(m_manager->saveLandmarks(&landmarks));

    QList<QLandmarkId> lmIds;
    for (int i=0; i < landmarks.count(); ++i) {
        lmIds.append(landmarks.at(i).landmarkId());
    }

    removeRequest.setLandmarkIds(lmIds);
    removeRequest.start();
    QTest::qWait(50);
    removeRequest.cancel();
    QVERIFY(waitForAsync(spy, &removeRequest,QLandmarkManager::CancelError));
    bool foundCancelError = false;
    for (int i=0; i < removeRequest.landmarkIds().count(); ++i) {
        if (removeRequest.errorMap().value(i) == QLandmarkManager::CancelError) {
            foundCancelError = true;
            for (int j=i; j < removeRequest.landmarkIds().count(); ++j) {
                QVERIFY(removeRequest.errorMap().value(j) == QLandmarkManager::CancelError);
            }
        } else {
            QVERIFY(removeRequest.errorMap().value(i) == QLandmarkManager::NoError);
        }
    }
    QVERIFY(foundCancelError);
    this->connectNotifications();
}

void tst_QLandmarkManagerEngineSqlite::listCategoryIds() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QList<QLandmarkCategoryId> ids = m_manager->categoryIds();

    QCOMPARE(ids.size(), 3);
    QCOMPARE(ids.at(0), cat1.categoryId());
    QCOMPARE(ids.at(1), cat2.categoryId());
    QCOMPARE(ids.at(2), cat3.categoryId());
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksLimitMatches() {
    for (int i = 0; i < 50; ++i) {
        QLandmark lm;
        lm.setName(QString("LM%1").arg(i));
        QVERIFY(m_manager->saveLandmark(&lm));
    }

    QList<QLandmarkId> ids = m_manager->landmarkIds();
    QCOMPARE(ids.size(), 50);

    //default
    QLandmarkFilter filter;
    QLandmarkFetchHint fetchHint;
    QLandmarkSortOrder sortOrder;

    fetchHint.setMaxItems(100);
    ids = m_manager->landmarkIds(filter, sortOrder, fetchHint);

    QCOMPARE(ids.size(), 50);

    QLandmarkNameSort nameSort(Qt::DescendingOrder);
    fetchHint.setMaxItems(25);
    ids = m_manager->landmarkIds(filter, nameSort, fetchHint);

    QCOMPARE(ids.size(), 25);
    QCOMPARE(m_manager->landmark(ids.at(0)).name(), QString("LM9"));
    QCOMPARE(m_manager->landmark(ids.at(24)).name(), QString("LM31"));

    QList<QLandmark> lms = m_manager->landmarks(filter, nameSort, fetchHint);
    QCOMPARE(lms.size(), 25);
    QCOMPARE(lms.at(0).name(), QString("LM9"));
    QCOMPARE(lms.at(24).name(), QString("LM31"));

    //try with an offset and max items
    fetchHint.setOffset(10);
    fetchHint.setMaxItems(10);
    lms = m_manager->landmarks(filter, nameSort, fetchHint);
    ids = m_manager->landmarkIds(filter, nameSort, fetchHint);
    QVERIFY(checkIds(lms, ids));

    QCOMPARE(lms.size(), 10);
    QCOMPARE(lms.at(0).name(), QString("LM44"));
    QCOMPARE(lms.at(9).name(), QString("LM36"));

    //try with an offset and no max items
    fetchHint.setMaxItems(-1);
    fetchHint.setOffset(10);
    lms = m_manager->landmarks(filter,QLandmarkNameSort(Qt::AscendingOrder), fetchHint);
    ids = m_manager->landmarkIds(filter, QLandmarkNameSort(Qt::AscendingOrder), fetchHint);
    QVERIFY(checkIds(lms, ids));

    QCOMPARE(lms.size(), 40);
    QCOMPARE(lms.at(0).name(), QString("LM18"));
    QCOMPARE(lms.at(39).name(), QString("LM9"));

    //try with an offset of -1
    fetchHint.setMaxItems(-1);
    fetchHint.setOffset(-1);
    lms = m_manager->landmarks(filter, QLandmarkNameSort(Qt::AscendingOrder), fetchHint);
    ids = m_manager->landmarkIds(filter, QLandmarkNameSort(Qt::AscendingOrder), fetchHint);
    QVERIFY(checkIds(lms, ids));

    QCOMPARE(lms.count(), 50);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksLimitMatchesAsync() {
    for (int i = 0; i < 50; ++i) {
        QLandmark lm;
        lm.setName(QString("LM%1").arg(i));
        QVERIFY(m_manager->saveLandmark(&lm));
    }

    QLandmarkFetchRequest fetchRequest(m_manager);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark> lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, fetchRequest.filter()));

    QCOMPARE(lms.size(), 50);

    QLandmarkFetchHint fetchHint;
    fetchHint.setMaxItems(100);
    fetchRequest.setFetchHint(fetchHint);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, fetchRequest.filter(), fetchRequest.sorting(), fetchRequest.fetchHint()));
    QCOMPARE(lms.size(), 50);

    //try with a sort order and limit
    QLandmarkNameSort nameSort(Qt::DescendingOrder);
    fetchHint.setMaxItems(25);
    fetchRequest.setFetchHint(fetchHint);
    fetchRequest.setSorting(nameSort);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, fetchRequest.filter(),fetchRequest.sorting(), fetchRequest.fetchHint()));
    QCOMPARE(lms.size(), 25);
    QCOMPARE(lms.at(0).name(), QString("LM9"));
    QCOMPARE(lms.at(24).name(), QString("LM31"));

    //try with an offset and max items
    fetchHint.setOffset(10);
    fetchHint.setMaxItems(10);
    fetchRequest.setFetchHint(fetchHint);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, fetchRequest.filter(), fetchRequest.sorting(),
                                fetchRequest.fetchHint()));
    QCOMPARE(lms.size(), 10);
    QCOMPARE(lms.at(0).name(), QString("LM44"));
    QCOMPARE(lms.at(9).name(), QString("LM36"));

    //try with an offset and no max items
    fetchHint.setMaxItems(-1);
    fetchHint.setOffset(10);
    fetchRequest.setFetchHint(fetchHint);
    fetchRequest.setSorting(QLandmarkNameSort(Qt::AscendingOrder));
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, fetchRequest.filter(), fetchRequest.sorting(),
                                fetchRequest.fetchHint()));
    QCOMPARE(lms.size(), 40);
    QCOMPARE(lms.at(0).name(), QString("LM18"));
    QCOMPARE(lms.at(39).name(), QString("LM9"));

    //try with an offset of -1
    fetchHint.setMaxItems(-1);
    fetchHint.setOffset(-1);
    fetchRequest.setFetchHint(fetchHint);
    fetchRequest.setSorting(QLandmarkNameSort(Qt::AscendingOrder));
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QCOMPARE(lms.count(), 50);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksDefault() {
    QLandmark lm1;
    lm1.setName("LM1");
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("LM2");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("LM3");
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmarkFilter filter;

    QList<QLandmarkId> ids = m_manager->landmarkIds(filter);

    QCOMPARE(ids.size(), 3);
    QCOMPARE(ids.at(0), lm1.landmarkId());
    QCOMPARE(ids.at(1), lm2.landmarkId());
    QCOMPARE(ids.at(2), lm3.landmarkId());
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksName() {
    QLandmark lm1;
    lm1.setName("test");
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("junk1");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("TEST");
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setName("junk2");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("tEsT");
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmark lm6;
    lm6.setName("junk3");
    QVERIFY(m_manager->saveLandmark(&lm6));

    QLandmarkNameFilter filter("TEST");
    filter.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QLandmarkId> ids1 = m_manager->landmarkIds(filter);

    QCOMPARE(ids1.size(), 3);
    QCOMPARE(ids1.at(0), lm1.landmarkId());
    QCOMPARE(ids1.at(1), lm3.landmarkId());
    QCOMPARE(ids1.at(2), lm5.landmarkId());

    filter.setCaseSensitivity(Qt::CaseSensitive);
    QList<QLandmarkId> ids2 = m_manager->landmarkIds(filter);

    QCOMPARE(ids2.size(), 1);
    QCOMPARE(ids2.at(0), lm3.landmarkId());

    filter.setName("No match");
    QList<QLandmarkId> ids3 = m_manager->landmarkIds(filter);
    QCOMPARE(ids3.size(), 0);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksNameAsync() {
    QLandmark lm1;
    lm1.setName("test");
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("junk1");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("TEST");
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setName("junk2");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("tEsT");
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmark lm6;
    lm6.setName("junk3");
    QVERIFY(m_manager->saveLandmark(&lm6));

    QLandmarkNameFilter filter("TEST");
    filter.setCaseSensitivity(Qt::CaseInsensitive);

    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(filter);

    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.start();
    QTest::qWait(1000);
    QCOMPARE(spy.count(),2);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QCOMPARE(fetchRequest.errorString(), QString());

    QList<QLandmark> lms1 = fetchRequest.landmarks();

    QCOMPARE(lms1.size(), 3);
    QCOMPARE(lms1.at(0).landmarkId(), lm1.landmarkId());
    QCOMPARE(lms1.at(1).landmarkId(), lm3.landmarkId());
    QCOMPARE(lms1.at(2).landmarkId(), lm5.landmarkId());

    QVERIFY(checkIdFetchRequest(lms1,filter));

    //do a case sensitive filter this time
    filter.setCaseSensitivity(Qt::CaseSensitive);
    fetchRequest.setFilter(filter);
    spy.clear();
    fetchRequest.start();
    QTest::qWait(1000);
    QCOMPARE(spy.count(),2);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QCOMPARE(fetchRequest.errorString(), QString());

    QList<QLandmark> lms2 = fetchRequest.landmarks();

    QCOMPARE(lms2.size(), 1);
    QCOMPARE(lms2.at(0).landmarkId(), lm3.landmarkId());

    QVERIFY(checkIdFetchRequest(lms2,filter));

    //try a filter which won't have a match
    filter.setName("No match");
    fetchRequest.setFilter(filter);
    spy.clear();
    fetchRequest.start();
    QTest::qWait(1000);
    QCOMPARE(spy.count(),2);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QCOMPARE(fetchRequest.errorString(), QString());

    QList<QLandmark> lms3 = fetchRequest.landmarks();
    QCOMPARE(lms3.size(), 0);

    QVERIFY(checkIdFetchRequest(lms3,filter));
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksProximity() {
    QList<QGeoCoordinate> greenwhichFilterCoords;
    QList<QGeoCoordinate> datelineFilterCoords;
    QList<QGeoCoordinate> northPoleFilterCoords;
    QList<QGeoCoordinate> southPoleFilterCoords;
    QList<QGeoCoordinate> northFilterCoords;
    QList<QGeoCoordinate> eastFilterCoords;
    QList<QGeoCoordinate> northeastFilterCoords;

    QList<QGeoCoordinate> greenwhichLmCoords;
    QList<QGeoCoordinate> datelineLmCoords;
    QList<QGeoCoordinate> northPoleLmCoords;
    QList<QGeoCoordinate> southPoleLmCoords;
    QList<QGeoCoordinate> northLmCoords;
    QList<QGeoCoordinate> eastLmCoords;
    QList<QGeoCoordinate> northeastLmCoords;

    greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

    datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

    /*TODO: test poles
        northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
        northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
        northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
        northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

        southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
        southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
        southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
        southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);
*/

    eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
    eastFilterCoords << QGeoCoordinate(0.1, 10.0);
    northFilterCoords << QGeoCoordinate(10.0, -0.1);
    northFilterCoords << QGeoCoordinate(10.0, 0.1);
    northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

    greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

    datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
    datelineLmCoords << QGeoCoordinate(1.0, 179.0);

    northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
    northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

    southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

    eastLmCoords << QGeoCoordinate(-1.0, 11.0);
    eastLmCoords << QGeoCoordinate(1.0, 11.0);
    northLmCoords << QGeoCoordinate(11.0, -1.0);
    northLmCoords << QGeoCoordinate(11.0, 1.0);
    northeastLmCoords << QGeoCoordinate(11.0, 11.0);

    QList<QList<QGeoCoordinate> > coords;
    coords << greenwhichLmCoords;
    coords << datelineLmCoords;
    //TODO: test poles
    //        coords << northPoleLmCoords;
    //        coords << southPoleLmCoords;
    coords << eastLmCoords;
    coords << northLmCoords;
    coords << northeastLmCoords;

    for (int i = 0; i < coords.size(); ++i) {
        QList<QGeoCoordinate> c = coords.at(i);
        for (int j = 0; j < c.size(); ++j) {
            QLandmark lm;
            lm.setCoordinate(c.at(j));
            QVERIFY(m_manager->saveLandmark(&lm));
        }
    }

    QList<QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> > > testSets;
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(greenwhichFilterCoords, greenwhichLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(datelineFilterCoords, datelineLmCoords);
    //TODO: test poles
    //        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northPoleFilterCoords, northPoleLmCoords);
    //        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(southPoleFilterCoords, southPoleLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northFilterCoords, northLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(eastFilterCoords, eastLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northeastFilterCoords, northeastLmCoords);

    double dist = QGeoCoordinate(0.0, 0.0).distanceTo(QGeoCoordinate(5.0, 5.0));

    for (int i = 0; i < testSets.size(); ++i) {
        QList<QGeoCoordinate> filterCoords = testSets.at(i).first;
        QList<QGeoCoordinate> lmCoords = testSets.at(i).second;

        for (int j = 0; j < filterCoords.size(); ++j) {
            QLandmarkProximityFilter filter(filterCoords.at(j), dist);

            QList<QLandmark> lms = m_manager->landmarks(filter);

            if (lms.size() != lmCoords.size()) {
                for (int k = 0; k < lms.size(); ++k)
                    qWarning() << "lms" << lms.at(k).coordinate().toString();
                for (int k = 0; k < lmCoords.size(); ++k)
                    qWarning() << "lmCoords" << lmCoords.at(k).toString();
            }

            QCOMPARE(lms.size(), lmCoords.size());

            for (int k = 0; k < lms.size(); ++k) {
                QVERIFY(lmCoords.contains(lms.at(k).coordinate()));
            }
        }
    }
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksProximityAsync() {
    QList<QGeoCoordinate> greenwhichFilterCoords;
    QList<QGeoCoordinate> datelineFilterCoords;
    QList<QGeoCoordinate> northPoleFilterCoords;
    QList<QGeoCoordinate> southPoleFilterCoords;
    QList<QGeoCoordinate> northFilterCoords;
    QList<QGeoCoordinate> eastFilterCoords;
    QList<QGeoCoordinate> northeastFilterCoords;

    QList<QGeoCoordinate> greenwhichLmCoords;
    QList<QGeoCoordinate> datelineLmCoords;
    QList<QGeoCoordinate> northPoleLmCoords;
    QList<QGeoCoordinate> southPoleLmCoords;
    QList<QGeoCoordinate> northLmCoords;
    QList<QGeoCoordinate> eastLmCoords;
    QList<QGeoCoordinate> northeastLmCoords;

    greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

    datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

    northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
    northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

    southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
    southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

    eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
    eastFilterCoords << QGeoCoordinate(0.1, 10.0);
    northFilterCoords << QGeoCoordinate(10.0, -0.1);
    northFilterCoords << QGeoCoordinate(10.0, 0.1);
    northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

    greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

    datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
    datelineLmCoords << QGeoCoordinate(1.0, 179.0);


    //TODO: test poles
    //northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
    //northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
    //northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
    //northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

    //southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
    //southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
    //southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
    //southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

    eastLmCoords << QGeoCoordinate(-1.0, 11.0);
    eastLmCoords << QGeoCoordinate(1.0, 11.0);
    northLmCoords << QGeoCoordinate(11.0, -1.0);
    northLmCoords << QGeoCoordinate(11.0, 1.0);
    northeastLmCoords << QGeoCoordinate(11.0, 11.0);

    QList<QList<QGeoCoordinate> > coords;
    coords << greenwhichLmCoords;
    coords << datelineLmCoords;
    coords << northPoleLmCoords;
    coords << southPoleLmCoords;
    coords << eastLmCoords;
    coords << northLmCoords;
    coords << northeastLmCoords;

    for (int i = 0; i < coords.size(); ++i) {
        QList<QGeoCoordinate> c = coords.at(i);
        for (int j = 0; j < c.size(); ++j) {
            QLandmark lm;
            lm.setCoordinate(c.at(j));
            QVERIFY(m_manager->saveLandmark(&lm));
        }
    }

    QList<QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> > > testSets;
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(greenwhichFilterCoords, greenwhichLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(datelineFilterCoords, datelineLmCoords);
    //TODO: test poles
    //testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northPoleFilterCoords, northPoleLmCoords);
    //testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(southPoleFilterCoords, southPoleLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northFilterCoords, northLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(eastFilterCoords, eastLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northeastFilterCoords, northeastLmCoords);

    double dist = QGeoCoordinate(0.0, 0.0).distanceTo(QGeoCoordinate(5.0, 5.0));

    for (int i = 0; i < testSets.size(); ++i) {
        QList<QGeoCoordinate> filterCoords = testSets.at(i).first;
        QList<QGeoCoordinate> lmCoords = testSets.at(i).second;

        for (int j = 0; j < filterCoords.size(); ++j) {
            QLandmarkProximityFilter filter(filterCoords.at(j), dist);
            QLandmarkFetchRequest fetchRequest(m_manager);
            fetchRequest.setFilter(filter);
            QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
            QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::InactiveState);
            fetchRequest.start();

            QVERIFY(waitForAsync(spy, &fetchRequest));
            QList<QLandmark>lms = fetchRequest.landmarks();

            QVERIFY(checkIdFetchRequest(lms,filter));

            if (lms.size() != lmCoords.size()) {
                for (int k = 0; k < lms.size(); ++k)
                    qWarning() << "lms" << lms.at(k).coordinate().toString();
                for (int k = 0; k < lmCoords.size(); ++k)
                    qWarning() << "lmCoords" << lmCoords.at(k).toString();
            }

            QCOMPARE(lms.size(), lmCoords.size());

            for (int k = 0; k < lms.size(); ++k) {
                QVERIFY(lmCoords.contains(lms.at(k).coordinate()));
            }
        }
    }
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksNearest() {
    QList<QGeoCoordinate> greenwhichFilterCoords;
    QList<QGeoCoordinate> datelineFilterCoords;
    QList<QGeoCoordinate> northPoleFilterCoords;
    QList<QGeoCoordinate> southPoleFilterCoords;
    QList<QGeoCoordinate> northFilterCoords;
    QList<QGeoCoordinate> eastFilterCoords;
    QList<QGeoCoordinate> northeastFilterCoords;

    QList<QGeoCoordinate> greenwhichLmCoords;
    QList<QGeoCoordinate> datelineLmCoords;
    QList<QGeoCoordinate> northPoleLmCoords;
    QList<QGeoCoordinate> southPoleLmCoords;
    QList<QGeoCoordinate> northLmCoords;
    QList<QGeoCoordinate> eastLmCoords;
    QList<QGeoCoordinate> northeastLmCoords;

    greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

    datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

    northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
    northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

    southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
    southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

    eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
    eastFilterCoords << QGeoCoordinate(0.1, 10.0);
    northFilterCoords << QGeoCoordinate(10.0, -0.1);
    northFilterCoords << QGeoCoordinate(10.0, 0.1);
    northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

    greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

    datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
    datelineLmCoords << QGeoCoordinate(1.0, 179.0);

    northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
    northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

    southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

    eastLmCoords << QGeoCoordinate(-1.0, 11.0);
    eastLmCoords << QGeoCoordinate(1.0, 11.0);
    northLmCoords << QGeoCoordinate(11.0, -1.0);
    northLmCoords << QGeoCoordinate(11.0, 1.0);
    northeastLmCoords << QGeoCoordinate(11.0, 11.0);

    QList<QList<QGeoCoordinate> > coords;
    coords << greenwhichLmCoords;
    coords << datelineLmCoords;
    coords << northPoleLmCoords;
    coords << southPoleLmCoords;
    coords << eastLmCoords;
    coords << northLmCoords;
    coords << northeastLmCoords;

    for (int i = 0; i < coords.size(); ++i) {
        QList<QGeoCoordinate> c = coords.at(i);
        for (int j = 0; j < c.size(); ++j) {
            QLandmark lm;
            lm.setCoordinate(c.at(j));
            QVERIFY(m_manager->saveLandmark(&lm));
        }
    }

    QList<QPair<QGeoCoordinate, QGeoCoordinate> > testSets;

    for (int i = 0; i < greenwhichFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(greenwhichFilterCoords.at(i), greenwhichLmCoords.at(i));
    }

    for (int i = 0; i < datelineFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(datelineFilterCoords.at(i), datelineLmCoords.at(i));
    }

    for (int i = 0; i < northPoleFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northPoleFilterCoords.at(i), northPoleLmCoords.at(i));
    }

    for (int i = 0; i < southPoleFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(southPoleFilterCoords.at(i), southPoleLmCoords.at(i));
    }

    for (int i = 0; i < northFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northFilterCoords.at(i), northLmCoords.at(i));
    }

    for (int i = 0; i < eastFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(eastFilterCoords.at(i), eastLmCoords.at(i));
    }

    for (int i = 0; i < northeastFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northeastFilterCoords.at(i), northeastLmCoords.at(i));
    }

    for (int i = 0; i < testSets.size(); ++i) {
        QGeoCoordinate filterCoord = testSets.at(i).first;
        QGeoCoordinate lmCoord = testSets.at(i).second;

        QLandmarkProximityFilter filter(filterCoord);
        filter.setSelection(QLandmarkProximityFilter::SelectNearestOnly);

        QList<QLandmark> lms = m_manager->landmarks(filter);

        QCOMPARE(lms.size(), 1);

        QCOMPARE(lms.at(0).coordinate(), lmCoord);
    }

    QLandmarkProximityFilter filter1(QGeoCoordinate(-10.0, -10.0), -1.0);
    filter1.setSelection(QLandmarkProximityFilter::SelectNearestOnly);

    QList<QLandmark> lms = m_manager->landmarks(filter1);
    QCOMPARE(lms.size(), 1);
    QCOMPARE(lms.at(0).coordinate(), QGeoCoordinate(-1.0, -1.0));

    QLandmarkProximityFilter filter2(QGeoCoordinate(-10.0, -10.0), 100.0);
    filter2.setSelection(QLandmarkProximityFilter::SelectNearestOnly);

    lms = m_manager->landmarks(filter2);
    QCOMPARE(lms.size(), 0);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksNearestAsync() {
    QList<QGeoCoordinate> greenwhichFilterCoords;
    QList<QGeoCoordinate> datelineFilterCoords;
    QList<QGeoCoordinate> northPoleFilterCoords;
    QList<QGeoCoordinate> southPoleFilterCoords;
    QList<QGeoCoordinate> northFilterCoords;
    QList<QGeoCoordinate> eastFilterCoords;
    QList<QGeoCoordinate> northeastFilterCoords;

    QList<QGeoCoordinate> greenwhichLmCoords;
    QList<QGeoCoordinate> datelineLmCoords;
    QList<QGeoCoordinate> northPoleLmCoords;
    QList<QGeoCoordinate> southPoleLmCoords;
    QList<QGeoCoordinate> northLmCoords;
    QList<QGeoCoordinate> eastLmCoords;
    QList<QGeoCoordinate> northeastLmCoords;

    greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

    datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

    northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
    northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

    southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
    southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

    eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
    eastFilterCoords << QGeoCoordinate(0.1, 10.0);
    northFilterCoords << QGeoCoordinate(10.0, -0.1);
    northFilterCoords << QGeoCoordinate(10.0, 0.1);
    northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

    greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

    datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
    datelineLmCoords << QGeoCoordinate(1.0, 179.0);

    northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
    northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

    southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

    eastLmCoords << QGeoCoordinate(-1.0, 11.0);
    eastLmCoords << QGeoCoordinate(1.0, 11.0);
    northLmCoords << QGeoCoordinate(11.0, -1.0);
    northLmCoords << QGeoCoordinate(11.0, 1.0);
    northeastLmCoords << QGeoCoordinate(11.0, 11.0);

    QList<QList<QGeoCoordinate> > coords;
    coords << greenwhichLmCoords;
    coords << datelineLmCoords;
    coords << northPoleLmCoords;
    coords << southPoleLmCoords;
    coords << eastLmCoords;
    coords << northLmCoords;
    coords << northeastLmCoords;

    for (int i = 0; i < coords.size(); ++i) {
        QList<QGeoCoordinate> c = coords.at(i);
        for (int j = 0; j < c.size(); ++j) {
            QLandmark lm;
            lm.setCoordinate(c.at(j));
            QVERIFY(m_manager->saveLandmark(&lm));
        }
    }

    QList<QPair<QGeoCoordinate, QGeoCoordinate> > testSets;

    for (int i = 0; i < greenwhichFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(greenwhichFilterCoords.at(i), greenwhichLmCoords.at(i));
    }

    for (int i = 0; i < datelineFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(datelineFilterCoords.at(i), datelineLmCoords.at(i));
    }

    for (int i = 0; i < northPoleFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northPoleFilterCoords.at(i), northPoleLmCoords.at(i));
    }

    for (int i = 0; i < southPoleFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(southPoleFilterCoords.at(i), southPoleLmCoords.at(i));
    }

    for (int i = 0; i < northFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northFilterCoords.at(i), northLmCoords.at(i));
    }

    for (int i = 0; i < eastFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(eastFilterCoords.at(i), eastLmCoords.at(i));
    }

    for (int i = 0; i < northeastFilterCoords.size(); ++i) {
        testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northeastFilterCoords.at(i), northeastLmCoords.at(i));
    }

    for (int i = 0; i < testSets.size(); ++i) {
        QGeoCoordinate filterCoord = testSets.at(i).first;
        QGeoCoordinate lmCoord = testSets.at(i).second;
        QLandmarkProximityFilter filter(filterCoord);
        filter.setSelection(QLandmarkProximityFilter::SelectNearestOnly);
        QLandmarkFetchRequest fetchRequest(m_manager);
        fetchRequest.setFilter(filter);
        QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
        fetchRequest.start();

        QVERIFY(waitForAsync(spy, &fetchRequest));
        QList<QLandmark> lms = fetchRequest.landmarks();

        QVERIFY(checkIdFetchRequest(lms,filter));

        QCOMPARE(lms.size(), 1);
        QCOMPARE(lms.at(0).coordinate(), lmCoord);
    }

    QLandmarkProximityFilter filter1(QGeoCoordinate(-10.0, -10.0), -1.0);
    filter1.setSelection(QLandmarkProximityFilter::SelectNearestOnly);

    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(filter1);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark> lms = fetchRequest.landmarks();

    QVERIFY(checkIdFetchRequest(lms,filter1));

    QCOMPARE(lms.size(), 1);
    QCOMPARE(lms.at(0).coordinate(), QGeoCoordinate(-1.0, -1.0));

    QLandmarkProximityFilter filter2(QGeoCoordinate(-10.0, -10.0), 100.0);
    filter2.setSelection(QLandmarkProximityFilter::SelectNearestOnly);
    fetchRequest.setFilter(filter2);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();

    QVERIFY(checkIdFetchRequest(lms,filter2));

    QCOMPARE(lms.size(), 0);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksCategory() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm1;
    lm1.setName("LM1");
    lm1.addCategoryId(cat1.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("LM2");
    lm2.addCategoryId(cat1.categoryId());
    lm2.addCategoryId(cat2.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(cat1.categoryId());
    lm3.addCategoryId(cat2.categoryId());
    lm3.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setName("LM4");
    lm4.addCategoryId(cat2.categoryId());
    lm4.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("LM5");
    lm5.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmarkCategoryFilter filter(cat2.categoryId());

    QList<QLandmark> lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 3);

    QSet<QString> names;
    for (int i = 0; i < lms.size(); ++i) {
        names.insert(lms.at(i).name());
    }

    QSet<QString> expectedNames;
    expectedNames.insert("LM2");
    expectedNames.insert("LM3");
    expectedNames.insert("LM4");

    QCOMPARE(names, expectedNames);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksCategoryAsync() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm1;
    lm1.setName("LM1");
    lm1.addCategoryId(cat1.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("LM2");
    lm2.addCategoryId(cat1.categoryId());
    lm2.addCategoryId(cat2.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(cat1.categoryId());
    lm3.addCategoryId(cat2.categoryId());
    lm3.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setName("LM4");
    lm4.addCategoryId(cat2.categoryId());
    lm4.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("LM5");
    lm5.addCategoryId(cat3.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmarkCategoryFilter filter(cat2.categoryId());
    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(filter);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark> lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter));

    QCOMPARE(lms.size(), 3);

    QSet<QString> names;
    for (int i = 0; i < lms.size(); ++i) {
        names.insert(lms.at(i).name());
    }

    QSet<QString> expectedNames;
    expectedNames.insert("LM2");
    expectedNames.insert("LM3");
    expectedNames.insert("LM4");

    QCOMPARE(names, expectedNames);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksBox() {
    QList<QGeoCoordinate> outBox;

    QList<QGeoCoordinate> inBox1;

    inBox1 << QGeoCoordinate(-5.0, -5.0);
    inBox1 << QGeoCoordinate(-5.0, 0.0);
    inBox1 << QGeoCoordinate(-5.0, 5.0);
    inBox1 << QGeoCoordinate(0.0, -5.0);
    inBox1 << QGeoCoordinate(0.0, 0.0);
    inBox1 << QGeoCoordinate(0.0, 5.0);
    inBox1 << QGeoCoordinate(5.0, -5.0);
    inBox1 << QGeoCoordinate(5.0, 0.0);
    inBox1 << QGeoCoordinate(5.0, 5.0);

    outBox << QGeoCoordinate(-5.0, -6.0);
    outBox << QGeoCoordinate(5.0, -6.0);
    outBox << QGeoCoordinate(-6.0, -5.0);
    outBox << QGeoCoordinate(6.0, -5.0);
    outBox << QGeoCoordinate(-6.0, 5.0);
    outBox << QGeoCoordinate(6.0, 5.0);
    outBox << QGeoCoordinate(-5.0, 6.0);
    outBox << QGeoCoordinate(5.0, 6.0);

    QList<QGeoCoordinate> inBox2;

    inBox2 << QGeoCoordinate(-5.0, 10.0);
    inBox2 << QGeoCoordinate(-5.0, 15.0);
    inBox2 << QGeoCoordinate(-5.0, 20.0);
    inBox2 << QGeoCoordinate(0.0, 10.0);
    inBox2 << QGeoCoordinate(0.0, 15.0);
    inBox2 << QGeoCoordinate(0.0, 20.0);
    inBox2 << QGeoCoordinate(5.0, 10.0);
    inBox2 << QGeoCoordinate(5.0, 15.0);
    inBox2 << QGeoCoordinate(5.0, 20.0);

    outBox << QGeoCoordinate(-5.0, 9.0);
    outBox << QGeoCoordinate(5.0,  9.0);
    outBox << QGeoCoordinate(-6.0, 10.0);
    outBox << QGeoCoordinate(6.0,  10.0);
    outBox << QGeoCoordinate(-6.0, 20.0);
    outBox << QGeoCoordinate(6.0, 20.0);
    outBox << QGeoCoordinate(-5.0, 21.0);
    outBox << QGeoCoordinate(5.0, 21.0);

    QList<QGeoCoordinate> inBox3;

    inBox3 << QGeoCoordinate(10.0, -5.0);
    inBox3 << QGeoCoordinate(10.0, 0.0);
    inBox3 << QGeoCoordinate(10.0, 5.0);
    inBox3 << QGeoCoordinate(15.0, -5.0);
    inBox3 << QGeoCoordinate(15.0, 0.0);
    inBox3 << QGeoCoordinate(15.0, 5.0);
    inBox3 << QGeoCoordinate(20.0, -5.0);
    inBox3 << QGeoCoordinate(20.0, 0.0);
    inBox3 << QGeoCoordinate(20.0, 5.0);

    outBox << QGeoCoordinate(10.0, -6.0);
    outBox << QGeoCoordinate(20.0, -6.0);
    outBox << QGeoCoordinate(9.0, -5.0);
    outBox << QGeoCoordinate(21.0, -5.0);
    outBox << QGeoCoordinate(9.0, 5.0);
    outBox << QGeoCoordinate(21.0, 5.0);
    outBox << QGeoCoordinate(10.0, 6.0);
    outBox << QGeoCoordinate(20.0, 6.0);

    QList<QGeoCoordinate> inBox4;

    inBox4 << QGeoCoordinate(10.0, 10.0);
    inBox4 << QGeoCoordinate(10.0, 15.0);
    inBox4 << QGeoCoordinate(10.0, 20.0);
    inBox4 << QGeoCoordinate(15.0, 10.0);
    inBox4 << QGeoCoordinate(15.0, 15.0);
    inBox4 << QGeoCoordinate(15.0, 20.0);
    inBox4 << QGeoCoordinate(20.0, 10.0);
    inBox4 << QGeoCoordinate(20.0, 15.0);
    inBox4 << QGeoCoordinate(20.0, 20.0);

    outBox << QGeoCoordinate(10.0, 9.0);
    outBox << QGeoCoordinate(20.0, 9.0);
    outBox << QGeoCoordinate(9.0, 10.0);
    outBox << QGeoCoordinate(21.0, 10.0);
    outBox << QGeoCoordinate(9.0, 20.0);
    outBox << QGeoCoordinate(21.0, 20.0);
    outBox << QGeoCoordinate(10.0, 21.0);
    outBox << QGeoCoordinate(20.0, 21.0);

    QList<QGeoCoordinate> inBox5;

    inBox5 << QGeoCoordinate(-5.0, 175.0);
    inBox5 << QGeoCoordinate(-5.0, 180.0);
    inBox5 << QGeoCoordinate(-5.0, -175.0);
    inBox5 << QGeoCoordinate(0.0, 175.0);
    inBox5 << QGeoCoordinate(0.0, 180.0);
    inBox5 << QGeoCoordinate(0.0, -175.0);
    inBox5 << QGeoCoordinate(5.0, 175.0);
    inBox5 << QGeoCoordinate(5.0, 180.0);
    inBox5 << QGeoCoordinate(5.0, -175.0);

    outBox << QGeoCoordinate(-6.0, 175.0);
    outBox << QGeoCoordinate(-6.0, -175.0);
    outBox << QGeoCoordinate(-5.0, 174.0);
    outBox << QGeoCoordinate(-5.0, -174.0);
    outBox << QGeoCoordinate(5.0, 174.0);
    outBox << QGeoCoordinate(5.0, -174.0);
    outBox << QGeoCoordinate(6.0, 175.0);
    outBox << QGeoCoordinate(6.0, -175.0);

    QList<QGeoCoordinate> coords = outBox;
    coords.append(inBox1);
    coords.append(inBox2);
    coords.append(inBox3);
    coords.append(inBox4);
    coords.append(inBox5);

    for (int i = 0; i < coords.size(); ++i) {
        QLandmark lm;
        lm.setCoordinate(coords.at(i));
        QVERIFY(m_manager->saveLandmark(&lm));
    }

    QLandmarkBoxFilter filter1(QGeoCoordinate(5.0, -5.0), QGeoCoordinate(-5.0, 5.0));
    QList<QLandmark> lms1 = m_manager->landmarks(filter1);

    QCOMPARE(lms1.size(), inBox1.size());

    QSet<QString> testSet1;
    for (int i = 0; i < lms1.size(); ++i)
        testSet1.insert(lms1.at(i).coordinate().toString());

    QSet<QString> inBoxSet1;
    for (int i = 0; i < inBox1.size(); ++i)
        inBoxSet1.insert(inBox1.at(i).toString());

    QCOMPARE(testSet1, inBoxSet1);

    QLandmarkBoxFilter filter2(QGeoCoordinate(5.0, 10.0), QGeoCoordinate(-5.0, 20.0));
    QList<QLandmark> lms2 = m_manager->landmarks(filter2);

    QCOMPARE(lms2.size(), inBox2.size());

    QSet<QString> testSet2;
    for (int i = 0; i < lms2.size(); ++i)
        testSet2.insert(lms2.at(i).coordinate().toString());

    QSet<QString> inBoxSet2;
    for (int i = 0; i < inBox2.size(); ++i)
        inBoxSet2.insert(inBox2.at(i).toString());

    QCOMPARE(testSet2, inBoxSet2);

    QLandmarkBoxFilter filter3(QGeoCoordinate(20.0, -5.0), QGeoCoordinate(10.0, 5.0));
    QList<QLandmark> lms3 = m_manager->landmarks(filter3);

    QCOMPARE(lms3.size(), inBox3.size());

    QSet<QString> testSet3;
    for (int i = 0; i < lms3.size(); ++i)
        testSet3.insert(lms3.at(i).coordinate().toString());

    QSet<QString> inBoxSet3;
    for (int i = 0; i < inBox3.size(); ++i)
        inBoxSet3.insert(inBox3.at(i).toString());

    QCOMPARE(testSet3, inBoxSet3);

    QLandmarkBoxFilter filter4(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0, 20.0));
    QList<QLandmark> lms4 = m_manager->landmarks(filter4);

    QCOMPARE(lms4.size(), inBox4.size());

    QSet<QString> testSet4;
    for (int i = 0; i < lms4.size(); ++i)
        testSet4.insert(lms4.at(i).coordinate().toString());

    QSet<QString> inBoxSet4;
    for (int i = 0; i < inBox4.size(); ++i)
        inBoxSet4.insert(inBox4.at(i).toString());

    QCOMPARE(testSet4, inBoxSet4);

    QLandmarkBoxFilter filter5(QGeoCoordinate(5.0, 175.0), QGeoCoordinate(-5.0, -175.0));
    QList<QLandmark> lms5 = m_manager->landmarks(filter5);

    QCOMPARE(lms5.size(), inBox5.size());

    QSet<QString> testSet5;
    for (int i = 0; i < lms5.size(); ++i)
        testSet5.insert(lms5.at(i).coordinate().toString());

    QSet<QString> inBoxSet5;
    for (int i = 0; i < inBox5.size(); ++i)
        inBoxSet5.insert(inBox5.at(i).toString());

    QCOMPARE(testSet5, inBoxSet5);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksBoxAsync() {
    QList<QGeoCoordinate> outBox;

    QList<QGeoCoordinate> inBox1;

    inBox1 << QGeoCoordinate(-5.0, -5.0);
    inBox1 << QGeoCoordinate(-5.0, 0.0);
    inBox1 << QGeoCoordinate(-5.0, 5.0);
    inBox1 << QGeoCoordinate(0.0, -5.0);
    inBox1 << QGeoCoordinate(0.0, 0.0);
    inBox1 << QGeoCoordinate(0.0, 5.0);
    inBox1 << QGeoCoordinate(5.0, -5.0);
    inBox1 << QGeoCoordinate(5.0, 0.0);
    inBox1 << QGeoCoordinate(5.0, 5.0);

    outBox << QGeoCoordinate(-5.0, -6.0);
    outBox << QGeoCoordinate(5.0, -6.0);
    outBox << QGeoCoordinate(-6.0, -5.0);
    outBox << QGeoCoordinate(6.0, -5.0);
    outBox << QGeoCoordinate(-6.0, 5.0);
    outBox << QGeoCoordinate(6.0, 5.0);
    outBox << QGeoCoordinate(-5.0, 6.0);
    outBox << QGeoCoordinate(5.0, 6.0);

    QList<QGeoCoordinate> inBox2;

    inBox2 << QGeoCoordinate(-5.0, 10.0);
    inBox2 << QGeoCoordinate(-5.0, 15.0);
    inBox2 << QGeoCoordinate(-5.0, 20.0);
    inBox2 << QGeoCoordinate(0.0, 10.0);
    inBox2 << QGeoCoordinate(0.0, 15.0);
    inBox2 << QGeoCoordinate(0.0, 20.0);
    inBox2 << QGeoCoordinate(5.0, 10.0);
    inBox2 << QGeoCoordinate(5.0, 15.0);
    inBox2 << QGeoCoordinate(5.0, 20.0);

    outBox << QGeoCoordinate(-5.0, 9.0);
    outBox << QGeoCoordinate(5.0,  9.0);
    outBox << QGeoCoordinate(-6.0, 10.0);
    outBox << QGeoCoordinate(6.0,  10.0);
    outBox << QGeoCoordinate(-6.0, 20.0);
    outBox << QGeoCoordinate(6.0, 20.0);
    outBox << QGeoCoordinate(-5.0, 21.0);
    outBox << QGeoCoordinate(5.0, 21.0);

    QList<QGeoCoordinate> inBox3;

    inBox3 << QGeoCoordinate(10.0, -5.0);
    inBox3 << QGeoCoordinate(10.0, 0.0);
    inBox3 << QGeoCoordinate(10.0, 5.0);
    inBox3 << QGeoCoordinate(15.0, -5.0);
    inBox3 << QGeoCoordinate(15.0, 0.0);
    inBox3 << QGeoCoordinate(15.0, 5.0);
    inBox3 << QGeoCoordinate(20.0, -5.0);
    inBox3 << QGeoCoordinate(20.0, 0.0);
    inBox3 << QGeoCoordinate(20.0, 5.0);

    outBox << QGeoCoordinate(10.0, -6.0);
    outBox << QGeoCoordinate(20.0, -6.0);
    outBox << QGeoCoordinate(9.0, -5.0);
    outBox << QGeoCoordinate(21.0, -5.0);
    outBox << QGeoCoordinate(9.0, 5.0);
    outBox << QGeoCoordinate(21.0, 5.0);
    outBox << QGeoCoordinate(10.0, 6.0);
    outBox << QGeoCoordinate(20.0, 6.0);

    QList<QGeoCoordinate> inBox4;

    inBox4 << QGeoCoordinate(10.0, 10.0);
    inBox4 << QGeoCoordinate(10.0, 15.0);
    inBox4 << QGeoCoordinate(10.0, 20.0);
    inBox4 << QGeoCoordinate(15.0, 10.0);
    inBox4 << QGeoCoordinate(15.0, 15.0);
    inBox4 << QGeoCoordinate(15.0, 20.0);
    inBox4 << QGeoCoordinate(20.0, 10.0);
    inBox4 << QGeoCoordinate(20.0, 15.0);
    inBox4 << QGeoCoordinate(20.0, 20.0);

    outBox << QGeoCoordinate(10.0, 9.0);
    outBox << QGeoCoordinate(20.0, 9.0);
    outBox << QGeoCoordinate(9.0, 10.0);
    outBox << QGeoCoordinate(21.0, 10.0);
    outBox << QGeoCoordinate(9.0, 20.0);
    outBox << QGeoCoordinate(21.0, 20.0);
    outBox << QGeoCoordinate(10.0, 21.0);
    outBox << QGeoCoordinate(20.0, 21.0);

    QList<QGeoCoordinate> inBox5;

    inBox5 << QGeoCoordinate(-5.0, 175.0);
    inBox5 << QGeoCoordinate(-5.0, 180.0);
    inBox5 << QGeoCoordinate(-5.0, -175.0);
    inBox5 << QGeoCoordinate(0.0, 175.0);
    inBox5 << QGeoCoordinate(0.0, 180.0);
    inBox5 << QGeoCoordinate(0.0, -175.0);
    inBox5 << QGeoCoordinate(5.0, 175.0);
    inBox5 << QGeoCoordinate(5.0, 180.0);
    inBox5 << QGeoCoordinate(5.0, -175.0);

    outBox << QGeoCoordinate(-6.0, 175.0);
    outBox << QGeoCoordinate(-6.0, -175.0);
    outBox << QGeoCoordinate(-5.0, 174.0);
    outBox << QGeoCoordinate(-5.0, -174.0);
    outBox << QGeoCoordinate(5.0, 174.0);
    outBox << QGeoCoordinate(5.0, -174.0);
    outBox << QGeoCoordinate(6.0, 175.0);
    outBox << QGeoCoordinate(6.0, -175.0);

    QList<QGeoCoordinate> coords = outBox;
    coords.append(inBox1);
    coords.append(inBox2);
    coords.append(inBox3);
    coords.append(inBox4);
    coords.append(inBox5);

    for (int i = 0; i < coords.size(); ++i) {
        QLandmark lm;
        lm.setCoordinate(coords.at(i));
        QVERIFY(m_manager->saveLandmark(&lm));
    }

    QLandmarkBoxFilter filter1(QGeoCoordinate(5.0, -5.0), QGeoCoordinate(-5.0, 5.0));
    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(filter1);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::InactiveState);
    fetchRequest.start();

    QTest::qWait(500);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::FinishedState);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QVERIFY(fetchRequest.errorString().isEmpty());
    QList<QLandmark> lms1 = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms1,filter1));

    QCOMPARE(lms1.size(), inBox1.size());

    QSet<QString> testSet1;
    for (int i = 0; i < lms1.size(); ++i)
        testSet1.insert(lms1.at(i).coordinate().toString());

    QSet<QString> inBoxSet1;
    for (int i = 0; i < inBox1.size(); ++i)
        inBoxSet1.insert(inBox1.at(i).toString());

    QCOMPARE(testSet1, inBoxSet1);

    QLandmarkBoxFilter filter2(QGeoCoordinate(5.0, 10.0), QGeoCoordinate(-5.0, 20.0));
    fetchRequest.setFilter(filter2);
    fetchRequest.start();

    QTest::qWait(500);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::FinishedState);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QVERIFY(fetchRequest.errorString().isEmpty());

    QList<QLandmark> lms2 = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms2,filter2));

    QCOMPARE(lms2.size(), inBox2.size());

    QSet<QString> testSet2;
    for (int i = 0; i < lms2.size(); ++i)
        testSet2.insert(lms2.at(i).coordinate().toString());

    QSet<QString> inBoxSet2;
    for (int i = 0; i < inBox2.size(); ++i)
        inBoxSet2.insert(inBox2.at(i).toString());

    QCOMPARE(testSet2, inBoxSet2);

    QLandmarkBoxFilter filter3(QGeoCoordinate(20.0, -5.0), QGeoCoordinate(10.0, 5.0));
    fetchRequest.setFilter(filter3);
    fetchRequest.start();

    QTest::qWait(500);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::FinishedState);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QVERIFY(fetchRequest.errorString().isEmpty());

    QList<QLandmark> lms3 = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms3,filter3));

    QCOMPARE(lms3.size(), inBox3.size());

    QSet<QString> testSet3;
    for (int i = 0; i < lms3.size(); ++i)
        testSet3.insert(lms3.at(i).coordinate().toString());

    QSet<QString> inBoxSet3;
    for (int i = 0; i < inBox3.size(); ++i)
        inBoxSet3.insert(inBox3.at(i).toString());

    QCOMPARE(testSet3, inBoxSet3);

    QLandmarkBoxFilter filter4(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0, 20.0));
    fetchRequest.setFilter(filter4);
    fetchRequest.start();

    QTest::qWait(500);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::FinishedState);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QVERIFY(fetchRequest.errorString().isEmpty());

    QList<QLandmark> lms4 = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms4,filter4));
    QCOMPARE(lms4.size(), inBox4.size());

    QSet<QString> testSet4;
    for (int i = 0; i < lms4.size(); ++i)
        testSet4.insert(lms4.at(i).coordinate().toString());

    QSet<QString> inBoxSet4;
    for (int i = 0; i < inBox4.size(); ++i)
        inBoxSet4.insert(inBox4.at(i).toString());

    QCOMPARE(testSet4, inBoxSet4);

    QLandmarkBoxFilter filter5(QGeoCoordinate(5.0, 175.0), QGeoCoordinate(-5.0, -175.0));
    fetchRequest.setFilter(filter5);
    fetchRequest.start();

    QTest::qWait(500);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::FinishedState);
    QCOMPARE(fetchRequest.error(), QLandmarkManager::NoError);
    QVERIFY(fetchRequest.errorString().isEmpty());

    QList<QLandmark> lms5 = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms5, filter5));
    QCOMPARE(lms5.size(), inBox5.size());

    QSet<QString> testSet5;
    for (int i = 0; i < lms5.size(); ++i)
        testSet5.insert(lms5.at(i).coordinate().toString());

    QSet<QString> inBoxSet5;
    for (int i = 0; i < inBox5.size(); ++i)
        inBoxSet5.insert(inBox5.at(i).toString());

    QCOMPARE(testSet5, inBoxSet5);
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksIntersection() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QList<QLandmarkCategoryId> ids;
    ids << cat1.categoryId();
    ids << cat2.categoryId();
    ids << cat3.categoryId();

    QList<QString> names;
    names << "LM1";
    names << "LM2";
    names << "LM3";

    QList<QGeoCoordinate> coords;
    coords << QGeoCoordinate(0.0, 0.0);
    coords << QGeoCoordinate(25.0, 25.0);
    coords << QGeoCoordinate(50.0, 50.0);

    QList<QLandmark> lmPool;

    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < names.size(); ++j) {
            for (int k = 0; k < coords.size(); ++k) {
                QLandmark lm;
                lm.addCategoryId(ids.at(i));
                lm.setName(names.at(j));
                lm.setCoordinate(coords.at(k));
                QVERIFY(m_manager->saveLandmark(&lm));
                lmPool << lm;
            }
        }
    }

    QLandmarkCategoryFilter f1(cat2.categoryId());
    QLandmarkNameFilter f2("LM2");
    QLandmarkProximityFilter f3(QGeoCoordinate(25.0, 25.0), 5.0);

    QLandmarkIntersectionFilter filter;
    filter << f1 << f2 << f3;

    QList<QLandmark> lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 1);

    QSet<QString> idSet;
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            && (lm.name() == "LM2")
            && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.remove(f2);

    lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 3);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.prepend(f2);

    lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 1);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            && (lm.name() == "LM2")
            && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksIntersectionAsync() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QList<QLandmarkCategoryId> ids;
    ids << cat1.categoryId();
    ids << cat2.categoryId();
    ids << cat3.categoryId();

    QList<QString> names;
    names << "LM1";
    names << "LM2";
    names << "LM3";

    QList<QGeoCoordinate> coords;
    coords << QGeoCoordinate(0.0, 0.0);
    coords << QGeoCoordinate(25.0, 25.0);
    coords << QGeoCoordinate(50.0, 50.0);

    QList<QLandmark> lmPool;

    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < names.size(); ++j) {
            for (int k = 0; k < coords.size(); ++k) {
                QLandmark lm;
                lm.addCategoryId(ids.at(i));
                lm.setName(names.at(j));
                lm.setCoordinate(coords.at(k));
                QVERIFY(m_manager->saveLandmark(&lm));
                lmPool << lm;
            }
        }
    }

    QLandmarkCategoryFilter f1(cat2.categoryId());
    QLandmarkNameFilter f2("LM2");
    QLandmarkProximityFilter f3(QGeoCoordinate(25.0, 25.0), 5.0);

    QLandmarkIntersectionFilter filter;
    filter << f1 << f2 << f3;

    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(filter);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::InactiveState);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark>lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter));

    QCOMPARE(lms.size(), 1);

    QSet<QString> idSet;
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            && (lm.name() == "LM2")
            && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.remove(f2);
    fetchRequest.setFilter(filter);
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest));

    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter));

    QCOMPARE(lms.size(), 3);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.prepend(f2);

    fetchRequest.setFilter(filter);
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter));

    QCOMPARE(lms.size(), 1);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            && (lm.name() == "LM2")
            && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksUnion() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QList<QLandmarkCategoryId> ids;
    ids << cat1.categoryId();
    ids << cat2.categoryId();
    ids << cat3.categoryId();

    QList<QString> names;
    names << "LM1";
    names << "LM2";
    names << "LM3";

    QList<QGeoCoordinate> coords;
    coords << QGeoCoordinate(0.0, 0.0);
    coords << QGeoCoordinate(25.0, 25.0);
    coords << QGeoCoordinate(50.0, 50.0);

    QList<QLandmark> lmPool;

    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < names.size(); ++j) {
            for (int k = 0; k < coords.size(); ++k) {
                QLandmark lm;
                lm.addCategoryId(ids.at(i));
                lm.setName(names.at(j));
                lm.setCoordinate(coords.at(k));
                QVERIFY(m_manager->saveLandmark(&lm));
                lmPool << lm;
            }
        }
    }

    QLandmarkCategoryFilter f1(cat2.categoryId());
    QLandmarkNameFilter f2("LM2");
    QLandmarkProximityFilter f3(QGeoCoordinate(25.0, 25.0), 5.0);

    QLandmarkUnionFilter filter;
    filter << f1 << f2 << f3;

    QList<QLandmark> lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 19);

    QSet<QString> idSet;
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            || (lm.name() == "LM2")
            || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.remove(f2);

    lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 15);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.prepend(f2);

    lms = m_manager->landmarks(filter);

    QCOMPARE(lms.size(), 19);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            || (lm.name() == "LM2")
            || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }
}

void tst_QLandmarkManagerEngineSqlite::filterLandmarksUnionAsync() {
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QList<QLandmarkCategoryId> ids;
    ids << cat1.categoryId();
    ids << cat2.categoryId();
    ids << cat3.categoryId();

    QList<QString> names;
    names << "LM1";
    names << "LM2";
    names << "LM3";

    QList<QGeoCoordinate> coords;
    coords << QGeoCoordinate(0.0, 0.0);
    coords << QGeoCoordinate(25.0, 25.0);
    coords << QGeoCoordinate(50.0, 50.0);

    QList<QLandmark> lmPool;

    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < names.size(); ++j) {
            for (int k = 0; k < coords.size(); ++k) {
                QLandmark lm;
                lm.addCategoryId(ids.at(i));
                lm.setName(names.at(j));
                lm.setCoordinate(coords.at(k));
                QVERIFY(m_manager->saveLandmark(&lm));
                lmPool << lm;
            }
        }
    }

    QLandmarkCategoryFilter f1(cat2.categoryId());
    QLandmarkNameFilter f2("LM2");
    QLandmarkProximityFilter f3(QGeoCoordinate(25.0, 25.0), 5.0);

    QLandmarkUnionFilter filter;
    filter << f1 << f2 << f3;

    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setFilter(filter);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    QCOMPARE(fetchRequest.state(), QLandmarkAbstractRequest::InactiveState);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark>lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, filter));

    QCOMPARE(lms.size(), 19);

    QSet<QString> idSet;
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            || (lm.name() == "LM2")
            || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.remove(f2);

    fetchRequest.setFilter(filter);
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter));

    QCOMPARE(lms.size(), 15);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }

    filter.prepend(f2);

    fetchRequest.setFilter(filter);
    fetchRequest.start();
    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter));

    QCOMPARE(lms.size(), 19);

    idSet.clear();
    for (int i = 0; i < lms.size(); ++i)
        idSet.insert(lms.at(i).landmarkId().localId());

    for (int i = 0; i < lmPool.size(); ++i) {
        QLandmark lm = lmPool.at(i);
        if ((lm.categoryIds().at(0) == cat2.categoryId())
            || (lm.name() == "LM2")
            || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), true);
        } else {
            QCOMPARE(idSet.contains(lm.landmarkId().localId()), false);
        }
    }
}

void tst_QLandmarkManagerEngineSqlite::sortLandmarksNull() {
    QLandmark lm1;
    lm1.setName("b");
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("a");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("c");
    QVERIFY(m_manager->saveLandmark(&lm3));

    QList<QLandmark> expected;
    expected << lm1;
    expected << lm2;
    expected << lm3;

    QLandmarkFilter filter;
    QLandmarkSortOrder sortOrder;
    QList<QLandmarkSortOrder> sortOrders;

    QList<QLandmark> lms = m_manager->landmarks(filter);
    QCOMPARE(lms, expected);

    lms = m_manager->landmarks(filter, sortOrder);
    QCOMPARE(lms, expected);

    lms = m_manager->landmarks(filter, sortOrders);
    QCOMPARE(lms, expected);

    sortOrders << sortOrder;
    lms = m_manager->landmarks(filter, sortOrders);
    QCOMPARE(lms, expected);

    sortOrders << sortOrder;
    lms = m_manager->landmarks(filter, sortOrders);
    QCOMPARE(lms, expected);
}

void tst_QLandmarkManagerEngineSqlite::sortLandmarksName() {
    QLandmark lm1;
    lm1.setName("b");
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("a");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("c");
    QVERIFY(m_manager->saveLandmark(&lm3));

    QList<QLandmark> expectedAscending;
    expectedAscending << lm2;
    expectedAscending << lm1;
    expectedAscending << lm3;

    QList<QLandmark> expectedDescending;
    expectedDescending << lm3;
    expectedDescending << lm1;
    expectedDescending << lm2;

    QLandmarkFilter filter;
    QLandmarkNameSort sortAscending(Qt::AscendingOrder);

    QList<QLandmark> lms = m_manager->landmarks(filter, sortAscending);
    QCOMPARE(lms, expectedAscending);

    QLandmarkNameSort sortDescending(Qt::DescendingOrder);

    lms = m_manager->landmarks(filter, sortDescending);
    QCOMPARE(lms, expectedDescending);
}

void tst_QLandmarkManagerEngineSqlite::sortLandmarksNameAsync() {
    QLandmark lm1;
    lm1.setName("b");
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("a");
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("c");
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setName("C");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmark lm5;
    lm5.setName("A");
    QVERIFY(m_manager->saveLandmark(&lm5));

    QLandmark lm6;
    lm6.setName("B");
    QVERIFY(m_manager->saveLandmark(&lm6));

    QList<QLandmark> expectedAscending;
    expectedAscending << lm2;
    expectedAscending << lm5;
    expectedAscending << lm1;
    expectedAscending << lm6;
    expectedAscending << lm3;
    expectedAscending << lm4;

    QList<QLandmark> expectedDescending;
    expectedDescending << lm3;
    expectedDescending << lm4;
    expectedDescending << lm1;
    expectedDescending << lm6;
    expectedDescending << lm2;
    expectedDescending << lm5;

    //test case insensitive ascending order
    QLandmarkFilter filter;
    QLandmarkNameSort sortAscending(Qt::AscendingOrder);
    QLandmarkFetchRequest fetchRequest(m_manager);
    fetchRequest.setSorting(sortAscending);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.start();

    QVERIFY(waitForAsync(spy,&fetchRequest));
    QList<QLandmark> lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter, sortAscending));

    QCOMPARE(lms, expectedAscending);

    //test case insensitive descending order
    QLandmarkNameSort sortDescending(Qt::DescendingOrder);
    fetchRequest.setSorting(sortDescending);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms, filter, sortDescending));

    QCOMPARE(lms, expectedDescending);

    //test case sensitive ascending order
    expectedAscending.clear();
    expectedAscending << lm5;
    expectedAscending << lm6;
    expectedAscending << lm4;
    expectedAscending << lm2;
    expectedAscending << lm1;
    expectedAscending << lm3;

    sortAscending.setCaseSensitivity(Qt::CaseSensitive);
    fetchRequest.setSorting(sortAscending);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter,sortAscending));

    QCOMPARE(lms, expectedAscending);

    //test case sensitive descending order
    expectedDescending.clear();
    expectedDescending << lm3;
    expectedDescending << lm1;
    expectedDescending << lm2;
    expectedDescending << lm4;
    expectedDescending << lm6;
    expectedDescending << lm5;

    sortDescending.setCaseSensitivity(Qt::CaseSensitive);
    fetchRequest.setSorting(sortDescending);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter,sortDescending));

    QCOMPARE(lms, expectedDescending);
}

void tst_QLandmarkManagerEngineSqlite::sortLandmarksDistance() {
    QLandmark lm1;
    lm1.setCoordinate(QGeoCoordinate(2.0, 2.0));
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setCoordinate(QGeoCoordinate(1.0, 1.0));
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setCoordinate(QGeoCoordinate(3.0, 3.0));
    QVERIFY(m_manager->saveLandmark(&lm4));

    QList<QLandmark> expectedAscending;
    expectedAscending << lm2;
    expectedAscending << lm1;
    expectedAscending << lm4;
    expectedAscending << lm3;

    QList<QLandmark> expectedDescending;
    expectedDescending << lm3;
    expectedDescending << lm4;
    expectedDescending << lm1;
    expectedDescending << lm2;

    QLandmarkFilter filter;
    QLandmarkDistanceSort sortAscending(QGeoCoordinate(0.0, 0.0), Qt::AscendingOrder);

    QList<QLandmark> lms = m_manager->landmarks(filter, sortAscending);

    QCOMPARE(lms, expectedAscending);

    QLandmarkDistanceSort sortDescending(QGeoCoordinate(0.0, 0.0), Qt::DescendingOrder);

    lms = m_manager->landmarks(filter, sortDescending);

    QCOMPARE(lms, expectedDescending);
}

void tst_QLandmarkManagerEngineSqlite::sortLandmarksDistanceAsync() {
    QLandmark lm1;
    lm1.setCoordinate(QGeoCoordinate(2.0, 2.0));
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setCoordinate(QGeoCoordinate(1.0, 1.0));
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setCoordinate(QGeoCoordinate(3.0, 3.0));
    QVERIFY(m_manager->saveLandmark(&lm4));

    QList<QLandmark> expectedAscending;
    expectedAscending << lm2;
    expectedAscending << lm1;
    expectedAscending << lm4;
    expectedAscending << lm3;

    QList<QLandmark> expectedDescending;
    expectedDescending << lm3;
    expectedDescending << lm4;
    expectedDescending << lm1;
    expectedDescending << lm2;

    QLandmarkFilter filter;
    QLandmarkDistanceSort sortAscending(QGeoCoordinate(0.0, 0.0), Qt::AscendingOrder);

    QLandmarkFetchRequest fetchRequest(m_manager);
    QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    fetchRequest.setFilter(filter);
    fetchRequest.setSorting(sortAscending);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    QList<QLandmark> lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter,sortAscending));

    QCOMPARE(lms, expectedAscending);

    QLandmarkDistanceSort sortDescending(QGeoCoordinate(0.0, 0.0), Qt::DescendingOrder);
    fetchRequest.setSorting(sortDescending);
    fetchRequest.start();

    QVERIFY(waitForAsync(spy, &fetchRequest));
    lms = fetchRequest.landmarks();
    QVERIFY(checkIdFetchRequest(lms,filter,sortDescending));

    QCOMPARE(lms, expectedDescending);
}

void tst_QLandmarkManagerEngineSqlite::importGpx() {
    QVERIFY(m_manager->importLandmarks(":data/McDonalds-AUS-Queensland.gpx", "GpxV1.1"));
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    QList<QLandmark> landmarks = m_manager->landmarks(QLandmarkFilter());

    QLandmark lmFirst;
    lmFirst.setName("McDonald s Airlie Beac... (sample)");
    lmFirst.setCoordinate(QGeoCoordinate(-20.269213, 148.718128));
    lmFirst.setLandmarkId(landmarks.first().landmarkId());
    QCOMPARE(lmFirst, landmarks.first());

    QLandmark lmLast;
    lmLast.setName("McDonald s Yamant... (sample)");
    lmLast.setCoordinate(QGeoCoordinate(-27.660976,152.738973));
    lmLast.setLandmarkId(landmarks.last().landmarkId());

    QCOMPARE(lmLast, landmarks.last());
}

/* Disable importing of landmarks test because there
is a flood of notifications being caused by the
saving of landmarks.  These notifications are interefering
with the waiting for a request to finish.

void tst_QLandmarkManagerEngineSqlite::importGpxAsync() {
    QLandmarkImportRequest importRequest(m_manager);
    QSignalSpy spy(&importRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    importRequest.setFileName(":data/McDonalds-AUS-Queensland.gpx");
    importRequest.setFormat("GpxV1.1");
    importRequest.start();

    QVERIFY(waitForAsync(spy, &importRequest, QLandmarkManager::NoError,2000));

    QList<QLandmark> landmarks = m_manager->landmarks(QLandmarkFilter());

    QLandmark lmFirst;
    lmFirst.setName("McDonald s Airlie Beac... (sample)");
    lmFirst.setCoordinate(QGeoCoordinate(-20.269213, 148.718128));
    lmFirst.setLandmarkId(landmarks.first().landmarkId());
    QCOMPARE(lmFirst, landmarks.first());

    QLandmark lmLast;
    lmLast.setName("McDonald s Yamant... (sample)");
    lmLast.setCoordinate(QGeoCoordinate(-27.660976,152.738973));
    lmLast.setLandmarkId(landmarks.last().landmarkId());

    QCOMPARE(lmLast, landmarks.last());

    //try a non-existent file
    importRequest.setFileName("doesnnotexist.gpx");
    importRequest.setFormat("GpxV1.1");
    importRequest.start();

    QVERIFY(waitForAsync(spy, &importRequest, QLandmarkManager::DoesNotExistError));

    //try cancelling and impot halfway
    importRequest.setFileName(":data/long.gpx");
    importRequest.setFormat("GpxV1.1");
    importRequest.start();
    QTest::qWait(250);
    importRequest.cancel();
    QVERIFY(waitForAsync(spy, &importRequest, QLandmarkManager::CancelError,
                         3000, QLandmarkAbstractRequest::FinishedState));
}*/

void tst_QLandmarkManagerEngineSqlite::exportGpx() {
    QLandmark lm1;
    lm1.setName("lm1");
    QGeoCoordinate coord1(10,20);
    lm1.setCoordinate(coord1);
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("lm2");
    QGeoCoordinate coord2(10,20);
    lm2.setCoordinate(coord2);
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("lm3");
    QGeoCoordinate coord3(10,20);
    lm3.setCoordinate(coord3);
    QVERIFY(m_manager->saveLandmark(&lm3));

    //note: the gpx file handler should skip over lm4 since
    //gpx can't doesn't allow nan coordiates.
    QLandmark lm4;
    lm4.setName("lm4");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QFile file("myexport.gpx");
    if (file.exists())
        file.remove();
    QVERIFY(!file.exists());

    QVERIFY(m_manager->exportLandmarks("myexport.gpx","GpxV1.1"));
    QVERIFY(file.exists());

    QVERIFY(m_manager->importLandmarks("myexport.gpx", "GpxV1.1"));
    QList<QLandmark> lms = m_manager->landmarks();
    QCOMPARE(lms.count(), 7);
    QLandmarkNameFilter nameFilter;
    nameFilter.setName("lm1");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm2");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm3");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm4");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),1);

    //try supplying a list of landmark ids
    QList<QLandmarkId> lmIds;
    lmIds << lm1.landmarkId();
    lmIds << lm3.landmarkId();
    QVERIFY(file.remove());
    QVERIFY(m_manager->exportLandmarks("myexport.gpx", "GpxV1.1", lmIds));

    QVERIFY(m_manager->importLandmarks("myexport.gpx", "GpxV1.1"));
    lms = m_manager->landmarks();
    QCOMPARE(lms.count(), 9);

    nameFilter.setName("lm1");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),3);
    nameFilter.setName("lm2");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm3");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),3);
    nameFilter.setName("lm4");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),1);
    QVERIFY(file.remove());
}

void tst_QLandmarkManagerEngineSqlite::exportGpxAsync() {
    QLandmark lm1;
    lm1.setName("lm1");
    QGeoCoordinate coord1(10,20);
    lm1.setCoordinate(coord1);
    QVERIFY(m_manager->saveLandmark(&lm1));

    QLandmark lm2;
    lm2.setName("lm2");
    QGeoCoordinate coord2(10,20);
    lm2.setCoordinate(coord2);
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmark lm3;
    lm3.setName("lm3");
    QGeoCoordinate coord3(10,20);
    lm3.setCoordinate(coord3);
    QVERIFY(m_manager->saveLandmark(&lm3));

    //note: the gpx file handler should skip over lm4 since
    //gpx can't doesn't allow nan coordiates.
    QLandmark lm4;
    lm4.setName("lm4");
    QVERIFY(m_manager->saveLandmark(&lm4));

    QFile file("myexport.gpx");
    if (file.exists())
        file.remove();
    QVERIFY(!file.exists());

    QLandmarkExportRequest exportRequest(m_manager);
    QSignalSpy spy(&exportRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    exportRequest.setFileName(file.fileName());
    exportRequest.setFormat("GpxV1.1");
    exportRequest.start();

    QVERIFY(waitForAsync(spy, &exportRequest, QLandmarkManager::NoError));
    QVERIFY(file.exists());

    QVERIFY(m_manager->importLandmarks("myexport.gpx", "GpxV1.1"));
    QList<QLandmark> lms = m_manager->landmarks();
    QCOMPARE(lms.count(), 7);
    QLandmarkNameFilter nameFilter;
    nameFilter.setName("lm1");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm2");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm3");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm4");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),1);

    //try supplying a list of landmark ids
    QList<QLandmarkId> lmIds;
    lmIds << lm1.landmarkId();
    lmIds << lm3.landmarkId();
    exportRequest.setLandmarkIds(lmIds);

    QVERIFY(file.remove());
    exportRequest.start();
    QVERIFY(waitForAsync(spy, &exportRequest, QLandmarkManager::NoError));

    QVERIFY(m_manager->importLandmarks("myexport.gpx", "GpxV1.1"));
    lms = m_manager->landmarks();
    QCOMPARE(lms.count(), 9);

    nameFilter.setName("lm1");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),3);
    nameFilter.setName("lm2");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),2);
    nameFilter.setName("lm3");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),3);
    nameFilter.setName("lm4");
    QCOMPARE(m_manager->landmarks(nameFilter).count(),1);
    QVERIFY(file.remove());
}

QTEST_MAIN(tst_QLandmarkManagerEngineSqlite)
#include "tst_qlandmarkmanagerengine_sqlite.moc"

