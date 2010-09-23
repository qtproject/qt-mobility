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
#include <qlandmarkattributefilter.h>

#include <qlandmarksortorder.h>
#include <qlandmarknamesort.h>
#include <qlandmarkabstractrequest.h>
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarkfetchbyidrequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarkcategoryidfetchrequest.h>
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategoryfetchbyidrequest.h>
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

#ifdef Q_OS_SYMBIAN
#include <EPos_CPosLmDatabaseManager.h>
#else
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#endif

//defines to turn on and off tests
#define SAVE_CATEGORY
#define SAVE_LANDMARK

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

class tst_QLandmarkManager : public QObject
{
    Q_OBJECT
public:
    tst_QLandmarkManager() {
    qRegisterMetaType<QList<QLandmarkCategoryId> >();
    qRegisterMetaType<QList<QLandmarkId> >();
    qRegisterMetaType<QList<QLandmark> >();
    qRegisterMetaType<QList<QLandmarkCategory> >();
    qRegisterMetaType<QLandmarkAbstractRequest *>();
    qRegisterMetaType<QLandmarkFetchRequest *>();
    qRegisterMetaType<QLandmarkSaveRequest *>();
    qRegisterMetaType<QLandmarkRemoveRequest *>();
    qRegisterMetaType<QLandmarkCategoryFetchRequest *>();
    qRegisterMetaType<QLandmarkCategorySaveRequest *>();
    qRegisterMetaType<QLandmarkCategoryRemoveRequest *>();
    qRegisterMetaType<QLandmarkManager::Error>();
    qRegisterMetaType<QLandmarkAbstractRequest::State>();

    exportFile = "exportfile";
    QFile::remove(exportFile);

#ifdef Q_OS_SYMBIAN
    deleteDefaultDb();
#else
    QFile::remove("test.db");
    //TODO: verify if this is needed
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","landmarks");
    db.setDatabaseName("test.db");
#endif
    }

private:
    QLandmarkManager *m_manager;
    ManagerListener *m_listener;
    QString exportFile;

