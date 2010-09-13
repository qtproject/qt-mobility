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
#include "qdeclarativelandmarkcategorymodel_p.h"
#include "qlandmarkid.h"
#include "qlandmarkcategoryid.h"
#include "qdeclarativelist.h"
#include <QDeclarativeListProperty>
#include <QString>
#include <QUuid>


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

#define DB_FILENAME "test.db"
#define LEAVE_DB_AFTER_TESTRUN true

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeLandmarkModel::SortOrder)
Q_DECLARE_METATYPE(QDeclarativeLandmarkModel::SortKey)

class tst_QDeclarativeLandmark : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeLandmark();
    virtual ~tst_QDeclarativeLandmark();

    enum ObjectType {
        ObjectTypeLandmark = 1,
        ObjectTypeCategory = 2
    };

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:

    void construction();
    void construction_data();
    void defaultProperties();
    void basicSignals();
    void update();
    void update_data();
    void basicFetch();
    void basicFetch_data();
    void databaseChanges();
    void declarativeLandmarkList();
    void declarativeCategoryList();
    void updateCancel();
    void robustness();
    void robustness_data();
    void categoriesOfLandmarkFetch();
    void categoriesOfLandmarkFetch_data();

private:
    QObject* createComponent(const QString& componentString);
    void createDb(QString fileName = DB_FILENAME);
    void deleteDb(QString fileName = DB_FILENAME);
    void populateTypicalDb();
    QLandmarkId addLandmarkToDb();
    void removeLandmarkFromDb(QLandmarkId);
    QLandmarkCategoryId addCategoryToDb();
    void removeCategoryFromDb(QLandmarkCategoryId categoryId);
    bool modelContainsItem(QDeclarativeLandmarkCategoryModel* model, QLandmarkCategoryId categoryId);
    bool modelContainsItem(QDeclarativeLandmarkModel* model, QLandmarkId landmarkId);

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine m_engine;
    QLandmarkManager* m_manager;
};

Q_DECLARE_METATYPE(tst_QDeclarativeLandmark::ObjectType)

tst_QDeclarativeLandmark::tst_QDeclarativeLandmark() : m_engine(0), m_manager(0) {}
tst_QDeclarativeLandmark::~tst_QDeclarativeLandmark() {}

void tst_QDeclarativeLandmark::initTestCase()
{
    qRegisterMetaType<QDeclarativeLandmarkModel::SortOrder>("QDeclarativeLandmarkModel::SortOrder");
    qRegisterMetaType<QDeclarativeLandmarkModel::SortKey>("QDeclarativeLandmarkModel::SortKey");
    qRegisterMetaType<QDeclarativeLandmark*>("QDeclarativeLandmark*");

    // If true, leave a database behind from this testcase.
    // The db is located in /tests/auto/qdeclarativelandmark/
    if (LEAVE_DB_AFTER_TESTRUN) {
        deleteDb("generatedExampleLandmarkDb.db"); // Clear previous
        createDb("generatedExampleLandmarkDb.db");
        populateTypicalDb();

    }
}

void tst_QDeclarativeLandmark::cleanupTestCase()
{
    if (m_manager != 0) {
        delete m_manager;
        m_manager = 0;
    }
    deleteDb();
}

void tst_QDeclarativeLandmark::init()
{
    // Delete possible database before each teststep
    deleteDb();
    createDb();
}

void tst_QDeclarativeLandmark::createDb(QString fileName)
{
    QMap<QString, QString> map;
    map["filename"] = fileName;
    if (m_manager != 0) {
        delete m_manager;
        m_manager = 0;
    }
    m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
}

