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
#include <qlandmarkintersectionfilter.h>
#include <qlandmarkidfilter.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkproximityfilter.h>
#include <qlandmarkunionfilter.h>

#include <qlandmarknamesort.h>

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

    static int compareName(const QLandmark &a, const QLandmark &b, const QLandmarkNameSort &nameSort)
    {
        return QLandmarkManagerEngine::compareName(a,b,nameSort);
    }

    static int compareLandmark(const QLandmark& a, const QLandmark& b,
                       const QList<QLandmarkSortOrder>& sortOrders) {
        return QLandmarkManagerEngine::compareLandmark(a,b,sortOrders);
    }

    static QList<QLandmarkId> sortLandmarks(const QList<QLandmark>& landmarks,
                            const QList<QLandmarkSortOrder>& sortOrders)
    {
        return QLandmarkManagerEngine::sortLandmarks(landmarks, sortOrders);
    }
};

class tst_QLandmarkManagerEngine : public QObject
{
    Q_OBJECT

public:
    tst_QLandmarkManagerEngine() {
    }

private slots:
    void testFilterAttribute() {
        QLandmark lm;
        QGeoAddress address;
        address.setStreet("lm1 street");
        address.setCountry("Australia");
        lm.setAddress(address);
        lm.setDescription("Down under");

        //test exact match
        QLandmarkAttributeFilter filter;
        filter.setAttribute("street", "lm1 street");

        QVERIFY(MockEngine::testFilter(filter,lm));

        //test no match
        filter.setAttribute("street", "sesame street");
        QVERIFY(!MockEngine::testFilter(filter, lm));

        //test no match with an empty qvariant
        filter.setAttribute("street", QVariant());
        QVERIFY(!MockEngine::testFilter(filter, lm));


        //test multiple manager attributes AND operation
        filter.clearAttributes();
        filter.setAttribute("street", "lm", QLandmarkFilter::MatchStartsWith);
        filter.setAttribute("country", "Australia");
        filter.setAttribute("description", "Down under");
        QVERIFY(MockEngine::testFilter(filter, lm));

        //test that the AND operation fails when one attribute doesn't match
        filter.clearAttributes();
        filter.setAttribute("street", "lm", QLandmarkFilter::MatchEndsWith);
        filter.setAttribute("country", "australia");
        filter.setAttribute("description", "Down under");
        QVERIFY(!MockEngine::testFilter(filter, lm));

        //test multiple manager attributes OR operation
        filter.clearAttributes();
        filter.setOperationType(QLandmarkAttributeFilter::OrOperation);
        filter.setAttribute("street", "sesame", QLandmarkFilter::MatchStartsWith);
        filter.setAttribute("country", "Australia");
        filter.setAttribute("description", "Up over");
        QVERIFY(MockEngine::testFilter(filter,lm));

        //test multiple manager with an OR operation that doesn't match
        filter.clearAttributes();
        filter.setOperationType(QLandmarkAttributeFilter::OrOperation);
        filter.setAttribute("street", "sesame", QLandmarkFilter::MatchStartsWith);
        filter.setAttribute("country","united state of america");
        filter.setAttribute("description", "us");
        QVERIFY(!MockEngine::testFilter(filter,lm));
        //TODO: expand attribute search on different attribute types
            }

