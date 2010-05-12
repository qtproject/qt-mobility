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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <qlandmark.h>
#include <qlandmarkmanagerengine.h>


#include <qlandmark.h>
#include <qlandmarkid.h>
#include <qlandmarkcategory.h>
#include <qlandmarkcategoryid.h>
#include <qlandmarkfilter.h>
#include <qlandmarksortorder.h>

#include <qlandmarkattributefilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkcategoryfilter.h>

QTM_USE_NAMESPACE


//mock object so we can access protected static functions
//from qlandmarkmanagerengine.
class MockEngine : public QLandmarkManagerEngine
{
    Q_OBJECT
public:
    MockEngine();
    ~MockEngine() {}

    /* URI reporting */
    QString managerName() {return "mock";}
    QMap<QString, QString> managerParameters() const {return QMap<QString, QString>();}  // e.g. "filename=private.db"
    int managerVersion() const {return 1;}

    QString managerUri() const { return "com.mock";}

    /* Filtering */
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter &filter, const QList<QLandmarkSortOrder>& sortOrders,
                                           QLandmarkManager::Error *error, QString *errorString) const
    {
        return QList<QLandmarkId>();
    }

    QList<QLandmarkCategoryId> categoryIds(QLandmarkManager::Error *error,
            QString *errorString) const{
        return QList<QLandmarkCategoryId>();
     }

    /* Retrieval */
    QLandmark landmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
                               QString *errorString) const
    {
        return QLandmark();
    }
    QList<QLandmark> landmarks(const QLandmarkFilter &filter, const QList<QLandmarkSortOrder>& sortOrders,
                                       QLandmarkManager::Error *error, QString *errorString) const
    {
        return QList<QLandmark>();
    }

    QList<QLandmark> landmarks(const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
                                       QString *errorString) const
    {
        return QList<QLandmark>();
    }

    QLandmarkCategory category(const QLandmarkCategoryId &landmarkCategoryId, QLandmarkManager::Error *error,
                                       QString *errorString) const
    {
        return QLandmarkCategory();
    }

    QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds, QLandmarkManager::Error *error,
            QString *errorString) const
    {
        return QList<QLandmarkCategory>();
    }

    /*saving and removing*/
    bool saveLandmark(QLandmark* landmark, QLandmarkManager::Error *error, QString *errorString) { return false;}
    bool saveLandmarks(QList<QLandmark> * landmark, QMap<int, QLandmarkManager::Error> *errorMap,
                               QLandmarkManager::Error *error, QString *errorString) {return false;}
    bool removeLandmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) {return false;}
    bool removeLandmarks(const QList<QLandmarkId> &landmarkId, QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
                                 QString *errorString) { return false;}

    bool saveCategory(QLandmarkCategory* category, QLandmarkManager::Error *error, QString *errorString) { return false;}
    bool removeCategory(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString){return false;}

    bool importLandmarks(QIODevice *device, const QByteArray &format, QLandmarkManager::Error *error,
                                 QString *errorString) { return false;}
    bool exportLandmarks(QIODevice *device, const QByteArray &format, QList<QLandmarkId> landmarkIds,
                                 QLandmarkManager::Error *error, QString *errorString) const { return false;}

    bool isFilterSupported(QLandmarkFilter::FilterType filterType) const { return false;}

    bool isReadOnly() const{ return false;}
    bool isReadOnly(const QLandmarkId &landmarkId) const{ return false;}
    bool isReadOnly(const QLandmarkCategoryId &categoryId) const{ return false;}

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QLandmarkAbstractRequest* request) { return; }
    virtual bool startRequest(QLandmarkAbstractRequest* request) { return false; }
    virtual bool cancelRequest(QLandmarkAbstractRequest* request) { return false; }
    virtual bool waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs) { return false;}

    static bool testFilter(const QLandmarkFilter& filter, const QLandmark& landmark) {
        return QLandmarkManagerEngine::testFilter(filter, landmark);
    }
};

class tst_QLandmarkManagerEngine : public QObject
{
    Q_OBJECT

public:
    tst_QLandmarkManagerEngine() {
    }

private slots:
    void testFiltterAttribute() {
        QLandmark lm;
        lm.setAttribute("capacity", 30);

        //test exact match
        QLandmarkAttributeFilter filter;
        filter.setAttribute("capacity", 30);

        QVERIFY(MockEngine::testFilter(filter,lm));

        //test no match
        lm.setAttribute("capacity",29);
        QVERIFY(!MockEngine::testFilter(filter, lm));

        //filter should match any value
        filter.setAttribute("capacity", QVariant());
        QVERIFY(MockEngine::testFilter(filter, lm));

        lm.setAttribute("capacity", 45);
        QVERIFY(MockEngine::testFilter(filter, lm));
    }

