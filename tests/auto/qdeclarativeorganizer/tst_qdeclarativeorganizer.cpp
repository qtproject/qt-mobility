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
#include <QDeclarativeListProperty>


#include "qdeclarativeorganizermodel_p.h"
#include "qdeclarativeorganizeritemsortorder_p.h"
#include "qdeclarativeorganizeritemrecurrencerule_p.h" 
#include "qdeclarativeorganizeritem_p.h" 
#include "qdeclarativeorganizeritemfilter_p.h" 
#include "qdeclarativeorganizeritemdetail_p.h" 
#include "qdeclarativeorganizeritemfetchhint_p.h" 
#include "qdeclarativeorganizeritemcollection_p.h" 

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

QTM_USE_NAMESPACE


class tst_QDeclarativeOrganizer : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeOrganizer();
    virtual ~tst_QDeclarativeOrganizer();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:

    void construction();
    void construction_data();


private:
    QObject* createComponent(const QString& componentString);

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine m_engine;
};

tst_QDeclarativeOrganizer::tst_QDeclarativeOrganizer() {}
tst_QDeclarativeOrganizer::~tst_QDeclarativeOrganizer() {}

void tst_QDeclarativeOrganizer::initTestCase()
{
}

void tst_QDeclarativeOrganizer::cleanupTestCase()
{
}

void tst_QDeclarativeOrganizer::init()
{

}
void tst_QDeclarativeOrganizer::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);

    QObject* obj = createComponent(componentString);

    if (shouldSucceed) {
        if (obj == 0) {
            if (component.isError()) {
                qDebug() << "Error(s) occured when trying to instantiate. QDeclarativeComponent::errors(): ";
                qDebug() << component.errors();
            }
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
            qWarning("--------- could not instantiate components from organizer   -------------- ");
            qWarning("--------- declarative plugin. make sure it is built and found ------------");
            qWarning(" ---------under {QTDIR}/imports, or c:/sys/bin on Symbian     ----------- ");
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
        }
        QVERIFY(obj != 0);
        QCOMPARE(obj->metaObject()->className(), expectedClassName.toAscii().constData());
    } else {
        QVERIFY(obj == 0);
    }
    delete obj;
}

void tst_QDeclarativeOrganizer::construction_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<bool>("shouldSucceed");
    // OrganizerModel
    QTest::newRow("OrganizerModel: No properties auto update false") <<  "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {autoUpdate: false}" << true;
    QTest::newRow("OrganizerModel: Only id property") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId}" << true;
    QTest::newRow("OrganizerModel: Valuetype properties") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId; manager:'memory'; startPeriod:'2010-08-12T13:22:01' endPeriod:'2010-09-12T13:22:01'}" << true;
    QTest::newRow("OrganizerModel: With filter") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId; filter: OrganizerItemDateTimePeriodFilter{id: filter;start:'2010-08-12T13:22:01';end:'2010-09-12T13:22:01'} }" << true;
