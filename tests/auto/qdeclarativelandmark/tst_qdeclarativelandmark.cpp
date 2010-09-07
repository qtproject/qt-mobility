/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QMetaObject>
#include <QDateTime>
#include <QMap>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <qlandmarkmanager.h>
#include <qlandmarksortorder.h>
#include "qdeclarativepositionsource_p.h"
#include "qdeclarativelandmarkmodel_p.h"
#include <QString>

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

#ifndef QTRY_WAIT
#define QTRY_WAIT(code, __expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)
#endif

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeLandmarkModel::SortOrder)
Q_DECLARE_METATYPE(QDeclarativeLandmarkModel::SortKey)

class tst_QDeclarativeLandmark : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeLandmark();
    virtual ~tst_QDeclarativeLandmark();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:
    void construction();
    void construction_data();
    void defaultProperties();
    void basicSignals();
    void basicLandmarkFetch();
    void basicLandmarkFetch_data();
    void update();

private:
    QObject* createComponent(const QString& componentString);
    void createDb();
    void deleteDb();
    void populateTypicalDb();

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine m_engine;
    QLandmarkManager* m_manager;
};

tst_QDeclarativeLandmark::tst_QDeclarativeLandmark() : m_manager(0) {}
tst_QDeclarativeLandmark::~tst_QDeclarativeLandmark() {}
void tst_QDeclarativeLandmark::initTestCase()
{
    qRegisterMetaType<QDeclarativeLandmarkModel::SortOrder>("QDeclarativeLandmarkModel::SortOrder");
    qRegisterMetaType<QDeclarativeLandmarkModel::SortKey>("QDeclarativeLandmarkModel::SortKey");
}

void tst_QDeclarativeLandmark::cleanupTestCase()
{
    if (m_manager != 0) {
        delete m_manager;
        m_manager = 0;
    }
}
void tst_QDeclarativeLandmark::init()
{
    // Delete possible database before each teststep
    deleteDb();
    createDb();
}

void tst_QDeclarativeLandmark::createDb()
{
    QMap<QString, QString> map;
    map["filename"] = "test.db";
    if (m_manager != 0) {
        delete m_manager;
        m_manager = 0;
    }
    m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
}

void tst_QDeclarativeLandmark::deleteDb()
{
    delete m_manager;
    m_manager = 0;
    QFile file("test.db");
    file.remove();
}

/*
  Tests Landmark elements instantiating from plugin.
*/

void tst_QDeclarativeLandmark::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);
    // Component encapsulates one component description
    QDeclarativeComponent component(&m_engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* obj = component.create();

    if (shouldSucceed) {
        if (obj == 0) {
            if (component.isError()) {
                qDebug() << "Error(s) occured when trying to instantiate. QDeclarativeComponent::errors(): ";
                qDebug() << component.errors();
            }
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
            qWarning("--------- could not instantiate components from location   -------------- ");
            qWarning("--------- declarative plugin. make sure it is built and found ------------");
            qWarning(" ---------under {QTDIR}/imports, or c:/sys/bin on Symbian     ----------- ");
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
        }
        QVERIFY(obj != 0);
        QCOMPARE(obj->metaObject()->className(), expectedClassName.toAscii().constData());
    } else {
        QVERIFY(obj == 0);
    }
    // The above check is a bit clumsy, but testing with qobject_cast<> will not work,
    // because the obj is instantiated from a aplugin _but_ also the header has been compiled in this
    // test binary --> static meta object addresses do not match. Test below indicates why the
    // qobject_cast would fail, although the types are actually same. (This could probably be worked around by
    // declaring the interface of QDeclarativeLandmark into the meta-system).
    // qDebug() << "Address of the plugin-instantiated metaobject of the object: " << QString::number(uint(obj->metaObject()));
    // qDebug() << "Whereas the address of the metaobject compiled in this binary is: " << QString::number(uint(&((QDeclarativeLandmark*)0)->staticMetaObject));
    // QDeclarativeLandmark* position = qobject_cast<QDeclarativeLandmark*>(obj);
    // QVERIFY(position != 0);
    delete obj;
}