    void testFilterBox() {
        QLandmarkBoxFilter boxFilter;
        boxFilter.setTopLeftCoordinate(QGeoCoordinate(20,30));
        boxFilter.setBottomRightCoordinate(QGeoCoordinate(10,40));

        //landmark is in box
        QLandmark lm;
        lm.setCoordinate(QGeoCoordinate(15,35));
        QVERIFY(MockEngine::testFilter(boxFilter,lm));

        //landmark is outside box
        lm.setCoordinate(QGeoCoordinate(50,50));
        QVERIFY(!MockEngine::testFilter(boxFilter,lm));

        //test landmark inside box when box crosses dateline
        boxFilter.setTopLeftCoordinate(QGeoCoordinate(20,170));
        boxFilter.setBottomRightCoordinate(QGeoCoordinate(10,-170));

        lm.setCoordinate(QGeoCoordinate(15,-175));
        QVERIFY(MockEngine::testFilter(boxFilter, lm));

        lm.setCoordinate(QGeoCoordinate(15, 175));
        QVERIFY(MockEngine::testFilter(boxFilter, lm));


        //test landmark outside box when box crosses dateline
        lm.setCoordinate(QGeoCoordinate(15, 160));
        QVERIFY(!MockEngine::testFilter(boxFilter, lm));

        lm.setCoordinate(QGeoCoordinate(15, -160));
        QVERIFY(!MockEngine::testFilter(boxFilter, lm));
    }

    void testFilterCategory() {
        //test category matches
        QLandmarkCategoryFilter categoryFilter;
        QLandmarkCategoryId catFilterId;
        catFilterId.setLocalId("1");
        catFilterId.setManagerUri("qtlandmarks:mock:");
        categoryFilter.setCategoryId(catFilterId);

        QLandmark lm;
        QLandmarkCategoryId lmCatId;
        lmCatId.setLocalId("1");
        lmCatId.setManagerUri("qtlandmarks:mock:");
        lm.addCategoryId(lmCatId);

        QVERIFY(MockEngine::testFilter(categoryFilter,lm));

        //test category id doesn't match
        lm.removeCategoryId(lmCatId);
        QVERIFY(lm.categoryIds().count() == 0);
        lmCatId.setLocalId("2");
        lm.addCategoryId(lmCatId);

        QVERIFY(!MockEngine::testFilter(categoryFilter,lm));

        //test category uri that doesn't match
        QList<QLandmarkCategoryId> catIdList;
        lm.setCategoryIds(catIdList);
        QVERIFY(lm.categoryIds().count() == 0);
        lmCatId.setLocalId("1");
        lmCatId.setManagerUri("qtlandmarks:fake:");
        catIdList.append(lmCatId);
        lm.setCategoryIds(catIdList);
        QVERIFY(lm.categoryIds().count() == 1);
        QVERIFY(!MockEngine::testFilter(categoryFilter,lm));


        //try match a category when the
        //landmark has multiple categories

        QLandmarkCategoryId lmCatId2;
        QLandmarkCategoryId lmCatId3;

        lmCatId.setLocalId("1");
        lmCatId.setManagerUri("qtlandmarks:mock:");

        lmCatId2.setLocalId("2");
        lmCatId2.setManagerUri("qtlandmarks:mock:");

        lmCatId3.setLocalId("3");
        lmCatId3.setManagerUri("qtlandmarks:mock:");

        catIdList.clear();
        catIdList << lmCatId << lmCatId2 << lmCatId3;
        lm.setCategoryIds(catIdList);

        catFilterId.setLocalId("2");
        catFilterId.setManagerUri("qtlandmarks:mock:");

        categoryFilter.setCategoryId(catFilterId);
        QVERIFY(MockEngine::testFilter(categoryFilter, lm));

        //category id doesn't match when the landmark
        //has multipl catgories
        catFilterId.setLocalId("4");
        categoryFilter.setCategoryId(catFilterId);
    }

    void testFilterDefault()
    {
        QLandmarkFilter filter;
        QLandmark lm;
        QVERIFY(MockEngine::testFilter(filter,lm));
    }


};

QTEST_MAIN(tst_QLandmarkManagerEngine)
#include "tst_qlandmarkmanagerengine.moc"

