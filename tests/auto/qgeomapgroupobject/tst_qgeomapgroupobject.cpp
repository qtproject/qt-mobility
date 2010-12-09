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

#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomapgroupobject.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgraphicsgeomap.h"
#include "qgeomapcircleobject.h"
#include "qgeomappixmapobject.h"
#include "qgeomaprectangleobject.h"
#include "qgeomappolygonobject.h"
#include "qgeomappolylineobject.h"
#include "qgeomaprouteobject.h"
#include "qgeomaptextobject.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoBoundingBox)
Q_DECLARE_METATYPE(QGeoMapObject*)
Q_DECLARE_METATYPE(QList<QGeoMapObject*>)
Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoMapGroupObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomapgroupobject_data();
    void qgeomapgroupobject();

    void addChildObject_data();
    void addChildObject();
    void boundingBox_data();
    void boundingBox();
    void childObjects_data();
    void childObjects();
    void clearChildObjects_data();
    void clearChildObjects();
    void contains_data();
    void contains();
    void removeChildObject_data();
    void removeChildObject();
    void setMapData_data();
    void setMapData();
    void zvalue_data();
    void zvalue();
    void isSelected();
    void isVisible();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapGroupObject::initTestCase()
{
    qRegisterMetaType<QGeoMapObject*> ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapGroupObject::cleanupTestCase()
{

}

// This will be called before each test function is executed.
void tst_QGeoMapGroupObject::init()
{
    m_helper = new TestHelper();
    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapGroupObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapGroupObject::qgeomapgroupobject_data()
{
}

void tst_QGeoMapGroupObject::qgeomapgroupobject()
{

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QCOMPARE(object->type(), QGeoMapObject::GroupType);
    QCOMPARE(object->boundingBox(), QGeoBoundingBox());
    QCOMPARE(object->childObjects(), QList<QGeoMapObject*>());
    QCOMPARE(object->contains(QGeoCoordinate()), false);

    object->addChildObject((QGeoMapObject*) 0);
    object->removeChildObject((QGeoMapObject*) 0);
    object->clearChildObjects();
    object->setMapData((QGeoMapData*) 0);

    QCOMPARE(object->zValue(), 0);
    QCOMPARE(object->isSelected(),false);
    QCOMPARE(object->isVisible(),true);
    QCOMPARE(object->boundingBox(),QGeoBoundingBox());
    QCOMPARE(object->contains(QGeoCoordinate()),false);
    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY2(object->info(),"info object not created");
    QVERIFY2(object->mapData(),"no map data set");

    map->removeMapObject(object);

    QVERIFY2(!object->info(),"info object not deleted");
    QVERIFY2(!object->mapData(),"no map data still set");
    delete (object);
}

void tst_QGeoMapGroupObject::addChildObject_data()
{
    QTest::addColumn<int>("childObjectCount");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
}

// public void addChildObject(QGeoMapObject* childObject)
void tst_QGeoMapGroupObject::addChildObject()
{

    QFETCH(int, childObjectCount);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->addChildObject(childObject);
    }

    QCOMPARE(object->childObjects().size(),childObjectCount );

    QCOMPARE(spy0.count(), childObjectCount );
    QCOMPARE(spy1.count(), 0);

}

void tst_QGeoMapGroupObject::childObjects_data()
{

    QTest::addColumn<QList<QGeoMapObject*> >("childObjects");
    QList<QGeoMapObject*> list;

    QList<QGeoCoordinate> path;

    path.append(QGeoCoordinate(10, 10));
    path.append(QGeoCoordinate(10, 20));
    path.append(QGeoCoordinate(0, 20));
    path.append(QGeoCoordinate(0, 10));
    QGeoBoundingBox box(QGeoCoordinate(10, 10), QGeoCoordinate(0, 20));

    list.append(new QGeoMapCircleObject(box.center(), 1000));
    list.append(new QGeoMapPixmapObject(box.center(), QPoint(-200, -200), QPixmap(400, 400)));
    list.append(new QGeoMapRectangleObject(box));
    list.append(new QGeoMapTextObject(box.center(), "TEST"));

    QTest::newRow(
        "QGeoMapGroupObject,QGeoMapPixmapObject,QGeoMapRectangleObject,QGeoMapTextObject") << list;

    QList<QGeoMapObject*> list2;

    QGeoMapPolygonObject* polygon = new QGeoMapPolygonObject();
    polygon->setPath(path);

    list2.append(polygon);

    QGeoMapPolylineObject* polyline = new QGeoMapPolylineObject();
    //make path across rectangle
    path.removeAt(1);
    path.removeAt(3);

    polyline->setPath(path);

    list2.append(polyline);

    QGeoMapRouteObject* route = new QGeoMapRouteObject();

    QGeoRoute r;
    r.setPath(path);
    route->setRoute(r);
    //list2.append(route);

    QTest::newRow("QGeoMapPolygonObject,QGeoMapPolylineObject,QGeoMapRouteObject") << list2;

}

// public QList<QGeoMapObject*> childObjects() const
void tst_QGeoMapGroupObject::childObjects()
{
    QFETCH(QList<QGeoMapObject*>, childObjects);

    QGeoBoundingBox box(QGeoCoordinate(10, 10), QGeoCoordinate(0, 20));

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    map->setCenter(box.center());

    QPointF point = map->coordinateToScreenPosition(box.center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    foreach (QGeoMapObject* child, childObjects) {
        object->addChildObject(child);
    }

    QCOMPARE(object->childObjects().count(), childObjects.count());

    //check if object are there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),childObjects.count());

    QCOMPARE(spy0.count(), childObjects.count());
    QCOMPARE(spy1.count(), 0);

}

void tst_QGeoMapGroupObject::clearChildObjects_data()
{
    QTest::addColumn<int>("childObjectCount");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
}

// public void clearChildObjects()
void tst_QGeoMapGroupObject::clearChildObjects()
{

    QFETCH(int, childObjectCount);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->addChildObject(childObject);
    }

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    object->clearChildObjects();

    list = map->mapObjects();

    QCOMPARE(list.at(0), static_cast<QGeoMapObject*>(object));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), childObjectCount);

}