    bool waitForAsync(QSignalSpy &spy, QLandmarkAbstractRequest *request,
                    QLandmarkManager::Error error = QLandmarkManager::NoError,
                    int ms=1500, QLandmarkAbstractRequest::State state = QLandmarkAbstractRequest::FinishedState) {
        bool ret = true;
        int msWaitedSoFar =0;
        while(msWaitedSoFar < ms) {
            QTest::qWait(100);
            msWaitedSoFar +=100;
            if (spy.count() ==2)
                break;
        }

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

    bool doCategoryFetch(const QString type, const QList<QLandmarkCategoryId> &ids, QList<QLandmarkCategory> *cats, QLandmarkManager::Error expectedError = QLandmarkManager::NoError) {
        bool result = false;

        if (type== "sync") {
            *cats = m_manager->categories();
            result = (m_manager->error() == expectedError);
        }  else if (type == "async") {
            QLandmarkCategoryFetchByIdRequest fetchRequest(m_manager);
            QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
            fetchRequest.setCategoryIds(ids);
            fetchRequest.start();
            bool waitResult = waitForAsync(spy, &fetchRequest,expectedError,100);
            if (!waitResult)
                qWarning("Wait for async failed for category fetch");

            *cats = fetchRequest.categories();
            result = result && waitResult;
        } else {
            qFatal("Unknown test row type");
        }
        return result;
    }

    bool doFetch(const QString type, const QLandmarkFilter &filter, QList<QLandmark> *lms){
        bool result =false;
        if (type== "sync") {
            *lms = m_manager->landmarks(filter);
            result = (m_manager->error() == QLandmarkManager::NoError);
            QList<QLandmarkId> lmIds = m_manager->landmarkIds(filter);
            bool checkIdsResult = checkIds(*lms,lmIds);
            if (!checkIdsResult)
                qWarning("sync landmark id fetch failed to match sync landmark fetch");
            result = result && checkIdsResult;
        } else if (type == "async") {
            QLandmarkFetchRequest fetchRequest(m_manager);
            QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
            fetchRequest.setFilter(filter);
            fetchRequest.start();
            bool waitResult = waitForAsync(spy, &fetchRequest,QLandmarkManager::NoError,100);
            if (!waitResult)
                    qWarning("Wait for async failed for landmark fetch");
            result = waitResult;
            *lms = fetchRequest.landmarks();

            QLandmarkIdFetchRequest idFetchRequest(m_manager);
            QSignalSpy spyId(&idFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
            idFetchRequest.setFilter(filter);
            idFetchRequest.start();
            waitResult = waitForAsync(spyId, &idFetchRequest,QLandmarkManager::NoError,100);
            if (!waitResult)
                qWarning("Wait for async failed for landmark id fetch");
            QList<QLandmarkId> lmIds = idFetchRequest.landmarkIds();
            bool checkIdsResult = checkIds(*lms, lmIds);
            if (!checkIdsResult)
                qWarning("async landmark id fetch failed to match async landmark fetch");
            result = result && waitResult && checkIdsResult;
        } else {
            qFatal("Unknown test row type");
        }
        return result;
    }

     bool doFetchById(const QString type, const QList<QLandmarkId>lmIds, QList<QLandmark> *lms,
                        QLandmarkManager::Error error = QLandmarkManager::NoError){
        bool result =false;
        if (type== "sync") {
            *lms = m_manager->landmarks(lmIds);
            result = (m_manager->error() == error);
        } else if (type == "async") {
            QLandmarkFetchByIdRequest fetchRequest(m_manager);
            QSignalSpy spy(&fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
            fetchRequest.setLandmarkIds(lmIds);
            fetchRequest.start();
            result = waitForAsync(spy, &fetchRequest,error,100);
            *lms = fetchRequest.landmarks();
        } else {
            qFatal("Unknown test row type");
        }
        return result;
    }

     bool doSingleCategorySave(const QString &type, QLandmarkCategory *category,
                 QLandmarkManager::Error error = QLandmarkManager::NoError)
     {
         bool result = false;
         if (type == "sync") {
             if (error == QLandmarkManager::NoError) {
                 result = m_manager->saveCategory(category)
                          && (m_manager->error() == QLandmarkManager::NoError);
             } else {
                 result = (!m_manager->saveCategory(category))
                          && (m_manager->error() == error);
             }
             if (!result) {
                 qWarning() << "Actual error = " << m_manager->error();
                 qWarning() << "Actual error string = " << m_manager->errorString();
                 qWarning() << "Expected error=" << error;
             }
         } else if (type == "async") {
             QLandmarkCategorySaveRequest catSaveRequest(m_manager);
             QSignalSpy spy(&catSaveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
             QSignalSpy resultsAvailableSpy(&catSaveRequest, SIGNAL(resultsAvailable()));
             catSaveRequest.setCategory(*category);
             catSaveRequest.start();
             result = waitForAsync(spy, &catSaveRequest,error,1000);
             if (!result) {
                 qWarning() << "Wait for asynchronous request failed";
             }

             QMap<int, QLandmarkManager::Error> errorMap;
             errorMap = catSaveRequest.errorMap();
             result = result && (catSaveRequest.categories().count() ==1);

             if (error == QLandmarkManager::NoError) {
                 result = result && (errorMap.count() ==0);
             } else {
                 result = result && (errorMap.count() == 1)
                           && (errorMap.value(0) == error);
             }
             if (!result) {
                 qWarning() << "Actual error " << catSaveRequest.error();
                 qWarning() << "Actual error string = " << catSaveRequest.errorString();
                 qWarning() << "Expected error = " << error;
             }

             *category = catSaveRequest.categories().at(0);
         } else {
             qFatal("Unknown test row type");
         }
         return result;
     }

     bool doSingleLandmarkSave(const QString &type, QLandmark *landmark,
                 QLandmarkManager::Error error = QLandmarkManager::NoError)
     {
         bool result = false;
         if (type == "sync") {
             if (error == QLandmarkManager::NoError) {
                 result = m_manager->saveLandmark(landmark)
                          && (m_manager->error() == QLandmarkManager::NoError);
             } else {
                 result = (!m_manager->saveLandmark(landmark))
                          && (m_manager->error() == error);
             }
         } else if (type == "async") {
             QLandmarkSaveRequest lmSaveRequest(m_manager);
             QSignalSpy spy(&lmSaveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
             QSignalSpy resultsAvailableSpy(&lmSaveRequest, SIGNAL(resultsAvailable()));
             lmSaveRequest.setLandmark(*landmark);
             lmSaveRequest.start();
             result = waitForAsync(spy, &lmSaveRequest,error,1000);
             result = result && (resultsAvailableSpy.count()==1);
             QMap<int, QLandmarkManager::Error> errorMap;
             errorMap = lmSaveRequest.errorMap();
             result = result && (lmSaveRequest.landmarks().count() ==1);
             if (error == QLandmarkManager::NoError) {
                 result = result && (errorMap.count() ==0);
             } else {
                 result = result && (errorMap.count() == 1)
                           && (errorMap.value(0) == error);
             }
             *landmark = lmSaveRequest.landmarks().at(0);
         } else {
             qFatal("Unknown test row type");
         }
         return result;
     }

     bool doSave(const QString &type, QList<QLandmark> *lms,
                 QLandmarkManager::Error error = QLandmarkManager::NoError,
                 QMap<int,QLandmarkManager::Error> *errorMap = 0)
     {
         bool result = false;
         if (type == "sync") {
             if (error == QLandmarkManager::NoError) {
                 result = m_manager->saveLandmarks(lms,errorMap)
                          && (m_manager->error() == QLandmarkManager::NoError);
             } else {
                 result = (!m_manager->saveLandmarks(lms,errorMap))
                          && (m_manager->error() == error);
             }
         } else if (type == "async") {
             QLandmarkSaveRequest saveRequest(m_manager);
             QSignalSpy spy(&saveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
             saveRequest.setLandmarks(*lms);
             saveRequest.start();
             result = waitForAsync(spy, &saveRequest,error,1000);
             if (errorMap)
                *errorMap = saveRequest.errorMap();
             *lms = saveRequest.landmarks();
         } else {
             qFatal("Unknown test row type");
         }
         return result;
     }

     bool doRemove(const QString &type, QList<QLandmarkId> lmIds,
                 QLandmarkManager::Error error = QLandmarkManager::NoError,
                 QMap<int,QLandmarkManager::Error> *errorMap = 0)
     {
         bool result = false;
         if (type == "sync") {
             if (error == QLandmarkManager::NoError) {
                 result = m_manager->removeLandmarks(lmIds,errorMap)
                          && (m_manager->error() == QLandmarkManager::NoError);
             } else {
                 result = (!m_manager->removeLandmarks(lmIds,errorMap))
                          && (m_manager->error() == error);
             }
         } else if (type == "async") {
             QLandmarkRemoveRequest removeRequest(m_manager);
             QSignalSpy spy(&removeRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
             removeRequest.setLandmarkIds(lmIds);
             removeRequest.start();
             result = waitForAsync(spy, &removeRequest,error,200);
             if (errorMap)
                 *errorMap = removeRequest.errorMap();
         } else {
             qFatal("Unknown test row type");
         }
         return result;
     }

    bool doImport(const QString &type, const QString filename,
                  QLandmarkManager::Error error =QLandmarkManager::NoError){
        bool result =false;
        if (type== "sync") {
            if (error == QLandmarkManager::NoError) {
                result = (m_manager->importLandmarks(filename))
                         && (m_manager->error() == QLandmarkManager::NoError);
            } else {
                result = (!m_manager->importLandmarks(filename))
                        && (m_manager->error() == error);
            }
        } else if (type == "async") {
            QLandmarkImportRequest importRequest(m_manager);
            QSignalSpy spy(&importRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
            importRequest.setFileName(filename);
            importRequest.start();
            result = waitForAsync(spy, &importRequest,error,150);
        } else {
            qFatal("Unknown test row type");
        }
        return result;
    }

    bool checkCategoryCount(int expectedCount) {
        return (m_manager->categories().count() == expectedCount)
                && checkIds(m_manager->categories(),m_manager->categoryIds());
    }

    bool checkLandmarkCount(int expectedCount) {
        return (m_manager->landmarks().count() == expectedCount)
                && checkIds(m_manager->landmarks(), m_manager->landmarkIds());
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
        return true;
    }

        bool checkIds(const QList<QLandmarkCategory> &cats, const QList<QLandmarkCategoryId> &catIds)
    {
        if (cats.count() != catIds.count())
            return false;
        for (int i=0; i < cats.count(); ++i) {
            if (cats.at(i).categoryId() != catIds.at(i)) {
                return false;
            }
        }
        return true;
    }

    //checks if an id fetch request will return the ids for the same landmarks as in \a lms
    bool checkIdFetchRequest(const QList<QLandmark> &lms, const QLandmarkFilter &filter,
                            const QList<QLandmarkSortOrder> &sorting = QList<QLandmarkSortOrder>(),
                            int limit =-1, int offset=0) {
        //check that the id request will return the same results
        QLandmarkIdFetchRequest idFetchRequest(m_manager);
        idFetchRequest.setFilter(filter);
        idFetchRequest.setSorting(sorting);
        idFetchRequest.setLimit(limit);
        idFetchRequest.setOffset(offset);

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
                            int limit=-1, int offset=0)
    {
        QList<QLandmarkSortOrder> sortOrders;
        sortOrders << sorting;
        return  checkIdFetchRequest(lms, filter, sortOrders, limit, offset);
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
        if (m_manager != 0) {
            delete m_manager;
            m_manager =0;
        }

        if (m_listener !=0) {
            delete m_listener;
            m_listener =0;
        }
        QMap<QString, QString> map;
#ifdef Q_OS_SYMBIAN
        m_manager = new QLandmarkManager();
#else        

        map["filename"] = "test.db";
        m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
#endif
        m_listener = new ManagerListener;
        connectNotifications();
    }

#ifdef Q_OS_SYMBIAN
    void deleteDefaultDb(){
        CPosLmDatabaseManager* lmDbManager = CPosLmDatabaseManager::NewL();
        CleanupStack::PushL(lmDbManager);
        HBufC* defaultDbUri = lmDbManager->DefaultDatabaseUriLC();
        if (lmDbManager->DatabaseExistsL(defaultDbUri->Des())) {
            lmDbManager->DeleteDatabaseL(defaultDbUri->Des());
        }
        CleanupStack::PopAndDestroy(defaultDbUri);
        CleanupStack::PopAndDestroy(lmDbManager);
    }
#endif

    void deleteDb() {
        QFile file;

#ifdef Q_OS_SYMBIAN
        QList<QLandmarkId> lmIds = m_manager->landmarkIds();
        for(int i=0; i < lmIds.count(); ++i) {
            QVERIFY(m_manager->removeLandmark(lmIds.at(i)));
        }

        QList<QLandmarkCategoryId> catIds = m_manager->categoryIds();
        for (int i=0; i < catIds.count(); ++i) {
            if (!m_manager->isReadOnly(catIds.at(i)))
                QVERIFY(m_manager->removeCategory(catIds.at(i)));
        }
#else
        {
            QSqlDatabase db = QSqlDatabase::database("landmarks");
            QSqlQuery q1("delete from landmark;", db);
            q1.exec();
            QSqlQuery q2("delete from category;", db);
            q2.exec();
            QSqlQuery q3("delete from landmark_category;", db);
            q3.exec();
        }

        QFile::remove("test.db");
#endif
        delete m_manager;
        m_manager = 0;

        delete m_listener;
        m_listener =0;

        QFile::remove(exportFile);
        file.setFileName("nopermfile");
        file.setPermissions(QFile::WriteOwner | QFile::WriteUser | QFile::WriteOther);
        file.remove();
    }

#ifdef Q_OS_SYMBIAN
    void clearDb() {

        QList<QLandmarkId> lmIds = m_manager->landmarkIds();
        for(int i=0; i < lmIds.count(); ++i)
            QVERIFY(m_manager->removeLandmark(lmIds.at(i)));

        QList<QLandmarkCategoryId> catIds = m_manager->categoryIds();
        for (int i=0; i < catIds.count(); ++i) {
            if (!m_manager->isReadOnly(catIds.at(i)))
                QVERIFY(m_manager->removeCategory(catIds.at(i)));
        }

        QTest::qWait(20);//try ensure notifications for these deletions
                         //are made prior to each test function
    }
#endif


#ifndef Q_OS_SYMBIAN
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
#endif

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

#ifndef Q_OS_SYMBIAN
    void createDbNew();
    void createDbExists();
#endif

/*RE-ENABLE
    void retrieveCategory();
    void retrieveCategory_data();
 RE-ENABLE*/

#ifdef RESTORE
    void categoryFetchCancelAsync();
#endif
/* RE-ENABLE
    void retrieveLandmark();
    void retrieveLandmark_data();
*/
    /* TODO: restore
    void asyncLandmarkFetchCancel();
*/

#ifdef SAVE_CATEGORY
    void saveCategory();
    void saveCategory_data();
#endif

#ifdef SAVE_LANDMARK
    void saveLandmark();
    void saveLandmark_data();
#endif

    /*
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
    void saveMultipleLandmarks_data();

    void removeMultipleLandmarks();
    void removeMultipleLandmarks_data();

    void listCategoryIds();

    void filterLandmarksLimitMatches();
    void filterLandmarksLimitMatchesAsync();

    void filterLandmarksDefault();

    void filterLandmarksName();

    void filterLandmarksProximity();
    void filterLandmarksProximityAsync();

    void filterLandmarksProximityOrder();
    void filterLandmarksProximityOrder_data();

    void filterLandmarksNearest();
    void filterLandmarksNearestAsync();

    void filterLandmarksCategory();
    void filterLandmarksCategoryAsync();

    void filterLandmarksBox();
    void filterLandmarksBoxAsync();

    void filterLandmarksIntersection();
    void filterLandmarksIntersection_data();
    void filterLandmarksIntersectionAsync();

    void filterLandmarksMultipleBox();
    void filterLandmarksMultipleBox_data();

    void filterLandmarksUnion();
    void filterLandmarksUnionAsync();

    void filterAttribute();
    void filterAttribute_data();

    void sortLandmarksNull();

    void sortLandmarksName();
    void sortLandmarksNameAsync();

    void importGpx();
    void importGpx_data();

    void importLmx();
    void importLmx_data();

    void exportGpx(); //async testing done too
    void exportGpx_data();


    void exportLmx();//async testing done too
    void exportLmx_data();

    void importFile();
    void importFile_data();

    void supportedFormats();

    void filterSupportLevel();
    void sortOrderSupportLevel();

    void isFeatureSupported();


    void categoryLimitOffset();
    //TODO: void categoryLimitOffsetAsync()

    void notificationCheck();*/
};


void tst_QLandmarkManager::initTestCase() {
    m_manager = 0;
    m_listener = 0;
}

void tst_QLandmarkManager::init() {
    createDb();
#ifdef Q_OS_SYMBIAN
    clearDb();
#endif
}

void tst_QLandmarkManager::cleanup() {
    deleteDb();
}

void tst_QLandmarkManager::cleanupTestCase() {
    QFile::remove(exportFile);
}

#ifndef Q_OS_SYMBIAN
void tst_QLandmarkManager::createDbNew() {
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());

    deleteDb();
    createDb();
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

    QVERIFY(tablesExist());
}

void tst_QLandmarkManager::createDbExists() {
    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());

    createDb();

    QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
    QVERIFY(tablesExist());
}
#endif

/* RE-ENABLE
void tst_QLandmarkManager::retrieveCategory() {
    QFETCH(QString, type);
    QLandmarkCategoryId id1;

    QLandmarkCategoryFetchByIdRequest catFetchByIdRequest(m_manager);
    QSignalSpy spy(&catFetchByIdRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    QList<QLandmarkCategory> cats;

    if ( type == "sync") {
        //try a default constructed category id
        QCOMPARE(m_manager->category(QLandmarkCategoryId()), QLandmarkCategory());
        QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
        QCOMPARE(m_manager->category(QLandmarkCategoryId()).categoryId().isValid(), false);

        //try an id does not exist
        id1.setManagerUri(m_manager->managerUri());
        id1.setLocalId("1");
        QCOMPARE(m_manager->category(id1), QLandmarkCategory());
        QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
        QCOMPARE(m_manager->category(id1).categoryId().isValid(), false);
    } else if (type == "async") {
        //try a default constructed id
        catFetchByIdRequest.setCategoryId(QLandmarkCategoryId());
        catFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &catFetchByIdRequest,QLandmarkManager::DoesNotExistError,100));
        cats = catFetchByIdRequest.categories();
        QCOMPARE(cats.count(), 1);
        QCOMPARE(cats.at(0), QLandmarkCategory());
        QCOMPARE(cats.at(0).categoryId().isValid(), false);

        //try an id that does not exist
        id1.setManagerUri(m_manager->managerUri());
        id1.setLocalId("1");
        catFetchByIdRequest.setCategoryId(id1);
        catFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &catFetchByIdRequest,QLandmarkManager::DoesNotExistError,100));
        cats = catFetchByIdRequest.categories();
        QCOMPARE(cats.count(), 1);
        QCOMPARE(cats.at(0), QLandmarkCategory());
        QCOMPARE(cats.at(0).categoryId().isValid(), false);
    } else {
        qFatal("Unknown test row type");
    }

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    cat2.setIconUrl(QUrl("CAT2Icon"));
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategoryId id2;

    if ( type == "sync") {
        //try a manager that doesn't match
        id2.setManagerUri("wrong.manager");
        id2.setLocalId(cat2.categoryId().localId());
        QCOMPARE(m_manager->category(id2), QLandmarkCategory());
        QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
        QCOMPARE(m_manager->category(id2).categoryId().isValid(), false);

        //try an existing id
        id2.setManagerUri(cat2.categoryId().managerUri());
        id2.setLocalId(cat2.categoryId().localId());
        QCOMPARE(m_manager->category(id2), cat2);
        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QCOMPARE(m_manager->category(id2).categoryId().isValid(), true);
    } else if (type == "async") {
        //try a manager that doesn't match
        id2.setManagerUri("wrong.manager");
        id2.setLocalId(cat2.categoryId().localId());
        catFetchByIdRequest.setCategoryId(id2);
        catFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &catFetchByIdRequest,QLandmarkManager::DoesNotExistError,100));
        cats = catFetchByIdRequest.categories();
        QCOMPARE(cats.count(), 1);
        QCOMPARE(cats.at(0), QLandmarkCategory());
        QCOMPARE(cats.at(0).categoryId().isValid(), false);

        //try an existing id
        id2.setManagerUri(cat2.categoryId().managerUri());
        id2.setLocalId(cat2.categoryId().localId());
        catFetchByIdRequest.setCategoryId(id2);
        catFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &catFetchByIdRequest,QLandmarkManager::NoError,100));
        cats = catFetchByIdRequest.categories();
        QCOMPARE(cats.count(), 1);
        QCOMPARE(cats.at(0), cat2);
        QCOMPARE(cats.at(0).categoryId().isValid(), true);
    } else {
        qFatal("Unknown test row type");
    }
}

void tst_QLandmarkManager::retrieveCategory_data() {
    QTest::addColumn<QString>("type");

    QTest::newRow("sync") << "sync";
    QTest::newRow("async") << "async";
}
*/

#ifdef RESTORE_CANCEL_CATEGORY_FETCH
void tst_QLandmarkManager::categoryFetchCancelAsync()
{
    disconnectNotifications();
    QLandmarkCategory cat;
    for(int i=0; i < 550; ++i) {
        cat.clear();
        cat.setName(QString("CAT") + QString::number(i));
        QVERIFY(m_manager->saveCategory(&cat));
    }

    connectNotifications();
    QLandmarkCategoryFetchRequest catFetchRequest(m_manager);
    QSignalSpy spy(&catFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    catFetchRequest.start();
#ifdef Q_OS_SYMBIAN
#else
    QTest::qWait(50);
#endif
    catFetchRequest.cancel();
    QVERIFY(waitForAsync(spy, &catFetchRequest, QLandmarkManager::CancelError,2000));
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
#endif
/* RE-ENABLE
void tst_QLandmarkManager::retrieveLandmark() {
    QFETCH(QString, type);

    //pre-populate the manager to make sure we don't accidentally retrieve these
    //landmarks
    QLandmark lmAlpha;
    lmAlpha.setName("LM-ALPHA");
    QVERIFY(m_manager->saveLandmark(&lmAlpha));

    QLandmark lmBeta;
    lmBeta.setName("LM-BETA");
    QVERIFY(m_manager->saveLandmark(&lmBeta));

    QLandmark lmGamma;
    lmGamma.setName("LM-GAMMA");
    QVERIFY(m_manager->saveLandmark(&lmGamma));

    QLandmarkFetchByIdRequest lmFetchByIdRequest(m_manager);
    QSignalSpy spy(&lmFetchByIdRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
    QList<QLandmark> lms;

    QLandmarkId id1;
    if (type == "sync") {
        //try a default constructed landmark id
        QCOMPARE(m_manager->landmark(QLandmarkId()), QLandmark());
        QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
        QCOMPARE(m_manager->landmark(QLandmarkId()).landmarkId().isValid(),false);

        //try an id that does not exist
        id1.setManagerUri(m_manager->managerUri());
        id1.setLocalId("100");
        QCOMPARE(m_manager->landmark(id1), QLandmark());
        QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
        QCOMPARE(m_manager->landmark(id1).landmarkId().isValid(), false);
    } else if (type == "async") {
        //try a default constructed landmark id
        lmFetchByIdRequest.setLandmarkId(QLandmarkId());
        lmFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &lmFetchByIdRequest,QLandmarkManager::DoesNotExistError,100));
        QCOMPARE(lmFetchByIdRequest.errorMap().count(),1);
        QCOMPARE(lmFetchByIdRequest.errorMap().keys().at(0),0);
        QCOMPARE(lmFetchByIdRequest.errorMap().value(0), QLandmarkManager::DoesNotExistError);

        lms = lmFetchByIdRequest.landmarks();
        QCOMPARE(lms.count(), 1);
        QCOMPARE(lms.at(0), QLandmark());
        QCOMPARE(lms.at(0).landmarkId().isValid(), false);

        //try an id that does not exist
        id1.setManagerUri(m_manager->managerUri());
        id1.setLocalId("100");
        lmFetchByIdRequest.setLandmarkId(id1);
        lmFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &lmFetchByIdRequest,QLandmarkManager::DoesNotExistError,100));
        QCOMPARE(lmFetchByIdRequest.errorMap().count(),1);
        QCOMPARE(lmFetchByIdRequest.errorMap().keys().at(0),0);
        QCOMPARE(lmFetchByIdRequest.errorMap().value(0), QLandmarkManager::DoesNotExistError);

        lms = lmFetchByIdRequest.landmarks();
        QCOMPARE(lms.count(), 1);
        QCOMPARE(lms.at(0), QLandmark());
        QCOMPARE(lms.at(0).landmarkId().isValid(), false);
    } else {
        qFatal("Unknown test row type");
    }

    QLandmark lm2;
    lm2.setName("LM2");
    QGeoAddress address;
    address.setStreet("LM2 street");
    address.setDistrict("LM2 district");
    address.setCity("LM2 city");
    address.setState("LM2 State");
    address.setCountry("LM2 Country");
    address.setCountryCode("LM2CountryCode");
    address.setPostCode("LM2 post code");
    lm2.setAddress(address);
    QGeoCoordinate coordinate(10,20);
    lm2.setCoordinate(coordinate);
    lm2.setPhoneNumber("2nd phone number");
    lm2.setIconUrl(QUrl("2nd iconUrl"));
    lm2.setRadius(2000);
    lm2.setUrl(QUrl("2nd url"));
    QVERIFY(m_manager->saveLandmark(&lm2));

    QLandmarkId id2;
    id2.setManagerUri(m_manager->managerUri());
    id2.setLocalId(lm2.landmarkId().localId());
    if (type == "sync") {
        id2.setManagerUri("wrong.manager");
        id2.setLocalId(lm2.landmarkId().localId());
        QCOMPARE(m_manager->landmark(id2), QLandmark());
        QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
        QCOMPARE(m_manager->landmark(id2).landmarkId().isValid(), false);

        id2 = lm2.landmarkId();
        QCOMPARE(m_manager->landmark(id2), lm2);
        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QCOMPARE(m_manager->landmark(id2).landmarkId().isValid(), true);
    } else if (type == "async") {
        //try a manager that doesn't match
        id2.setManagerUri("wrong.manager");
        id2.setLocalId(lm2.landmarkId().localId());
        lmFetchByIdRequest.setLandmarkId(id2);
        lmFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &lmFetchByIdRequest, QLandmarkManager::DoesNotExistError,100));
        QCOMPARE(lmFetchByIdRequest.errorMap().count(),1);
        QCOMPARE(lmFetchByIdRequest.errorMap().keys().at(0),0);
        QCOMPARE(lmFetchByIdRequest.errorMap().value(0), QLandmarkManager::DoesNotExistError);

        lms = lmFetchByIdRequest.landmarks();
        QCOMPARE(lms.count(), 1);
        QCOMPARE(lms.at(0), QLandmark());
        QCOMPARE(lms.at(0).landmarkId().isValid(), false);

        //try an existing id
        id2 = lm2.landmarkId();
        lmFetchByIdRequest.setLandmarkId(id2);
        lmFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &lmFetchByIdRequest, QLandmarkManager::NoError,100));
        QCOMPARE(lmFetchByIdRequest.errorMap().count(),0);

        lms = lmFetchByIdRequest.landmarks();
        QCOMPARE(lms.count(), 1);
        QCOMPARE(lms.at(0), lm2);
        QCOMPARE(lms.at(0).landmarkId().isValid(), true);
    } else {
        qFatal("Unkown test row type");
    }

    //try retrieving a landmark with a category
    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("CAT3");
    QVERIFY(m_manager->saveCategory(&cat3));

    QLandmark lm3;
    lm3.setName("LM3");
    lm3.addCategoryId(cat1.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm3));

    QLandmark lm4;
    lm4.setName("LM4");
    lm4.addCategoryId(cat3.categoryId());
    lm4.addCategoryId(cat2.categoryId());
    QVERIFY(m_manager->saveLandmark(&lm4));

    QLandmarkId id3;
    id3 = lm3.landmarkId();

    QLandmarkId id4;
    id4 = lm4.landmarkId();

    if (type == "sync") {
        //check that we can retrieve a landmark a single catergory
        QLandmark lm3Retrieved = m_manager->landmark(id3);
        QCOMPARE(lm3Retrieved, lm3);
        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QList<QLandmarkCategoryId> lm3RetrievedCatIds;
        lm3RetrievedCatIds = lm3Retrieved.categoryIds();
        QCOMPARE(lm3RetrievedCatIds.count(),1);
        QVERIFY(lm3RetrievedCatIds.contains(cat1.categoryId()));

        //check that we can retrieve a landmark with multiple categories
        QLandmark lm4Retrieved = m_manager->landmark(id4);
        QCOMPARE(lm4Retrieved, lm4);
        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);

        QList<QLandmarkCategoryId> lm4RetrievedCatIds;
        lm4RetrievedCatIds = lm4Retrieved.categoryIds();
        QCOMPARE(lm4RetrievedCatIds.count(),2);
        QVERIFY(lm4RetrievedCatIds.contains(cat2.categoryId()));
        QVERIFY(lm4RetrievedCatIds.contains(cat3.categoryId()));
    } else if (type == "async") {
        //check that we can retrieve a landmark a single catergory
        lmFetchByIdRequest.setLandmarkId(id3);
        lmFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &lmFetchByIdRequest, QLandmarkManager::NoError,100));
        QCOMPARE(lmFetchByIdRequest.errorMap().count(),0);

        lms = lmFetchByIdRequest.landmarks();
        QCOMPARE(lms.count(), 1);
        QLandmark lm3Retrieved = lms.at(0);
        QCOMPARE(lm3Retrieved, lm3);
        QList<QLandmarkCategoryId> lm3RetrievedCatIds = lms.at(0).categoryIds();
        QCOMPARE(lm3RetrievedCatIds.count(),1);
        QVERIFY(lm3RetrievedCatIds.contains(cat1.categoryId()));

        //try an existing id
        lmFetchByIdRequest.setLandmarkId(id4);
        lmFetchByIdRequest.start();
        QVERIFY(waitForAsync(spy, &lmFetchByIdRequest, QLandmarkManager::NoError,100));
        QCOMPARE(lmFetchByIdRequest.errorMap().count(),0);

        lms = lmFetchByIdRequest.landmarks();
        QCOMPARE(lms.count(), 1);
        QLandmark lm4Retrieved = lms.at(0);
        QCOMPARE(lm4Retrieved, lm4);

        QList<QLandmarkCategoryId> lm4RetrievedCatIds = lms.at(0).categoryIds();
        QCOMPARE(lm4RetrievedCatIds.count(),2);
        QVERIFY(lm4RetrievedCatIds.contains(cat2.categoryId()));
        QVERIFY(lm4RetrievedCatIds.contains(cat3.categoryId()));
    } else {
        qFatal("Unknown test row type");
    }
}

void tst_QLandmarkManager::retrieveLandmark_data() {
    QTest::addColumn<QString>("type");

    QTest::newRow("sync") << "sync";
    QTest::newRow("async") << "async";
}
*/

#ifdef SAVE_CATEGORY
void tst_QLandmarkManager::saveCategory() {
    QFETCH(QString, type);
    QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
    QSignalSpy spyChange(m_manager,SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));
    QSignalSpy spyRemove(m_manager,SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));

    int originalCategoryCount = m_manager->categoryIds().count();
    QCOMPARE(originalCategoryCount, m_manager->categories().count());

    //try save an empty category name
    QLandmarkCategory emptyCategory;
    QVERIFY(doSingleCategorySave(type,&emptyCategory,QLandmarkManager::BadArgumentError));
    QVERIFY(checkCategoryCount(originalCategoryCount));

    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    cat1.setIconUrl(QUrl("CAT1Url"));

    QLandmarkCategory cat1Initial = cat1;
    QVERIFY(doSingleCategorySave(type,&cat1,QLandmarkManager::NoError));
    cat1Initial.setCategoryId(cat1.categoryId());
    QCOMPARE(cat1, cat1Initial);

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.categoryId());
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyRemove.count(), 0);
    spyAdd.clear();
    QVERIFY(checkCategoryCount(originalCategoryCount+1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(doSingleCategorySave(type, &cat2, QLandmarkManager::NoError));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat2.categoryId());
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyRemove.count(), 0);
    spyAdd.clear();

    QVERIFY(checkCategoryCount(originalCategoryCount + 2));
    int newCategoryCount = originalCategoryCount + 2;

   //try adding a category when the category name already exists
   QLandmarkCategory cat1Duplicate;
   cat1Duplicate.setName("CAT1");
   QVERIFY(doSingleCategorySave(type, &cat1Duplicate,QLandmarkManager::AlreadyExistsError));
   QCOMPARE(m_manager->categories().count(), newCategoryCount);
   QVERIFY(checkCategoryCount(newCategoryCount));

   //try renaming an existing category to a name that already exists
   QLandmarkCategory cat2Original = cat2;
   QLandmarkCategoryId cat2OriginalId = cat2.categoryId();
   cat2.setName("CAT1");
   QVERIFY(doSingleCategorySave(type, &cat2,QLandmarkManager::AlreadyExistsError));
   QVERIFY(checkCategoryCount(newCategoryCount));
   QCOMPARE(m_manager->category(cat2OriginalId).name(), QString("CAT2"));

   //try save a category whose manager uri does not match
   cat2.setName("CAT2next");
   QLandmarkCategoryId id;
   id = cat2.categoryId();
   id.setManagerUri("wrong.manager");
   cat2.setCategoryId(id);
   QVERIFY(doSingleCategorySave(type, &cat2, QLandmarkManager::DoesNotExistError));
   QCOMPARE(m_manager->category(cat2OriginalId).name(), QString("CAT2"));
   QVERIFY(checkCategoryCount(newCategoryCount));

   QLandmarkCategory accommodation;
   QList<QLandmarkCategory> cats = m_manager->categories();
   for (int i=0; i < cats.count(); ++i) {
       if (cats.at(i).name() == "Accommodation")
           accommodation = cats.at(i);
   }

#ifdef Q_OS_SYMBIAN
   //check that global category is read-only
   QVERIFY(m_manager->isReadOnly(accommodation.categoryId()));

#ifdef UNRESOLVED_SAVING_GLOBAL_CATEGORY
   //try save a global category
   m_manager->saveCategory(&accommodation);
   QVERIFY(doSingleCategorySave(type, &accommodation, QLandmarkManager::PermissionsError));

   //try rename a global category;
   accommodation.setName("hotels");
   m_manager->saveCategory(&accommodation);
   QVERIFY(doSingleCategorySave(type, &accommodation, QLandmarkManager::PermissionsError));
#endif

   //try save a (new) category with the same name as a global category
   QLandmarkCategory accommodationDuplicate;
   accommodationDuplicate.setName("Accommodation");
   QVERIFY(doSingleCategorySave(type, &accommodationDuplicate, QLandmarkManager::AlreadyExistsError));


#endif

   //try modify a category that does not exist
   QLandmarkCategory catDoesNotExist = cat2Original;
   catDoesNotExist.setName("DoesNotExist");
   QLandmarkCategoryId catDoesNotExistId = cat2Original.categoryId();
   catDoesNotExistId.setLocalId("10000");
   catDoesNotExist.setCategoryId(catDoesNotExistId);
   QVERIFY(doSingleCategorySave(type, &catDoesNotExist,QLandmarkManager::DoesNotExistError));
   QVERIFY(checkCategoryCount(newCategoryCount));

   QTest::qWait(10);
   QCOMPARE(spyAdd.count(), 0);
   QCOMPARE(spyChange.count(), 0);
   QCOMPARE(spyRemove.count(), 0);

   //try modify a category that does already exist
   QLandmarkCategory cat2Modified = cat2Original;
   cat2Modified.setName("CAT2Modified");
   cat2Modified.setIconUrl(QUrl("cat2 url modified"));
   QVERIFY(doSingleCategorySave(type,&cat2Modified,QLandmarkManager::NoError));
   QVERIFY(checkCategoryCount(newCategoryCount));
   QCOMPARE(m_manager->category(cat2OriginalId).name(), QString("CAT2Modified"));
   QCOMPARE(m_manager->category(cat2OriginalId).iconUrl(), QUrl("cat2 url modified"));

   QTest::qWait(10);
   QCOMPARE(spyAdd.count(), 0);
   QCOMPARE(spyChange.count(), 1);
   QCOMPARE(spyRemove.count(), 0);
   spyChange.clear();

   if (type == "async"){
       //save multiple categories where the
       //first is a new category, second should fail, third is an update
       QLandmarkCategory catNew;
       catNew.setName("CATNew");

       QLandmarkCategory catBad;
       catBad.setName("CATBad");
       QLandmarkCategoryId catBadId;
       catBadId = cat2Modified.categoryId();
       catBadId.setLocalId("1000");
       catBad.setCategoryId(catBadId);

       QLandmarkCategory catChange = cat2Modified;
       catChange.setName("CATChange");

       QList<QLandmarkCategory> cats;
       cats << catNew << catBad << catChange;
       QLandmarkCategorySaveRequest saveCategoryRequest(m_manager);
       QSignalSpy spy(&saveCategoryRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
       saveCategoryRequest.setCategories(cats);
       saveCategoryRequest.start();
       QVERIFY(waitForAsync(spy, &saveCategoryRequest, QLandmarkManager::DoesNotExistError));
       QCOMPARE(saveCategoryRequest.categories().count(), 3);
       QCOMPARE(saveCategoryRequest.errorMap().count(),1);
       QCOMPARE(saveCategoryRequest.errorMap().value(1) , QLandmarkManager::DoesNotExistError);

       QLandmarkCategoryId firstCategoryId = saveCategoryRequest.categories().at(0).categoryId();
       QLandmarkCategoryId secondCategoryId = saveCategoryRequest.categories().at(1).categoryId();
       QLandmarkCategoryId thirdCategoryId = saveCategoryRequest.categories().at(2).categoryId();

       //check that the new and changed categories will match the originals
       //(which wouldn't have category ids so we just assign those now)
       catNew.setCategoryId(firstCategoryId);
       catChange.setCategoryId(thirdCategoryId);

       QCOMPARE(catNew, m_manager->category(firstCategoryId));
       QVERIFY(!m_manager->category(secondCategoryId).categoryId().isValid());
       QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
       QCOMPARE(catChange, m_manager->category(thirdCategoryId));

       QCOMPARE(catNew, saveCategoryRequest.categories().at(0));
       QCOMPARE(catBad, saveCategoryRequest.categories().at(1));
       QCOMPARE(catChange, saveCategoryRequest.categories().at(2));

       QTest::qWait(10);
       QCOMPARE(spyAdd.count(), 1);
       QCOMPARE(spyChange.count(), 1);
       QCOMPARE(spyRemove.count(), 0);
       QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), firstCategoryId);
       QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), thirdCategoryId);
       spyAdd.clear();
       spyChange.clear();

       //try saving multiple new categories
       QLandmarkCategory catNew1;
       catNew1.setName("CATNew1");
       catNew1.setIconUrl(QUrl("CATNew1 iconUrl"));

       QLandmarkCategory catNew2;
       catNew2.setName("CATNew2");
       catNew2.setIconUrl(QUrl("CATNew2 iconUrl"));

       QLandmarkCategory catNew3;
       catNew3.setName("CATNew3");
       catNew3.setIconUrl(QUrl("CATNew3 iconUrl"));
       cats.clear();
       cats << catNew1 << catNew2 << catNew3;
       saveCategoryRequest.setCategories(cats);
       saveCategoryRequest.start();
       QVERIFY(waitForAsync(spy, &saveCategoryRequest, QLandmarkManager::NoError));
       QCOMPARE(saveCategoryRequest.categories().count(), 3);
       QCOMPARE(saveCategoryRequest.errorMap().count(),0);

       firstCategoryId = saveCategoryRequest.categories().at(0).categoryId();
       secondCategoryId = saveCategoryRequest.categories().at(1).categoryId();
       thirdCategoryId = saveCategoryRequest.categories().at(2).categoryId();

       catNew1.setCategoryId(firstCategoryId);
       catNew2.setCategoryId(secondCategoryId);
       catNew3.setCategoryId(thirdCategoryId);
       QCOMPARE(catNew1, m_manager->category(firstCategoryId));
       QCOMPARE(catNew2, m_manager->category(secondCategoryId));
       QCOMPARE(catNew3, m_manager->category(thirdCategoryId));

       QCOMPARE(catNew1, saveCategoryRequest.categories().at(0));
       QCOMPARE(catNew2, saveCategoryRequest.categories().at(1));
       QCOMPARE(catNew3, saveCategoryRequest.categories().at(2));

       QTest::qWait(10);
       QCOMPARE(spyAdd.count(), 1);
       QCOMPARE(spyChange.count(), 0);
       QCOMPARE(spyRemove.count(), 0);
       QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), firstCategoryId);
       QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(1), secondCategoryId);
       QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(2), thirdCategoryId);
       spyAdd.clear();

       //try changing multiple categories
       catNew1.setName("CATNew1Changed");
       catNew2.setIconUrl(QUrl("CATNew2 iconUrlChanged"));
       catNew3.setName("CATNew3Changed");
       catNew3.setIconUrl(QUrl("catNew3Changed"));
       cats.clear();
       cats << catNew1 << catNew2 << catNew3;
       saveCategoryRequest.setCategories(cats);
       saveCategoryRequest.start();
       QVERIFY(waitForAsync(spy, &saveCategoryRequest, QLandmarkManager::NoError));
       QCOMPARE(saveCategoryRequest.categories().count(), 3);
       QCOMPARE(saveCategoryRequest.errorMap().count(),0);

       firstCategoryId = saveCategoryRequest.categories().at(0).categoryId();
       secondCategoryId = saveCategoryRequest.categories().at(1).categoryId();
       thirdCategoryId = saveCategoryRequest.categories().at(2).categoryId();

       QCOMPARE(catNew1, m_manager->category(firstCategoryId));
       QCOMPARE(catNew2, m_manager->category(secondCategoryId));
       QCOMPARE(catNew3, m_manager->category(thirdCategoryId));

       QCOMPARE(catNew1, saveCategoryRequest.categories().at(0));
       QCOMPARE(catNew2, saveCategoryRequest.categories().at(1));
       QCOMPARE(catNew3, saveCategoryRequest.categories().at(2));

       QTest::qWait(10);
       QCOMPARE(spyAdd.count(), 0);
       QCOMPARE(spyChange.count(), 1);
       QCOMPARE(spyRemove.count(), 0);
       QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), firstCategoryId);
       QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(1), secondCategoryId);
       QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(2), thirdCategoryId);
       spyChange.clear();
   }
}