    void testFilterBox() {
        QLandmarkBoxFilter boxFilter(QGeoCoordinate(20,30),QGeoCoordinate(10,40));

        //landmark is in box
        QLandmark lm;
        lm.setCoordinate(QGeoCoordinate(15,35));
        QVERIFY(MockEngine::testFilter(boxFilter,lm));

        //landmark is outside box
        lm.setCoordinate(QGeoCoordinate(50,50));
        QVERIFY(!MockEngine::testFilter(boxFilter,lm));

        //test landmark inside box when box crosses dateline
        QGeoBoundingBox box;
        box.setTopLeft(QGeoCoordinate(20,170));
        box.setBottomRight(QGeoCoordinate(10,-170));
        boxFilter.setBoundingBox(box);

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

    void testFilterIntersection()
    {
        //test a negative match
        QLandmarkIntersectionFilter intersectionFilter;
        QLandmarkNameFilter nameFilter1("Sasuke");
        QLandmarkNameFilter nameFilter2("Kakashi");
        QLandmarkNameFilter nameFilter3("Itachi");

        QLandmark lm;
        lm.setName("Kakashi");

        intersectionFilter << nameFilter1 << nameFilter2 << nameFilter3;
        QVERIFY(!MockEngine::testFilter(intersectionFilter,lm));

        //test a positive match
        QLandmarkCategoryId cat1;
        cat1.setLocalId("konoha");
        cat1.setManagerUri("qtlandmarks:mock:");
        QLandmarkCategoryFilter categoryFilter(cat1);

        QLandmarkAttributeFilter attributeFilter;
        attributeFilter.setAttribute("description","sharingan");

        lm.setDescription("sharingan");
        lm.addCategoryId(cat1);

        intersectionFilter.clear();
        intersectionFilter << attributeFilter << categoryFilter << nameFilter2;

        QVERIFY(MockEngine::testFilter(intersectionFilter,lm));

        //test empty intersection filter
        intersectionFilter.clear();
        QVERIFY(!MockEngine::testFilter(intersectionFilter,lm));
    }

    void testFilterLandmarkId()
    {
        //test landmark id filter that matches
        QLandmarkId lmId1, lmId2, lmId3;
        lmId1.setLocalId("1");
        lmId1.setManagerUri("qtlandmarks:mock:");

        lmId2.setLocalId("2");
        lmId2.setManagerUri("qtlandmarks:mock:");

        lmId3.setLocalId("3");
        lmId3.setManagerUri("qtlandmarks:mock:");

        QLandmarkIdFilter idFilter;
        idFilter << lmId1 << lmId2 << lmId3;

        QLandmark lm;
        lm.setLandmarkId(lmId2);

        QVERIFY(MockEngine::testFilter(idFilter,lm));

        //test landmark id filter that doesn't match
        QLandmarkId lmId4;
        lmId4.setLocalId("4");
        lmId4.setManagerUri("qtlandmarks:mock:");

        lm.setLandmarkId(lmId4);
        QVERIFY(!MockEngine::testFilter(idFilter, lm));
    }

    void testFilterName()
    {
        //test for match - case matches, filter-case insensitive
        QLandmarkNameFilter nameFilter;
        QLandmark lm;
        lm.setName("madara");

        //test case insensitive, start with
        nameFilter.setName("mad");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchStartsWith);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Mad");
        QVERIFY(MockEngine::testFilter(nameFilter,lm));

        //test case sensitive starts with
        nameFilter.setName("mad");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchCaseSensitive | QLandmarkFilter::MatchStartsWith);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Mad");
        QVERIFY(!MockEngine::testFilter(nameFilter,lm));