void tst_QDeclarativeLandmark::construction_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<bool>("shouldSucceed");
    // LandmarkModel
    QTest::newRow("LandmarkModel: No properties") <<  "QDeclarativeLandmarkModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {}" << true;
    QTest::newRow("LandmarkModel: Only id property") << "QDeclarativeLandmarkModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId}" << true;
    QTest::newRow("LandmarkModel: Valuetype properties") << "QDeclarativeLandmarkModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; autoUpdate:true; limit: 5; offset: 2; sortBy: LandmarkModel.NameSort; sortOrder: LandmarkModel.DescendingOrder}" << true;
    QTest::newRow("LandmarkModel: With filter") << "QDeclarativeLandmarkModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; autoUpdate:true; limit: 5; offset: 2; filter: LandmarkFilter{id: filter} }" << true;
    // Landmark
    QTest::newRow("Landmark: No properties") << "QDeclarativeLandmark" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Landmark {}" << true;
    QTest::newRow("Landmark: Only id property") << "QDeclarativeLandmark" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Landmark {id: landmark}" << true;
    QTest::newRow("Landmark: Valuetype properties") << "QDeclarativeLandmark" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Landmark {id: landmark; name: 'jeesbox'; phoneNumber:'555 999'; description: 'Planet town plaza';radius: 0.3; iconSource: 'www.com'; url: 'urlitus'}" << true;
    // LandmarkCategoryModel
    QTest::newRow("LandmarkCategoryModel: No properties") <<  "QDeclarativeLandmarkCategoryModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {}" << true;
    QTest::newRow("LandmarkCategoryModel: Only id property") << "QDeclarativeLandmarkCategoryModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {id: landmarkCategoryModelId}" << true;
    QTest::newRow("LandmarkCategoryModel: Valuetype properties") << "QDeclarativeLandmarkCategoryModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {id: landmarkCategoryModelId; autoUpdate: false; }" << true;
    // LandmarkCategory
    QTest::newRow("LandmarkCategory: No properties") <<  "QDeclarativeLandmarkCategory" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategory {}" << true;
    QTest::newRow("LandmarkCategory: Only id property") << "QDeclarativeLandmarkCategory" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategory {id: landmarkCategory}" << true;
    QTest::newRow("LandmarkCategory: Valuetype properties") << "QDeclarativeLandmarkCategory" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategory {id: landmarkCategory; name: 'Franz plaza'; iconSource: 'www.fi' }" << true;
    // LandmarkFilter
    QTest::newRow("LandmarkFilter: No properties") << "QDeclarativeLandmarkFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkFilter {}" << true;
    QTest::newRow("LandmarkFilter: Only id property") << "QDeclarativeLandmarkFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkFilter {id: landmarkFilterId}" << true;
    QTest::newRow("LandmarkFilter: Name filter") << "QDeclarativeLandmarkFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkFilter {id: landmark; type: LandmarkFilter.Name; value: \"landmarkName\" }" << true;
    QTest::newRow("LandmarkFilter: Proximity filter") << "QDeclarativeLandmarkFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkFilter {id: landmark; type: LandmarkFilter.Proximity; value: Position {id: position} }" << true;
    // LandmarkUnionFilter
    QTest::newRow("LandmarkUnionFilter: No properties") << "QDeclarativeLandmarkUnionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkUnionFilter {}" << true;
    QTest::newRow("LandmarkUnionFilter: Only id property") << "QDeclarativeLandmarkUnionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkUnionFilter {id: landmarkUnionFilterId}" << true;
    QTest::newRow("LandmarkUnionFilter: One plain filter") << "QDeclarativeLandmarkUnionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkUnionFilter {id: landmarkUnionFilterId; LandmarkFilter {id: filter} }" << true;
    QTest::newRow("LandmarkUnionFilter: Two plain filters") << "QDeclarativeLandmarkUnionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkUnionFilter {id: landmarkUnionFilterId; LandmarkFilter {id: filter_1} LandmarkFilter{id: filter_2} }" << true;
    // LandmarkIntersectionFilter
    QTest::newRow("LandmarkIntersectionFilter: No properties") << "QDeclarativeLandmarkIntersectionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkIntersectionFilter {}" << true;
    QTest::newRow("LandmarkIntersectionFilter: Only id property") << "QDeclarativeLandmarkIntersectionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkIntersectionFilter {id: landmarkIntersectionFilterId}" << true;
    QTest::newRow("LandmarkIntersectionFilter: One plain filter") << "QDeclarativeLandmarkIntersectionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkIntersectionFilter {id: landmarkIntersectionFilterId; LandmarkFilter {id: filter} }" << true;
    QTest::newRow("LandmarkIntersectionFilter: Two plain filters") << "QDeclarativeLandmarkIntersectionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkIntersectionFilter {id: landmarkIntersectionFilterId; LandmarkFilter {id: filter_1} LandmarkFilter{id: filter_2} }" << true;
    // Complex filter
    QTest::newRow("Intersection (Filter, Union(Filter,Filter) ") << "QDeclarativeLandmarkIntersectionFilter" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkIntersectionFilter{LandmarkFilter{} LandmarkUnionFilter{LandmarkFilter{} LandmarkFilter{}}}" << true;
}

