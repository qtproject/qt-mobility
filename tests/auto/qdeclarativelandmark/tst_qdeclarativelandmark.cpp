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
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include "qdeclarativepositionsource_p.h"
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

class tst_QDeclarativeLandmark : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeLandmark();
    virtual ~tst_QDeclarativeLandmark();

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void construction();
    void construction_data();
    void defaultProperties();

private:
    QObject* createComponent(const QString& componentString);

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine engine;
};

tst_QDeclarativeLandmark::tst_QDeclarativeLandmark() {}
tst_QDeclarativeLandmark::~tst_QDeclarativeLandmark() {}
void tst_QDeclarativeLandmark::initTestCase() {}
void tst_QDeclarativeLandmark::cleanupTestCase() {}

/*
  Tests Landmark elements instantiating from plugin.
*/

void tst_QDeclarativeLandmark::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);
    // Component encapsulates one component description
    QDeclarativeComponent component(&engine);
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
    QTest::newRow("LandmarkModel: Valuetype properties") << "QDeclarativeLandmarkModel" << "import Qt 4.7 \n import QtMobility.location 1.1 \n LandmarkModel {id: landmarkModelId; autoUpdate:true; limit: 5; offset: 2}" << true;
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
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeLandmark::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    Q_ASSERT(source_obj != 0);
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeLandmark)
#include "tst_qdeclarativelandmark.moc"