void tst_QGeoMapGroupObject::removeChildObject_data()
{
    QTest::addColumn<int>("childObjectCount");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
}

// public void removeChildObject(QGeoMapObject* childObject)
void tst_QGeoMapGroupObject::removeChildObject()
{

    QFETCH(int, childObjectCount);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->addChildObject(childObject);
    }

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = object->childObjects().first();
        object->removeChildObject(childObject);
        delete childObject;
    }

    QCOMPARE(object->childObjects().size(),0 );

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), childObjectCount);
}

void tst_QGeoMapGroupObject::setMapData_data()
{
    QTest::addColumn<int>("mapDataCount");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void setMapData(QGeoMapData* mapData)
void tst_QGeoMapGroupObject::setMapData()
{

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* circle1 = new QGeoMapCircleObject(center, 1000);
    QGeoMapCircleObject* circle2 = new QGeoMapCircleObject(center, 1000);
    QGeoMapCircleObject* circle3 = new QGeoMapCircleObject(center, 1000);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    object->addChildObject(circle1);
    object->addChildObject(circle2);
    object->addChildObject(circle3);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.count(),1);

    foreach (QGeoMapObject* o, object->childObjects()) {
        QVERIFY2(o->info()!=0,"missing info object");
    }

    object->setMapData(0);

    foreach (QGeoMapObject* o, object->childObjects()) {
        QVERIFY2(o->info()==0,"info object not deleted");
    }

}

void tst_QGeoMapGroupObject::zvalue_data()
{
    QTest::addColumn<int>("zValue1");
    QTest::addColumn<int>("zValue2");
    QTest::addColumn<int>("zValue3");
    QTest::newRow("1,2,3") << 1 << 2 << 3;
    QTest::newRow("3,2,1") << 3 << 2 << 1;
    QTest::newRow("2,1,3") << 2 << 1 << 3;
}