void tst_QLandmarkManager::saveCategory_data()
{
    QTest::addColumn<QString>("type");

    QTest::newRow("sync") << "sync";
    QTest::newRow("async") << "async";
}
#endif


#ifdef SAVE_LANDMARK
void tst_QLandmarkManager::saveLandmark() {
    QFETCH(QString, type);
    QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
    QSignalSpy spyChange(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)));
    QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));

     int originalLandmarkCount = m_manager->landmarks().count();
    QLandmark emptyLandmark;
    QVERIFY(doSingleLandmarkSave(type, &emptyLandmark,QLandmarkManager::NoError));
    QVERIFY(checkLandmarkCount(originalLandmarkCount + 1));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyRemove.count(),0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), emptyLandmark.landmarkId());
    spyAdd.clear();

    QLandmarkCategory cat1;
    cat1.setName("CAT1");
    QVERIFY(m_manager->saveCategory(&cat1));

    QLandmarkCategory cat2;
    cat2.setName("CAT2");
    QVERIFY(m_manager->saveCategory(&cat2));

    QLandmarkCategory cat3;
    cat3.setName("cat3");
    QVERIFY(m_manager->saveCategory(&cat3));

    //save a new landamrk
    QLandmark lm1;
    lm1.setName("LM1");
    QGeoAddress address;
    address.setStreet("LM1 street");
    address.setDistrict("LM1 district");
    address.setCity("LM1 city");
    address.setState("LM1 State");
    address.setCountry("LM1 Country");
    address.setCountryCode("LM1CountryCode");
    address.setPostCode("LM1 post code");
    lm1.setAddress(address);
    QGeoCoordinate coordinate(10,20);
    lm1.setCoordinate(coordinate);
    lm1.setPhoneNumber("LM1 phone number");
    lm1.setIconUrl(QUrl("LM1 iconUrl"));
    lm1.setRadius(1000);
    lm1.setUrl(QUrl("LM1 url"));
    lm1.addCategoryId(cat2.categoryId());
    lm1.addCategoryId(cat3.categoryId());
    QLandmark lm1Initial = lm1;

    QVERIFY(doSingleLandmarkSave(type, &lm1,QLandmarkManager::NoError));
    QVERIFY(checkLandmarkCount(originalLandmarkCount +2));
    int newLandmarkCount = originalLandmarkCount + 2;

    QCOMPARE(lm1, m_manager->landmark(lm1.landmarkId()));
    lm1Initial.setLandmarkId(lm1.landmarkId());
    QCOMPARE(lm1,lm1Initial);

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyRemove.count(),0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.landmarkId());
    spyAdd.clear();

    //update a landmark
    QLandmark lm1Changed = lm1;
    lm1Changed.setName("LM1Changed");
    address.setStreet("LM1Changed street");
    address.setDistrict("LM1Changed district");
    address.setCity("LM1Changed city");
    address.setState("LM1Changed State");
    address.setCountry("LM1Changed Country");
    address.setCountryCode("LM1Changed CountryCode");
    address.setPostCode("LM1Changed Post code");
    lm1Changed.setAddress(address);
    coordinate.setLatitude(11);
    coordinate.setLongitude(21);
    lm1Changed.setCoordinate(coordinate);
    lm1Changed.setPhoneNumber("LM1Changed phone number");
    lm1Changed.setIconUrl(QUrl("LM1Changed iconUrl"));
    lm1Changed.setRadius(1100);
    lm1Changed.setUrl(QUrl("LM1Changed url"));
    lm1Changed.removeCategoryId(cat3.categoryId());
    lm1Changed.removeCategoryId(cat2.categoryId());
    lm1Changed.addCategoryId(cat1.categoryId());
    QLandmark lm1ChangedInitial = lm1Changed;

    QVERIFY(doSingleLandmarkSave(type, &lm1Changed, QLandmarkManager::NoError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(lm1ChangedInitial, m_manager->landmark(lm1.landmarkId()));
    QCOMPARE(lm1ChangedInitial, lm1Changed);

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyRemove.count(),0);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1Changed.landmarkId());
    spyChange.clear();

    //try clearing one of the fields