void tst_QDeclarativeLandmark::deleteDb(QString fileName)
{
    delete m_manager;
    m_manager = 0;
    QFile file(fileName);
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
    QCOMPARE(source_obj->property("autoUpdate").toBool(), true);
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
    QCOMPARE(source_obj->property("autoUpdate").toBool(), true);
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

    source_obj->setProperty("autoUpdate", false);
    QTRY_VERIFY(!autoUpdateChangedSpy.isEmpty());
    autoUpdateChangedSpy.clear();
    source_obj->setProperty("autoUpdate", false);
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

void tst_QDeclarativeLandmark::basicFetch()
{
    QFETCH(QString, componentString);
    QFETCH(int, expectedMatches);
    populateTypicalDb();

    QObject* source_obj = createComponent(componentString);
    QDeclarativeLandmarkAbstractModel* model = static_cast<QDeclarativeLandmarkAbstractModel*>(source_obj);
    model->setDbFileName(DB_FILENAME);

    //QTest::qWait(10);
    if (expectedMatches == -1) { // All should match
        if (componentString.contains("LandmarkCategoryModel"))
            QTRY_COMPARE(source_obj->property("count").toInt(), m_manager->categories().count());
        else
            QTRY_COMPARE(source_obj->property("count").toInt(), m_manager->landmarks().count());
    } else {
        QTRY_COMPARE(source_obj->property("count").toInt(), expectedMatches);
    }
    delete source_obj;
}

void tst_QDeclarativeLandmark::basicFetch_data()
{
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<int>("expectedMatches");
    // Landmarks
    // Simple filters
    QTest::newRow("No match (non-matching filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Name; value: \"Nonexistent landmark\" } }"  << 0;
    QTest::newRow("All (no filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true;}" << -1;
    QTest::newRow("One match (name filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Name; value: \"Brisbane\"} }" << 1;
    QTest::newRow("Two match (name filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Name; value: \"Tower\"} }" << 2;
    QTest::newRow("One match (proximity filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 20; latitude: 20} } }" << 1;
    QTest::newRow("Two match (proximity filter, no radius)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 60; latitude: 60} } }" << 2;
    QGeoCoordinate from(70,70);
    QGeoCoordinate to(71,71);
    QString distance = QString::number(from.distanceTo(to));

    QTest::newRow("Two match (proximity filter, radius)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 70; latitude: 70; radius: " + distance + " } } }" << 2;
    QTest::newRow("One match (two, but offset'd)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {offset: 1; autoUpdate:true; filter: LandmarkFilter{id: filter; type: LandmarkFilter.Proximity; value: Position {longitude: 70; latitude: 70; radius: " + distance + " } } }" << 1;
    QTest::newRow("Four match (all, but limit'd)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {limit: 4; autoUpdate:true;}" << 4;
    // Compound filters
    QTest::newRow("All (empty intersection filter i.e. no filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkIntersectionFilter{ id : filter; } }"  << -1;
    QTest::newRow("All (empty union filter i.e. no filter)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkUnionFilter{id: filter; } }"  << -1;
    QTest::newRow("Two matches (union of two names)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkUnionFilter{ LandmarkFilter{type: LandmarkFilter.Name; value: \"London\"} LandmarkFilter{type: LandmarkFilter.Name; value: \"Sydney\"} } }"  << 2;
    QTest::newRow("Two matches (union of name and prox)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkUnionFilter{ LandmarkFilter{type: LandmarkFilter.Name; value: \"Brisbane\"} LandmarkFilter{type: LandmarkFilter.Proximity; value: Position {longitude:20; latitude:20} } } }"  << 2;
    QTest::newRow("One match (intersect of name and prox)") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true; filter: LandmarkIntersectionFilter{ LandmarkFilter{type: LandmarkFilter.Name; value: \"Tower\"} LandmarkFilter{type: LandmarkFilter.Proximity; value: Position {longitude:71; latitude:71} } } }"  << 1;
    // Categories
    QTest::newRow("All categories") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:true;}" << -1;
    QTest::newRow("One match (all, but limit'd") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:true; limit: 1}" << 1;
}

void tst_QDeclarativeLandmark::categoriesOfLandmarkFetch()
{
    QFETCH(QString, componentString);
    QFETCH(QString, landmarkName);
    QFETCH(int, expectedMatches);
    populateTypicalDb();

    QObject* source_obj = createComponent(componentString);
    QDeclarativeLandmarkCategoryModel* categoryModel = static_cast<QDeclarativeLandmarkCategoryModel*>(source_obj);
    categoryModel->setDbFileName(DB_FILENAME);

    source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true;}");
    QDeclarativeLandmarkModel* landmarkModel = static_cast<QDeclarativeLandmarkModel*>(source_obj);
    landmarkModel->setDbFileName(DB_FILENAME);
    QTest::qWait(50);

    // Iterate through landmarks in the landmark model. Match against interesting landmarknames.
    // Upon such, set the item as the 'landmark' in the category model, and verify that
    // model gives the categories the landmark belongs to.
    QDeclarativeLandmark* landmark(0);
    QDeclarativeListProperty<QDeclarativeLandmark> declarativeList = landmarkModel->landmarks();
    for (int i = 0; i < landmarkModel->count(); i++) {
        if (QDeclarativeLandmarkModel::landmarks_at(&declarativeList, i)->name() == landmarkName) {
            landmark = QDeclarativeLandmarkModel::landmarks_at(&declarativeList, i);
            break;
        }
    }
    QVERIFY(landmark != 0);
    QSignalSpy countChangedSpy(categoryModel, SIGNAL(countChanged()));
    categoryModel->setLandmark(landmark); // change to set via property system
    categoryModel->metaObject()->invokeMethod(categoryModel, "update");
    if (expectedMatches > 0)
        QTRY_VERIFY(!countChangedSpy.isEmpty());
    QTRY_COMPARE(categoryModel->property("count").toInt(), expectedMatches);

    delete categoryModel;
    delete landmarkModel;
}

void tst_QDeclarativeLandmark::categoriesOfLandmarkFetch_data()
{
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<QString>("landmarkName");
    QTest::addColumn<int>("expectedMatches");

    QTest::newRow("no category") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:false;}" << "Alpha centauri" << 0;
    QTest::newRow("one category") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:false;}" << "London" << 1;
    QTest::newRow("two categories") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:false;}" << "Brisbane" << 2;
}

// Update database without autoUpdate with update() and verify signals are received an count updates
void tst_QDeclarativeLandmark::update()
{
    QFETCH(QString, componentString);
    QFETCH(ObjectType, objectType);

    QObject* source_obj = createComponent(componentString);
    QTest::qWait(10);
    QVERIFY(source_obj->property("count").toInt() == 0);
    QSignalSpy countChangedSpy(source_obj, SIGNAL(countChanged()));
    populateTypicalDb();
    QDeclarativeLandmarkAbstractModel* model = static_cast<QDeclarativeLandmarkAbstractModel*>(source_obj);
    model->setDbFileName(DB_FILENAME);
    source_obj->metaObject()->invokeMethod(source_obj, "update");
    QTRY_VERIFY(!countChangedSpy.isEmpty());
    if (objectType == ObjectTypeLandmark)
        QTRY_COMPARE(source_obj->property("count").toInt(), m_manager->landmarks().count());
    else
        QTRY_COMPARE(source_obj->property("count").toInt(), m_manager->categories().count());
    delete source_obj;
}

void tst_QDeclarativeLandmark::update_data()
{
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<ObjectType>("objectType");
    QTest::newRow("LandmarkModel") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {autoUpdate:false;}" << ObjectTypeLandmark;
    QTest::newRow("LandmarkCategoryModel") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:false;}"<< ObjectTypeCategory;
}

void tst_QDeclarativeLandmark::databaseChanges()
{
    // Test adding and removing of categories
    QDeclarativeLandmarkCategoryModel* categoryModel = static_cast<QDeclarativeLandmarkCategoryModel*>(createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:true;}"));
    categoryModel->setDbFileName(DB_FILENAME);
    QSignalSpy categoryModelChangedSpy(categoryModel, SIGNAL(modelChanged()));
    QSignalSpy categoryCountChangedSpy(categoryModel, SIGNAL(countChanged()));
    // Add first category
    QLandmarkCategoryId categoryId1 = addCategoryToDb();
    QTRY_COMPARE(categoryModelChangedSpy.count(), 1);
    QTRY_COMPARE(categoryCountChangedSpy.count(), 1);
    QTRY_COMPARE(categoryModel->property("count").toInt(), 1);
    QVERIFY(modelContainsItem(categoryModel, categoryId1));

    // Add second category
    categoryModelChangedSpy.clear();
    categoryCountChangedSpy.clear();
    QLandmarkCategoryId categoryId2 = addCategoryToDb();
    QTRY_COMPARE(categoryModelChangedSpy.count(), 1);
    QTRY_COMPARE(categoryCountChangedSpy.count(), 1);
    QTRY_COMPARE(categoryModel->property("count").toInt(), 2);
    QVERIFY(modelContainsItem(categoryModel, categoryId1));
    QVERIFY(modelContainsItem(categoryModel, categoryId2));
    // Remove first category
    categoryModelChangedSpy.clear();
    categoryCountChangedSpy.clear();
    removeCategoryFromDb(categoryId1);
    QTRY_COMPARE(categoryModelChangedSpy.count(), 1);
    QTRY_COMPARE(categoryCountChangedSpy.count(), 1);
    QTRY_COMPARE(categoryModel->property("count").toInt(), 1);
    QVERIFY(!modelContainsItem(categoryModel, categoryId1));
    QVERIFY(modelContainsItem(categoryModel, categoryId2));
    // Clear the remaining category
    QDeclarativeListProperty<QDeclarativeLandmarkCategory> declarativeCategoryList = categoryModel->categories();
    QDeclarativeLandmarkCategoryModel::categories_clear(&declarativeCategoryList);
    QVERIFY(!modelContainsItem(categoryModel, categoryId2));
    delete categoryModel;

    // Test adding and removing of landmarks
    QDeclarativeLandmarkModel* landmarkModel = static_cast<QDeclarativeLandmarkModel*>(createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {autoUpdate:true;}"));
    landmarkModel->setDbFileName(DB_FILENAME);

    // Add first landmark to database
    QSignalSpy landmarkModelChangedSpy(landmarkModel, SIGNAL(modelChanged()));
    QSignalSpy landmarkCountChangedSpy(landmarkModel, SIGNAL(countChanged()));
    QLandmarkId landmarkId1 = addLandmarkToDb();
    QTRY_COMPARE(landmarkModelChangedSpy.count(), 1);
    QTRY_COMPARE(landmarkCountChangedSpy.count(), 1);
    QTRY_COMPARE(landmarkModel->property("count").toInt(), 1);
    QVERIFY(modelContainsItem(landmarkModel, landmarkId1));
    // Add second landmark to database
    landmarkModelChangedSpy.clear();
    landmarkCountChangedSpy.clear();
    QLandmarkId landmarkId2 = addLandmarkToDb();
    QTRY_COMPARE(landmarkModelChangedSpy.count(), 1);
    QTRY_COMPARE(landmarkCountChangedSpy.count(), 1);
    QTRY_COMPARE(landmarkModel->property("count").toInt(), 2);
    QVERIFY(modelContainsItem(landmarkModel, landmarkId1));
    QVERIFY(modelContainsItem(landmarkModel, landmarkId2));
    // Remove first landmark from the database
    landmarkModelChangedSpy.clear();
    landmarkCountChangedSpy.clear();
    removeLandmarkFromDb(landmarkId1);
    QTRY_COMPARE(landmarkModelChangedSpy.count(), 1);
    QTRY_COMPARE(landmarkCountChangedSpy.count(), 1);
    QTRY_COMPARE(landmarkModel->property("count").toInt(), 1);
    QTRY_COMPARE(landmarkModel->landmarkList().at(0).name(), m_manager->landmark(landmarkId2).name());
    QVERIFY(!modelContainsItem(landmarkModel, landmarkId1));
    QVERIFY(modelContainsItem(landmarkModel, landmarkId2));
    // Clear the remaining landmark
    QDeclarativeListProperty<QDeclarativeLandmark> declarativeLandmarkList = landmarkModel->landmarks();
    QDeclarativeLandmarkModel::landmarks_clear(&declarativeLandmarkList);
    QVERIFY(!modelContainsItem(landmarkModel, landmarkId2));
    delete landmarkModel;
}

/*
Sanitytest interface robustness
*/
void tst_QDeclarativeLandmark::robustness()
{
    QFETCH(QString, componentString);
    QObject* source_obj = createComponent(componentString);

    QDeclarativeLandmarkAbstractModel* model = static_cast<QDeclarativeLandmarkAbstractModel*>(source_obj);
    model->setDbFileName(DB_FILENAME);

    // Bomb update() calls with empty database, should not crash
    for (int interval = 0; interval < 30; interval += 5) {
        source_obj->metaObject()->invokeMethod(source_obj, "update");
        QTest::qWait(interval);
    }
    // Bomb update() calls with prepopulated database, should not crash
    populateTypicalDb();
    QTest::qWait(100);

    for (int interval = 0; interval < 30; interval += 5) {
        source_obj->metaObject()->invokeMethod(source_obj, "update");
        QTest::qWait(interval);
    }
    delete source_obj;
}

void tst_QDeclarativeLandmark::robustness_data()
{
    QTest::addColumn<QString>("componentString");
    QTest::newRow("LandmarkModel without autoUpdate") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {autoUpdate:false;}";
    QTest::newRow("LandmarkModel with autoUpdate") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {autoUpdate:false;}";
    QTest::newRow("LandmarkCategoryModel without autoUpdate") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:false;}";
    QTest::newRow("LandmarkCategoryModel with autoUpdate") << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel {autoUpdate:true;}";
}

// Very simple case to check that deletion of classes does not crash during an update
void tst_QDeclarativeLandmark::updateCancel()
{
    QObject* source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {autoUpdate:true;}");
    QDeclarativeLandmarkModel* landmarkModel = static_cast<QDeclarativeLandmarkModel*>(source_obj);
    landmarkModel->setDbFileName(DB_FILENAME);
    addLandmarkToDb();
    landmarkModel->update();
    QTest::qWait(10);
    delete landmarkModel;
}

// Verify declarative list -property of landmarks works
void tst_QDeclarativeLandmark::declarativeLandmarkList()
{
    populateTypicalDb();
    QObject* source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel { autoUpdate:true;}");
    QDeclarativeLandmarkModel* model = static_cast<QDeclarativeLandmarkModel*>(source_obj);
    model->setDbFileName(DB_FILENAME);
    QTRY_VERIFY(model->count() > 0);
    QDeclarativeListProperty<QDeclarativeLandmark> declarativeList = model->landmarks();

    // Count()
    QTRY_COMPARE(QDeclarativeLandmarkModel::landmarks_count(&declarativeList), model->count());
    // At()
    for (int i = 0; i < model->count(); i++) {
        QCOMPARE(QDeclarativeLandmarkModel::landmarks_at(&declarativeList, i)->name(), model->landmarkList().at(i).name());
    }
    // Append() (not supported but should not crash)
    qDebug("Following warning is OK (testing that unsupported feature does not crash).");
    QDeclarativeLandmarkModel::landmarks_append(&declarativeList, 0);
    // Clear()
    QDeclarativeLandmarkModel::landmarks_clear(&declarativeList);
    delete model;
}

// Verify declarative list -property of landmarks works
void tst_QDeclarativeLandmark::declarativeCategoryList()
{
    populateTypicalDb();
    QObject* source_obj = createComponent("import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkCategoryModel { autoUpdate:true;}");
    QDeclarativeLandmarkCategoryModel* model = static_cast<QDeclarativeLandmarkCategoryModel*>(source_obj);
    model->setDbFileName(DB_FILENAME);
    QTRY_VERIFY(model->count() > 0);
    QDeclarativeListProperty<QDeclarativeLandmarkCategory> declarativeList = model->categories();

    // Count()
    QTRY_COMPARE(QDeclarativeLandmarkCategoryModel::categories_count(&declarativeList), model->count());
    // At()
    for (int i = 0; i < model->count(); i++) {
        // TODO QCOMPARE(QDeclarativeLandmarkCategoryModel::categories_at(&declarativeList, i)->name(), model->categories().at(i)->name());
    }
    // Append() (not supported but should not crash)
    qDebug("Following warning is OK (testing that unsupported feature does not crash).");
    QDeclarativeLandmarkCategoryModel::categories_append(&declarativeList, 0);
    // Clear()
    QDeclarativeLandmarkCategoryModel::categories_clear(&declarativeList);
    delete model;
}

/*
 Adds randomly named landmark/category to database and returns the id.
 */

QLandmarkId tst_QDeclarativeLandmark::addLandmarkToDb()
{
    Q_ASSERT(m_manager);
    QLandmark landmark;
    landmark.setName(QUuid::createUuid().toString());
    m_manager->saveLandmark(&landmark);
    return landmark.landmarkId();
}

void tst_QDeclarativeLandmark::removeLandmarkFromDb(QLandmarkId landmarkId)
{
    Q_ASSERT(m_manager);
    m_manager->removeLandmark(landmarkId);
    QTest::qWait(10);
}

void tst_QDeclarativeLandmark::removeCategoryFromDb(QLandmarkCategoryId categoryId)
{
    Q_ASSERT(m_manager);
    m_manager->removeCategory(categoryId);
    QTest::qWait(10);
}

QLandmarkCategoryId tst_QDeclarativeLandmark::addCategoryToDb()
{
    Q_ASSERT(m_manager);
    QLandmarkCategory category;
    category.setName(QUuid::createUuid().toString());
    m_manager->saveCategory(&category);
    return category.categoryId();
}

bool tst_QDeclarativeLandmark::modelContainsItem(QDeclarativeLandmarkCategoryModel* model, QLandmarkCategoryId categoryId)
{
    QList<QLandmarkCategory> categories = model->categoryList();
    foreach(const QLandmarkCategory category, categories) {
        if (category.categoryId() == categoryId)
            return true;
    }
    return false;
}

bool tst_QDeclarativeLandmark::modelContainsItem(QDeclarativeLandmarkModel* model, QLandmarkId landmarkId)
{
    QList<QLandmark> landmarks = model->landmarkList();
    foreach (const QLandmark landmark, landmarks) {
        if (landmark.landmarkId() == landmarkId)
            return true;
    }
    return false;
}

/*
 Helper function to populate normal valid database
*/

void tst_QDeclarativeLandmark::populateTypicalDb()
{
    Q_ASSERT(m_manager);

    QLandmarkCategory categorySunshineCities;
    categorySunshineCities.setName("Sunshine cities");
    m_manager->saveCategory(&categorySunshineCities);
    QLandmarkCategoryId categoryIdSunshineCities = categorySunshineCities.categoryId();

    QLandmarkCategory categoryEmpty;
    categoryEmpty.setName("Empty");
    m_manager->saveCategory(&categoryEmpty);

    QLandmark uncategorized;
    uncategorized.setName("Alpha centauri");
    m_manager->saveLandmark(&uncategorized);

    QLandmarkCategory categoryCities;
    categoryCities.setName("Cities");
    m_manager->saveCategory(&categoryCities);
    QLandmarkCategoryId categoryIdCities = categoryCities.categoryId();
    QLandmark brisbane;
    brisbane.setName("Brisbane");
    brisbane.setCoordinate(QGeoCoordinate(50, 50));
    brisbane.addCategoryId(categoryIdCities);
    brisbane.addCategoryId(categoryIdSunshineCities);
    m_manager->saveLandmark(&brisbane);
    QLandmark sydney;
    sydney.setName("Sydney");
    sydney.setCoordinate(QGeoCoordinate(60, 60));
    sydney.addCategoryId(categoryIdCities);
    sydney.addCategoryId(categoryIdSunshineCities);
    m_manager->saveLandmark(&sydney);
    QLandmark london;
    london.setName("London");
    london.setCoordinate(QGeoCoordinate(70, 70));
    london.addCategoryId(categoryIdCities);
    m_manager->saveLandmark(&london);

    QLandmarkCategory categorySights;
    categorySights.setName("Sights");
    m_manager->saveCategory(&categorySights);
    QLandmarkCategoryId categoryIdSights = categorySights.categoryId();
    QLandmark tower1;
    tower1.setName("Tower");
    tower1.setCoordinate(QGeoCoordinate(20, 20));
    tower1.addCategoryId(categoryIdSights);
    m_manager->saveLandmark(&tower1);
    QLandmark tower2;
    tower2.setName("Tower");
    tower2.setCoordinate(QGeoCoordinate(71, 71));
    tower2.addCategoryId(categoryIdSights);
    m_manager->saveLandmark(&tower2);
    QLandmark museum;
    museum.setName("Opera House");
    museum.setCoordinate(QGeoCoordinate(60, 60));
    museum.addCategoryId(categoryIdSights);
    m_manager->saveLandmark(&museum);
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