/*
    Tests the default values of properties
*/
void tst_QDeclarativeLandmark::defaultProperties()
{
    QObject* source_obj(0);
    QObject* filter_obj(0);

    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModel }");
    QCOMPARE(source_obj->property("error").toString(), QString());
    QCOMPARE(source_obj->property("autoUpdate").toBool(), false);
    filter_obj = source_obj->property("nameFilter").value<QObject*>();
    QVERIFY(filter_obj == 0); delete filter_obj;
    filter_obj = source_obj->property("proximityFilter").value<QObject*>();
    QVERIFY(filter_obj == 0); delete filter_obj;
    QCOMPARE(source_obj->property("limit").toInt(), -1);
    QCOMPARE(source_obj->property("offset").toInt(), -1);
    QCOMPARE(source_obj->property("count").toInt(), 0);
    delete source_obj;
    
    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n Landmark {id: landmark }");
    QCOMPARE(source_obj->property("name").toString(), QString());
    QCOMPARE(source_obj->property("phoneNumber").toString(), QString());
    QCOMPARE(source_obj->property("description").toString(), QString());
    QCOMPARE(source_obj->property("radius").toDouble(), -1.0);
    QCOMPARE(source_obj->property("iconSource").toUrl(), QUrl());
    QCOMPARE(source_obj->property("url").toUrl(), QUrl());
    delete source_obj;
    
    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategory {id: landmark }");
    QCOMPARE(source_obj->property("name").toString(), QString());
    QCOMPARE(source_obj->property("iconSource").toUrl(), QUrl());
    delete source_obj;

    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {id: landmark }");
    QCOMPARE(source_obj->property("error").toString(), QString());
    QCOMPARE(source_obj->property("count").toInt(), 0);
    QCOMPARE(source_obj->property("autoUpdate").toBool(), false);
    QCOMPARE(source_obj->property("limit").toInt(), -1);
    QCOMPARE(source_obj->property("offset").toInt(), -1);
    delete source_obj;
}