#ifdef RESTORE_INVALID_COORD
    lm1Changed.setPhoneNumber("");
    QVERIFY(doSingleLandmarkSave(type, &lm1Changed, QLandmarkManager::NoError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(m_manager->landmark(lm1Changed.landmarkId()).phoneNumber(),QString());

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyRemove.count(),0);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1Changed.landmarkId());
    spyChange.clear();


    //try saving a landmark with an invalid coordinate
    //i.e. one of latitude/longitude is Nan and the other is a valid number
    QLandmark lmInvalidCoordinate;
    lmInvalidCoordinate.setName("LMInvalid");
    QGeoCoordinate invalidCoordinate;
    invalidCoordinate.setLatitude(20);
    lmInvalidCoordinate.setCoordinate(invalidCoordinate);
    QLandmark lmInvalidCoordinateInitial = lmInvalidCoordinate;
    m_manager->saveLandmark(&lmInvalidCoordinate);

    QVERIFY(doSingleLandmarkSave(type, &lmInvalidCoordinate,QLandmarkManager::BadArgumentError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(lmInvalidCoordinate, lmInvalidCoordinateInitial);

    //try saving a landmark where a coordinate is out of range
    QLandmark lmOutOfRange;
    lmOutOfRange.setName("lmOutOfRange");
    QGeoCoordinate outOfRangeCoord(91,45);
    lmOutOfRange.setCoordinate(outOfRangeCoord);
    QLandmark lmOutOfRangeInitial = lmOutOfRange;
    QVERIFY(doSingleLandmarkSave(type, &lmOutOfRange,QLandmarkManager::BadArgumentError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(lmOutOfRange, lmOutOfRangeInitial);
#endif

    //try saving a landmark with a category that doesn't exist
    QLandmarkCategoryId catIdNotExist;
    catIdNotExist.setLocalId("1000");
    catIdNotExist.setManagerUri(m_manager->managerUri());
    QLandmark lmCatNotExist;
    lmCatNotExist.setName("LMCatNoExist");
    lmCatNotExist.addCategoryId(cat1.categoryId());
    lmCatNotExist.addCategoryId(catIdNotExist);
    QLandmark lmCatNotExistInitial = lmCatNotExist;
    QVERIFY(doSingleLandmarkSave(type, &lmCatNotExist,QLandmarkManager::BadArgumentError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(lmCatNotExist, lmCatNotExistInitial);

    //try saving a landmark with a category belonging to a different manager
    QLandmarkCategoryId catIdManagerMismatch;
    catIdManagerMismatch.setLocalId(cat1.categoryId().localId());
    catIdManagerMismatch.setManagerUri("wrong.manager");
    QLandmark lmCatManagerMismatch;
    lmCatManagerMismatch.setName("LMCatManagerMismatch");
    lmCatManagerMismatch.addCategoryId(catIdManagerMismatch);
    lmCatManagerMismatch.addCategoryId(cat1.categoryId());
    QLandmark lmCatManagerMismatchInitial = lmCatManagerMismatch;
    m_manager->saveLandmark(&lmCatManagerMismatch);
    QVERIFY(doSingleLandmarkSave(type, &lmCatManagerMismatch, QLandmarkManager::BadArgumentError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(lmCatManagerMismatch, lmCatManagerMismatchInitial);

    //try removing a category from a landmark
    QLandmark lm2;
    lm2.setName("LM2");
    lm2.addCategoryId(cat2.categoryId());
    QVERIFY(doSingleLandmarkSave(type, &lm2,QLandmarkManager::NoError));
    QVERIFY(m_manager->landmark(lm2.landmarkId()).categoryIds().contains(cat2.categoryId()));
    newLandmarkCount = newLandmarkCount +1;
    lm2.removeCategoryId(cat2.categoryId());
    QVERIFY(doSingleLandmarkSave(type, &lm2, QLandmarkManager::NoError));
    QCOMPARE(m_manager->landmark(lm2.landmarkId()).categoryIds().count(), 0);

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyRemove.count(),0);
    spyAdd.clear();
    spyChange.clear();

    //try saving a landmark which has an id but does not exist
    QLandmark lmNotExist = lm1Changed;
    QLandmarkId lmIdNotExist;
    lmIdNotExist.setLocalId("2000");
    lmIdNotExist.setManagerUri(m_manager->managerUri());
    lmNotExist.setLandmarkId(lmIdNotExist);
    QLandmark lmNotExistInitial = lmNotExist;
    QVERIFY(doSingleLandmarkSave(type, &lmNotExist, QLandmarkManager::DoesNotExistError));
    QVERIFY(checkLandmarkCount(newLandmarkCount));
    QCOMPARE(lmNotExist, lmNotExistInitial);

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyRemove.count(),0);

    //try saving multiple landamrks the first is new, the second should fail and the third is an update
    QLandmark lmNew;
    lmNew.setName("LMNew");
    address.clear();
    address.setStreet("LMNew Street");
    lmNew.setAddress(address);

    QLandmark lmBad;
    lmBad.setName("LMBad");
    address.clear();
    address.setStreet("LMBad Street");
    lmBad.setAddress(address);
    QLandmarkId lmBadId;
    lmBadId = lm1Changed.landmarkId();
    lmBadId.setLocalId("1000");
    lmBad.setLandmarkId(lmBadId);

    QLandmark lmChange = lm1Changed;
    lmChange.setName("LMChange");
    address.clear();
    address.setStreet("LMChange Street");
    lmChange.setAddress(address);

    QList<QLandmark> lms;
    lms << lmNew << lmBad << lmChange;
    QMap<int, QLandmarkManager::Error> errorMap;
    QVERIFY(doSave(type, &lms,QLandmarkManager::DoesNotExistError,&errorMap));
    QCOMPARE(lms.count(), 3);
    QCOMPARE(errorMap.count(), 1);
    QCOMPARE(errorMap.value(1), QLandmarkManager::DoesNotExistError);

    QLandmarkId firstLandmarkId = lms.at(0).landmarkId();
    QLandmarkId secondLandmarkId = lms.at(1).landmarkId();
    QLandmarkId thirdLandmarkId = lms.at(2).landmarkId();

    lmNew.setLandmarkId(firstLandmarkId);
    lmChange.setLandmarkId(thirdLandmarkId);

    QCOMPARE(lmNew, m_manager->landmark(firstLandmarkId));
    QCOMPARE(QLandmark(),m_manager->landmark(secondLandmarkId));
    QCOMPARE(m_manager->error(), QLandmarkManager::DoesNotExistError);
    QCOMPARE(lmChange, m_manager->landmark(thirdLandmarkId));

    QCOMPARE(lmNew, lms.at(0));
    QCOMPARE(lmBad, lms.at(1));
    QCOMPARE(lmChange, lms.at(2));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyRemove.count(), 0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), firstLandmarkId);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), thirdLandmarkId);
    spyAdd.clear();
    spyChange.clear();

    //try saving multiple new landmarks
    QLandmark lmNew1;
    lmNew1.setName("LMNew1");
    lmNew1.setDescription("LMNew1 description");

    QLandmark lmNew2;
    lmNew2.setName("LMNew2");
    coordinate.setLatitude(80);
    coordinate.setLongitude(80);
    lmNew2.setCoordinate(coordinate);

    QLandmark lmNew3;
    lmNew3.setName("LMNew3");
    address.clear();
    address.setCountry("LMNew3 Country");
    lmNew3.setAddress(address);

    lms.clear();
    lms << lmNew1 << lmNew2 << lmNew3;
    QVERIFY(doSave(type, &lms,QLandmarkManager::NoError, &errorMap));
    QCOMPARE(lms.count(), 3);
    QCOMPARE(errorMap.count(), 0);

    firstLandmarkId = lms.at(0).landmarkId();
    secondLandmarkId = lms.at(1).landmarkId();
    thirdLandmarkId = lms.at(2).landmarkId();

    lmNew1.setLandmarkId(firstLandmarkId);
    lmNew2.setLandmarkId(secondLandmarkId);
    lmNew3.setLandmarkId(thirdLandmarkId);

    QCOMPARE(lmNew1, m_manager->landmark(firstLandmarkId));
    QCOMPARE(lmNew2, m_manager->landmark(secondLandmarkId));
    QCOMPARE(lmNew3, m_manager->landmark(thirdLandmarkId));

    QCOMPARE(lmNew1, lms.at(0));
    QCOMPARE(lmNew2, lms.at(1));
    QCOMPARE(lmNew3, lms.at(2));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 1);
    QCOMPARE(spyChange.count(), 0);
    QCOMPARE(spyRemove.count(), 0);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), firstLandmarkId);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(1), secondLandmarkId);
    QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(2), thirdLandmarkId);
    spyAdd.clear();

    //try updating multiple landmarks
    lmNew1.setDescription("");
    lmNew2.setRadius(50);
    coordinate.setLatitude(1);
    coordinate.setLongitude(2);
    lmNew3.setCoordinate(coordinate);
    lms.clear();
    lms << lmNew1 << lmNew2 << lmNew3;
    QVERIFY(doSave(type, &lms, QLandmarkManager::NoError, &errorMap));
    QCOMPARE(lms.count(), 3);
    QCOMPARE(errorMap.count(), 0);

    lmNew1.setLandmarkId(firstLandmarkId);
    lmNew2.setLandmarkId(secondLandmarkId);
    lmNew3.setLandmarkId(thirdLandmarkId);

    QCOMPARE(lmNew1, m_manager->landmark(firstLandmarkId));
    QCOMPARE(lmNew2, m_manager->landmark(secondLandmarkId));
    QCOMPARE(lmNew3, m_manager->landmark(thirdLandmarkId));

    QCOMPARE(lmNew1, lms.at(0));
    QCOMPARE(lmNew2, lms.at(1));
    QCOMPARE(lmNew3, lms.at(2));

    QTest::qWait(10);
    QCOMPARE(spyAdd.count(), 0);
    QCOMPARE(spyChange.count(), 1);
    QCOMPARE(spyRemove.count(), 0);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), firstLandmarkId);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(1), secondLandmarkId);
    QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(2), thirdLandmarkId);
    spyChange.clear();

    if (type == "async") {
        //check that errorMap is cleared for async landmark save request
        QLandmarkSaveRequest lmSaveRequest(m_manager);
        QSignalSpy spy(&lmSaveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)));
        QSignalSpy spyResults(&lmSaveRequest, SIGNAL(resultsAvailable()));
        lms.clear();
        lms << lmNew << lmBad << lmChange;
        lmSaveRequest.setLandmarks(lms);
        lmSaveRequest.start();
        QVERIFY(waitForAsync(spy, &lmSaveRequest, QLandmarkManager::DoesNotExistError));
        QCOMPARE(lmSaveRequest.errorMap().count(), 1);
        QCOMPARE(lmSaveRequest.errorMap().value(1), QLandmarkManager::DoesNotExistError);
        QCOMPARE(spyResults.count(), 1);
        spyResults.clear();

        lms.clear();
        lms << lmNew1 << lmNew2 << lmNew3;
        lmSaveRequest.setLandmarks(lms);
        lmSaveRequest.start();
        QVERIFY(waitForAsync(spy, &lmSaveRequest, QLandmarkManager::NoError));
        QCOMPARE(lmSaveRequest.errorMap().count(), 0);
        QCOMPARE(spyResults.count(), 1);
    }
}

void tst_QLandmarkManager::saveLandmark_data() {
    QTest::addColumn<QString>("type");

    QTest::newRow("sync") << "sync";
    QTest::newRow("async") << "async";
}
#endif

QTEST_MAIN(tst_QLandmarkManager)
#include "tst_qlandmarkmanager.moc"