//    // Landmark
//    QTest::newRow("Landmark: No properties") << "QDeclarativeOrganizer" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n Landmark {}" << true;
//    QTest::newRow("Landmark: Only id property") << "QDeclarativeOrganizer" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n Landmark {id: landmark}" << true;
//    QTest::newRow("Landmark: Valuetype properties") << "QDeclarativeOrganizer" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n Landmark {id: landmark; name: 'jeesbox'; phoneNumber:'555 999'; description: 'Planet town plaza';radius: 0.3; iconSource: 'www.com'; url: 'urlitus'}" << true;
//    // LandmarkCategoryModel
//    QTest::newRow("LandmarkCategoryModel: No properties") <<  "QDeclarativeOrganizerCategoryModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkCategoryModel {}" << true;
//    QTest::newRow("LandmarkCategoryModel: Only id property") << "QDeclarativeOrganizerCategoryModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkCategoryModel {id: landmarkCategoryModelId}" << true;
//    QTest::newRow("LandmarkCategoryModel: Valuetype properties") << "QDeclarativeOrganizerCategoryModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkCategoryModel {id: landmarkCategoryModelId; autoUpdate: false; }" << true;
//    // LandmarkCategory
//    QTest::newRow("LandmarkCategory: No properties") <<  "QDeclarativeOrganizerCategory" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkCategory {}" << true;
//    QTest::newRow("LandmarkCategory: Only id property") << "QDeclarativeOrganizerCategory" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkCategory {id: landmarkCategory}" << true;
//    QTest::newRow("LandmarkCategory: Valuetype properties") << "QDeclarativeOrganizerCategory" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkCategory {id: landmarkCategory; name: 'Franz plaza'; iconSource: 'www.fi' }" << true;
//    // LandmarkNameFilter
//    QTest::newRow("LandmarkNameFilter: No properties") << "QDeclarativeOrganizerNameFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkNameFilter {}" << true;
//    QTest::newRow("LandmarkNameFilter: Only id property") << "QDeclarativeOrganizerNameFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkNameFilter {id: landmarkFilterId}" << true;
//    QTest::newRow("LandmarkNameFilter: with name") << "QDeclarativeOrganizerNameFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkNameFilter {name: \"landmarkName\" }" << true;
//    // LandmarkProximityFilter
//    QTest::newRow("LandmarkProximityFilter: No properties") << "QDeclarativeOrganizerProximityFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkProximityFilter {}" << true;
//    QTest::newRow("LandmarkProximityFilter: Only id property") << "QDeclarativeOrganizerProximityFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkProximityFilter {id: landmarkFilterId}" << true;
//    QTest::newRow("LandmarkProximityFilter: with coordinate") << "QDeclarativeOrganizerProximityFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkProximityFilter {center: Coordinate {id: coordinate} radius: 20 }" << true;
//    // LandmarkUnionFilter
//    QTest::newRow("LandmarkUnionFilter: No properties") << "QDeclarativeOrganizerUnionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkUnionFilter {}" << true;
//    QTest::newRow("LandmarkUnionFilter: Only id property") << "QDeclarativeOrganizerUnionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkUnionFilter {id: landmarkUnionFilterId}" << true;
//    QTest::newRow("LandmarkUnionFilter: One plain filter") << "QDeclarativeOrganizerUnionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkUnionFilter {id: landmarkUnionFilterId; LandmarkNameFilter {id: filter} }" << true;
//    QTest::newRow("LandmarkUnionFilter: Two plain filters") << "QDeclarativeOrganizerUnionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkUnionFilter {id: landmarkUnionFilterId; LandmarkNameFilter {id: filter_1} LandmarkProximityFilter{id: filter_2} }" << true;
//    // LandmarkIntersectionFilter
//    QTest::newRow("LandmarkIntersectionFilter: No properties") << "QDeclarativeOrganizerIntersectionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkIntersectionFilter {}" << true;
//    QTest::newRow("LandmarkIntersectionFilter: Only id property") << "QDeclarativeOrganizerIntersectionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkIntersectionFilter {id: landmarkIntersectionFilterId}" << true;
//    QTest::newRow("LandmarkIntersectionFilter: One plain filter") << "QDeclarativeOrganizerIntersectionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkIntersectionFilter {id: landmarkIntersectionFilterId; LandmarkNameFilter {id: filter} }" << true;
//    QTest::newRow("LandmarkIntersectionFilter: Two plain filters") << "QDeclarativeOrganizerIntersectionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkIntersectionFilter {id: landmarkIntersectionFilterId; LandmarkNameFilter {id: filter_1} LandmarkProximityFilter{id: filter_2} }" << true;
//    // Complex filter
//    QTest::newRow("Intersection (Filter, Union(Filter,Filter) ") << "QDeclarativeOrganizerIntersectionFilter" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n LandmarkIntersectionFilter{LandmarkProximityFilter{} LandmarkUnionFilter{LandmarkProximityFilter{} LandmarkNameFilter{}}}" << true;
}


/*
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeOrganizer::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&m_engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    Q_ASSERT(source_obj != 0);
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeOrganizer)
#include "tst_qdeclarativeorganizer.moc"