/*
    Tests the basic signals of properties
*/
void tst_QDeclarativeLandmark::basicSignals()
{
    // LandmarkModel
    QObject* source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {}");
    QSignalSpy limitChangedSpy(source_obj, SIGNAL(limitChanged()));
    QSignalSpy offsetChangedSpy(source_obj, SIGNAL(offsetChanged()));
    QSignalSpy sortByChangedSpy(source_obj, SIGNAL(sortByChanged()));
    QSignalSpy sortOrderChangedSpy(source_obj, SIGNAL(sortOrderChanged()));
    QSignalSpy autoUpdateChangedSpy(source_obj, SIGNAL(autoUpdateChanged()));

    source_obj->setProperty("limit", 2);
    QTRY_VERIFY(!limitChangedSpy.isEmpty());
    limitChangedSpy.clear();
    source_obj->setProperty("limit", 2);
    QTest::qWait(10); // wait that signals propagate
    QTRY_VERIFY(limitChangedSpy.isEmpty());

    source_obj->setProperty("offset", 2);
    QTRY_VERIFY(!offsetChangedSpy.isEmpty());
    offsetChangedSpy.clear();
    source_obj->setProperty("offset", 2);
    QTest::qWait(10);
    QTRY_VERIFY(offsetChangedSpy.isEmpty());

    source_obj->setProperty("sortOrder", QDeclarativeLandmarkModel::DescendingOrder);
    QTRY_VERIFY(!sortOrderChangedSpy.isEmpty());
    sortOrderChangedSpy.clear();
    source_obj->setProperty("sortOrder", QDeclarativeLandmarkModel::DescendingOrder);
    QTest::qWait(10);
    QTRY_VERIFY(sortOrderChangedSpy.isEmpty());

    source_obj->setProperty("sortBy", QLandmarkSortOrder::NameSort);
    QTRY_VERIFY(!sortByChangedSpy.isEmpty());
    sortByChangedSpy.clear();
    source_obj->setProperty("sortBy",  QLandmarkSortOrder::NameSort);
    QTest::qWait(10);
    QTRY_VERIFY(sortByChangedSpy.isEmpty());

    source_obj->setProperty("autoUpdate", true);
    QTRY_VERIFY(!autoUpdateChangedSpy.isEmpty());
    autoUpdateChangedSpy.clear();
    source_obj->setProperty("autoUpdate", true);
    QTest::qWait(10);
    QTRY_VERIFY(autoUpdateChangedSpy.isEmpty());

    delete source_obj;

    // Landmark
    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n Landmark {id: landmark }");
    QSignalSpy nameChangedSpy(source_obj, SIGNAL(nameChanged()));
    QSignalSpy phoneNumberChangedSpy(source_obj, SIGNAL(phoneNumberChanged()));
    QSignalSpy descriptionChangedSpy(source_obj, SIGNAL(descriptionChanged()));
    QSignalSpy radiusChangedSpy(source_obj, SIGNAL(radiusChanged()));
    QSignalSpy iconSourceChangedSpy(source_obj, SIGNAL(iconSourceChanged()));
    QSignalSpy urlChangedSpy(source_obj, SIGNAL(urlChanged()));

    source_obj->setProperty("name", "new value");
    QTRY_VERIFY(!nameChangedSpy.isEmpty());
    nameChangedSpy.clear();
    source_obj->setProperty("name", "new value");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(nameChangedSpy.isEmpty());

    source_obj->setProperty("phoneNumber", "new value");
    QTRY_VERIFY(!phoneNumberChangedSpy.isEmpty());
    phoneNumberChangedSpy.clear();
    source_obj->setProperty("phoneNumber", "new value");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(phoneNumberChangedSpy.isEmpty());

    source_obj->setProperty("description", "new value");
    QTRY_VERIFY(!descriptionChangedSpy.isEmpty());
    descriptionChangedSpy.clear();
    source_obj->setProperty("description", "new value");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(descriptionChangedSpy.isEmpty());

    source_obj->setProperty("radius", 2);
    QTRY_VERIFY(!radiusChangedSpy.isEmpty());
    radiusChangedSpy.clear();
    source_obj->setProperty("radius", 2);
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(radiusChangedSpy.isEmpty());

    source_obj->setProperty("iconSource", "www.yle.fi");
    QTRY_VERIFY(!iconSourceChangedSpy.isEmpty());
    iconSourceChangedSpy.clear();
    source_obj->setProperty("iconSource", "www.yle.fi");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(iconSourceChangedSpy.isEmpty());

    source_obj->setProperty("url", "www.yle.fi");
    QTRY_VERIFY(!urlChangedSpy.isEmpty());
    urlChangedSpy.clear();
    source_obj->setProperty("url", "www.yle.fi");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(urlChangedSpy.isEmpty());
    delete source_obj;


    // LandmarkCategory

    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategory {}");
    QSignalSpy nameChangedSpy2(source_obj, SIGNAL(nameChanged()));
    QSignalSpy iconSourceChangedSpy2(source_obj, SIGNAL(iconSourceChanged()));

    source_obj->setProperty("iconSource", "www.yle.fi");
    QTRY_VERIFY(!iconSourceChangedSpy2.isEmpty());
    iconSourceChangedSpy2.clear();
    source_obj->setProperty("iconSource", "www.yle.fi");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(iconSourceChangedSpy2.isEmpty());

    source_obj->setProperty("name", "new value");
    QTRY_VERIFY(!nameChangedSpy2.isEmpty());
    nameChangedSpy2.clear();
    source_obj->setProperty("name", "new value");
    QTest::qWait(10); // wait that signals propagate (who should not come)
    QTRY_VERIFY(nameChangedSpy2.isEmpty());
    delete source_obj;

    // LandmarkCategoryModel
    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {}");
    QSignalSpy limitChangedSpy3(source_obj, SIGNAL(limitChanged()));
    QSignalSpy offsetChangedSpy3(source_obj, SIGNAL(offsetChanged()));
    QSignalSpy autoUpdateChangedSpy3(source_obj, SIGNAL(autoUpdateChanged()));

    source_obj->setProperty("limit", 2);
    QTRY_VERIFY(!limitChangedSpy3.isEmpty());
    limitChangedSpy3.clear();
    source_obj->setProperty("limit", 2);
    QTest::qWait(10); // wait that signals propagate
    QTRY_VERIFY(limitChangedSpy3.isEmpty());

    source_obj->setProperty("offset", 2);
    QTRY_VERIFY(!offsetChangedSpy3.isEmpty());
    offsetChangedSpy3.clear();
    source_obj->setProperty("offset", 2);
    QTest::qWait(10);
    QTRY_VERIFY(offsetChangedSpy3.isEmpty());
    delete source_obj;

    // LandmarkFilter
    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkFilter {}");
    QSignalSpy typeChangedSpy(source_obj, SIGNAL(typeChanged()));
    QSignalSpy valueChangedSpy(source_obj, SIGNAL(valueChanged()));

    source_obj->setProperty("type", QDeclarativeLandmarkFilter::Proximity);
    QTRY_VERIFY(!typeChangedSpy.isEmpty());
    typeChangedSpy.clear();
    source_obj->setProperty("type", QDeclarativeLandmarkFilter::Proximity);
    QTest::qWait(10); // wait that signals propagate
    QTRY_VERIFY(typeChangedSpy.isEmpty());
    delete source_obj;
}