        //test case insensitive, ends with
        nameFilter.setName("ara");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchEndsWith);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Ara");
        QVERIFY(MockEngine::testFilter(nameFilter,lm));

        //test case sensitive, ends with
        nameFilter.setName("ara");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchCaseSensitive | QLandmarkFilter::MatchEndsWith);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Ara");
        QVERIFY(!MockEngine::testFilter(nameFilter,lm));

        //test case insensitive, contains
        nameFilter.setName("ada");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchContains);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Ada");
        QVERIFY(MockEngine::testFilter(nameFilter,lm));

        //test case sensitive, contains
        nameFilter.setName("ada");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchCaseSensitive | QLandmarkFilter::MatchContains);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Ada");
        QVERIFY(!MockEngine::testFilter(nameFilter,lm));

        //test case insensitive, fixed string
        nameFilter.setName("madara");
        nameFilter.setMatchFlags( QLandmarkFilter::MatchFixedString);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Madara");
        QVERIFY(MockEngine::testFilter(nameFilter,lm));

        //test case sensitive, fixed string
        nameFilter.setName("madara");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchCaseSensitive | QLandmarkFilter::MatchFixedString);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Madara");
        QVERIFY(!MockEngine::testFilter(nameFilter,lm));


        //test case variant match
        nameFilter.setName("madara");
        nameFilter.setMatchFlags(0);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Madara");
        QVERIFY(!MockEngine::testFilter(nameFilter,lm));

        nameFilter.setName("madara");
        nameFilter.setMatchFlags(QLandmarkFilter::MatchCaseSensitive);
        QVERIFY(MockEngine::testFilter(nameFilter,lm));
        nameFilter.setName("Madara");
        QVERIFY(!MockEngine::testFilter(nameFilter,lm));
    }

    void testProximityFilter()
    {
        QLandmarkProximityFilter proximityFilter;
        proximityFilter.setCenter(QGeoCoordinate(30,30));
        proximityFilter.setRadius( QGeoCoordinate(30,30).distanceTo(QGeoCoordinate(30,32)) );

        //test landmark in the centre
        QLandmark lm;
        lm.setCoordinate(QGeoCoordinate(30,30));
        QVERIFY(MockEngine::testFilter(proximityFilter, lm));

        //test landmark just within the radius
        lm.setCoordinate(QGeoCoordinate(30,28.1));
        QVERIFY(MockEngine::testFilter(proximityFilter, lm));

        //test landmark on the edge the radius
        lm.setCoordinate(QGeoCoordinate(30,28.0));
        QVERIFY(MockEngine::testFilter(proximityFilter, lm));

        //test landmark just outside radius
        lm.setCoordinate(QGeoCoordinate(30,27.9));
        QVERIFY(!MockEngine::testFilter(proximityFilter, lm));

        //test landmark outside radius
        lm.setCoordinate(QGeoCoordinate(30,26));
        QVERIFY(!MockEngine::testFilter(proximityFilter, lm));
    }

    void testUnionFilter()
    {
        QLandmarkCategoryId catId;
        catId.setLocalId("1");
        catId.setManagerUri("qtlandmarks:mock:");

        QLandmarkCategoryId catId2;
        catId2.setLocalId("2");
        catId2.setManagerUri("qtlandmarks:mock:");

        //test a match with the union filter
        QLandmarkUnionFilter unionFilter;
        QLandmarkNameFilter nameFilter("beach");
        QLandmarkProximityFilter proximityFilter(QGeoCoordinate(30,30));
        proximityFilter.setRadius(QGeoCoordinate(30,30).distanceTo(QGeoCoordinate(30,32)));
        QLandmarkCategoryFilter categoryFilter;
        categoryFilter.setCategoryId(catId);

        QLandmark lm;
        lm.setName("statue");
        lm.setCoordinate(QGeoCoordinate(-30,-29));
        lm.addCategoryId(catId);

        unionFilter << nameFilter << categoryFilter << proximityFilter;
        QVERIFY(MockEngine::testFilter(unionFilter,lm));

        //test no match with union filter
        lm.removeCategoryId(catId);
        lm.addCategoryId(catId2);
        QVERIFY(!MockEngine::testFilter(unionFilter,lm));

        //test empty union filter
        unionFilter.clear();
        QVERIFY(!MockEngine::testFilter(unionFilter,lm));
    }

    void testCompareName()
    {
        QLandmark lmA, lma, lmB, lmb, lmC, lmc;
        lmA.setName("Alabama");
        lma.setName("alabama");
        lmB.setName("Brisbane");
        lmb.setName("brisbane");
        lmC.setName("ChristChurch");
        lmc.setName("christChurch");

        QLandmarkNameSort nameSort;
        nameSort.setDirection(Qt::AscendingOrder);
        nameSort.setCaseSensitivity(Qt::CaseInsensitive);
        QVERIFY(MockEngine::compareName(lmA,lmC, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmC,lmA, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmC,lmC, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmC,lmc, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmc,lmC, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmc,lmc, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lma,lmc, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmc,lmA, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmA,lmc, nameSort) < 0);


        nameSort.setDirection(Qt::DescendingOrder);
        nameSort.setCaseSensitivity(Qt::CaseInsensitive);
        QVERIFY(MockEngine::compareName(lmA,lmC, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmC,lmA, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmC,lmC, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmC,lmc, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmc,lmC, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmc,lmc, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lma,lmc, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmc,lmA, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmA,lmc, nameSort) > 0);

        nameSort.setDirection(Qt::AscendingOrder);
        nameSort.setCaseSensitivity(Qt::CaseSensitive);
        QVERIFY(MockEngine::compareName(lmA,lmC, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmC,lmA, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmC,lmC, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmC,lmc, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmc,lmC, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmc,lmc, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lma,lmc, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmc,lmA, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmA,lmc, nameSort) < 0);

        nameSort.setDirection(Qt::DescendingOrder);
        nameSort.setCaseSensitivity(Qt::CaseSensitive);
        QVERIFY(MockEngine::compareName(lmA,lmC, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmC,lmA, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmC,lmC, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lmC,lmc, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmc,lmC, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmc,lmc, nameSort) == 0);
        QVERIFY(MockEngine::compareName(lma,lmc, nameSort) > 0);
        QVERIFY(MockEngine::compareName(lmc,lmA, nameSort) < 0);
        QVERIFY(MockEngine::compareName(lmA,lmc, nameSort) > 0);
    }


    void testSortLandmarks() {
        QLandmark lmA, lmB, lmC, lmD, lmZ;
        QLandmarkId idA, idB, idC, idD, idZ;

        idA.setLocalId("1");
        lmA.setLandmarkId(idA);
        lmA.setName("Level A");
        lmA.setCoordinate(QGeoCoordinate(0,30));

        idB.setLocalId("2");
        lmB.setLandmarkId(idB);
        lmB.setName("Level B");
        lmB.setCoordinate(QGeoCoordinate(0,30));

        idC.setLocalId("3");
        lmC.setLandmarkId(idC);
        lmC.setName("Level C");
        lmC.setCoordinate(QGeoCoordinate(0,30));

        idD.setLocalId("4");
        lmD.setLandmarkId(idD);
        lmD.setName("Level D");
        lmD.setCoordinate(QGeoCoordinate(0,30));

        idZ.setLocalId("26");
        lmZ.setLandmarkId(idZ);
        lmZ.setName("Level Z");
        lmZ.setCoordinate(QGeoCoordinate(0,30));

        QList<QLandmark> landmarks;
        landmarks << lmD << lmC << lmA << lmZ << lmB;
        QLandmarkNameSort nameSort;

        QList<QLandmarkSortOrder> sortOrders;
        sortOrders << nameSort;
        QList<QLandmarkId> landmarkIds;
        landmarkIds = MockEngine::sortLandmarks(landmarks, sortOrders);
        QVERIFY(landmarkIds.count() == 5);

        QList<QLandmarkId> expectedAscIds;
        expectedAscIds << idA << idB << idC << idD << idZ;
        QVERIFY(landmarkIds == expectedAscIds);

        //test descending order by name
        sortOrders[0].setDirection(Qt::DescendingOrder);
        landmarkIds = MockEngine::sortLandmarks(landmarks, sortOrders);
        QList<QLandmarkId> expectedDescIds;
        expectedDescIds << idZ << idD << idC << idB << idA;
        QVERIFY(landmarkIds == expectedDescIds);
    }
};

QTEST_MAIN(tst_QLandmarkManagerEngine)
#include "tst_qlandmarkmanagerengine.moc"