// public int zValue() const
void tst_QGeoMapGroupObject::zvalue()
{

    QFETCH(int, zValue1);
    QFETCH(int, zValue2);
    QFETCH(int, zValue3);

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* circle1 = new QGeoMapCircleObject(center, 1000);
    QGeoMapCircleObject* circle2 = new QGeoMapCircleObject(center, 1000);
    QGeoMapCircleObject* circle3 = new QGeoMapCircleObject(center, 1000);

    QGeoMapGroupObject* object1 = new QGeoMapGroupObject();
    object1->addChildObject(circle1);
    QGeoMapGroupObject* object2 = new QGeoMapGroupObject();
    object2->addChildObject(circle2);
    QGeoMapGroupObject* object3 = new QGeoMapGroupObject();
    object3->addChildObject(circle3);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object1);
    map->addMapObject(object2);
    map->addMapObject(object3);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.count(),3);

    QVERIFY(list.at(0)==object1);
    QVERIFY(list.at(1)==object2);
    QVERIFY(list.at(2)==object3);

    QSignalSpy spy0(object1, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object1, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object1, SIGNAL(zValueChanged(int)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(0)==circle1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(1)==circle2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(2)==circle3);

    circle1->setZValue(3);
    circle2->setZValue(1);
    circle3->setZValue(2);

    QCOMPARE(circle1->zValue(), 3);
    QCOMPARE(circle2->zValue(), 1);
    QCOMPARE(circle3->zValue(), 2);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(0)==circle1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(1)==circle2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(2)==circle3);

    object1->setZValue(zValue1);
    object2->setZValue(zValue2);
    object3->setZValue(zValue3);

    QCOMPARE(object1->zValue(), zValue1);
    QCOMPARE(object2->zValue(), zValue2);
    QCOMPARE(object3->zValue(), zValue3);
    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue1-1)==circle1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue2-1)==circle2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue3-1)==circle3);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

// public bool isVisible() const
void tst_QGeoMapGroupObject::isVisible()
{

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* circle = new QGeoMapCircleObject(center, 1000);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    object->addChildObject(circle);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    object->setVisible(false);

    QCOMPARE(object->isVisible(), false);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setVisible(true);

    QCOMPARE(object->isVisible(), true);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 2);
    QCOMPARE(spy2.count(), 0);

}

// public bool isSelected() const
void tst_QGeoMapGroupObject::isSelected()
{
#if 0

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    QCOMPARE(object->isSelected(), isSelected);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoMapGroupObject::contains_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("10,10") << QGeoCoordinate(10, 10, 0);
    QTest::newRow("20,20") << QGeoCoordinate(20, 20, 0);
    QTest::newRow("0,0") << QGeoCoordinate(0, 0, 0);

}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapGroupObject::contains()
{
    QFETCH(QGeoCoordinate, coordinate);

    QGeoCoordinate center(0, 0, 0);

    QGeoMapCircleObject* circle = new QGeoMapCircleObject(center, 1000);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();
    object->addChildObject(circle);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(coordinate);

    QPointF point = map->coordinateToScreenPosition(coordinate);

    bool contains = map->mapObjectsAtScreenPosition(point).size() == 1;

    QCOMPARE(object->contains(coordinate), contains);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapGroupObject::boundingBox_data()
{
#if 0
    QTest::addColumn<QGeoBoundingBox>("boundingBox");
    QTest::newRow("null") << QGeoBoundingBox();
#endif
}


// public QGeoBoundingBox boundingBox() const
void tst_QGeoMapGroupObject::boundingBox()
{

    QGeoCoordinate center(0, 0, 0);

    QGeoMapCircleObject* circle = new QGeoMapCircleObject(center, 1000);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();
    object->addChildObject(circle);
    QVERIFY(object->childObjects().at(0)==circle);

    QGraphicsGeoMap* map = m_helper->map();

    map->setCenter(center);
    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(circle->boundingBox().width()>0,"no bounding box");
    QVERIFY2(circle->boundingBox().height()>0,"no bounding box");
    QVERIFY2(object->boundingBox().width()>0,"no bounding box");
    QVERIFY2(object->boundingBox().height()>0,"no bounding box");

}

QTEST_MAIN(tst_QGeoMapGroupObject)
#include "tst_qgeomapgroupobject.moc"