void tst_QDeclarativeLandmark::basicLandmarkFetch()
{
    QFETCH(QString, componentString);
    QFETCH(int, expectedMatches);
    populateTypicalDb();

    QObject* source_obj = createComponent(componentString);
    QTest::qWait(100);
    if (expectedMatches == -1) { // All should match
        QTRY_VERIFY(source_obj->property("count").toInt() == m_manager->landmarks().count());
    } else {
        QTRY_VERIFY(source_obj->property("count").toInt() == expectedMatches);
    }
    delete source_obj;
}

void tst_QDeclarativeLandmark::basicLandmarkFetch_data()
{
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<int>("expectedMatches");

    // Simple filters
    QTest::newRow("No match (non-matching filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Name; value: \"Nonexistent landmark\" } }"  << 0;
    QTest::newRow("All (no filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true;}" << -1;
    QTest::newRow("One match (name filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Name; value: \"Uniquely named powerhouse\"} }" << 1;
    QTest::newRow("Two match (name filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Name; value: \"Duplicate named bridge\"} }" << 2;
    QTest::newRow("One match (proximity filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 70; latitude: 70} } }" << 1;
    QTest::newRow("Two match (proximity filter, no radius)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 50; latitude: 50} } }" << 2;
    QGeoCoordinate from(50,50);
    QGeoCoordinate to(51,51);
    QString distance = QString::number(from.distanceTo(to));
    QTest::newRow("Three match (proximity filter, radius)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 50; latitude: 50; radius: " + distance + " } } }" << 3;

    // Compound filters
    QTest::newRow("All (empty intersection filter i.e. no filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkIntersectionFilter{ id : filter; } }"  << -1;
    QTest::newRow("All (empty union filter i.e. no filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkUnionFilter{id: filter; } }"  << -1;
    QTest::newRow("Two matches (union of two names)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkUnionFilter{ LandmarkFilter{type: LandmarkFilter.Name; value: \"Uniquely named powerhouse\"} LandmarkFilter{type: LandmarkFilter.Name; value: \"Uniquely named southbank\"} } }"  << 2;
    QTest::newRow("Two matches (union of name and prox)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkUnionFilter{ LandmarkFilter{type: LandmarkFilter.Name; value: \"Uniquely named powerhouse\"} LandmarkFilter{type: LandmarkFilter.Proximity; value: Position {longitude:70; latitude:70} } } }"  << 2;
    QTest::newRow("One match (intersect of name and prox)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:true; filter: LandmarkIntersectionFilter{ LandmarkFilter{type: LandmarkFilter.Name; value: \"Duplicate named bridge\"} LandmarkFilter{type: LandmarkFilter.Proximity; value: Position {longitude:51; latitude:51} } } }"  << 1;
}

// Update database without autoUpdate with update() and verify signals are received an count updates
void tst_QDeclarativeLandmark::update()
{
    QObject* source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; dbFileName: \"test.db\"; autoUpdate:false;}");
    QTest::qWait(100);
    QVERIFY(source_obj->property("count").toInt() == 0);
    QSignalSpy countChangedSpy(source_obj, SIGNAL(countChanged()));
    populateTypicalDb();
    source_obj->metaObject()->invokeMethod(source_obj, "update");
    QTRY_VERIFY(!countChangedSpy.isEmpty());
    QTRY_VERIFY(source_obj->property("count").toInt() == m_manager->landmarks().count());
    delete source_obj;
}

/*
 Helper function to populate normal valid database
*/

void tst_QDeclarativeLandmark::populateTypicalDb()
{
    Q_ASSERT(m_manager);

    // Clumsy but we don't need that much
    QLandmark landmark1;
    landmark1.setName("Uniquely named powerhouse");
    landmark1.setCoordinate(QGeoCoordinate(50, 50));
    m_manager->saveLandmark(&landmark1);

    QLandmark landmark2;
    landmark2.setName("Duplicate named bridge");
    landmark2.setCoordinate(QGeoCoordinate(50, 50));
    m_manager->saveLandmark(&landmark2);

    QLandmark landmark3;
    landmark3.setName("Duplicate named bridge");
    landmark3.setCoordinate(QGeoCoordinate(51, 51));
    m_manager->saveLandmark(&landmark3);

    QLandmark landmark4;
    landmark4.setName("Uniquely located Brisbane");
    landmark4.setCoordinate(QGeoCoordinate(70, 70));
    m_manager->saveLandmark(&landmark4);

    QLandmark landmark5;
    landmark5.setName("Uniquely named southbank");
    landmark5.setCoordinate(QGeoCoordinate(52, 52));
    m_manager->saveLandmark(&landmark5);

    QTest::qWait(50);
}

/*
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeLandmark::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&m_engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    Q_ASSERT(source_obj != 0);
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeLandmark)
#include "tst_qdeclarativelandmark.moc"
